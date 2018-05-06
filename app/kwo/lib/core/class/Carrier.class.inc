<?php

class Carrier extends ActiveRecord {

  const FLAG_POSTAL = 2;
  const FLAG_FRANCE = 4;

//  public static $flags = array(self::FLAG_FRANCE => 'FR uniquement');
  
  const TABLE_RATES = 'core_carrier_rate';
  
  const STATUS_OFFLINE = 0;
  const STATUS_ONLINE = 1;

  public function commit($hash) {
    $rebuild = false;
    if (isset($hash['rates']) && md5(strval($this->rates)) != md5($hash['rates'])) {
      $rebuild = true;
    } 
    parent::commit($hash);
    if ($rebuild === true) {
      $this->rebuild();
    }
  }

  public function getShippingCost($order) {
    if ($order->isDigital()) return 0;
    $cost = 0;
    foreach ($order->getShipments() as $shipment) {
      $cost += $shipment->calculateCost($this->getId());
    }
    return $cost;
  }

  protected function onAfterDestroy() {
    $this->removeRates();
  }

  private function removeRates() {
    $this->dbo->exec('DELETE FROM '.self::TABLE_RATES
                     .    ' WHERE carrier_id='.$this->getId());
  }

  public static $countries = null;

  private function rebuild() { 
    $this->removeRates();
    if (empty($this->rates)) return ;
    $rates = str_replace('" ', '', $this->rates);
    if (self::$countries === null) {
      self::$countries = $this->dbo->asHash('SELECT UPPER(code), id'
                                            .' FROM '.T('country'));
    }
    $world = null;
    $europe = null;
    $dom = null;
    $tom = null;

    $lines = explode("\n", $rates); 
    $used = array(0 => 1);
    foreach ($lines as $i => $line) {
      $row = array('carrier_id' => $this->getId());
      list ($location, $row['amount'], 
            $row['volume_min'], $row['volume_max'], 
            $row['weight_min'], $row['weight_max'], 
            $row['amount_min'], $row['amount_max']) = explode(';', $line);
      list ($country, $row['admin2']) = explode('-', $location);
      foreach ($row as &$value) {
        $value = trim($value);
        $value = strtolower($value);
      }
      foreach (array('volume_max', 'weight_max', 'amount_max') as $key) {
        if ($row[$key] == 'x') $row[$key] = '1000000';
      }
      $country = strtoupper($country);
      if ($country === 'MONDE' || $country === 'WORLD' || $country === 'EUROPE' || 
          $country === 'DOM' || $country === 'TOM') { 
        if ($country === 'EUROPE') { 
          if ($europe === null) { 
            $europe = $this->dbo->asHash('SELECT UPPER(code), id'
                                         .' FROM '.T('country')
                                         .' WHERE (flags & '.Country::FLAG_EUROPEAN.') >= 1'
                                         .  ' AND id NOT IN ('.join(',', array_keys($used)).')'); 
            foreach ($europe as $code => $id) {
              $used[intval($id)] = 1; 
            }
          }
          $countries = $europe;
        }
        elseif ($country === 'DOM') { 
          if ($dom === null) { 
            $dom = $this->dbo->asHash('SELECT UPPER(code), id'
                                         .' FROM '.T('country')
                                         .' WHERE (flags & '.Country::FLAG_DOM.') >= 1'
                                         .  ' AND id NOT IN ('.join(',', array_keys($used)).')'); 
            foreach ($dom as $code => $id) {
              $used[intval($id)] = 1; 
            }
          }
          $countries = $dom;
        }
        elseif ($country === 'TOM') { 
          if ($tom === null) { 
            $tom = $this->dbo->asHash('SELECT UPPER(code), id'
                                         .' FROM '.T('country')
                                         .' WHERE (flags & '.Country::FLAG_TOM.') >= 1'
                                         .  ' AND id NOT IN ('.join(',', array_keys($used)).')'); 
            foreach ($tom as $code => $id) {
              $used[intval($id)] = 1; 
            }
          }
          $countries = $tom;
        }
        else {
          if ($world === null) {
            $world = $this->dbo->asHash('SELECT UPPER(code), id'
                                        .' FROM '.T('country')
                                        .' WHERE id NOT IN ('.join(',', array_keys($used)).')');
          }
          $countries = $world;
        }
        //        $countries = $country === 'europe' ? $europe : $world; 
      }
      else {
        $country_id = (int) self::$countries[$country];
        $countries = array($country => $country_id);
        $used[$country_id] = 1; 
      }
      /*      if (empty($row['admin2']) && $i > 0) { 
        $this->dbo->exec('DELETE FROM '.self::TABLE_RATES
                         .    ' WHERE carrier_id='.$this->getId()
                         .      ' AND country_id IN ([countries])', 
                         array('[countries]' => array_values($countries))); 
                         }*/
      
      foreach ($countries as $code => $id) {
        $row['country_id'] = $id;
        if ($id < 1) continue ;
        $this->dbo->insert(self::TABLE_RATES, $row);
      }
    }
  }

  public static function getDefault() {
    $id = S('dbo')->fetchValue('SELECT id'
                               .' FROM '.T('carrier')
                               .' WHERE (app="'.S('app')->getName().'" OR app="")'
                               .  ' AND status='.self::STATUS_ONLINE
                               .' ORDER BY position ASC'
                               .' LIMIT 1');
    if ($id < 1) return null;
    return Carrier::getInstance($id);
  }

}
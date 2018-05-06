<?php

class Search extends ActiveRecord {

  const TABLE_RESULTS = 'core_search_results';
  const TABLE_UPDATES = 'core_search_updates';

  const MAX_SIZE = 5000;

  private $limit = 0;
//  private $offset = 0;
  private $page = 0;

  public function refresh() {
    $dbo = S('dbo');
    $opts = unserialize($this->opts);
    $crawler = explode(':', $this->crawler);
    $h = call_user_func($crawler, $opts);
    $h['metas'] = (array) $h['metas'];
    $h['results'] = (array) $h['results'];
    $h['results'] = array_slice($h['results'], 0, self::MAX_SIZE);
    $results = array();
    foreach ($h['results'] as $result) {
      $results[] = is_array($result) ? $result : array($this->model_id, $result);
    }
    $rs = $dbo->query('SELECT model_id, record_id'
                      .' FROM '.self::TABLE_UPDATES
                      .' WHERE search_id='.$this->id);
    $updates = array();
    foreach ($rs as $row) {
      $model_id = $row[0];
      $record_id = $row[1];
      if (!$updates[$model_id]) $updates[$model_id] = array();
      $updates[$model_id][] = $record_id;
    }
    $opts = array('modifiers' => array('ignore'),
                  'cols' => array('model_id', 'record_id', 'search_id', 'added_on'));
    $diff = array();
    $i = 0;
    foreach ($results as $row) {
      $model_id = (int) $row[0];
      $record_id = (int) $row[1];
      $updates[$model_id] = (array) $updates[$model_id];
      if (in_array($record_id, $updates[$model_id])) continue ;
      $diff[] = array($model_id, $record_id, $this->id, DH::today());
      if (($i++ % 1000) === 0) {
        $dbo->massInsert(self::TABLE_UPDATES, $diff, $opts);
      }
    }
    if (count($diff) > 0) {
      $dbo->massInsert(self::TABLE_UPDATES, $diff, $opts);
    }
    $dbo->exec('DELETE FROM '.self::TABLE_RESULTS.' WHERE search_id='.$this->id);

    $rows = array();
    $row = array('search_id' => $this->id, 'model_id' => 0, 'record_id' => 0, 'position' => 0);
    $position = 0;
    foreach ($results as $result) {
      $row['model_id'] = (int) $result[0];
      $row['record_id'] = (int) $result[1];
      $row['position'] = $position++;
      $rows[] = $row;
      if (($position % 1000) === 0) {
        $dbo->massInsert(self::TABLE_RESULTS, $rows);
        $rows = array();
      }
    }
    if (count($rows) > 0) {
      $dbo->massInsert(self::TABLE_RESULTS, $rows);
    }
    $attrs = array('found_rows' => (int) $h['found_rows'],
                   'stored_rows' => $position,
                   'metas' => serialize($h['metas']),
                   'refreshed_at' => DH::now());
    $this->setAttributes($attrs);
  }

  public function getName($locale=null) {
    if ($this->name) return $this->name;
    if ($this->keyword) return $this->keyword;
    return 'search('.$this->id.')';
  }

  public function getPagination($opts=null) {
    $opts = (array) $opts;
    $opts += array('limit' => $this->limit,
                   'page' => $this->page,
                   'found_rows' => $this->getStoredRows());
    return new Pagination($opts);
  }

  protected function hasItems() {
    return $this->found_rows >= 1;
  }

  // TODO : passer en $opts pour pouvoir passer 'page' ou 'offset'
  public function getItems($page=1, $limit=10) {
    $this->page = $page >= 1 ? (int) $page : 1;
    $this->limit = $limit >= 1 ? (int) $limit : 10;
    $items = array();
    $rs = S('dbo')->query('SELECT model_id, record_id'
                          .' FROM '.self::TABLE_RESULTS
                          .' WHERE search_id='.$this->id
                          .' ORDER BY position'
                          .' LIMIT '.$this->limit
                          .' OFFSET '.(($this->page - 1) * $this->limit));
    foreach ($rs as $row) {
      try {
        $items[] = Item::getInstance((int) $row['model_id'], (int) $row['record_id']);
      }
      catch (Exception $e) {
        S('log')->error($e, __METHOD__);
      }
    }
    $req = S('req');
    if ($this->page < 1 && $req->isFront()) {
      $this->log();
    }
    return $items;
  }

  public function getFoundRows() {
    return (int) $this->found_rows;
  }

  public function getStoredRows() {
    return (int) $this->stored_rows;
  }

  public function getMetas() {
    if (empty($this->metas)) return array();
    return unserialize($this->metas);
  }

  //----------------------------------------

  public static function refreshKeywordSearch($search_opts) {
    $opts = array('query' => $search_opts['keyword'],
                  'limit' => self::MAX_SIZE);
    $searcher = Searcher::getInstance($opts);
    $h = array('results' => $searcher->asArray(),
               'found_rows' => $searcher->getFoundRows(),
               'metas' => array());
    return $h;
  }

  public static function factory($opts, $crawler=null) {
    if (is_numeric($opts)) {
      return Search::getInstance((int) $opts);
    }
    if (isset($opts['keyword']) && !isset($opts['crawler'])) {
      $opts['crawler'] = array('Search', 'refreshKeywordSearch');
    }

    foreach (array('limit', 'offset', 'page') as $key) {
      unset($opts[$key]);
    }
    $opts['crawler'] = join(':', $crawler);
    $opts['model_id'] = (int) $opts['model_id'];
    $opts['keyword'] = self::clean($opts['keyword']);

    $crc = AH::crc($opts);
    $id = (int) Search::id(array('crc' => $crc, 'status' => 1));
    if ($id >= 1) {
      return Search::getInstance($id);
    }

    $row = array('crawler' => $opts['crawler'],
                 'crc' => $crc,
                 'latitude' => $opts['latitude'],
                 'longitude' => $opts['longitude'],
                 'model_id' => $opts['model_id'],
                 'opts' => serialize($opts),
                 'radius' => $opts['radius'],
                 'keyword' => $opts['keyword']);
    $id = (int) S('dbo')->insert(T('search'), $row);
    $search = Search::getInstance($id);
    $search->refresh();
    return $search;
  }

  public static function invalidate($item) {
    $dbo = S('dbo');
    $model_id = $item->getModelId();
    $record_id = $item->getId();
    $searches = $dbo->asSet('SELECT search_id'
                            .' FROM '.self::TABLE_UPDATES
                            .' WHERE model_id='.$model_id
                            .  ' AND record_id='.$record_id);
    if (empty($searches)) return ;
    S('dbo')->query('UPDATE '.T('search')
                    . ' SET found_rows=found_rows-1,'
                    .     ' stored_rows=stored_rows-1,'
                    .     ' status=0'
                    .' WHERE id IN ('.join(',', $searches).')');
    $dbo->exec('DELETE FROM '.self::TABLE_UPDATES
               .    ' WHERE model_id='.$model_id
               .      ' AND record_id='.$record_id);
  }

  public static function clean($keyword) {
    $keyword = strtolower($keyword);
    $keyword = preg_replace('/[^\w\'-]/u', ' ', $keyword);
    $keyword = preg_replace('/(\s){2,}/u', ' ', $keyword);
    $keyword = trim($keyword, ' -_');
    return $keyword;
  }

}

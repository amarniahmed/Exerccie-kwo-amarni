<?php

//define('DB_SERVER', ':/var/lib/mysql/mysql.sock');

class DataBaseObject implements Iterator
{
  const ERROR_DUPLICATION = -1;

  const CLIENT_MULTI_STATEMENTS = 65536;

  private $bindings     = array();
  private $current      = false;
  private $host         = null;
  private $is_connected = false;
  private $link         = false;
  private $login        = null;
  private $name         = null;
  private $password     = null;
  private $sql          = null;
  private $tables       = null;
  private $trace_sql    = false;

  public function __construct($opts) {
    $opts = (array) $opts;
    $this->host     = (string) $opts['host'];
    $this->login    = (string) $opts['login'];
    $this->password = (string) $opts['password'];
    $this->name     = (string) $opts['name'];
  }

  /*** Iterator ***/

  public function rewind()  {
    if ($this->tables === null) {
      $this->tables = $this->getTables();
    }
    reset($this->tables);
  }

  public function current() { return current($this->tables); }
  public function key()     { return key($this->tables); }
  public function next()    { return next($this->tables); }
  public function valid()   { return $this->current() !== false; }

  /****************/

  public function getName() { return $this->name; }
  public function name() { return $this->name; }
  public function link() { return $this->link; }
  public function resetTables() { $this->tables = null; }

  public function version() {
    $this->connect();
    return mysql_get_server_info();
  }

  public function isConnected() {
    return $this->is_connected === true;
  }

  public function connect() {
    if ($this->is_connected === true) {
      if (!$this->isAvailable()) {
        $this->ping();
      }
      return ;
    }
    $this->link = @mysql_connect($this->host, $this->login, $this->password, true);
    if ($this->link === false) {
      return S('log')->fatal('server connection ['.mysql_error().']', __METHOD__);
    }
    if (@mysql_select_db($this->name, $this->link) === false) {
      return S('log')->fatal('db selection ['.$this->name.']', __METHOD__);
    }
    mysql_query('SET CHARACTER SET utf8', $this->link);
    mysql_query("SET NAMES 'utf8' COLLATE 'utf8_general_ci'", $this->link);
    $this->is_connected = true;
  }

  public function prepare($sql, $bindings) {
    $this->connect();
    if (!is_array($bindings) || count($bindings) < 1) {
      return $sql;
    }
    foreach ($bindings as $key => $value) {
      if ($value === false || $value === null) {
        $this->traceError('bad value (FALSE|NULL) for key '.$key.' in SQL ['.$sql.']', false);
      }
      if ($key[0] == '#') {
        $bindings[$key] = $this->protect($value);
      }
      elseif ($key[0] == '[') {
	if (is_array($value)) {
	  if (count($value) >= 1) {
	    foreach ($value as $k => $v) {
              $value[$k] = "'".$this->protect($v)."'";
            }
	    $bindings[$key] = join(',', $value);
	  }
	  else $bindings[$key] = "'kwo_empty_value'";
	}
	else $bindings[$key] =  "'".$this->protect($value)."'";
      }
      elseif ($key[0] == '%') {
        error_log('DEPRECATED PREPARE %');
	$bindings[$key] =  "'%,".$this->protect($value).",%'";
      }
      elseif ($key[0] == '/') {
        error_log('DEPRECATED PREPARE /');
	foreach ($value as $k => $v) $value[$k] = ",".$this->protect($v).",";
	$bindings[$key] =  "'".join('|', $value)."'";
      }
      elseif ($key[0] == '{') {
	if (is_array($value)) {
	  if (count($value) >= 1) {
	    foreach ($value as $k => $v) $value[$k] = $this->protect($v);
	    $bindings[$key] = join(',',$value);
	  }
	  else $bindings[$key] = "'kwo_empty_value'";
	}
	else $bindings[$key] =  "'".$this->protect($value)."'";
      }
      else {
        $bindings[$key] = "'".$this->protect($value)."'";
      }
    }
    $sql = str_replace(array_keys($bindings),
                       array_values($bindings), $sql);
    return $sql;
  }

  public function query($sql, $bindings=null, $modifiers=null) {
    /* [HIGH_PRIORITY] [STRAIGHT_JOIN]
       [SQL_SMALL_RESULT] [SQL_BIG_RESULT] [SQL_BUFFER_RESULT]
       [SQL_CACHE | SQL_NO_CACHE] [SQL_CALC_FOUND_ROWS]
    */
    $sql = ltrim($sql);
    if ($modifiers !== null) {
      if (!is_array($modifiers)) $modifiers = (array) $modifiers;
      if (($sql[0] === 's' || $sql[0] === 'S') && $sql[6] === ' ') {
        $sql = 'SELECT '.join(' ', $modifiers).substr($sql, 6);
      }
    }
    $this->sql = $sql = $this->prepare($sql, $bindings);
    $resource = mysql_query($sql, $this->link);
    $this->trace($sql);
    if ($resource === false) {
      $this->traceError($sql);
    }
    return new ResultSet($resource, $sql, $this->link);
  }

  private function uquery($sql, $bindings=null, $cache=true) {
    $modifiers = $cache ? ' SQL_CACHE' : '';
    $sql = ltrim($sql);
    if (($sql[0] === 's' || $sql[0] === 'S') && $sql[6] === ' ') {
      $sql = 'SELECT HIGH_PRIORITY SQL_SMALL_RESULT'.$modifiers.substr($sql, 6);
    }
    $sql = $this->prepare($sql, $bindings);
    $this->sql = $sql;
    $resource = mysql_unbuffered_query($sql, $this->link);
    $this->trace($sql);
    if ($resource === false) {
      $this->traceError($sql);
    }
    return $resource;
  }

  public function exec($sql, $bindings=null) {
    if (is_array($sql)) {
      $this->sql = $this->prepare(join("\n", $sql), $bindings);
      $resource = mysql_query($this->sql, $this->link);
      $this->trace($this->sql);
      return ;
    }
    $this->sql = $this->prepare($sql, $bindings);
    $ret = mysql_query($this->sql, $this->link);
    $this->trace($this->sql);
    if ($ret === false) {
      $this->traceError($this->sql);
    }
    return $this->affectedRows();
  }

  public function update($table, $record, $opts=null, $bindings=null, $modifiers=null) {
    // LOW_PRIORITY | IGNORE

    $opts = (array) $opts;
    $this->connect();
    if (!is_null($modifiers)) {
      S('log')->trace('DEPERECATED modifiers param in update ['.$table.'], use opts[modifiers]', __METHOD__);
    }
    if (!is_null($bindings)) {
      S('log')->trace('DEPERECATED bindings param, user $dbo->exec with bindings', __METHOD__);
    }
    $table = $table instanceof Table ? $table->name() : $table;
    $record = (array) $record;
    if (count($record) === 0) return false;
    $set = array();
    foreach ($record as $key => $value) {
      $set[] = is_int($key) ? $value : '`'.$key.'`'."='".$this->protect($value)."'";
    }
    $set = join(',', $set);
    $limit = $opts['limit'];
    $order = $opts['order'];
    $modifiers = $opts['modifiers'];
    $where = '';



    //    if (is_array($opts)) {
    //    $this->bindings = array();
    if (is_array($opts['filters'])) {
      $where = ' WHERE '.$this->hash2sql($opts['filters']);
    }
    //    $bindings = $this->bindings;

    if (is_array($opts['modifiers'])) {
      $modifiers = join(' ', $opts['modifiers']);
    }
    $modifiers = $modifiers ? $modifiers.' ' : $modifiers;

    $limit = strlen($opts['limit']) >= 1 ? ' LIMIT '.$opts['limit'] : '';


    if (is_array($order)) {
      $orders = array();
      foreach ($order as $key => $value) {
        if (!preg_match('/^[\w\.]+$/', $key.$value)) {
          S('log')->error('invalid order value ['.$key.$value.']', __METHOD__);
          continue ;
        }
        if (is_numeric($key)) {
          $orders[] = $value;
        }
        else {
          $orders[] = $key.(strtoupper($value) ===  'DESC' ? ' DESC' : '');
        }
      }
      $order = ' ORDER BY '.join(', ', $orders);
    }
    /*    elseif (strlen($opts['order']) >= 1) {
      S('log')->warn('order must be a hash', __METHOD__);
      }*/
    //    }
    /*    else {
     $where = ' WHERE '.(empty($opts) ? 1 : $opts);
     if ($modifiers !== null) {
        if ($modifiers === true) {
          $modifiers = 'IGNORE';
        }
        elseif (is_array($modifiers)) {
          $modifiers = join(' ', $modifiers);
        }
        $modifiers .= ' ';
      }
      else {
        $modifiers = '';
      }
      }*/
    $sql = 'UPDATE ' . $modifiers . $table . ' SET ' . $set . $where . $order . $limit;
    if (is_array($opts['bindings'])) {
      $sql = $this->prepare($sql, $opts['bindings']);
    }
    $this->sql = $sql;
    $ret = mysql_query($sql, $this->link);
    $this->trace($sql);
    if ($ret === false) {
      $this->traceError($sql);
    }
    return $this->affectedRows();
  }

  public function set($table, $row, $filters=null) {
    if (empty($filters) || (is_numeric($filters) && $filters < 1)) {
      return $this->insert($table, $row);
    }
    $filters = is_array($filters) ? $filters : array('id' => $filters);
    return $this->update($table, $row,
                         array('filters' => $filters,
                               'limit' => 1));
  }

  public function replace($table, $record, $modifiers=null) {
    // LOW_PRIORITY | DELAYED
    $this->connect();
    $record = (array) $record;
    if (count($record) == 0) return true;
    if ($modifiers !== null) {
      if (!is_array($modifiers)) {
        $modifiers = (array) $modifiers;
      }
      $modifiers = join(' ', $modifiers).' ';
    }
    else {
      $modifiers = '';
    }
    $set = array();
    foreach ($record as $key => $value) {
      if (strlen($key) === 0) {
        if (strlen($value) === 0) continue;
        $set[] = $value;
      }
      else {
        $set[] = '`'.$key.'`=\''.$this->protect($value).'\'';
      }
    }
    $this->sql = $sql = 'REPLACE '.$modifiers.'INTO '.$table.' SET '.join(', ', $set);
    $ret = mysql_query($sql, $this->link);
    $this->trace($sql);
    if ($ret === false) {
      $this->traceError($sql);
    }
    return $this->affectedRows();
  }

  public function insert($table, $record, $opts=null) {
    // [LOW_PRIORITY | DELAYED | HIGH_PRIORITY] [IGNORE]
    if (is_bool($opts)) {
      S('log')->warn('DEPRECATED : third param can not be a boolean',
                     __METHOD__);
      $opts = null;
    }
    $opts = (array) $opts;
    $this->connect();
    $table = $table instanceof Table ? $table->name() : $table;
    if (is_array(current($record)) || $record instanceof Iterator) {
      return $this->massInsert($table, $record, $opts);
    }
    $columns = $values = array();
    foreach ($record as $k => $v) {
      if (!is_numeric($k)) {
        $columns[] = '`'.$k.'`';
      }
      $values[] = "'".$this->protect($v)."'";
    }
    $modifiers = empty($opts) ? '' : ' '.join(' ', $opts);
    $sql = 'INSERT'.$modifiers.' INTO '.$table.' ';
    if (count($columns) >= 1) {
      $sql .= '('.join(',', $columns).') VALUES ('.join(',', $values).')';
    }
    else {
      $sql .= 'VALUES ('.join(',', $values).')';
    }
    $this->sql = $sql;
    $ret = mysql_query($sql, $this->link);
    $this->trace($sql);
    if ($ret === false) {
      if (mysql_errno($this->link) === 1062) {
        return -1;
      }
      $this->traceError($sql);
    }
    return $this->affectedRows() == 1 ? mysql_insert_id($this->link) : 0;
  }

  private function massInsert($table, $records, $opts=null) {
    if (is_bool($opts)) {
      S('log')->warn('DEPRECATED : third param can not be a boolean',
                     __METHOD__);
      $opts = null;
    }
    $opts = (array) $opts;
    $as_hash = false;
    if ($records instanceof Iterator) {
      $cols = $records->getFields();
      $as_hash = true;
    }
    else {
      $cols = array_keys(current($records));
    }
    foreach ($cols as &$col) {
      $col = '`'.$col.'`';
    }
    $modifiers = empty($opts) ? '' : ' '.join(' ', $opts);
    $values = array();
    foreach ($records as $record) {
      $record = $as_hash === true ? $record->asHash() : $record;
      $tmp = array();
      foreach ($record as $value) {
        $tmp[] = "'".$this->protect($value)."'";
      }
      array_push($values, '('.join(',', $tmp).')');
    }
    $this->sql = 'INSERT'.$modifiers
                 .' INTO '.$table.' ('.join(',', $cols).')'
                 .' VALUES '.join(', ', $values);
    $ret = mysql_query($this->sql, $this->link);
    $this->trace($this->sql);
    if ($ret === false) {
      $this->traceError($this->sql);
    }
    return $this->affectedRows();
  }

  public function affectedRows() {
    $ret = mysql_affected_rows($this->link);
    if ($ret === -1) {
      if (mysql_errno($this->link) === 1062) {
        return 0;
      }
      $this->traceError($this->sql);
    }
    return $ret;
  }

  public function matchedRows() {
    $str = mysql_info($this->link);
    if ($str === false) {
      S('log')->trace('error', __METHOD__);
      return 0;
    }
    if (strpos($str, 'Rows matched') === false) return 0;
    $matches = array();
    $ret = preg_match('/Rows matched: (?P<matched>[0-9]*)/', $str, $matches);
    if ($ret === false) return 0;
    // S('log')->debug($matches, $str);
    return intval($matches['matched']);
  }

  public function asArray($sql, $bindings=null, $key=null) {
    $rs = $this->query($sql, $bindings);
    if ($rs === false || $rs->numRows() < 1) return array();
    $ret = $rs->asArray($key);
    $rs->free();
    return $ret;
  }

  public function asSet($sql, $bindings=null, $key_field=null) {
    /* TODO : passer en uquery et éviter ResultSet */
    $rs = $this->query($sql, $bindings);
    if ($rs === false || $rs->numRows() < 1) return array();
    $ret = $rs->asSet($key_field);
    $rs->free();
    return $ret;
  }

  public function asHash($sql, $bindings=null, $key_field=null, $value_field=null) {
    /* TODO : passer en uquery et éviter ResultSet */
    $this->connect();
    $rs = $this->query($sql, $bindings, array('SQL_CACHE'));
    if ($rs === false || $rs->numRows() < 1) return array();
    if ($key_field === null) {
      $fields = $rs->getFields();
      $key_field = $fields[0];
      $value_field = $fields[1];
    }
    $ret = $rs->asHash($key_field, $value_field);
    $rs->free();
    return $ret;
  }

  public function fetchObject($sql, $bindings=null) {
    $rs = $this->uquery($sql, $bindings, false);
    $this->trace($sql);
    return $rs ? mysql_fetch_object($rs) : false;
  }

  public function fetchArray($sql, $bindings=null) {
    $rs = $this->uquery($sql, $bindings, false);
    $this->trace($sql);
    return $rs ? mysql_fetch_array($rs, MYSQL_NUM) : false;
  }

  public function fetchAttributes($sql, $bindings=null) {
    return $this->fetchHash($sql, $bindings);
  }

  public function fetchHash($sql, $bindings=null) {
    $rs = $this->uquery($sql, $bindings, false);
    $this->trace($sql);
    if ($rs === false) return false;
    $ret = mysql_fetch_array($rs, MYSQL_ASSOC);
    mysql_free_result($rs);
    return $ret;
  }

  public function fetchValue($sql, $bindings=null) {
    $rs = $this->uquery($sql, $bindings, false);
    $this->trace($sql);
    if ($rs === false) return false;
    $row = mysql_fetch_array($rs, MYSQL_NUM);
    return $row === false ? false : $row[0];
  }

  public function resetAutoIncrement($table) {
    $this->exec('ALTER TABLE `'.$table.'` AUTO_INCREMENT =1');
  }

  public function monitor() {
    $this->trace_sql = ! $this->trace_sql;
  }

  private function trace($sql) {
    if ($this->trace_sql === true) {
      S('log')->trace($sql);
    }
  }

  public function getVariable($var) {
    $h = $this->fetchHash('SHOW VARIABLES LIKE "'.$var.'"');
    return $h['Value'];
  }

  private function traceError($msg, $trace_error=true) {
    S('log')->error($msg);
    if ($trace_error === true) {
      S('log')->error('['.mysql_errno($this->link).'] '.mysql_error($this->link), __METHOD__);
    }
    throw new DataBaseException('dbo error', mysql_errno($this->link));
  }

  public function close() {
    $this->is_connected = false;
    mysql_close($this->link);
  }

  public function isAvailable() {
    return mysql_ping($this->link);
  }

  public function ping() {
    if (is_bool($this->link) || is_null($this->link)) return ;
    if (!mysql_ping($this->link)) {
      $this->close();
      $this->connect();
    }
    //    $this->close();
    //    $this->connect();
  }

  public function free() {
    $this->connect();
    //    mysql_free_result($this->result);
  }

  public function protect($str) {
    if (is_numeric($str)) return $str;
    $this->connect();
    $str = (string) $str;
    return mysql_real_escape_string($str);
  }

  public function debug() {
    S('log')->debug($this->sql);
  }

  public function hasTable($name, $use_cache=true) {
    if (strlen($name) <= 1) return false;
    if ($use_cache === false) {
      $this->tables = null;
    }
    return in_array($name, $this->getTables());
  }

  public function getTable($name) {
    $this->connect();
    if (!$this->hasTable($name)) {
      S('log')->backtrace();
      S('log')->error('nonexistent table ['.$name.']', __METHOD__);
      throw new DataBaseException();
    }
    return $this->tables[$name];
  }

  public function getTables($prefix=false) {
    if (!is_array($this->tables)) {
      $this->connect();
      $rs = @mysql_query('SHOW TABLES FROM `'.$this->name.'`');
      if ($rs === false) {
        S('log')->error(mysql_error());
        //        throw new DataBaseException();
        return array();
      }
      $this->tables = array();
      while ($row = mysql_fetch_row($rs)) {
        $this->tables[$row[0]] = new Table($row[0], $this);
      }
    }
    if ($prefix === false) return $this->tables;
    if (empty($prefix)) return array();
    $tables = array();
    $prefix = str_replace(array('*'), array(''), $prefix);
    foreach ($this->tables as $name => $table) {
      if (preg_match('/^'.$prefix.'/', $name)) $tables[$name] = $table;
    }
    return $tables;
  }

  public function hash2sql($hash) {
    $sql = '';
    $i = 0;
    foreach ($hash as $key => $value) {
      if ($i === 0) { $ope = ''; }
      elseif (!is_array($value)) $ope = ' AND ';
      elseif (count($value) === 1 && key($value) === 'OR') $ope = ' OR ';
      else $ope = ' AND ';
      $sql .= $ope;
      if (is_array($value) && (is_numeric($key) || $key === 'OR')) {
        $sql .= count($value) > 1 ? '(' : '';
        $sql .= $this->hash2sql($value);
        $sql .= count($value) > 1 ? ')' : '';
      }
      else {
        $sql .= $this->pair2sql($key, $value);
      }
      $i++;
    }
    return empty($sql) ? '1' : $sql;
  }

  public function pair2sql($key, $value) {
    $sep = '-'.count($this->bindings);
    if ($key[0] === '|') {
      list(, $ope, $key) = explode('|', $key);
      $ope = strtolower($ope);
      if ($ope[0] === '!') {
        $not = true;
        $ope = substr($ope, 1);
      }
    }
    $col = '`'.$key.'`';
    $ope = $ope === null ? '=' : $ope;
    if ($ope === '=' && is_array($value)) {
      $ope = 'in';
    }
    elseif ($ope === '%%' && is_array($value)) {
      $ope = 'regexp';
    }
    if (in_array($ope, array('=', '<', '>', '<=', '>=', '<>', 'LIKE'))) {
      $ope = $ope === 'LIKE' ? ' '.$ope.' ' : $ope;
      $value = is_numeric($value) ? $value : $this->protect($value);
      $sql = $col.$ope."'".$value."'";
    }
    elseif ($ope === '%') {
      $sql = $col.' LIKE "#'.$key.$sep.'#%"';
      $this->bindings['#'.$key.$sep.'#'] = $value;
    }
    elseif ($ope === 'y') {
      $sql = 'YEAR('.$col.')='.intval($value);
    }
    elseif ($ope === 'm') {
      $sql = 'MONTH('.$col.')='.intval($value);
    }
    elseif ($ope === '%%') {
      $sql = $col.' LIKE '.'"%#'.$key.$sep.'#%"';
      $this->bindings['#'.$key.$sep.'#'] = $value;
    }
    elseif ($ope === 'in') {
      if (AH::isNumeric($value)) {
        $sql = $col.' IN '.'('.join(',', $value).')';
      }
      else {
        $sql = $col.' IN '.'(['.$key.$sep.'])';
        $this->bindings['['.$key.$sep.']'] = $value;
      }
    }
    elseif ($ope === '#') {
      $sql = $value;
    }
    elseif ($ope === '><') {
      $sql = '('.$col.' BETWEEN '.':'.$key.$sep.$i.'0: AND :'.$key.$sep.$i.'1:)';
      $this->bindings[':'.$key.$sep.$i.'0:'] = $value[0];
      $this->bindings[':'.$key.$sep.$i.'1:'] = $value[1];
    }
    elseif ($ope === 'regexp') {
      if (is_array($value)) {
        $sql = $col.' REGEXP '."'(#".$key.$sep."#)'";
        $this->bindings['#'.$key.$sep.'#'] = join('|', $value);
      }
      else {
        $sql = $col.' REGEXP '."'#".$key.$sep."#'";
        $this->bindings['#'.$key.$sep.'#'] = $value;
      }
    }
    elseif ($ope === 'empty') {
      $sql = $col.' = ""';
    }

    elseif ($ope === 'have') {
      $sql = ' FIND_IN_SET(:'.$key.$sep.':, '.$col.')';
      $this->bindings[':'.$key.$sep.':'] = $value;
    }
    return $not === true ? 'NOT ('.$sql.')' : $sql;
  }

  public function flush() {
    if ($this->login != 'root') {
      S('log')->trace('must be root', __METHOD__);
      return ;
    }
    $this->exec('FLUSH TABLES');
  }

  public function optimize() {
    $rs = $this->query('SHOW TABLE STATUS');
    foreach ($rs as $row) {
      if ($row['Data_free'] <= 0) continue ;
      $this->exec('OPTIMIZE TABLE `'.$row['Name'].'`');
//      S('log')->trace($row['Name'].', free space cleaning ('.$row['Data_free'].')', __METHOD__);
    }
  }

  public function disconnect() {
    if (is_resource($this->link)) {
      mysql_close($this->link);
      $this->is_connected = false;
      $this->link = null;
      $this->sql = '';
    }
  }

  public function __clone() {
    $this->is_connected = false;
    $this->link = null;
    $this->sql = '';
  }

  public function __destruct() {
    $this->disconnect();
  }

  public function __toString() {
    $this->connect();
    $str  = '';
    $str .= '- server version : '.mysql_get_server_info($this->link)."\n";
    $str .= '- base : '.$this->name()."\n";
    $str .= '- table count : '.count($this->getTables())."\n";
    return $str;
  }

  public static function getInstance($opts=null) {
    if ($opts === null) {
      if (Platform::hasSingleton('dbo')) return ;
      $opts = array();
      $opts['host']     = P('app.dsn.host');
      $opts['name']     = P('app.dsn.name');
      $opts['login']    = P('app.dsn.login');
      $opts['password'] = P('app.dsn.password');
      Platform::addSingleton('dbo', new DataBaseObject($opts));
      return ;
    }
    //$opts['host'] = $opts['host'] ?: DB_HOST;
    $opts['host'] = $opts['host'] ?: P('app.dsn.host'); // mluberry 08/03/13
    return new DataBaseObject($opts);
  }

}



/***

final class DataBaseException extends Exception {
  public function __construct($msg='', $code = 0) { parent::__construct($msg, $code); }
}

function S($key) {
  return $GLOBALS[$key];
}

class Logger
{
  public function __construct() {
    if (php_sapi_name() === 'cli') {
      ini_set('error_log', '.logs');
    }
  }

  public function __call($method, $args) {
    error_log('['.strtoupper($method).'] '.join(',', $args));
  }
}

$log = new Logger();
$dbo = new DataBaseObject('server', 'login', 'password', 'dbname');

***/

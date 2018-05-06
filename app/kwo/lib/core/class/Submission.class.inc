<?php

class Submission extends ActiveRecord
{
  const FLAG_PREMIUM = 1;
  const FLAG_FORWARDED = 2;
  const FLAG_REPLIED = 4;
  const FLAG_SPAM = 8;
  const FLAG_ERROR = 16;

  public static $flags = array(self::FLAG_PREMIUM => 'premium',
                               self::FLAG_FORWARDED => 'transmis',
                               self::FLAG_REPLIED => 'répondu',
                               self::FLAG_SPAM => 'spam',
                               self::FLAG_ERROR => 'erreur');

  const TABLE_FIELDS = 'core_field_submission';

  protected function onBeforeCommit(&$attrs) {
    if (isset($attrs['fields'])) {
      $fields = $attrs['fields'];
      $errors = array();
      if ($this->isItem()) {
        $form = $this->getForm();
      }
      else {
        $form = Form::getInstance(intval($attrs['form_id']));
      }
      foreach ($fields as $key => $value) {
        if (!array_key_exists($key, $form->fields)) {
          $errors[] = '-';
          break;
        }
        if ($form->fields[$key]['required'] === true) {
          if (is_array($value) && count($value) < 1) {
            $errors[] = $key;
          }
          elseif ($form->fields[$key]['type'] === 'email' &&
                  !Email::isValid($value)) {
            $errors[] = $key;
          }
          elseif (empty($value)) {
            $errors[] = $key;
          }
        }
      }
      if (count($errors) >= 1) {
        $e = new Exception();
        $e->fields = $errors;
        throw $e;
      }
    }
  }

  public function getFields() {
    $form = $this->getForm();
    $fields = array();
    foreach ($form->getFields() as $field) {
      $fields[$field->getId()] = $field->code;
    }
    $h = S('dbo')->asHash('SELECT field_id, value'
                             .' FROM '.Submission::TABLE_FIELDS
                             .' WHERE submission_id='.$this->getId());
    $out = array();
    foreach ($h as $field_id => $value) {
      $out[$fields[$field_id]] = $value;
    }
    return $out;
  }

  public function asEditHash() {
    $h = array();
    $fields = $this->getFields();
    foreach ($fields as $key => &$value) {
      if (strpos($value, ' ') === false && Email::isValid($value)) {
        $value = '<a href="mailto:'.$value.'">'.$value.'</a>';
      }
      elseif (strpos($value, UploadManager::path()) === 0 && file_exists($value)) {
        $tmp = $value;
        $value = '<a href="/'.$value.'" target="_blank">'.$value.'</a>';
        if (File::isImage($tmp)) {
          $value .= "\n".'<img src="/'.$tmp.'" style="max-width:300px; max-height:150px; margin-top:6px;" />';
        }
      }
      $value = nl2br($value);
    }
    $h['fields'] = $fields;
    return $h;
  }

  protected function onAfterDestroy() {
    S('dbo')->exec('DELETE FROM '.self::TABLE_FIELDS
                   .    ' WHERE submission_id='.$this->getId());
  }

}
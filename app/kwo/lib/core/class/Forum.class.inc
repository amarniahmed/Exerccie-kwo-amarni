<?php

class Forum extends ActiveRecord 
{
  public static $topic_counts = null;

  public function getDetails() {
    return array('nbre de discussions' => $this->getTopicCount());
  }

  public function getTopicCount() {
    if (self::$topic_counts === null) {
      self::$topic_counts = $this->dbo->asHash('SELECT T.forum_id, count(*)'
                                               .' FROM '.T('topic').' as T'
                                               .' GROUP BY T.forum_id');
    }
    return (int) self::$topic_counts[$this->getId()];
  }

  public function truncate() {
    $i = 0;
    foreach ($this->getTopics() as $topic) {
      try {
        $topic->destroy();
        $i++;
      }
      catch (Exception $e) {  }
    }
    return $i;
  }

}
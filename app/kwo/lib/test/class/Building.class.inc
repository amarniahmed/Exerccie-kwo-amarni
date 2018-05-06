<?php

class Building extends ActiveRecord {
  public static function list_superficie_building(&$req)
  {
      $req = S('dbo')->fetchValue('SELECT  P.type,B.name;sum(P.superficie)'
                                  .'FROM piece P, settlem S, building B'
                                  .'INNER join S ON S.id=P.settlem_id'
                                  .'INNER join B ON B.id=S.building_id'
                                  .'GROUPBY P.type');
  retrurn($req);
}
}

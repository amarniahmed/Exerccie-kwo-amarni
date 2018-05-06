<?php

try {

  if (!is_array($req->criterions) || count($req->criterions) < 1) {
    throw new Exception(l('error'));
  }

  foreach ($req->criterions as $criterion_id => $grade) {
    if ($grade < 1) throw new Exception('tous les critères doivent être notés');
  }

  $req->cleanAttributes();

  $user = $req->getUser();

  $item = Item::getInstanceByKey($req->item);

  try {
    $args = array('model_id' => $item->getModelId(),
                  'record_id' => $item->getId(),
                  'user_id' => $user->getId());
    $valuation = Valuation::getInstance($args,
                                        array('ignore' => true));
    if ($valuation instanceof Valuation) {
      $valuation->destroy();
    }
  }
  catch (Exception $e) {}

  $grade = ceil(array_sum($req->criterions) / count($req->criterions));

  $valuation = Valuation::getInstance();
  $attrs = array('model_id' => $item->getModelId(),
                 'record_id' => $item->getId(),
                 'comment' => $req->comment,
                 'opinion' => (int) $req->opinion,
                 'grade' => $grade,
                 'user_id' => $user->getId());
  $valuation->commit($attrs);
  $valuation->setUser($user);
  $valuation->setVisit($req);

  foreach ($req->criterions as $criterion_id => $grade) {
    $row = array('criterion_id' => $criterion_id,
                 'valuation_id' => $valuation->getId(),
                 'grade' => $grade);
    $dbo->insert(Valuation::TABLE_LOG, $row);
  }

}
catch (Exception $e) { $err->add($e); }

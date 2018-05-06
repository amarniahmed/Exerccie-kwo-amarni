<?php

try {

  $opts = array('code' => $req->code);
  $faq = Faq::getInstance($opts, array('properties' => true));

  if (!$faq->isOnline()) {
    throw new MaliciousException();
  }

  $sections = $section_components = $components = array();

  $opts = array('filters' => array('status' => 1), 'properties' => true);
  foreach ($faq->getComponents($opts) as $component) {
    if ($component->section_id && Section::exists(array('id' => $component->section_id, 'status' => 1))) {
      $section_components[$component->section_id][] = $component;
    } else {
      $components[] = $component;
    }
  }
	if (count($section_components)) {
		$opts = array('filters' => array('faq_id' => $faq->id,
																		 'id' => array_keys($section_components),
																		 '|>=|status' => 1),
									'order' => array('position'=>'asc'),
									'properties' => true
								 );
		$sections = Section::collect($opts);
	}
  $faq->track();
  $ctx->faq = $faq;
  $ctx->sections = $sections;
  $ctx->section_components = $section_components;
  $ctx->components = $components;

  $req->track($faq);
  $res->setHead($faq);
  $res->useTemplate();

}
catch (Exception $e) {
  $log->error($e);
  $err->add($e);
}
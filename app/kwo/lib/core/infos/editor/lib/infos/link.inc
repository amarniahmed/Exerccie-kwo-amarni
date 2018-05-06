<?php 

parse_str($req->params,$params_arr);
$style_arr['STYLELINK'] = array('rouge');

$res->setSeo('Lien');
$res->useTemplates('//layer');

?>

<script>
var curLink = 0;
var editor = parent.Kwo.registry["<?=$params_arr['name']?>"];

function setSelect(_selectId, _value) {
  var thisSelect = $(_selectId);
  for (var i = 0; i < thisSelect.options.length; i++) {
    if (thisSelect.options[i].value == _value) {
      thisSelect.options[i].selected = true;
    }
  }
}

function initLinkForm() {
  var sel = editor.selection;
  curLink = sel.moveToAncestorNode('A');
  if (curLink == undefined) { 
    curLink = 0; 
    return false; 
  }

  var href = curLink.getAttribute("href");
  var target = curLink.getAttribute("target");
  var className = curLink.className;

  if (href.indexOf("page_id=")!=-1) {
    tmp_arr = href.split("page_id=");
    href = tmp_arr[1];
    setSelect("link_type", "intern://");
  }
  else if (href.indexOf("/file/download?file_path=") != -1) {
    tmp_arr = href.split("file_path=");
    href = tmp_arr[1];
    setSelect("link_type", "download://");
  }
  
  else if (href.indexOf("http://")!=-1) {
    tmp_arr = href.split("http://");
    href = tmp_arr[1];
    setSelect("link_type", "http://");
    $("search_link").style.display = "none";   
  }
  else if (href.indexOf("mailto:")!=-1) {
    tmp_arr = href.split("mailto:");
    href = tmp_arr[1];
    setSelect("link_type", "mailto:");
    $("search_link").style.display = "none";   
  }
  
  $('link_url').value = href;
  if ($('target')) setSelect('target', target);
  if ($('linkClass')) setSelect('linkClass', className);
  
  $("submitLink").innerHTML = "modifier le lien";
  $("removeLink").style.display = "inline";
}

function putLink() {
  var thisLink = "";
  var className = "";
  var target = "";
  var type = "";
  var href = "";

  if ($("link_url").value.blank()) { 
    parent.hideAllKwoLayer();
    return false;
  }
  
  thisLink = (curLink == 0) ? editor.createLink($("link_url").value) : curLink;
  type = $("link_type").options[$("link_type").selectedIndex].value;
  href = $F("link_url");
  //  if (editor.editor_mode=="push" && !href.startsWith('http://')) return alert('le lien n\'est pas valide');
  
  if (editor.editor_mode!="push") target = $("target").options[$("target").selectedIndex].value;
  
  if (type == "intern://") href = "/?page_id="+href;
  else if (type == "download://") href = "/file/download?file_path="+escape(href);
  else if (type == "http://") href = "http://"+href; 
  else if (type == "mailto:") href = "mailto:"+href; 
  
  if (thisLink != undefined) {
    thisLink.setAttribute("href", href);
    if (editor.editor_mode != "push") {
      if (target != undefined && target != 0) thisLink.setAttribute("target", target);
      else thisLink.removeAttribute("target");
    }
    if ($("linkClass")) {
      if ($F("linkClass")!=0) thisLink.className = $F("linkClass");
      else { thisLink.removeAttribute("class"); }
    }
  }
  parent.hideAllKwoLayer();
}

function removeLink() {
  var sel = editor.selection;
  curLink = sel.moveToAncestorNode('A');
  if (curLink != undefined) {
    html = curLink.innerHTML;
    curLink.parentNode.removeChild(curLink);
    editor.insertHtml(html);
  }
  parent.hideAllKwoLayer();
}

// fonctions de gestion des pages internes.

function checkInternDialog(_select) {
  var thisOption = _select.options[_select.selectedIndex].value;
  if (thisOption == "intern://" || thisOption == "download://") $("search_link").style.display = "inline";   
  else $("search_link").style.display = "none";
}

function toggleInternDialog() {
  var type = $F("link_type");
  if (type == "intern://") dlg = "searchField";
  else if (type == "download://") dlg = "fileField";

  if ($(dlg).style.display == "block") {
    $(dlg).style.display = "none";
    $("mainField").style.display="block";
  }
  else {
    $("mainField").style.display="none";
    $(dlg).style.display = "block";
  }
  return false;
}

function selectInternLink(idlink, sname) {
  $("link_url").value = idlink;
  if (editor.editor_mode=="push") $("pageName").value = sname;
  toggleInternDialog();
}

function selectDownloadLink(url) {
  $("link_url").value = url;
  toggleInternDialog();
}

Event.observe(window, "load", initLinkForm, false);
</script>

<div id="mainField" style="display:block">

<table class="layout">
<tr>
 <td style="width:13%;">type</td>
 <td>
  <select name="link_type" id="link_type" onChange="checkInternDialog(this);">
   <option value="http://">site externe</option>
<?php if($params_arr['mode']!="push"): ?>   
   <option value="intern://">site interne</option>
   <option value="download://">téléchargement</option>
   <option value="mailto:">e-Mail</option>
<?php endif; ?>
 </select>
 </td>
</tr>
<tr>
 <td>adr</td>
 <td>
  <input type="text" name="link_url" id="link_url" style='width:170px' /> 
   <a id="search_link" href="#" onClick="toggleInternDialog();" style="display:none;">
  <img src="/<?=PIX_PATH?>/action/search.png" title="chercher une page" style="width:16px; border:0; vertical-align:bottom"></a>
 </td>
</tr>
<?php if($params_arr['mode']!="push"): ?>
<tr>
 <td>cible</td>
 <td>
  <select name="target" id="target">
   <option value="0">aucune</option>
   <option value="_blank">nouvelle fenêtre</option>
   <option value="_parent">page parente</option>
  </select>
 </td>
</tr>
<tr>
 <td>style</td>
 <td><select name="linkClass" id="linkClass">
     <option value="0">defaut</option>
<?php foreach ($style_arr['STYLELINK'] as $style):?>
<option value="<?=$style?>"><?=$style?></option>
<?php endforeach;?>
   </select>
 </td>
</tr>
<?php endif; ?>
</table>

<div class="actions">
 <a href="javascript:putLink();" id="submitLink">insérer le lien</a><br/>
 <a href="javascript:removeLink()" id="removeLink" style="display:none;">supprimer le lien</a>
</div>

<input type="hidden" id="pageName" name="pageName" value="" />

</div>

<div style="display:none;" id="searchField">
 <iframe id="searchFrame" name="searchFrame" style="width:100%; height:210px; border:0;" 
         src="/back/editor/searchpage"></iframe>
</div>

<div style="display:none;" id="fileField">
 <iframe id="fileFrame" name="fileFrame" style="width:100%; height:210px; border:0;" 
         src="/back/editor/download?path=doc&p_filter=."></iframe>
</div>


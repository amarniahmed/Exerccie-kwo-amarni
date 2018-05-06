
//http://webkit.org/demos/editingToolbar/FancyToolbar.js


function Editor(name) {

  this.current_range = null;
  this.editor_doc    = null;
  this.editor_field  = $(name);
  this.editor_frame  = name+"_frame";
  this.editor_name   = name;
  this.editor_win    = null;
  this.source_mode   = false;
  this.link;

  if(Prototype.Browser.IE){
    this.editor_win = window.frames[this.editor_frame];
    this.editor_doc = this.editor_win.document;
  }
  else {
    this.editor_win = $(this.editor_frame).contentWindow;
    this.editor_doc = $(this.editor_frame).contentDocument;
  }

  this.content = $F(this.editor_field);
  this.content = this.content.blank() ? "votre contenu" : this.content;

  this.editor_doc.open("text/html");
  this.editor_doc.write("<html><head><style>"+
                        "BODY   { height:20px; margin:0; color:#777; padding:3px; background:#F5F2E9; font-family:Tahoma; font-size:12px; padding:4px; }\n"+
"TABLE  { border:1px solid #ececec; }\n"+
"TD     { margin:20px; padding:5px 5px 5px 5px; border:1px solid #ececec; }\n"+
"OL, UL { list-style-position:inside; }\n"+ 
"</style></head><body></body></html>");
  this.editor_doc.close();
  if (this.content.length >= 1) {
    this.editor_doc.body.innerHTML = this.content;
  }

  //METHOD ------------------------------

  this.init = function () {
    if (!this.editor_doc || this.editor_doc.designMode === undefined) {
      return alert("Oops! Bad browser, please use Firefox >= 2."); 
    }
    if (this.editor_doc.designMode != 'on') { 
      this.editor_doc.designMode = 'on'; 
    }
    this.selection = new Selection(this); 
    if (Prototype.Browser.Gecko) {
      this.editor_doc.execCommand("useCSS", false, true);
      this.editor_doc.execCommand("styleWithCSS", false, false);
    }
//    this.editor_win.focus();
  }

  this.setContent = function (action, params){
    if (this.source_mode == true) return;

    if (Prototype.Browser.IE && this.current_range!=null) {
      this.current_range.execCommand(action, false, params);
      this.current_range = null;
    }
/*    else if (Prototype.Browser.Gecko && action === "backColor") {
      var sTempFlag = new Date().getTime();
      this.editor_doc.execCommand('FontName',false, sTempFlag);
      var font_arr = this.editor_doc.getElementsByTagName("font");
      for (var i=0; i<font_arr.length; i++) {
	if (font_arr[i].face == sTempFlag) {
	  font_arr[i].face = "";
	  font_arr[i].style.backgroundColor = params;
	  font_arr[i].removeAttribute("face");
	}
      }  
    }*/
    else { 
      this.editor_doc.execCommand(action, false, params); 
    }
    
    if (action != "removeFormat" && Prototype.Browser.Gecko) { 
      // pb avec createlink
//      this.unselect();
    }

    if (action == "removeFormat") {
      thisH1 = this.selection.moveToAncestorNode('H1');
      thisH2 = this.selection.moveToAncestorNode('H2');
      if (!(thisH1 == undefined && thisH2 == undefined))
      this.editor_doc.execCommand("formatBlock", false, "p");
    }
    this.editor_win.focus();
  }

  this.initLinkDialog = function() {
    this.link = this.selection.moveToAncestorNode("A");
    if (this.link == undefined) { 
      return; 
    }
    this.link = Element.extend(this.link);
    var href = this.link.getAttribute("href");

    if (href.startsWith("mailto:")) {
      href = href.split("mailto:")[1];
      $("link_type").setValue("mailto:");
    }
    else if (href.startsWith("/file/get?path=")) {
      href = href.split("/file/get?path=")[1];
      $("link_type").setValue("download:");
    }
    $("link_url").setValue(href);
    
    if (this.link.getAttribute("target") === "_blank") {
      $("link_target").setValue("_blank");
    }

    $("button-remove").show();
  }

  this.insertLink = function() {
    if (this.link == undefined) { 
      this.link = Element.extend(this.createLink($F("link_url")));
    }
    var link = this.link;
    var type = $F("link_type");
    var href = $F("link_url");
    var target = $F("link_target");

    if (type === "download:") {
      href = "/file/get?path="+escape(href);
    }
    else if (type === "http:") {
      if (!(href.startsWith("/") || href.startsWith("http://"))) {
        href = "http://"+href; 
      }
    }
    else if (type === "mailto:") {
      if (!href.include("@")) {
        return alert("Oops! L'adresse email n'est pas valide.");
      }
      href = "mailto:"+href;
    }
    link.setAttribute("href", href);

    if ($F("link_class") != 0) {
      link.className = $F("link_class");
    }
    else { 
      link.removeAttribute("class"); 
    }

    if (target == "_blank") {
      link.setAttribute("target", target);
    }

    Kwo.getDialog().close();
  }

  this.removeLink = function() {
    if (this.link == undefined) { return false; }
    var html = this.link.innerHTML;
    this.link.parentNode.removeChild(this.link);
    this.insertHtml(html);
    Kwo.getDialog().close();
  }
  
  this.createLink = function(url) {
    var range;
    if (this.source_mode == true) return;
    if (Prototype.Browser.IE) range = this.current_range;
    this.setContent('Unlink') ;
    if (url.length < 1) {
      return ;
    }
    var sTempUrl = "javascript:void("+(new Date().getTime())+")";
    if (Prototype.Browser.IE) { this.current_range = range; }
    this.setContent('CreateLink', sTempUrl) ;
    var oLinks = this.editor_doc.getElementsByTagName("A");
    
    for (var i = 0; i < oLinks.length; i++) {
      if (oLinks[i].href == sTempUrl ) {
	oLinks[i].href = url;
	return (oLinks[i]);
      }
    }
  }
  
  this.insertTable = function(o) {	
    var i, j, html;

    //    if (Prototype.Browser.IE) rng = this.current_range;
    html = "<table "+o._spacing+" "+o._padding+" "+o._width+" "+o._height;
    //    if (o._class!=undefined && o._class!=-1) html += " class='"+o._class+"'";
    html += " "+o._layout;
    //    if (o._properties!=undefined && o._properties!=-1) html += " "+o._properties;
    html += ">";
    for (i = 0; i <= o._rows - 1; i++) {
      html = html + "<tr>";
      for(j = 0; j < o._cols; j++) {
        html = html + " <td></td>";
      }
      html = html + "</tr>";
    }
    html = html + "</table>";
    this.insertHtml(html);
    Kwo.getDialog().close();
  }

  this.insertCharacter = function(c) {  
    this.insertHtml(c);
    Kwo.getDialog().close();
  }

  this.setFgColor = function(color) {
    if (Prototype.Browser.Gecko) { 
      var selection = this.getSelection();
      if (this.isCollapsed() && selection.anchorOffset != 0) { 
        return alert('Oops! Veuillez sélectionner le texte.');          
      }
      if (selection.anchorOffset == 0) {
        var elt = this.selection.getParentElement();
        elt.style.color = color;
        this.unselect();
        return ;
      }
    }
    this.setContent("foreColor", color);
  }

  this.setBgColor = function(color) { 
    if (Prototype.Browser.Gecko) { 
      var selection = this.getSelection();
      if (selection.anchorOffset == 0) {
        var elt = this.selection.getParentElement();
        elt.style.backgroundColor = color;
        this.unselect();
        return ;
      }
      if (this.isCollapsed()) {
        this.setContent("backColor", color);
      }
      else {
        var range = this.getSelection().getRangeAt(0);
        if (range != null) {
          var span = document.createElement('span');
          span.style.backgroundColor = color;
          range.surroundContents(span);
        }
        this.unselect();
      }
    }
    else {
      this.setContent("backColor", color);
    }
  }

  this.setFont = function() { 
    var selection = this.getSelection();
    if (this.isCollapsed() && selection.anchorOffset != 0) {
      
    }
    else if (selection.anchorOffset == 0) {
      var elt = this.selection.getParentElement();
      if (!$F("font-family").blank()) {
        elt.style.fontFamily = $F("font-family");
      }
      if (!$F("font-size").blank()) {
        elt.style.fontSize = $F("font-size");
      }
    }
    else {
      var range = selection.getRangeAt(0);
      if (range != null) {
        var span = document.createElement('span');
        if (!$F("font-family").blank()) {
          span.style.fontFamily = $F("font-family");
        }
        if (!$F("font-size").blank()) {
          span.style.fontSize = $F("font-size");
        }
        range.surroundContents(span);
      }
      this.unselect();
    }
    Kwo.getDialog().close();
  }
  
  this.isCollapsed = function () {
    if (Prototype.Browser.Gecko) {
      var selection = this.getSelection();
      return selection.isCollapsed;
    }
  }
  
  this.unselect = function () {
    if (Prototype.Browser.Gecko) {
      var selection = this.getSelection();
      if (selection.isCollapsed === false) {
        selection.collapseToStart();
      }
    }
  }

  this.getSelection = function () {
    return this.editor_win.getSelection();
  }

  this.getContent = function () {
    if (this.source_mode === true) {
      this.toggleSource();
    }
    return this.editor_doc.body.innerHTML;
  }

  this.setStyle = function(style) {
    var sTempFlag = new Date().getTime();
    if (Prototype.Browser.IE && this.current_range != null) {
      this.current_range.execCommand('RemoveFormat', false, null);
      this.current_range.execCommand('FontName',false,sTempFlag);
      this.current_range = null;
    }
    else {
      this.editor_doc.execCommand('RemoveFormat', false, null);
      this.editor_doc.execCommand('FontName',false,sTempFlag);
    }
    var font_arr = editor_doc.getElementsByTagName("font");
    for (i=0; i<font_arr.length; i++) {
      if (font_arr[i].face == sTempFlag) {
	font_arr[i].face = "";
	font_arr[i].className = style;
	font_arr[i].removeAttribute("face");
      }
    } 
  }

  this.store = function () { 
    this.editor_field.value = this.editor_doc.body.innerHTML; 
  }

  this.openDialog = function(method) {
    if (this.source_mode == true) {
      return;
    }
    if (Prototype.Browser.IE) {
      this.current_range = this.editor_doc.selection.createRange(); 
      if (this.current_range.boundingLeft <= 0) {
        return alert("Oops! Veuillez vous placer dans l'éditeur.");
      }
    }
    Kwo.setEditor(this);
    if (method == "img") {
      new Kwo.Filepicker();
    }
    else if (method == "fgcolor") {
      new Kwo.Colorpicker(null, {"goal": "fgcolor"});
    }
    else if (method == "bgcolor") {
      new Kwo.Colorpicker(null, {"goal": "bgcolor"});
    }
    else {
      if (method == "cell") {
        var cell = this.selection.moveToAncestorNode("TD");
        if (cell == undefined) {
          return alert("Oops! Veuillez selectionner une cellule.");
        }
        this.cell = new Cell(cell);
      }
      new Kwo.Dialog("/back/editor/dialog."+method);
    }
  }

  this.debugRange = function(o) {
    var ret;
    if (o==undefined || o==null) return('TextRange object undefined');
    ret  = 'boundingHeight:'+o.boundingHeight+"\n";
    ret += 'boundingLeft:'+o.boundingLeft+"\n";
    ret += 'boundingTop:'+o.boundingTop+"\n";
    ret += 'boundingWidth:'+o.boundingWidth+"\n";
    ret += 'htmlText:['+o.htmlText+"]\n";
    ret += 'text:['+o.text+"]\n";
    alert(ret);
  }

  this.insertHtml = function(html) {
    if (this.source_mode) return;
    this.editor_win.focus();
    if (Prototype.Browser.IE) {
      if (this.current_range == null) {
        return alert('Oops! Error with selection.');
      }
      this.current_range.pasteHTML(html);
      this.current_range.collapse(false);
      this.current_range = null;
    } 
    else {
      this.editor_doc.execCommand('insertHTML', false, html);
    }
  }

  
  this.toggleSource = function() {
    if (this.source_mode === true) {
      this.editor_doc.body.innerHTML = this.editor_field.value;
      $(this.editor_field, this.editor_frame).invoke("toggle");
      if (Prototype.Browser.Gecko) {
	this.editor_doc.designMode = "off"; //WORKAROUND
	this.editor_doc.designMode = "on";
	this.editor_doc.execCommand("useCSS", false, true);
      }
      this.source_mode = false;
      this.editor_win.focus();
    }
    else {
//      alert(this.getContent());
      $(this.editor_frame, this.editor_field).invoke("toggle");
      this.source_mode = true;
      this.editor_field.focus();
    }
  }
  
/*  this.cleanHTML = function(str) {
    this.editor_field.value = convertToEntities(str);
    //    this.editor_field.value = convertToEntities(this.dom2xml(this.editor_doc.body, ''));
    //    alert(this.editor_field.value);
  }*/
  
}

Cell = Class.create({

  "initialize": function(node) {
    this.node = Element.extend(node);
  },
  
  "getStyle": function(style) {
    this.node.getStyle(style);
  }

});


// Class : Selection

function Selection(editor) {	

  this.editor = editor;
  
  this.getType = function () {
    if (Prototype.Browser.IE) {
      return this.editor.editor_doc.selection.type;
    }
    type = 'Text';
    var oSel = this.editor.editor_win.getSelection() ;
    if (oSel.rangeCount == 1) {
      var oRange = oSel.getRangeAt(0) ;
      if (oRange.startContainer == oRange.endContainer && 
          (oRange.endOffset - oRange.startOffset) == 1) {
        type = 'Control' ;
      }
    }
    return type;
  }

  this.getNode = function () {
    if (Prototype.Browser.IE) {
      if (this.getType() == 'Control' ) {
	var oRange = this.editor.editor_doc.selection.createRange() ;
	if ( oRange && oRange.item )
	  return this.editor.editor_doc.selection.createRange().item(0) ;
      }
    }
    else {
      if (this.getType() == 'Control') {
	var oSel = this.editor.editor_win.getSelection() ;
	var node = oSel.anchorNode.childNodes[oSel.anchorOffset] ;
        return node;
      }
/*      node = this.editor.editor_win.getSelection().anchorNode;
      alert(node.tagName);
      return node;*/
    }
  }

  this.getParentElement = function () {
    if (Prototype.Browser.IE) {
      if (this.getType() == 'Control') return this.getNode().parentElement ;
      else return this.editor.editor_doc.selection.createRange().parentElement() ;
    }
    else {
      if (this.getType() == 'Control') return this.getNode().offsetParent ;
      else {
	var oNode = this.editor.editor_win.getSelection().anchorNode ;
	while (oNode && oNode.nodeType!=1) oNode = oNode.parentNode ;
	return oNode ;
      }
    }
  }
  
  this.moveToAncestorNode = function (nodeTagName) {
    var oNode ;
    if (Prototype.Browser.IE) {
      if ( this.editor.editor_doc.selection.type == "Control" ) {
	var oRange = this.editor.editor_doc.selection.createRange();
	for ( i = 0 ; i < oRange.length ; i++ ) {
	  if (oRange(i).parentNode) { oNode = oRange(i).parentNode; break; }
	}
      }
      else {
	var oRange  = this.editor.editor_doc.selection.createRange() ;
	oNode = oRange.parentElement() ;
      }
      while ( oNode && oNode.nodeName != nodeTagName )
      oNode = oNode.parentNode ;
      return oNode ;
    }
    
    var oContainer = this.getNode() ;
    if (!oContainer) oContainer = this.editor.editor_win.getSelection().getRangeAt(0).startContainer ;
    
    while ( oContainer ) {
      if (oContainer.tagName==nodeTagName ) return oContainer ;
      oContainer = oContainer.parentNode ;
    }
  }

  this.hasAncestorNode = function(nodeTagName) {
    var elt = this.editor.selection.getParentElement();
    while (elt) {
      if (elt.tagName == nodeTagName) { break; }
      elt = elt.offsetParent;
    }
    if (elt && elt.tagName == nodeTagName) { return (true); }
    else return false;
  }

  this.getAncestorElement = function(tag_name) {
//    console.debug(this.editor.selection.anchorNode.tagName);
    if (!elt) return elt;
    if (elt.tagName == tag_name.uppercase()) {
      return elt;
    }
    return elt.up('TD');
  }

}


/*

If you want to wrap the current selection into an element then you can
use the range API e.g.
   var range = window.getSelection().getRangeAt(0);
   if (range != null) {
     var span = document.createElement('span');
     span.className = 'myClass';
     range.surroundContents(span);
   } 

*/

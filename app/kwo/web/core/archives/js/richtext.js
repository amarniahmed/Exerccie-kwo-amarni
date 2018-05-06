/*********
carret missing
**********/


Kwo.Richtext = Class.create({

  clicked: false,
  debug: false,
  dialog: null,  
  doc: null,  
  iframe: null,
  range: null,
  selection: null,
  src: null,
  toolbar: null,
  win: null,

  initialize: function(elt) { 
    var that = this;
    elt = $(elt);
//    if (!("execCommand" in window.document) || Prototype.Browser.IE) return ;
    this.toolbar = $(elt).up("div");
    elt.onclick = null;
    elt.onload = null;
    elt.observe("click", this.toggleSource.bindAsEventListener(this));
    elt.stopObserving("load");
    this.src = this.toolbar.next("textarea");
    this.src.addClassName("html-source");
    this.src.hide();
    var actions = {"bold": "gras", "italic": "italique", "underline": "souligné",
                   "indent": "bloc de citation",
                   "increasefontsize": "augmenter la taille", "decreasefontsize": "réduire la taille",
                   "h1": "titre 1", "h2": "titre 2", "h3": "titre 3",
                   "insertunorderedlist": "liste non ordonnée", "insertorderedlist": "liste ordonnée", 
//                   "outdent": false,
                   "justifyleft": "aligner à gauche", "justifyright": "aligner à droite", "justifycenter": "centrer",
                   "setstyle": "associer un style",
                   "insertimage": "insérer une image", "insertembed": "insérer un élément multimedia", 
                   "inserttable": "insérer un tableau",               
//                   "justifyfull": false,
                   "createlink": "créer un lien", "insertcharacter": "insérer un caractère",
                   "clipboard": "faire un copier/coller", "removeformat": "supprimer les styles"};
    if (this.debug == true) {
      actions["debug"] = false;
    }
    this.iframe = this.toolbar.next("iframe");
    this.win = this.iframe.contentWindow;
    this.doc = this.iframe.contentDocument;
    this.doc.open("text/html");
    this.doc.write("<html><head>"
                   + "<style>"
                   + "BODY { background:" + this.src.getStyle("background-color") + "; cursor:text; height:100%; "
                   + "       line-height:1.4em; " + "\n"
                   + "       border:none; color:#777; font-family:monospace; font-size:12px; margin:0px 2px 0 4px; }" + "\n"
                   + "TABLE.fixed { table-layout:fixed; width:100%;  }" + "\n"
                   + "TABLE TD, TABLE TH { border:1px solid #aaa; }" + "\n"
                   + "BIG { font-size:1.4em; }" + "\n"
                   + "SMALL { font-size:0.8em; }" + "\n"
                   + "OBJECT { background:#ffc; cursor:pointer; border:1px dotted #c00; display:inline-block; }" + "\n"
                   + "IFRAME { border:1px dotted #c00; padding:2px; }" + "\n"
                   + ".underline { text-decoration:underline; }" + "\n"
                   + "</style></head><body></body></html>");
    this.doc.close();
    if (this.src.getValue().length >= 1) {
      this.doc.body.innerHTML = this.src.getValue();
    }
    this.iframe.observe("mouseout", this.onContentSave.bindAsEventListener(this));
    this.doc.addEventListener("keyup", this.onContentSave.bindAsEventListener(this), false);
    var action, param, img, key;
    var toolbar = this.toolbar;
    for (key in actions) {
      if (key.length == 2) { action = "heading"; param = key; }
      else { action = key; param = actions[key]; }
      img = new Element("img", {"src": "/web/core/images/richtext/" + key + ".gif", 
                                "class": "action", 
                                "width": 10, 
                                "height": 10});
      img.observe("click", this.onExec.bind(this, action, param));
      img.title = actions[key];
      toolbar.insert(img);
    };
    this.toolbar.observe("mousedown", function(evt) {
      Event.stop(evt);
    });
    this.doc.designMode = "on";
    var link = this.doc.createElement("link");
    link.type = "text/css";
    link.rel = "stylesheet";
    link.href = "/web/core/css/styles.css";
    link.media = "screen";
    link.async = true;
    this.doc.getElementsByTagName("head")[0].appendChild(link);
    elt.src = "/web/core/images/richtext/src.gif";
    this.doc.body.addEventListener("click", function (evt) {
      that.clicked = true;
      that.toolbar.show();
//      console.log(evt.target.tagName);
    }, false);
    this.iframe.up("DIV.elem").observe("mouseout", function (evt) {
//      that.toolbar.hide();
    });
    this.iframe.up("DIV.elem").observe("mouseover", function (evt) {
//      if (that.clicked) that.toolbar.show();
    });
    this.onRegister();
  },

  onRegister: function() {
    //this.doc.execCommand("enableObjectResizing", false, false);
    this.doc.execCommand("styleWithCSS", false, false);
//    this.doc.execCommand("insertBrOnReturn", false, false);
  },

  onContentSave: function() {
    //console.log(this.doc.body.innerHTML);
    var source = this.doc.body.innerHTML;
    source = source.replace(/<b>(.*)<\/b>/gi, "<strong>$1</strong>");
    source = source.replace(/<i>(.*)<\/i>/gi, "<em>$1</em>");
    source = source.replace(/\.\.\/doc\//g, "/doc/");
    source = source.replace(/src\=\"doc\//g, "src=\"/doc/");
    //console.log(source);
    this.src.value = source;
  },

  toggleSource: function() {
    if (this.src.visible()) {
      this.doc.body.innerHTML = this.src.value;
      this.src.hide();
      this.onRegister();
      this.iframe.show();
      this.toolbar.select("IMG.action").invoke("show");
    }
    else {
      this.onContentSave();
      this.iframe.hide();
      this.src.show();
      this.toolbar.select("IMG.action").invoke("hide");
      this.src.focus();
    }
  },

  onStateRestore: function() {
    this.focus(); 
    var selection = this.win.getSelection();
    selection.removeAllRanges();
    selection.addRange(this.range);
  },

  onHTMLInsert: function(html) {
    if (this.dialog) {
      this.dialog.close();
    }
    this.doc.execCommand("insertHTML", false, html);
    this.range.collapse(false);
    this.onContentSave();
  },

  onLinkRemove: function(link) {
    var html = link.innerHTML;
    link.parentNode.removeChild(link);
    this.onHTMLInsert(html);
  },

  onLinkInsert: function(h, link) {
    if (h["link-url"].length < 1) {
      return alert("Oops! Veuillez préciser le lien.");
    }
    this.dialog.close();
    var html = "";
    if (link == null) {
      if (this.range.collapsed) {
        var text = this.doc.createTextNode(h["link-url"]);
        this.range.insertNode(text);
        this.range.selectNode(text);
      }
      var elt = this.doc.createElement("a");
      this.range.surroundContents(elt);
      link = elt;
    }
    if (h["link-type"] === "download") {
      h["link-url"] = "/file/get?path=" + escape(h["link-url"]);
    }
    else if (h["link-type"] === "http") {
      if (!(h["link-url"].startsWith("/") || h["link-url"].startsWith("http"))) {
        h["link-url"] = "http://" + h["link-url"];
      }
    }
    else if (h["link-type"] === "mailto") {
      if (!h["link-url"].include("@")) {
        return alert("Oops! L’adresse email semble invalide.");
      }
      h["link-url"] = "mailto:" + h["link-url"];
    }
    if (h["link-type"] != "mailto" &&
        !(h["link-url"].toLowerCase().startsWith("http://") ||
          h["link-url"].toLowerCase().startsWith("/") ||
          h["link-url"].toLowerCase().startsWith("https://"))) {
      h["link-url"] = "http://" + h["link-url"];
    }
    link.setAttribute("href", h["link-url"]);
    if (h["link-class"].length > 1) {
      link.className = h["link-class"];
    }
    else {
      link.removeAttribute("class");
    }
    if (h["link-title"].length > 0) {
      link.setAttribute("title", h["link-title"]);
    }
    else {
      link.removeAttribute("title");
    }
    if (h["link-target"] == "_blank") {
      link.setAttribute("target", h["link-target"]);
    }
    else {
      link.removeAttribute("target");
    }
    this.range.collapse(false);
    this.onContentSave();
  },

  onCharacterInsert: function(html) {
    this.onHTMLInsert(html);
  },

  onClipboardPaste: function(html) {
    html = html.replace(/\n/g, "<br/>");
    this.onHTMLInsert(html);
  },

  onEmbedInsert: function(html) {
    this.onHTMLInsert(html);
  },

  onTableInsert: function(h) {
    var html, i, j;
    h["width"] = h["width"] ? h["width"] : "100%";
    h["rows"] = h["rows"].intval() >= 1 ? h["rows"].intval() : 2;
    h["cols"] = h["cols"].intval() >= 1 ? h["cols"].intval() : 2;
    html = '<table class="fixed" style="width:' + h["width"] + '"><tbody>';
    for (i = 1; i <= h["rows"]; i++) {
      html += "<tr>";
      for (j = 1; j <= h["cols"]; j++) {
        html += "<td></td>";
      }
      html += "</tr>";
    }
    html += "</tbody></table>";

    this.onHTMLInsert(html);
  },

  onImageInsert: function(file) {
    var style = "";
    if (!file["width"].empty()) {
      style += "width:" + file["width"] + "px;";
    }
    if (!file["float"].empty()) {
      style += "float:" + file["float"] + ";";
    }
    if (!style.empty()) {
      style = ' style="' + style + '"';
    }
    style += '"';
    var html = '<img src="' + file["path"] + '" alt="' + file["alt"] + '"' + style + '/> ';
    
    this.onHTMLInsert(html);
  },

  onStyleApply: function(style, tag) {
    this.dialog.close();
    if (tag.length > 0) {  
      var node = this.range.commonAncestorContainer;
      while (node) {
        if (node.tagName == tag || node.tagName == "BODY") break ;
        node = node.parentNode;
      }
      if (node && node.tagName == tag) {
        node.className = style;
      }
    }
    else if (this.range != null) { 
      var elt = this.doc.createElement("span");
      elt.className = style;
      this.range.surroundContents(elt);
    }
    this.range.collapse(false);
    this.onContentSave();
  },

  onExec: function(name, value) {
    this.focus();
    this.dialog = null;
    this.selection = this.win.getSelection();
    if (this.selection.rangeCount > 0) {
      this.range = this.selection.getRangeAt(0);
    }
    else {
      alert("no range");
    }
    this.range = this.range.cloneRange();
    if (name == "createlink") {
      this.dialog = Kwo.Layer("dialog", "link", this);
    }
    else if (name == "setstyle") {
      if (this.range.collapsed) return alert("Veuillez sélectionner le texte");
      this.dialog = Kwo.Layer("dialog", "styles");
      this.dialog.onSelect = this.onStyleApply.bind(this);
    }
    else if (name == "insertcharacter") {
      this.dialog = Kwo.Layer("dialog", "characters");
      this.dialog.onSelect = this.onCharacterInsert.bind(this);
    }
    else if (name == "clipboard") {
      this.dialog = Kwo.Layer("dialog", "clipboard");
      this.dialog.onPaste = this.onClipboardPaste.bind(this);
    }
    else if (name == "insertimage") {
      this.dialog = new Kwo.Class.ImageManager(this);
      this.dialog.onSelect = this.onImageInsert.bind(this);
    }
    else if (name == "insertembed") {
      this.dialog = Kwo.Layer("dialog", "insertembed");
      this.dialog.onPaste = this.onEmbedInsert.bind(this);
    }
    else if (name == "inserttable") {
      this.dialog = Kwo.Layer("dialog", "table");
      this.dialog.onAccept = this.onTableInsert.bind(this);
    }
    else if (name == "debug") {
      console.log(this.range);
      console.log(this.selection);
    }
    else if (name == "removeformat") {
      if (this.range.collapsed == false) {
        var snode, enode;
        snode = this.selection.anchorNode.nodeType != 1
              ? this.selection.anchorNode.parentNode
              : this.selection.anchorNode;
        enode = this.selection.focusNode.nodeType != 1
              ? this.selection.focusNode.parentNode
              : this.selection.focusNode;
        if (snode == enode && 
            (snode.tagName == "H1" || snode.tagName == "H2" ||
             snode.tagName == "H3" || snode.tagName == "H4" ||
             snode.tagName == "DIV")) {
//          console.log(snode); 
          var txt = this.doc.createTextNode(snode.textContent);
          snode.parentNode.replaceChild(txt, snode);
        }
        else {
          this.doc.execCommand("removeformat", false, value);
          this.selection.collapseToEnd();
        }
      }
      this.focus();
    }
    else {
      this.doc.execCommand(name, false, value);
      this.selection.collapseToEnd();
      this.focus();
    }
    if (this.dialog) {
      this.selection.removeAllRanges();
      var that = this;
      this.dialog.onAfterClose = function() {
        that.onStateRestore();
        that.dialog = null;
      };
    }
    this.onContentSave();
  },

  focus: function() {
    this.win.focus();
  }

});

Kwo.Handlers["link"] = {

  link: null,
  editor: null,
  file_panel: null,

  initialize: function($super, editor) {
    this.name = "link";
    this.className = "layout-hbox";
    this.width = 400;
    this.height = 350;
    this.editor = editor;
    var node = editor.range.commonAncestorContainer.parentNode;
    while (node && node.tagName != "BODY") {
      if (node.tagName == "A") {
        this.link = node;
        break ;
      }
      node = node.parentNode;
    }
    $super(this.onDisplay);
  },

  onDisplay: function() {
    Kwo.exec("/back/core/editor.link", {},
             {container: this.support,
              callback: this.onBind.bind(this) });
  },

  onBind: function() { 
    var that = this;
    $("button-insert").observe("click", function(evt) {
      that.editor.onLinkInsert(evt.element().up("FORM").serialize(true), 
                               that.link);
    });
    $("button-lookup").observe("click", this.onSearch.bindAsEventListener(this));
    if (this.link == null) return ; 
    var href = this.link.getAttribute("href");
    if (href.startsWith("mailto")) {
      href = href.split("mailto:")[1];
      $("link-type").setValue("mailto");
    }
    else if (href.startsWith("/file/get?path=")) {
      href = href.split("/file/get?path=")[1];
      $("link-type").setValue("download");
    }
    $("link-class").setValue(this.link.className);
    $("link-title").setValue(this.link.getAttribute("title"));
    $("link-url").setValue(href);
    if (this.link.getAttribute("target") === "_blank") {
      $("link-target").setValue("_blank");
    }
    $("button-remove").show();
    $("button-remove").observe("click", function(evt) {
      var elt = evt.element();
      elt.stopObserving("click");
      that.editor.onLinkRemove(that.link);
    });
  },

  onSearch: function(evt) {
    if ($F("link-type") == "mailto") return ;
    if ($F("link-type") == "download") {
      var elt = evt.element();
      var fm = new Kwo.Class.FileManager(elt);
      fm.onSelect = function (path) {
        elt.up("DIV").down("INPUT").value = "/" + path;
        fm.close();
      };
    }
    else {
      var elt = evt.element();
      var picker = new Kwo.Class.ItemPicker(elt);
      picker.onSelect = function(item) {
        elt.up("DIV").down("INPUT").value = item["url"];
        picker.close()
      };
    }
  }

};

Kwo.Handlers["clipboard"] = {

  initialize: function($super) {
    this.name = "clipboard";
    this.className = "layout-hbox";
    this.width = 700;
    this.height = 450;
    $super(this.onDisplay);
  },
  
  onDisplay: function() {
    Kwo.exec("/back/core/editor.clipboard", {},
             {container: this.support, 
              callback: this.onDisplayCallback.bind(this) });
  },

  onDisplayCallback: function() {
    var that = this;
    this.support.down("INPUT[type=button]").observe("click", function(evt) {
      var elt = Event.element(evt);
      elt.stopObserving("click");
      that.onPaste(elt.up("FORM").down("TEXTAREA").getValue());
    });
  },

  onPaste: function(txt) {
    alert(txt);
  }

};

Kwo.Handlers["insertembed"] = {

  initialize: function($super) {
    this.name = "insertembed";
    this.className = "layout-hbox";
    this.width = 700;
    this.height = 450;
    $super(this.onDisplay);
  },
  
  onDisplay: function() {
    Kwo.exec("/back/core/editor.embed", {},
             {"container": this.support, 
              "callback": this.onDisplayCallback.bind(this) });
  },

  onDisplayCallback: function() {
    var that = this;
    this.support.down("INPUT[type=button]").observe("click", function(evt) {
      var elt = Event.element(evt);
      elt.stopObserving("click");
      that.onPaste(elt.up("FORM").down("TEXTAREA").getValue());
    });
  },

  onPaste: function(txt) {
    alert(txt);
  }

};

Kwo.Handlers["characters"] = {

  initialize: function($super) {
    this.name = "characters";
    this.className = "layout-hbox";
    this.width = 410;
    this.height = 176;
    $super(this.onDisplay);
  },

  onDisplay: function() {
    Kwo.exec("/back/core/editor.characters", {},
             {"container": this.support,
              "callback": this.onDisplayCallback.bind(this) });
  },

  onDisplayCallback: function() {
    var that = this;
    this.support.down("DIV").observe("click", function(evt) {
      var elt = Event.element(evt); 
      if (elt.tagName.toUpperCase() != "A") return ;
      elt.up("DIV").stopObserving("click");
      that.onSelect(elt.readAttribute("data-value"));
    });
  },

  onSelect: function(txt) {
    alert(txt);
  }

};

Kwo.Handlers["styles"] = {

  initialize: function($super) {
    this.name = "styles";
    this.className = "layout-hbox";
    this.width = 500;
    this.height = 300;
    $super(this.onDisplay);
  },

  onDisplay: function() {
    Kwo.exec("/back/core/editor.styles", {},
             {"container": this.support,
              "callback": this.onDisplayCallback.bind(this) });
  },

  onDisplayCallback: function() {
    var that = this;
    this.support.down("div").observe("click", function(evt) {
      var elt = Event.element(evt);
      if (elt.tagName.toUpperCase() != "A") return ;
      that.onSelect(elt.getAttribute("data-style"),
                    elt.getAttribute("data-tag"));
    });
  },

  onSelect: function(style, tag) {
    alert(style + " - " + tag);
  }

};

Kwo.Handlers["table"] = {

  initialize: function($super) {
    this.name = "table";
    this.className = "layout-hbox";
    this.width = 600;
    this.height = 400;
    $super(this.onDisplay);
  },

  onDisplay: function() {
    Kwo.exec("/back/core/editor.table", {},
             {"container": this.support,
              "callback": this.onDisplayCallback.bind(this) });
  },

  onDisplayCallback: function() {
    var that = this;
    this.support.down("input[type=button]").observe("click", function(evt) {
      evt.stop();
      that.onAccept($(Event.element(evt).form).serialize(true));
    });
  },

  onAccept: function(h) {
    alert(Object.toJSON(h));
  }

};


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
    if (false) {
      elt = $(elt);
      this.toolbar = $(elt).up("DIV");
    }
    else {
      this.toolbar = $(elt).previous("DIV");
      $(elt).remove();
      elt = this.elt = this.toolbar.down(".source");
    }
    elt.onclick = null;
    elt.onload = null;
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
                   "justifyleft": "aligner à gauche", "justifyright": "aligner à droite", "justifycenter": "centrer", "justifyfull": "justifié",
                   "setstyle": "associer un style",
                   "insertimage": "insérer une image", "insertembed": "insérer un élément multimedia",
                   "inserttable": "insérer un tableau",
                   "createlink": "créer un lien", "insertcharacter": "insérer un caractère",
                   "clipboard": "faire un copier/coller", "removeformat": "supprimer les styles"};
    if (this.debug == true) {
      actions["debug"] = false;
    }
    this.iframe = this.toolbar.next("IFRAME");

    // BUG : src="javascript:;" -> ne déclenche pas le onload de firefox
    //       src="data:image/gif;" -> n'est pas accepté par WebKit
    if (Prototype.Browser.WebKit) {
//    this.iframe.observe("load", this.onLoad.bindAsEventListener(this));
      this.onLoad();
    }
    else {
      this.onLoad();
    }

    var action, param, img, key;
    var toolbar = this.toolbar;
    for (key in actions) {
      if (key.length == 2) {
        action = "heading";
        param = key; }
      else {
        action = key;
        param = actions[key];
      }
      img = new Element("img", {"src": "/web/core/images/richtext/" + key + ".gif",
                                "class": "action",
                                "width": 10,
                                "height": 10});
      img.observe("click", this.onExec.bind(this, action, param));
      img.title = actions[key];
      toolbar.insert(img);
    };
    this.toolbar.observe("mousedown", function(evt) {
      evt.stop();
    });
    elt.src = "/web/core/images/richtext/src.gif";
    elt.observe("click", this.toggleSource.bindAsEventListener(this));

//    this.onRegister();
  },

  onLoad: function(evt) {
    if (evt instanceof Event) {
      console.log(evt);
    }
    var that = this;
    this.win = this.iframe.contentWindow;
    this.doc = this.iframe.contentDocument;

    if (this.iframe.hasClassName("has-content")) {
//      console.log(this.doc.body);
      this.iframe.observe("mouseout", this.onContentSave.bindAsEventListener(this));
      this.doc.addEventListener("keyup", this.onContentSave.bindAsEventListener(this), false);
      this.doc.designMode = "on";
    }
    else {
      this.doc.open("text/html");
      this.doc.write("<html><head>"
                     + "<style>"
                     + "BODY { background:" + this.src.getStyle("background-color") + "; cursor:text; height:100%; "
                     + "       line-height:1.4em; " + "\n"
                     + "       border:none; color:#777; font-family:monospace; font-size:12px; margin:0px 2px 0 4px; }" + "\n"
                     + "TABLE.fixed { table-layout:fixed; width:100%; }" + "\n"
                     + "TABLE TD, TABLE TH { border:1px solid #aaa; vertical-align:top; padding:5px; }" + "\n"
                     + "BIG { font-size:1.4em; }" + "\n"
                     + "SMALL { font-size:0.8em; }" + "\n"
                     + "OBJECT { background:#ffc; cursor:pointer; border:1px dotted #c00; display:inline-block; }" + "\n"
                     + "IFRAME { border:1px dotted #c00; padding:2px; }" + "\n"
                     + ".underline { text-decoration:underline; }" + "\n"
                     + "</style>" + "\n"
                     + '<link href="/web/core/styles/stylesheet.css" rel="stylesheet" type="text/css" async="async" />' + "\n"
                     + "</head><body></body></html>");
      this.doc.close();
      if (this.src.getValue().length >= 1) {
        this.doc.body.innerHTML = this.src.getValue();
      }
/*      var link = this.doc.createElement("link");
      link.type = "text/css";
      link.rel = "stylesheet";
      link.href = "/web/core/styles/stylesheet.css";
      link.media = "screen";
      link.async = true;
      this.doc.getElementsByTagName("head")[0].appendChild(link); */
      this.iframe.observe("mouseout", this.onContentSave.bindAsEventListener(this));
      this.doc.addEventListener("keyup", this.onContentSave.bindAsEventListener(this), false);
//      this.doc.designMode = "on";
      this.doc.body.contentEditable = "true";
    }

    this.doc.body.addEventListener("click", function (evt) {
      that.clicked = true;
      that.toolbar.show();
    }, false);
    this.win.addEventListener("paste", this.onPaste.bindAsEventListener(this), false);
    //    this.iframe.addEventListener("beforepaste", this.onPaste.bindAsEventListener(this), false);
    //    this.win.addEventListener("beforepaste", this.onPaste.bindAsEventListener(this), false);
    //    this.doc.addEventListener("beforepaste", this.onPaste.bindAsEventListener(this), false);
  },

  onRegister: function() {
    //this.doc.execCommand("enableObjectResizing", false, false);
//    this.doc.execCommand("styleWithCSS", false, false);
//    this.doc.execCommand("useCSS", false, false);
//    this.doc.execCommand("insertBrOnReturn", false, false);
  },

  onPaste: function(evt) {
//    evt.stop();
//    console.log(evt);
//    console.log(window);
    if (!("clipboardData" in evt)) return ;
    evt.preventDefault();
    evt.stopPropagation();
//    console.log(evt);
    var cb = evt.clipboardData;
    var txt = cb.getData("text/plain").replace(/\n/g, "<br/>");
//    cb.setData("text/plain", "coucou");
    this.onHtmlInsert(txt);
  },

  onContentSave: function() {
    //console.log(this.doc.body.innerHTML);
    var source = this.doc.body.innerHTML;
    source = source.replace(/<b>(.*)<\/b>/gi, "<strong>$1</strong>");
    source = source.replace(/<i>(.*)<\/i>/gi, "<em>$1</em>");
    source = source.replace(/(<\/h.>)<br\/?>/gi, "$1\n");
    source = source.replace(/\.\.\/doc\//g, "/doc/");
    source = source.replace(/src\=\"doc\//g, "src=\"/doc/");
    source = source.replace(/%28%28/g, "((");
    source = source.replace(/%29%29/g, "))");
    source = source.replace(/ align="center"/g, ' style="text-align:center;"');
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

  onHtmlInsert: function(html) {
    if (this.dialog) {
      this.dialog.close();
    }
    this.doc.execCommand("insertHTML", false, html);
    if (this.range) {
      this.range.collapse(false);
    }
    this.onContentSave();
  },

  onLinkRemove: function(link) {
    var html = link.innerHTML;
    link.parentNode.removeChild(link);
    this.onHtmlInsert(html);
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
      h["link-url"] = "/file.get?path=" + escape(h["link-url"]);
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
    this.onHtmlInsert(html);
  },

  onClipboardPaste: function(html) {
    html = html.replace(/\n/g, "<br/>");
    this.onHtmlInsert(html);
  },

  onEmbedInsert: function(html) {
    this.onHtmlInsert(html);
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

    this.onHtmlInsert(html);
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

    this.onHtmlInsert(html);
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
      this.dialog = new kwo.dialogs.LinkManager(this);
    }
    else if (name == "setstyle") {
      if (this.range.collapsed) return alert("Veuillez sélectionner le texte");
      this.dialog = new kwo.dialogs.StyleManager();
      this.dialog.onSelect = this.onStyleApply.bind(this);
    }
    else if (name == "insertcharacter") {
      this.dialog = new kwo.dialogs.CharacterPicker();
      this.dialog.onSelect = this.onCharacterInsert.bind(this);
    }
    else if (name == "clipboard") {
      this.dialog = new kwo.dialogs.ClipboardManager();
      this.dialog.onSelect = this.onClipboardPaste.bind(this);
    }
    else if (name == "insertimage") {
      this.dialog = new kwo.dialogs.ImageManager(this);
      this.dialog.onSelect = this.onImageInsert.bind(this);
    }
    else if (name == "insertembed") {
      this.dialog = new kwo.dialogs.EmbedManager(this);
      this.dialog.onSelect = this.onEmbedInsert.bind(this);
    }
    else if (name == "inserttable") {
      this.dialog = new kwo.dialogs.TableManager();
      this.dialog.onSelect = this.onTableInsert.bind(this);
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
      if (name == "heading") {
        if (Prototype.Browser.WebKit) {
          var elt = this.doc.createElement(value);
          this.range.surroundContents(elt);
        }
        else {
          this.doc.execCommand(name, false, value);
        }
      }
      else if (name == "increasefontsize" || name == "decreasefontsize") {
        if (this.doc.queryCommandSupported && this.doc.queryCommandSupported(name)) {
          this.doc.execCommand(name, false, value);
        }
        else {
          var tag = name == "increasefontsize" ? 'big' : 'small';
          var elt = this.doc.createElement(tag);
          this.range.surroundContents(elt);
        }
      }
      else {
        this.doc.execCommand(name, false, value);
      }
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
    if (this.win) {
      this.win.focus();
    }
    this.onRegister();
  }

});

kwo.dialogs.LinkManager = Class.create(kwo.ui.Dialog, {

  "link": null,
  "editor": null,
  "file_panel": null,

  initialize: function($super, editor) {
    this.editor = editor;
    var node = editor.range.commonAncestorContainer.parentNode;
    while (node && node.tagName != "BODY") {
      if (node.tagName == "A") {
        this.link = node;
        break ;
      }
      node = node.parentNode;
    }
    $super({"name": "link",
            "title": "lien",
            "width": 400,
            "height": 350});
    this.onDisplay();
  },

  onDisplay: function() {
    Kwo.exec("/back/core/editor.link", null,
             {"container": this.container,
              "callback": this.onBind.bind(this) });
  },

  onBind: function() {
    var that = this;
    $("button-insert").observe("click", function(evt) {
      that.editor.onLinkInsert(evt.element().up("FORM").serialize(true),
                               that.link);
    });
    $("button-lookup").observe("click", this.onSearch.bindAsEventListener(this));
    $("link-type").observe("change", this.onTypeChange.bindAsEventListener(this));
    if (this.link == null) return ;
    var href = this.link.getAttribute("href");
    if (href.startsWith("mailto")) {
      href = href.split("mailto:")[1];
      $("link-type").setValue("mailto");
    }
    else if (href.startsWith("/file.get?path=")) {
      href = href.split("/file.get?path=")[1];
      $("link-type").setValue("download");
      $("button-lookup").show();
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

  onTypeChange: function(evt) {
    evt.stop();
    var type = $("link-type").getValue();
    if (type == "download") {
      $("button-lookup").show();
    }
    else {
      $("button-lookup").hide();
    }
  },

  onSearch: function(evt) {
    evt.stop();
    var elt = evt.element();
    if ($F("link-type") == "mailto") return ;
    if ($F("link-type") == "download") {
      var fm = new kwo.dialogs.FileManager(elt);
      fm.onSelect = function(path) {
        elt.up("DIV").down("INPUT").value = path;
        fm.close();
      };
    }
    else {
      var ip = new kwo.dialogs.ItemPicker(elt);
      ip.onSelect = function(items) {
        var item = items[0];
        elt.up("DIV").down("INPUT").value = item["url"];
        ip.close()
      };
    }
  }

});

kwo.dialogs.ClipboardManager = Class.create(kwo.ui.Dialog, {

  initialize: function($super) {
    $super({"name": "clipboard",
            "title": "zone de copier/coller",
            "width": 700,
            "height": 450});
    this.onDisplay();
  },

  onDisplay: function() {
    Kwo.exec("/back/core/editor.clipboard", null,
             {"container": this.container,
              "callback": this.onBind.bind(this) });
  },

  onBind: function() {
    var that = this;
    this.container.down("INPUT[type=button]").observe("click", function(evt) {
      evt.stop();
      var elt = evt.element();
      that.onSelect(elt.up("FORM").down("TEXTAREA").getValue());
    });
  },

  onSelect: function(txt) {
    console.log(txt);
  }

});

kwo.dialogs.EmbedManager = Class.create(kwo.ui.Dialog, {

  initialize: function($super) {
    $super({"name": "embed",
            "title": "insertion d’une animation",
            "width": 700,
            "height": 450})
    this.onDisplay();
  },

  onDisplay: function() {
    Kwo.exec("/back/core/editor.embed", null,
             {"container": this.container,
              "callback": this.onBind.bind(this)});
  },

  onBind: function() {
    var that = this;
    this.container.down("INPUT[type=button]").observe("click", function(evt) {
      evt.stop();
      var elt = evt.element();
      that.onSelect(elt.up("FORM").down("TEXTAREA").getValue());
    });
  },

  onSelect: function(txt) {
    console.log(txt);
  }

});

kwo.dialogs.CharacterPicker = Class.create(kwo.ui.Dialog, {

  initialize: function($super) {
    $super({"name": "character-picker",
            "title": "caractères",
            "width": 410,
            "height": 176});
    this.onDisplay();
  },

  onDisplay: function() {
    Kwo.exec("/back/core/editor.characters", null,
             {"container": this.container,
              "callback": this.onBind.bind(this)});
  },

  onBind: function() {
    var that = this;
    this.container.observe("click", function(evt) {
      evt.stop();
      var elt = evt.element();
      if (elt.tagName != "A") return ;
      elt.up("DIV").stopObserving("click");
      that.onSelect(elt.readAttribute("data-value"));
    });
  },

  onSelect: function(txt) {
    console.log(txt);
  }

});

kwo.dialogs.StyleManager = Class.create(kwo.ui.Dialog, {

  initialize: function($super) {
    $super({"name": "style",
            "title": "styles",
            "width": 500,
            "height": 300});
    this.onDisplay();
  },

  onDisplay: function() {
    Kwo.exec("/back/core/editor.styles", null,
             {"container": this.container,
              "callback": this.onBind.bind(this) });
  },

  onBind: function() {
    var that = this; console.log("ici");
    this.container.observe("click", function(evt) {
      evt.stop();
      var elt = evt.element();
      if (elt.tagName != "A") return ;
      that.onSelect(elt.getAttribute("data-style"),
                    elt.getAttribute("data-tag"));
    });
  },

  onSelect: function(style, tag) {
    console.log(style, tag);
  }

});

kwo.dialogs.TableManager = Class.create(kwo.ui.Dialog, {

  initialize: function($super) {
    $super({"name": "table",
            "title": "tableau",
            "width": 600,
            "height": 280});
    this.onDisplay();
  },

  onDisplay: function() {
    Kwo.exec("/back/core/editor.table", null,
             {"container": this.container,
              "callback": this.onBind.bind(this)});
  },

  onBind: function() {
    var that = this;
    this.container.down("input[type=button]").observe("click", function(evt) {
      evt.stop();
      var elt = evt.element();
      that.onSelect(elt.form.serialize(true));
    });
  },

  onSelect: function(h) {
    console.log(h);
  }

});


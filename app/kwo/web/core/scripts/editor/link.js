Kwo.Link = {

  file_panel: null,

  init: function() {
    $("button-insert").show();
    if (editor.link == null) return ;
    editor.link = Element.extend(editor.link);
    var href = editor.link.getAttribute("href");
    if (href.startsWith("mailto")) {
      href = href.split("mailto:")[1];
      $("link_type").setValue("mailto");
    }
    else if (href.startsWith("/file/get?path=")) {
      href = href.split("/file/get?path=")[1];
      $("link_type").setValue("download");
    }
    $("link_class").setValue(editor.link.className);
    $("link_url").setValue(href);
    if (editor.link.getAttribute("target") === "_blank") {
      $("link_target").setValue("_blank");
    }
    $("button-remove").show();
    Kwo.Link.onTypeChange($("link_type"));
  },

  onTypeChange: function (elt) {
    elt = $(elt);
    if ($F(elt) === "mailto") {
      elt.up("TABLE").select(".ico")[0].hide();
      $("item-selector-box").hide();
      $("file-selector-box").hide();
    }
    else {
      elt.up("TABLE").select(".ico")[0].show();
      if ($F(elt) === "download") {
        $("item-selector-box").hide();
      }
      else {
        $("file-selector-box").hide();
      }
    }
  },

  onLookup: function(elt) {
    if ($F("link_type") == "mailto") {
      $("item-selector-box").hide(); 
      $("file-selector-box").hide();
    } 
    else if ($F("link_type") == "download") {
      $("item-selector-box").hide(); 
      $("file-selector-box").show();
      if (Kwo.Link.file_panel == null) {
        Kwo.Link.file_panel = new Kwo.Class.FilePanel($("file-selector-box"));
        Kwo.Link.file_panel.onFileSelect = function (file) {
          $("link_url").value = "/" + file["path"];
          $("file-selector-box").hide();
        };
      }
    }
    else {
      $("file-selector-box").hide();
      $("item-selector-box").show(); 
    }
  },

  onItemSelect: function(item) {
    $("link_url").value = item["url"];
    $("item-selector-box").hide();
  },

  onItemSearch: function(args) {
    Kwo.exec("/back/core/item.search", args, 
             {container: "item-results-box"});
  },

  insert: function() {
    editor.insertLink($("link_editor"));
  },

  remove: function() {
    editor.removeLink();
  }

}

editor = Kwo.getEditor();
Kwo.Link.init(); 

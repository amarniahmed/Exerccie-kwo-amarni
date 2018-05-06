
function S(name) {
  return new Kwo.Class.Obj(name);
}

Kwo.trees = {};

Kwo.Class.Obj = Class.create({

  action: "",
  elt: null,
  edit_container: null,

  initialize: function(name) {
    this.name = name;
  },

  onDestroy: function(elt) {
    this.action = "destroy";
    this.elt = $(elt).up("." + this.name + "-box");
    Kwo.exec(this.root + "/" + this.name + ".destroy", this.getArgs(this.elt),
             {"confirm": "êtes vous sûr de vouloir supprimer cet élément ?",
              "callback": this.onDestroyCallback.bind(this)});
  },

  onDestroyCallback: function (res) {
    if (Kwo.hasError(res)) return Kwo.error(res);
    
    this.elt.setAttribute("data-id", 0);

    if (res["result"]["is_node"] == true) {
      this.elt.down("." + this.name + "-edit-box").update();
      Kwo.trees[this.name].onRefresh(res["result"]["ancestor_id"]);
      return ;
    }
    
    if (res["result"]["action"] == "edit") {
      this.onEdit(this.elt);
      return ;
    }

    if (this.elt.hasClassName("workspace-box")) {
      this.elt.down("." + this.name + "-edit-box").update()
    }

    this.onHome(this.elt);
  },

  onDuplicate: function(elt) {
    this.action = "duplicate";
    this.elt = $(elt).up("." + this.name + "-box");
    var args = this.getArgs(elt);
    Kwo.exec(this.root + "/" + this.name + ".duplicate", args,
             {"confirm": "êtes vous sûr de vouloir dupliquer cet élément ?",
              "callback": this.onDuplicateCallback.bind(this)});
  },

  onDuplicateCallback: function (res) {
    if (Kwo.hasError(res)) return Kwo.error(res);
//    if (confirm("duplication effectuée.\nSouhaitez vous accéder au nouvel élément ?")) {
    this.elt.setAttribute("data-id", res["result"]["id"]);
    if (res["result"]["is_node"] == true) {
      Kwo.trees[this.name].onRefresh(res["result"]["id"]);
    }
//    console.log(this.elt);
    this.onEdit(this.elt);
    //    }
  },

  onEdit: function(elt) {
    this.action = "edit";
    this.elt = elt;

    var container;
    if (elt.hasClassName(this.name + "-box") || elt.hasClassName(this.name + "-edit-box")) { 
      container = elt;
    }
    else { 
      container = elt.up("." + this.name + "-box") || elt.up(".hbox").down("." + this.name + "-box");
      if (container.up().hasClassName("deck")) {
        container.up().raise(container);
      }
    }
    if (container.hasClassName("workspace-box")) { 
      container = container.down("." + this.name + "-edit-box");
    }
    
    args = this.getArgs(elt);
    if (elt.hasAttribute("data-id")) {
      args["id"] = elt.getAttribute("data-id") || 0;
    }

    Kwo.exec(this.root + "/" + this.name + ".edit", args,
             {"container": container});
    
  },

  onSearch: function(elt) {
    this.action = "search";
    elt = $(elt);
    var container = elt.up("DIV.finder-container");
    if (!container) {
      container = document.body;
    }
//    var args = [this.enhance(elt), {scroll: elt.up(".finder").down(".dtg-body").scrollTop}]
    var args = this.getArgs(elt, elt);
    Kwo.exec("/back/core/finder", args,
             {"container": container});
  },

  onHome: function(elt, opts) {
    elt = $(elt);
    opts = opts || {};
    this.action = "home";
 
    var args = this.getArgs(elt);
    
    var container = elt.hasClassName(this.name + "-box") ? elt : elt.up("." + this.name + "-box");
    if (!container) {
      container = elt.up(".hbox").down("." + this.name + "-box");
    }
    else if (container.hasClassName("workspace-box")) {
      container = container.down("." + this.name + "-home-box");
    } 
 

    if (elt.up().hasClassName("link") && container.hasClassName("bhv-updated")) { 
      if (container.up().hasClassName("deck")) {
        container.up().raise(container);
      }
      else {
        container.show();
      }
    }
    else {
      container.addClassName("bhv-updated");
      if (container.up().hasClassName("deck")) {
        container.up().raise(container);
      }
      Kwo.exec(this.root + "/" + this.name + ".home", args,
               {"container": container, 
                "callback": opts["callback"]});
    }

  },

  onSort: function(elt) {
    this.action = "sort";
    this.elt = $(elt);
    Kwo.exec(this.root + "/" + this.name + "." + this.action,
             {"resources": Sortable.sequence($(elt).id).join(",")},
             {"callback": this.onHome.bind(elt)});
  },


  onCallback: function (res) {
    var previous_action = this.action;
    if (Kwo.hasError(res)) {
      if (this.elt && this.elt.src && this.elt.getAttribute("data-src")) {
        this.elt.src = this.elt.getAttribute("data-src");
      }
      return Kwo.error(res);
    }
    this.action = "";
    if ("callback_message" in res["result"]) {
      Kwo.warn(res);
    }
    if ("callback_url" in res["result"]) {
      if (res["result"]["callback_url"] == "/") {
        window.top.location.href = "/";
      }
      else {
        Kwo.go(res["result"]["callback_url"]);
      }
      return ;
    }
    
    if ("id" in res["result"]) {
      var id = res["result"]["id"];
      
      if (previous_action == "store") {

        this.notify("enregistrement réalisé avec succès.");
        
        if (id >= 1) {
          this.elt.setAttribute("data-id", id); 
        }
        
        if (res["result"]["is_node"] == true) {
          Kwo.trees[this.name].onRefresh(id); 
          this.onEdit(this.elt);
          return ;
        }


/*        if ($(this.name + "-home-box")) {
          var args = {};
          args[this.name + "_id"] = id;
          this.onHome(args, {"callback": this.onEdit.curry(id).bind(this)});
        }
        else if (id >= 1) {
          this.onEdit(id);
        }*/

        if (id >= 1) {
          if (this.elt.up("." + this.name + "-box").hasClassName("workspace-box")) {
            this.onHome(this.elt, 
                        {"callback": this.onEdit.curry(this.elt).bind(this)});
          }
          else {
            this.onEdit(this.elt);
          }
        }


      }
      else {
        if (id >= 1) {
          this.onEdit(this.elt);
        }
      }
    }
  },

  onExec: function(action, elt, opts) {
    this.action = "exec";
    this.elt = $(elt);
    if (!Kwo.hasClickExpired(this.elt)) return ;
    var that = this;
    opts = opts || {}; 
    if ("callback" in opts) {
      if (opts["callback"] == true || opts["callback"] == "true") {
        opts["callback"] = this.onCallback.bind(this);
      }
    }
    else if (!("container" in opts)) {
      opts["callback"] = function (resp) {
        if ("callback_notification" in resp["result"]) {
          that.notify(resp["result"]["callback_notification"]);
        }
      };
    }
    if ("container" in opts && opts["container"]) {
      if (!Object.isElement(opts["container"]) &&
          (opts["container"] === "panel" || opts["container"] === "_blank")) {
        if (opts["container"] === "panel") {
          opts["container"] = null;
          opts["callback"] = function(res) {
            var panel = new Kwo.Class.CallbackPanel(elt, res);
          };
        }
        else if (opts["container"] === "_blank") {
          
        }
      }
      else { 
        var container = $(opts["container"]); 
        if (container.up().hasClassName("deck")) {
          container.up().raise(container);
        }
      }
    }
    if (action.indexOf("/") == -1) {
      action = this.name + "." + action;
      action = this.root + "/" + action;
    } 
    var args = opts["args"] || {};
    args = this.getArgs(elt, args);
    if (opts["container"] && opts["container"] == "_blank") {
      opts["container"] = null;
      Kwo.go(action, args, opts);
    }
    else {
      Kwo.exec(action, args, opts);
    }
  },

  onPreview: function(elt) {
    this.action = "preview";
    var m = new Kwo.Preview(elt);
  },

  onStore: function(elt) {
    this.action = "store";
    this.elt = $(elt);
    if (!Kwo.hasClickExpired(this.elt)) return ;
    var args = this.getArgs(this.elt, this.elt.up("FORM"));
    Kwo.exec(this.root + "/" + this.name + ".store", args,
             {"callback": this.onCallback.bind(this) });
  },

  getArgs: function(elt, args) { 
    args = args || {};
    if (Object.isElement(elt)) {
      args = Kwo.mergeArgs([elt, args]);
      args = args.toJSON();
    } 
    args = kwo.tool.Contextualize(elt, args); 
    if (!("id" in args) && args["branch[" + this.name + "_id]"] >= 1) {
      args["id"] = args["branch[" + this.name + "_id]"];
    }
    else if (!("id" in args) && args["branches[" + this.name + "_id]"] >= 1) {
      args["id"] = args["branches[" + this.name + "_id]"];
    }
    return args;
  }, 

/*  enhance: function(args, node) {
    var ctx = {};
    if (args === null || Object.isUndefined(args)) {
      args = {};
    }
    else if (Object.isElement(args) || Object.isElement(node)) {
      var start = node || args;
      $(start).ancestors().each(function (elt) {
        if (!elt.hasAttribute("data-branch-key")) return ;
        ctx[elt.getAttribute("data-branch-key")] = elt.getAttribute("data-branch-value");
      });
    }
//    else {
//      $$("[data-branch-key]").each(function (elt) {
//        ctx[elt.getAttribute("data-branch-key")] = elt.getAttribute("data-branch-value");
//      });
//    }

    args = Kwo.mergeArgs(args || {});

    for (var key in ctx) {
      args.set("context[" + key + "]", ctx[key]);
      args.set("branch[" + key + "]", ctx[key]);
    };

    if (args.get("id") === undefined && ctx[this.name + "_id"] >= 1) {
      args.set("id", ctx[this.name + "_id"]);
    }

//    args.each(function (pair) {
//      if (pair.key.include("[") > -1 && pair.key.endsWith("_id]")) {
//        if (pair.value >= 1) return ;
//        var parts = pair.key.split("[");
//        parts[0] = parts[0].substring(0, parts[0].length - 1);
//        if (ctx[parts[0]] >= 1) {
//          args[pair.key] = ctx[parts[0]];
//        }
//      }
//    });
    
    $$("[data-branch-key]").each(function (elt) { 
      if (!elt.visible()) return ;
      var key = "branches[" + elt.getAttribute("data-branch-key") + "]";
      var value = elt.getAttribute("data-branch-value");
      if (value && value != "0" && !args[key]) { 
        args.set(key, value);
      }
    });

    return args;
  },*/

/*  onRedirect: function(action, args, opts) {
    return this.go(action, args, opts);
  },*/

  go: function(action, args, opts) {
    this.action = "go";
    opts = opts || {};
    if (action.indexOf("/") == -1) {
      action = this.name + "." + action;
      action = this.root + "/" + action;
    }
    args = this.getArgs(args);
    Kwo.go(action, args, opts);
  },

  notify: function(msg, level) {
    if (msg.length < 1) return ;
    var div;
    if ($("notification-box")) {
      div = $("notification-box");
    }
    else {
      div = new Element("div", {"class": "notification",
                                "id": "notification-box"});
      document.body.appendChild(div);
    }
    div.update(msg.ucfirst());
    setTimeout(function () {
      emile(div.setStyle({"top": "-38px"}), "top:0px;", {"duration": 250, "after": function () {
        setTimeout(function () {
          emile(div.setStyle({"top": "0px"}), "top:-38px;", {"duration": 250});
        }, 1500);
      }});
    }, 50);
  }


});



document.observe("dom:loaded", function() {

  $(document.body).observe("drop", function(evt) {
//    alert("Uniquement sur les zones 'Drop Zone'");
    evt.stop();
    return false;
  });

  $(document.body).observe("dragenter", function(evt) {
    evt.stop();
    return false;
  });

  $(document.body).observe("dragleave", function(evt) {
    evt.stop();
    return false;
  });

  $(document.body).observe("dragover", function(evt) {
    evt.stop();
    return false;
  });

  $(document.body).observe("dragstart", function(evt) {
    var elt = evt.element();
    if (elt.hasAttribute("draggable")) return ;
    evt.stop();
    return false;
  });

/*  $(document.body).observe("click", function(evt) {
    console.log("click");
    evt.stop();
    return false;
  }); */

  if ($("menubar")) {
    $("menubar").observe("click", function (evt) {
      var elt = evt.element();
      if (elt.readAttribute("data-url")) {
        var menutitle = elt.up("LI");
        var extension = menutitle.readAttribute("data-extension");
        window.document.title = extension.ucfirst() + " - KWO";
        var url = elt.readAttribute("data-url");
//        url = url.startsWith("/back/") ? url : "/back/" + extension + "/" + url;
        window.top._main_iframe.location = url;
        if ($("menutitle-selected") != menutitle) {
          if ($("menutitle-selected")) {
            $("menutitle-selected").removeAttribute("id");
          }
          menutitle.id = "menutitle-selected";
        }
      }
      else if (elt.tagName == "A") {
        if (elt.hasClassName("icon-files")) {
          if (!window.files_popup || window.files_popup.closed) {
            window.files_popup = window.open("/back/file/dir",
                                             "_blank",
			                     "toolbar=no,location=no,directories=no,status=no,menubar=no," +
			                     "scrollbars=no,resizable=no,copyhistory=no,width=780,height=460,hotkeys=no");
            return true;
          }
          window.files_popup.focus();
        }
        else if (elt.hasClassName("icon-logout")) {
          if (!confirm("Quitter ?")) return ;
          window.top.location='/back/core/logout';
        }
      }

    })
  }
  else {
    Object.extend(Kwo.Class.Obj.prototype,
                  {scheme: _scope,
                   extension: _extension,
                   action: _action,
                   root: "/" + _scope + "/" + _extension});
  }
});

// voir http://www.prototypejs.org/learn/extensions






Element.Methods.raise = function(element, id) {
  if (id === undefined) {
    if ($(element).visible()) return ;
    $(element.parentNode).immediateDescendants().invoke("hide");
    $(element).show();
    return;
  }
  if ($(id).visible()) {
    return ;
  }
  $(element).immediateDescendants().invoke("hide");
  $(id).show();
};

Element.Methods.highlight = function(element, class_name) {
  if ($(id).hasClassName(class_name) == true) return ;
  $(element.parentNode).immediateDescendants().invoke("removeClassName", class_name);
  $(id).addClassName(class_name);
};

Element.Methods.reset = function(element) {
  $$("#" + element.id + " INPUT",
     "#" + element.id + " SELECT",
     "#" + element.id + " TEXTAREA").each(function(e) {
    if (e.tagName == "SELECT") {
      e.selectedIndex = 0;
    }
    else if (e.tagName == "TEXTAREA") {
      e.innerText = "";
    }
    else if (e.type != "hidden") {
      e.value = "";
    }
  });
}

Element.addMethods();





Kwo.Select = {

  onOptionAdd: function(res) {
    if (Kwo.hasError(res)) return Kwo.error(res);
    this.insert({top: "<option value=" + res["result"]["id"] + ">" + res["result"]["name"] + "</option>"});
    this.selectedIndex = 0;
  },

  onOptionPrompt: function(elt, model, from) {
    elt = $(elt);
    if (elt.value != -1) return ;
    var name = prompt("Nom du nouvel élément");
    if (name == null || name.blank()) return false;
    Kwo.exec('/back/core/item.create', {"model": model, "name": name, "from": from},
             {callback: Kwo.Select.onOptionAdd.bind(elt)});
  }

}

Kwo.Form = {

  populate: function (id) {
    //    val = $(id+'[0]').options[$(id+'[0]').selectedIndex].value;
    val = $F(id+'[0]');
    eval('var h = $H(top.'+id+'["'+val+'"])');
    $(id+'[1]').length = 0;
    h.each(function(item) {
      $(id+'[1]').options[$(id+'[1]').length] = new Option(item['value'],item['key']);
    });
  },

  populateSelect: function (id,data_json) {
    $(id).length = 0;
    if (data_json.substring(0, 1)=="[") {
      arr = data_json.evalJSON();
      for (i = 0; i < arr.length; i++) {
	$(id).options[$(id).length] = new Option(arr[i],arr[i]);
      }
    }
    else if (data_json.substring(0, 1)=="{") {
      hash = $H(data_json.evalJSON());
      hash.each(function(item) {
	$(id).options[$(id).length] = new Option(item['value'],item['key']);
      });
    }
    else alert('strange json data');
  },

  select_transfer: function (from,to) {
    selFrom = $(from);
    optsel = selFrom.options[selFrom.selectedIndex];
    if (optsel<0) return;
    selFrom.remove(selFrom.selectedIndex);
    selTo = $(to);
    selTo.options[selTo.options.length] = new Option(optsel.text,optsel.value,false,false);
  },

  select_add: function (id) {
    var selFrom = $(id+'_1');
    var selTo = $(id+'_2');
    var selToBis = $(id+'_3');
    if (selFrom.selectedIndex<0) return;
    optsel = selFrom.options[selFrom.selectedIndex];
    if (optsel<0) return;
    selFrom.remove(selFrom.selectedIndex);
    selTo.options[selTo.options.length] = new Option(optsel.text,optsel.value,false,false);
    selToBis.options[selToBis.options.length] = new Option(optsel.text,optsel.value,true,true);
  },

  select_remove: function (id) {
    var selFrom = $(id+'_2');
    var selFromBis = $(id+'_3');
    if (selFrom.selectedIndex<0) return;
    var selTo = $(id+'_1');
    var optsel = selFrom.options[selFrom.selectedIndex];
    if (optsel<0) return;
    selFrom.remove(selFrom.selectedIndex);
    for (var i=0;i<selFromBis.length;i++) {
      if (selFromBis.options[i].value == optsel.value) {
      selFromBis.remove(i);
      break;
      }
    }
    selTo.options[selTo.options.length] = new Option(optsel.text,optsel.value,false,false);
  },

  option_move: function (id,way) {
    var sel = $(id+'_2');
    if (sel.selectedIndex<0) return;
    if (sel.options.length<2) return;
    if (way=='up' && sel.selectedIndex==0) return;
    else if (way=='down' && sel.selectedIndex==(sel.length-1)) return;
    var opt = sel.options[sel.selectedIndex];
    var before = null;
    var before_index;
    if (way=='up') {
      before_index = sel.selectedIndex-1;
      before = sel.options[before_index];
      sel.remove(sel.selectedIndex);
      if (Kwo.isIE) sel.add(opt,before_index);
      else sel.add(opt,before);
    }
    else {
      if (sel.selectedIndex==(sel.length-2)) before = null;
      else {
	before_index = sel.selectedIndex;
	before = sel.options[before_index+2];
      }
      sel.remove(sel.selectedIndex);
      if (Kwo.isIE) {
	if (before==null) sel.add(opt);
	else sel.add(opt,before_index+1);
      }
      else sel.add(opt,before);
    }
    //    sel.selectedIndex = -1;
    if ($(id + "_3")) {
      $(id + "_3").options.length = 0;
      var opt_new;
      for (var i=0;i<sel.options.length;i++) {
	opt_new = new Option(sel.options[i].text,sel.options[i].value,true,true);
	if (Kwo.isIE) $(id + "_3").add(opt_new);
	else $(id + "_3").add(opt_new,null);
      }
    }
  }

};

Kwo.NoteBook = {

  onSwitchPage: function(elt) {
    elt = $(elt);
    elt.siblings().invoke("removeClassName", "active");
    elt.addClassName("active");
    var ctx = {"selected": null, 
               "className": "tabpanel-" + elt.getAttribute("data-num")};
    elt.up(".tabbox").down(".tabpanels").childElements().each(function (tabpanel) {
      tabpanel.hide();
      if (tabpanel.hasClassName(ctx["className"])) {
        ctx["selected"] = tabpanel;
      }
    }, ctx);
    ctx["selected"].show();
  }

};




Kwo.Class.FileManager = Class.create(Kwo.Dialog, {

  initialize: function($super, elt, opts) {
    this.name = "filemanager";
    this.className = "layout-hbox";
    this.elt = $(elt);
    this.width = 980;
    this.height = 480;
    this.opts = opts || {};
    this.panel = null;
    $super(this.onDisplay);
  },

 onDisplay: function() {
    Kwo.exec("/back/core/file.manager", {},
             {"container": this.support,
              "callback": this.onBind.bind(this)});
  },

  getPath: function() {
    return this.panel.getPath();
  },
 
  onBind: function() {
    var that = this; 

    this.details = this.support.down("TD.details");
    this.actions = this.support.down("TD.actions");
    this.dropzone = new Kwo.Class.DropZone(this.support.down(".dropzone"));

    var path = "";
    if (Object.isElement(this.elt)) {
      if (this.elt.match("img")) {
        path = this.elt.src.empty() ? "" : this.elt.src;
      }
      else if (this.elt.match("input")) {
        if (this.elt.value.empty()) {
          if ("path" in this.opts) {
            path = this.opts["path"];
          }
        }
        else { 
          path = this.elt.value.dirname();
        }
      }
    }

    this.panel = new Kwo.Class.FilePanel($("file-panel"),
                                         {"path": path,
                                          "filter": this.opts["filter"]});
    this.panel.onFileSelect = this.onFilePreview.bind(this);
    
    $("create-folder-button").observe("click", function (evt) {
      var dirname = prompt("Nom du dossier :");
      if (!dirname || dirname.length <= 1) return ;
      
      Kwo.exec("/back/core/folder.create",
               {"path": that.getPath(),
                "name": dirname},
               {"callback": function (resp) {
                 if (Kwo.hasError(resp)) return Kwo.error(resp);
                 that.panel.onDirSelect(resp["result"]["folder_path"]);
                 that.onClean();
               }});

    });

    this.dropzone.onRequestComplete = this.panel.onRefresh.bind(this.panel);
    this.dropzone.onBeforeUpload = function() {
      that.dropzone.setDestination(that.panel.getPath());
    };
    /*    this.dropzone.elt.observe("click", function(evt) {
      evt.stop();
      var manager = new Kwo.Class.UploadManager({"origin": that,
                                                 "destination": that.panel.path});
    });*/
    if (this.opts["filter"] == "image") {
      this.dropzone.input.accept = "image/*";
    }

    this.details.observe("click", function(evt) {
      var elt = evt.element();
      var action = elt.getAttribute("data-action");
      var filepath = elt.getAttribute("data-filepath");
      if (action == "select") {
        that.onSelect(filepath);
      }
      else if (action == "remove") {
        that.onRemove(elt);
      }
      else if (action == "download") {
        Kwo.go("/back/file/file.download", 
               {"filepath": elt.getAttribute("data-filepath")});
      }
    });

  },

  onRemove: function(elt) {
    var that = this;
    Kwo.exec("/back/core/file.remove", 
             {"filepath": elt.getAttribute("data-filepath")},
             {"confirm": elt,
              "callback": function(res) {
                that.onClean();
                that.panel.onRefresh();
              }});
  },
  
  onFilePreview: function(file) {
    this.onClean();
    Kwo.exec("/back/core/file.details", 
             {"filepath": file["path"]},
             {"container": this.details});
  },
  
  onClean: function() {
    this.details.update();
    this.actions.update();
  },
  
  onSelect: function(file_path) {
    if (Object.isElement(this.elt)) {
      if (this.elt.tagName == "IMG") {
        this.elt.src = "/" + file_path;
        var previous = this.elt.previous();
        if (!Object.isUndefined(previous) && previous.tagName == "INPUT") {
          previous.setValue("/" + file_path);
          previous.enable();
        }
      }
      else if (this.elt.tagName == "INPUT") {
        this.elt.setValue("/" + file_path);
      }
    }
    else {
      alert(file_path);
    }
    this.close();
  }
  
});

Kwo.Class.FilePanel = Class.create({
  
  container: null,
  opts: null,
  path: null,

  initialize: function(container, opts) {
    this.opts = opts || {};
    this.container = $(container);
    if ("style" in this.opts) {
      this.container.setStyle(this.opts["style"]);
    }
    if (this.opts["class"]) this.container.addClassName(opts["class"]);
    this.onDirSelect(this.opts["path"] || null);
  },

  onDirSelect: function(path) {
    this.path = path == null || path.indexOf(".") == -1 ? path : path.dirname();
    this.container.writeAttribute("data-path", path);
    Kwo.exec("/back/core/file.panel",
             {path: this.path,
              filter: this.opts["filter"]},
             {callback: this.onCallback.bind(this)});
  },

  onFileSelect: function(file) {
    alert(file["path"]);
  },

  onRefresh: function() {
    this.onDirSelect(this.getPath());
  },

  onCallback: function(res) {
    if (Kwo.hasError(res)) return Kwo.error(res);
    var that = this;
    var ul = new Element("UL", {"class": "file-panel"});
    this.container.update(ul);
    var html, elt, file, i, l = res["result"]["files"].length;
    var file;
    for (i = 0; i < l; ++i) {
      file = res["result"]["files"][i];
      elt = new Element("LI", {});
      elt.file = file;
      html = "";
      html += '<img src="/web/core/images/mime/' + file["ico"] +'" />';
      html += file["name"];
      elt.update(html);
      if (file["is_folder"] == 1) {
        elt.observe("click", this.onDirSelect.curry(file["path"]).bind(this));
      }
      else {
        elt.observe("click", function (evt) {
          evt.stop();
          var elt = Event.element(evt);
          elt.up().select("LI.selected").invoke("removeClassName", "selected");
          elt.addClassName("selected");
          that.onFileSelect(elt.file);
        });
      }
      ul.appendChild(elt);
    }
  },

  getPath: function() {
    return this.path;
  }

});


Kwo.Class.ImageManager = Class.create(Kwo.Dialog, {

  initialize: function($super, elt, opts) {
    this.name = "image";
    this.className = "layout-hbox";
    this.elt = $(elt);
    this.width = 900;
    this.height = 450;
    this.opts = opts || {};
    $super(this.onDisplay);
  },

  onDisplay: function() {
    Kwo.exec("/back/core/editor.image", {},
             {container: this.support,
              callback: this.onBind.bind(this)});
  },

  onBind: function() {
    var that = this;
    var panel = new Kwo.Class.FilePanel($("file-panel"),
                                        {path: "", filter: "image"});
    panel.onFileSelect = function(file) {
      $("image-preview").src = "/" + file["path"];
      $("image-path").value = "/" + file["path"];
      $("image-select-button").show();
    };
    $("image-select-button").observe("click", function (evt) {
      var args = {path: $("image-path").value,
                  alt: $("image-alt").value.replace('"', "'"),
                  width: $("image-width").value,
                  height: $("image-height").value,
                  float: $("image-float").value};
      that.onSelect(args);
    });
    this.dropzone = new Kwo.Class.DropZone(this.support.down(".dropzone"));
    this.dropzone.input.accept = "image/*";
    this.dropzone.onRequestComplete = panel.onRefresh.bind(panel);
    this.dropzone.onBeforeUpload = function() {
      that.dropzone.setDestination(panel.getPath());
    };
/*    this.dropzone.elt.observe("click", function(evt) {
      evt.stop();
      var manager = new Kwo.Class.UploadManager({origin: that,
                                                 destination: panel.getPath()});
    });*/
    $("create-folder-button").observe("click", function (evt) {
      var dirname = prompt("Nom du dossier :");
      if (!dirname || dirname.length <= 1) return ;
      Kwo.exec("/back/core/folder.create",
               {"path": panel.getPath(),
                "name": dirname},
               {"callback": function (resp) {
                 if (Kwo.hasError(resp)) return Kwo.error(resp);
                 panel.onDirSelect(resp["result"]["folder_path"]);
               }});

    });
  },

  onSelect: function(file_path) {
    alert(file_path);
    this.close();
  }

});

Kwo.Class.FolderManager = Class.create(Kwo.Dialog, {

  initialize: function($super, callback, opts) {
    this.name = "folder";
    this.width = 900;
    this.height = 400;
    this.opts = opts || {};
    this.callback = callback;
    $super(this.onDisplay);
  },

  onDisplay: function() {
    Kwo.exec("/back/core/folder.picker", {},
             {container: this.support,
              callback: this.onBind.bind(this)});
  },

  onBind: function() {
    var that = this;
    var panel = new Kwo.Class.FilePanel($("folder-panel"),
                                        {path: $F(this.callback)});
    panel.onDirSelect = function(file) {
      that.file = file;
      Kwo.exec("/back/core/file.panel",
               {"path": that.file, filter: "directory"},
               {"callback": that.onCallback.curry(panel).bind(that)});
    };
    panel.onFileSelect = Prototype.emptyFunction;
    $("apply-button").observe("click", function (evt) {
      that.onSelect({"path": "/" + that.file});
    });
    $("create-folder-button").observe("click", function (evt) {
      var dirname = prompt("Nom du dossier :");
      if (dirname && dirname.length > 0) {
        var args = {"path": that.file, "name": dirname};
        Kwo.exec("/back/core/folder.create", args, 
                 {"callback": function (resp) {
                   if (Kwo.hasError(resp)) return Kwo.error(resp);
                   panel.onDirSelect(args["path"] + "/" + args["name"]);
                 }});
      }
    });
    panel.onDirSelect($F(this.callback));
  },

  onCallback: function(panel, res) {
    panel.onCallback(res);
    Kwo.exec("/back/core/folder.details",
             {"path": this.file},
             {"container": $('folder-details'),
              "callback": Element.show.curry($("apply-button"))});
  },

  onSelect: function(file) {
    if (Object.isElement(this.callback)) {
      this.callback.setValue(file.path);
    }
    else {
      alert(file.path);
    }
    this.close();
  }

});

Kwo.Class.ItemPicker = Class.create(Kwo.Dialog, {

  initialize: function($super, elt, opts) {
    this.elt = $(elt);
    this.models = this.elt.getAttribute("data-models");
    this.name = "item-picker";
    this.className = "layout-hbox";
    this.width = 400;
    this.height = 500;
    this.opts = opts || {};
    this.application = ""; // editor, input, relation
    this.exclude = this.elt.getAttribute("data-exclude");
    this.item = this.elt.getAttribute("data-item");

    $super(this.onDisplay);
  },

  onDisplay: function(args) {
    args = args || {}
    args["models"] = this.models;
    args["application"] = this.application;
    args["exclude"] = this.exclude;
    args["item"] = this.item;
    args = kwo.tool.Contextualize(this.elt, args);
    Kwo.exec("/back/core/item.picker", args,
             {"container": this.support,
              "callback": this.onBind.bind(this)});
  },

  onBind: function() {
    var that = this;
    var form = this.support.down("FORM");
    form.observe("submit", function(evt) {
      evt.stop();
      that.onSearch();
    });
    this.support.down("SELECT").observe("change", function(evt) {
      evt.stop();
      that.onSearch();
    });
    if (this.support.down("UL")) {
      this.support.down("UL").observe("click", function(evt) {
        evt.stop();
        var elt = evt.element();
        if (elt.tagName == "LI") {
          var args = {"model_id": elt.getAttribute("data-model-id"),
                      "record_id": elt.getAttribute("data-id")};
          Kwo.exec("/back/core/item.pick", args,
                   {"callback": function(resp) {
                     if (Kwo.hasError(resp)) return alert("item error");
                     that.onSelect(resp["result"]["item"]);
                   }});
        }
      });
    }
    if (this.support.down(".pagination")) {
      this.support.down(".pagination").observe("click", function(evt) {
        evt.stop();
        var elt = evt.element();
        if (elt.tagName == "A") {
          that.support.down("INPUT[name=offset]").value = elt.getAttribute("data-offset");
          that.onSearch();
        }
      });
    }
    this.support.down(".item-search-button").observe("click", function(evt) {
      evt.stop();
      that.onSearch();
    });
    if (this.support.down(".item-editor-button")) {
      this.support.down(".item-editor-button").observe("click", function(evt) {
        evt.stop();
        this.setAttribute("data-model", that.getModel());
        var editor = new Kwo.Class.ItemEditor(this);
        var item = {};
        editor.onBeforeClose = function() {
          var datas = this.support.down("DIV.data-item");
          var args = {"model_id": datas.getAttribute("data-model-id"),
                      "record_id": datas.getAttribute("data-id")};
          if (args["record_id"].intval() < 1) return ;
          Kwo.exec("/back/core/item.pick", args,
                   {"callback": function(resp) {
                     if (Kwo.hasError(resp)) return alert("item error");
                     that.onSelect(resp["result"]["item"]);
                   }});
        };
        editor.onAfterClose = function() { 
          if ("record_id" in item && item["record_id"].intval() >= 1) {
            that.onSelect(item);
          }
        };
      });
    }
  },

  getModel: function() {
    return this.support.down("SELECT").getValue();
  },

  onSearch: function() {
    var args = {};
    args["query"] = this.support.down("INPUT[name=query]").getValue();
    args["model"] = this.support.down("SELECT").getValue();
    args["offset"] = this.support.down("INPUT[name=offset]").getValue();
    this.onDisplay(args);
  },

  onSelect: function(item) {  
    if (this.elt.hasClassName("item-picker-button")) {
      this.elt.up("DIV").down("INPUT.input-record-id").value = item["record_id"];
      var name = item["name"];
      if (this.elt.up("DIV").down("INPUT.input-model-id")) {
        this.elt.up("DIV").down("INPUT.input-model-id").value = item["model_id"];
        name = item["model"] + ": " + name;
      }
      this.elt.up("DIV").down("INPUT[type=text]").value = name;
      this.close();
    }
  }

});

Kwo.Class.ItemEditor = Class.create(Kwo.Dialog, {

  initialize: function($super, elt) {
    this.elt = $(elt);
    this.name = "item-editor";
    this.args = {};
    this.width = "94%";
    this.height = 620;
    this.className = "layout-hbox";
    $super(this.onDisplay);
  },

  onDisplay: function() {
    var url = "/back/core/item.edit";
    var model = this.elt.getAttribute("data-model");
    url = "/back/core/" + model + ".edit";
    var args = {};
    if (this.elt.hasAttribute("data-record-id")) {
      args["id"] = this.elt.getAttribute("data-record-id");
    }
    this.support.update('<h1>Edition</h1>' + 
                        '<div style="padding:6px 10px; margin:0;"' + 
                        '     class="' + model + '-box" >' +
                        '</div>');
    Kwo.exec(url, args, {"container": this.support.down("DIV")})
  }

});

Kwo.Class.UploadManager = Class.create(Kwo.Dialog, {

  initialize: function($super, opts) {
    this.name = "upload.manager";
    this.className = "layout-hbox";
    this.width = 400;
    this.height = 200;
    this.opts = opts || {};
    this.form = null;
    this.iframe = null;
    $super(this.onDisplay);
  },

  onDisplay: function() {
    Kwo.exec("/back/core/file.chooser", {},
             {container: this.support,
              callback: this.onBind.bind(this)});
  },

  onBind: function() {
    this.iframe = this.support.down("IFRAME");
    this.form = this.support.down("FORM");
    this.support.down("INPUT[type=hidden]").value = this.opts["destination"] || "";
    this.support.down("INPUT[type=button]").observe("click", this.onSubmit.bind(this));
  },

  onSubmit: function(evt) {
    if (this.form.down("INPUT[type=file]").value.blank()) {
      return ;
    }
    this.form.submit();
  },

  onUploadCompleted: function() {
    if (this.opts["origin"] && this.opts["origin"].panel) {
      this.opts["origin"].panel.onRefresh();
    }
    this.form.reset();
    alert("Transfert effectué avec succès.");
  }

});



Kwo.USR = Class.create(Kwo.Dialog, {

  initialize: function($super, elt) {
    elt = $(elt);
    this.name = "usr";
    this.args = {id: elt.readAttribute("data-id")};
    this.width = 800;
    this.height = 520;
    this.className = "layout-hbox";
    $super(this.onDisplay);
  },

  onDisplay: function() {
    Kwo.exec("/back/social/user.summary", this.args,
             {container: this.support});
  }

});



Kwo.Class.BitmapEditor = Class.create(Kwo.Dialog, {

  initialize: function($super, elt) {
    this.elt = $(elt);
    this.name = "bitmapeditor";
    this.args = {};
//    this.file_path = $(elt).readAttribute("data-file-path");
    this.file_path = this.elt.src;
    this.width = "90%";
    this.height = 580;
    this.className = "layout-hbox";
    $super(this.onDisplay);
  },

  onDisplay: function() {
    this.support.update('<h1>Editeur d’image</h1><div style="padding:0; margin:0;"><iframe src="javascript:;" style="width:100%;"></iframe></div>');
    this.support.down("IFRAME").src = "/back/core/bitmap.editor?file_path=" + this.file_path;
    this.support.down("IFRAME").style.height = (this.height - 30) + "px";
  },

  onAfterClose: function() {
    this.elt.src = this.file_path + "?t=" + Math.random();
  }

});


Kwo.Preview = Class.create(Kwo.Dialog, {

  initialize: function($super, elt) {
    elt = $(elt);
    this.name = "usr";
    this.args = {};
    if ($(elt).readAttribute("data-item")) {
      this.args["item_key"] = $(elt).readAttribute("data-item");
    }
    else {
      this.args["url"] = $(elt).readAttribute("data-url");
    }
    this.width = 1100;
    this.height = 580;
    this.className = "layout-hbox";
    $super(this.onDisplay);
  },

  onDisplay: function() {
    Kwo.exec("/back/core/item.preview", this.args,
             {container: this.support,
              callback: this.onLoad.bind(this)});
  },

  onLoad: function() {
    var iframe = this.support.down("IFRAME");
    iframe.src = iframe.readAttribute("data-url");
  }

});

Kwo.CMT = {

  onRemove: function(elt) {
    elt = $(elt);
    Kwo.F("comment").onExec("destroy", {"id": elt.getAttribute("data-id")},
                            {"confirm": elt,
                             "callback": $("comments-button").onclick.bind("comments-button")});
  }

};

Kwo.ACL = {

  onAdd: function(elt) {
    elt = $(elt);
    var model = elt.up("FIELDSET").readAttribute("data-model");
    Kwo.F(model).onExec("acl.add", elt,
                        {callback: $(model + "-acl-button").onclick.bind($(model + "-acl-button"))})
  },

  onRemove: function(elt) {
    elt = $(elt);
    var model = elt.up("FIELDSET").readAttribute("data-model");
    Kwo.F(model).onExec("acl.remove", {user_id: elt.readAttribute("data-id")},
                        {confirm: elt,
                         callback: $(model + "-acl-button").onclick.bind($(model + "-acl-button"))})
  },

  onUserRemove: function(elt) {
    elt = $(elt);
    Kwo.F("user").onExec("acl.remove", {item_key: elt.readAttribute("data-item")},
                         {confirm: elt,
                          callback: $("acl-button").onclick.bind($("acl-button"))})
  }

};

Kwo.Visit = {

  onDisplay: function(elt) {
    elt = $(elt);
    var id = elt.readAttribute("data-id");
    var target = elt.readAttribute("data-target");
    if (elt.up(".dashboard-precision")) {
      if (elt.hasClassName("visit-sibling")) {
        window.open("/back/tracker/visit?id=" + id,
                    "_blank",
                    "toolbar=no,location=yes,directories=no,status=no,menubar=no,scrollbars=no,resizable=no,copyhistory=no,width=900,height=480,hotkeys=no");
      }
      else {
        Kwo.exec("/back/tracker/visit", {id: id},
                 {container: elt.up(".dashboard-precision").down("DIV.wrapper")});
      }
    }
    else {
      if (window["_action"] == "visit") {
        Kwo.go("/back/tracker/visit?id=" + id);
      }
      else {
        window.open("/back/tracker/visit?id=" + id,
                    "_blank",
                    "toolbar=no,location=yes,directories=no,status=no,menubar=no,scrollbars=no,resizable=no,copyhistory=no,width=900,height=480,hotkeys=no");
      }
    }
  },

  displaySiblings: function(elt) {
    elt = $(elt);
    var dest = elt.up(".visit").down(".visit-path").hide().next().show();
    if (dest.hasClassName("kwo-completed")) {
      return ;
    }
    Kwo.exec("/back/tracker/visit.siblings", {id: elt.readAttribute("data-id")},
             {container: dest});
    dest.addClassName("kwo-completed");
  },

  displayPath: function(elt) {
    elt = $(elt);
    elt.up(".visit").down(".visit-siblings").hide().previous().show();
  },

};

Kwo.Dashboard = {

  onPeriodChange: function(elt) {
    $("dashboard-overview").update('<div class="wrapper"></div>');
    $("dashboard-precision").update('<div class="wrapper"></div>');
    elt = $(elt);
    var args = {};
    args["year"] = $("year").getValue();
    if (elt.id == "month") {
      args["month"] = $("month").getValue();
    }
    if (args["year"].intval() < 1) {
      elt.up(".period").down("TABLE.month").hide().next().hide();
      return ;
    }
    Kwo.exec("/back/core/period.picker", args,
             {container: $("dashboard-period")});
  },

  onPeriodSelect: function(elt) {
    var root;
    $("dashboard-overview").update('<div class="wrapper"></div>');
    $("dashboard-precision").update('<div class="wrapper"></div>');
    elt.up(".period").select(".period-row-selected").invoke("removeClassName",
                                                            "period-row-selected");
    elt = $(elt);
    var args = {year:0, month:0, day:0};
    if (elt.tagName == "A") { 
      elt.up("TABLE").select("A.selected").invoke("removeClassName", "selected");
      elt.addClassName("selected");
      args["day"] = elt.readAttribute("data-num");
      args["month"] = $("month").getValue();
      args["year"] = $("year").getValue();
      elt.up(".period").select(".period-row").invoke("addClassName",
                                                     "period-row-selected");
    }
    else {
      root = elt.up(".period");
      args["year"] = $("year").getValue();
      root.down(".period-year").addClassName("period-row-selected");
      if (elt.up("TABLE").hasClassName("month")) {
        args["month"] = $("month").getValue();
        root.down(".period-month").addClassName("period-row-selected");
      }
      elt.up(".period").select("A.selected").invoke("removeClassName", "selected");
    }
    args["period"] = args["year"] + "-" + args["month"] + "-" + args["day"];
    elt.up(".dashboard").writeAttribute("data-period", args["period"]);
    Kwo.exec("/back/" + window["_extension"] + "/dashboard.overview", args,
             {container: $("dashboard-overview").down("DIV.wrapper")});
  },

  onRefine: function(elt) {
    elt = $(elt);
    var action = "/back/" + window["_extension"] + "/" + elt.getAttribute("data-action");
    var args = {"period": elt.up(".dashboard").getAttribute("data-period")};
    var s = elt.getAttribute("data-values");
    if (s != null && s.length > 1) {
      args = [args, s.evalJSON()];
    }
    Kwo.exec(action, args, {"container": elt.up(".dashboard-overview").next().down("DIV.wrapper")});
  }

};


Kwo.Developer = {

  displayScheduler: function(elt) {
    Kwo.exec("/back/core/developer.scheduler", {},
             {"container": $("developer-scheduler")});
  },

  displaySchedulerLog: function(elt) {
    elt = $(elt);
    Kwo.exec("/back/core/developer.scheduler.log",
             {"filename": elt.getAttribute("data-filename")},
             {"container": elt.up("TD").next()});
  },

  displayLogs: function(elt) {
    Kwo.exec("/back/core/developer.logs", {},
             {"container": $("developer-scheduler"),
              "callback": function () {
                $("ta-code").scrollTop = $("ta-code").scrollHeight;
                $("ta-url").scrollTop = $("ta-url").scrollHeight;
              }});
  },

  resetLogs: function(elt) {
    Kwo.exec("/back/core/developer.logs.reset", {},
             {"confirm": "êtes vous sûr ?"});
  },

  downloadLogs: function(elt) {
    Kwo.go("/back/core/developer.logs.download", {},
           {"confirm": "êtes vous sûr ?"});
  },


  displayModels: function(elt) {
    Kwo.exec("/back/core/developer.models", {},
             {"container": $("developer-models")});
  },

  displayTables: function(elt) {
    Kwo.exec("/back/core/developer.tables", {},
             {"container": $("developer-tables")});
  },

  processes: function(elt) {
    Kwo.exec("/back/core/developer.db.processes", {},
             {"container": $("developer-db-processes")});
  },

  displayServer: function(elt) {
    Kwo.exec("/back/core/developer.server", {},
             {"container": $("developer-server")});
  },

  buildModels: function(elt) {
    Kwo.exec("/back/core/developer.models.build", {},
             {"confirm": "êtes vous sûr ?\n(Opération dangereuse)",
              "callback": function (res) {
                if (Kwo.hasError(resp)) return Kwo.error(resp);
                alert("Build ... done.");
              }})
  },

  cacheClean: function(elt) {
    Kwo.exec("/back/core/developer.cache.clean", {},
             {"confirm": "êtes vous sûr ?",
              "callback": function () { alert("Cache ... cleaned."); }})
  },

  phpinfos: function(elt) {
    elt.writeAttribute("data-url", "/back/core/sys.infos");
    new Kwo.Preview(elt);
  },

  onShellExec: function(elt) {
    elt = $(elt);
    $("shell_output").update("");
    Kwo.exec("/back/core/shell.exec", elt.up("FORM"),
             {"callback": function(res) {
               $("shell_output").update(res["result"]["output"]);
             }});
  }

};

Kwo.Finder = {

  onSwitchPage: function(elt) {
    elt = $(elt);
    var finder = elt.up(".finder");
    var model = finder.getAttribute("data-model");
    var num = elt.down("INPUT[type=text]").value.intval();
    if (num < 1) {
      num = 1;
    }
    else if (num > finder.getAttribute("data-page-count").intval()) {
      num = finder.getAttribute("data-page-count").intval();
    }
    finder.down("INPUT[name=offset]").value = num - 1;
    Kwo.F(model).onSearch(finder.down('FORM'));
  },

  onExec: function(elt) {
    elt = $(elt);
    var finder = elt.up(".finder");
    var model = finder.getAttribute("data-model");
    var args = {};
    var opts = {};
    var action = elt.getAttribute("data-action");
    if (action == "attribute.store") {
      args = elt.up(".dtg-datas").down(".dtg-body").down("FORM");
    }
    else {
      args["id[]"] = [];
      elt.up(".dtg-datas").select("TR.checked").each(function (e) {
        args["id[]"].push(e.getAttribute("data-id"));
      });
      var msg = args["id[]"].length + " élément(s) sélectionné(s)" + "\n";
      opts["confirm"] = msg + elt.getAttribute("data-confirm").ucfirst();
    }

    opts["callback"] = function(res) {
      Kwo.F(model).onHome(elt);
      Kwo.F(model).notify(res["result"]["callback_message"]);
    };
    Kwo.exec("/back/core/" + model + "." + action, args, opts);
  },

  onExport: function(elt) {
    elt = $(elt);
    var finder = elt.up(".finder");
    Kwo.go("/back/core/export", finder.down("FORM"),
           {"confirm": "êtes vous sûr de vouloir procéder à l’export ?"});
  },

  onNext: function(elt) {
    elt = $(elt);
    var finder = elt.up(".finder");
    var model = finder.getAttribute("data-model");
    finder.down("INPUT[name=offset]").value = finder.down("INPUT[name=offset]").value.intval() + 1;
    Kwo.F(model).onSearch(finder.down("FORM"));
  },

  onPrev: function(elt) {
    elt = $(elt);
    var finder = elt.up(".finder");
    var model = finder.getAttribute("data-model");
    finder.down("INPUT[name=offset]").value = finder.down("INPUT[name=offset]").value.intval() - 1;
    Kwo.F(model).onSearch(finder.down("FORM"));
  },

  onSearch: function(elt) {
    elt = $(elt);
    var finder = elt.up(".finder");
    var model = finder.readAttribute("data-model");
    Kwo.F(model).onSearch(finder.down("FORM"));
  },

  onReset: function(elt) {
    elt = $(elt);
    var finder = elt.up(".finder");
    var model = finder.readAttribute("data-model");
    finder.down('INPUT[name=reset]').value = 1;
    Kwo.F(model).onSearch(finder.down("FORM"));
  },

  onClick: function(evt) {
    var elt = Object.isElement(evt) ? evt : evt.element();
    if (elt.hasClassName("inline-input") || elt.tagName == "OPTION") return ;
    if (elt.tagName == "INPUT") {
//      if (elt.getAttribute("type") != "checkbox") return ;
      if (elt.hasClassName("checkbox-controller")) {
        var table = elt.up("DIV").next("DIV").down("TABLE");
//        var n = table.select("TR[class=checked]").size();
        if (elt.checked == false) {
          elt.up(".dtg-datas").down(".actions").down("SPAN").hide();
          table.select("TR").each(function (e) {
            e.removeClassName("checked");
            e.down("INPUT[type=checkbox]").checked = false;
          });
        }
        else {
          elt.up(".dtg-datas").down(".actions").down("SPAN").show();
          table.select("TR").each(function (e) {
            e.addClassName("checked");
            e.down("INPUT[type=checkbox]").checked = true;
          });
        }
      }
      else {
        elt.up("TR").toggleClassName("checked");
        var n = elt.up("TABLE").select("TR[class=checked]").size();
        if (n >= 1) {
          elt.up(".dtg-datas").down(".actions").down("SPAN").show();
        }
        else {
          elt.up(".dtg-datas").down(".actions").down("SPAN").hide();
        }
      }
    }
    else if (elt.tagName == "TD" && elt.hasClassName("checkbox")) {
      return ;
    }
    else if (elt.tagName == "TD" && elt.down(".inline-editor")) {
      Kwo.Finder.onClick(elt.down(".inline-editor"));
    }
    else if (elt.tagName == "SPAN" && elt.hasClassName("inline-editor")) {
      if ("stop" in evt) evt.stop();
      var finder = elt.up(".finder");
      Kwo.exec("/back/core/" + finder.readAttribute("data-model") + ".attribute.edit",
               {"id": elt.readAttribute("data-id"),
                "attribute": elt.getAttribute("data-attribute")},
               {"container": elt.up()});
      elt.remove();
//      elt.update(elt.readAttribute("data-id"));
      finder.down(".button-save").show();
      return ;
    }
    else if (elt.tagName == "A") {
      return ;
    }
    else {
//      console.log(elt.tagName);
      var finder = elt.up(".finder");
      var model = finder.readAttribute("data-model");
      //      window["_scrolls"] = window["_scrolls"] || {}
      //      window["_scrolls"][model] = finder.down(".dtg-body").scrollTop;
      Kwo.F(model).onEdit(elt.up("TR"));
      evt.stop();
    }
  },

  onLoad: function(elt) {

  }

};


Kwo.Class.DropZone =  Class.create({

//  accept: "image/*",
  accept: "",
  destination: "",
  elt: null,
  filters: "",
  files: [],
  label: null,
  pending: 0,
  input: 0,

  initialize: function(elt) {
    this.elt = $(elt);

    var that = this;

    this.elt.observe("dragenter", function(evt) {
      evt.stop();
      that.elt.setAttribute("dragenter", true);
    });

    this.elt.observe("dragleave", function(evt) {
      evt.stop();
      that.elt.removeAttribute("dragenter");
    });

    this.elt.observe("dragover", function(evt) {
      evt.stop();
    });

    this.elt.observe("drop", this.onDrop.bind(this));

//https://developer.mozilla.org/en/using_files_from_web_applications#Using_hidden_file_input_elements_using_the_click%28%29_method

    if (Prototype.Browser.WebKit) {
      this.elt.update("<span>drop zone</span><input type='file' multiple='true' style='opacity:0; float:left; width:0; height:0;' />");
    }
    else {
      this.elt.update("<span>drop zone</span><input type='file' multiple='true' style='display:none;' />");
    }
     
    this.elt.setStyle({"cursor": "pointer"});
    
    var input = this.elt.down("INPUT");
    input.accept = that.accept;
    input.addEventListener("click", function(evt) {  
      evt.stopPropagation();
    }, false);
    input.addEventListener("change", function(evt) { 
      var files = this.files;
      /*          for (var i = 0; i < files.length; i++) {
            console.log(files.item(i));
          }*/
      that.onFilePush(files);
    }, false);
    
    this.elt.addEventListener("click", function(evt) { 
      evt.stopPropagation();
      evt.preventDefault(); 
      input.click();
    }, false);
    
    this.label = this.elt.down("SPAN");

    this.input = input;

  },

  onFilePush: function(files) {
    this.pending++;
    this.onBeforeUpload();
    for (var i = 0; i < files.length; i++) {
      this.files.push(files.item(i));
//      this.elt.update("transfering " + this.files.length);
    }
    this.label.update("transfering " + this.files.length);
    if (this.pending > 1) return ;
    this.onUpload();
    this.pending--;
  },

  onDrop: function(evt) {
    evt.stop();
    var files = evt.dataTransfer.files;
    this.onFilePush(files);
/*    this.pending++;
    for (var i = 0; i < files.length; i++) {
      this.files.push(files.item(i));
//      this.elt.update("transfering " + this.files.length);
    }
    this.elt.update("transfering " + this.files.length);
//    console.log(this.files);
    if (this.pending > 1) return ;
    this.onUpload();
    this.pending--;*/
  },

  onBeforeUpload: function() {},

  onUpload: function() {
    var file = this.files[0];
    var xhr = new XMLHttpRequest();
    var script = "/back/core/file.upload";

    if (this.elt.readAttribute("data-script")) {
      script = this.elt.readAttribute("data-script");
    }

    var that = this;

    xhr.upload.addEventListener("progress", this.onUploadProgress.bind(this), false);
    xhr.upload.addEventListener("load", this.onUploadComplete, false);
    xhr.upload.addEventListener("error", this.onUploadFailed, false);
    xhr.upload.addEventListener("abort", this.onUploadCanceled, false);

    xhr.open("post", script, true);

    xhr.onreadystatechange = function() {
      if (this.readyState != 4) {
        that.files.shift();
        if (that.files.length < 1) {

/* PB NON MISE à JOUR de l input*/

          that.label.update("drop zone");
        }
        else {
//          that.elt.update("transfering " + that.files.length);
          that.onUpload();
//          var t = setTimeout(function () { that.onUpload(); }, 3000);
        }
        that.onRequestComplete();
      }
    };

    xhr.overrideMimeType("text/plain; charset=x-user-defined-binary");

    xhr.setRequestHeader("Cache-Control", "no-cache");
    xhr.setRequestHeader("X-Request-With", "XMLHttpRequest");
    xhr.setRequestHeader("X-Prototype-Version", Prototype.Version);
    xhr.setRequestHeader("X-File-Destination", this.destination);
    xhr.setRequestHeader("X-File-Name", file.fileName);
    xhr.setRequestHeader("X-File-Size", file.fileSize);
    xhr.setRequestHeader("X-File-Type", file.type);
    if (this.elt.up("[data-item]")) {
      xhr.setRequestHeader("X-Item",
                           this.elt.up("[data-item]").getAttribute("data-item"));
    }

//    this.elt.update("transfering " + this.files.length);
//    console.log(file);
    if ("getAsBinary" in file) {
      xhr.sendAsBinary(file.getAsBinary());
    }
    else {
      xhr.send(file);
    }
  },

  onUploadProgress: function(evt) {
/*
 http://hacks.mozilla.org/2009/06/xhr-progress-and-richer-file-uploading-feedback/
 http://hacks.mozilla.org/2009/12/uploading-files-with-xmlhttprequest/
 http://hacks.mozilla.org/2010/02/an-html5-offline-image-editor-and-uploader-application/
*/

//    console.log("evt");
    this.label.update("transfering " + this.files.length);

    return ;
    var percentage;
    if (!evt.lengthComputable) return ;
    percentage = Math.round((evt.loaded * 100) / evt.total);
    if (percentage < 100) {
      evt.target.log.firstChild.nextSibling.firstChild.style.width = (percentage * 2) + "px";
      evt.target.log.firstChild.nextSibling.firstChild.textContent = percentage + "%";
    }
  },

  onRequestComplete: function(evt) {},

  onUploadComplete: function(evt) {},

  onUploadFailed: function(error) { console.log("error: " + error); },

  onUploadCanceled: function(error) { console.log("error: " + error); },

  setDestination: function(path) {
    this.destination = path;
  }

});

Kwo.Class.CallbackPanel = Class.create(Kwo.Dialog, {

  initialize: function($super, elt, resp) {
    this.name = "callback-panel";
    this.className = "layout-hbox";
    this.elt = $(elt);
    this.resp = resp;
    this.width = 500;
    this.height = 480;
    $super(this.onDisplay); 
  },

  onDisplay: function() {
    Kwo.exec("/back/core/callback.panel", {},
             {"container": this.support,
              "callback": this.onUpdate.bind(this)});
  },

  onUpdate: function() {
    var html = ".";
    if (Kwo.hasError(this.resp)) {
      html = "<p class='error'>ERROR</p>" + this.resp["result"]["msg"].join("<br/>");
    }
    else if ("callback_message" in this.resp["result"]) {
      html = this.resp["result"]["callback_message"];
    }
    this.support.down("DIV").update(html);
  }

});

var kwo = {"ux": {}, "ui": {}, "manager": {}, "tool": {}};




kwo.manager.Column = Class.create({

  "edited": null,
  "elt": null,
  "node_model": "",
  "tree_id": 0,
  "tree_model": "",
  "tabpanel": "",

  initialize: function(elt) {
    this.elt = $(elt).previous();
    this.tabpanel = this.elt.up(".workspace-box");
    this.tabpanel.setAttribute("data-branch-key", "parent_path");
    this.node_model = this.elt.getAttribute("data-node-model");
    this.tree_model = this.elt.getAttribute("data-tree-model");
    this.tree_id = this.elt.getAttribute("data-tree-id");
    var n = this.elt.getAttribute("data-column-count");
    this.elt.setStyle({"width": (n * 150) + "px"});
    Kwo.trees[this.node_model] = this;
    this.elt.select(".column UL").each(this.bindColumn.bind(this));
    this.elt.observe("click", this.onClick.bind(this));
  },

  onClick: function (evt) {
    var that = this;
    evt.stop();
    var elt = evt.element();
    if (elt.hasClassName("node-move")) {
      return ;
    }
    else if (elt.hasClassName("node-edit")) {
      this.setBranch(elt);
      var li = elt.up("LI");
      Kwo.F(that.node_model).onEdit(li);
      if (that["edited"]) {
        that["edited"].removeClassName("edited");
      }
      that["edited"] = li;
      this.elt.select("IMG.node-add-selected").invoke("removeClassName", "node-add-selected");
      li.addClassName("edited");
    }
    else if (elt.hasClassName("node-name")) {
      this.setBranch(elt);
      that.onRefresh();
    }
    else if (elt.hasClassName("node-add")) {
      this.setBranch(elt);
      this.elt.select("IMG.node-add-selected").invoke("removeClassName", "node-add-selected");
      if (that["edited"]) {
        that["edited"].removeClassName("edited");
      }
      elt.addClassName("node-add-selected");
      Kwo.F(that.node_model).onEdit(elt);
    }
  },

  setBranch: function(elt) {
    this.tabpanel.setAttribute("data-branch-value",
                               elt.up(".column").getAttribute("data-path"));
    if (elt.hasAttribute("data-id")) {
      this.elt.setAttribute("data-branch-value", 
                            elt.getAttribute("data-id")); 
    }
    else {
      this.elt.setAttribute("data-branch-value", 
                            elt.up("LI").getAttribute("data-id")); 
    }
  },

  bindColumn: function (elt) {
    Sortable.create(elt.identify(), 
                    {"tag": "li", 
                     "scroll": elt, 
                     "handle": "node-move",
                     "onUpdate": this.onSort.bind(this)});
  },

  onSort: function (elt) {
    var args = kwo.tool.Contextualize(elt);
    args["elements"] = Sortable.sequence($(elt).identify()).join(",");
    Kwo.exec("/back/core/" + this.node_model + ".sort", args,
             {"callback": this.onRefresh.bind(this)});

  },

  onRefresh: function (node_id) {
/*    var args = {"node_id": node_id || 0,
                "id": this.tree_id};*/
//    console.log(this.elt);
    if (node_id) {
      this.elt.setAttribute("data-branch-value", node_id);
    }
    Kwo.F(this.tree_model).onExec("columnview",
                                  this.elt,
                                  {"container": this.elt.up("." + this.node_model + "-home-box")});
  }

});

kwo.manager.Field = Class.create({

  editbox: null,
  elt: null,
  homebox: null,
  list: null,

  initialize: function(elt) {
    this.elt = $(elt).up(".field-box");
    this.homebox = this.elt.down(".field-home-box");
    this.editbox = this.elt.down(".field-edit-box");

    var elts = document.querySelectorAll(".field-types SPAN");
    var elt = null;
    for (var i = 0; i < elts.length; i++) {
      elt = elts[i];
      elt.setAttribute("draggable", "true");
      elt.addEventListener("dragstart", function (evt) {
        evt.dataTransfer.effectAllowed = "copy";
        evt.dataTransfer.setData("text/plain", this.getAttribute("data-field-type"));
      }, false);
    }

    this.onListDisplay();

  },

  onListDisplay: function() {

    this.list = this.homebox.down(".field-list UL");
//    this.list.identify();

    var elt;
    

    var that = this;

    this.list.observe("click", function (evt) {
      evt.stop();
      var elt = evt.element();
      if (elt.tagName == "A") {
        Kwo.F("field").onEdit(elt.up("LI"));
      }
    }, false);

    var elts = document.querySelectorAll(".field-list UL *");

    for (var i = 0; i < elts.length; i++) {
      elt = elts[i];

      elt.observe("dragover", function (evt) {
        evt.stop();
        evt.dataTransfer.dropEffect = "copy";
        var elt = evt.findElement("LI");
        if (!elt.hasAttribute("dragenter")) {
          elt.setAttribute("dragenter", true);
        }
        return false;
      });

/*      elt.observe("dragenter", function (evt) {
        evt.stop();
        var elt = evt.findElement("LI");
        console.log(elt);
        evt.findElement("LI").setAttribute("dragenter", true);
      }); */

      elt.observe("dragleave", function (evt) {
        evt.stop();
        evt.findElement("LI").removeAttribute("dragenter");
      });

      elt.observe("drop", function (evt) { 
        evt.stop();
        var args = kwo.tool.Contextualize(that.elt, {});
        args["position"] = evt.findElement("LI").getAttribute("data-position");
        args["type"] = evt.dataTransfer.getData("text/plain");
        Kwo.exec("/back/form/field.create", args,
                 {"callback": that.onRefresh.bind(that)});
        return false;
      });

    }

    Sortable.create(this.list.identify(),
                    {"handle": "handle",
                     "scroll": this.homebox,
                     "onUpdate": this.onSort.bind(this)});
  },

  onRefresh: function() {
    this.editbox.update();
    var args = kwo.tool.Contextualize(this.elt); 
    Kwo.exec("/back/form/field.home", args,
             {"container": this.homebox,
              "callback": this.onListDisplay.bind(this)});
  },

  onSort: function(elt) {
    var args = kwo.tool.Contextualize(elt);
    args["elements"] = Sortable.sequence(this.list.identify()).join(",");
    Kwo.exec("/back/form/field.sort", args,
             {"callback": this.onRefresh.bind(this)});
  }

});


kwo.tool.Contextualize = function(node, out) {
  out = out || {};
  var key, value;
  if (node) {
    node = $(node);
    var ancestors = node.ancestors();
    ancestors.unshift(node);
    ancestors.each(function (elt) {
      if (!elt.hasAttribute("data-branch-key")) return ;
      value = elt.getAttribute("data-branch-value");
      key = "branch[" + elt.getAttribute("data-branch-key") + "]";
      if (!(key in out)) {
        out[key] = value;
      }
    });
  }
  $$("[data-branch-key]").each(function (elt) {
    if (!elt.visible()) return ;
    key = "branches[" + elt.getAttribute("data-branch-key") + "]";
    value = elt.hasAttribute("data-branch-value") ? elt.getAttribute("data-branch-value").intval() : "";
    if (value > 0 && !(key in out)) {
      out[key] = value;
    }
  });
  return out;
}

kwo.tool.setBranch = function(elt, key, value) {
  elt.setAttribute("data-branch-key", key);
  elt.setAttribute("data-branch-value", value);
};



kwo.manager.Relation = Class.create({

  elt: null,
  item: null,
  models: null,
  rows: null,
  wrapper: null,
  table: null,

  initialize: function(elt) { 
    var that = this;
    this.elt = elt = $(elt);

    this.wrapper = elt.up(".relations"); 
    this.table = this.wrapper.getAttribute("data-table"); 
    this.item = this.wrapper.getAttribute("data-item");
    this.rows = this.wrapper.down("DIV.rows");
    this.models = this.wrapper.down(".item-attach-button").getAttribute("data-models");

    this.wrapper.down(".item-attach-button").observe("click", function(evt) {
      evt.stop();
      var picker = new Kwo.Class.ItemPicker(this); 
      picker.onSelect = function(args) {
        kwo.tool.Contextualize(that.elt, args);
        args["relation"] = args["key"];
        args["item"] = that.item;
        args["table"] = that.table;
        args["models"] = that.models;
        if (that.isEditable()) {
          Kwo.exec("/back/core/relation.edit", args, 
                   {"container": that.rows});
          this.close();
        }
        else {
          Kwo.exec("/back/core/relation.store", args, 
                   {"callback": function (res) {
                     that.onRefresh();
                   }});
          this.close();
        }
      }
    });

    this.rows.observe("click", function(evt) {
      evt.stop();
      var target = $(evt.target);
      var args = that.getArgs();
      if (target.hasClassName("destroy")) {
        args["relation"] = target.up("LI").getAttribute("data-item");
        Kwo.exec("/back/core/relation.destroy",args,
                 {"callback": function(resp) { that.onRefresh(); },
                  "confirm": "êtes vous sûr ?"});
      }
      else if (target.hasClassName("save")) {
        var ed = target.up(".editor");
        ed.select("INPUT[type=text]").each(function (elt) {
          args["row[" + elt.getAttribute("data-field") + "]"] = elt.getValue();
        });
        args["relation"] = ed.getAttribute("data-relation");
        Kwo.exec("/back/core/relation.store",args, 
                 {"callback": function (resp) {
                   that.onRefresh();
                 }});
      }
      else if (target.hasClassName("cancel")) {
        that.onRefresh();
      }
    });

    this.rows.on("click", "SPAN.content", function(evt, elt) { 
      evt.stop();
      if (!that.isEditable()) return ;
      var args = that.getArgs();                                                     
      args["relation"] = elt.up("LI").getAttribute("data-item");
      Kwo.exec("/back/core/relation.edit", args, 
               {"container": that.rows});
    });

    this.onAfterDisplay();

//    elt.remove();

  },

  getArgs: function() {
    var args = {"item": this.item,
                "table": this.table,
                "models": this.models};
    return args;
  },

  isEditable: function() {
    return this.rows.hasClassName("editable");
  },

  onAfterDisplay: function() { 
    var ul = this.rows.down("UL");
    if (!(ul && ul.hasClassName("sortable"))) {
      return ;
    } 
    Sortable.create(ul.id,
                    {"handle": "handle", 
                     "scroll": this.rows,
                     "onUpdate": this.onSort.bind(this)});
  },

  onSort: function (elt) { 
    var that = this;
    var args = this.getArgs();
    args["relations"] = Sortable.sequence($(elt).id).join(",");
    Kwo.exec("/back/core/relation.sort", args,
             {"callback": function(resp) { that.onRefresh(); }});
  },

  onRefresh: function() {
    var args = this.getArgs();
    Kwo.exec("/back/core/item.relations", args,
             {"container": this.rows,
              "callback": this.onAfterDisplay.bind(this)});
  }

});


kwo.manager.Resource = Class.create({

  dropContainer: null,
  item_key: null,
  wrapper: null,
  list: null,

  dropzone: null,

  initialize: function(elt) {

    elt = $(elt);

    this.wrapper = elt.up(".resources");
    this.list = this.wrapper.down("DIV.list");
    this.dropContainer = this.wrapper.down(".dropzone");

    this.dropzone = new Kwo.Class.DropZone(this.dropContainer);
    this.dropzone.onRequestComplete = this.onRefresh.bind(this);
    this.dropzone.input.accept = "image/*";

    this.item_key = elt.up("[data-item]").getAttribute("data-item");

    var that = this;

    this.wrapper.down(".file-select").on("click", function (evt) {
      evt.stop();
      var opts = {};
      if (this.hasAttribute("data-filter")) {
        opts["filter"] = this.getAttribute("data-filter");
      }
      var fm = new Kwo.Class.FileManager(opts);
      var script = this.getAttribute("data-script");
      fm.onSelect = function (filepath) {
        Kwo.exec(script,
                 {"item": that.item_key,
                  "filepath": filepath},
                 {"callback": function() {
                   that.onRefresh();
                   fm.close();
                 }});
      };
    });

    this.list.observe("click", function(evt) {
      evt.stop();
      var t = $(evt.target);
      if (t.tagName == "SPAN") {
        Kwo.exec("/back/core/resource.destroy",
                 {"id": t.up("LI").getAttribute("data-id")},
                 {"callback": function(resp) { that.onRefresh(); },
                  "confirm": "êtes vous sûr ?"});
      }
      else if (t.tagName == "A") {
        var ie = new Kwo.Class.ItemEditor(t);
        ie.onAfterClose = that.onRefresh.bind(that);
      }
    });

    this.onListDisplay();

/*    this.dropContainer.observe("dragenter", function(evt) {
      that.dropContainer.setAttribute("dragenter", true);
//      that.dropListing.innerHTML = "";
      evt.stop();
    });

    this.dropContainer.observe("dragleave", function(evt) {
      that.dropContainer.removeAttribute("dragenter");
//      that.dropListing.innerHTML = "";
      evt.stop();
    });

    this.dropContainer.observe("dragover", function(evt) {
      evt.stop();
    });

    this.dropContainer.observe("drop", this.onDrop.bind(this)); */

  },

  onListDisplay: function() {
    if (!this.list.down("UL")) {
//      console.log("no ul");
      return ;
    }
    Sortable.create(this.item_key,
                    {"handle": "handle", 
                     "scroll": this.list,
                     "onUpdate": this.onSort.bind(this)});
  },

  onSort: function (elt) {
    var that = this;
    Kwo.exec("/back/core/resource.sort",
             {"resources": Sortable.sequence($(elt).id).join(",")},
             {"callback": function(resp) { that.onRefresh(); }});
  },

  onRefresh: function() {
    Kwo.exec("/back/core/item.resources", {"item_key": this.item_key},
             {"container": this.list,
              "callback": this.onListDisplay.bind(this)});
  }

/*  onDrop: function(evt) {
    var dt = evt.dataTransfer;
    var files = dt.files;
    var imgPreviewFragment = document.createDocumentFragment();
    var count = files.length;
    var domElements;

    evt.stop();

    for (var i = 0; i < count; i++) {

      if(files[i].fileSize >= 1048576) {
        alert("file is too big, needs to be below 1mb");
        continue ;
      }

      domElements = [ document.createElement("LI"),
                      document.createElement("A"),
                      document.createElement("IMG") ];

      domElements[2].src = files[i].getAsDataURL();
      domElements[2].width = 300;
      domElements[2].height = 200;
      domElements[1].appendChild(domElements[2]);
      domElements[0].id = "item" + i;
      domElements[0].appendChild(domElements[1]);

      imgPreviewFragment.appendChild(domElements[0]);

//      this.dropListing.appendChild(imgPreviewFragment);

      this.onUploadStart(files.item(i), i);
    }

  },

  onUploadStart: function(file, index) {
    var xhr = new XMLHttpRequest();
    var cont = document.getElementById("item" + index);
    var fileUpload = xhr.upload;

    var progressDomElements = [
      document.createElement("DIV"),
      document.createElement("P")
    ];

    progressDomElements[0].className = "progressBar";
    progressDomElements[1].textContent = "0%";
    progressDomElements[0].appendChild(progressDomElements[1]);
//    console.log("ici");
    container.appendChild(progressDomElements[0]);

    fileUpload.log = cont;
    fileUpload.addEventListener("progress", this.onUploadProgress, false);
    fileUpload.addEventListener("load", this.onUploadComplete.bind(this), false);
    fileUpload.addEventListener("error", this.onUploadFailed, false);
    fileUpload.addEventListener("abort", this.onUploadCanceled, false);

    xhr.open("post", "/back/core/resource.upload", true);
    xhr.overrideMimeType("text/plain; charset=x-user-defined-binary");
    xhr.setRequestHeader("Cache-Control", "no-cache");
    xhr.setRequestHeader("X-Request-With", "XMLHttpRequest");
    xhr.setRequestHeader("X-Item", this.item_key);
    xhr.setRequestHeader("X-Prototype-Version", Prototype.Version);
    xhr.setRequestHeader("X-File-Name", file.fileName);
    xhr.setRequestHeader("X-File-Size", file.fileSize);

    xhr.sendAsBinary(file.getAsBinary());
  },

  onUploadProgress: function(evt) {
    return ;
    var percentage;
    if (!evt.lengthComputable) return ;
    percentage = Math.round((evt.loaded * 100) / evt.total);
    if (percentage < 100) {
      evt.target.log.firstChild.nextSibling.firstChild.style.width = (percentage * 2) + "px";
      evt.target.log.firstChild.nextSibling.firstChild.textContent = percentage + "%";
    }
  },

  onUploadComplete: function(evt) {
//    var currentImageItem = evt.target.log;
//    currentImageItem.className = "loaded";
//    console.log("xhr upload of " + evt.target.log.id + " complete");
    this.onRefresh();
  },


  onUploadFailed: function(error) {
    console.log("error: " + error);
  },

  onUploadCanceled: function(error) {
    console.log("error: " + error);
  } */

});

kwo.ux = {

  onNavigate: function(elt) {
    elt = $(elt);
    if (elt.hasClassName("disabled")) return ;
    var opts = {};
    opts["callback"] = function (resp) {
      if (Kwo.hasError(resp)) return Kwo.error(resp);
      var id = resp["result"]["sibling_id"];
      if (id >= 1) {
        elt.setAttribute("data-id", id);
        Kwo.F(elt.getAttribute("data-model")).onEdit(elt);
      }
      else {
        elt.setStyle({"opacity": "0.2"});
        elt.addClassName("disabled");
      }
    };
    opts["args"] = {"direction": elt.hasClassName("prev") ? "prev" : "next"};
    Kwo.F(elt.getAttribute("data-model")).onExec("navigate", elt, opts);
  },

  onExec: function(elt) { 
    if (Object.isElement(elt)) {
      elt = $(elt);
    }
    else { 
      elt = elt.element();
    }
    var opts = {};
    var model = elt.getAttribute("data-model");
    var action = elt.getAttribute("data-action");
    if (elt.hasAttribute("data-callback")) {
      opts["callback"] = elt.getAttribute("data-callback");
      if (!Object.isFunction(opts["callback"]) && opts["callback"] != "true" && opts["callback"] != true) {
        if ($(opts["callback"])) {
          var btn = $(opts["callback"]);
          opts["callback"] = function (resp) {
            if (Kwo.hasError(resp)) {
              return Kwo.error(resp);
            }
            btn.onclick();
          }
        }
      }
    }
    
    if (elt.hasAttribute("data-container")) {
      var container = elt.getAttribute("data-container");
      if (Object.isElement(container)) {
        opts["container"] = $(container);
      }
      else if (container === "panel") {
        opts["container"] = null;
        opts["callback"] = function(resp) {
          var panel = new Kwo.Class.CallbackPanel(elt, resp);
        };
      }
      else {
        if (container === "deck") {
          container = "container-" + (new Date().getTime());
          elt.setAttribute("data-container", container);
          if (!$(container)) {
            var deck = elt.up(".hbox").down(".deck");
            var pane = new Element("DIV", {"id": container, "class": "container deck-pane", "style": "display:none;"});
            deck.insert(pane);
          }
        }
        else if (container === "deck-pane") {
          container = elt.up(".deck-pane");
        }
        else if ($(container)) {
          container = $(container);
        }
        opts["container"] = container;
      }
    }
    if (elt.hasAttribute("data-confirm")) {
      opts["confirm"] = elt.getAttribute("data-confirm");
    }
    if (elt.hasAttribute("data-args")) {
      var args = elt.getAttribute("data-args");
      if (args && args.length > 1) {
        opts["args"] = args.evalJSON();
      }
    }
    
    Kwo.F(model).onExec(action, elt, opts);
  },

  onInputReset: function(elt) {
    elt = $(elt);
    var elm = elt.up("DIV.elem");
    elt.up("DIV").select("INPUT").each(function(e) {
      e.value = "";
    });
    if (elm.hasClassName("elem-image")) {
      elm.down("INPUT[type=hidden]").enable().value = "";
      elm.down("IMG.elt-image").src = "/web/core/images/unknown.gif";
    }
  },

  onInputSelect: function(elt, opts) {
    elt = $(elt);
    opts = opts || {};
    opts['filter'] = "image";
    var elm = elt.up("DIV.elem");
    if (elm.hasClassName("elem-image")) {
      var fm = new Kwo.Class.FileManager(elt.previous(), opts);
      fm.onSelect = function (file_path) {
        elm.down("IMG.elt-image").src = "/" + file_path;
        elm.down("INPUT[type=hidden]").enable().value = "/" + file_path;
        elm.down("INPUT[type=text]").value = "/" + file_path;
        fm.close();
      };
    }
  },

  onInputChange: function(elt) {
    elt = $(elt);
    var elm = elt.up("DIV.elem");
    if (elm.hasClassName("elem-image")) {
      elm.down("INPUT[type=hidden]").enable().value = elt.value;
      if (elt.value.empty()) {
        elm.down("IMG.elt-image").src = "/web/core/images/unknown.gif";
      }
    }
  },

  onGoogleMapsLoaded: function() {
    $(document).fire("google:maps:loaded");
  }

};

/*Kwo.Stackpanel = {

  doclear: false,

  onClick: function(elt) {
    elt = $(elt);
    elt.up(".stackpanel")
      .select(".opened")[0]
      .hide()
      .removeClassName("opened")
      .select("INPUT", "SELECT", "TEXTAREA")
      .invoke("disable");
    var panel = elt.next().addClassName("opened");
    if (!panel.hasClassName("cleared")) {
      panel.addClassName("cleared");
      Kwo.Stackpanel.doclear = true;
    }
    else {
      Kwo.Stackpanel.doclear = false;
    }
    panel.select("INPUT", "SELECT", "TEXTAREA").each(function (e) {
      if (Kwo.Stackpanel.doclear === true && e.getAttribute("type") != "hidden") {
        e.clear();
      }
      e.enable();
    });
    panel.show();
  }

};*/

kwo.manager.Stackpanel = Class.create({

  "elt": null,

  initialize: function(elt) {
    this.elt = $(elt).previous();
    $(elt).remove();
    var that = this;
    this.elt.select(".panel-title").each(function(elt) {
      elt.observe("click", that.onClick.bindAsEventListener(that));
    });
    this.disablePanels();
    this.enablePanel();
  },

  onClick: function(evt) {
    evt.stop();
    var elt = evt.element();
    this.disablePanels();
    this.elt.down(".opened").hide().removeClassName("opened");
    elt.next().show().addClassName("opened");
    this.enablePanel();
  },

  disablePanels: function() {
    this.elt.select("INPUT", "SELECT", "TEXTAREA").each(function (elt) {
      if (!elt.hasAttribute("data-name")) {
        elt.setAttribute("data-name", elt.getAttribute("name"));
      }
      elt.removeAttribute("name");
    });
  },

  enablePanel: function() {
    var opened = this.elt.down(".opened");
    opened.select("INPUT", "SELECT", "TEXTAREA").each(function (elt) {
      elt.setAttribute("name", elt.getAttribute("data-name"));
    });
  }

});



// http://code.google.com/apis/maps/documentation/places/supported_types.html
kwo.manager.Geocoder = Class.create({
  
  "circle": null,
  "elt": null,
  "form": null,
  "geocoder": null,
  "loaded": false,
  "map": null,
  "marker": null,
  "model": null,
  "address": null,

  initialize: function(elt) {
    if (this.loaded === false) {
      Kwo.load("http://maps.google.com/maps/api/js?" + 
               "sensor=false&libraries=geometry&callback=kwo.ux.onGoogleMapsLoaded&language=en");
    }
    var that = this;
    this.elt = $(elt);
    var table = this.elt.previous();
    this.address = table.down("INPUT.address");
    this.map = table.down(".map");
    this.form = table.down("FORM.attributes");
    this.model = this.form.getAttribute("data-model");

    if ("google" in window && "maps" in window["google"]) {
      that.display();
    }
    $(document).observe("google:maps:loaded", function (evt) {
      that.display();
    });

    this.form.observe("submit", 
                      this.onSave.bindAsEventListener(this));

    table.down("FORM.map-search").observe("submit", 
                                          this.onSearch.bindAsEventListener(this));
    
    table.down("A.marker").observe("click", function (evt) {
      that.marker.setPosition(that.map.getCenter());
      that.marker.setVisible(true);
      that.geocode();
    });

  },

  onSearch: function(evt) {
    evt.stop();
    var args = {};
    if ((args["address"] = this.getAddress()) != "") {
      this.geocode(args);
    }
  },

  onSave: function(evt) {
    evt.stop();
    var elt = evt.element();
    Kwo.F(this.model).onExec("geolocation.save", this.form, {
      "callback": function(resp) { alert("ok"); }
    });
  },

  display: function() {
    if (this.loaded === true) return ;
    var that = this;
    this.loaded = true;
    this.geocoder = new google.maps.Geocoder();
    var zoom = 4, lat = 47.9958632, lng = 13.496798000000013;
    if (!this.form.down(".zoom").value.empty()) {
      zoom = parseInt(this.form.down(".zoom").value);
      lat = parseFloat(this.form.down(".latitude").value);
      lng = parseFloat(this.form.down(".longitude").value);
    }
    var myOptions = {
      "zoom": zoom,
      "center": new google.maps.LatLng(lat, lng),
      "mapTypeId": google.maps.MapTypeId.ROADMAP,
      "mapTypeControl": false,
      "navigationControl": true,
      "streetViewControl": false
    }
    this.map = new google.maps.Map(this.map, myOptions);
    this.marker = new google.maps.Marker({"draggable": true, 
                                          "visible": false,
                                          "map": that.map,
                                          "cursor": "move"});
    
    google.maps.event.addListener(this.marker, "dragend", (function() {
      that.geocode();
    }));
    google.maps.event.addListener(this.map, "zoom_changed", function() {
      that.setZoom();
    });
  },

  geocode: function(args) {
    var that = this;
    if (Object.isUndefined(args)) {
      args = {"latLng": that.marker.getPosition(),
//              "bounds": this.map.getBounds()
             };
    }
    if (this.circle) this.circle.setMap(null);
    this.geocoder.geocode(args, function(results, status) {
      if (status == google.maps.GeocoderStatus.OK) {
        var location = results[0].geometry.location;
        if ("address" in args) {
          if ("bounds" in results[0].geometry) {
            that.map.fitBounds(results[0].geometry.bounds);
          }
          else {
            that.map.fitBounds(results[0].geometry.viewport);
          }
        }
        that.marker.setPosition(location);
        that.marker.setVisible(true);
        that.setForm(results);
      } 
      else {
        console.log("Geocode was not successful for the following reason: " + status);
      }
    });

  },

  getAddress: function() {
    return this.address.getValue();
  },

  setZoom: function() {
    this.form.down(".zoom").value = this.map.getZoom();
  },

  setForm: function(results) {
    this.form.reset();
    var location = results[0].geometry.location;
    var components = results[0].address_components;
    this.setZoom();
    this.form.down(".latitude").value = location.lat();
    this.form.down(".longitude").value = location.lng();
    this.form.down(".formatted_address").value = results[0].formatted_address;
    for (var i = 0; i < components.length; i++) {
      if (components[i]["types"][0] == "administrative_area_level_1") {
        this.form.down(".admin1").value = components[i]["long_name"];
        continue ;
      }
      if (components[i]["types"][0] == "administrative_area_level_2") {
        this.form.down(".admin2").value = components[i]["long_name"];
        continue ;
      }
      if (components[i]["types"][0] == "administrative_area_level_3") {
        this.form.down(".admin3").value = components[i]["long_name"];
        continue ;
      }
      if (components[i]["types"][0] == "country") {
        this.form.down(".country").value = components[i]["long_name"];
        this.form.down(".country_code").value = components[i]["short_name"];
        continue ;
      }
      if (components[i]["types"][0] == "locality") {
        this.form.down(".locality").value = components[i]["long_name"];
        continue ;
      }
      if (components[i]["types"][0] == "sublocality") {
        this.form.down(".sublocality").value = components[i]["long_name"];
        continue ;
      }
      if (components[i]["types"][0] == "postal_code") {
        this.form.down(".postal_code").value = components[i]["long_name"];
        continue ;
      }
    }
    if (this.circle) {
      this.circle.setMap(null);
    }
    if ("bounds" in results[0].geometry) {
      var bounds = results[0].geometry.bounds;
      var radius = google.maps.geometry.spherical.computeDistanceBetween(bounds.getNorthEast(), 
                                                                         bounds.getSouthWest()); 
      radius = radius / 2;
      this.form.down(".latitude").value = bounds.getCenter().lat();
      this.form.down(".longitude").value = bounds.getCenter().lng();
      this.form.down(".radius").value = radius / 1609.344;
      this.circle = new google.maps.Circle({"map": this.map,
                                            "center": bounds.getCenter(),
                                            "radius": radius,
                                            "strokeColor": "#FF0000",
                                            "strokeOpacity": 0.8,
                                            "strokeWeight": 2,
                                            "fillColor": "#FF0000",
                                            "fillOpacity": 0.35});
    }
  }
  
});


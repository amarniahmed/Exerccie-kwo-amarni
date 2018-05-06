
function S(name) {
  return new Kwo.Class.Obj(name);
}

Kwo.trees = {};

kwo.platform = {"models": {}};

kwo.str2func = function(str) {
  var arr = str.split(".");
  var fn = (window || this);
  for (var i = 0, len = arr.length; i < len; i++) {
    fn = fn[arr[i]];
  }
  if (typeof fn !== "function") {
    throw new Error("function not found");
  }
  return  fn;
};

kwo.confirm = function(msg, onaccept, oncancel) {
  var diag = new kwo.dialogs.Confirm(msg);
  diag.onAccept = onaccept;
  if (oncancel) {
    diag.onCancel = oncancel;
  }
};

kwo.alert = function(msg, opts) {
  opts = opts || {};
  var diag = new kwo.dialogs.Alert(msg, opts);
};

kwo.error = function(msg, opts) {
  opts = opts || {};
  opts["kind"] = "error"
  var diag = new kwo.dialogs.Alert(msg, opts);
};

Kwo.Class.Obj = Class.create({

  name: null,
  model: null,

  "action": "",
  "elt": null,
  "edit_container": null,

  initialize: function(name) {
    this.name = name;
    this.model = name;
  },

  onDestroy: function(elt) {
    this.action = "destroy";
    this.elt = $(elt).up("." + this.name + "-box");
    var that = this;
    kwo.confirm(elt.readAttribute("data-confirm"),
                function() {
                  Kwo.exec(that.root + "/" + that.name + ".destroy", that.getArgs(that.elt),
                           {"callback": that.onDestroyCallback.bind(that)});
                });
  },

  onDestroyCallback: function (resp) {
    resp = new kwo.Response(resp);
    if (resp.hasError()) return resp.alert();

    this.elt.setAttribute("data-id", 0);

    if (resp.getAttribute("is_node") == true) {
      this.elt.down("." + this.name + "-edit-box").update();
      Kwo.trees[this.name].onRefresh(resp.getAttribute("ancestor_id"));
      return ;
   }

    if (resp.getAttribute("action") == "edit") {
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
             {"confirm": elt.readAttribute("data-confirm"),
              "callback": this.onDuplicateCallback.bind(this)});
  },

  onDuplicateCallback: function (resp) {
    resp = new kwo.Response(resp);
    if (resp.hasError()) return resp.alert();
    this.elt.setAttribute("data-id", resp.getAttribute("id"));
    if (resp.getAttribute("is_node") == true) {
      Kwo.trees[this.name].onRefresh(resp.getAttribute("id"));
    }
    this.onEdit(this.elt);
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
    var container;
    if (elt.hasClassName("finder-container")) {
      container = elt;
    }
    else {
      container = elt.up("DIV.finder-container");
    }
    if (!container) {
      container = document.body;
    }
    var args = this.getArgs(elt, elt);
    args["model"] = this.model;
    Kwo.exec("/back/core/finder", args,
             {"container": container});
  },

  onPrev: function(elt) {
    $(elt).addClassName("prev");
    this.onNavigate(elt);
  },

  onNext: function(elt) {
    $(elt).addClassName("next");
    this.onNavigate(elt);
  },

  onNavigate: function(elt) {
    elt = $(elt);
    if (elt.hasClassName("disabled")) return ;
    var opts = {};
    var that = this;
    opts["callback"] = function (resp) {
      resp = new kwo.Response(resp);
      if (resp.hasError()) return resp.alert();
      var id = resp.getAttribute("sibling_id");
      if (id >= 1) {
        elt.setAttribute("data-id", id);
        that.onEdit(elt);
      }
      else {
        elt.addClassName("off");
      }
    };
    opts["args"] = {"direction": elt.hasClassName("prev") ? "prev" : "next"};
    this.onExec("navigate", elt, opts);
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
      if (container.hasAttribute("data-home-layout") && container.getAttribute("data-home-layout") == "finder") {
        if (!container.down(".finder-" + this.model)) {
          container.update('<div class="finder-container finder-' + this.model + '"><span></span></div>');
        }
        this.onSearch(container.down(".finder-" + this.model + " span"));
        return ;
      }
      var action = this.root + "/" + this.name + ".home";
      Kwo.exec(action, args,
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

  onCallback: function (resp) {
    var previous_action = this.action;
    resp = new kwo.Response(resp);

    if (resp.hasError()) {
      if (this.elt && this.elt.src && this.elt.getAttribute("data-src")) {
        this.elt.src = this.elt.getAttribute("data-src");
      }
      return resp.alert();
    }
    this.action = "";
    if (resp.hasMessage()) {
      resp.warn();
    }
    if (resp.isRedirection()) {
      if (resp.getUrl() == "/") {
        window.top.location.href = "/";
      }
      else {
        resp.redirect();
      }
      return ;
    }

    if (resp.hasAttribute("id")) {
      var id = resp.getAttribute("id");
      if (previous_action == "store") {
        if (resp.hasAttribute("notify")) {
          this.notify(resp.getAttribute("notify"));
        }
        else {
          this.notify("changes saved");
        }

        if (id >= 1) {
          this.elt.setAttribute("data-id", id);
        }

        if (resp.getAttribute("is_node") == true) {
          Kwo.trees[this.name].onRefresh(id);
          this.onEdit(this.elt);
          return ;
        }

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
      else if (previous_action == "storeandadd") {
        this.elt.setAttribute("data-id", 0);
        this.onEdit(this.elt);
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
        resp = new kwo.Response(resp);
        if (resp.hasNotification()) {
          that.notify(resp.getNotification());
        }
      };
    }
    if ("container" in opts && opts["container"]) {
      if (!Object.isElement(opts["container"]) &&
          (opts["container"] === "panel" || opts["container"] === "_blank" ||
           opts["container"] === "stdout")) {
        if (opts["container"] === "panel") {
          opts["container"] = null;
          opts["callback"] = function(resp) {
            var panel = new Kwo.Class.CallbackPanel(elt, resp);
            panel.update(resp);
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
    if (opts["container"] && (opts["container"] == "_blank" || opts["container"] == "stdout")) {
      opts["container"] = null;
      Kwo.go(action, args, opts);
    }
    else {
      if (opts["callback"] && Object.isString(opts["callback"])) {
        var callback = opts["callback"];
        opts["callback"] = function (resp) {
          resp = new kwo.Response(resp);
          if (resp.hasError()) return resp.alert();
          if (callback == "hide") {
            elt.remove();
          }
          else if (callback == "hide.up") {
            elt.up().remove();
          }
          else if (callback == "hide.down") {
            elt.down().remove();
          }
          else if (callback == "alert") {
            alert(resp.getMessage());
          }
          else if (callback == "notify") {
            alert(resp.getMessage());
          }
        }
      }
      Kwo.exec(action, args, opts);
    }
  },

  onPreview: function(elt) {
    this.action = "preview";
    var m = new kwo.dialogs.Preview(elt);
  },

  onStoreAndAdd: function(elt) {
    this.action = "storeandadd";
    this.elt = $(elt);
    var args = this.getArgs(this.elt, this.elt.up("FORM"));
    Kwo.exec(this.root + "/" + this.name + ".store", args,
             {"callback": this.onCallback.bind(this) });
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
    if (true) {
      div.addClassName("slidein");
      div.addEventListener("animationend", function (evt) {
        $(this).removeClassName("slidein");
      }, false);
      div.addEventListener("webkitAnimationEnd", function (evt) {
        $(this).removeClassName("slidein");
      }, false);
    }
    else {
      setTimeout(function () {
        emile(div.setStyle({"top": "-38px"}), "top:0px;", {"duration": 300, "after": function () {
          setTimeout(function () {
            emile(div.setStyle({"top": "0px"}), "top:-38px;", {"duration": 300});
          }, 1900);
        }});
      }, 50);
    }
  }

});



document.observe("dom:loaded", function() {

  $(document.body).observe("drop", function(evt) {
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

  if ($("menubar")) {
    $("menubar").observe("click", function (evt) {
      var elt = evt.element();
      if (elt.readAttribute("data-url")) {
        var menutitle = elt.up("LI");
        var extension = menutitle.readAttribute("data-menu");
        window.document.title = extension.ucfirst() + " - KWO";
        var url = elt.readAttribute("data-url");
        window.top._main_iframe.location = url;
        if ($("menutitle-selected") != menutitle) {
          if ($("menutitle-selected")) {
            $("menutitle-selected").removeAttribute("id");
          }
          menutitle.id = "menutitle-selected";
        }
      }
      else if (elt.hasClassName("fa")) {
        if (elt.hasClassName("filemanager")) {
          if (!window.files_popup || window.files_popup.closed) {
            window.files_popup = window.open("/back/core/folder",
                                             "_blank",
			                     "toolbar=no,location=no,directories=no,status=no,menubar=no," +
			                     "scrollbars=no,resizable=no,copyhistory=no,width=780,height=460,hotkeys=no");
            return true;
          }
          window.files_popup.focus();
        }
        else if (elt.hasClassName("logout")) {
          if (!confirm("Quitter ?")) return ;
          window.top.location='/back/core/logout';
        }
      }
    });
  }
  else {
    Object.extend(Kwo.Class.Obj.prototype,
                  {"scheme": _scope,
                   "extension": _extension,
                   "action": _action,
                   "root": "/" + _scope + "/" + _extension});
  }
});

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

  onOptionAdd: function(resp) {
    resp = new kwo.Response(resp);
    if (resp.hasError()) return resp.alert();
    this.insert({"top": "<option value=" + resp.getAttribute("id") + ">" + resp.getAttribute("name") + "</option>"});
    this.selectedIndex = 0;
  },

  onOptionPrompt: function(elt, model, from) {
    elt = $(elt);
    if (elt.value != -1) return ;
    var name = prompt("Nom du nouvel élément");
    if (name == null || name.blank()) return false;
    Kwo.exec("/back/core/item.create",
             {"model": model, "name": name, "from": from},
             {"callback": Kwo.Select.onOptionAdd.bind(elt)});
  }

}

Kwo.Form = {

  populate: function (id) {
    val = $F(id + "[0]");
    eval('var h = $H(top.' + id + '["' + val + '"])');
    $(id + "[1]").length = 0;
    h.each(function(item) {
      $(id + "[1]").options[$(id + "[1]").length] = new Option(item["value"], item["key"]);
    });
  },

  populateSelect: function (id,data_json) {
    $(id).length = 0;
    if (data_json.substring(0, 1) == "[") {
      arr = data_json.evalJSON();
      for (i = 0; i < arr.length; i++) {
	$(id).options[$(id).length] = new Option(arr[i],arr[i]);
      }
    }
    else if (data_json.substring(0, 1) == "{") {
      hash = $H(data_json.evalJSON());
      hash.each(function(item) {
	$(id).options[$(id).length] = new Option(item["value"], item["key"]);
      });
    }
    else alert("strange json data");
  },

  select_transfer: function (from,to) {
    selFrom = $(from);
    optsel = selFrom.options[selFrom.selectedIndex];
    if (optsel < 0) return;
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
             {"path": this.path,
              "filter": this.opts["filter"]},
             {"callback": this.onCallback.bind(this)});
  },

  onFileSelect: function(file) {
    alert(file["path"]);
  },

  onRefresh: function() {
    this.onDirSelect(this.getPath());
  },

  onCallback: function(resp) {
    resp = new kwo.Response(resp);
    if (resp.hasError()) return resp.alert();
    var that = this;
    var ul = new Element("UL", {"class": "file-panel"});
    this.container.update(ul);
    var files = resp.getAttribute("files");
    var html, elt, file, i, l = files.length;
    var file;
    for (i = 0; i < l; ++i) {
      file = files[i];
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

Kwo.Developer = {

  displayScheduler: function(elt) {
    var container = $("developer-scheduler");
    Kwo.exec("/back/core/developer.scheduler", {},
             {"container": container,
              "callback": function () {
                container.up().raise(container);}
             });
  },

  displaySchedulerLog: function(elt) {
    elt = $(elt);
    Kwo.exec("/back/core/developer.scheduler.log",
             {"filename": elt.getAttribute("data-filename")},
             {"container": elt.up("TD").next()});
  },

  displayLogs: function(elt) {
    var container = $("developer-logs");
    Kwo.exec("/back/core/developer.logs", {},
             {"container": container,
              "callback": function () {
                container.up().raise(container);
                $("ta-code").scrollTop = $("ta-code").scrollHeight;
                $("ta-url").scrollTop = $("ta-url").scrollHeight;
              }});
  },

  resetLogs: function(elt) {
    Kwo.exec("/back/core/developer.logs.reset", {}, {"confirm": elt});
  },

  downloadLogs: function(elt) {
    Kwo.go("/back/core/developer.logs.download", {}, {"confirm": elt});
  },

  phpinfos: function(elt) {
    elt.writeAttribute("data-url", "/back/core/sys.infos");
    new kwo.dialogs.Preview(elt);
  },

  onShellExec: function(elt) {
    elt = $(elt);
    $("shell_output").update("");
    Kwo.exec("/back/core/shell.exec", elt.up("FORM"),
             {"callback": function(resp) {
               resp = new kwo.Response(resp);
               $("shell_output").update(resp.getAttribute("output"));
             }});
  }

};



Kwo.Finder = {

  onSwitchPage: function(elt) {
    elt = $(elt);
    var finder = elt.up(".finder");
    var model = finder.getAttribute("data-model");
    var page = elt.down("INPUT[type=text]").value.intval();
    if (page < 1) {
      page = 1;
    }
    else if (page > finder.getAttribute("data-page-count").intval()) {
      page = finder.getAttribute("data-page-count").intval();
    }
    finder.down("INPUT[name=page]").value = page;
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
      if (elt.getAttribute("data-action") == "destroy" && args["id[]"].length < 1) {
        return kwo.alert(elt.getAttribute("data-empty") + ".");
      }
      var bm = new kwo.dialogs.BatchManager(elt, model, args["id[]"], finder.getAttribute("data-finder"));
      bm.onAfterClose = function() {
        if (bm.submitted === true) {
          Kwo.F(model).onHome(elt);
        }
      };
      return ;
      var msg = args["id[]"].length + " " + elt.getAttribute("data-selection") + "\n";
      opts["confirm"] = msg + elt.getAttribute("data-confirm").ucfirst();
    }

    opts["callback"] = function(resp) {
      resp = new kwo.Response(resp);
      Kwo.F(model).onHome(elt);
      Kwo.F(model).notify(resp.getMessage());
    };
    Kwo.exec("/back/core/" + model + "." + action, args, opts);
  },

  onNext: function(elt) {
    elt = $(elt);
    var finder = elt.up(".finder");
    var model = finder.getAttribute("data-model");
    finder.down("INPUT[name=page]").value = finder.down("INPUT[name=page]").value.intval() + 1;
    console.log(finder.down("INPUT[name=page]"));
    Kwo.F(model).onSearch(finder.down("FORM"));
  },

  onPrev: function(elt) {
    elt = $(elt);
    var finder = elt.up(".finder");
    var model = finder.getAttribute("data-model");
    finder.down("INPUT[name=page]").value = finder.down("INPUT[name=page]").value.intval() - 1;
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
      if (elt.hasClassName("checkbox-controller")) {
        var table = elt.up("DIV").next("DIV").down("TABLE");
        if (elt.checked == false) {
          table.select("TR").each(function (e) {
            e.removeClassName("checked");
            e.down("INPUT[type=checkbox]").checked = false;
          });
        }
        else {
          table.select("TR").each(function (e) {
            e.addClassName("checked");
            e.down("INPUT[type=checkbox]").checked = true;
          });
        }
      }
      else {
        elt.up("TR").toggleClassName("checked");
        var n = elt.up("TABLE").select("TR[class=checked]").size();
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
      Kwo.exec("/back/core/" + finder.getAttribute("data-model") + ".attribute.edit",
               {"id": elt.readAttribute("data-id"),
                "attribute": elt.getAttribute("data-attribute")},
               {"container": elt.up()});
      elt.remove();
      finder.down(".button-save").show();
      return ;
    }
    else if (elt.tagName == "A") {
      return ;
    }
    else {
      var finder = elt.up(".finder");
      var model = finder.readAttribute("data-model");
      Kwo.F(model).onEdit(elt.up("TR"));
      evt.stop();
    }
  },

  onLoad: function(elt) {

  },

  load: function(elt) {
    var span = $(elt).previous().down("span");
    $(elt).remove();
    Kwo.F(span.getAttribute("data-model")).onSearch(span);
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
      that.onFilePush(this.files);
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
    var is_first = this.files.length < 1;
    this.onBeforeUpload();
    for (var i = 0; i < files.length; i++) {
      this.files.push(files.item(i));
    }
    if (is_first == true) {
      this.onUpload();
    }
  },

  onDrop: function(evt) {
    evt.stop();
    var files = evt.dataTransfer.files;
    this.onFilePush(files);
  },

  onBeforeUpload: function() {},

  onUpload: function() {
//    var file = this.files[0];
    this.label.update(this.files.length + " file(s) in queue");
    var file = this.files.shift();
    var xhr = new XMLHttpRequest();
    var script = "/back/core/file.upload";

    if (this.elt.hasAttribute("data-script")) {
      script = this.elt.getAttribute("data-script");
    }
    if (this.elt.hasAttribute("data-destination")) {
      this.setDestination(this.elt.getAttribute("data-destination"));
    }

    var that = this;

    xhr.upload.addEventListener("progress", function(evt) {
      that.onUploadProgress(evt);
    }, false);
    xhr.upload.addEventListener("load", function(evt) {
      that.onUploadComplete(evt);
    }, false);
    xhr.upload.addEventListener("error",  function(evt) {
      that.onUploadFailed(evt);
    }, false);
    xhr.upload.addEventListener("abort",  function(evt) {
      that.onUploadCanceled(evt);
    }, false);

    xhr.open("post", script, true);

    xhr.onreadystatechange = function(evt) {
//      if (this.readyState != 4) {

      /**
         READYSTATE_UNINITIALIZED (0)
         The object has been created, but not initialized (the open method has not been called).
         READYSTATE_LOADING (1)
         A request has been opened, but the send method has not been called.
         READYSTATE_LOADED (2)
         The send method has been called. No data is available yet.
         READYSTATE_INTERACTIVE (3)
         Some data has been received; however, neither responseText nor responseBody is available.
         READYSTATE_COMPLETE (4)
         All the data has been received.
       **/
      if (this.readyState == 4) {
        if (that.files.length >= 1) {
          that.onUpload();
          //          var t = setTimeout(function () { that.onUpload(); }, 3000);that.files.length < 1) {
        }
        else {
          /* PB NON MISE à JOUR de l input*/
          that.label.update("drop zone");
          that.onRequestComplete();
        }
      }
    };

    xhr.overrideMimeType("text/plain; charset=x-user-defined-binary");

    var filename = "name" in file ? file.name : file.fileName;
    var filesize = "size" in file ? file.size : file.fileSize;

    xhr.setRequestHeader("Cache-Control", "no-cache");
    xhr.setRequestHeader("X-Request-With", "XMLHttpRequest");
    xhr.setRequestHeader("X-Prototype-Version", Prototype.Version);
    xhr.setRequestHeader("X-File-Destination", this.destination);
    xhr.setRequestHeader("X-File-Name", filename);
    xhr.setRequestHeader("X-File-Size", filesize);
    xhr.setRequestHeader("X-File-Type", file.type);
    if (this.elt.up("[data-item]")) {
      xhr.setRequestHeader("X-Item", this.elt.up("[data-item]").getAttribute("data-item"));
    }

    if ("name" in file) {
      xhr.send(file);
    }
    else {
      xhr.sendAsBinary(file.getAsBinary());
    }
  },

  onUploadProgress: function(evt) {
/*
 http://hacks.mozilla.org/2009/06/xhr-progress-and-richer-file-uploading-feedback/
 http://hacks.mozilla.org/2009/12/uploading-files-with-xmlhttprequest/
 http://hacks.mozilla.org/2010/02/an-html5-offline-image-editor-and-uploader-application/
*/
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

kwo.managers.Launcher = Class.create({

  elt: null,

  initialize: function(elt) {
    this.elt = $(elt).previous();
    $(elt).remove();
    this.observe();
  },

  observe: function() {
    if (this.elt.inDialog()) {
      this.elt.down("[data-name=search]").remove();
      this.elt.down("[data-name=prev]").remove();
      this.elt.down("[data-name=next]").remove();
    }
  }

});

kwo.managers.ItemTracer = Class.create({

  parent: null,

  initialize: function(elt) {
    this.parent = $(elt).previous();
    this.iframe = this.parent.down("IFRAME");
    this.observe();
  },

  down: function(expr) {
    return this.parent.down(expr);
  },

  observe: function() {
    var that = this;
    this.down("FORM").observe("submit", function(evt) {
      evt.stop();
      that.down(".list").update("");
      that.iframe.src = this.down("INPUT").getValue() || "/";
    });
    this.down("SPAN.refresh").observe("click", function(evt) {
      evt.stop();
      that.down(".list").update('<img src="/web/core/images/throbbers/default.gif" />');
      that.listItems();
    });
    document.observe("page:dom:loaded", function(evt) {
      that.down(".list").update('<img src="/web/core/images/throbbers/default.gif" />');
      that.listItems();
      that.down("INPUT").setValue(that.iframe.contentDocument.location);
    });

  },

  listItems: function(items) {
    var that = this;
    setTimeout(function () {
      Kwo.exec("/back/core/item.tracer.list", {},
               {"container": that.down(".list"),
                "callback": that.observeItems.bind(that)});
    }, 900);
  },

  observeItems: function() {
    var that = this;
    this.down(".list").select("LI").each(function(elt) {
      elt.observe("click", function(evt) {
        evt.stop();
        var ie = new kwo.dialogs.ItemEditor(this);
        ie.onBeforeClose = function() {
          that.iframe.contentDocument.location = that.iframe.contentDocument.location;
        };
      });
    })
  }

});

kwo.managers.RealtimeOverview = Class.create({

  parent: null,
  pe: null,
  nodes: {},
  datas: {},
/*  fields: ["locales", "mediums", "pages", "countries", "locales",
           "queries", "newcomers", "visitors", "users"], */

  initialize: function(elt) {
    var that = this;
    this.parent = $(elt).previous();
    $(elt).observe("DOMNodeRemoved", this.stopUpdating.bindAsEventListener(this));
    this.refresh();
    if (this.parent.down("BUTTON[value=stop]")) {
      this.parent.down("BUTTON[value=stop]").observe("click", function(evt) {
        evt.stop();
        $(elt).remove();
        if (that.pe) that.pe.stop();
      });
    }
    if (this.parent.down(".refresh")) {
      this.parent.down(".refresh").observe("click", function(evt) {
        evt.stop();
        that.refresh();
      });
    }
    if (this.parent.down("[data-date]")) {
      this.parent.down("[data-date]").observe("click", function(evt) {
        evt.stop();
        Kwo.go("/back/core/visit.dashboard", {date: this.getAttribute("data-date")});
      });
    }
//    this.startUpdating();
  },

  down: function(expr) {
    return this.parent.down(expr);
  },

  node: function(key) {
    if (!this.nodes[key]) {
      var expr = "." + key;
      this.nodes[key] = this.down(expr);
      if (!this.nodes[key]) console.log(expr + " : node unknown");
    }
    return this.nodes[key];
  },

  startUpdating: function() {
    var that = this;
    this.pe = new PeriodicalExecuter(function(pe) { that.refresh(); }, 10);
  },

  refresh: function() {
    var that = this;
    Kwo.exec("/back/core/visit.realtime.datas", null,
             {"callback": function (resp) {
               resp = new kwo.Response(resp);
               if (resp.hasError()) return console.log(resp);
               var key, h = resp.getAttributes();
               that.datas = h["apps"][1];
               var fields = $H(that.datas).keys();
               for (var i = 0; i < fields.length; i++) {
                 key = fields[i];
                 that.update(that.node(key), that.datas[key]);
               }
             }});
  },

  update: function(elt, val) {
    var layout = elt.getAttribute("data-layout") || (val instanceof Array ? "list" : "inline");
    elt.setAttribute("data-layout", layout);
    this["update" + layout.ucfirst()](elt, val);
    elt.addClassName("anim-fade");
    var evt_name = Prototype.Browser.WebKit ? "webkitAnimationEnd" : "animationend";
    elt.addEventListener(evt_name, function (evt) {
      evt.stop();
      $(this).removeClassName("anim-fade");
    }, false);
  },

  updateInline: function(elt, val) {
    if (val instanceof Array) {
      val = JSON.stringify(val);
    }
    elt.update(val);
  },

  updateList: function(elt, val) {
    var pair;
    if (val.length < 1) {
      elt.update(elt.getAttribute("data-fallback") + ".");
    }
    else {
      var ul = new Element("ul");
      for (var i=0; i < val.length; i++) {
        pair = val[i];
        ul.insert(new Element("li").update("<span>" + pair[1] + "</span><p>" + pair[0] + "</p>"));
      }
      elt.update(ul);
    }
  },

  stopUpdating: function(evt) {
    console.log("stop updating");
    this.pe.stop();
  }

});

kwo.managers.Dashboard = Class.create({

  elt: null,
  period: null,
  overview: null,
  precision: null,
  model: null,
  year: null,
  month: null,

  initialize: function(elt) {
    this.elt = $(elt).previous();
    $(elt).remove();
    this.period = this.elt.down(".dashboard-period");
    this.overview = this.elt.down(".dashboard-overview");
    this.precision = this.elt.down(".dashboard-precision");
    this.model = this.elt.getAttribute("data-model");
    this.period.observe("click", this.onPeriodSelect.bindAsEventListener(this));
    this.period.observe("change", this.onPeriodChange.bindAsEventListener(this));
    this.year = this.elt.down(".period-year SELECT");
    this.month = this.elt.down(".period-month SELECT");
    if (this.elt.getAttribute("data-date")) {
      var parts = this.elt.getAttribute("data-date").split("-");
      if (parts[0].intval() > 0) {
        this.year.setValue(parts[0].intval());
        if (!parts[1] || parts[1].intval() < 1) {
          this.elt.down(".period-year span").click();
        }
      }
      if (parts[1].intval() > 0) {
        this.month.setValue(parts[1].intval());
        if (!parts[2] || parts[2].intval() < 1) {
          this.elt.down(".period-month span").click();
        }
      }
    }
  },

  onPeriodChange: function(evt) {
    evt.stop();
    var elt = evt.element();
    this.overview.update('<div class="wrapper"></div>');
    this.precision.update('<div class="wrapper"></div>');
    var args = {};
    args["year"] = $("year").getValue();
    if (elt.id == "month") {
      args["month"] = $("month").getValue();
    }
    if (args["year"].intval() < 1) {
      elt.up(".period").down("TABLE.month").up().hide().next().hide();
      return ;
    }
    Kwo.exec("/back/core/dashboard.period.picker", args,
             {"container": this.period});
  },

  onPeriodSelect: function(evt) {
    evt.stop();
    var elt = evt.element();
    if (!(elt.tagName == "SPAN" || elt.tagName == "A")) return ;
    this.overview.update('<div class="wrapper"></div>');
    this.precision.update('<div class="wrapper"></div>');
    this.period.select(".period-row-selected").invoke("removeClassName", "period-row-selected");
    var args = {"year":0, "month":0, "day":0};
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
      var root = elt.up(".period");
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
    var that = this;
    Kwo.exec("/back/core/" + this.model + ".dashboard.overview", args,
             {"container": this.overview.down("DIV.wrapper"),
              "callback": function () { that.bind(that.overview); }});
  },

  bind: function(container) {
    var that = this;
    container.select("[data-action]").each(function(elt) {
      elt.observe("click", that.onRefine.bindAsEventListener(that));
    });
  },

  onRefine: function(evt) {
    evt.stop();
    var elt = evt.currentTarget ? $(evt.currentTarget) : $(evt.srcElement);
    var action = elt.getAttribute("data-action");
    if (action[0] != "/") {
      action = "/back/core/" + elt.getAttribute("data-action");
    }
    var args = {"period": this.elt.getAttribute("data-period")};
    var s = elt.getAttribute("data-values");
    if (s && s.length > 1) {
      args = [args, s.evalJSON()];
    }
    Kwo.exec(action, args, {"container": this.precision.down("DIV.wrapper")});
  }

});

kwo.managers.Column = Class.create({

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
    if (node_id) {
      this.elt.setAttribute("data-branch-value", node_id);
    }
    Kwo.F(this.tree_model).onExec("columnview",
                                  this.elt,
                                  {"container": this.elt.up("." + this.node_model + "-home-box")});
  }

});

kwo.managers.Field = Class.create({

  editbox: null,
  homebox: null,
  list: null,
  workspace: null,

  initialize: function(elt) {
//    this.workspace = $(elt).up(".field-box");
    this.workspace = $(elt).previous();
//    this.onBind();
    this.homebox = this.workspace.down(".field-home-box");
    this.editbox = this.workspace.down(".field-edit-box");
    var that = this;
    this.homebox.observe("container:updated", function(evt) {
      evt.stop();
      that.onBind();
    });
    this.homebox.fire("container:updated");
    $(elt).remove();
  },

  onBind: function() {
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

/*    this.onListDisplay();

  },

  onListDisplay: function() {*/

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
        Kwo.exec("/back/core/field.create", args,
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
    var args = kwo.tool.Contextualize(this.workspace);
/*    Kwo.exec("/back/core/field.home", args,
             {"container": this.homebox,
              "callback": this.onBind.bind(this)});*/
    Kwo.exec("/back/core/field.home", args,
             {"container": this.homebox});
  },

  onSort: function(elt) {
    var args = kwo.tool.Contextualize(elt);
    args["elements"] = Sortable.sequence(this.list.identify()).join(",");
    Kwo.exec("/back/core/field.sort", args,
             {"callback": this.onRefresh.bind(this)});
  }

});

kwo.tool.Contextualize = function(node, out) {
  out = out || {};
  var key, value, dialog, elt, ancestors;
  if (node) {
    node = $(node);
    ancestors = node.ancestors();
    ancestors.unshift(node);
    ancestors.each(function (elt) {
      if (!elt.hasAttribute("data-branch-key")) return ;
      value = elt.getAttribute("data-branch-value");
      key = "branch[" + elt.getAttribute("data-branch-key") + "]";
      if (!(key in out)) {
        out[key] = value;
      }
    });
    if (node && kwo.registry["dialogs"].length >= 1) {
      for (var i = kwo.registry["dialogs"].length - 1; i >= 0; i--) {
        dialog = kwo.registry["dialogs"][i];
        if (dialog.modal == true) continue ;
        elt = dialog.getElement();
        ancestors = elt.ancestors();
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
    }
  }
  $$("[data-branch-key]").each(function (elt) {
    if (!elt.visible()) return ;
    key = "branches[" + elt.getAttribute("data-branch-key") + "]";
    value = elt.hasAttribute("data-branch-value") ? elt.getAttribute("data-branch-value").intval() : "";
    if (value > 0 && !(key in out)) {
      out[key] = value;
    }
  });
  if (kwo.registry["dialogs"].length > 0) {
    var item, parts;
    for (var i = 0; i < kwo.registry["dialogs"].length; i++) {
      dialog = kwo.registry["dialogs"][i];
      if ("getItem" in dialog && dialog.getItem()) {
        item = dialog.getItem();
        parts = item.split("-");
        out["dialogs[" + kwo.platform.models[parts[0]] + "]"] = parts[1];
      }
      if ("getCallerItem" in dialog && dialog.getCallerItem()) {
        out["caller_item"] = dialog.getCallerItem();
      }
    }
  }
  return out;
}

kwo.tool.model = function(model) {
  if (Object.isElement(model)) {
    model = model.getAttribute("data-item");
  }
  if (model.match(/\-/)) {
    var parts = model.split("-");
    return kwo.platform.models[parts[0]];
  }
  return kwo.platform.models[model];
}

kwo.tool.setBranch = function(elt, key, value) {
  elt.setAttribute("data-branch-key", key);
  elt.setAttribute("data-branch-value", value);
};


kwo.managers.Acl = Class.create({

  elt: null,
  wrapper: null,
  rows: null,
  item: null,

  initialize: function(elt) {
    var that = this;
    this.elt = elt = $(elt);
    this.wrapper = elt.up(".acl");
    this.item = this.wrapper.getAttribute("data-item");
    this.rows = this.wrapper.down("DIV.rows");
    this.onAfterDisplay();

    this.rows.observe("click", function(evt) {
      var target = $(evt.target);
      var args = that.getArgs();
      if (target.hasClassName("destroy")) {
        evt.stop();
        args["user_id"] = target.up("LI").getAttribute("data-user-id");
        Kwo.exec("/back/core/permission.destroy", args,
                 {"callback": function(resp) { that.onRefresh(); },
                  "confirm": that.wrapper.getAttribute("data-destroy-confirm")});
      }
      else if (target.hasClassName("content")) {
        evt.stop();
        args["user_id"] = target.up("LI").getAttribute("data-user-id");
        Kwo.exec("/back/core/permission.edit", args,
                 {"container": that.rows});
      }
      else if (target.hasClassName("save")) {
        evt.stop();
        var ed = target.up(".editor");
        args["user_id"] = ed.down("[data-field=user_id]").getValue();
        if (ed.down("[name=permissions]")) {
          args["permissions"] = ed.down("[name=permissions]").getValue();
        }
        Kwo.exec("/back/core/permission.store",args,
                 {"callback": function(resp) { that.onRefresh(); }});
      }
      else if (target.hasClassName("user-picker")) {
        evt.stop();
        var ip = new kwo.dialogs.ItemPicker(target);
        ip.onSelect = function(h) {
          h = h[0];
          var user = that.rows.down(".user");
          user.down("INPUT[type=hidden]").setValue(h["id"]);
          user.down("A").hide();
          user.down("SPAN").update(h["attrs"]["email"]).show();
          that.rows.down("INPUT[type=button]").show();
          this.close();
        }
      }
      else if (target.hasClassName("cancel")) {
        evt.stop();
        that.onRefresh();
      }
    });

    this.wrapper.down(".item-attach-button").observe("click", function(evt) {
      evt.stop();
      Kwo.exec("/back/core/permission.edit", that.getArgs(),
               {"container": that.rows});
    });

  },

  onAfterDisplay: function() {
  },

  getArgs: function() {
    var args = {"item": this.item};
    return args;
  },

  onRefresh: function() {
    Kwo.exec("/back/core/item.users", this.getArgs(),
             {"container": this.rows,
              "callback": this.onAfterDisplay.bind(this)});
  }

});

kwo.managers.Conversion = Class.create({

  elt: null,
  item: null,

  initialize: function(elt) {
    var that = this;
    this.elt = $(elt).previous();
    this.item = this.elt.up("[data-item]").getAttribute("data-item");
    $(elt).remove()
    this.observe();
  },

  observe: function() {
    var that = this;
    this.elt.select("A, SPAN").each(function(elt) {
      elt.observe("click", function(evt) {
        evt.stop();
        var elt = evt.element();
        var parent = elt.up("[data-visit-id]");
        var visit_id = parent.getAttribute("data-visit-id");
        if (elt.hasClassName("select")) {
          that.onSelect(visit_id);
        }
        else if (elt.hasClassName("visit-detail")) {
          new kwo.dialogs.Visit(parent);
        }
      })
    });
  },

  onSelect: function(visit_id) {
    var that = this;
    var args = {"visit_id": visit_id, "item": this.item};
    Kwo.exec("/back/core/conversion.visit.set", args,
             {"confirm": this.elt.getAttribute("data-confirm") + " <" + visit_id + "> ?",
              "callback": function(resp) {
                resp = new kwo.Response(resp);
                if (resp.hasError()) return resp.alert();
                Kwo.F("conversion").onEdit(that.elt);
              }});
  }

});

kwo.managers.Relation = Class.create({

  elt: null,
  item: null,
  models: null,
  rows: null,
  wrapper: null,
  table: null,
  callback: null,

  initialize: function(elt) {
    var that = this;
    this.elt = elt = $(elt);
    this.wrapper = elt.up(".relations");
    this.table = this.wrapper.getAttribute("data-table");
    this.item = this.wrapper.getAttribute("data-item");
    this.rows = this.wrapper.down("DIV.rows");
    if (this.wrapper.hasAttribute("data-callback")) {
      this.callback = kwo.str2func(this.wrapper.getAttribute("data-callback"));
    }
    this.models = this.wrapper.down(".item-attach-button").getAttribute("data-models");

    if (this.rows.hasClassName("pickable")) {
      this.wrapper.down(".item-attach-button").observe("click", function (evt) {
        evt.stop();
        var picker = new kwo.dialogs.ItemPicker(this);
        picker.onSelect = function(args) {
          var items = args;
          args = {};
          kwo.tool.Contextualize(that.elt, args);
          args["item"] = that.item;
          args["table"] = that.table;
          args["models"] = that.models;
          if (that.isRowEditable()) {
            args["relation"] = items[0]["key"];
            Kwo.exec("/back/core/relation.edit", args,
                   {"container": that.rows});
            this.close();
            return ;
          }
          for (var i = 0; i < items.length; i++) {
            args["relation"] = items[i]["key"];
            Kwo.exec("/back/core/relation.store", args,
                     {"callback": function (resp) {
                       that.onRefresh();
                     }});
          }
          this.close();
        }
      });
    }
    else {
      this.wrapper.down(".item-attach-button").observe("click", function (evt) {
        evt.stop();
        var elt = evt.element();
        this.setAttribute("data-model", that.models);
        var ie = new kwo.dialogs.ItemEditor(this);
        ie.caller_item = elt.getAttribute("data-caller-item");
        ie.onAfterClose = that.onRefresh.bind(that)
      });
    }

    this.rows.observe("click", function(evt) {
      evt.stop();
      var target = $(evt.target);
      var args = that.getArgs();
      if (target.hasClassName("destroy")) {
        args["relation"] = target.up("LI").getAttribute("data-item");
        Kwo.exec("/back/core/relation.destroy", args,
                 {"callback": function(resp) { that.onRefresh(); },
                  "confirm": that.wrapper.getAttribute("data-destroy-confirm")});
      }
      else if (target.hasClassName("save")) {
        var ed = target.up(".editor");
        ed.select("*[data-field]").each(function (elt) {
          args["row[" + elt.getAttribute("data-field") + "]"] = elt.getValue();
        });
        args["relation"] = ed.getAttribute("data-relation");
        Kwo.exec("/back/core/relation.store", args,
                 {"callback": function (resp) { that.onRefresh(); }});
      }
      else if (target.hasClassName("cancel")) {
        that.onRefresh();
      }
    });

    this.rows.on("click", "span.icon-edit-row", function (evt, elt) {
      evt.stop();
      var li = elt.up("li");
      var args = that.getArgs();
      args["relation"] = li.getAttribute("data-item");
      Kwo.exec("/back/core/relation.edit", args,
               {"container": that.rows});
    });

    this.rows.on("click", "span.content", function (evt, elt) {
      evt.stop();
      var li = elt.up("li");
      var ie = new kwo.dialogs.ItemEditor(li);
      ie.onAfterClose = that.onRefresh.bind(that);
    });

    this.onAfterDisplay();

  },

  getArgs: function() {
    var args = {"item": this.item,
                "table": this.table,
                "models": this.models};
    return args;
  },

  isRowEditable: function() {
    return this.rows.hasClassName("edition-row");
  },

  onAfterDisplay: function() {
    var ul = this.rows.down("ul");
    if ((ul && ul.hasClassName("sortable"))) {
        /*
      Sortable.create(ul.id,
                      {"handle": "handle",
                       "scroll": this.rows,
                       "onUpdate": this.onSort.bind(this)});
      */
      new ScrollSort(ul, this.onSort.bind(this));
    }
    if (this.isRowEditable()) {
      this.rows.select("span.content").each(function(elt) {
        elt.insert({after:'<span class="icon-sort icon-edit-row"></span>'});
      });
    }
    if (this.callback) {
      this.callback.call(this);
    }
  },

  onSort: function (elt) {
    var that = this;
    var args = this.getArgs();
    args["relations"] = this.rows.down("ul").select("li").pluck("dataset").pluck("item").join(",");
    Kwo.exec("/back/core/relation.sort", args,
             {"callback": function (resp) { that.onRefresh(); }});
  },

  onRefresh: function() {
    var args = this.getArgs();
    Kwo.exec("/back/core/item.relations", args,
             {"container": this.rows,
              "callback": this.onAfterDisplay.bind(this)});
  }

});


kwo.managers.Resource = Class.create({

  dropContainer: null,
  item: null,
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
    this.item = elt.up("[data-item]").getAttribute("data-item");

    var that = this;

    this.wrapper.down(".file-select > span").on("click", function (evt) {
      evt.stop();
      var opts = {};
      if (this.hasAttribute("data-filter")) {
        opts["filter"] = this.getAttribute("data-filter");
      }
      if (this.hasAttribute("data-path")) {
        opts["path"] = this.getAttribute("data-path");
      }
      var fm = new kwo.dialogs.FileManager(opts);
      var script = this.getAttribute("data-script");
      fm.onSelect = function (filepath) {
        Kwo.exec(script,
                 {"item": that.item,
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
                 {"callback": function (resp) { that.onRefresh(); },
                  "confirm": that.wrapper.getAttribute("data-destroy-confirm")});
      }
      else if (t.tagName == "A") {
        var ie = new kwo.dialogs.ItemEditor(t);
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
      return ;
    }
    Sortable.create(this.item,
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
    Kwo.exec("/back/core/item.resources", {"item": this.item},
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
    xhr.setRequestHeader("X-Item", this.item);
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
      resp = new kwo.Response(resp);
      if (resp.hasError()) return resp.alert();
      var id = resp.getAttribute("sibling_id");
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
    var model, opts = {};
    if (elt.hasAttribute("data-model")) {
      model = elt.getAttribute("data-model");
    }
    else {
      model = kwo.tool.model(elt);
    }
    var action = elt.getAttribute("data-action");
    if (elt.hasAttribute("data-callback")) {
      opts["callback"] = elt.getAttribute("data-callback");
      if (!Object.isFunction(opts["callback"]) && opts["callback"] != "true" && opts["callback"] != true) {
        if ($(opts["callback"])) {
          var btn = $(opts["callback"]);
          opts["callback"] = function (resp) {
            resp = new kwo.Response(resp);
            if (resp.hasError()) return resp.alert();
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
          var cp = new kwo.dialogs.CallbackPanel(elt);
          cp.update(resp);
        };
      }
      else {
        if (container === "deck") {
          container = "container-" + (new Date().getTime());
          elt.setAttribute("data-container", container);
          if (!$(container)) {
            var deck = elt.up(".hbox").down(".deck");
            var pane = new Element("DIV", {"id": container,
                                           "class": "container deck-pane",
                                           "style": "display:none;"});
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
      if (args == "form") {
        if ("form" in arg && arg.form) {
          var form = $(arg.form)
          opts["args"] = form.serialize(true);
        }
      }
      else if (args && args.length > 1) {
        opts["args"] = args.evalJSON();
      }
    }
    Kwo.F(model).onExec(action, elt, opts);
  },

  onInputReset: function(elt) {
    elt = $(elt);
    var elm = elt.up("DIV.elem");
    elt.up("DIV").select("INPUT").each(function(elt) {
      elt.setValue("");
      console.log(elt);
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
      var fm = new kwo.dialogs.FileManager(elt.previous(), opts);
      fm.onSelect = function (file_path) {
        elm.down("IMG.elt-image").src = file_path;
        elm.down("INPUT[type=hidden]").enable().value = file_path;
        elm.down("INPUT[type=text]").value = file_path;
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
    $(document.body).addClassName("google-maps-loaded");
  },

  onReload: function(elt) {
    elt = elt.up(".box-edit");
    Kwo.F(elt.getAttribute("data-model")).onEdit(elt);
  },

  onItemEdit: function(elt) {
    elt = $(elt);
    var ed = new kwo.dialogs.ItemEditor(elt);
    if (elt.hasAttribute("data-item")) {
      if (elt.hasAttribute("data-callback")) {
        ed.onAfterClose = function() {
          kwo.ux.onReload(elt);
        }
      }
      else {
        var container = elt;
        if (elt.hasAttribute("data-container")) {
          container = $(elt.getAttribute("data-container"));
          if (container == "previous") {
            container = container.previous();
          }
          else if (container == "next") {
            container = container.next();
          }
          else if (container == "up") {
            container = container.up();
          }
          else if (container == "down") {
            container = container.down();
          }
        }
        var args = {};
        args["item"] = elt.getAttribute("data-item");
        if (elt.hasAttribute("data-view")) {
          args["view"] = elt.getAttribute("data-view");
        }
        ed.onAfterClose = function() {
          if (container.tagName == "INPUT") {
            Kwo.exec("/back/core/item.name", args,
                     {"callback": function(resp) {
                       resp = new kwo.Response(resp);
                       if (resp.hasError()) return resp.alert();
                       elt.setValue(resp.getAttribute("name"));
                     }});
          }
          else {
            Kwo.exec("/back/core/item.view", args,
                     {"container": container});
          }
        }
      }
    }
    else {
      ed.onAfterClose = function() {
        Kwo.exec("/back/core/item.attrs", {"item": elt.getAttribute("data-item")},
                 {"callback": function(resp) {
                   resp = new kwo.Response(resp);
                   if (resp.hasError()) return resp.alert();
                   if (elt.tagName == "INPUT") {
                     elt.setValue(resp.getAttribute("name"));
                   }
                 }});
      };
    }
  },

  onInputPeriodClick: function(evt) {
    var src = $(evt.currentTarget);
    var elt = $(evt.target);
    evt.stop();
    if (elt.hasClassName("reset")) {
      src.setAttribute("data-from", "");
      src.setAttribute("data-to", "");
      src.down(".from").setValue("");
      src.down(".to").setValue("");
    }
    else if (elt.tagName != "INPUT") {
      src.setAttribute("data-from", src.down(".from").getValue());
      src.setAttribute("data-to", src.down(".to").getValue());
      var pp = new kwo.dialogs.PeriodPicker(src);
      pp.onSelect = function(from, to) {
        src.down(".from").setValue(from.toLocaleDate());
        src.down(".to").setValue(to.toLocaleDate());
        pp.close();
      };
    }
  },

  onInputDateClick: function(evt) {
    var src = $(evt.currentTarget);
    var elt = $(evt.target);
    evt.stop();
    if (elt.hasClassName("reset")) {
      src.setAttribute("data-date", "");
      src.down("INPUT").setValue("");
    }
    else if (elt.tagName != "INPUT") {
      src.setAttribute("data-date", src.down("INPUT").getValue());
      var dp = new kwo.dialogs.DatePicker(src);
      dp.onSelect = function(date) {
        src.down("INPUT").setValue(date.toLocaleDate());
        dp.close();
      };
    }
  },

  onInputDatetimeClick: function(evt) {
    var src = $(evt.currentTarget);
    var elt = $(evt.target);
    evt.stop();
    if (elt.hasClassName("reset")) {
      src.setAttribute("data-datetime", "");
      src.down("INPUT").setValue("");
    }
    else if (elt.tagName != "INPUT") {
      src.setAttribute("data-datetime", src.down("INPUT").getValue());
      var dp = new kwo.dialogs.DatetimePicker(src);
      dp.onSelect = function(datetime) {
        src.down("INPUT").setValue(datetime);
        dp.close();
      };
    }
  }

};



kwo.managers.Stackpanel = Class.create({

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

kwo.managers.Discount = Class.create({
  "id": null,
  "type": null,
  "elt": null,

  initialize: function(elt) {
    this.id = $(elt).previous().getAttribute("data-id");
    this.type = $(elt).previous().getAttribute("data-type");
    this.elt = $(elt).up(".discount-composer");
    $(elt).remove();
    var that = this;
    this.elt.observe("container:updated", function(evt) {
      evt.stop();
      var elt = evt.element();
      if (elt.hasClassName("discount-composer")) {
      }
    });
    that.onBind();
    this.elt.fire("container:updated");
  },

  onBind: function() {
    this.elt.observe("click", this.onClick.bindAsEventListener(this));
  },

  onClick: function(evt) {
    evt.stop();
    var that = this;
    var elt = evt.element();
    if (elt.hasClassName("condition-edit") || elt.up().hasClassName("condition-edit")) {
      elt = elt.hasClassName("condition-edit") ? elt : elt.up();
      Kwo.exec("/back/core/discount.condition.edit",
               {"id": this.id, "condition_id": elt.getAttribute("data-id")},
               {"container": this.elt.down(".discount-conditions"),
                "callback": function() {
                  this.down("[type=button]").observe("click", that.onConditionSave.bindAsEventListener(that));
                  this.down("[name=model_id]").observe("change", function(evt) {
                    evt.stop();
                    var elt = evt.element();
                    var form = elt.up("FORM");
                    if (form.down("[name=threshold_type]")) {
                      if (elt.getValue() < 1000) {
                        form.down("[name=threshold_type]").setValue('');
                        form.down("[name=threshold]").setValue('');
                        form.down("[name=threshold_type]").disable();
                        form.down("[name=threshold]").disable();
                      }
                      else {
                        form.down("[name=threshold_type]").enable();
                        form.down("[name=threshold]").enable();
                      }
                    }
                  });
                }});
    }
    else if (elt.hasClassName("action-edit") || elt.up().hasClassName("action-edit")) {
      elt = elt.hasClassName("action-edit") ? elt : elt.up();
      Kwo.exec("/back/core/discount.action.edit", {"id": this.id},
               {"container": this.elt.down(".discount-action"),
                "callback": function() {
                  this.down("[type=button]").observe("click", that.onActionSave.bindAsEventListener(that));
                  if (this.down("[name=target]")) {
                    this.down("[name=target]").observe("change", function(evt) {
                      evt.stop();
                      var elt = evt.element();
                      var form = elt.up("FORM");
                      if (elt.getValue().match(/(shipping|amount)/)) {
                        form.down("[name=threshold_type]").setValue('');
                        form.down("[name=threshold_type]").disable();
                      }
                      else {
                        form.down("[name=threshold_type]").enable();
                      }
                    });
                  }
                }});
    }
    else if (elt.hasClassName("condition-remove")) {
      Kwo.exec("/back/core/discount.condition.remove",
               {"id": this.id, "condition_id": elt.getAttribute("data-id")},
               {"confirm": true,
                "callback": this.onRefresh.bind(this)});
    }
    else if (elt.hasClassName("cancel")) {
      this.onRefresh();
    }

  },

  onConditionSave: function(evt) {
    evt.stop();
    var that = this;
    var args = evt.element().form.serialize(true);
    args["id"] = this.id;
    Kwo.exec("/back/core/discount.condition.save", args,
             {"callback": function (resp) {
               resp = new kwo.Response(resp)
               if (resp.hasError()) return resp.alert();
               that.onRefresh();
             }})

  },

  onActionSave: function(evt) {
    evt.stop();
    var that = this;
    var args = evt.element().form.serialize(true);
    args["id"] = this.id;
    Kwo.exec("/back/core/discount.action.save", args,
             {"callback": function (resp) {
               resp = new kwo.Response(resp)
               if (resp.hasError()) return resp.alert();
               that.onRefresh();
             }})

  },

  onRefresh: function() {
    Kwo.exec("/back/core/discount.composer", {"id": this.id},
             {"container": this.elt});
  }

});

kwo.managers.Date = Class.create({

  container: null,

  initialize: function(elt) {
    this.container = $(elt);
//    $(elt).remove();
    this.onPaint({"date": this.container.getAttribute("data-date")});
  },

  onPaint: function(args) {
    Kwo.exec("/back/core/date.picker", args,
             {"container": this.container,
              "callback": this.onBind.bind(this)})
  },

  down: function(expr) {
    return this.container.down(expr);
  },

  onBind: function() {
    var that = this;
    this.down(".weeks").select("TD[data-date]").each(function(elt) {
      elt.observe("click", that.onDayClick.bindAsEventListener(that));
    });
    this.container.select("SELECT").each(function(elt) {
      elt.observe("change", that.onMonthYearChange.bindAsEventListener(that));
    });
    this.container.select(".nav").each(function(elt) {
      elt.observe("click", that.onNavClick.bindAsEventListener(that));
    });
  },

  onNavClick: function(evt) {
    evt.stop();
    var elt = evt.element();
    var month = this.down(".month").down("SELECT").getValue().intval();
    month = elt.hasClassName("prev") ? month - 1 : month + 1;
    this.onPaint({"month": month,
                  "year": this.down(".year").down("SELECT").getValue()});
  },

  onMonthYearChange: function(evt) {
    evt.stop();
    this.onPaint({"month": this.down(".month").down("SELECT").getValue(),
                  "year": this.down(".year").down("SELECT").getValue()});
  },

  onDayClick: function(evt) {
    evt.stop();
    this.container.select(".selected").invoke("removeClassName", "selected");
    var elt = evt.element();
    elt.addClassName("selected");
    this.onSelect(elt.getAttribute("data-date"));
  },

  onSelect: function(date) {
    console.log(date);
  }

});

kwo.managers.MarkerCluster = Class.create({

  "markers": null,
  "parent": null,
  "model": null,
  "map": null,
  "window": null,

  initialize: function(elt) {
    var that = this;
    this.parent = $(elt).previous();
    $(elt).remove();
    this.model = this.parent.getAttribute("data-model");
    if (!$(document.body).hasClassName("google-maps-loaded")) {
      Kwo.load("/web/core/scripts/markerclusterer_packed.js");
      Kwo.load("http://maps.google.com/maps/api/js?" +
               "sensor=false&language=fr&libraries=geometry&callback=kwo.ux.onGoogleMapsLoaded&language=en");
      $(document).observe("google:maps:loaded", function (evt) {
        that.display();
      });
    }
    else {
      this.display();
    }
  },

  display: function() {
    this.map = new google.maps.Map(this.parent,
                                   {"mapTypeId": google.maps.MapTypeId.ROADMAP,
                                    "scaleControl": false,
                                    "navigationControl": true,
                                    "panControl": false,
                                    "streetViewControl": false,
                                    "zoomControl": true,
                                    "mapTypeControl": false,
                                    "overviewMapControl": false,
                                    "navigationControlOptions": {
                                      "style": google.maps.NavigationControlStyle.SMALL
                                    },
                                    "draggable":true,
                                    "scrollwheel": true});
    Kwo.exec("/back/core/" + this.model + ".markers", {},
             {"callback": this.onMarkersLoaded.bind(this)});
    this.window = new google.maps.InfoWindow();
  },

  onMarkersLoaded: function(resp) {
    resp = new kwo.Response(resp)
    if (resp.hasError()) return resp.alert();
    var bounds, marker, markers=[], pos, n, that = this;
    bounds = new google.maps.LatLngBounds();
    this.markers = resp.getAttribute("markers");
    n = this.markers.length;
    for (var i = 0; i < n; i++) {
      pos = new google.maps.LatLng(that.markers[i]["lat"],
                                   that.markers[i]["lng"]);
      marker = new google.maps.Marker({
        "position": pos,
        "title": that.markers[i]["name"],
//        "icon": 'http://www.google.com/intl/en_ALL/mapfiles/marker' + mags[i]['color'] +'.png',
        "code": that.markers[i]["code"],
        "name": that.markers[i]["name"],
        "image": that.markers[i]["image"]
      });
      google.maps.event.addListener(marker, "click", function(evt) {
        that.window.close();
        that.window.setContent('<div style="height:85px;">' + this["name"] + '</div>');
        that.window.setPosition(this.getPosition());
        that.window.open(that.map);
      });
      markers.push(marker);
      bounds.extend(pos);
    }
    this.map.fitBounds(bounds);
    new MarkerClusterer(this.map, markers, {"gridSize": 40, "maxZoom": 12});
  }

});

// http://code.google.com/apis/maps/documentation/places/supported_types.html
kwo.managers.Geocoder = Class.create({

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

    this.form.observe("submit", this.onSave.bindAsEventListener(this));

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
    var that = this, marker_visible = false;
    this.loaded = true;
    this.geocoder = new google.maps.Geocoder();
    var zoom = 4, lat = 47.9958632, lng = 13.496798000000013;
    if (!this.form.down(".zoom").value.empty()) {
      zoom = parseInt(this.form.down(".zoom").value);
      lat = parseFloat(this.form.down(".latitude").value);
      lng = parseFloat(this.form.down(".longitude").value);
      marker_visible = true;
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
                                          "visible": marker_visible,
                                          "position": new google.maps.LatLng(lat, lng),
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
    var fields = ["admin1", "admin2", "admin3", "country", "country_code",
                  "locality", "sublocality", "postal_code"];
    that = this;
    fields.each(function(field) {
      that.form.down("."+field).value = "";
    });
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
      if (components[i]["types"][0] == "route") {
        this.form.down(".route").value = components[i]["long_name"];
        continue ;
      }
      if (components[i]["types"][0] == "street_number") {
        this.form.down(".street_number").value = components[i]["long_name"];
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

Kwo.CMT = {

  onRemove: function(elt) {
    elt = $(elt);
    Kwo.F("comment").onExec("destroy", {"id": elt.getAttribute("data-id")},
                            {"confirm": elt,
                             "callback": $("comments-button").onclick.bind("comments-button")});
  }

};


//-- File Popup ----------------------------------------------------------------

Kwo.Dir = {

  addFolder: function(elt)   {
    var elt = $(elt);
    var name;
    if (!(name = prompt(elt.getAttribute("data-prompt"), ""))) return ;
    Kwo.exec("/back/core/folder.create", {"path": _path, "name": name},
             {"callback": Kwo.Dir.view});
  },

  addFile: function() {
    $("form_upload").submit();
  },

  compress: function(elt) {
    var elt = $(elt);
    if (!confirm(elt.getAttribute("data-confirm") + "\n" + "<" + _path + "> ?")) return ;
    Kwo.exec("/back/core/folder.compress", {"path": _path},
            {"callback": Kwo.Dir.view});
  },

  filter: function(elt) {
    if (!window.keyword) window.keyword = "";
    var elt = $(elt);
    window.keyword = prompt(elt.getAttribute("data-prompt"), window.keyword);
    Kwo.go("/back/core/folder",
           {"path":_path, "keyword":keyword});
    window.keyword = keyword;
  },

  check: function ()   {
    Kwo.exec("/back/core/file.check",
             {"remote_file_path": $F("remote_file_path"),
              "path": _path},
             {"callback": Kwo.Dir.onCheckCallback});
  },

  onCheckCallback: function (resp) {
    resp = new kwo.Response(resp);
    if (resp.hasError()) return resp.alert();
    if (resp.getAttribute("has_file") == 1 && !confirm(resp.getAttribute("has_file_confirm"))) return ;
    Kwo.Dir.addFile();
  },

  selectFile: function() {
    if (window._file_path.length >= 2) {
      $("file_box", "upload_box").invoke("toggle");
    }
    else {
      $("upload_box").toggle();
    }
  },

  unlink: function(elt) {
    var elt = $(elt);
    if (!confirm(elt.getAttribute("data-confirm"))) return ;
    Kwo.exec("/back/core/folder.remove", {"path": _path},
             {"callback": Kwo.Dir.view});
  },

  view: function(resp) {
    var args = {};
    if (resp === undefined) {
      args["path"] = _path;
      if ($("layout")) {
        args["layout"] = $F("layout");
      }
    }
    else if (Object.isString(resp)) {
      args["path"] = resp;
    }
    else if (Object.isElement(resp)) {
      args["path"] = resp.getAttribute("data-path");
    }
    else {
      resp = new kwo.Response(resp);
      if (resp.hasError()) return resp.alert();
      args["path"] = resp.hasAttribute("path") ? resp.getAttribute("path") : resp.getAttribute("folder_path");
    }
    Kwo.go("/back/core/folder", args);
  },

  refresh: function(resp) {
    resp = new kwo.Response(resp);
    if (resp.hasError()) return resp.alert();
    Kwo.Dir.view();
  }

};

Kwo.File = {

  observe: function(elt) {
    elt = $(elt).previous();
    var dz = new Kwo.Class.DropZone(elt.down(".dropzone"));
    dz.setDestination(elt.getAttribute("data-path"));
    dz.onRequestComplete = function() { Kwo.Dir.view(); };
  },

  download: function() {
    Kwo.go("/back/core/file.download", {"file_path": _file_path});
  },

  edit: function() {
    Kwo.go("/back/core/file.content.edit", {"file_path":_file_path});
  },

  encode: function() {
    Kwo.exec("/back/core/file.encode", {"file_path":_file_path}, {callback: Kwo.File.view});
  },

  move: function(node) {
    if (!confirm(node.getAttribute("title").ucfirst()+" ?")) return ;
    Kwo.exec("/back/core/file.move", {"path": _path});
  },

  rename:  function(elt) {
    var elt = $(elt);
    var new_filename;
    if (new_filename = prompt(elt.readAttribute("data-prompt"), _filename)) {
      if (new_filename != _filename) {
	       Kwo.exec("/back/core/file.rename",
                 {"file_path": _file_path, "new_filename": new_filename},
                 {"callback": Kwo.File.view});
      }
    }
  },

  preview: function(path) {
    var thumb = $("thumb");
    if (Object.isElement(path)) {
      path = path.getAttribute("data-path");
    }
    $("dim").update("");
    $("file_box", "upload_box").invoke("hide");
    $("file_box").show();
    window._file_path = path;
    $("file_path").value = path;
    if (path.indexOf(".jpg") > 0 || path.indexOf(".jpeg") > 0 ||
        path.indexOf(".png") > 0 || path.indexOf(".gif") > 0) {
      thumb.hide();
      var img = new Image();
      img.onload = function() {
        var max = 240;
        $("dim").innerHTML = this.width + "x" + this.height;
        thumb.src = this.src;
        if (this.width <= max && this.height <= max) {
          thumb.setStyle({width: this.width + "px", height: this.height + "px"});
        }
        else {
          if (this.width > max) {
            thumb.setStyle({height: Math.ceil(this.height * (max / this.width)) + "px", width: max + "px"});
          }
        }
        thumb.show();
      };
      img.src = path;
    }
    else {
      thumb.hide();
    }
  },

  resize: function() {
    Kwo.exec("/back/core/img.resize",
             {"file_path": _file_path, "width": $F("width"), "height": $F("height")},
             {"callback": Kwo.File.view});
  },

  rotate: function() {
    var angle = "90";
    if ($("angle180").checked == true) angle = 180;
    Kwo.exec("/back/core/img.rotate",
             {"file_path": _file_path, "angle": angle},
             {"callback": Kwo.File.view});
  },

  select: function() {
    Kwo.exec("/back/core/file.select", {"file_path": _file_path});
  },

  unlink: function(elt) {
    var elt = $(elt);
    if (!confirm(elt.getAttribute("data-confirm"))) return ;
    $("file_box").hide();
    Kwo.exec("/back/core/file.remove", {"filepath": _file_path},
             {"callback": Kwo.Dir.view});
  },

  unzip: function(elt) {
    var elt = $(elt);
    if (!confirm(elt.getAttribute("data-confirm"))) return ;
    Kwo.exec("/back/core/file.unzip", {"file_path": _file_path},
             {"callback": Kwo.Dir.refresh});
  },

  view: function(resp) {
    if (Object.isString(resp)) {
      return Kwo.go("/back/core/file", {"path": resp});
    }
    resp = new kwo.Response(resp);
    if (resp.hasError()) return resp.alert();
    Kwo.File.view(resp.getAttribute("file_path"));
  }

};


//-- Dialogs -------------------------------------------------------------------

kwo.ui.Dialog = Class.create({

  "dimensions": null,
  "elt": null,
  "width": null,
  "height": null,
  "modal": false,
  "name": null,
  "title": null,
  "timeout": null,
  "onescape": null,
  "ondrag": null,
  "overlay": null,
  "support": null,
  "titlebar": null,
  "container": null,

  "ox": null,
  "oy": null,
  "cx": null,
  "cy": null,

  initialize: function(opts) {
    var that = this;
    opts = opts || {};
    this.elt = opts["elt"];
    this.width = "width" in opts ? opts["width"] : 500;
    this.height = "height" in opts ? opts["height"] : 300;
    this.name = "name" in opts ? opts["name"] : "anonymous";
    this.title = "title" in opts ? opts["title"] : "dialog";
    this.modal = "modal" in opts ? opts["modal"] : false;
    kwo.registry.dialogs.push(this);
    if (this.overlay === null) {
      this.overlay = new Element("div", {"class": "overlay"});
      this.support = new Element("div", {"class": "dialog dialog-" + this.name});
      this.titlebar = new Element("div", {"class": "titlebar"});
      this.container = new Element("div", {"class": "container"});
      var cross = new Element("A", {"href": "javascript:;"}).addClassName("close fa fa-times-circle");
      this.titlebar.appendChild(cross);
      this.title = this.title || this.name;
      this.titlebar.appendChild(new Element("h1").update(this.title.ucfirst()));
      this.support.appendChild(this.titlebar);
      this.support.appendChild(this.container);
      this.place();
      document.body.appendChild(this.overlay);
      document.body.appendChild(this.support);
      if (this.modal == true) {
        cross.setOpacity(0);
      }
      else {
        cross.observe("click", this.close.bindAsEventListener(this));
      }
      this.overlay.observe("click", this.close.bindAsEventListener(this));
      if (kwo.registry.dialogs.length == 1) {
        this.onescape = this.onEscape.bindAsEventListener(this);
        document.observe("keyup", this.onescape);
      }
      this.ondrag = this.onDrag.bindAsEventListener(this);
      this.titlebar.observe("mousedown", function(evt) {
        if (evt.element().tagName == "A" || evt.element().tagName == "IMG") return ;
        evt.stop();
        that.dimensions = document.viewport.getDimensions();
        that.support.addClassName("dialog-moving");
        var style = that.support.style;
        that.ox = parseInt(style.left.substr(0, style.left.length - 2));
        that.oy = parseInt(style.top.substr(0, style.top.length - 2));
        that.cx = evt.clientX;
        that.cy = evt.clientY;
        document.observe("mousemove", that.ondrag);
      });
      this.ondrop = this.onDrop.bindAsEventListener(this);
      this.titlebar.observe("mouseup", that.ondrop);
    }
    Event.observe(window, "resize", Kwo.onViewportChange);
    if ("onAfterPlace" in this) {
      this.onAfterPlace();
    }
    cross.focus();
  },

  getElement: function() {
    return this.elt;
  },

  onDrop: function(evt) {
    if (evt) evt.stop();
    if (typeof this.timeout == "number") {
      window.clearTimeout(this.timeout);
    }
    this.support.removeClassName("dialog-moving");
    document.stopObserving("mousemove", this.ondrag);
  },

  onDrag: function(evt) {
    evt.stop();
    var left = (this.ox - (this.cx - evt.clientX));
    var top = (this.oy - (this.cy - evt.clientY));
    if (top < 0) {
      top = 0;
//      this.onDrop();
    }
    if (left < 0) {
      left = 0;
//      this.onDrop();
    }
    if (top > (this.dimensions["height"] - (this.height + 38))) {
      top = this.dimensions["height"] - (this.height + 38);
//      this.onDrop();
    }
    if (left > (this.dimensions["width"] - (this.width + 12))) {
      left = this.dimensions["width"] - (this.width + 12);
//      this.onDrop();
    }
    this.support.style.left = left + "px";
    this.support.style.top = top + "px";
  },

  place: function() {
    var dimensions = document.viewport.getDimensions();
    var offsets = document.viewport.getScrollOffsets();
    var height = this.height;
    if (Object.isString(height) && height.endsWith("%")) {
      height = height.substr(0, height.length - 1);
      height = parseInt(dimensions.height * height / 100);
    }
    var dh = height;

    dh = height + 26;

    var width = this.width;
    if (Object.isString(width) && width.endsWith("%")) {
      width = width.substr(0, width.length - 1);
      width = parseInt(dimensions.width * width / 100);
    }
    var left = parseInt(offsets.left + ((dimensions.width - width)/ 2));
    var top = parseInt(offsets.top + ((dimensions.height - dh)/ 2));
    top = top < 0 ? 0 : top;
    left = left < 0 ? 0 : left;
    var oh = dimensions.height;
/*    if (Prototype.Browser.Gecko) {
      oh = document.documentElement.scrollHeight;
    } */
/*    this.overlay.setStyle({"width": dimensions.width + "px",
                           "height": oh + "px",
                           "left": offsets.left + "px",
                           "top": offsets.top + "px"});*/
    this.support.setStyle({"width": width + "px",
                          "height": dh + "px",
                          "left": left + "px",
                          "top": top + "px"});
    this.container.setStyle({"height": height + "px",
                             "width": width + "px"});
  },

  onEscape: function (evt) {
    evt.stop();
    if (evt.keyCode == Event.KEY_ESC) {
      var dialog = kwo.registry.dialogs[kwo.registry.dialogs.length - 1];
      if (dialog.modal != true) {
        dialog.close();
      }
    }
  },

  close: function(evt) {
    if (evt instanceof Event) {
      evt.stop();
    }
    if (Object.isFunction(this.onBeforeClose)) {
      this.onBeforeClose();
    }
    this.support.remove();
    this.overlay.remove();
    if (Object.isFunction(this.onAfterClose)) {
      this.onAfterClose();
    }
    if (kwo.registry.dialogs.length == 1) {
      document.stopObserving("keyup", this.onescape);
    }
    document.stopObserving("mousemove", this.ondrag);

    var dialog = kwo.registry.dialogs.pop();
    dialog = null;
  },

  setContent: function(html) {
    this.container.update(html);
  },

  setTitle: function(title) {
    this.support.down("H1").update(title.ucfirst());
  },

  resizeTo: function() {
  }

});

kwo.dialogs.BatchManager = Class.create(kwo.ui.Dialog, {

/*"opts": null,
  "panel": null,
  "details": null,
  "actions": null,
  "dropzone": null, */

  is_download: null,
  model: null,
  items: null,
  finder: null,
  confirm: null,
  action: null,
  wrapper: null,
  controls: null,
  submitted: false,

  initialize: function($super, elt, model, items, finder) {
    this.items = items || [];
    this.model = model;
    this.finder = finder;
    $super({"elt": $(elt),
            "name": "batch-manager",
            "title": _dict["dialogs"]["batch-manager"],
            "width": 980,
            "height": 480});
    this.paint();
  },

  paint: function() {
    var args = {model: this.model,
                items: this.items.join(","),
                finder: this.finder};
    Kwo.exec("/back/core/batch.manager", args,
             {"container": this.container,
              "callback": this.observe.bind(this)});
  },

  observe: function() {
    var that = this;
    this.wrapper = this.container.down(".batch-wrapper");
    this.controls = this.container.down(".batch-controls");
    this.controls.down(".batch-submit").observe("click", function(evt) {
      evt.stop();
      if (!that.container.down("form")) return kwo.alert("invalid form");
      if (!that.action) return kwo.alert("invalid action");
      if (that.confirm) {
        kwo.confirm(that.confirm, function() {
          that.submit();
        });
        return ;
      }
      that.submit();
    });
    if (this.elt.hasAttribute("data-manager")) {
      var manager = new kwo.str2func(this.elt.getAttribute("data-manager"))(this);
      return ;
    }
    var action = this.elt.getAttribute("data-action");
    if (action == "destroy") {
      this.action = "/back/core/" + this.model + ".destroy";
      this.confirm = this.elt.readAttribute("data-confirm");
      this.setDestroyForm();
    }
    else if (action == "export") {
      this.action = "/back/core/" + this.model + ".export";
      this.is_download = true;
      this.setVoidForm();
    }
    else if (action == "validate") {
      this.action = "/back/core/" + this.model + ".validate";
      this.setVoidForm();
    }
  },

  setDestroyForm: function() {
    this.wrapper.update("<form>" + this.wrapper.getAttribute("data-warning") + "</form>");
    this.controls.down(".items-found").up("label").remove();
  },

  setVoidForm: function() {
    this.wrapper.update("<form></form>");
  },

  isSelection: function() {
    return this.controls.down(".items-selected") && this.controls.down(".items-selected").checked == true;
  },

  getEltCount: function() {
    if (this.controls.down(".items-selected") && this.controls.down(".items-selected").checked == true) {
      return this.controls.down(".items-selected").getAttribute("data-count").intval();
    }
    return this.controls.down(".items-found").getAttribute("data-count").intval();
  },

  getArgs: function() {
    var args = this.wrapper.down("form").serialize(true);
    if (this.isSelection()) {
      args["items"] = this.items.join(",");
    }
    else {
      args["finder"] = this.finder;
    }
    args["model"] = this.model;
    return args;
  },

  submit: function() {
    var that = this;
    if (this.is_download) {
      Kwo.go(this.action, this.getArgs());
      return ;
    }
    kwo.post(this.action, this.getArgs(),
             {"callback": function(resp) {
               resp = new kwo.Response(resp);
               if (resp.hasError()) return resp.alert();
               that.submitted = true;
               var msg = "OK";
               if (resp.hasMessage()) msg = resp.getMessage();
               that.wrapper.update('<div class="batch-response">' + msg + '</div>');
               that.controls.update();
             }});
  }

});

kwo.dialogs.ImageManager = Class.create(kwo.ui.Dialog, {

  initialize: function($super, elt) {
    $super({"elt": $(elt),
            "name": "image",
            "title": _dict["dialogs"]["image"],
            "width": 900,
            "height": 450});
    this.onDisplay();
  },

  onDisplay: function() {
    Kwo.exec("/back/core/editor.image", {},
             {"container": this.container,
              "callback": this.onBind.bind(this)});
  },

  onBind: function() {
    var that = this;
    var panel = new Kwo.Class.FilePanel($("file-panel"),
                                        {"path": "",
                                         "filter": "image"});
    panel.onFileSelect = function(file) {
      $("image-preview").src = "/" + file["path"];
      $("image-path").value = "/" + file["path"];
      $("image-select-button").show();
    };
    $("image-select-button").observe("click", function (evt) {
      var args = {"path": $("image-path").value,
                  "alt": $("image-alt").value.replace('"', "'"),
                  "width": $("image-width").value,
                  "height": $("image-height").value,
                  "float": $("image-float").value};
      that.onSelect(args);
    });
    this.dropzone = new Kwo.Class.DropZone(this.container.down(".dropzone"));
    this.dropzone.input.accept = "image/*";
    this.dropzone.onRequestComplete = panel.onRefresh.bind(panel);
    this.dropzone.onBeforeUpload = function() {
      that.dropzone.setDestination(panel.getPath());
    };
    $("create-folder-button").observe("click", function (evt) {
      var dirname = prompt("Nom du dossier :");
      if (!dirname || dirname.length <= 1) return ;
      Kwo.exec("/back/core/folder.create",
               {"path": panel.getPath(),
                "name": dirname},
               {"callback": function (resp) {
                 resp = new kwo.Response(resp);
                 if (resp.hasError()) return resp.alert();
                 panel.onDirSelect(resp.getAttribute("folder_path"));
               }});

    });
  },

  onSelect: function(file_path) {
    console.log(file_path);
    this.close();
  }

});

kwo.dialogs.CallbackPanel = Class.create(kwo.ui.Dialog, {

  initialize: function($super, elt) {
    $super({"elt": $(elt),
            "name": "callback-panel",
            "title": _dict["dialogs"]["callback-panel"],
            "width": 500,
            "height": 480});
  },

  update: function(resp) {
    var html = ".";
    resp = new kwo.Response(resp);
    if (resp.hasError()) {
      html = "<p class='error'>ERROR</p>" + resp.getError("<br/>");
    }
    else if (resp.hasMessage()) {
      html = resp.getMessage();
    }
    this.container.update(html);
  }

});

kwo.dialogs.FolderManager = Class.create(kwo.ui.Dialog, {

  "file": null,

  initialize: function($super, elt) {
    $super({"elt": $(elt),
            "name": "folder-manager",
            "title": _dict["dialogs"]["folder-manager"],
            "width": 900,
            "height": 400});
    this.onDisplay();
  },

  onDisplay: function() {
    Kwo.exec("/back/core/folder.picker", null,
             {"container": this.container,
              "callback": this.onBind.bind(this)});
  },

  onBind: function() {
    var that = this;
    var panel = new Kwo.Class.FilePanel($("folder-panel"),
                                        {"path": $F(this.elt)});
    panel.onDirSelect = function(file) {
      that.file = file;
      Kwo.exec("/back/core/file.panel",
               {"path": that.file,
                "filter": "directory"},
               {"callback": that.onCallback.curry(panel).bind(that)});
    };
    panel.onFileSelect = Prototype.emptyFunction;
    $("apply-button").observe("click", function (evt) {
      that.onSelect({"path": "/" + that.file});
    });
    $("create-folder-button").observe("click", function (evt) {
      var dirname = prompt($("create-folder-button").getAttribute("data-prompt") + " :");
      if (dirname && dirname.length > 0) {
        var args = {"path": that.file, "name": dirname};
        Kwo.exec("/back/core/folder.create", args,
                 {"callback": function (resp) {
                   resp = new kwo.Response(resp);
                   if (resp.hasError()) return resp.alert();
                   panel.onDirSelect(args["path"] + "/" + args["name"]);
                 }});
      }
    });
    panel.onDirSelect($F(this.elt));
  },

  onCallback: function(panel, resp) {
    panel.onCallback(resp);
    Kwo.exec("/back/core/folder.details",
             {"path": this.file},
             {"container": $('folder-details'),
              "callback": Element.show.curry($("apply-button"))});
  },

  onSelect: function(file) {
    if (Object.isElement(this.elt)) {
      this.elt.setValue(file.path);
    }
    else {
      console.log(file.path);
    }
    this.close();
  }

});

kwo.dialogs.ItemPicker = Class.create(kwo.ui.Dialog, {

  "elt": null,
  "item": null,
  "models": null,
  "caller_model": null, // nécessaire qd on ouvre depuis une model instance
  "selection": null,

  initialize: function($super, elt) {
    elt = $(elt);
    if (elt.hasAttribute("data-model")) {
      this.models = elt.getAttribute("data-model");
    }
    else {
      this.models = elt.getAttribute("data-models");
    }
    this.selection = elt.getAttribute("data-selection"); // single , multiple
    this.item = elt.getAttribute("data-caller-item");
    this.caller_model = elt.getAttribute("data-caller-model");
    this.value = elt.getAttribute("data-value");
    $super({"elt": elt,
            "name": "item-picker",
            "title": _dict["dialogs"]["item-picker"],
            "width": 400,
            "height": 500});
    this.onDisplay();
    this.onObserve();
  },

  onObserve: function() {
    var that = this;
    this.container.observe("item:pick", function(evt) {
      evt.stop();
      that.onPick(evt.memo);
    });
  },

  onPick: function(args) {
    var that = this;
    Kwo.exec("/back/core/item.pick", args,
             {"callback": function(resp) {
               resp = new kwo.Response(resp);
               if (resp.hasError()) return alert("item error");
               var items = resp.getAttribute("items");
               that.onSelect(items);
             }});
  },

  onDisplay: function(args) {
    args = args || {}
    args["models"] = this.models;
    args["caller_model"] = this.caller_model;
    args["item"] = this.item;
    args["selection"] = this.selection;
    args = kwo.tool.Contextualize(this.elt, args);
    Kwo.exec("/back/core/item.picker", args,
             {"container": this.container,
              "callback": this.onBind.bind(this)});
  },

  onBind: function() {
    if (!(this.container.down(".nav") && this.container.down(".items"))) {
      return ;
    }
    var that = this;
    var form = this.container.down("FORM");
    form.observe("submit", function(evt) {
      evt.stop();
      that.onSearch();
    });
    this.container.down("SELECT").observe("change", function(evt) {
      evt.stop();
      that.onSearch();
    });
    if (this.container.down("UL")) {
      this.container.down("UL").observe("click", function(evt) {
        var elt = evt.element();
        if (elt.tagName == "INPUT") {
          that.container.down(".select-button").show();
        }
        else {
          evt.stop();
          elt = elt.tagName == "LI" ? elt : elt.up("LI");
          if (that.selection == "multiple") {
            elt.down("input").checked = true;
            that.container.down(".select-button").show();
          }
          else {
            var args = {"model_id": elt.getAttribute("data-model-id"),
                        "record_id": elt.getAttribute("data-id")};
            that.onPick(args);
          }
        }
      });
    }
    if (this.container.down(".select-button")) {
      this.container.down(".select-button").observe("click", function(evt) {
        evt.stop();
        var args = {"items": []};
        that.container.select("input[type=checkbox]").each(function(elt) {
          if (elt.checked == true) args["items"].push(elt.up("li").getAttribute("data-item"));
        });
        args["items"] = args["items"].join(",");
        console.log(args);
        that.onPick(args);
      });
    }
    if (this.container.down(".pagination")) {
      this.container.down(".pagination").observe("click", function(evt) {
        evt.stop();
        var elt = evt.element();
        if (elt.tagName == "A") {
          that.container.down("INPUT[name=page]").value = elt.getAttribute("data-page");
          that.onSearch();
        }
      });
    }
    this.container.down(".item-search-button").observe("click", function(evt) {
      evt.stop();
      that.onSearch();
    });
    if (this.container.down(".item-editor-button")) {
      this.container.down(".item-editor-button").observe("click", function(evt) {
        evt.stop();
        this.setAttribute("data-model", that.getModel());
	      this.setAttribute("data-caller-item", that.item);
        var editor = new kwo.dialogs.ItemEditor(this);
        var item = {};
        editor.onBeforeClose = function() {
          var datas = this.container.down("DIV.data-item");
          var args = {"model_id": datas.getAttribute("data-model-id"),
                      "record_id": datas.getAttribute("data-id")};
          if (args["record_id"].intval() < 1) return ;
          that.onPick(args);
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
    return this.container.down("SELECT").getValue();
  },

  onSearch: function() {
    var args = {};
    args["query"] = this.container.down("INPUT[name=query]").getValue();
    args["model"] = this.container.down("SELECT").getValue();
    args["page"] = this.container.down("INPUT[name=page]").getValue();
    this.onDisplay(args);
  },

  onSelect: function(items) {
    if (this.elt.hasClassName("item-picker-button")) {
      var item = items[0];
      this.elt.up("DIV").down("INPUT.input-record-id").value = item["record_id"];
      var name = item["name"];
      if (this.elt.up("DIV").down("INPUT.input-model-id")) {
        this.elt.up("DIV").down("INPUT.input-model-id").value = item["model_id"];
        name = item["model"] + ": " + name;
      }
      this.elt.up("DIV").down("INPUT[type=text]").value = name;
      this.elt.up("DIV").down("INPUT[type=text]").setAttribute("data-item", item["key"]);
    }
    else {
      console.log(items);
    }
    this.close();
  }

});

kwo.dialogs.ItemEditor = Class.create(kwo.ui.Dialog, {

  "item": null,
  "caller_item": null,

  initialize: function($super, elt) {
    elt = $(elt);
    $super({"elt": elt,
            "name": "item-editor",
            "title": _dict["dialogs"]["item-editor"],
            "width": "94%",
            "height": 620});
    this.onDisplay();
  },

  getCallerItem: function() {
    return this.caller_item || "";
  },

  getItem: function() {
    return this.item || "";
  },

  onDisplay: function() {
    var model;
    var args = kwo.tool.Contextualize(this.elt);
    if (this.elt.hasAttribute("data-item")) {
      model = kwo.tool.model(this.elt);
      args["item"] = this.elt.getAttribute("data-item");
      this.item = this.elt.getAttribute("data-item");
    }
    else if (this.elt.hasAttribute("data-record-id")) {
      args["id"] = this.elt.getAttribute("data-record-id");
    }
    if (this.elt.hasAttribute("data-model")) {
      model = this.elt.getAttribute("data-model");
    }
    var url = "/back/core/" + model + ".edit";
    if (this.elt.hasAttribute("data-caller-model")) {
      args["caller_model"] = this.elt.getAttribute("data-caller-model")
    }
    if (this.elt.hasAttribute("data-caller-item")) {
      this.caller_item = this.elt.getAttribute("data-caller-item")
      args["caller_item"] = this.elt.getAttribute("data-caller-item")
    }
    this.container.setStyle({"padding": "4px 6px 0 6px"});
    this.container.addClassName(model + "-box");
    Kwo.exec(url, args, {"container": this.container})
  }

});

kwo.dialogs.BitmapEditor = Class.create(kwo.ui.Dialog, {

  "filepath": null,

  initialize: function($super, elt) {
    elt = $(elt);
    this.filepath = elt.src;
    $super({"elt": elt,
            "name": "bitmap-editor",
            "title": _dict["dialogs"]["bitmap-editor"],
            "width": "90%",
            "height": 580});
    this.onDisplay();
  },

  onDisplay: function() {
    this.container.setStyle({"overflow": "hidden"});
    this.container.update('<iframe src="javascript:;" style="width:100%; height:100%;"></iframe>');
    this.container.down("IFRAME").src = "/back/core/bitmap.editor?filepath=" + this.filepath;
  },

  onAfterClose: function() {
    this.elt.src = this.filepath + "?t=" + Math.random();
  }

});

kwo.dialogs.PeriodPicker = Class.create(kwo.ui.Dialog, {

  "from": "",
  "to": "",
  "input_from": null,
  "input_to": null,

  initialize: function($super, elt) {
    elt = $(elt);
    if (elt.hasAttribute("data-from")) {
      this.from = elt.getAttribute("data-from");
      this.to = elt.getAttribute("data-to");
    }
    else if (Object.isElement(elt) && elt.tagName == "INPUT") {
      var val = elt.getValue();
      console.log(val, val.length);
      if (val.length == 21) {
        var pair = val.split(" ");
        this.from = pair[0];
        this.to = pair[1];
      }
    }
    $super({"elt": elt,
            "name": "period-picker",
            "title": _dict["dialogs"]["period-picker"],
            "height": 280});
    this.onDisplay();
  },

  onDisplay: function() {
    Kwo.exec("/back/core/period.picker", {"from": this.from, "to": this.to},
             {"container": this.container,
              "callback": this.onBind.bind(this)});
  },

  onBind: function() {
    var that = this;
    this.input_from = this.container.down(".from").down("INPUT");
    this.input_to = this.container.down(".to").down("INPUT");
    var from = new kwo.managers.Date(this.container.down(".from").down());
    from.onSelect = function(date) {
      that.input_from.setValue(date.toLocaleDate());
    }
    var to = new kwo.managers.Date(this.container.down(".to").down());
    to.onSelect = function(date) {
      that.input_to.setValue(date.toLocaleDate());
    }
    this.container.down("INPUT[type=button]").observe("click", function(evt) {
      evt.stop();
      var elt = evt.element();
      var from = that.input_from.getValue();
      var to = that.input_to.getValue();
      if (from.indexOf('/')) {
        from = from.split('/');
        from_value = from[2] + "" + from[1] + "" + from[0];
        from = from[2] + "-" + from[1] + "-" + from[0];
        to = to.split('/');
        to_value = to[2] + "" + to[1] + "" + to[0];
        to = to[2] + "-" + to[1] + "-" + to[0];
      }
      else {
        from = from.split('-');
        from_value = from[0] + "" + from[1] + "" + from[2];
        from = from[0] + "-" + from[1] + "-" + from[2];
        to = to.split('-');
        to_value = to[0] + "" + to[1] + "" + to[2];
        to = to[0] + "-" + to[1] + "-" + to[2];
      }
      if (from_value > to_value) {
        from = to;
        to = from;
      }
      if (from.length > 7 && to.length > 7) {
        that.onSelect(from, to);
      }
    });
  },

  onSelect: function(from, to) {
    if (Object.isElement(this.elt) && this.elt.tagName == "INPUT") {
      this.elt.setValue(from + " " + to);
    }
    else {
      console.log(from, to);
    }
    this.close();
  }

});

kwo.dialogs.DatetimePicker = Class.create(kwo.ui.Dialog, {

  initialize: function($super, elt) {
    $super({"elt": $(elt),
            "name": "datetime-picker",
            "title": _dict["dialogs"]["datetime-picker"],
            "width": 260,
            "height": 230});
    this.onDisplay();
  },

  onDisplay: function() {
    var that = this;
    var wrapper = new Element("div", {"class": "wrapper"});
    var date = "", hour = "", minute = "";
    var datetime = this.elt.getAttribute("data-datetime");
    if (datetime && datetime.length > 5) {
      var parts = datetime.split(" ");
      date = parts[0];
      if (parts[1] && parts[1].length >= 3) {
        parts = parts[1].split(":");
        hour = parts[0];
        minute = parts[1];
      }
    }
    wrapper.setAttribute("data-date", datetime);
    this.container.appendChild(wrapper);
    var controls = new Element("div", {"class": "controls"});
    this.container.appendChild(controls);
    var button = new Element("input", {"type": "button", "value": "sélectionner"});
    controls.appendChild(button);
    var input_date = new Element("input", {"type": "text", "class": "date", "value": date.toLocaleDate()});
    controls.appendChild(input_date);
    var input_hour = new Element("input", {"type": "text", "class": "hour", "placeholder": "hh", "value": hour});
    controls.appendChild(input_hour);
    var input_minute = new Element("input", {"type": "text", "class": "minute", "placeholder": "mm", "value": minute});
    controls.appendChild(input_minute);
    var dm = new kwo.managers.Date(wrapper);
    dm.onSelect = function(date) {
      controls.down(".date").setValue(date.toLocaleDate());
    };
    button.observe("click", function(evt) {
      evt.stop();
      that.onSelect(input_date.getValue() + " " +
                    input_hour.getValue() + ":" +
                    input_minute.getValue());
    })
  },

  onSelect: function(date) {
    console.log(date);
    this.close();
  }

});

kwo.dialogs.DatePicker = Class.create(kwo.ui.Dialog, {

  initialize: function($super, elt) {
    $super({"elt": elt,
            "name": "date-picker",
            "title": _dict["dialogs"]["date-picker"],
            "width": 260,
            "height": 192});
    this.onDisplay();
  },

  onDisplay: function() {
    var that = this;
    var wrapper = new Element("div", {"class": "wrapper"});
    wrapper.setAttribute("data-date", this.elt.getAttribute("data-date"));
    this.container.appendChild(wrapper);
    var dm = new kwo.managers.Date(wrapper);
    dm.onSelect = function(date) {
      that.onSelect(date);
    };
  },

  onSelect: function(date) {
    if (Object.isElement(this.elt) && this.elt.tagName == "INPUT") {
      this.elt.setValue(date);
    }
    else {
      console.log(date);
    }
    this.close();
  }

});

kwo.dialogs.Preview = Class.create(kwo.ui.Dialog, {

  "args": null,

  initialize: function($super, elt) {
    elt = $(elt);
    this.args = {};
    if (elt.hasAttribute("data-item")) {
      this.args["item"] = elt.getAttribute("data-item");
    }
    else {
      this.args["url"] = elt.getAttribute("data-url");
    }
    $super({"elt": elt,
            "name": "preview",
            "title": _dict["dialogs"]["preview"],
            "width": 1100,
            "height": 580});
    this.onDisplay();
  },

  onDisplay: function() {
    Kwo.exec("/back/core/item.preview", this.args,
             {"container": this.container,
              "callback": this.onLoad.bind(this)});
  },

  onLoad: function() {
    this.container.setStyle({"overflow": "hidden"});
    var iframe = this.support.down("IFRAME");
    iframe.src = iframe.getAttribute("data-url");
  }

});

kwo.dialogs.User = Class.create(kwo.ui.Dialog, {

  "args": null,

  initialize: function($super, elt) {
    elt = $(elt);
    this.args = {"id": elt.getAttribute("data-id")};
    $super({"elt": elt,
            "name": "user",
            "title": "user",
            "width": 800,
            "height": 520});
    this.onDisplay();
  },

  onDisplay: function() {
    Kwo.exec("/back/core/user.summary", this.args,
             {"container": this.container});
  }

});

kwo.dialogs.Visit = Class.create(kwo.ui.Dialog, {

  "id": null,

  initialize: function($super, elt) {
    var width = 1000;
    width -= ($(document.body).select(".dialog-visit").size() - 1) * 40;
    $super({"elt": $(elt),
            "name": "visit",
            "title": _dict["dialogs"]["visit"],
            "width": width,
            "height": 480});
    this.onDisplay(elt.getAttribute("data-visit-id"));
  },

  onDisplay: function(id) {
    this.id = id;
    Kwo.exec("/back/core/visit", {"id": id},
             {"container": this.container,
              "callback": this.onBind.bind(this)});
  },

  onBind: function() {
    this.container.down(".secondary .panels").setStyle({height: this.height + "px"});
    var that = this;
    this.container.select(".nav span").each(function(elt) {
      elt.observe("click", function(evt) {
        evt.stop();
        that.onDisplay(evt.element().getAttribute("data-visit-id"));
      });
    });

    this.container.select(".button-siblings").each(function(elt) {
      elt.observe("click", function(evt) {
        evt.stop();
        that.onSiblings(elt);
      });
    });

    if (this.container.down(".button-path")) {
      this.container.down(".button-path").observe("click", function(evt) {
        evt.stop();
        that.onPath();
      });
    }

    if (this.container.down(".button-ripe")) {
      this.container.down(".button-ripe").observe("click", function(evt) {
        evt.stop();
        that.onRipe();
      });
    }

  },

  onSiblings: function(elt) {
    var that = this;
    var dest = this.showPanel("siblings");
    var args = {"id": this.id};
    if (elt.hasAttribute("data-type")) {
      args["type"] = elt.getAttribute("data-type");
    }
    Kwo.exec("/back/core/visit.siblings", args,
             {"container": dest,
              "callback": function() {
                that.container.down(".panel-siblings").select("[data-visit-id]").each(function(elt) {
                  elt.observe("click", function(evt) {
                    evt.stop();
                    new kwo.dialogs.Visit(evt.element());
                  });
                })
              }});
  },

  onPath: function() {
    this.showPanel("path");
  },

  onRipe: function() {
    var dest = this.showPanel("ripe");
    var args = {"id": this.id};
    Kwo.exec("/back/core/visit.ripe", args,
             {"container": dest});
  },

  showPanel: function(name) {
    this.container.select(".panel").invoke("hide");
    return this.container.down(".panel-" + name).show();
  }

});

kwo.dialogs.Confirm = Class.create(kwo.ui.Dialog, {

  initialize: function($super, msg) {
    $super({"elt": $(document),
            "modal": true,
            "name": "confirm",
            "title": _dict["dialogs"]["confirm"],
            "width": 400,
            "height": 120});
    this.paint();
    this.setMessage(msg);
  },

  paint: function() {
    this.container.update("<div class='message'></div>" +
                          "<div class='controls'>" +
                          "<input type='button' value='" + _dict["buttons"]["accept"].ucfirst() + "' class='accept' />" +
                          "<input type='button' value='" + _dict["buttons"]["cancel"].ucfirst() + "' class='cancel' />" +
                          "</div>");
    this.observe();
  },

  observe: function() {
    var that = this;
    this.container.down("input.accept").observe("click", function(evt) {
      evt.stop();
      that.onAccept();
      that.close();
    });
    this.container.down("input.accept").focus();
    this.container.down("input.cancel").observe("click", function(evt) {
      evt.stop();
      that.onCancel();
      that.close();
    });
  },

  setMessage: function(msg) {
    this.container.down(".message").update(msg.ucfirst());
  },

  onAccept: function() {
    console.log("accept");
  },

  onCancel: function() {
    console.log("cancel");
  }

});

kwo.dialogs.Alert = Class.create(kwo.ui.Dialog, {

  opts: null,

  initialize: function($super, msg, opts) {
    this.opts = opts || {};
    $super({"elt": $(document),
            "modal": true,
            "name": "alert",
            "title": _dict["dialogs"]["alert"],
            "width": 400,
            "height": 120});
    this.paint();
    this.setMessage(msg);
  },

  paint: function() {
    var classes = "";
    if ("kind" in this.opts) {
      classes += "kind-" + this.opts["kind"];
    }
    this.container.update("<div class='message " + classes + "'></div>" +
                          "<div class='controls " + classes + "'>" +
                          "<input type='button' value='" + _dict["buttons"]["close"].ucfirst() + "' class='ok' />" +
                          "</div>");
    this.observe();
  },

  observe: function() {
    var that = this;
    var input = this.container.down("input[type=button]")
    input.observe("click", function(evt) {
      evt.stop();
      that.close();
    });
    input.focus();
  },

  setMessage: function(msg) {
    var msg = msg || "";
    this.container.down(".message").update(msg.ucfirst());
  }

});

kwo.dialogs.TranslationManager = Class.create(kwo.ui.Dialog, {

  "item": null,
  "model": null,

  initialize: function($super, elt) {
    elt = $(elt);
    this.item = elt.getAttribute("data-item");
    this.model = kwo.tool.model(this.item);
    $super({"elt": elt,
            "name": "translation-manager",
            "title": this.model.ucfirst() + ": " + _dict["dialogs"]["translation-manager"],
            "width": 1000,
            "height": 600});
    this.render();
  },

  render: function() {
    Kwo.exec("/back/core/item.translations", {"item": this.item},
             {"container": this.container,
              "callback": this.observe.bind(this)});
  },

  observe: function() {
    var that = this;
    this.container.down("FORM").observe("submit", function(evt) {
      evt.stop();
      Kwo.exec("/back/core/item.translations.store", [this, {"item": that.item} ],
               {"callback": that.onSubmitCallback.bind(that)});
    });
  },

  onSubmitCallback: function(resp) {
    resp = new kwo.Response(resp);
    if (resp.hasError()) return resp.alert();
    this.render();
    S(this.model).notify("ok");
  }

});


kwo.dialogs.FileManager = Class.create(kwo.ui.Dialog, {

  "opts": null,
  "panel": null,
  "details": null,
  "actions": null,
  "dropzone": null,

  initialize: function($super, elt, opts) {
    this.opts = opts || {};
    $super({"elt": $(elt),
            "name": "file-manager",
            "title": _dict["dialogs"]["file-manager"],
            "width": 980,
            "height": 480});
    this.onDisplay();
  },

  onDisplay: function() {
    Kwo.exec("/back/core/file.manager", null,
             {"container": this.container,
              "callback": this.onBind.bind(this)});
  },

  getPath: function() {
    return this.panel.getPath();
  },

  onBind: function() {
    var that = this;

    this.details = this.container.down("TD.details");
    this.details.setStyle({"height": this.height + "px"});
    this.actions = this.container.down("TD.actions");
    this.dropzone = new Kwo.Class.DropZone(this.container.down(".dropzone"));

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
    else if ("path" in this.elt) {
      path = this.elt["path"];
    }

    this.panel = new Kwo.Class.FilePanel($("file-panel"),
                                         {"path": path,
                                          "filter": this.opts["filter"]});
    this.panel.onFileSelect = this.onFilePreview.bind(this);

    $("create-folder-button").observe("click", function (evt) {
      var dirname = prompt($("create-folder-button").getAttribute("data-prompt") + " :");
      if (!dirname || dirname.length <= 1) return ;
      Kwo.exec("/back/core/folder.create",
               {"path": that.getPath(),
                "name": dirname},
               {"callback": function (resp) {
                 resp = new kwo.Response(resp);
                 if (resp.hasError()) return resp.alert();
                 that.panel.onDirSelect(resp.getAttribute("folder_path"));
                 that.onClean();
               }});
    });

    this.dropzone.onRequestComplete = this.panel.onRefresh.bind(this.panel);
    this.dropzone.onBeforeUpload = function() {
      that.dropzone.setDestination(that.panel.getPath());
    };
    if (this.opts["filter"] == "image") {
      this.dropzone.input.accept = "image/*";
    }

    this.details.observe("click", function(evt) {
      var elt = evt.element();
      var action = elt.getAttribute("data-action");
      var filepath = elt.getAttribute("data-filepath");
      if (action == "select") {
        filepath = "/" + filepath;
        that.onSelect(filepath);
      }
      else if (action == "remove") {
        that.onRemove(elt);
      }
      else if (action == "download") {
        Kwo.go("/back/core/file.download",
               {"filepath": elt.getAttribute("data-filepath")});
      }
    });

  },

  onRemove: function(elt) {
    var that = this;
    Kwo.exec("/back/core/file.remove",
             {"filepath": elt.getAttribute("data-filepath")},
             {"confirm": elt,
              "callback": function(resp) {
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
        this.elt.src = file_path;
        var previous = this.elt.previous();
        if (!Object.isUndefined(previous) && previous.tagName == "INPUT") {
          previous.setValue(file_path);
          previous.enable();
        }
      }
      else if (this.elt.tagName == "INPUT") {
        this.elt.setValue(file_path);
      }
      else {
        console.log(file_path);
      }
    }
    this.close();
  }

});


/*Kwo.ACL = {

  onAdd: function(elt) {
    elt = $(elt);
    var model = elt.up("FIELDSET").getAttribute("data-model");
    Kwo.F(model).onExec("acl.add", elt,
                        {"callback": $(model + "-acl-button").onclick.bind($(model + "-acl-button"))})
  },

  onRemove: function(elt) {
    elt = $(elt);
    var model = elt.up("FIELDSET").getAttribute("data-model");
    Kwo.F(model).onExec("acl.remove", {"user_id": elt.readAttribute("data-id")},
                        {"confirm": elt,
                         "callback": $(model + "-acl-button").onclick.bind($(model + "-acl-button"))})
  },

  onUserRemove: function(elt) {
    elt = $(elt);
    Kwo.F("user").onExec("acl.remove", {"item": elt.getAttribute("data-item")},
                         {"confirm": elt,
                          "callback": $("acl-button").onclick.bind($("acl-button"))})
  }

};*/

var ScrollSort = Class.create({
  elt: null,
  clone: null,
  callback: null,
  drag_callback: null,
  over_callback: null,
  end_callback: null,
  initialize: function (elt, callback, opts) {
    this.elt = elt;
    this.callback = callback;
    this.drag_callback = this.drag.bindAsEventListener(this);
    this.over_callback = this.over.bindAsEventListener(this);
    this.end_callback = this.end.bindAsEventListener(this);
    $(this.elt).select('li .handle').invoke('observe', 'dragstart', this.drag_callback);
    $(this.elt).select('li').invoke('observe', 'dragend', this.end_callback);
    $(this.elt).select('li').invoke('observe', 'dragenter', this.over_callback);
  },
  drag: function (event) {
    var pos = $(event.target).viewportOffset();
    this.from = $(event.target).up('li');
    event.dataTransfer.setData('Text', '');
    event.dataTransfer.setDragImage(this.from, event.clientX - pos.left, event.clientY - pos.top);
    $(this.from).addClassName('dragged');
    $(this.from).stopObserving('dragenter', this.over_callback);
    return true;
  },
  over: function (event) {
    target = event.currentTarget;
    if ($(target).tagName != "LI") {
      try { target = $(target).up('li'); }
      catch (e) { console.log(event) }
    }
    var pos = $(target).viewportOffset();
    if (pos.top + $(target).measure('height') / 2 < event.clientY) {
      $(target).insert({'before': this.from});
    } else {
      $(target).insert({'after': this.from});
    }
  },
  end: function (event) {
    $(this.from).observe('dragenter', this.over_callback);
    $(this.from).removeClassName('dragged');
    this.callback();
  }
});
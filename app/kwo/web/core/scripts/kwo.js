
/**
globals
_auth_dialog_width
_auth_dialog_height
**/

if (window["console"] === undefined) {
  window.console = {"log": Prototype.emptyFunction};
}

var kwo = {

  "dialogs": {}, // composers | pickers | speakers
  "managers": {},
  "widgets": {},
  "platform": {"models":{}},
  "registry": {"dialogs":[]},
  "events": {},
  "ux": {},
  "ui": {},
  "tool": {},
  "vars": {},

  "get": function(action, args, options) {
    options = options || {};
    options["method"] = "get";
    return Kwo.exec(action, args, options);
  },

  "post": function(action, args, options) {
    options = options || {};
    options["method"] = "post";
    return Kwo.exec(action, args, options);
  },

  "go": function(action, args, options) {
    return Kwo.go(action, args, options);
  }

};

Kwo = {

  "registry": {},
  "Class": {},
  "Composer": {},
  "Visitor": {},
  "dialog": null,
  "dialogs": [],
  "Dialogs": {},
  "Ws": {},
  "Handlers": {},
  "Layers": {},
  "callbacks": {},
  "flags": {},
  "scripts": {},

  "getLastDialog": function() {
    if (Kwo.isBack()) {
      if (kwo.registry["dialogs"].length < 1) return null;
      return kwo.registry["dialogs"][kwo.registry["dialogs"].length - 1];
    }
    else {
      if (Kwo.dialogs.length < 1) return null;
      return Kwo.dialogs[Kwo.dialogs.length - 1];
    }
  },

  "setDialog": function(dialog) {
    Kwo.dialogs.push(dialog);
  },

  "unsetDialog": function() {
    return Kwo.dialogs.pop();
  },

  "onViewportChange": function(e) {
    if (Kwo.dialogs.length > 0) {
      var l = Kwo.dialogs.length;
      for (var i = 0; i < l; i++) {
        Kwo.dialogs[i].place();
      }
    }
    if (kwo.registry.dialogs && kwo.registry.dialogs.length > 0) {
      for (var i = 0; i < kwo.registry.dialogs.length; i++) {
        kwo.registry.dialogs[i].place();
      }
    }
  },

  "F": function(model) {
    model = model.toLowerCase();
    if (model in Kwo.registry) return Kwo.registry[model];
    Kwo.registry[model] = new Kwo.Class.Obj(model);
    return Kwo.registry[model];
  },

  "mergeArgs": function() {
    var h = new Hash({}), n = arguments.length, arg;
    for (var i = 0; i < n; i++) {
      arg = arguments[i];
      if (arg === undefined || arg === null || arg === false || arg === true) {
        continue;
      }
      if (Object.isString(arg)) {
        arg = arg.toQueryParams();
        h.update(arg);
      }
      else if (typeof arg == "object") {
        if (Object.isArray(arg)) {
          arg.each(function (item) {
            h.update(Kwo.mergeArgs(item));
          });
        }
        else if (Object.isElement(arg)) {
          var s = $(arg).readAttribute("data-values");
          if (s != null && s.length > 1) {
            h.update(s.evalJSON());
          }
          if (arg.tagName.toUpperCase() == "FORM") {
            if (Kwo.isBack()) {
              var xml_file = arg.getAttribute("data-xml-file");
              if (xml_file) {
                h.set("xml_file", xml_file);
              }
            }
            arg = $(arg).serialize(true);
          }
          else if ($(arg)) {
            var form;
            if ("form" in arg && arg.form) {
              form = $(arg.form)
              arg = form.serialize(true);
            }
            else {
              form = $(arg).up("form");
              arg = form ? form.serialize(true) : {};
            }
            if (form && Kwo.isBack()) {
              var xml_file = form.getAttribute("data-xml-file");
              if (xml_file) {
                h.set("xml_file", xml_file);
              }
            }
          }
          h.update(arg);
        }
        else {
          h.update(arg);
        }
      }
    }
    return h;
  },

  "isBack": function() {
    return "_scope" in window && window["_scope"] === "back";
  },

  "isFront": function() {
    return "_scope" in window && window["_scope"] === "front";
  },

  "isAccount": function() {
    return "_scope" in window && window["_scope"] === "account";
  },

  "isMiddle": function() {
    return "_scope" in window && window["_scope"] === "middle";
  },



  "exec": function(action, args, options) {

    if (Object.isElement(action)) {
      var elt = $(action);
      action = elt.readAttribute("data-action");
      args = {};
      if (elt.hasAttribute("data-args")) {
        args = Object.toJSON(elt.readAttribute("data-args"));
      }
      if (Kwo.isBack()) {
        args["context"] = {};
        elt.ancestors().each(function (ancestor) {
          if (ancestor.tagName != "DIV" || !ancestor.hasClassName("data-item")) return ;
          args["context[" + ancestor.getAttribute("data-model") + "_id]"] = ancestor.getAttribute("data-id");

        });
      }
      options = {};
      if (elt.hasAttribute("data-confirm")) {
        options["confirm"] = elt.getAttribute("data-confirm");
      }
      if (elt.hasAttribute("data-callback")) {
        options["callback"] = elt.getAttribute("data-callback");
      }
      if (elt.hasAttribute("data-container")) {
        options["container"] = elt.getAttribute("data-container");
      }
    }

    options = options || {};

    if ("method" in options) {
      options["method"] = options["method"].toLowerCase();
    }
    else {
      options["method"] = "post";
    }

    if ("disable" in options) {
      if (options["disable"] === true) {
        options["disable"] = args;
      }
      options["disable"] = $(options["disable"]);
      if (options["disable"].hasClassName("kwo-disabled")) {
        return ;
      }
    }

    if ("confirm" in options) {
      var msg;
      if (Object.isElement(options["confirm"])) {
        msg = $(options["confirm"]).getAttribute("data-confirm");
      }
      else {
        msg = options["confirm"] == true ? "êtes vous sûr ?" : options["confirm"];
      }
      if (msg && msg.length >= 1 && !confirm(msg.ucfirst())) { return false; }
    }


    var params;
    if (options["method"] == "get") {
      params = Kwo.mergeArgs(args, {"__token": Math.random()});
    }
    else {
      params = Kwo.mergeArgs(args);
    }
    var opts = {};

    if ("container" in options && options["container"]) {
      var container = $(options["container"]);
      if (!container) {
        console.log("Oops! No AJAX Container.");
        return false;
      }
      opts = {"parameters": params.toObject(),
              "method": options["method"],
              "evalScripts": false,
              "onCreate": function() {
                if ("disable" in options) {
                  options["disable"].addClassName("kwo-disabled");
                  if (options["disable"].tagName.toUpperCase() == "FORM") {
                    options["disable"].disable();
                  }
                }
              },
              "onComplete": function () {
                if ("disable" in options) {
                  options["disable"].removeClassName("kwo-disabled");
                  if (options["disable"].tagName.toUpperCase() == "FORM") {
                    options["disable"].enable();
                    //fix aleksandr
                    options["disable"].select('.permanent-disabled input, .permanent-disabled select, .permanent-disabled textarea, .permanent-disabled-elt').each(function(_e) {
                      if (_e && _e.disable) {
                        _e.disable();
                      }
                    });
                  }
                }
//                if ($(document.body).hasClassName("scripts-deferred")) {
                  container.select("[data-on-load]").each(function(elt) {
                    var scriptlet = elt.readAttribute("data-on-load").replace('\(this\)', '(elt)');
                    elt.writeAttribute("data-on-load", "");
                    if (scriptlet.length > 1) eval(scriptlet);
                  });
//                }
                if ("callback" in options && options["callback"]) {
                  options["callback"].call(container);
                };
                container.fire("container:updated", {"action": action,
                                                     "args": args});
              },
              "requestHeaders": {"X-Kwo-Href": window.location.href || "",
                                 "X-Kwo-Referer": document.referrer || "",
                                 "X-Kwo-Requested-With": "xhr",
                                 "X-Kwo-Request": "update"}};

      if ("insertion" in options) {
        opts["insertion"] = options["insertion"];
      }

      new Ajax.Updater(container, action, opts);
      return false;
    }

    if ("callback" in options && Object.isString(options["callback"])) {
      params["callback"] = options["callback"];
      action += action.indexOf("?") == -1 ? "?" : "&";
      Kwo.load(action + params.toQueryString());
      return false;
    }

    opts = {
      "method": options["method"],
      "requestHeaders": {"X-Kwo-Href": window.location.href || "",
                         "X-Kwo-Referer": document.referrer || "",
                         "X-Kwo-Requested-With": "xhr",
                         "X-Kwo-Request": "exec"},
      "asynchronous": "async" in options ? options["async"] : true,
      "evalJS": false,
      "evalJSON": false,
//      "parameters": {},
      "parameters": params.toObject(),
      "onCreate": function() {
//        if (window.top.$("loading")) { window.top.$("loading").show(); }
        if ("toggle" in options) { $(options.toggle).toggle(); }
        if ("disable" in options) {
          options["disable"].addClassName("kwo-disabled");
          if (options["disable"].tagName.toUpperCase() == "FORM") {
            options["disable"].disable();
          }
        }
      },
      "onSuccess": function(t) {
        var res = t.responseText.evalJSON();
        if (options["callback"] == true || options["callback"] == "true") {
          options["callback"] = true;
          if ((action.indexOf("/") == -1 && window.location.href.indexOf("/account/") != -1) ||
              action.indexOf("/account") != -1) {
            options["callback"] = Kwo.Account.refresh;
          }
          else {
            options["callback"] = Kwo.callback;
          }
        }
        if (Kwo.hasError(res)) {
          if ("callback" in options && !Object.isElement(options["callback"])) {
            options["callback"].call(null, res);
          }
          else if (res["error"] == 401) {
            return new Kwo.Dialogs.AuthManager();
          }
          else {
            console.log(res);
          }
        }
        else {
          if ("callback" in options) {
            if (options["callback"] === null || Object.isUndefined(options["callback"])) {

            }
            else if (Object.isElement(options["callback"])) {
              var elt = $(options["callback"]);
              if (elt.tagName.toUpperCase() == "SELECT") {
                elt.length = 0;
                $H(res["result"]["values"]).each(function (pair) {
                  elt.insert("<option value='" + pair.key + "'>" + pair.value + "</option>");
                });
                elt.selectedIndex = 0;
              }
              else {
                if (!elt.visible()) {
                  elt.show();
                }
                elt.update(res["result"]["callback_message"]);
                elt.addClassName("node-updated");
                if (elt.hasClassName("vanish")) {
                  elt.addClassName("vanish-on");
                  setTimeout(function () {
                    elt.removeClassName("vanish-on");
                    elt.hide(); }, 3000);
                }
                if (elt.hasClassName("dialog-close")) {
                  setTimeout(function () { Kwo.getLastDialog().close(); }, 2000);
                }
              }
            }
           else {
              options["callback"].call(null, res);
            }
          }
          if ("reset" in options) {
            options["reset"] = Object.isElement(options["reset"])
                             ? $(options["reset"])
                             : $(args);
            options["reset"].reset();
          }
        }
      },
      "on404": function(t) {
        console.log("AJAX Error : " + t.statusText + " was not found");
      },
      "onFailure": function(t) {
        console.log("AJAX Failure [" + t.status + "] : " + t.statusText);
      },
      "onException": function(t, e) {
        if (Kwo.isBack()) console.log(e);
        console.log("AJAX Exception [" + e.name + "] : " + e.message);
      },
      "onComplete": function(t) {
        if ("toggle" in options) { $(options.toggle).toggle(); }
        if ("disable" in options) {
          options["disable"].removeClassName("kwo-disabled");
          if (options["disable"].tagName.toUpperCase() == "FORM") {
            options["disable"].enable();
            //fix aleksandr
            options["disable"].select('.permanent-disabled input, .permanent-disabled select, .permanent-disabled textarea, .permanent-disabled-elt').each(function(_e) {
              if (_e && _e.disable) {
                _e.disable();
              }
            });
          }
        }
      }
    };
    new Ajax.Request(action, opts);

    return false;
  },


  "go": function(action, args, options) {
    var url = action;
    options = options || {};

    if (Object.isElement(url)) {
      url = $(action).readAttribute("data-url");
    }

    if (!Object.isString(action) && "result" in action && "callback_url" in action["result"]) {
      url = action["result"]["callback_url"];
    }

    if ("confirm" in options) {
      var msg;
      if (Object.isElement(options["confirm"])) {
        msg = $(options["confirm"]).getAttribute("confirm");
      }
      else {
        msg = options["confirm"] == true ? "OK ?" : options["confirm"];
      }
      if (msg.length >= 2 && !confirm(msg.ucfirst())) return ;
    }

    if (!Object.isUndefined(args) && args != null) {
      args = Kwo.mergeArgs(args);
      if (!args.toQueryString().empty()) {
        url = action + "?" + args.toQueryString();
      }
    }

    if ("target" in options) {
      if (options["target"] == "blank") {
        window.open(url);
      }
      else {
        $(options["target"]).src = url;
      }
      return ;
    }

    window.location.href = url;

    return false;
  },

  "anchor": function(name) {
    document.anchors.item(name).scrollIntoView();
    return false;
  },

  "callback": function(resp) {
    resp = new kwo.Response(resp);
    if (resp.hasError()) {
      return resp.alert();
    }
    if (resp.hasAttribute("callback_message")) {
      if (resp.hasAttribute("callback_container")) {
        $(resp.getAttribute("callback_container")).update(resp.getAttribute("callback_message"));
      }
      else {
        Kwo.warn(resp.getAttribute("callback_message"));
      }
    }
    else if (resp.isRedirection()) {
      var url = resp.getAttribute("callback_url");
      if (url == "reload") {
        Kwo.reload();
      }
      else {
        Kwo.go(url);
      }
      return ;
    }
  },

  "home": function(h) {
    window.location.href= "/";
  },

  "refresh": function(classes) {
    classes = classes || [];
    if (!(classes instanceof Array)) {
      classes = [classes];
    }
    var i, n = classes.length;
    $(document.body).select("[data-content-provider]").each(function(elt) {
      if (n < 1) {
        elt.updateWithProvider();
      }
      else {
        for (i = 0; i < n; i++) {
          if (elt.hasClassName(classes[i])) {
            elt.updateWithProvider();
            break ;
          }
        }
      }
    });
  },

  "reload": function() {
    window.location.reload();
  },

  "hasError": function(resp) {
    if (Object.isUndefined(resp)) return false;
    return resp["error"] >= 1;
  },

  "error": function(args) {
    if (typeof args == "object" && "result" in args && "msg" in args["result"]) {
      args = args["result"]["msg"];
    }
    if (args instanceof Array) {
      var out = "Oops!\n";
      args.each(function(item) {
        out += " - " + item + "\n";
      });
      alert(out);
    }
    else {
      alert(args.ucfirst());
    }
    return false;
  },

  "warn": function(args) { console.log(args);
    if (typeof args == "object" && "result" in args && "callback_message" in args["result"]) {
      args = args["result"]["callback_message"];
    }
    if (args instanceof Array) {
      var out = "";
      args.each(function(item) {
        out += item + "\n";
      });
      alert(out);
    }
    else {
      alert(args.ucfirst());
    }
    return false;
  },

  "isAuth": function() {
    /*if ("_user_id" in window) {
      return window["_user_id"] >= 1;
    }*/
    if ("_user_id" in window && window["_user_id"] >= 1) return true;
    kwo.post("/user.id", {},
            {"async": false,
             "callback": function(resp) {
               resp = new kwo.Response(resp)
               if (resp.hasError()) return ;
               window["_user_id"] = resp.getAttribute("user_id");
             }});
    return ("_user_id" in window) && (window["_user_id"] >= 1);
  },

  "load": function(src, callback, args) {
    var elt = null;
    if (Object.isElement(src)) {
      elt = src;
      src = elt.getAttribute("data-script");
      if (Object.isUndefined(callback) && elt.hasAttribute("data-callback")) {
        callback = elt.getAttribute("data-callback");
        if (callback == "true") {
          callback = function() { console.log(src + " is loaded"); }
        }
        else {
          callback = callback.toFunction();
          callback = callback.bind(elt);
        }
      }
    }
    if (!Object.isUndefined(callback) && !Object.isUndefined(args)) {
      callback = callback.curry(args);
    }
    if (Kwo.scripts[src] == true) {
      if (!Object.isUndefined(callback)) {
        callback();
      }
      return ;
    }
    var script = new Element("script",
                             {"type": "text/javascript",
                              "src": src,
                              "async": true});
    if (!Object.isUndefined(callback)) {
      if (Prototype.Browser.IE) {
        script.onreadystatechange = function() {
          if (script.readyState == "loaded" ||
              script.readyState == "complete"){
            script.onreadystatechange = null;
            callback();
          }
        }
      }
      else {
        script.onload = callback;
      }
    }
    Kwo.scripts[src] = true;
    $$("head")[0].insert(script);
  },

  "redirect": function(elt) {
    elt = $(elt);
    if (!Kwo.hasClickExpired(elt)) return false;
    var args = {}, opts = {};
    args["url"] = elt.readAttribute("data-url");
    args["item"] = elt.readAttribute("data-item");
    if (elt.readAttribute("target")) {
      opts["target"] = elt.readAttribute("target");
    }
    Kwo.go("/redirect", args, opts);
    return false;
  },

  "hasClickExpired": function(elt) {
    if (!Object.isElement(elt)) return ;
    var expiration = elt.readAttribute("data-expiration");
    expiration = Object.isString(expiration) ? expiration.intval() : 0;
    var now = new Date().getTime();
    if (expiration > now) return false;
    elt.writeAttribute("data-expiration", (now + 500));
    return true;
  },

  "onNotify": function(resp) {
    resp = new kwo.Response(resp)
    if (resp.hasError()) return resp.alert();
    alert(resp.getMessage());
  },

  "onDomLoaded": function() {
//    console.log("dom:loaded");
    if (Kwo.isFront() || Kwo.isAccount()) {
      //    console.log("refresh");
      Kwo.refresh();
    }
//    if ($(document.body).hasClassName("scripts-deferred")) {
//    console.log("deferred");
    $(document).body.select("[data-on-load]").each(function(elt) {
      var scriptlet = elt.readAttribute("data-on-load").replace('\(this\)', '(elt)');
      elt.writeAttribute("data-on-load", "");
      if (scriptlet.length > 1) eval(scriptlet);
    });
    //    }
    if ($(document).body.hasClassName("page-traced") && top && top.document) {
      $(top.document).fire("page:dom:loaded");
    }
  }

};

kwo.track = function(elt) {
  // data-interaction-(item,type,value,origin)
  // data-
}


kwo.events.onGoogleMapsLoaded = function() {
  $(document).fire("google:maps:loaded");
  $(document.body).addClassName("google-maps-loaded");
};

kwo.Response = Class.create({

  resp: null,

  AUTH_ERROR: 401,
  UNDEFINED_ERROR: 411,

  initialize: function(resp) {
    if (resp === undefined) {
      resp = {"error": this.UNDEFINED_ERROR,
              "result": {"msg": ["invalid error"]}};
    }
    this.resp = resp;
  },

  hasAttribute: function(attr) {
    return (attr in this.resp["result"]);
  },

  getAttribute: function(attr) {
    return this.resp["result"][attr];
  },

  getAttributes: function() {
    return this.resp["result"];
  },

  isAuthError: function() {
    return this.hasError() && this.resp["error"] == this.AUTH_ERROR;
  },

  isRedirection: function() {
    return this.hasUrl();
  },

  redirect: function() {
    Kwo.go(this.getUrl());
  },

  getUrl: function() {
    return this.getAttribute("callback_url");
  },

  alert: function() {
    if (this.hasError()) {
      var fun = (typeof kwo["error"] == "function") ? kwo.error : alert;
      return fun(this.getError("\n"));
    }
    var fun = (typeof kwo["alert"] == "function") ? kwo.alert : alert;
    return fun(this.getMessage());
  },

  warn: function() {
    alert(this.getMessage());
  },

  hasUrl: function() {
    return !this.hasError() && this.hasAttribute("callback_url");
  },

  hasNotification: function() {
    return !this.hasError() && this.hasAttribute("callback_notification");
  },

  hasMessage: function() {
    return !this.hasError() && this.hasAttribute("callback_message");
  },

  getMessage: function() {
    return this.getAttribute("callback_message");
  },

  getNotification: function() {
    return this.getAttribute("callback_notification");
  },

  getResult: function() {
    return this.resp["result"] || {};
  },

  getErrors: function() {
    return this.resp["result"]["msg"];
  },

  getError: function(sep) {
    sep = sep || "<br/>";
    var args = this.resp["result"]["msg"];
    var out = "";
    if (args instanceof Array) {
      out = args.join(sep);
/*      args.each(function(item) {
        out += item + sep;
      }); */
    }
    return out;
  },

  hasError: function() {
    return this.resp["error"] >= 1;
  },

  getErrorCode: function() {
    return this.resp["error"];
  }

});

if (!Kwo.isBack()) {
  var args = {};
  if (window["_events"]) args["events"] = window["_events"];
  if (window["_interactions"]) args["interactions"] = window["_interactions"];
  if ("userAgent" in navigator && !navigator.userAgent.match(/(bot|crawl|spid)/)) {
//    console.log("tracker");
    kwo.post("/track",
             {"params": Object.toJSON(args)},
             {"callback": function(resp) {
//               console.log("tracker:done");
               resp = new kwo.Response(resp);
               if (resp.hasError()) {
                 console.log(resp);
                 return ;
               }
//               console.log(resp);
               var attrs = resp.getAttributes();
               kwo.vars.tracking = attrs || {};
               kwo.vars.visit_id = attrs["visit_id"] || 0;
               kwo.vars.visitor_id = attrs["visitor_id"] || 0;
               kwo.vars.is_new_visit = attrs["is_new_visit"] || 0;
               kwo.vars.is_new_visitor = attrs["is_new_visitor"] || 0;
               //               console.log(kwo.vars);
               if (kwo.vars.tracking.popup_id && kwo.vars.tracking.popup_id.intval() > 0) {
                 new kwo.dialogs.Popup(kwo.vars.tracking.popup_id);
               }
               if (kwo.vars.is_new_visit == 1) $(document).fire("tracker:visit:new");
               if (kwo.vars.is_new_visitor == 1) $(document).fire("tracker:visitor:new");
             }});
  }
/*  if ("_defer" in window && window["_defer"] == 1) {
    Kwo.onDomLoaded();
  }
  else { */


  // todo : déplacer le code de onDomLoaded directement dans la func
  // adaniloff fix [
  //document.observe("dom:loaded", function() { Kwo.onDomLoaded(); });
  Event.observe(window, "load", function() { Kwo.onDomLoaded(); });
  // ]
/*  } */
  window["_events"] = null;
  window["_interactions"] = null;
}

Kwo.Locale = {

  onOpen: function (elt) {
    $("kwo-locales-box").toggle();
    var pos = $(elt).cumulativeOffset();
    $("kwo-locales-box").setStyle({"top": (pos[1] + $(elt).getHeight()) + "px",
                                   "left": pos[0] + "px"});
  },

  onSet: function (locale, fallback) {
    if ($("kwo-locales-box")) $("kwo-locales-box").toggle();
    kwo.post("/locale.set", {"locale": locale},
             {"callback": Kwo.Locale.onCallback.curry(fallback)});
  },

  onCallback: function (fallback, h) {
    if (fallback == undefined) {
      var parts = window.location.pathname.substring(1).split("/", 1);
      if (parts.length != 1 || parts[0].length != 2) {
        Kwo.reload();
        return ;
      }
      window.location.href = window.location.href.replace(new RegExp("/" + parts[0] + "(/|$)"),
                                                          "/" + h["result"]["locale"] + "/");
      return ;
    }
    Kwo.go(fallback);
  }

};


Kwo.DateManager = {

  birthdate: function (elt) {
    var input = $(elt).previous("INPUT"), date = {}, tmp;
    tmp = input;
    for (var i = 1; i <= 3; i++) {
      tmp = tmp.next("SELECT");
      if (tmp.className.indexOf("day") != -1) date["day"] = $F(tmp);
      else if (tmp.className.indexOf("month") != -1) date["month"] = $F(tmp);
      else date["year"] = $F(tmp);
    }
    input.value = date["year"] + "-" + date["month"] + "-" + date["day"];
  },

  monthyear: function(elt) {
    var input = $(elt).previous("INPUT"), date = {}, tmp;
    tmp = input;
    for (var i = 1; i <= 2; i++) {
      tmp = tmp.next("SELECT");
      if (tmp.className.indexOf("month") != -1) date["month"] = $F(tmp);
      else date["year"] = $F(tmp);
    }
    input.value = date["year"] + "-" + date["month"] + "-01";
  }

};

Kwo.Tooltip = {

  "elt": null,

  "hide": function(anchor, id) {
    document.stopObserving("mousemove", Kwo.Tooltip.onMouseMove);
    Kwo.Tooltip.elt.hide();
  },

  "show": function(anchor, id) {
    anchor = $(anchor);
    Kwo.Tooltip.elt = id === undefined
                    ? anchor.previous(".kwo-tooltip")
                    : $(id);
    document.observe("mousemove", Kwo.Tooltip.onMouseMove);
    Kwo.Tooltip.elt.show();
    if (window.event) Kwo.Tooltip.onMouseMove(window.event);
  },

  "onMouseMove": function(e) {
    var dim = document.viewport.getDimensions();
    var pos = document.viewport.getScrollOffsets();
    var size = Kwo.Tooltip.elt.getDimensions();
    var top = (Event.pointerY(e) + 10);
    var left = (Event.pointerX(e) + 10);
    if (left + size["width"] > dim["width"] + pos["left"]) {
      left -= (left + size["width"]) - (dim["width"] + pos["left"]) + 5;
    }
    if (top + size["height"] > dim["height"] + pos["top"]) {
      top -= size["height"] + 20;
    }
    Kwo.Tooltip.elt.setStyle({"top": top + "px", "left": left + "px"});
  }

};

Kwo.Editor = Class.create({

  "doc": null,
  "iframe": null,
  "range": null,
  "selection": null,
  "src": null,
  "version": 1.1,
  "win": null,

  initialize: function(name) {
    if (!("execCommand" in window.document)) return ;
    var actions = {"bold": false, "italic": false, "underline": false,
                   "increasefontsize": false, "indent":true,"outdent":true,
                   "insertorderedlist": false, "insertunorderedlist": false,
                   "createlink": false, "insertimage": false, "paste": false,
                   "removeformat": false};
    this.src = $(name);
    this.src.hide();
    this.iframe = new Element("iframe", {"designmode": "on", "name": "_" + "name", "id": "_" + name});
    this.iframe.setStyle({height: this.src.getStyle("height"), width: this.src.getStyle("width")});
    this.iframe.addClassName("richtext");
    this.src.insert({"after": this.iframe});
    if (Prototype.Browser.IE) {
      this.win = window.frames["_" + name];
      this.doc = this.win.document;
    }
    else {
      this.win = this.iframe.contentWindow;
      this.doc = this.iframe.contentDocument;
    }
    if (!Prototype.Browser.Gecko) {
      this.doc.designMode = "on";
    }
    this.doc.open("text/html");
    this.doc.write("<html><head><style>"
                   + "BODY { background:" + this.src.getStyle("background-color") + "; cursor:text; height:100%; "
                   + "       border:none; color:#777; font-family:monospace; margin:0; padding:0 4px; }"
                   + "BLOCKQUOTE { border-left:2px solid #eee; padding-left:4px; margin-left:4px; }"

                   + "</style></head><body></body></html>");
    this.doc.close();
    if (Prototype.Browser.Gecko) {
      this.doc.designMode = "on";
    }
    if (this.src.getValue().length >= 1) {
      this.doc.body.innerHTML = this.src.getValue();
    }
    if (Prototype.Browser.Gecko || Prototype.Browser.WebKit) {
      this.doc.execCommand("styleWithCSS", false, false);
    }
    this.iframe.observe("mouseout", this.onStore.bindAsEventListener(this));
    if (this.doc.addEventListener) {
      this.doc.addEventListener("keyup", this.onStore.bindAsEventListener(this), false);
    }
    else {
      this.doc.attachEvent("onkeyup", this.onStore.bindAsEventListener(this));
    }
    var toolbar = new Element("div").setStyle({width: this.src.getStyle("width")});
    toolbar.addClassName("kwo-toolbar");
    this.src.insert({after: toolbar});
    var img;
    for (key in actions) {
      if ((Prototype.Browser.IE ||  Prototype.Browser.WebKit) &&
          key == "increasefontsize") continue ;
      img = new Element("img", {"src": "/web/core/images/editor/" + key + ".png"});
      img.observe("click", this.exec.bind(this, key, actions[key]));
      toolbar.insert(img);
    };
  },

  onStore: function() {
    var source = this.doc.body.innerHTML;
    source = source.replace(/<br class\="webkit-block-placeholder">/gi, "<br />");
    source = source.replace(/<span class="Apple-style-span">(.*)<\/span>/gi, "$1");
    source = source.replace(/ class="Apple-style-span"/gi, "");
    source = source.replace(/ style="">/gi, "");
    source = source.replace(/<span style="font-weight: bold;">(.*)<\/span>/gi, "<strong>$1</strong>");
    source = source.replace(/<span style="font-style: italic;">(.*)<\/span>/gi, "<em>$1</em>");
    source = source.replace(/<br>/gi, "<br />");
    source = source.replace(/<br \/>\s*<\/(h1|h2|h3|h4|h5|h6|li|p)/gi, "</$1");
    source = source.replace(/<b(\s+|>)/g, "<strong$1");
    source = source.replace(/<\/b(\s+|>)/g, "</strong$1");
    source = source.replace(/<i(\s+|>)/g, "<em$1");
    source = source.replace(/<\/i(\s+|>)/g, "</em$1");
    source = source.replace(/(<[^\/]>|<[^\/][^>]*[^\/]>)\s*<\/[^>]*>/gi, "");
    source = source.replace(/\.\.\/doc\//g, "/doc/");
    this.src.value = source;
    //this.log();
  },

  exec: function(name, value) {
    var that = this;
    if (name == "createlink") {
      if (value == false) {
        this.saveRange();
        new Kwo.Prompt({"title": "Adresse du site ?", "prefix": "http://"},
                       this.exec.bind(this).curry(name));
        return ;
      }
      if (!Object.isString(value) || value.legnth < 5) return ;
      if (!(value.toLowerCase().startsWith("http://") || value.toLowerCase().startsWith("https://"))) {
        value = "http://" + value;
      }
      if (value.toLowerCase().startsWith("http://http://")) {
        value = value.substr(7);
      }
      this.restoreRange();
      if (Prototype.Browser.IE && this.range) {
        this.range.select();
      }
      this.doc.execCommand(name, false, value);
    }
    else if (name == "paste") {
      if (value == false) {
        this.saveRange();
        var d = new Kwo.Class.PasteManager(this);
        return ;
      }
      this.restoreRange();
      this.insertHTML(value.replace(/\n/g, "<br/>"));
    }
    else if (name == "insertimage") {
      if (value == false) {
        this.saveRange();
        var fm = new Kwo.Dialogs.FileManager(this.src, {"filter": "image"});
        fm.onSelect = function(h) {
          that.exec(name, h["name"]);
          this.close();
        }
        return ;
      }
      this.restoreRange();
      this.insertHTML('<img src="/' + value + '" />');
    }
    else {
      this.win.focus();
      this.doc.execCommand(name, false, value);
    }
    if (Prototype.Browser.Gecko || Prototype.Browser.WebKit) {
      this.win.getSelection().collapseToEnd();
    }
    this.onStore();
  },

  saveRange: function() {
    if (Prototype.Browser.IE) {
      this.range = null;
      if (this.doc.selection.createRange().text.length >= 1) {
        this.range = this.doc.selection.createRange();
      }
    }
    else if (Prototype.Browser.Gecko) {
      this.selection = this.win.getSelection();
      this.range = this.selection.getRangeAt(0).cloneRange();
//      console.log(this.range);
    }
  },

  restoreRange: function() {
    this.win.focus();
    if (!Prototype.Browser.Gecko) return ;
    var selection = this.win.getSelection();
    selection.removeAllRanges();
    if (this.range) {
      selection.addRange(this.range);
    }
  },

  insertHTML: function(content) {
    this.win.focus();
    if (Prototype.Browser.IE) {
      if (this.range === null) {
        this.range = this.doc.selection.createRange();
      }
      this.range.pasteHTML(content);
      this.range.collapse(false);
      this.range = null;
    }
    else {
      this.doc.execCommand("insertHTML", false, content);
    }
  },

  log: function() {
    console.log(this.doc.body.innerHTML);
    console.log(this.src.value);
  }

});



Kwo.Layer = function() {
  var args = $A(arguments);
  var type = args.shift();
  var name = args.shift().toLowerCase();
  var code = type + ":" + name;
  if (!(code in Kwo.Layers)) {
    Kwo.Layers[code] = Class.create(Kwo[type.ucfirst()], Kwo.Handlers[name]);
  }
//  console.log(code);
/*  var that = {initialize: function($super) {
    $super.apply(this, args);
  }};*/
  return new (Class.create(Kwo.Layers[code],
                           {"initialize": function($super) {
                             $super.apply(this, args);
                           }}));
}

Kwo.Dialog = Class.create({

  "args": null,
//  "bind": null,
  "className": null,
  "width": null,
  "height": null,
  "layout": null,
  "name": null,
  "opts": null,
  "overlay": null,
  "shadow": null,
  "support": null,

  "scroll": {},

  initialize: function(paint_method, args, opts) {
    opts = opts || {};
    this.args = args || this.args;
    if (this.opts === null) {
      this.opts = opts;
    }
    this.name = this.opts["name"] || this.name || "dialog";
    this.className = this.opts["className"] || this.className || "";
    if (this.layout) {
      this.className +=  "layout-" + this.layout;
    }
    this.width = this.opts["width"] || this.width || 500;
    this.height = this.opts["height"] || this.height || 300;
    if (this.overlay === null) {
      var class_name;
      this.overlay = new Element("div").setStyle("display:none; position:absolute; top:0; left:0; opacity:0.4;");
      class_name = "dialog-overlay dialog-overlay-" + this.name;
//      if (this.overlay_class) class_name += " " + this.overlay_class;
      this.overlay.addClassName(class_name);
      this.shadow = new Element("div").setStyle("display:none; position:absolute; top:0; left:0;");
      class_name = "dialog-shadow dialog-shadow-" + this.name;
//      class_name = "dialog-shadow dialog-shadow-" + this.name + " kwo-layer";
//      if (this.shadow_class) class_name += " " + this.shadow_class;
      this.shadow.addClassName(class_name);
      this.support = new Element("div").setStyle("display:none;");
      class_name = "dialog-support dialog-support-" + this.name + " dialog-" + this.name + " " + this.className;
//      if (this.support_class) class_name += " " + this.support_class;
      this.support.addClassName(class_name);

      var cross = new Element("A", {"href": "javascript:;"}).addClassName("dialog-close");
//      if (this.opts["noclose"] != 1 && window["_scope"] != "back") {
      if (this.opts["noclose"] != 1) {
        var that = this;
        this.shadow.appendChild(cross).observe("click", function () {
          that.close();
        });
        cross.observe("mousedown", function(evt) {
          Event.stop(evt);
        });
      }
      /*if (Prototype.Browser.IE) {
        this.place();
      }*/

      if (Kwo.dialogs.length < 1) {
        if (Prototype.Browser.Gecko) {
          this.scroll["x"] = window.pageXOffset;
          this.scroll["y"] = window.pageYOffset;
        }
        Event.observe(document, "keyup", this.onEscape.bindAsEventListener(this));
/*        Event.observe(document, "scroll", this.onScroll); */
        document.documentElement.style.overflow = "hidden";
/*        console.log(document.body.style.overflow); */
      }
      /*if (!Prototype.Browser.IE) {
        this.place();
      }*/
      this.place();
      document.body.appendChild(this.overlay);
      document.body.appendChild(this.shadow);
      this.shadow.appendChild(this.support);
      if (this.opts["noclose"] != 1) {
        this.overlay.observe("click", this.close);
      }
    }
//    this.bind = this.place.bindAsEventListener(this);
    Event.observe(window, "resize", Kwo.onViewportChange);
    Event.observe(document, "MozScrollAreaChanged", Kwo.onViewportChange);
    Kwo.setDialog(this);
/*    if (Prototype.Browser.IE && navigator.userAgent.indexOf("MSIE 6") > -1) {
      $$("SELECT").invoke("hide");
    } */
    this.support.update();
    if (Object.isFunction(paint_method)) {
      paint_method.call(this, this.args);
    }
    else if (!Object.isUndefined(paint_method)) {
      kwo.post(paint_method, this.args, {"async": true, "container": this.support});
    }
    $(this.overlay, this.shadow, this.support).invoke("show");
    if ("onAfterPlace" in this) {
      this.onAfterPlace();
    }
    if (cross) cross.focus();
    if (window["ga"]) {
      ga("send", "event", "dialog", this.name);
    }
  },

  onScroll: function(event) {
//    console.log(event);
  },

  onEscape: function (event) {
    if (event.keyCode == Event.KEY_ESC) {
/*      var dialog = Kwo.getLastDialog();
      if (dialog.opts["noclose"] == 1) return ;
      event.stopPropagation();
      dialog.close();*/
      if (this.opts["noclose"] == 1) return ;
      event.stopPropagation();
      this.close();
    }
  },

  place: function () {
    var dimensions = document.viewport.getDimensions();
    var offsets = document.viewport.getScrollOffsets();
    var height = this.height;
    if (Object.isString(height) && height.endsWith("%")) {
      height = height.substr(0, height.length - 1);
      height = parseInt(dimensions.height * height / 100);
    }
    var dh = height;
    if (dh >= dimensions.height) {
      dh = dimensions.height - 30;
    }
    var width = this.width;
    if (Object.isString(width) && width.endsWith("%")) {
      width = width.substr(0, width.length - 1);
      width = parseInt(dimensions.width * width / 100);
    }
//    console.log(width);
    var left = parseInt(offsets.left + ((dimensions.width - width)/ 2));
    var top = parseInt(offsets.top + ((dimensions.height - dh)/ 2));
    top = top < 0 ? 0 : top;
    left = left < 0 ? 0 : left;
//    top += 15;
//    var width = dimensions.width;
//    var height = dimensions.height;
    var oh = dimensions.height;
    if (Prototype.Browser.Gecko) {
      oh = document.documentElement.scrollHeight;
    }
    this.overlay.setStyle({"width": dimensions.width + "px",
                           "height": oh + "px",
                           "left": offsets.left + "px",
                           "top": offsets.top + "px"});
    this.shadow.setStyle({"width": width + "px",
                          "height": dh + "px",
                          "left": left + "px",
                          "top": top + "px"});
    this.support.setStyle({"width": width + "px",
                           "height": dh + "px"});
  },

  apply: function(value) {
    if (Object.isUndefined(this.callback)) alert(value);
    else if (Object.isElement(this.callback)) this.callback.value = value;
    else this.callback(value);
    this.close();
  },

  close: function() {
    var dialog = Kwo.getLastDialog();
    if (Object.isFunction(dialog.onBeforeClose)) {
      dialog.onBeforeClose();
    }
    if (Prototype.Browser.IE) {
      dialog.shadow.hide().remove();
      dialog.overlay.hide().remove();
    }
    else {
      dialog.shadow.remove();
      dialog.overlay.remove();
    }
    if (Kwo.dialogs.length == 1) {
      if (Prototype.Browser.IE) {
        document.documentElement.style.overflow = "auto";
      }
      else {
        var dimensions = document.viewport.getDimensions();
        document.documentElement.style.overflowX = document.documentElement.scrollWidth > dimensions.width ? "scroll" : "auto";
        if (document.documentElement.scrollHeight > dimensions.height) {
          document.documentElement.style.overflowY = "scroll";
          // WORKAROUND
          if (Prototype.Browser.Gecko && dialog.scroll["y"] > 0) {
            window.scroll(dialog.scroll["x"],
                          dialog.scroll["y"]);
          }
        }
        else {
          document.documentElement.style.overflowY = "auto";
        }
      }
      //console.log({sw: document.documentElement.scrollWidth, dw: dimensions.width});
/*      if (Prototype.Browser.IE && navigator.userAgent.indexOf("MSIE 6") > -1) {
        $$("SELECT").invoke("show");
      } */
      Event.stopObserving(document, "keyup", dialog.onEscape.bindAsEventListener(dialog));
    }
    Kwo.unsetDialog();
//    Kwo.unsetDialog(dialog.name);
    if (Object.isFunction(dialog.onAfterClose)) {
      dialog.onAfterClose();
    }
  }

});

Kwo.Class.PasteManager = Class.create(Kwo.Dialog, {

  initialize: function($super, editor) {
    this.editor = editor;
    this.name = "paste-manager";
    this.className = "layout-hbox";
    this.width = 350;
    this.height = 250;
    $super(this.onDisplay);
  },

  onDisplay: function() {
    kwo.post("/editor.paste", {},
             {"container": this.support,
              "callback": this.onBind.bind(this)});
  },

  onBind: function() {
    var that = this;
    this.support.down("FORM").observe("submit", function(evt) {
      evt.stop();
      that.onSubmit(evt.element());
    });
  },

  onSubmit: function(elt) {
    this.editor.exec("paste", this.support.down("TEXTAREA").getValue());
    this.close();
  }

});

Kwo.W = Class.create({

  "args": null,
  "bind": null,
  "className": null,
  "width": null,
  "height": null,
  "layout": null,
  "name": null,
  "opts": null,
  "overlay": null,
  "shadow": null,
  "support": null,

  initialize: function(paint_method, args, opts) {
    opts = opts || {};
    this.args = args || this.args;
    if (this.opts === null) {
      this.opts = opts;
    }
    this.name = this.opts["name"] || this.name || "widget";
    this.className = this.opts["className"] || this.className || "";
    if (this.layout) {
      this.className +=  "layout-" + this.layout;
    }
    this.width = this.opts["width"] || this.width || 500;
    this.height = this.opts["height"] || this.height || 300;
    if (!("y_position" in this.opts)
         || (this.opts["y_position"] != "top"
             && this.opts["y_position"] != "bottom")) {
      if (Object.isElement(this.elt)
          && document.viewport.getHeight() < this.elt.viewportOffset().top + this.height) {
        this.y_position = "top";
      }
      else {
        this.y_position = "bottom";
      }
    }
    else {
      this.y_position = this.opts["y_position"] || "bottom";
    }
    if (!("x_position" in this.opts)
         || (this.opts["x_position"] != "left"
             && this.opts["x_position"] != "right")) {
      if (Object.isElement(this.elt)
          && document.viewport.getWidth() < this.elt.viewportOffset().left + this.width) {
        this.x_position = "right";
      }
      else {
        this.x_position = "left";
      }
    }
    else {
      this.x_position = this.opts["x_position"] || "left";
    }
    if (this.overlay === null) {

//      this.overlay = new Element("div").setStyle("display:none; opacity:0; position:absolute; top:0; left:0; background-color:black;").addClassName("widget-overlay"); by spina 2013-09-05
      this.overlay = new Element("div").addClassName("widget-overlay").hide();
//      this.shadow = new Element("div").setStyle("display:none; position:absolute; top:0; left:0;").addClassName("widget-shadow kwo-layer");
      this.shadow = new Element("div").setStyle("display:none; position:absolute; top:0; left:0;").addClassName("widget-shadow");
      this.support = new Element("div").hide();
      this.support.addClassName("widget-support widget-" + this.name + " " + this.className);
      var cross = new Element("div").addClassName("dialog-close");
      if (Kwo.dialogs.length < 1) {
        Event.observe(document, "keyup", this.onEscape);
       // document.documentElement.style.overflow = "hidden";
      }
      this.place();
      document.body.appendChild(this.overlay);
      document.body.appendChild(this.shadow);
      this.shadow.appendChild(this.support);
      if (this.opts["noclose"] != 1) {
        this.overlay.observe("click", this.close);
      }
    }
    this.bind = this.place.bindAsEventListener(this);
    Event.observe(window, "resize", Kwo.onViewportChange);
    Kwo.setDialog(this);
/*    if (Prototype.Browser.IE && navigator.userAgent.indexOf("MSIE 6") > -1) {
      $$("SELECT").invoke("hide");
    } */
    this.support.update();
    if (Object.isFunction(paint_method)) {
      paint_method.call(this, this.args);
    }
    else if (!Object.isUndefined(paint_method)) {
      kwo.post(paint_method, this.args, {async: true, container: this.support});
    }
    $(this.overlay, this.shadow, this.support).invoke("show");
//    $(this.shadow).focus();
    if ("onAfterPlace" in this) {
      this.onAfterPlace();
    }
  },

  onEscape: function (event) {
    if (event.keyCode == Event.KEY_ESC) {
      var dialog = Kwo.getLastDialog();
      if (dialog.opts["noclose"] == 1) return ;
      event.stopPropagation();
      dialog.close();
    }
  },

  place: function () {
    var scrolloffset = document.viewport.getScrollOffsets();
    var offset_top = this.y_position == "bottom" ? this.elt.getHeight() : -this.height;
    var offset_left = this.x_position == "left" ? 0 : -this.width + this.elt.getWidth();
    if (this.x_position == "right") {
      var border_right_width = parseInt(this.elt.getStyle("border-right-width"));
      var border_left_width = parseInt(this.elt.getStyle("border-left-width"));
      offset_left -= isNaN(border_right_width) ? 0 : border_right_width;
      offset_left -= isNaN(border_left_width) ? 0 : border_left_width;
    }
    var top = this.elt.viewportOffset()[1] + offset_top + scrolloffset.top;
    var left = this.elt.viewportOffset()[0] + offset_left + scrolloffset.left;
    var width = $(document.body).getWidth();
    var height = $(document.body).getHeight();
/* by spina 2013-09-05
    this.overlay.setStyle({"width": width + "px",
                           "height": height + "px"});
*/
    this.shadow.setStyle({"width": this.width + "px",
                          "height": this.height + "px",
                          "left": left + "px",
                          "top": top + "px"});
    this.support.setStyle({"width": this.width + "px",
                           "height": this.height + "px"});
  },

  apply: function(value) {
    if (Object.isUndefined(this.callback)) alert(value);
    else if (Object.isElement(this.callback)) this.callback.value = value;
    else this.callback(value);
    this.close();
  },

  close: function() {
    var dialog = Kwo.getLastDialog();
    dialog.shadow.remove();
    dialog.overlay.remove();
    if (Kwo.dialogs.length == 1) {
/*      if (Prototype.Browser.IE && navigator.userAgent.indexOf("MSIE 6") > -1) {
        $$("SELECT").invoke("show");
      } */
      Event.stopObserving(document, "keyup", this.onEscape);
    }
//    Kwo.unsetDialog(dialog.name);
    Kwo.unsetDialog();
    if (Object.isFunction(dialog.onAfterClose)) {
      dialog.onAfterClose();
    }
  }

});

Kwo.Prompt = Class.create(Kwo.Dialog, {

  initialize: function($super, opts) {
    this.name = "prompt";
    this.className = "layout-hbox";
    this.opts = opts || {};
    if ("title" in this.opts && Object.isElement(this.opts["title"])) {
      this.opts["title"] = this.opts["title"].readAttribute("data-title");
    }
    if ("description" in this.opts && Object.isElement(this.opts["description"])) {
      this.opts["description"] = this.opts["description"].readAttribute("data-description");
    }
    if ("value" in this.opts && Object.isElement(this.opts["value"])) {
      this.opts["value"] = this.opts["value"].readAttribute("data-value");
    }
    this.height = 150;
    if ("rows" in this.opts) {
      this.height += this.opts["rows"] * 15;
    }
    $super(this.onDisplay);
  },

  onDisplay: function() {
    kwo.post("/dialog.prompt", this.opts,
             {"container": this.support,
              "callback": this.onDisplayCompleted.bind(this)});
  },

  onDisplayCompleted: function() {
    this.input = this.support.down(".text");
    if (Object.isElement(this.opts["onsucceed"])) {
      this.onSucceed = function () {
        this.opts["onsucceed"].value = this.input.getValue();
        this.close();
      }
    }
    this.support.down(".button-ok").observe("click", this.onSucceed.bind(this));
    this.support.down(".elt-link").observe("click", this.onCancel.bind(this));
  },

  onCancel: function() {
    this.close();
  },

  onSucceed: function() {
    alert(this.input.value);
  },

  getValue: function() {
    return this.input.getValue();
  }

});

Kwo.Class.Subscription = Class.create(Kwo.Dialog, {

  initialize: function($super, elt) {
    this.name = "subscription";
    this.className = "layout-hbox";
    this.width = 400;
    this.height = 300;
    this.args = {"item": $(elt).readAttribute("data-item")};
    $super(this.onDisplay);
  },

  onDisplay: function(args) {
    kwo.post("/subscription.prompt", this.args,
             {"container": this.support,
              "callback": this.onBind.bind(this)});
  },

  onBind: function() {
    var that = this;
    this.support.down("FORM").observe("submit", function(evt) {
      evt.stop();
      that.onSave(evt.element());
    });
  },

  onSave: function(elt) {
    if (!Kwo.isAuth()) {
      var that = this;
      var am = new Kwo.Dialogs.AuthManager();
      am.onCallback = function() {
        that.onSave(elt);
        this.close();
      };
      return ;
    }
    kwo.post("/subscription.save", this.args,
             {"callback": elt});
  }

});

Kwo.Composer.Favorite = Class.create(Kwo.Dialog, {

  initialize: function($super, elt) {
    elt = $(elt);
    this.name = "favorite";
    this.layout = "hbox";
    this.args = {"item": elt.readAttribute("data-item"),
                 "url": elt.readAttribute("data-url")};
    this.width = 400;
    this.height = 320;
    $super(this.onDisplay);
  },

  onDisplay: function() {
    kwo.post("/favorite.prompt", this.args,
             {"container": this.support,
              "callback": this.onBind.bind(this)});
  },

  onBind: function() {
    var that = this;
    this.support.down("FORM").observe("submit", function(evt) {
      evt.stop();
      that.onSave(evt.element());
    });
  },

  onSave: function(elt) {
    if (!Kwo.isAuth()) {
      var that = this;
      var am = new Kwo.Dialogs.AuthManager();
      am.onCallback = function() {
        that.onSave(elt);
        this.close();
      }
      return ;
    }
    kwo.post("/favorite.save", [this.args, elt],
             {"callback": elt.down("UL"),
              "disable": elt});
  }

});

Kwo.Class.Like = Class.create({

  initialize: function(elt) {
    this.elt = $(elt);
    this.onSubmit();
  },

  onSubmit: function() {
    if (!Kwo.isAuth()) {
      var that = this;
      var am = new Kwo.Dialogs.AuthManager();
      am.onCallback = function() {
        that.onSubmit();
        this.close();
      }
      return ;
    }
    var args = {"item": this.elt.readAttribute("data-item")};
    kwo.post("/item.like", args,
             {"callback": this.onCallback.bind(this)});
  },

  onCallback: function(resp) {
    resp = new kwo.Response(resp);
    if (resp.hasError()) return resp.alert();
    this.elt.down("SPAN").update("(" + resp.getAttribute("like_count") + ")");
  }

});

kwo.dialogs.Popup = Class.create(Kwo.Dialog, {

  "id": null,

  initialize: function($super, id) {
    this.name = "popup";
    this.id = id;
    this.width = 550;
    this.height = 400;
    $super(this.paint);
  },

  paint: function() {
    kwo.post("/popup", {"id": this.id},
             {"container": this.support,
              "callback": this.onDomLoaded.bind(this)});
  },

  onDomLoaded: function() {
  }

});

Kwo.Dialogs.FileManager = Class.create(Kwo.Dialog, {

  "is_uploading": false,
  "last_uploaded_file": null,
  "filter": null,

  initialize: function($super, elt, opts) {
    this.name = "file-manager";
    this.elt = $(elt);
    this.className = "layout-hbox";
    this.width = 550;
    this.height = 400;
    opts = opts || {};
    if ("filter" in opts) {
      this.filter = opts["filter"]
    }
    $super(this.onDisplay);
  },

  onDisplay: function() {
    if (this.filter == null) {
      this.filter = this.elt.readAttribute("data-filter") || "";
    }
    this.onRefresh();
  },

  onRefresh: function() {
    var args = {};
    if (this.filter.length > 1) {
      args["filter"] = this.filter;
    }
    kwo.post("/file.manager", args,
             {"container": this.support,
              "callback": this.onBind.bind(this)});
  },

  down: function(expr) { return this.support.down(expr); },
  select: function(expr) { return this.support.select(expr); },

  onBind: function() {
    var that = this;
    var tmp;
    this.select(".side LI").each(function(elt) {
      elt.observe("click", function(evt) {
        evt.stop();
        that.onSwitchPanel(elt);
      });
    });
    this.select(".list LI").each(function(elt) {
      elt.observe("click", function(evt) {
        evt.stop();
        that.onPreview(elt);
      });
      if (that.last_uploaded_file) {
        tmp = elt.readAttribute("data-file");
        if (that.last_uploaded_file == tmp) {
          that.onPreview(elt);
          that.last_uploaded_file = null;
        }
      }
    });
    this.last_uploaded_file = null;
    this.down("FORM").observe("submit", function(evt) {
      evt.stop();
      that.onUpload(evt.element());
    });
    if (this.filter == "image") {
      this.down("INPUT[name=filter]").setValue("image");
      this.down("INPUT[type=file]").writeAttribute("accept", "image/*");
    }
  },

  onPreview: function(elt) {
    kwo.post("/file.preview",
             {"file": elt.readAttribute("data-file")},
             {"container": this.down(".preview"),
              "callback": this.onPreviewCallback.bind(this)});
  },

  onPreviewCallback: function() {
    var that = this;
    this.down(".preview INPUT[type=button]").observe("click", function(evt) {
      evt.stop();
      var btn = evt.element();
      that.onSelect({"name": btn.readAttribute("data-name"),
                     "path": btn.readAttribute("data-path")});
    });
  },

  /* name, path */
  /* SUCHARGER CETTE METHODE */
  onSelect: function(h) {
    if (Object.isElement(this.elt) && this.elt.tagName == "INPUT") {
      console.log(this.elt);
      if (this.elt.hasClassName("field-file")) {
        // a deplacer dans FormManager
        var parts = h["name"].split("/");
        this.elt.setValue(parts.pop());
        this.elt.previous("INPUT").setValue(h["name"]);
      }
      else {
        this.elt.setValue(h["name"]);
      }
      this.close();
      return ;
    }
  },

  onUpload: function(form) {
    if (this.is_uploading === true) return ;
    if (form.down("INPUT[type=file]").value.length < 5) return ;
    var that = this;
    $(document).observe("user:upload:completed", function(evt) {
      that.onUploadCompleted(evt);
    });
    this.is_uploading = true;
    form.action = "/file.upload";
    form.target = "upload-container";
    form.method = "post";
    form.enctype = "multipart/form-data";
    if (Prototype.Browser.IE) {
      form.encoding = "multipart/form-data";
    }
    var img = this.support.down("IMG.progress");
    img.src = img.readAttribute("data-progress");
//    form.enable();
    form.submit();
  },

  onUploadCompleted: function(evt) {
    $(document).stopObserving("user:upload:completed");
    this.is_uploading = false;
    if ("error" in evt.memo) {
      var img = this.support.down("IMG.progress");
      img.src = img.readAttribute("data-initial");
//      this.support.down("FORM").enable();
      return alert(evt.memo.error);
    }
    this.last_uploaded_file = evt.memo.files[0];
    if (evt.memo["user_id"] > 0 && this.down(".list")) {
      this.onRefresh();
    }
    else {
      this.onSelect({"name": this.last_uploaded_file,
                     "path": this.last_uploaded_file})
    }
  },

  onSwitchPanel: function(elt) {
    this.select(".panels > DIV").each(function(elt) {
      elt.hide();
    });
    this.down("." + elt.readAttribute("data-panel")).show();
  }

});

Kwo.Handlers["date"] = {

  "initialize": function($super, elt, opts) {
    this.name = "date";
    this.className = "layout-hbox";
    this.opts = opts || {};
    this.width = this.opts["has_time"] == true ? 350 : 320;
    this.height = 310;
    this.elt = $(elt);
    if ("periods" in this.opts) {
      this.opts["periods"] = Object.toJSON(this.opts["periods"]);
    }
    else if (this.elt.readAttribute("data-periods")) {
      this.opts["periods"] = this.elt.readAttribute("data-periods");
    }

    if (this.elt.readAttribute("data-from")) {
      this.opts["from"] = this.elt.readAttribute("data-from");
    }
    if (this.elt.readAttribute("data-to")) {
      this.opts["to"] = this.elt.readAttribute("data-to");
    }
    if (this.elt.readAttribute("data-default_type")) {
      this.opts["default_type"] = this.elt.readAttribute("data-default_type");
    }
    $super(this.onDisplay, elt);
  },

  onDisplay: function(elt) {
    var v = Object.isElement(elt) ? $F(elt) : elt;
    this.opts["datetime"] = v;
    kwo.post("/date.select", this.opts,
             {"container": this.support});
  },

  onAfterPlace: function() {
    this.listeners = {};
    var that = this;
    this.listeners["change"] = function(event) {
      var target = Event.element(event);
      if (target.hasAttribute("data-change")) {
        that.onDisplay(target.readAttribute("data-change"));
        event.stop();
      }
    };
    this.listeners["click"] = function(event) {
      var target = Event.element(event);
      if (target.hasAttribute("data-date")) {
        if (that.opts["has_time"]) {
          var datetimeinput = that.support.down("[name=\"datetime\"]");
          datetimeinput.value = target.readAttribute("data-date");
          that.support.select('a').invoke('removeClassName', 'selected');
          target.addClassName('selected');
        }
        else {
          that.onSelect(target.readAttribute("data-date"));
        }
        event.stop();
      }
      else if (target.hasAttribute("data-datetime")) {
        that.onSelect(target.readAttribute("data-datetime"));
        event.stop();
      }
      else if (target.hasAttribute("data-change")) {
        that.onDisplay(target.readAttribute("data-change"));
        event.stop();
      }
    };
    this.support.observe("click", this.listeners["click"]);
    this.support.observe("change", this.listeners["change"]);
  },

  onAfterClose: function() {
    this.support.stopObserving("click", this.listeners["click"]);
    this.support.stopObserving("change", this.listeners["change"]);
  },

  onSelect: function(datetime) {
    if (Object.isElement(this.elt)) {
      this.elt.value = datetime;
      if (typeof this.elt.onchange == "function") this.elt.onchange();
      this.close();
      return ;
    }
    alert(datetime);
  }

};

Kwo.Datepicker = Class.create(Kwo.Dialog, Kwo.Handlers["date"]);

Kwo.Dialogs.Date = Class.create(Kwo.Dialog, Kwo.Handlers["date"]);

Kwo.Dialogs.ColorPicker = Class.create(Kwo.Dialog, {

  "strhex": "0123456789ABCDEF",
  "image": "",
  "image_width": 0,
  "image_height": 0,
  "text": "",
  "button": "",
  "preview": "",
  "elt": null,
  "is_mouse_down": false,
  "is_mouse_over": false,

  initialize: function($super, elt) {
    this.name = "color";
    this.elt = $(elt);
    this.width = 220;
    this.height = 180;
    $super(this.onDisplay);
  },

  onDisplay: function() {
    kwo.post("/color.picker", null,
             {"container": this.support,
              "callback": this.onDomPainted.bind(this)});
  },

  onDomPainted: function() {
    var that = this;
    this.image = this.support.down(".image");
    this.text = this.support.down(".text");
    this.button = this.support.down(".button");
    this.preview = this.support.down(".preview");
    this.image.observe("load", function(evt) {
      evt.stop();
      that.onBind();
    });
    if (Object.isElement(this.elt) && !this.elt.getValue().empty()) {
      this.setColor(this.elt.getValue());
    }
  },

  onBind: function() {
    var that = this;
    this.image_width = this.image.getWidth() / 6;
    this.image_height = this.image.getHeight() - 5;

    this.image.observe("mousedown", function(evt) {
      evt.stop();
      that.is_mouse_down = true;
    });
    this.image.observe("mouseup", function(evt) {
      evt.stop();
      that.is_mouse_down = false;
    });
    this.image.observe("mouseover", function(evt) {
      evt.stop();
      that.is_mouse_over = true;
    });
    this.image.observe("mouseout", function(evt) {
      evt.stop();
      that.is_mouse_over = false;
    });
    this.image.observe("click", function(evt) {
      evt.stop();
      that.onColorChange(evt);
    });
    this.image.observe("mousemove", function(evt) {
      evt.stop();
      if (that.is_mouse_down && that.is_mouse_over) {
        that.onColorChange(evt);
      }
    });
    this.text.observe("keyup", function(evt) {
      evt.stop();
      his.preview.style.backgroundColor = "#" + this.getValue();
    });
    this.button.observe("click", function(evt) {
      evt.stop();
      that.onSelect(that.text.getValue());
    });
  },

  setColor: function(color) {
    if ('#' == color.charAt(0)) {
      color = color.substring(1, color.length);
    }
    this.preview.style.backgroundColor = "#" + color;
    this.text.setValue(color);
  },

  dec2hex: function(n) {
    return this.strhex.charAt(Math.floor(n / 16)) + this.strhex.charAt(n % 16);
  },

  onColorChange: function(evt) {
//    var x = evt.offsetX ? evt.offsetX : (evt.target ? evt.layerX - evt.target.x : 0);
//    var y = evt.offsetY ? evt.offsetY : (evt.target ? evt.layerY - evt.target.y : 0);
    var x = evt.layerX || evt.offsetX;
    var y = evt.layerY || evt.offsetY;
    var width = this.image_width;
    var height = this.image_height;

    if ((x >= 0 && x <= 110) && (y >= 0 && y <= 5)) {
      var red = 255;
      var green = 255;
      var blue = 255;
    }
    else if ((x > 110 && x <= 220) && (y >= 0 && y <= 5)) {
      var red = 0;
      var green = 0;
      var blue = 0;
    }
    else {
      y -= 5;
      var red = (x >= 0) * (x < width) * 255
        + (x >= width) * (x < 2*width) * (2 * 255 - x * 255 / width)
        + (x >= 4 * width) * (x < 5 * width) * (-4 * 255 + x * 255 / width)
        + (x >= 5 * width) * (x < 6 * width) * 255;
      var blue = (x >= 2 * width) * (x < 3 * width) * (-2 * 255 + x * 255 / width)
        + (x >= 3 * width) * (x < 5 * width) * 255
        + (x >= 5 * width) * (x < 6*width) * (6 * 255 - x * 255 / width);
      var green = (x >= 0) * (x < width) * (x * 255 / width)
        + (x >= width) * (x < 3 * width) * 255
        + (x >= 3 * width) * (x < 4 * width) * (4 * 255 - x * 255 / width);

      var coef = (height - y) / height;

      red = 128 + (red - 128) * coef;
      green = 128 + (green - 128) * coef;
      blue = 128 + (blue - 128) * coef;
    }
    var color = this.dec2hex(red) + this.dec2hex(green) + this.dec2hex(blue);
    this.setColor(color);
  },

  onSelect: function(color) {
    if ('#' == color.charAt(0)) {
      color = color.substring(1, color.length);
    }
    if (Object.isElement(this.elt)) {
      this.elt.setValue(color);
    }
    this.close();
  }

});

Kwo.Class.Snippet = Class.create(Kwo.Dialog, {

  initialize: function($super, code) {
    this.name = "snippet";
    if (Object.isElement(code)) {
      code = code.readAttribute("data-code");
    }
    this.args = {"code": code};
    this.className = "layout-hbox";
    this.width = 550;
    this.height = 400;
    $super(this.onDisplay);
  },

  onDisplay: function() {
    kwo.get("/snippet", this.args,
            {"container": this.support});
  }

});

Kwo.Class.Faq = Class.create(Kwo.Dialog, {

  initialize: function($super, elt) {
    this.name = "faq";
    this.className = "layout-hbox";
    this.args = {"code": $(elt).readAttribute("data-code")};
    this.width = 600;
    this.height = 450;
    $super(this.onDisplay);
  },

  onDisplay: function() {
    kwo.get("/faq", this.args,
             {"container": this.support,
              "callback": this.onBind.bind(this)});
  },

  onBind: function() {
    var that = this;
    this.support.select(".event-rewind").each(function(elt) {
      elt.observe("click", function(evt) {
        evt.stop();
        that.support.down("DIV").scrollTop = 0;
      });
    });
    this.support.select(".event-select").each(function(elt) {
      elt.observe("click", function(evt) {
        evt.stop();
        that.onSelect(evt);
      });
    });
  },

  onSelect: function(evt) {
    var id = evt.element().readAttribute("data-id");
    var pos = $("component-" + id).positionedOffset();
    this.support.down("DIV").scrollTop = pos[1];
  }

});


Kwo.Composer.Feed = Class.create(Kwo.Dialog, {

  initialize: function($super, elt) {
    this.name = "feed";
    this.className = "layout-hbox";
    this.args = {"url": $(elt).readAttribute("data-url")};
    this.width = 400;
    this.height = 340;
    $super(this.onDisplay, this.args);
  },

  onDisplay: function() {
    kwo.post("/feed.prompt", this.args,
             {"container": this.support});
  }

});

Kwo.Class.Valuation = Class.create(Kwo.Dialog, {

  initialize: function($super, args) {
    this.name = "valuation";
    this.className = "layout-hbox";
    this.args = {"item": $(args).readAttribute("data-item")};
    this.width = 700;
    this.height = 400;
    $super(this.onDisplay, this.args);
  },

  onDisplay: function() {
    kwo.post("/valuation", this.args,
             {"container": this.support,
              "callback": this.onBind.bind(this)});
  },

  onBind: function() {
    var that = this;
    $$(".valuation-stars")[0].observe("click", this.onClick);
    this.support.down("FORM").observe("submit", function(evt) {
      evt.stop();
      that.onSubmit(evt.element());
    });
  },

  onSubmit: function(elt) {
    kwo.post("/valuation.save",
             [elt, {"item": this.args["item"]}],
             {"disable": elt,
              "callback": this.onCallback.bind(this)});
  },

  onCallback: function(resp) {
    resp = new kwo.Response(resp);
    if (resp.hasError()) return resp.alert();
    this.close();
    Kwo.reload();
  },

  onClick: function(evt) {
    var elt = evt.element();
    if (!elt.hasClassName("star")) return ;
    elt.up("TR").select("IMG.star").each(function (img) {
      img.src = img.src.sub("-on", "-off");
    });
    elt.src = elt.src.sub("-off", "-on");
    elt.previousSiblings().each(function (img) {
      img.src = img.src.sub("-off", "-on");
    });
    elt.up("TR").down("SPAN").update(elt.readAttribute("data-value") + " / 5");
    elt.up("TR").down("INPUT").value = elt.readAttribute("data-value");
  }

});


Kwo.Elt = {

  onTagAdd: function(elt) {
    if (elt.value.blank() || elt.value == elt.title) return ;
    var tags = elt.getValue().stripTags().split(",");
    var input = elt.previous(0).getValue().split(",");
    elt.clear();
    var tag, span, spans = elt.next("DIV");
    for (var i = 0; i < tags.length; i++) {
      tag = tags[i].strip();
      if (tag.blank() || input.indexOf(tag) != -1) continue ;
      span = new Element("SPAN", {onclick: "Kwo.Elt.onTagEdit($(this))"}).update(tag);
      spans.insert(span);
    }
    Kwo.Elt.onTagStore(spans);
  },

  onTagEdit: function(elt) {
    var spans = elt.up();
    spans.previous("INPUT").value = elt.innerHTML;
    elt.remove();
    Kwo.Elt.onTagStore(spans);
  },

  onTagFocus: function(elt) {
    elt = $(elt);
    if (elt.value == elt.title) {
      elt.clear();
    }
    elt.next().show()
  },

  onTagStore: function(elt) {
    var tags = [];
    elt.select("SPAN").each(function (span) {
      tags.push(span.innerHTML);
    });
    elt.previous("INPUT", 1).value = tags.join(",");
  },

  onTooltipHide: function(elt) {
    elt.up("DIV.elt").select("DIV.elt-tooltip")[0].hide();
  },

  onTooltipShow: function(elt) {
    var tooltip = elt.up("DIV.elt").select("DIV.elt-tooltip")[0];
    if (!tooltip.hasClassName("elt-widget")) {
      tooltip.addClassName("elt-widget");
      var pointer = new Element("DIV").addClassName("elt-tooltip-pointer-down elt-widget");
      pointer.insert(new Element("DIV").addClassName("elt-tooltip-pointer-down-inner"));
      tooltip.insert(pointer);
    }
    var pos = elt.cumulativeOffset();
    var dim = tooltip.getDimensions();
    var top  = pos["top"] - (dim["height"] + 16);
    var left = (pos["left"] - Math.ceil(dim["width"] / 2)) + 5;
    tooltip.setStyle({"top": top + "px", "left": left + "px"});
    tooltip.show();
  },

  onNodeSelect: function(elt, target_id) {
    elt = $(elt);
    var target = $(target_id);
    if (elt.getValue() == 0) {
      target.length = 0;
      target.update('<option value="0">-</option>');
      return ;
    }
    kwo.post("/node.children", {"node_id": elt.getValue()},
             {"callback": target});
  },

  onImageObserve: function(input, img) {
    var fm = new Kwo.Dialogs.FileManager(input, {"filter": "image"});
    fm.onSelect = function(h) {
      var path = h["name"];
      input.value = path.basename();
      input.hide();
      img.src = "/" + path;
      img.show();
      fm.close();
    }
  },

  onImageSelect: function(elt) {
    var input, img;
    if (elt.tagName === "IMG") {
      img = elt;
      input = elt.previous();
    }
    else {
      input = elt;
      img = elt.next();
    }
    Kwo.Elt.onImageObserve(input, img);
  }

};

Kwo.Currency = {

  onSelect: function(arg) {
    $("kwo-currencies").toggle();
    var pos = $(arg).cumulativeOffset();
    $("kwo-currencies").setStyle({"top": (pos[1] + $(arg).getHeight()) + "px",
                                  "left": pos[0] + "px"});
  },

  onSubmit: function(code) {
    $("kwo-currencies").hide();
    kwo.post("/currency.set", {"code": code},
             {"callback": Kwo.reload});
  }

};

Object.extend(String.prototype, {

  intval: function() {
    if (this.length < 1) return 0;
    var val = this.replace(/^0+/, "");
    if (val.length < 1) return 0;
    return parseInt(val);
  },

  toggle: function() {
    return parseInt(this) == 1 ? "0" : "1";
  },

  toLocaleDate: function() {
    var out = this;
    if (this.length > 5 && this.match(/-/)) {
      parts = this.split("-");
      out = parts[2] + "/" + parts[1] + "/" + parts[0];
    }
    return out;
  },

  ucfirst: function() {
   return this.charAt(0).toUpperCase() + this.substring(1);
  },

  basename: function() {
    return this.replace(/.*\//g, '');
  },

  dirname: function() {
    return this.match(/(.*)[\/\\]/)[1];
  },

  isImage: function() {
    return this.match(/\.(gif|jpeg|jpg|png)$/i);
  },

  stripAccents: function() {
    var exps = [ /[\xC0-\xC2]/g, /[\xE0-\xE2]/g,
                 /[\xC8-\xCA]/g, /[\xE8-\xEB]/g,
                 /[\xCC-\xCE]/g, /[\xEC-\xEE]/g,
                 /[\xD2-\xD4]/g, /[\xF2-\xF4]/g,
                 /[\xD9-\xDB]/g, /[\xF9-\xFB]/g ];
    var chars = ["A","a","E","e","I","i","O","o","U","u"];
    var s = this, l = exps.length;
    for (var i = 0; i < l; i++) {
      s = s.replace(exps[i], chars[i]);
    }
    return s;
  },

  toFunction: function() {
    var parts = this.split(".");
    var func = window;
    for (var i = 0; i < parts.length; i++) {
      func = func[parts[i]];
    }
    return func;
  }

});

Element.addMethods({

  turn: function(elt) {
    elt = $(elt);
    var img;
    if (elt.tagName.toUpperCase() == "IMG") {
      img = elt;
    }
    else {
      img = elt.down("IMG");
      if (Object.isUndefined(img)) return elt;
    }
    if (img.src.match("-off.")) {
      img.src = img.src.sub("-off.", "-on.", 1);
    }
    else {
      img.src = img.src.sub("-on.", "-off.", 1);
    }
    return elt;
  },

  inModal: function(elt) {
    elt = $(elt);
    return elt.up(".dialog-support") != undefined;
  },

  inDialog: function(elt) {
    elt = $(elt);
    return elt.up(".dialog") != undefined && elt.up(".deck") == undefined ;
  },

  updateWithProvider: function(elt) {
    var provider = elt.readAttribute("data-content-provider");
    if (provider) {
      kwo.get("/" + provider, {}, {"container": elt});
    }
  },

  fireDialogEvent: function(elt) {
    var values = elt.readAttribute("data-values");
    if (values != null && values.length > 1) {
      values = values.evalJSON();
    }
    else {
      values = {};
    }
    elt.up(".container").fire(elt.readAttribute("data-event"), values);
  }

});

Kwo.Composer.Abuse = Class.create(Kwo.Dialog, {

  initialize: function($super, elt) {
    this.name = "abuse";
    this.className = "layout-hbox";
    this.width = 440;
    this.height = 330;
    this.elt = $(elt);
    this.args = {"item": this.elt.readAttribute("data-item")};
    $super(this.onDisplay);
  },

  onDisplay: function(args) {
    kwo.post("/abuse.compose", this.args,
             {"container": this.support,
              "callback": this.onBind.bind(this)});
  },

  onBind: function() {
    this.support.down("FORM").observe("submit",
                                      this.onSubmit.bindAsEventListener(this));
  },

  onSubmit: function(evt) {
    evt.stop();
    var elt = evt.element();
    kwo.post("/abuse.report", elt,
             {"disable": elt,
              "callback": $("abuse-button")});
  }

});

Kwo.Composer.Comment = Class.create(Kwo.Dialog, {

  initialize: function($super, elt) {
    this.name = "comment";
    this.className = "layout-hbox";
    this.width = 600;
    this.height = 400;
    this.elt = $(elt);
    this.args = {"item": this.elt.readAttribute("data-item")};
    $super(this.onDisplay);
  },

  onDisplay: function() {
    kwo.post("/comment.compose", this.args,
             {"container": this.support,
              "callback": this.onBind.bind(this)});
  },

  onBind: function() {
    var that = this;
    this.support.down("FORM").observe("submit", function(evt) {
      evt.stop();
      that.onSubmit();
    });
  },

  onSubmit: function() {
    kwo.post("/item.comment", [this.args, this.support.down("FORM")],
             {"disable": this.elt,
              "callback": this.onCallback.bind(this)});
  },

  onCallback: function(resp) {
    resp = new kwo.Response(resp);
    if (resp.isAuthError()) {
      var that = this;
      var am = new Kwo.Dialogs.AuthManager();
      am.onCallback = function() {
        that.onSubmit();
        return ;
      };
    }
    if (resp.hasError()) return resp.alert();
    kwo.post("/comments", this.args,
             {"container": this.elt.up(".widget-comments")});
    this.close();
  }

});


Kwo.Class.Form = Class.create({

  form: null,

  initialize: function(elt) {
    var that = this;
    elt = $(elt);
    this.form = elt.previous();
    this.form.observe("submit", function(evt) {
      evt.stop();
      that.onSubmit();
    });
    elt.remove();
  },

  onSubmit: function() {
    this.form.select(".form-field").each(function (elt) {
      elt.removeClassName("form-field-failure");
    });
    var action = this.form.readAttribute("data-action") || "/submission.save";
    kwo.post(action, this.form,
             {"callback": this.onSubmitCallback.bind(this),
              "disable": this.form});
    return false;
  },

  onSubmitCallback: function(resp) {
    var that = this;
    resp = new kwo.Response(resp);
    if (resp.isAuthError()) {
      var am = new Kwo.Dialogs.AuthManager();
      am.onCallback = function() {
        that.onSubmit();
        this.close();
      }
      return ;
    }
    var result = resp.getResult();
    if (!("failures" in result)) {
      this.form.hide();
      this.form.next("DIV").update(result["ack"]).show();
      return ;
    }
    result["failures"].each(function(field_id) {
      that.form.down(".form-field-" + field_id).addClassName("form-field-failure");
    });
  }

});


Kwo.Comment = {

  onSwitchPage: function(elt, offset) {
    var widget = $(elt).up(".widget-comments")
    var args = {};
    args["offset"] = offset;
    args["item"] = widget.readAttribute("data-item");
    kwo.post("/comments", args,
             {"container": widget});
  }

};

Kwo.Poll = {

  onCallback: function(resp) {
    resp = new kwo.Response(resp);
    if (resp.hasError()) return resp.alert();
    kwo.post("/poll.result",
             {"id": resp.getAttribute("poll_id"),
              "choice_id": resp.getAttribute("choice_id")},
             {"container": this});
  },

  onSubmit: function(args) {
    kwo.post("/poll.vote", args,
             {"callback": Kwo.Poll.onCallback.bind($(args))});
  }

};

Kwo.Search = {

  "results": function(query, offset) {

    var args = {};
    args["offset"] = offset === undefined ? 0 : offset;
    if (query === undefined) {
      if ($("kwo-search-input")) {
        args["query"] = $F('kwo-search-query');
      }
      else {
        return false;
      }
    }
    args["query"] = query;

    if (args["query"].length < 1) return false ;

    Kwo.go("/search.results", args);

  }

};

Kwo.Glossary = {

  "term": "",

  "selectLetter": function(letter, glossary_id) {
    $("kwo-letter-" + letter).up(".kwo-letters").select("A").invoke("removeClassName", "selected");
    $("kwo-letter-" + letter).addClassName("selected");
    kwo.get("/terms",
            {"glossary_id": glossary_id, "letter": letter, "term": Kwo.Glossary.term},
            {"container": "kwo-terms"});
    Kwo.Glossary.term = "";
  },

  "selectTerm": function(element) {
    element = $(element);
    element.up("ul").select("p").invoke("hide");
    element.next("p").show();
  }

};

Kwo.Class.RateManager = Class.create({

  "elt": null,
  "max": null,

  initialize: function(elt) {
    this.elt = $(elt).previous().down(".rate-notes");
    elt.remove();
    this.max = parseInt(this.elt.readAttribute("data-max"));
    this.onBind();
  },

  onBind: function() {
    var that = this;
    this.elt.select(".rate-note").each(function(elt) {
      elt.src = elt.readAttribute("data-src");
      elt.observe("mouseover", function(evt) {
        evt.stop();
        that.onMouseOver(elt);
      });
    });
    this.elt.observe("click", function(evt) {
      evt.stop();
      var elt = evt.element();
      if (!elt.readAttribute("data-note")) return ;
      that.onClick(elt);
    });
    this.elt.observe("mouseout", function(evt) {
      evt.stop();
      that.onMouseOut();
    });
  },

  onClick: function(elt) {
    var args = {"item": this.elt.readAttribute("data-item"),
                "note": elt.readAttribute("data-note")};
    kwo.post("/note.save", args,
             {"callback": this.onClickCallback.bind(this)});
  },

  onClickCallback: function(resp) {
    resp = new kwo.Response(resp);
    if (resp.hasError()) return ;
    this.elt.update(resp.getMessage());
  },

  onMouseOut: function() {
    this.elt.select(".rate-note").each(function(elt) {
      elt.src = elt.readAttribute("data-src");
    });
  },

  onMouseOver: function(elt) {
    var i, img;
    var note = parseInt(elt.readAttribute("data-note"));
    for (i = 1; i <= note; i++) {
      img = this.elt.down(".rate-note-" + i);
      if (!img) break ;
      img.src = img.src.sub("-off", "-on");
    }
    while (i <= this.max) {
      img = this.elt.down(".rate-note-" + i);
      if (!img) break ;
      img.src = img.src.sub("-on", "-off");
      i++;
    }
  }

});

Kwo.Class.ThumbManager = Class.create({

  "elt": null,

  initialize: function(elt) {
    this.elt = $(elt).previous().down(".rate-thumbs");
    elt.remove();
    this.onBind();
  },

  onBind: function() {
    var that = this;
    this.elt.select(".rate-thumb").each(function(elt) {
      elt.observe("mouseover", function(evt) {
        evt.stop();
        elt.turn();
      });
      elt.observe("mouseout", function(evt) {
        evt.stop();
        elt.turn();
      });
    });
    this.elt.observe("click", function(evt) {
      evt.stop();
      var elt = evt.element();
      if (!elt.readAttribute("data-direction")) return ;
      that.onClick(elt);
    });
  },

  onClick: function(elt) {
    var args = {"item": this.elt.readAttribute("data-item"),
                "direction": elt.readAttribute("data-direction")};
    kwo.post("/thumb.save", args,
             {"callback": this.onClickCallback.bind(this)})
  },

  onClickCallback: function(resp) {
    resp = new kwo.Response(resp);
    if (resp.hasError()) return resp.alert();
    this.elt.update(resp.getMessage());
  }

});

Kwo.Subscriptions = {

  onSubmit: function(args) {
    kwo.post("/account/subscriptions.set", args,
             {"disable":true, "callback":Kwo.Account.refresh});
  }

};

Kwo.Newsletter = {

  onRecipientSave: function(elt) {
    kwo.post("/account/recipient.save", elt,
             {"disable": true, "callback": true});
  },

  onSubmit: function(elt) {
    elt = $(elt);
    var input = elt.down("INPUT[data-placeholder]");
    if (input) {
      if (input.readAttribute("data-placeholder") == input.getValue()) {
        input.value = "";
        input.focus();
        return ;
      }
    }
    kwo.post("/newsletter.subscribe", elt,
             {"disable":true, "reset":true,
              "callback": Kwo.Newsletter.onCallback.bind(elt)});
  },

  onCallback: function(resp) {
    resp = new kwo.Response(resp);
    if (resp.hasError()) return resp.alert();
    var box = this.down(".confirmation-box");
    if (box) {
      var msg = resp.getMessage().ucfirst() + ".";
      box.addClassName("confirmation").update(msg);
    }
  }

};

Kwo.Class.Newsletter = Class.create(Kwo.Dialog, {

  initialize: function($super, elt) {
    this.name = "newsletter";
    this.className = "layout-hbox";
    this.width = 500;
    this.height = 300;
    this.args = {"item": $(elt).readAttribute("data-item")};
    $super(this.onDisplay, this.args);
  },

  onDisplay: function() {
    kwo.post("/newsletter.signup", this.args,
             {"container": this.support});
  }

});

Kwo.Forum = {

  editTopic: function(args) {
    Kwo.go("/topic.edit", args);
  },

  onTopicSubmit: function(args) {
    kwo.post("/topic.save", args,
             {"callback": Kwo.Forum.onTopicCallback,
              "disable": true});
  },

  onTopicCallback: function(resp) {
    resp = new kwo.Response(resp);
    resp.hasError() ? resp.alert() : Kwo.go(resp);
  }

};

Kwo.Topic = {

  editComment: function(args) {
    Kwo.go("/comment.edit", args);
  },

  onCommentSubmit: function(args) {
    kwo.post("/item.comment", args,
             {"callback": Kwo.Topic.onCommentCallback,
              "disable":true});
  },

  onCommentCallback: function(resp) {
    resp = new kwo.Response(resp);
    resp.hasError() ? resp.alert() : resp.redirect();
  }

};

Kwo.Composer.Alert = Class.create(Kwo.Dialog, {

  submited: false,

  initialize: function($super, elt) {
    elt = $(elt);
    this.name = "alert";
    this.className = "layout-hbox";
    this.args = {"keyword": elt.readAttribute("data-keyword")};
    this.width = 400;
    this.height = 250;
    $super(this.onDisplay, this.args);
  },

  onDisplay: function() {
    kwo.post("/alert.prompt", this.args,
             {"container": this.support,
              "callback": this.onBind.bind(this)});
  },

  onBind: function() {
    var that = this;
    this.support.down("FORM").observe("submit", function (evt) {
      evt.stop();
      var elt = evt.element();
      that.onSave(elt);
    });
  },

  onSave: function(args) {
    if (!Kwo.isAuth()) {
      var that = this;
      var am = new Kwo.Dialogs.AuthManager();
      am.onCallback = function() {
        that.onSave(args);
        this.close();
      }
      return ;
    }
    this.submited = true;
    kwo.post("/alert.save", args,
             {"container": this.support});
  },

  onAfterClose: function() {
    if (this.submited === true && "_scope" in window &&
        window["_scope"] === "account") {
      Kwo.go("alert.list");
    }
  }

});

Kwo.Composer.Share = Class.create(Kwo.Dialog, {

  initialize: function($super, elt) {
    elt = $(elt);
    this.name = "share";
    this.width = 400;
    this.height = 480;
    this.className = "layout-hbox";
    this.args = {"item": elt.readAttribute("data-item"),
                 "mode": elt.readAttribute("data-mode"),
                 "url": elt.readAttribute("data-url")};
    $super(this.onDisplay, this.args);
  },

  onDisplay: function() {
    kwo.post("/notification.prompt", this.args,
             {"container": this.support,
              "callback": this.onBind.bind(this)});
  },

  onBind: function() {
    var that = this;
    this.support.select(".event-redirect").each(function(elt) {
      elt.observe("click", that.onRedirect.bindAsEventListener(that));
    });
    this.support.down("FORM").observe("submit", function(evt) {
      evt.stop();
      that.onSend(evt.element());
    });
  },

  onRedirect: function(evt) {
    evt.stop();
    this.args["service_id"] = evt.element().readAttribute("data-service-id");
    Kwo.go("/notification.redirect", this.args,
           {"target": "blank"});
    return false;
  },

  onSend: function(elt) {
    if ($("recipients").value.empty()) {
      $("recipients").addClassName("error");
      return ;
    }
    kwo.post("/notification.send", [elt, this.args],
             {"disable": elt,
              "reset": elt,
              "callback": elt.down("DIV")});
  }

});

// SHARE

Kwo.Class.SocialContacts = Class.create(Kwo.Dialog, {

  initialize: function($super, elt) {
    this.name = "social-contacts";
    this.className = "layout-hbox";
    this.width = 400;
    this.height = 550;
    $super(this.onDisplay);
  },

  onDisplay: function() {
    kwo.post("/import.prompt", null,
             {"container": this.support,
              "callback": this.onAuthBind.bind(this)});
  },

  onAuthBind: function() {
    var that = this;
    this.support.down("FORM.auth").observe("submit", function(evt) {
      evt.stop();
      that.onAuth(evt.element());
    });
  },

  onAuth: function(elt) {
    $("import-contact-login").removeClassName("error");
    $("import-contact-pass").removeClassName("error");
    if ($("import-contact-login").value == "") {
      $("import-contact-login").addClassName("error");
    }
    if ($("import-contact-pass").value == "") {
      $("import-contact-pass").addClassName("error");
    }
    elt = $(elt);
    kwo.post("/import.contacts", elt,
             {"container": $("contact-import-result"),
              "callback": this.onImportBind.bind(this)});
  },

  onImportBind: function() {
    var that = this;
    this.support.down("FORM.emails").observe("submit", function(evt) {
      evt.stop();
      that.onChoose(evt.element());
    });
  },

  onChoose: function(elt) {
    var contacts = [];
    elt.select("INPUT[type=checkbox]").each(function(input) {
      if (input.checked) contacts.push(input.value);
    });
    if (contacts.length < 1) {
      return alert("Veuillez sélectionner au moins un contact.");
    }
    this.onSelect(contacts);
  },

  onSelect: function(contacts) {
    console.log(contacts);
  }

/*  onSend: function(elt) {
    elt = $(elt);
    if ($("recipients").value.empty()) {
      $("recipients").addClassName("error");
      return ;
    }
    kwo.post("/notification.send", [elt, this.args],
             {"disable": elt, "reset": elt, "callback": elt.down("DIV")});
  } */

});

//--shop

Kwo.Cart = {

  onCouponPreCheck: function(elt) {
    elt = $(elt);
    if (elt.getValue().empty()) return ;
    kwo.post("/coupon.precheck", {"coupon": elt.getValue()},
             {"callback": function(resp) {
               resp = new kwo.Response(resp);
               if (resp.hasError()) {
                 elt.setValue("");
                 return resp.alert();
               }
               alert(resp.getMessage());
             }})
  },

  addPurchase: function(item, quantity) {
    var args = {"quantity": 1};
    if (Object.isElement(item)) {
      item = $(item);
      if ($(item).tagName.toUpperCase() === "FORM") {
        args = item;
      }
      else if ($(item).readAttribute("data-item")) {
        args["item"] = $(item).readAttribute("data-item");
      }
    }
    else {
      args["item"] = item;
      args["quantity"] = quantity || 1;
    }
    kwo.post("/cart.add", args,
            {"callback": Kwo.Cart.onPurchaseCallback});
  },

  confirmPurchase: function(msg) {
    if ("onPurchaseCallback" in window) {
      window.purchaseConfirm.call(this, msg);
    }
    else {
      Kwo.warn(msg);
    }
  },

  empty: function(elt) {
    kwo.post("/cart.empty", null,
             {"callback": Kwo.Cart.onUpdateCallback, "confirm": elt});
  },

  onPurchaseCallback: function(resp) {
    resp = new kwo.Response(resp);
    if (resp.hasError()) return resp.alert();
    Kwo.refresh("has-cart");
    $(document).fire("kwo:purchase:added");
    var pc = new Kwo.Dialogs.PurchaseConfirm(resp.getAttribute("item"));
  },

  onQuantityChange: function(elt) {
    elt = $(elt);
    elt.up("TABLE.cart").select(".total .quantity A")[0].show();
  },

  onUpdate: function(args) {
    kwo.post("/cart.update", args,
             {"callback": Kwo.Cart.onUpdateCallback});
  },

  onPurchaseDelete: function(elt) {
    elt = $(elt);
    var id = elt.readAttribute("data-id");
    var args = {};
    args["purchases[" + id + "]"] = 0;
    kwo.post("/cart.update", args,
             {"callback": Kwo.Cart.onUpdateCallback,
              "confirm": elt});
  },

  onUpdateCallback: function(resp) {
    resp = new kwo.Response(resp);
    if (resp.hasError()) {
      resp.alert();
      Kwo.Cart.view();
      return ;
    }
    if (resp.getAttribute("purchase_count") >= 1) {
      Kwo.Cart.view();
    }
    else {
      Kwo.go("/cart");
    }
  },

  view: function() {
    Kwo.go("/cart");
  }

};

Kwo.Dialogs.PurchaseConfirm = Class.create(Kwo.Dialog, {

  initialize: function($super, item) {
    this.name = "purchase-confirm";
    this.className = "layout-hbox";
    this.width = 600;
    this.height = 300;
    this.item = item;
    $super(this.onDisplay);
  },

  onDisplay: function() {
    kwo.post("/purchase.confirm", {"item": this.item},
             {"container": this.support,
              "callback": this.onBind.bind(this)});
  },

  onBind: function() {
    var that = this;
    this.support.down(".event-close").observe("click", function (evt) {
      that.close();
    });
  }

});


Kwo.Order = {

  current_step: null,

  onCouponFocus: function(elt) {
    elt.up(".order-coupon-compose").addClassName("order-coupon-focus");
  },

  onCouponCheck: function(elt) {
    var box = elt.up(".order-coupon-compose");
    var input = box.down("INPUT");
    box.down(".order-coupon-message").hide();
    kwo.post("/coupon.check", {"coupon": input.getValue()},
             {"callback": Kwo.Order.onCouponCheckCallback.bind(elt)});
  },

  onCouponRemove: function(elt) {
    var box = elt.up(".order-coupon-compose");
    box.down(".order-coupon-message").hide();
    kwo.post("/coupon.remove", {},
             {"callback": function (resp) {
               kwo.post("/order.coupon.compose", {},
                        {"container": box});
               Kwo.Order.onChange();
             }});
  },

  onCouponCheckCallback: function(resp) {
    var elt = this;
    var box = elt.up(".order-coupon-compose");
    resp = new kwo.Response(resp);
    if (resp.hasError()) {
      var msg = '<div class="error">' + resp.getError() + '</div>';
      box.down(".order-coupon-message").update(msg).show();
      return ;
    }
    Kwo.Order.onChange();
    kwo.post("/order.coupon.compose", {},
             {"container": box,
              "callback": function() {
                var msg = "<div>" + resp.getMessage() + "</div>";
                box.down(".order-coupon-message").update(msg).show();
                setTimeout(function () {
                  box.down(".order-coupon-message").update();
                }, 2000);
              }});

  },

  onFinalize: function(elt) {
    //$(elt).hide();
    elt = $(elt).up("FORM");
    kwo.post("/order.update", [elt, {"step": "finalize"}],
             {"callback": this.onCallback.bind(elt), "disable": elt});
  },

  compose: function() {
    if (!Kwo.isAuth()) {
      var am = new Kwo.Dialogs.AuthManager();
      am.onCallback = function() {
        Kwo.Order.compose();
      }
      return ;
    }
    Kwo.go("/order");
  },

  onCallback: function(resp) {
    resp = new kwo.Response(resp);
    if (resp.hasError()) return resp.alert();
    kwo.post("/payment.request", null,
             {"container": $("psp-container")});
  },

  onChange: function(elt) {
    var order = $$(".order")[0];
    var args = order.down("FORM");
    kwo.post("/order.update", args,
             {"callback": function (resp) {
               resp = new kwo.Response(resp);
               if (resp.hasError()) return resp.alert();
               order.select(".order-dynamic-box").each(function (elt) {
                 kwo.post("/order." + elt.readAttribute("data-action"), {},
                          {"container": elt});
               });
               $("kwo-amounts-box").update('<img src="/web/core/images/throbbers/default.gif" />');
               kwo.post("/order.amounts", null,
                        {"container": "kwo-amounts-box"});
             }});

  },

  onStepNext: function(elt) {
    elt = $(elt);
    this.current_step = elt.up(".order").down(".order-step-selected");
    var args = {"step": this.current_step.readAttribute("data-step")};
    kwo.post("/order.update", [elt.up("FORM"), args],
             {"disable": elt.up("FORM"),
              "callback": this.onStepCallback.bind(this)});
  },

  onStepCallback: function (resp) {
    resp = new kwo.Response(resp);
    if (resp.hasError()) {
      return resp.alert();
    }
    $("kwo-amounts-box").update('<img src="/web/core/images/throbbers/default.gif" />');
    kwo.post("/order.amounts", null,
             {"container": "kwo-amounts-box"});
    this.current_step.addClassName("order-step-visited").removeClassName("order-step-selected");
    this.next_step = this.current_step.next().addClassName("order-step-selected");
    this.current_step.up(".order-steps")
                     .removeClassName("order-steps-" + this.current_step.readAttribute("data-step"))
                     .addClassName("order-steps-" + this.next_step.readAttribute("data-step"));
    this.current_step.removeClassName("order-steps-" + this.current_step.readAttribute("data-step"));
    kwo.post("/order." + this.next_step.readAttribute("data-step"), null,
             {"container": $("order-section")});
  }

};

Kwo.Class.Addressee = Class.create(Kwo.Dialog, {

  initialize: function($super, elt) {
    this.name = "addressee";
    this.className = "layout-hbox";
    this.width = 500;
    this.height = 440;
    this.elt = $(elt);
    $super(this.onDisplay);
  },

  onDisplay: function() {
    kwo.post("/addressee.edit",
             {"id": this.elt.readAttribute("data-id")},
             {"container": this.support,
              "callback": this.onBind.bind(this)});
  },

  onBind: function() {
    var that = this;
    this.support.down("FORM").observe("submit", function(evt) {
      evt.stop();
      that.onSubmit();
    });
  },

  onSubmit: function() {
    var form = this.support.down("FORM");
    var args = {"id": this.elt.readAttribute("data-id")};
    kwo.post("/addressee.save", [args, form],
             {"callback": this.onCallback.bind(this),
              "disable": form});
  },

  onCallback: function(resp) {
    resp = new kwo.Response(resp);
    if (resp.hasError()) return resp.alert();
    var addressee_id = resp.getAttribute("id");
    kwo.post("/order.shipping", {"addressee_id": addressee_id},
             {"container": $("order-section"),
              "callback": function () {
                $("addressee-" + addressee_id).checked = true;
                Kwo.Order.onChange();
              }});
    this.close();
  }

});

Kwo.Composer.Return = Class.create(Kwo.Dialog, {

  elt: null,
  form: null,

  initialize: function($super, elt) {
    this.elt = $(elt);
    this.name = "return";
    this.layout = "hbox";
    this.args = {"id": this.elt.up("TR").readAttribute("data-id")};
    this.width = 500;
    this.height = 350;
    $super(this.onDisplay);
  },

  onDisplay: function() {
    kwo.post("/order.return.compose", {},
             {"container": this.support,
              "callback": this.onBind.bind(this)});
  },

  onBind: function() {
    this.form = this.support.down("FORM");
    this.form.onsubmit = "return false;"
    this.form.down(".submit").observe("click", this.onSubmit.bindAsEventListener(this));
    this.form.observe("submit", this.onSubmit.bindAsEventListener(this));
  },

  onSubmit: function(evt) {
    evt.stop();
    kwo.post("/order.return.save", [this.args, this.form],
             {"callback": this.form, "disable": this.form});
  }

});

//--social


Kwo.Auth = {

  "dialog": null,

  onLogIn: function(elt) {
    elt = $(elt);
    kwo.post("/user.login", elt,
             {"callback": Kwo.Auth.onAuthCallback,
              "disable": true});
  },

  onSignUp: function(elt) {
    elt = $(elt);
    var input = elt.down(".terms_of_use");
    if (!Object.isUndefined(input) && !input.checked) {
      return Kwo.warn(input.readAttribute("data-confirm"));
    }
    kwo.post("/user.signup", elt,
             {"callback": Kwo.Auth.onAuthCallback,
              "disable": true});
  },

  onAuthCallback: function(resp) {
    resp = new kwo.Response(resp);
    if (resp.hasError()) return resp.alert();
    window["_user_id"] = resp.getAttribute("user_id");
    Kwo.refresh("has-user");
    $(document).fire("kwo:user:logged");
    if (Kwo.Auth.dialog) {
      Kwo.Auth.dialog.onCallback();
      return ;
    }
    if ("onAuthCallback" in window) {
      window.onAuthCallback();
    }
    else if (window.location.href.indexOf("sign") != -1) {
      Kwo.home();
    }
    else {
      Kwo.reload();
    }
  },

  onPasswordRequest: function(args) {
    kwo.post("/password.send", args,
             {"callback": Kwo.Auth.onPasswordRequestCallback.bind($(args)),
              "disable": true});
  },

  onPasswordRequestCallback: function(resp) {
    resp = new kwo.Response(resp);
    if (resp.hasError()) return resp.alert();
    this.hide().previous().show();
    resp.warn();
  }

};

Kwo.Dialogs.AuthManager = Class.create(Kwo.Dialog, {

  initialize: function($super, opts) {
    this.opts = opts || {};
    this.name = "auth";
    this.width = this.opts["width"] || window["_auth_dialog_width"] || 720;
    this.height = this.opts["height"] || window["_auth_dialog_height"] || 450;
    $super(this.onDisplay);
  },

  onDisplay: function() {
    var args = {};
    if ("mode" in this.opts) {
      /** login | signup **/
      args["mode"] = this.opts["mode"];
    }
    kwo.post("/signup", args,
             {"container": this.support,
              "callback": this.onDisplayCompleted.bind(this)});
    Kwo.Auth.dialog = this;
  },

  onDisplayCompleted: function() {

  },

  onCallback: function() {
    Kwo.reload();
  },

  onBeforeClose: function() {
    Kwo.Auth.dialog = null;
  }

});

Kwo.Composer.Message = Class.create(Kwo.Dialog, {

  initialize: function($super, elt) {
    this.name = "message-composer";
    this.layout = "hbox-layout";
    this.width = 600;
    this.height = 400;
    this.elt = $(elt);
    $super(this.onDisplay);
  },

  onDisplay: function() {
    this.args = {"item": this.elt.readAttribute("data-item")};
    kwo.post("/message.compose", this.args,
             {"container": this.support,
              "callback": this.onBind.bind(this)});
  },

  onBind: function() {
    var that = this;
    this.support.down("FORM").observe("submit", function(evt) {
      evt.stop();
      that.onSubmit();
    });
  },

  onSubmit: function(elt) {
    var form = this.support.down("FORM");
    var args = [this.args, form];
    kwo.post("/message.send", args,
             {"callback": form, "disable": form});
  }

});

Kwo.Favorite = {

  onAlertUnset: function(elt) {
    elt = $(elt);
    kwo.post("/favorite.alert.unset",
             {"id": elt.up("TR").readAttribute("data-id")},
             {"confirm": elt,
              "callback": Kwo.Favorite.onCallback});
  },

  onCallback: function(resp) {
    resp = new kwo.Response(resp);
    if (resp.hasError()) return resp.alert();
    Kwo.reload();
  },

  onSave: function(elt) {
    elt = $(elt);
    kwo.post("/favorite.save", {'item': elt.getAttribute('data-item')}, {"callback": Kwo.Favorite.onCallback});
  },

  onDelete: function(elt) {
    elt = $(elt);
    kwo.post("/favorite.delete",
             {"id": elt.readAttribute("data-id")},
             {"confirm": elt,
              "callback": Kwo.Favorite.onCallback});
  }

};

Kwo.Notice = {

  onSubmit: function(args) {
    kwo.post("notice.save", args,
             {"callback": true,
              "disable": true});
  }

};

Kwo.Account = {

  timeout: null,

  onEnter: function(elt) {
    elt = $(elt);
    var url = elt.readAttribute("data-url") || "/account";
    if (!Kwo.isAuth()) {
      var opts = {};
      if (elt.readAttribute("data-layout")) {
        opts["mode"] = elt.readAttribute("data-layout");
      }
      var am = new Kwo.Dialogs.AuthManager(opts);
      am.onCallback = function() {
        Kwo.go(url);
      };
      return ;
    }
    Kwo.go(url);
  },

  onLeave: function(elt) {
    elt = $(elt);
    var url = elt.readAttribute("data-url") || "/";
    kwo.post("/user.logout", null,
             {"callback": function (resp) {
               resp = new kwo.Response(resp);
               if (resp.hasError()) return resp.alert();
               Kwo.go(url);
             },
              "confirm": elt});
  },

  refresh: function(resp) {
    resp = new kwo.Response(resp);
    if (resp.hasError()) return resp.alert();
    if (resp.isRedirection()) {
      return resp.redirect();
    }
    Kwo.reload();
  },

  setMessage: function(msg, error) {
    error = false;
    if (typeof msg == "object" && "error" in msg) {
      if (msg["error"] >= 1) {
        error = "Oops! " + msg["result"]["msg"].join(",");
      }
      else {
        msg = msg["result"]["callback_message"];
      }
    }
    else {
      error = error || false;
    }
    var pix = "ok.png";
    if (error != false) {
      pix = "ko.png";
      msg = error;
    }
    else {
      msg = Object.isUndefined(msg) ||  msg.empty() ? "ok" : msg;
    }
    if (!$("account-notice")) return ;
    var notice = $("account-notice");
    notice.show();
    notice.update(msg.ucfirst() + '<img src="/web/core/images/bullets/' + pix + '" />'
                                   +'<div style="clear:both;"></div>');
    window.clearTimeout(Kwo.Account.timeout);
    Kwo.Account.timeout = window.setTimeout(notice.hide.bind(notice), 5000);
  }

};


Kwo.Class.Connection = Class.create(Kwo.Dialog, {

  initialize: function($super, elt) {
    this.name = "connection";
    this.className = "layout-hbox";
    this.args = {"item": $(elt).readAttribute("data-item")};
    this.width = 350;
    this.height = 250;
    $super(this.onDisplay);
  },

  onDisplay: function() {
    kwo.post("/connection.prompt", this.args,
             {"container": this.support,
              "callback": this.onBind.bind(this)});
  },

  onBind: function() {
    var that = this;
    this.support.down("FORM").observe("submit", function (evt) {
      evt.stop();
      that.onSubmit(evt.element());
    });
  },

  onSubmit: function(elt) {
    kwo.post("/invitation.save", [this.args, elt],
             {"callback": elt});
  }

});

Kwo.Connection = {

  onRequest: function(elt) {
    kwo.post("/invitation.save", elt,
             {"callback": elt});
  },

  onAccept: function(user_id) {

  }

};

Kwo.Group = {

  onCancel: function(elt) {
    elt = $(elt);
    elt.up("TD.column").update();
  },

  onEdit: function(elt) {
    elt = $(elt);
    Kwo.go("/group.edit", {"id": elt.readAttribute("data-id")});
  },

  onDelete: function(elt) {
    elt = $(elt);
    kwo.post("/group.delete", elt.up("FORM"),
             {"callback": true,
              "disable": true,
              "confirm": true});
  },

  onInvitationCallback: function(resp) {
    resp = new kwo.Response(resp);
    this.update(resp.getMessage());
  },

  onInvitationSend: function(elt) {
    elt = $(elt);
    kwo.post("/invitation.save", elt,
             {"callback": Kwo.Group.onInvitationCallback.bind(elt),
              "disable": true});
  },

  onInvite: function(elt) {
    elt = $(elt);
    new Kwo.Dialog("/group.invite", {"item": elt.readAttribute("data-id")},
                   {"className": "layout-hbox", "width": 600, "height": 400});
  },

  onJoin: function(id) {},

  onLeave: function(elt) {
    elt = $(elt);
    kwo.post("/group.leave", {"id": elt.readAttribute("data-id")},
             {"callback": elt.up("DIV.group"), "confirm": true});
  },

  onSave: function(elt) {
    kwo.post("/group.save", elt,
             {"callback": true, "disable": true});
  }

};

Kwo.Invitation = {

  onAccept: function(elt) {
    elt = $(elt);
    kwo.post("/account/invitation.accept", {"id": elt.readAttribute("data-id")},
             {"callback": elt.up("DIV")});
  },

  onRefuse: function(elt) {
    elt = $(elt);
    kwo.post("/account/invitation.refuse", {"id": elt.readAttribute("data-id")},
             {"callback": elt.up("DIV"), "confirm": elt});
  }

};

Kwo.Network = {

  onRemove: function(elt) {
    elt = $(elt);
    kwo.post("/account/friend.remove", {"item": elt.readAttribute("data-item")},
             {"callback": elt.up("DIV"), "confirm": elt});
  }

}



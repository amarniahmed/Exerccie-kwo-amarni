kwo.Translator = Class.create({

  "timeout": null,
  "parent": null,
  "form": null,
  "filters": null,
  "list": null,
  "locale": null,
  "id": null,
  "console": null,

  initialize: function (elt) {
    var that = this;
    elt = $(elt);
    this.parent = elt;
    this.form = this.down("FORM");
    this.filters = {"locale": this.down("INPUT[name=locale]"),
                    "status": this.down("SELECT[name=status]"),
                    "model": this.down("SELECT[name=model_id]")};
    this.list = $("translations");
    this.locale = parseInt(this.filters.locale.value);
    this.console = $("console");
    this.onBind();
  },

  down: function(arg) {
    return this.parent.down(arg);
  },

  select: function(arg) {
    return this.parent.select(arg);
  },

  onBind: function () {
    var that = this;
    this.listBind();
    for (el in this.filters) {
      this.filters[el].observe("change", that.listRefresh.bindAsEventListener(that));
    }
    this.onTabBind(this.down(".tabbox"));
  },

  onTabBind: function (tabbox) {
    var that = this;
    var tab = tabbox.down(".tabs").down(".selected");
    if (!tab) {
      tab = tabbox.down(".tab").addClassName("selected");
    }
    tabbox.down(".tabs").childElements().each(function (tab) {
      tab.observe("click", that.onTabChange.bindAsEventListener(that));
    });
  },

  onTabChange: function (evt) {
    var tab;
    if (Object.isElement(evt)) {
      tab = evt;
    }
    else {
      evt.stop();
      // IE BUG
      if (evt.currentTarget) {
        tab = $(evt.currentTarget);
      } else {
        tab = evt.element();
        if (tab.tagName != "DIV") {
          tab = tab.up("DIV");
        }
      }
    }
    if (tab.hasClassName("selected")) return ;
    tab.up().down("DIV.selected").removeClassName("selected");
    var code = tab.addClassName("selected").readAttribute("data-tab");
    this.locale = code;
    this.filters.locale.value = this.locale;
    this.listRefresh();
  },

  listBind: function() {
    var that = this;
    this.list.select("a").each(function(el) {
      el.observe("click", that.onListSelect.bindAsEventListener(that));
    });
  },

  onListSelect: function(evt) {
    evt.stop();
    if (evt.currentTarget) {
      elt = $(evt.currentTarget);
    } else {
      elt = evt.element();
    }
    this.formRefresh(elt.readAttribute("data-id"));
  },

  listRefresh: function() {
    this.list.update('<img src="/web/core/images/throbbers/circle04.gif" alt="" style="margin:5px 0 0 5px;" />');
    Kwo.exec("/translation.list",
             {"locale": this.locale,
              "status": this.filters.status.getValue(),
              "model_id": this.filters.model.getValue()},
             {"container": this.list, "callback": this.listBind.bind(this)});
  },

  formBind: function() {
    this.id = parseInt(this.form.down("INPUT[name=id]").value);
    var that = this;
    var btn = this.form.down(".submit input");
    btn.observe("click", that.onSubmit.bindAsEventListener(that));
  },

  formRefresh: function(id) {
    Kwo.exec("/translation.edit",
             {"id": id},
             {"container": this.form, "callback": this.formBind.bind(this)});
  },

  onSubmit: function (evt) {
    evt.stop();
    // IE BUG
    var btn = evt.element();
    if (!Kwo.hasClickExpired(btn)) {
      console.log("double clic");
      return ;
    }
    this.select(".box-error").invoke("removeClassName", "box-error");
    this.select(".tab-error").invoke("removeClassName", "tab-error");
    var args = this.form.serialize(true);
    this.form.disable();
    Kwo.exec("/translation.save", args,
             {"callback": this.onSubmitCallback.bind(this)});
    return false;
  },

  onSubmitCallback: function(resp) {
    resp = new kwo.Response(resp);
    if (resp.hasError()) {
      return this.onSubmissionFailure(resp.getError());
    }
    if (resp.hasAttribute("errors")) {
      var that = this;
      var errors = resp.getAttribute("errors");
      this.onSubmissionFailure(errors.length + " erreur(s)");
      return ;
    }
    this.listRefresh();
    if (resp.getAttribute("status") == 80) {
      [$("form-edition"), $("form-validation")].invoke("toggle");
    }
    else {
      this.onSubmissionSuccess(resp.getAttribute("id"));
    }
  },

  onSubmissionComplete: function(id) {
    if (this.form && id) {
      this.formRefresh(id);
    }
  },

  onSubmissionFailure: function (msg) {
    msg = msg || "erreur";
    this.log(msg, true);
    this.form.enable();
  },

  onSubmissionSuccess: function (id, msg) {
    msg = msg || "les informations ont bien été enregistrées.";
    this.log(msg);
    this.onSubmissionComplete(id);
  },

  log: function(msg, error) {
    msg = msg || "";
    error = error || false;
    if (Object.isArray(msg)) {
      msg = msg.join("<br/>");
    }
    else if (Object.isElement(msg)) {
      msg = msg.innerHTML;
    }
    this.console.removeClassName("console-box-error");
    if (msg.empty()) {
      this.console.removeClassName("console-updated");
      this.console.update("");
    }
    else {
      this.console.addClassName("console-updated");
      if (error == true) {
        this.console.addClassName("console-box-error");
      }
      this.console.update(msg.ucfirst());
      this.console.show();
      var that = this;
      window.setTimeout(function () {
        that.console.update("");
        that.console.hide();
      }, 3000);
    }
  }

});
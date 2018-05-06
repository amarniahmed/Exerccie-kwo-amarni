document.observe("dom:loaded", function() { 
  new Repository(); 
});

Repository = Class.create({

  parent: null,

  initialize: function () {
    this.parent = $(document.body);
    this.observe1();
  },
  
  down: function(expr) {
    return this.parent.down(expr);
  },
  
  select: function(expr) {
    return this.parent.select(expr);
  },
  
  observe1: function() {
    var that = this;
    this.down(".btn-compare").observe("click", function(evt) {
      evt.stop();
      this.disable();
      that.compare();
    });
  },

  observe: function() {
    var that = this;
    this.down("article").select(".btn-diff").each(function(elt) {
      elt.observe("click", function(evt) {
        evt.stop();
        that.diff(this.up("tr"));
      })
    });
    this.down("article input[type=button]").observe("click", function(evt) {
      that.sync();
    });
  },
  
  sync: function() {
    if (this.down("textarea").getValue().length < 5) {
      return alert('veuillez préciser un commentaire concernant ce commit');
    }
    var files = {"push": [], "pull": []};
    this.down("article").select("input[type=checkbox]").each(function(elt) {
      if (!elt.checked) return ;
      var tr = elt.up("tr");
      files[tr.getAttribute("data-direction")].push(tr.getAttribute("data-file"));
    });
    console.log(files);
  },

  diff: function(file) {
    new kwo.dialogs.DiffViewer(file);
  },

  compare: function() {
    var that = this;
    var container = this.down("article");
    container.update('<img src="/web/core/images/throbbers/hbar01.gif" />');
    Kwo.exec("/back/core/repository.compare", null, 
             {"container": container,
              "callback": function() {
                that.down(".btn-compare").enable();
                that.observe();
              }});
  }
  
});

kwo.dialogs.DiffViewer = Class.create(kwo.ui.Dialog, {

  file: null,

  initialize: function($super, elt) {
    this.file = $(elt).getAttribute("data-file")
    $super({"elt": $(elt),
            "name": "diff",
            "title": "diff - " + this.file,
            "width": 1024,
            "height": 600});
    this.paint();
  },

  paint: function() {
    var args = {"file": this.file};
    Kwo.exec("/back/core/repository.diff", args, 
             {"container": this.container});
  }

});
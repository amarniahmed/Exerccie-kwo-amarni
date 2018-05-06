Kwo.SlideShow = Class.create();

Kwo.SlideShow.prototype  = {
  "elt": null,
  "timerOn": true,
  "t_timer": null,
  "timing": 6,
  "max": null,
  "img_path": null,

  initialize: function(source, path) {
    this.elt = $(source).previous();
    this.max = this.elt.readAttribute("data-max");
    this.img_path   =       path;
    this.elt.observe("click", this.onClick.bindAsEventListener(this));
    this.preload(this);
    var that = this;
    this.t_timer = new PeriodicalExecuter(function() { that.next(); }, this.timing);
    source.remove();
  },

  preload: function(ctx) {
    this.elt.select('.slide').each(function(s, index) {
      var url = s.getStyle("background-image").replace(/^url\(["']?/, '').replace(/["']?\)$/, '');
      var img = new Image();
      img.src = url;
    });
  },

  onClick: function(evt) {
    evt.stop();
    this.toggleTimer();
    var target = $(evt.target);
    if (target.hasClassName("next")) {
      this.next();
    }
    else if (target.hasClassName("prev")) {
      this.prev();
    }
    else if (target.hasClassName("access")) {
      this.access(target.readAttribute("data-number"));
    }
  },

  toggleTimer: function() {
    if (this.timerOn) {
      this.timerOn = false;
      this.t_timer.stop();
    } 
    else {
      this.timerOn = true;
      var that = this;
      this.t_timer = new PeriodicalExecuter(function() { 
        that.next(this); }, this.timing);
    }
  },

  access: function(number) {
    if (this.elt.select('.access')[number].hasClassName("access_selected")) return false;
    var current_slide = this.elt.down('.slide_selected');
    var next_slide = this.elt.select('.slide')[number];
    var access = this.clear_access();
    this.elt.select('.access')[number].addClassName("access_selected")

    new Effect.Opacity(next_slide.addClassName('slide_selected').setStyle({opacity: 0, zIndex: 2}), {
      from: 0,
      to: 1,
      duration: 0.3,
      afterFinish: function( ){
        current_slide.removeClassName('slide_selected');
        next_slide.setStyle({zIndex: 1});
      }
    });
  },

  clear_access: function() {
    var access = this.elt.down('.access_selected').removeClassName('access_selected');
    return access;
  },

  clear_slide: function() {
    var slide = this.elt.down('.slide_selected').removeClassName('slide_selected');
    return slide;
  },

  next: function () {
    var current_slide = this.elt.down('.slide_selected');
    var next_slide = this.elt.select('.slide').first();
    if (current_slide.next('.slide')) {
      next_slide = current_slide.next('.slide');
    }
    var access = this.clear_access();
    if (access.next('.access')) {
      access.next('.access').addClassName('access_selected');
    } else {
      this.elt.select('.access').first().addClassName('access_selected');
    }
    new Effect.Opacity(next_slide.addClassName('slide_selected').setStyle({opacity: 0, zIndex: 2}), {
      from: 0,
      to: 1,
      duration: 0.3,
      afterFinish: function( ){
        current_slide.removeClassName('slide_selected');
        next_slide.setStyle({zIndex: 1});
      }
    });
  },

  prev: function () {
    var current_slide = this.elt.down('.slide_selected');
    var prev_slide = this.elt.select('.slide').last();
    if (current_slide.previous('.slide')) {
      prev_slide = current_slide.previous('.slide');
    }
    var access = this.clear_access();
    if (access.previous('.access')) {
      access.previous('.access').addClassName('access_selected');
    } 
    else {
      this.elt.select('.access').last().addClassName('access_selected');
    }
    new Effect.Opacity(prev_slide.addClassName('slide_selected').setStyle({opacity: 0, zIndex: 2}), {
      from: 0,
      to: 1,
      duration: 0.3,
      afterFinish: function( ){
        current_slide.removeClassName('slide_selected');
        prev_slide.setStyle({zIndex: 1});
      }
    });
  }
};
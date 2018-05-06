Kwo.BitmapEditor = Class.create({

  "elt": null,
  "img": null,
  "file_path": null,
  "with": 0,
  "height": 0,
  
  initialize: function(elt) {
    this.elt = elt;
    this.file_path = elt.readAttribute("data-file-path");
//    alert(elt.width);
    var cropbox;
    var orgX, orgY;

    var that = this;    

    $("bitmap-button-dimensions").observe("click", function(evt) {
      evt.stop();
      var elt = $(evt.element().readAttribute("data-panel"));
      that.displayPanel(elt);
    });
    
    $("bitmap-button-rotate").observe("click", function(evt) {
      evt.stop();
      var elt = $(evt.element().readAttribute("data-panel"));
      that.displayPanel(elt);
    });
    
    $("bitmap-panel-dimensions").down("A").observe("click", function(evt) {
      evt.stop();
      if (!confirm("êtes vous sûr ?")) {
        this.up(".bitmap-panel").hide();
        return ;
      }
      that.onExec(evt.element().up("FORM"));
    });

    $("bitmap-panel-rotate").down("A").observe("click", function(evt) {
      evt.stop();
      if (!confirm("êtes vous sûr ?")) {
        this.up(".bitmap-panel").hide();
        return ;
      }
      that.onExec(evt.element().up("FORM"));
    });



    this.img = new Image();
    this.img.src = "/" + $("bitmap").readAttribute("data-file-path") + "?t" + Math.random();
    this.img.observe("load", function(evt) {
      that.width = this.width;
      $("bitmap-width").update(that.width);
      that.height = this.height;
      $("bitmap-height").update(that.height);
      that.onPaint();
    });

    Event.observe(window, "resize", this.onPaint.bindAsEventListener(this));

    var buffer = this.elt;

    $("bitmap-button-crop").addEventListener("click", function(evt) {
      if (!$("cropbox")) {
        cropbox = document.createElement("DIV");
        cropbox.id = "cropbox";
        cropbox.style.display = "none";
        document.body.appendChild(cropbox);
      }
      buffer.addEventListener("mousedown", mousedown, true);
      buffer.addEventListener("mouseup", mouseup, true);
    }, false);


    

    function mousedown(e) {
      orgX = e.clientX + document.body.scrollLeft + document.documentElement.scrollLeft;
      orgY = e.clientY + document.body.scrollTop + document.documentElement.scrollTop;
      buffer.addEventListener("mousemove", mousemove, true);
    }
    
    function mouseup(e) {
      buffer.removeEventListener("mousemove", mousemove, true);
      buffer.removeEventListener("mousedown", mousedown, true);
      buffer.removeEventListener("mouseup", mouseup, true);
      var x = cropbox.offsetLeft - buffer.offsetLeft - buffer.parentNode.offsetLeft;
      var y = cropbox.offsetTop - buffer.offsetTop - buffer.parentNode.offsetTop;
      var width = cropbox.offsetWidth;
      var height = cropbox.offsetHeight;
      
      document.body.removeChild(cropbox);
      that.onCrop(x, y, width, height);
    }
    
    function mousemove(e) {
      var posx = e.clientX + document.body.scrollLeft + document.documentElement.scrollLeft;
      var posy = e.clientY + document.body.scrollTop + document.documentElement.scrollTop;
      
      var x = orgX;
      var y = orgY;
      var width = posx - orgX;
      var height = posy - orgY;
      
      if (width < 0) {
        x += width;
        width *= -1;
      }
      
      if (height < 0) {
        y += height;
        height *= -1;
      }
      
      cropbox.style.display = "block";
      cropbox.style.left = x + "px";
      cropbox.style.top = y + "px";
      cropbox.style.width = width + "px";
      cropbox.style.height = height + "px";
    }
 
  },

  onPaint: function(evt) {
    if (this.width > (window.innerWidth - 118) || this.height > (window.innerHeight - 16)) {
      var ratio1 = (window.innerWidth - 118) / this.width;
      var ratio2 = (window.innerHeight - 16) / this.height;
      var ratio = ratio1 < ratio2 ? ratio1 : ratio2;
      var width = this.width * ratio;
      var height = this.height * ratio;
      this.elt.width = Math.ceil(width);
      this.elt.height = Math.ceil(height);
      $("bitmap-zoom").writeAttribute("data-zoom", ratio);
      $("bitmap-zoom").update(Math.ceil(ratio * 100) + "%");
    }
    else {
      $("bitmap-zoom").writeAttribute("data-zoom", 1);
      $("bitmap-zoom").update("100%");
    }
    if (this.elt.src != this.img.src) {
      this.elt.src = this.img.src;
      this.elt.show();
    }

  },

  onCrop: function(x, y, width, height) {
    var args = {operation: "crop",
                x: x, y: y,
                width: width, height: height,
                ratio: $("bitmap-zoom").readAttribute("data-zoom") };
    this.onExec(args);
    
  },
  
  onExec: function(args) {
    args = [{file_path: this.file_path}, args];
    Kwo.exec("/back/core/bitmap.transform", args, 
             {callback: function(res) {
               Kwo.reload();
             }});
  },
  
  displayPanel: function(elt) {
    if (elt.visible()) {
      elt.hide();
      return ;
    }
    $("bitmap-panels").childElements().each(function (panel) {
      $(panel).hide();
    });
    elt.show();
  }
  
});


document.addEventListener("DOMContentLoaded", function(evt) {
  new Kwo.BitmapEditor($("bitmap"));
}, false);


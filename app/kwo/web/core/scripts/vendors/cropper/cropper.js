function Cropper (id) {
    if (!$(id)) return false;
    
    this.image = $(id);
    
    this.element = document.createElement('div');
    this.element.setAttribute("class", "cropper");
    this.element.setAttribute("style", "display:none");
    document.body.appendChild(this.element);
    
    this.crop_zone = document.createElement('div');
    this.crop_zone.setAttribute("class", "cropzone");
    this.element.appendChild(this.crop_zone);
    
    this.shadow = document.createElement('div');
    this.shadow.setAttribute("class", "shadow");
    this.element.appendChild(this.shadow);
    
    this.block_top = document.createElement('div');
    this.block_top.setAttribute("class", "block");
    this.shadow.appendChild(this.block_top);
    
    this.block_bottom = document.createElement('div');
    this.block_bottom.setAttribute("class", "block");
    this.shadow.appendChild(this.block_bottom);
    
    this.block_left = document.createElement('div');
    this.block_left.setAttribute("class", "block");
    this.shadow.appendChild(this.block_left);
    
    this.block_right = document.createElement('div');
    this.block_right.setAttribute("class", "block");
    this.shadow.appendChild(this.block_right);

    this.slider_top_left = new SliderTopLeft(this);
    this.slider_top_right = new SliderTopRight(this);
    this.slider_bottom_left = new SliderBottomLeft(this);
    this.slider_bottom_right = new SliderBottomRight(this);
    
    this.slider_top = new SliderTop(this);
    this.slider_bottom = new SliderBottom(this);
    this.slider_left = new SliderLeft(this);
    this.slider_right = new SliderRight(this);

    this.setCropZone(20);
    
    Event.observe(window, 'resize', this.reflow.bindAsEventListener(this));
}

Cropper.prototype.toQueryString = function () {
    return "x="+(this.crop_zone.offsetLeft+1)+"&y="+(this.crop_zone.offsetTop+1)+"&width="+(this.crop_zone.offsetWidth-2)+"&height="+(this.crop_zone.offsetHeight-2);
}

Cropper.prototype.reflow = function (ratio) {
    var top = this.image.offsetTop;
    var left = this.image.offsetLeft;
    var width = this.image.width;
    var height = this.image.height;
    this.element.setAttribute("style","display:block;top:"+top+"px;left:"+left+"px;width:"+width+"px;height:"+height+"px;");

    this.update();
}

Cropper.prototype.setCropZone = function (ratio) {
    ratio = ratio/2;

    var top = 0;
    var left = 0;
    var width = this.image.width;
    var height = Math.round((this.image.height*ratio)/100);
    this.block_top.setAttribute("style","top:"+top+"px;left:"+left+"px;width:"+width+"px;height:"+height+"px;");

    var top = 0;
    var left = 0;
    var width = Math.round((this.image.width*ratio)/100);
    var height = this.image.height;
    this.block_left.setAttribute("style","top:"+top+"px;left:"+left+"px;width:"+width+"px;height:"+height+"px;");

    var top = this.image.height-Math.round((this.image.height*ratio)/100);
    var left = 0;
    var width = this.image.width;
    var height = Math.round((this.image.height*ratio)/100);
    this.block_bottom.setAttribute("style","top:"+top+"px;left:"+left+"px;width:"+width+"px;height:"+height+"px;");

    var top = 0;
    var left = this.image.width-Math.round((this.image.width*ratio)/100);
    var width = Math.round((this.image.width*ratio)/100);
    var height = this.image.height;
    this.block_right.setAttribute("style","top:"+top+"px;left:"+left+"px;;width:"+width+"px;height:"+height+"px;");

    var top = 0;
    var left = 0;
    var width = this.image.width;
    var height = this.image.height;
    this.shadow.setAttribute("style","top:"+top+"px;left:"+left+"px;width:"+width+"px;height:"+height+"px;");

    this.shadow.setOpacity(0.6);
    
    this.reflow();
};

Cropper.prototype.update = function () {
    this.updateCropZone();
    this.updateSliders();
}

Cropper.prototype.updateCropZone = function () {
    var top = this.block_top.offsetHeight-1;
    var left = this.block_left.offsetWidth-1;
    var width = this.image.width-(this.block_left.offsetWidth+this.block_right.offsetWidth);
    var height = this.image.height-(this.block_top.offsetHeight+this.block_bottom.offsetHeight);
    this.crop_zone.setAttribute("style","top:"+top+"px;left:"+left+"px;width:"+width+"px;height:"+height+"px;");
}

Cropper.prototype.updateSliders = function () {
    var top = this.crop_zone.offsetTop-3;
    var left = this.crop_zone.offsetLeft-3;
    this.slider_top_left.slider.setAttribute("style","top:"+top+"px;left:"+left+"px;");

    var top = this.crop_zone.offsetTop-3;
    var left = this.crop_zone.offsetLeft+this.crop_zone.offsetWidth-3;
    this.slider_top_right.slider.setAttribute("style","top:"+top+"px;left:"+left+"px;");

    var top = this.crop_zone.offsetTop+this.crop_zone.offsetHeight-3;
    var left = this.crop_zone.offsetLeft-3;
    this.slider_bottom_left.slider.setAttribute("style","top:"+top+"px;left:"+left+"px;");

    var top = this.crop_zone.offsetTop+this.crop_zone.offsetHeight-3;
    var left = this.crop_zone.offsetLeft+this.crop_zone.offsetWidth-3;
    this.slider_bottom_right.slider.setAttribute("style","top:"+top+"px;left:"+left+"px;");

    var top = this.crop_zone.offsetTop-3;
    var left = this.crop_zone.offsetLeft+(this.crop_zone.offsetWidth/2)-3;
    this.slider_top.slider.setAttribute("style","top:"+top+"px;left:"+left+"px;");

    var top = this.crop_zone.offsetTop+this.crop_zone.offsetHeight-3;
    var left = this.crop_zone.offsetLeft+(this.crop_zone.offsetWidth/2)-3;
    this.slider_bottom.slider.setAttribute("style","top:"+top+"px;left:"+left+"px;");

    var top = this.crop_zone.offsetTop+(this.crop_zone.offsetHeight/2)-3;
    var left = this.crop_zone.offsetLeft-3;
    this.slider_left.slider.setAttribute("style","top:"+top+"px;left:"+left+"px;");

    var top = this.crop_zone.offsetTop+(this.crop_zone.offsetHeight/2)-3;
    var left = this.crop_zone.offsetLeft+this.crop_zone.offsetWidth-3;
    this.slider_right.slider.setAttribute("style","top:"+top+"px;left:"+left+"px;");
}

function SliderTopLeft (cropper) {
    this.cropper = cropper;

    this.slider = document.createElement('div');
    this.slider.setAttribute("class", "slidertopleft");
    this.cropper.element.appendChild(this.slider);
        
    Event.observe(this.slider, 'mousedown', Slider.startDrag.bindAsEventListener(this));
    Event.observe(window, 'mouseup', Slider.stopDrag.bindAsEventListener(this));
};

SliderTopLeft.prototype.drag = function (event) {
    var top = 0;
    var left = 0;
    var width = this.cropper.image.width;
    if (Event.pointerY(event)-this.cropper.image.offsetTop >= this.cropper.image.offsetHeight-this.cropper.block_bottom.offsetHeight) {
        var height = this.cropper.image.offsetHeight-this.cropper.block_bottom.offsetHeight
    } else if (Event.pointerY(event)-this.cropper.image.offsetTop >= this.cropper.image.offsetHeight) {
        var height = this.cropper.image.offsetHeight;
    } else {
        var height = Event.pointerY(event)-this.cropper.image.offsetTop;
    }
    this.cropper.block_top.setAttribute("style","top:"+top+"px;left:"+left+"px;width:"+width+"px;height:"+height+"px;");

    var top = 0;
    var left = 0;
    var height = this.cropper.image.height;
    if (Event.pointerX(event)-this.cropper.image.offsetLeft >= this.cropper.image.offsetWidth-this.cropper.block_right.offsetWidth) {
        var width = this.cropper.image.offsetWidth-this.cropper.block_right.offsetWidth;
    } else if (Event.pointerX(event)-this.cropper.image.offsetLeft >= this.cropper.image.offsetWidth) {
        var width = this.cropper.image.offsetWidth;
    } else {
        var width = Event.pointerX(event)-this.cropper.image.offsetLeft;
    }
    this.cropper.block_left.setAttribute("style","top:"+top+"px;left:"+left+"px;width:"+width+"px;height:"+height+"px;");

    this.cropper.update();
};

function SliderTopRight (cropper) {
    this.cropper = cropper;

    this.slider = document.createElement('div');
    this.slider.setAttribute("class", "slidertopright");
    this.cropper.element.appendChild(this.slider);
        
    Event.observe(this.slider, 'mousedown', Slider.startDrag.bindAsEventListener(this));
    Event.observe(window, 'mouseup', Slider.stopDrag.bindAsEventListener(this));
};

SliderTopRight.prototype.drag = function (event) {
    var top = 0;
    var left = 0;
    var width = this.cropper.image.width;
    if (Event.pointerY(event)-this.cropper.image.offsetTop >= this.cropper.image.offsetHeight-this.cropper.block_bottom.offsetHeight) {
        var height = this.cropper.image.offsetHeight-this.cropper.block_bottom.offsetHeight
    } else if (Event.pointerY(event)-this.cropper.image.offsetTop >= this.cropper.image.offsetHeight) {
        var height = this.cropper.image.offsetHeight;
    } else {
        var height = Event.pointerY(event)-this.cropper.image.offsetTop;
    }
    this.cropper.block_top.setAttribute("style","top:"+top+"px;left:"+left+"px;width:"+width+"px;height:"+height+"px;");

    var top = 0;
    var height = this.cropper.image.height;
    if (Event.pointerX(event)-this.cropper.image.offsetLeft <= this.cropper.block_left.offsetWidth) {
        var left = this.cropper.block_left.offsetWidth;
        var width = this.cropper.image.offsetWidth-this.cropper.block_left.offsetWidth;
    } else {
        var left = Event.pointerX(event)-this.cropper.image.offsetLeft;
        var width = this.cropper.image.width-(Event.pointerX(event)-this.cropper.image.offsetLeft);
    }
    this.cropper.block_right.setAttribute("style","top:"+top+"px;left:"+left+"px;;width:"+width+"px;height:"+height+"px;");

    this.cropper.update();
};

function SliderBottomLeft (cropper) {
    this.cropper = cropper;
    
    this.slider = document.createElement('div');
    this.slider.setAttribute("class", "sliderbottomleft");
    this.cropper.element.appendChild(this.slider);

    Event.observe(this.slider, 'mousedown', Slider.startDrag.bindAsEventListener(this));
    Event.observe(window, 'mouseup', Slider.stopDrag.bindAsEventListener(this));
};

SliderBottomLeft.prototype.drag = function (event) {
    var left = 0;
    var width = this.cropper.image.width;
    if (Event.pointerY(event)-this.cropper.image.offsetTop <= this.cropper.block_top.offsetHeight) {
        var top = this.cropper.block_top.offsetHeight;
        var height = this.cropper.image.height-this.cropper.block_top.offsetHeight;
    } else {
        var top = Event.pointerY(event)-this.cropper.image.offsetTop;
        var height = this.cropper.image.height-(Event.pointerY(event)-this.cropper.image.offsetTop);
    }
    this.cropper.block_bottom.setAttribute("style","top:"+top+"px;left:"+left+"px;width:"+width+"px;height:"+height+"px;");

    var top = 0;
    var left = 0;
    var height = this.cropper.image.height;
    if (Event.pointerX(event)-this.cropper.image.offsetLeft >= this.cropper.image.offsetWidth-this.cropper.block_right.offsetWidth) {
        var width = this.cropper.image.offsetWidth-this.cropper.block_right.offsetWidth;
    } else if (Event.pointerX(event)-this.cropper.image.offsetLeft >= this.cropper.image.offsetWidth) {
        var width = this.cropper.image.offsetWidth;
    } else {
        var width = Event.pointerX(event)-this.cropper.image.offsetLeft;
    }
    this.cropper.block_left.setAttribute("style","top:"+top+"px;left:"+left+"px;width:"+width+"px;height:"+height+"px;");

    this.cropper.update();
};

function SliderBottomRight (cropper) {
    this.cropper = cropper;
    
    this.slider = document.createElement('div');
    this.slider.setAttribute("class", "sliderbottomright");
    this.cropper.element.appendChild(this.slider);

    Event.observe(this.slider, 'mousedown', Slider.startDrag.bindAsEventListener(this));
    Event.observe(window, 'mouseup', Slider.stopDrag.bindAsEventListener(this));
};

SliderBottomRight.prototype.drag = function (event) {
    var left = 0;
    var width = this.cropper.image.width;
    if (Event.pointerY(event)-this.cropper.image.offsetTop <= this.cropper.block_top.offsetHeight) {
        var top = this.cropper.block_top.offsetHeight;
        var height = this.cropper.image.height-this.cropper.block_top.offsetHeight;
    } else {
        var top = Event.pointerY(event)-this.cropper.image.offsetTop;
        var height = this.cropper.image.height-(Event.pointerY(event)-this.cropper.image.offsetTop);
    }
    this.cropper.block_bottom.setAttribute("style","top:"+top+"px;left:"+left+"px;width:"+width+"px;height:"+height+"px;");

    var top = 0;
    var height = this.cropper.image.height;
    if (Event.pointerX(event)-this.cropper.image.offsetLeft <= this.cropper.block_left.offsetWidth) {
        var left = this.cropper.block_left.offsetWidth;
        var width = this.cropper.image.offsetWidth-this.cropper.block_left.offsetWidth;
    } else {
        var left = Event.pointerX(event)-this.cropper.image.offsetLeft;
        var width = this.cropper.image.width-(Event.pointerX(event)-this.cropper.image.offsetLeft);
    }
    this.cropper.block_right.setAttribute("style","top:"+top+"px;left:"+left+"px;;width:"+width+"px;height:"+height+"px;");

    this.cropper.update();
};

function SliderTop (cropper) {
    this.cropper = cropper;

    this.slider = document.createElement('div');
    this.slider.setAttribute("class", "slidertop");
    this.cropper.element.appendChild(this.slider);

    Event.observe(this.slider, 'mousedown', Slider.startDrag.bindAsEventListener(this));
    Event.observe(window, 'mouseup', Slider.stopDrag.bindAsEventListener(this));
};

SliderTop.prototype.drag = function (event) {
    var top = 0;
    var left = 0;
    var width = this.cropper.image.width;
    if (Event.pointerY(event)-this.cropper.image.offsetTop >= this.cropper.image.offsetHeight-this.cropper.block_bottom.offsetHeight) {
        var height = this.cropper.image.offsetHeight-this.cropper.block_bottom.offsetHeight
    } else if (Event.pointerY(event)-this.cropper.image.offsetTop >= this.cropper.image.offsetHeight) {
        var height = this.cropper.image.offsetHeight;
    } else {
        var height = Event.pointerY(event)-this.cropper.image.offsetTop;
    }
    this.cropper.block_top.setAttribute("style","top:"+top+"px;left:"+left+"px;width:"+width+"px;height:"+height+"px;");

    this.cropper.update();
};

function SliderBottom (cropper) {
    this.cropper = cropper;

    this.slider = document.createElement('div');
    this.slider.setAttribute("class", "slidertop");
    this.cropper.element.appendChild(this.slider);

    Event.observe(this.slider, 'mousedown', Slider.startDrag.bindAsEventListener(this));
    Event.observe(window, 'mouseup', Slider.stopDrag.bindAsEventListener(this));
};

SliderBottom.prototype.drag = function (event) {
    var left = 0;
    var width = this.cropper.image.width;
    if (Event.pointerY(event)-this.cropper.image.offsetTop <= this.cropper.block_top.offsetHeight) {
        var top = this.cropper.block_top.offsetHeight;
        var height = this.cropper.image.height-this.cropper.block_top.offsetHeight;
    } else {
        var top = Event.pointerY(event)-this.cropper.image.offsetTop;
        var height = this.cropper.image.height-(Event.pointerY(event)-this.cropper.image.offsetTop);
    }
    this.cropper.block_bottom.setAttribute("style","top:"+top+"px;left:"+left+"px;width:"+width+"px;height:"+height+"px;");

    this.cropper.update();
};

function SliderLeft (cropper) {
    this.cropper = cropper;
    
    this.slider = document.createElement('div');
    this.slider.setAttribute("class", "sliderleft");
    this.cropper.element.appendChild(this.slider);

    Event.observe(this.slider, 'mousedown', Slider.startDrag.bindAsEventListener(this));
    Event.observe(window, 'mouseup', Slider.stopDrag.bindAsEventListener(this));
};

SliderLeft.prototype.drag = function (event) {
    var top = 0;
    var left = 0;
    var height = this.cropper.image.height;
    if (Event.pointerX(event)-this.cropper.image.offsetLeft >= this.cropper.image.offsetWidth-this.cropper.block_right.offsetWidth) {
        var width = this.cropper.image.offsetWidth-this.cropper.block_right.offsetWidth;
    } else if (Event.pointerX(event)-this.cropper.image.offsetLeft >= this.cropper.image.offsetWidth) {
        var width = this.cropper.image.offsetWidth;
    } else {
        var width = Event.pointerX(event)-this.cropper.image.offsetLeft;
    }
    this.cropper.block_left.setAttribute("style","top:"+top+"px;left:"+left+"px;width:"+width+"px;height:"+height+"px;");

    this.cropper.update();
};

function SliderRight (cropper) {
    this.cropper = cropper;
    
    this.slider = document.createElement('div');
    this.slider.setAttribute("class", "sliderright");
    this.cropper.element.appendChild(this.slider);

    Event.observe(this.slider, 'mousedown', Slider.startDrag.bindAsEventListener(this));
    Event.observe(window, 'mouseup', Slider.stopDrag.bindAsEventListener(this));
};

SliderRight.prototype.drag = function (event) {
    var top = 0;
    var height = this.cropper.image.height;
    if (Event.pointerX(event)-this.cropper.image.offsetLeft <= this.cropper.block_left.offsetWidth) {
        var left = this.cropper.block_left.offsetWidth;
        var width = this.cropper.image.offsetWidth-this.cropper.block_left.offsetWidth;
    } else {
        var left = Event.pointerX(event)-this.cropper.image.offsetLeft;
        var width = this.cropper.image.width-(Event.pointerX(event)-this.cropper.image.offsetLeft);
    }
    this.cropper.block_right.setAttribute("style","top:"+top+"px;left:"+left+"px;;width:"+width+"px;height:"+height+"px;");

    this.cropper.update();
};

Slider = {
    "startDrag": function (event) {
        this.bind = this.drag.bindAsEventListener(this);
        Event.observe(window, 'mousemove', this.bind);
    },
    "stopDrag": function (event) {
        Event.stopObserving(window, 'mousemove', this.bind);
    }
};

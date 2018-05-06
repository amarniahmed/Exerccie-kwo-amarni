if (!Array.prototype.filter) {
  Array.prototype.filter = function(fun) {
    var len = this.length;
    if (typeof fun != "function")
      throw new TypeError();
    var res = new Array();
    var thisp = arguments[1];
    for (var i = 0; i < len; i++) {
      if (i in this) {
        var val = this[i]; 
        if (fun.call(thisp, val, i, this))
          res.push(val);
      }
    }
    return res;
  };
}

function toHex(color) {
  var body  = createPopup().document.body,
      range = body.createTextRange();
  body.style.color = color;
  var value = range.queryCommandValue("ForeColor");
  value = ((value & 0x0000ff) << 16) | (value & 0x00ff00) | ((value & 0xff0000) >>> 16);
  value = value.toString(16);
  return "#000000".slice(0, 7 - value.length) + value;
};


Object.extend(Date.prototype, {
  strftime: function(format) {
    var day = this.getUTCDay(), month = this.getUTCMonth();
    var hours = this.getUTCHours(), minutes = this.getUTCMinutes();
    function pad(num) { return num.toPaddedString(2); };

    return format.gsub(/\%([aAbBcdDHiImMpSwyY])/, function(part) {
      switch(part[1]) {
        case 'a': return $w("Sun Mon Tue Wed Thu Fri Sat")[day]; break;
        case 'A': return $w("Sunday Monday Tuesday Wednesday Thursday Friday Saturday")[day]; break;
        case 'b': return $w("Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec")[month]; break;
        case 'B': return $w("January February March April May June July August September October November December")[month]; break;
        case 'c': return this.toString(); break;
        case 'd': return this.getUTCDate(); break;
        case 'D': return pad(this.getUTCDate()); break;
        case 'H': return pad(hours); break;
        case 'i': return (hours === 12 || hours === 0) ? 12 : (hours + 12) % 12; break;
        case 'I': return pad((hours === 12 || hours === 0) ? 12 : (hours + 12) % 12); break;
        case 'm': return pad(month + 1); break;
        case 'M': return pad(minutes); break;
        case 'p': return hours > 11 ? 'PM' : 'AM'; break;
        case 'S': return pad(this.getUTCSeconds()); break;
        case 'w': return day; break;
        case 'y': return pad(this.getUTCFullYear() % 100); break;
        case 'Y': return this.getUTCFullYear().toString(); break;
      }
    }.bind(this));
  }
});

Element.addMethods({

  redraw: function(element){
    element = $(element);
    var n = document.createTextNode(' ');
    element.appendChild(n);
    (function(){n.parentNode.removeChild(n)}).defer();
    return element;
  },

  toHash: function() {
    return $(this).select("input,select,textarea").collect(Form.Element.serialize).join("&"); 
  }

});

if (window['console'] === undefined) {
  window.console = { log: Prototype.emptyFunction };
}

'kwo.shop.cart'.split('.').inject(window, function(parent, child) {
  var o = parent[child] = { }; return o;
})

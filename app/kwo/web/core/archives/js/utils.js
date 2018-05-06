window.bookmark = function() {
  if ("sidebar" in window) {
    window.sidebar.addPanel(document.title, document.location, ""); 
    return ;
  }
  if ("external" in window) {
    window.external.AddFavorite(document.location, document.title);
    return ;
  }
  var msg = Prototype.Browser.Opera ? "CTRL-T" : "CTRL-D";
  alert(msg);
}


Function.prototype.debounce = function (threshold, execAsap) {
 
    var func = this, timeout;
 
    return function debounced () {
        var obj = this, args = arguments;
        function delayed () {
            if (!execAsap)
                func.apply(obj, args);
            timeout = null; 
        };
 
        if (timeout)
            clearTimeout(timeout);
        else if (execAsap)
            func.apply(obj, args);
 
        timeout = setTimeout(delayed, threshold || 100); 
    };
 
}

var KwoMethods = {
  
  "printZone": function(element) {
    var id = "print_zone";
    if (!top.$(id)) {
      var frm = window.top.document.createElement("iframe");
      frm.setAttribute("id", id);
      frm.setAttribute("name", id);
      frm.style.visibility = "hidden";
      window.top.document.getElementsByTagName("body")[0].appendChild(frm);
      window.top.frames[id].document.open("text/html");
      window.top.frames[id].document.writeln("<html><head>"+
                                             '<link href="/back/core/ui.css" rel="stylesheet" type="text/css" />'+
                                             '</head><body></body></html>');
      window.top.frames[id].document.close();
      window.top.$(id).setStyle({"height": "1px", "width": "1px"});     
    }
    window.top.frames[id].onload = function() {
      window.top.frames[id].document.body.innerHTML = $(element).innerHTML;
      window.top.frames[id].focus();
      window.top.frames[id].print(); 
    }
  },
  
  "getCaretPosition": function(element) {
    if (element.createTextRange) {
      var range = document.selection.createRange().duplicate();
      range.moveEnd("character", element.value.length);
      return range.text.empty() 
           ? element.value.length
           : element.value.lastIndexOf(range.text);
    }
    return element.selectionStart;
  }
  
};

Element.addMethods(KwoMethods);
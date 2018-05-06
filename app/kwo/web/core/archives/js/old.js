
/*Kwo.Menu = {
  "opened": null,
  "binded": {},
  "timeout": null,
  "id": null,
  "bind": function() {
    $$(".bind-menu").each(function(item) {
      if (!$("menu-" + item.id)) return ;
      $("menu-" + item.id).addClassName("menu");
      item.observe("mouseover", function() {
        if (Kwo.Menu.id != null && Kwo.Menu.id != item.id) {
          $("menu-" + Kwo.Menu.id).hide();
        }
        Kwo.Menu.id = item.id;
        if (Kwo.Menu.timeout) {
          window.clearTimeout(Kwo.Menu.timeout);
        }
        Kwo.Menu.opened = $("menu-" + item.id);
        var pos = this.cumulativeOffset();
        Kwo.Menu.opened.setStyle({"top": (pos.top + this.getHeight() + 1) + "px",
                                  "left": pos.left + "px"});
        if (!(Kwo.Menu.opened.id in Kwo.Menu.binded)) {
          Kwo.Menu.binded[Kwo.Menu.opened.id] = true;
          Kwo.Menu.opened.onmouseover = Kwo.Menu.over;
          Kwo.Menu.opened.onmouseout = Kwo.Menu.out;
        }
        Kwo.Menu.opened.show();
      });

      item.observe("mouseout", function() {
        window.clearTimeout(Kwo.Menu.timeout);
        Kwo.Menu.timeout = setTimeout(function () {
          Kwo.Menu.opened.hide();
        }, 500);
      });
    });
  },

  "reset": function() {
    window.clearTimeout(Kwo.Menu.timeout);
    if ($("menu-" + Kwo.Menu.id)) {
      $("menu-" + Kwo.Menu.id).hide();
    }
  },

  "out": function () {
    $(Kwo.Menu.id).removeClassName("selected");
    window.clearTimeout(Kwo.Menu.timeout);
    this.hide();
  },

  "over": function() {
    $(Kwo.Menu.id).addClassName("selected");
    window.clearTimeout(Kwo.Menu.timeout);
    this.show();
  }
};*/


Selection = Class.create({

  "initialize": function(editor) {
    this.editor = editor;
  },

  "getType": function() {
    var type = "Text";
    var selection = this.editor.win.getSelection() ;
    if (selection.rangeCount == 1) {
      var range = selection.getRangeAt(0) ;
      if (range.startContainer == range.endContainer &&
          (range.endOffset - range.startOffset) == 1) {
        type = "Control" ;
      }
    }
    return type;
  },

  "getNode": function() {
    if (this.getType() != "Control") return null;
    var selection = this.editor.win.getSelection() ;
    var node = selection.anchorNode.childNodes[selection.anchorOffset] ;
    return node;
  },

  "getParentElement": function() {
    if (this.getType() == "Control") return this.getNode().offsetParent ;
    var node = this.editor.win.getSelection().anchorNode ;
    while (node && node.nodeType != 1) node = node.parentNode ;
    return node;
  },

  "moveToAncestorNode": function(tag) {
    var container = this.getNode();
    if (!container) {
      container = this.editor.win.getSelection().getRangeAt(0).startContainer;
    }
    while (container) {
      if (container.tagName == tag) return container;
      container = container.parentNode;
    }
    return null;
  },

  "hasAncestorNode": function(tag) {
    var elt = this.editor.selection.getParentElement();
    while (elt) {
      if (elt.tagName == tag) break ;
      elt = elt.offsetParent;
    }
    return elt && elt.tagName == tag ? true : false;
  },

  "getAncestorElement": function(tag) {
    if (!elt) return elt;
    if (elt.tagName == tag.uppercase()) return elt;
    return elt.up("TD");
  },

  selElm : function() {
    var r = this.getRng();
    if(r.startContainer) {
      var contain = r.startContainer;
      if(r.cloneContents().childNodes.length == 1) {
        for(var i=0;i<contain.childNodes.length;i++) {
          var rng = contain.childNodes[i].ownerDocument.createRange();
          rng.selectNode(contain.childNodes[i]);
          if(r.compareBoundaryPoints(Range.START_TO_START,rng) != 1 &&
             r.compareBoundaryPoints(Range.END_TO_END,rng) != -1) {
            return $BK(contain.childNodes[i]);
          }
        }
      }
      return $BK(contain);
    } else {
      return $BK((this.getSel().type == "Control") ? r.item(0) : r.parentElement());
    }
  }

});

/*
Kwo.Geolocpicker = Class.create(Kwo.Dialog, {

  "initialize": function($super, input, opts) {
    this.name = "geoloc";
    this.input = input;
    $super("/core/dialog.geoloc", null, {"width": 640, "height":480});
    Kwo.setDialog("geoloc", this);
  },

  "getPoint": function () {
    if ($(this.input).up("tr") && $(this.input).up("tr").down("input.back")) {
      return new Array($(this.input).up("tr").down("input.latitude").value,
                       $(this.input).up("tr").down("input.longitude").value,
                       $(this.input).up("tr").down("input.zoom").value);
    } else {
      if (this.input.value.match(",")) {
        return this.input.value.split(",");
      }
    }
    return false;
  },

  "setValue": function(value) {
    if ($(this.input).up("tr") && $(this.input).up("tr").down("input.back")) {
      var values = value.split(",");
      $(this.input).up("tr").down("input.latitude").value = values[0];
      $(this.input).up("tr").down("input.longitude").value = values[1];
      $(this.input).up("tr").down("input.zoom").value = values[2];
    } else {
      $(this.input).setValue(value);
    }
    this.close();
  }
});

Kwo.GMap = {
  "dialog": null,
  "map": null,
  "geo": null,
  "marker": null,
  "init": function () {
    document.body.onunload = GUnload;
    Kwo.GMap.dialog = Kwo.getDialog();

    if (GBrowserIsCompatible()) {
      Kwo.GMap.map = new GMap2(document.getElementById('map'));
      Kwo.GMap.geo = new GClientGeocoder();
      var mapTypeControl = new GMapTypeControl();
      var topRight = new GControlPosition(G_ANCHOR_TOP_RIGHT, new GSize(10, 10));
      Kwo.GMap.map.addControl(mapTypeControl, topRight);
      Kwo.GMap.map.addControl(new GSmallMapControl());
      Kwo.GMap.map.enableScrollWheelZoom();
      GEvent.addListener(Kwo.GMap.map, 'zoomend', Kwo.GMap.changeZoom);
      GEvent.addListener(Kwo.GMap.map, 'click', Kwo.GMap.addOverlay);
      var point = Kwo.GMap.dialog.getPoint();
      if (point && point.length == 3) {
        Kwo.GMap.map.setCenter(new GLatLng(point[0], point[1]),parseInt(point[2]));
        Kwo.GMap.marker = new GMarker(new GLatLng(point[0], point[1]), {draggable: true});
        GEvent.addListener(Kwo.GMap.marker, 'dragstart', Kwo.GMap.moveOverlay);
        GEvent.addListener(Kwo.GMap.marker, 'dragend', Kwo.GMap.moveOverlay);
        GEvent.addListener(Kwo.GMap.marker, 'click', Kwo.GMap.moveOverlay);
        Kwo.GMap.map.addOverlay(Kwo.GMap.marker);
        $('geoloc').value = point[0]+','+point[1]+','+point[2];
      }
      else {
        Kwo.GMap.map.setCenter(new GLatLng(46.227638, 2.213749), 5);
      }
    }
  },

  "searchAddress": function (address) {
    Kwo.GMap.geo.getLatLng(address, Kwo.GMap.viewAddress);
  },

  "viewAddress": function (point) {
    Kwo.GMap.map.clearOverlays();
    if (point) {
      Kwo.GMap.marker = new GMarker(point, {draggable: true});
      GEvent.addListener(Kwo.GMap.marker, 'dragstart', Kwo.GMap.moveOverlay);
      GEvent.addListener(Kwo.GMap.marker, 'dragend', Kwo.GMap.moveOverlay);
      Kwo.GMap.map.addOverlay(Kwo.GMap.marker);
      Kwo.GMap.map.panTo(new GLatLng(point.y, point.x));
      $('geoloc').value = point.y+','+point.x+','+Kwo.GMap.map.getZoom();
    }
  },

  "addOverlay": function (overlay, point) {
    this.clearOverlays();
    if (point) {
      Kwo.GMap.marker = new GMarker(point, {draggable: true});
      GEvent.addListener(Kwo.GMap.marker, 'dragstart', Kwo.GMap.moveOverlay);
      GEvent.addListener(Kwo.GMap.marker, 'dragend', Kwo.GMap.moveOverlay);
      this.addOverlay(Kwo.GMap.marker);
      Kwo.GMap.map.panTo(new GLatLng(point.y, point.x));
      $('geoloc').value = point.y+','+point.x+','+Kwo.GMap.map.getZoom();
    }
  },

  "moveOverlay": function () {
    var point = this.getPoint();
    if (point) {
      $('geoloc').value = point.y+','+point.x+','+Kwo.GMap.map.getZoom();
    }
  },

  "changeZoom": function () {
    if (Kwo.GMap.marker) {
      var point = Kwo.GMap.marker.getPoint();
      $('geoloc').value = point.y+','+point.x+','+Kwo.GMap.map.getZoom();
    }
  }

};
*/
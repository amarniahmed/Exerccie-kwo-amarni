Kwo.Weather = {
  fetch: function(query) {
    if (query) {
      Kwo.Browser.setLocation(query);
      Kwo.exec("/core/widget.weather", {"query": query}, {"container": "weather"});
    }
  },
  search: function (query) {
    if (query) {
      var url = "/core/service.weather?type=search&query="+escape(query);
      new Ajax.Request(url, { method: "get", onSuccess: function (transport) {
        var resp = transport.responseText.evalJSON();
        var s = $("WeatherCode");
        while (o = s.lastChild) s.removeChild(o);
        Kwo.Weather.hide();
        if (resp["error"]) {
          $("WeatherError").innerHTML = resp["error"];
          $("WeatherError").show();
        } else {
          for (result in resp["result"]) {
            Kwo.Weather.addOption($("WeatherCode"),result,resp["result"][result],"",false);
          }
          $("WeatherSelect").show();;
        }
      } } );
    }
  },

  "addOption": function(obj, value, text, name, selected) {
    var opt = document.createElement("option");
    var txt = document.createTextNode(text)
    opt.appendChild(txt);
    opt.className = name;
    opt.value = value;
    opt.selected = selected;
    obj.appendChild(opt);
  },

  hide: function () {
    $("WeatherError").hide();
    $("WeatherSearch").hide();
    $("WeatherSelect").hide();
  },
  toggle: function () {
    if ($("WeatherSearch").style.display != "none" || $("WeatherSelect").style.display != "none") {
      Kwo.Weather.hide();
    } else {
      Kwo.Weather.hide();
      $("WeatherSearch").show();
    }
  }
};
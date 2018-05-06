

Kwo.WebSearch = {
  "search": function (query,type) {
    $("WebSearchResults").style.display = "none";
    $("WebSearchQuery").value = query;
    $("WebSearchQuery").focus();
    switch (type) {
    case "google":
      Kwo.WebSearch.googleSearch(query);
      break;
    case "yahoo":
      Kwo.WebSearch.yahooSearch(query);
      break;
    case "msn":
      Kwo.WebSearch.msnSearch(query);
      break;
    }
  },
  "googleSearch": function(query) {
    var google = new GwebSearch();
    google.setResultSetSize(GSearch.LARGE_RESULTSET);
    google.setSearchCompleteCallback(document, function () {
      $("WebSearchError").style.display = "none";
      $("WebSearchLinks").style.display = "none";
      var links = "";
      if (google.results.length == 0) {
        $("WebSearchError").style.display = "";
      } else {
        google.results.each(function(result) { 
          links += "<div class=\"link\"><a href=\""+result.url+"\" target=\"_blank\">"+result.titleNoFormatting+"</a>";
          if (result.content) links += "<div>"+result.content.replace(/<br>/,"")+"</div>";
          links += "</div>";
        } );
        $("WebSearchLinks").innerHTML = links;
        $("WebSearchLinks").style.display = "";
      }
      $("WebSearchResults").style.display = "block";
    } );
    $("WebSearchLinks").innerHTML = "";
    google.clearResults();
    google.execute(query);
  },
  "yahooSearch": function(query) {
    $("WebSearchError").style.display = "none";
    $("WebSearchLinks").style.display = "none";
    var url = "/core/service.websearch?type=yahoo&query="+escape(query);
    new Ajax.Request(url, { method: "get", onSuccess: function (transport) { 
      var resp = transport.responseText.evalJSON();
      if (resp["result"]["ResultSet"]["Result"].length == 0) {
        $("WebSearchError").style.display = "";
      } else {
        $("WebSearchLinks").innerHTML = "";
        var links = "";
        resp["result"]["ResultSet"]["Result"].each(function(result) { 
          links += "<div class=\"link\"><a href=\""+result.Url+"\" target=\"_blank\">"+result.Title+"</a>";
          if (result.Summary) links += "<div>"+result.Summary.replace(/<br>/,"")+"</div>";
          links += "</div>";
        } );
        $("WebSearchLinks").innerHTML = links;
        $("WebSearchLinks").style.display = "";
      }
      $("WebSearchResults").style.display = "";
    } } );
  },
  msnSearch: function(query) {
    $("WebSearchError").style.display = "none";
    $("WebSearchLinks").style.display = "none";
    var url = "/core/service.websearch?type=msn&query="+escape(query);
    new Ajax.Request(url, { method: "get", onSuccess: function (transport) { 
      var resp = transport.responseText.evalJSON();
      if (resp["result"]["Results"]["Result"] == undefined) {
        $("WebSearchError").style.display = "";
      } else {
        $("WebSearchLinks").innerHTML = "";
        var links = "";
        resp["result"]["Results"]["Result"].each(function(result) {
          links += "<div class=\"link\"><a href=\""+result.Url+"\" target=\"_blank\">"+result.Title+"</a>";
          if (result.Description) links += "<div>"+result.Description.replace(/<br>/,"")+"</div>";
          links += "</div>";
        } );
        $("WebSearchLinks").innerHTML = links;
        $("WebSearchLinks").style.display = "";
      }
      $("WebSearchResults").style.display = "";
    } } );
  },
  "getType": function() {
    if ($("TypeGoogle").checked) return "google";
    if ($("TypeYahoo").checked) return "yahoo";
    if ($("TypeMsn").checked) return "msn";
  } 
};
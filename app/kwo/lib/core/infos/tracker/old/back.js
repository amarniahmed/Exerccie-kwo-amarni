
Kwo.Stats = {

  onExec: function(elt) {
    elt = $(elt);
    var action = elt.readAttribute("data-action");
    var block = elt.up("TD.stat-block");
    var period = block.readAttribute("data-period");
    if ($(period)) {
      $(period).value = block.down("SELECT").getValue();
    }
    $("day").value = $F("select-day"); 
    $("month").value = $F("select-month"); 
    $("year").value = $F("select-year");
    if (period == "day") { }
    else if (period == "month") { 
      $("day").value = 0; 
    }
    else if (period == "year") { 
      $("day").value = 0; 
      $("month").value = 0; 
    }
    else {
      $("day").value = 0; 
      $("month").value = 0; 
      $("year").value = 0; 
    }
    if (action == "export") {
      Kwo.go("/back/tracker/export", $("formStat"));
      return ;
    }
    if (action == "load") {
      Kwo.go("/back/tracker/load", $("formStat"));
      return ;
    }
    if (action == "visits") {
      window.open("/back/tracker/visit?year=" + $F("year") + "&month=" + $F("month") + "&day=" + $F("day"),
                  "_blank",
                  "toolbar=no,location=yes,directories=no,status=no,menubar=no,scrollbars=no,resizable=no,copyhistory=no,width=900,height=480,hotkeys=no");
      return ;
    }
    var args = $("formStat");
    if (action == "referer") {
      args = [args, {referer_type: elt.readAttribute("data-type")}];
    }
    Kwo.exec("/back/tracker/" + action, args,
             {container: block.down("DIV.panel")});
  },

  onToggle: function(elt, n) {
    elt = $(elt);
    var panel = elt.up("DIV.panel");
    panel.select("DIV").invoke("hide");
    panel.down("DIV.divCat" + n).show();
  },

  onPrint: function(elt) {
    if ($F("nb_line")<1) return;
    window.focus();
    window.print();
  }

}




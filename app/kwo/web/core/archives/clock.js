
Kwo.Clock = {
  "get": function () {
    if (!$("clock")) return;
    var use_local_flag = false;
    var date = new Date(), s;
    if (window._locale === undefined) {
      var locale = 1;
    }
    else {
      var locale = _locale;
    }
    if (use_local_flag == true) {
      s = date.toLocaleString().substr(0, (s.length - 3)).capitalize();
    }
    else {
      var month_names = new Array();
      month_names[1] = new Array("Janvier", "Février", "Mars", "Avril", "Mai", "Juin", "Juillet", "Août", "Septembre", "Octobre", "Novembre", "Décembre");
      month_names[2] = new Array("January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December");
      
      var day_names = new Array();
      day_names[1] = new Array("Dimanche", "Lundi", "Mardi", "Mercredi", "Jeudi", "Vendredi", "Samedi");
      day_names[2] = new Array("Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday");
      
      var now = new Date();
      
      if (locale == 1) {
        s = day_names[locale][now.getDay()]+" "+now.getDate()+" "+month_names[locale][now.getMonth()]+" "+now.getFullYear();
      }
      else {
        s = day_names[locale][now.getDay()]+", "+month_names[locale][now.getMonth()]+" "+now.getDate()+", "+now.getFullYear();
      }
      s += " "+now.getHours()+":"+now.getMinutes().toPaddedString(2);
    }
    $("clock").innerHTML = s;
    setTimeout(function () { Kwo.Clock.get(); }, 10000);
  }
}
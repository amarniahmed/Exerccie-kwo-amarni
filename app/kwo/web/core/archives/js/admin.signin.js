document.observe("dom:loaded", function() {
  
  if (Modernizr.svg && Modernizr.canvas && Modernizr.video && 
      Modernizr.rgba && Modernizr.boxshadow && Modernizr.textshadow && 
      Modernizr.borderradius && Modernizr.fontface &&
      Modernizr.opacity && Modernizr.cssgradients && Modernizr.flexbox) {
    $("auth_form").show();
    $("login").focus();
    $("auth_form").observe("submit", function (evt) {
      evt.stop();
      Kwo.exec("/front/core/admin.login", this,
               {callback: function (res) {
                 if (Kwo.hasError(res)) {
                   $("auth_form").reset();
                   $("login").focus();
                   $("auth_error").show();
                   return ;
                 }
                 Kwo.go(res["result"]["callback_url"]);
               }});
    });
  }
  else {
    $("alert").show();
  }
  
});
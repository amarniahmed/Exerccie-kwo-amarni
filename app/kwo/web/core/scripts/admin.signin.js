document.observe("dom:loaded", function() {
  
/*  if (Modernizr.svg && Modernizr.canvas && Modernizr.video && 
      Modernizr.rgba && Modernizr.boxshadow && Modernizr.textshadow && 
      Modernizr.borderradius && Modernizr.fontface &&
      Modernizr.opacity && Modernizr.cssgradients && Modernizr.flexbox) {*/
  if (true) {
    $("auth_form").show();
    $("login").focus();
    $("auth_form").observe("submit", function (evt) {
      evt.stop();
      Kwo.exec("/admin.login", this,
               {"callback": function (resp) {
                 resp = new kwo.Response(resp);
                 if (resp.hasError()) {
                   $("auth_form").reset();
                   $("login").focus();
                   $("auth_error").show();
                   return ;
                 }
                 resp.redirect();
               }});
    });
  }
  else {
    $("alert").show();
  }
  
});

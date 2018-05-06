  this.dom2xml = function(node, indent) {
    var xbuf = '';
    var f_indent = '';
    var e_indent = '';
    var f_crlf = '';
    var e_crlf = '';
    var fx_crlf = '';
    for (var i=0; i<node.childNodes.length; i++) {
      var chnode = node.childNodes[i];
      var node_type = chnode.nodeType;
      if (node_type == 3) { // text node 
        if (KwoUtils.trim(chnode.nodeValue).length > 0) {
          xbuf += KwoUtils.trimLineBreaks(KwoUtils.htmlEncode(chnode.nodeValue));
        }
      }
      else if (node_type == 8) { // comment node
        xbuf += "<!--" + chnode.nodeValue + "-->";
      }
      else if (node_type == 1) { // html element
        if (chnode.getAttribute("__processed") == null) { // workaround to prevent elements from doubling
          chnode.setAttribute("__processed", true);
          // form attributes string
          var attr_str = '';
          for(var j=0; j<chnode.attributes.length; j++) {
            var attr = chnode.attributes[j];
            if (attr.nodeValue != null 
                && (chnode.getAttribute(attr.nodeName, 2)!=null || chnode.getAttribute(attr.nodeName, 0) != null)
                && attr.specified && attr.nodeName.toLowerCase()!="__processed" 
                && attr.nodeName.toLowerCase().indexOf("_moz") != 0) {
              var attrval = chnode.getAttribute(attr.nodeName, 2);
              if (attrval == null)
                attrval = chnode.getAttribute(attr.nodeName, 0);
              
              if (chnode.tagName.toLowerCase() != 'font') {
                if (attr.nodeName.toLowerCase() != 'class' && attr.nodeName.toLowerCase() != 'style'
                    && attr.nodeName.toLowerCase() != 'href' && attr.nodeName.toLowerCase() != 'src' 
                    && attr.nodeName.toLowerCase() != 'shape' && attr.nodeName.toLowerCase() != 'coords' // coords and shape attributes are lost in area tag in IE 
                    && attr.nodeName.toLowerCase() != 'type' && attr.nodeName.toLowerCase() != 'value' 
                    && attr.nodeName.toLowerCase() != 'enctype')  { // type and value get lost on IE in input tags
                  attr_str += ' ' + attr.nodeName.toLowerCase() + '="' + attrval + '"';
                }
              }
              else {
                // replace font tag with span
                if (attr.nodeName.toLowerCase() == 'face')
                  chnode.style.fontFamily = attrval;
                else if (attr.nodeName.toLowerCase() == 'size') {
                  switch(attrval) {
                  case '1':
                    attrval = 'xx-small';
                    break;
                  case '2':
                    attrval = 'x-small';
                    break;
                  case '3':
                    attrval = 'small';
                    break;
                  case '4':
                    attrval = 'medium';
                    break;
                  case '5':
                    attrval = 'large';
                    break;
                  case '6':
                    attrval = 'x-large';
                    break;
                  case '7':
                    attrval = 'xx-large';
                    break;
                  default:
                    attrval = 'medium';
                    break;
                  }
                  chnode.style.fontSize = attrval;
                } 
                else if (attr.nodeName.toLowerCase() == 'color') {
                  chnode.style.color = attrval; 
                }
                // crop attribute string
                attr_str = '';
              }
            }
            
          }
          // style attribute
          if (chnode.style.cssText && chnode.style.cssText != '') {
            attr_str += ' style="' + chnode.style.cssText + '"';
          }
          
          // class attribute
          if (chnode.className && chnode.className != '') {
            attr_str += ' class="' + chnode.className + '"';
          }
          if (chnode.type && chnode.type != '')
            attr_str += ' type="' + chnode.type + '"';
          if (chnode.value && chnode.value != '' 
              && !(chnode.tagName.toLowerCase() == 'li' && chnode.value == '-1')) // workaround for firefox 
            attr_str += ' value="' + chnode.value + '"';
          if (chnode.enctype && chnode.enctype != '' && chnode.enctype != 'application/x-www-form-urlencoded')
            attr_str += ' enctype="' + chnode.enctype + '"';
          
          // replace & to &amp; in href and src attributes
          if (chnode.href && chnode.href != '' && chnode.tagName.toLowerCase() != 'img')
                attr_str += ' href="' +  chnode.href.replace("&amp;","&").replace("&","&amp;") + '"';
          if (chnode.src && chnode.src != '')
            attr_str += ' src="' +  chnode.src.replace("&amp;","&").replace("&","&amp;") + '"';
          
              // shape and coords attributes
          if (chnode.coords && chnode.coords != '') 
            attr_str += ' coords="' + chnode.coords + '"'; 
          if (chnode.shape && chnode.shape != '') 
            attr_str += ' shape="' + chnode.shape + '"'; 
          
          if (true) {
            switch(chnode.tagName.toLowerCase()) {
            case "p":
            case "td":
            case "th":
            case "label":
            case "li":
              //            case "br":
              f_indent = indent;
              f_crlf = '\n';
              e_indent = '';
              e_crlf = '';
              break;
            case "table":
            case "thead":
            case "tfoot":
            case "tbody":
            case "tr":
            case "div":
            case "ul":
            case "ol":
              f_indent = indent;
              f_crlf = '\n';
              e_indent = indent;
              e_crlf = '\n';
              break;
             case "h1":
              f_indent = '';
              f_crlf = '';
              e_indent = '';
              e_crlf = '';
              fx_crlf = '\n';
              break;  
            default:
              f_indent = '';
              f_crlf = '';
              e_indent = '';
              e_crlf = '';
            }
          }  
          if (chnode.tagName.toLowerCase() != "script") {
            // replace font with span
            var tag_name = (chnode.tagName.toLowerCase() != 'font')?chnode.tagName.toLowerCase():'span';
            if (chnode.childNodes.length>0) {
              var innercode = this.dom2xml(chnode, indent + ((f_indent!="tmp")?"  ":""));
              if (KwoUtils.trim(innercode) == '') {
                innercode = '&nbsp;';
              }
              xbuf += f_crlf + f_indent + "<" + KwoUtils.trim(tag_name + attr_str) + ">" + innercode + e_crlf + e_indent + "</" + tag_name + ">" + fx_crlf;
            }
            else if (chnode.tagName.indexOf("/") == -1) { // empty tag (sometimes ending tag is passed as a separate node)
              if (tag_name == "img"
                  || tag_name == "br"
                  || tag_name == "wbr"
                  || tag_name == "hr"
                  || tag_name == "input") {
                xbuf += f_crlf + f_indent + "<" + KwoUtils.trim(tag_name + attr_str) + " />" + e_crlf + e_indent;
              }
              else {
                // don't generate empty useless tags
                if (tag_name != "b"
                    && tag_name != "i"
                    && tag_name != "u"
                    && tag_name != "strike"
                    && tag_name != "strong"
                    && tag_name != "em"
                    && tag_name != "i"
                    && tag_name != "span"
                    ) {
                  var innercode = '';
                  if (tag_name == 'p')
                    innercode = '&nbsp;';
                  xbuf += f_crlf + f_indent + "<" + KwoUtils.trim(tag_name + attr_str) + ">" + innercode + "</" + tag_name + ">";
                }
              }
            }
          }
          else {
            // script
            xbuf += f_crlf + f_indent + "<" + KwoUtils.trim(chnode.tagName.toLowerCase() + attr_str) + ">" + chnode.innerHTML + "</" + chnode.tagName.toLowerCase() + ">";
          }
        }
      }
    }
    return xbuf;
  }








function KwoUtils()
{
}
KwoUtils.ltrim = function(txt)
{
  var spacers = " \t\r\n";
  while (txt.length>0 && spacers.indexOf(txt.charAt(0)) != -1)
  {
    txt = txt.substr(1);
  }
  return(txt);
}
KwoUtils.rtrim = function(txt)
{
  var spacers = " \t\r\n";
  while (txt.length>0 && spacers.indexOf(txt.charAt(txt.length-1)) != -1)
  {
    txt = txt.substr(0,txt.length-1);
  }
  return(txt);
}
KwoUtils.trim = function(txt)
{
  return(KwoUtils.ltrim(KwoUtils.rtrim(txt)));
}
KwoUtils.trimLineBreaks = function(txt)
{
  var spacers = "\r\n";
  while (txt.length>0 && spacers.indexOf(txt.charAt(txt.length-1)) != -1)
  {
    txt = txt.substr(0,txt.length-1);
  }
  while (txt.length>0 && spacers.indexOf(txt.charAt(0)) != -1)
  {
    txt = txt.substr(1);
  }
  return(txt);  
}
KwoUtils.htmlEncode = function(txt)
{
  return txt.replace(/&/gm,'&amp;').replace(/</gm,'&lt;').replace(/>/gm, '&gt;').replace(/\u00A0/g, "&nbsp;");
}
KwoUtils.getPageOffsetLeft = function(obj)
{
    var elm = obj;
    x = obj.offsetLeft;
    while ((elm = elm.offsetParent) != null)
    {
      x += elm.offsetLeft;
    }
    return x;
}
KwoUtils.getPageOffsetTop = function(obj)
{
    var elm = obj;
    y = obj.offsetTop;
    while ((elm = elm.offsetParent) != null)
    {
      y += elm.offsetTop;
    }
    return y;
}

// replaces special characters with HTML entities
function convertToEntities(src_string)
{
  var result = src_string;
  
  var entities = {
    // Latin-1
    "Â¡":"&iexcl;",
    "Â¢":"&cent;",
    "Â£":"&pound;",
    "Â¤":"&curren;",
    "Â¥":"&yen;",
    "Â¦":"&brvbar;",
    "Â§":"&sect;",
    "Â¨":"&uml;",
    "Â©":"&copy;",
    "Âª":"&ordf;",
    "Â«":"&laquo;",
    "Â¬":"&not;",
    "Â­":"&shy;",
    "Â®":"&reg;",
    "Â¯":"&macr;",
    "Â°":"&deg;",
    "Â±":"&plusmn;",
    "Â²":"&sup2;",
    "Â³":"&sup3;",
    "Â´":"&acute;",
    "Âµ":"&micro;",
    "Â¶":"&para;",
    "Â·":"&middot;",
    "Â¸":"&cedil;",
    "Â¹":"&sup1;",
    "Âº":"&ordm;",
    "Â»":"&raquo;",
    "Â¼":"&frac14;",
    "Â½":"&frac12;",
    "Â¾":"&frac34;",
    "Â¿":"&iquest;",
    "Ã":"&Agrave;",
    "Ã":"&Aacute;",
    "Ã":"&Acirc;",
    "Ã":"&Atilde;",
    "Ã":"&Auml;",
    "Ã":"&Aring;",
    "Ã":"&AElig;",
    "Ã":"&Ccedil;",
    "Ã":"&Egrave;",
    "Ã":"&Eacute;",
    "Ã":"&Ecirc;",
    "Ã":"&Euml;",
    "Ã":"&Igrave;",
    "Ã":"&Iacute;",
    "Ã":"&Icirc;",
    "Ã":"&Iuml;",
    "Ã":"&ETH;",
    "Ã":"&Ntilde;",
    "Ã":"&Ograve;",
    "Ã":"&Oacute;",
    "Ã":"&Ocirc;",
    "Ã":"&Otilde;",
    "Ã":"&Ouml;",
    "Ã":"&times;",
    "Ã":"&Oslash;",
    "Ã":"&Ugrave;",
    "Ã":"&Uacute;",
    "Ã":"&Ucirc;",
    "Ã":"&Uuml;",
    "Ã":"&Yacute;",
    "Ã":"&THORN;",
    "Ã":"&szlig;",
    "Ã ":"&agrave;",
    "Ã¡":"&aacute;",
    "Ã¢":"&acirc;",
    "Ã£":"&atilde;",
    "Ã¤":"&auml;",
    "Ã¥":"&aring;",
    "Ã¦":"&aelig;",
    "Ã§":"&ccedil;",
    "Ã¨":"&egrave;",
    "Ã©":"&eacute;",
    "Ãª":"&ecirc;",
    "Ã«":"&euml;",
    "Ã¬":"&igrave;",
    "Ã­":"&iacute;",
    "Ã®":"&icirc;",
    "Ã¯":"&iuml;",
    "Ã°":"&eth;",
    "Ã±":"&ntilde;",
    "Ã²":"&ograve;",
    "Ã³":"&oacute;",
    "Ã´":"&ocirc;",
    "Ãµ":"&otilde;",
    "Ã¶":"&ouml;",
    "Ã·":"&divide;",
    "Ã¸":"&oslash;",
    "Ã¹":"&ugrave;",
    "Ãº":"&uacute;",
    "Ã»":"&ucirc;",
    "Ã¼":"&uuml;",
    "Ã½":"&yacute;",
    "Ã¾":"&thorn;",
    "Ã¿":"&yuml;",
    // symbols and greek
    "Æ":"&fnof;",
    "Î":"&Alpha;",
    "Î":"&Beta;",
    "Î³":"&Gamma;",
    "Î":"&Delta;",
    "Î":"&Epsilon;",
    "Î":"&Zeta;",
    "Î":"&Eta;",
    "Î":"&Theta;",
    "Î":"&Iota;",
    "Î":"&Kappa;",
    "Î":"&Lambda;",
    "Î":"&Mu;",
    "Î":"&Nu;",
    "Î":"&Xi;",
    "Î":"&Omicron;",
    "Î ":"&Pi;",
    "Î¡":"&Rho;",
    "Î£":"&Sigma;",
    "Î¤":"&Tau;",
    "Î¥":"&Upsilon;",
    "Î¦":"&Phi;",
    "Î§":"&Chi;",
    "Î¨":"&Psi;",
    "Î©":"&Omega;",
    "Î±":"&alpha;",
    "Î²":"&beta;",
    "Î³":"&gamma;",
    "Î´":"&delta;",
    "Îµ":"&epsilon;",
    "Î¶":"&zeta;",
    "Î·":"&eta;",
    "Î¸":"&theta;",
    "Î¹":"&iota;",
    "Îº":"&kappa;",
    "Î»":"&lambda;",
    "Î¼":"&mu;",
    "Î½":"&nu;",
    "Î¾":"&xi;",
    "Î¿":"&omicron;",
    "Ï":"&pi;",
    "Ï":"&rho;",
    "Ï":"&sigmaf;",
    "Ï":"&sigma;",
    "Ï":"&tau;",
    "Ï":"&upsilon;",
    "Ï":"&phi;",
    "Ï":"&chi;",
    "Ï":"&psi;",
    "Ï":"&omega;",
    "â¢":"&bull;",
    "â¦":"&hellip;",
    "â²":"&prime;",
    "â³":"&Prime;",
    "â¾":"&oline;",
    "â":"&frasl;",
    "â":"&weierp;",
    "â":"&image;",
    "â":"&real;",
    "â¢":"&trade;",
    "âµ":"&alefsym;",
    "â":"&larr;",
    "â":"&uarr;",
    "â":"&rarr;",
    "â":"&darr;",
    "â":"&harr;",
    "âµ":"&crarr;",
    "â":"&lArr;",
    "â":"&uArr;",
    "â":"&rArr;",
    "â":"&hArr;",
    "â":"&forall;",
    "â":"&part;",
    "â":"&exist;",
    "â":"&empty;",
    "â":"&nabla;",
    "â":"&isin;",
    "â":"&notin;",
    "â":"&ni;",
    "â":"&prod;",
    "â":"&sum;",
    "â":"&minus;",
    "â":"&lowast;",
    "â":"&radic;",
    "â":"&prop;",
    "â":"&infin;",
    "â§":"&and;",
    "â¨":"&or;",
    "â©":"&cap;",
    "âª":"&cup;",
    "â«":"&int;",
    "â":"&cong;",
    "â":"&asymp;",
    "â ":"&ne;",
    "â¡":"&equiv;",
    "â¤":"&le;",
    "â¥":"&ge;",
    "â":"&sub;",
    "â":"&sup;",
    "â":"&nsub;",
    "â":"&sube;",
    "â":"&supe;",
    "â":"&oplus;",
    "â":"&otimes;",
    "â¥":"&perp;",
    "â":"&sdot;",
    "â":"&lceil;",
    "â":"&rceil;",
    "â":"&lfloor;",
    "â":"&rfloor;",
    "â©":"&lang;",
    "âª":"&rang;",
    "â":"&loz;",
    "â ":"&spades;",
    "â£":"&clubs;",
    "â¥":"&hearts;",
    "â¦":"&diams;",
    // special chars
    "Å":"&OElig;",
    "Å":"&oelig;",
    "Å ":"&Scaron;",
    "Å¡":"&scaron;",
    "Å¸":"&Yuml;",
    "Ë":"&circ;",
    "Ë":"&tilde;",
    "â":"&ensp;",
    "â":"&emsp;",
    "â":"&thinsp;",
    "â":"&zwnj;",
    "â":"&zwj;",
    "â":"&lrm;",
    "â":"&rlm;",
    "â":"&ndash;",
    "â":"&mdash;",
    "â":"&lsquo;",
    "â":"&rsquo;",
    "â":"&sbquo;",
    "â":"&bdquo;",
    "â ":"&dagger;",
    "â¡":"&Dagger;",
    "â°":"&permil;",
    "â¹":"&lsaquo;",
    "âº":"&rsaquo;",
    "â¬":"&euro;",
    "â":"&ldquo;",
    "â":"&rdquo;"
  }
  var entities_str = "Â¡Â¢Â£Â¤Â¥Â¦Â§Â¨Â©ÂªÂ«Â¬Â­Â®Â¯Â°Â±Â²Â³Â´ÂµÂ¶Â·Â¸Â¹ÂºÂ»Â¼Â½Â¾Â¿ÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃÃ Ã¡Ã¢Ã£Ã¤Ã¥Ã¦Ã§Ã¨Ã©ÃªÃ«Ã¬Ã­Ã®Ã¯Ã°Ã±Ã²Ã³Ã´ÃµÃ¶Ã·Ã¸Ã¹ÃºÃ»Ã¼Ã½Ã¾Ã¿ÆÎÎÎ³ÎÎÎÎÎÎÎÎÎÎÎÎÎ Î¡Î£Î¤Î¥Î¦Î§Î¨Î©Î±Î²Î³Î´ÎµÎ¶Î·Î¸Î¹ÎºÎ»Î¼Î½Î¾Î¿ÏÏÏÏÏÏÏÏÏÏâ¢â¦â²â³â¾âââââ¢âµââââââµââââââââââââââââââââ§â¨â©âªâ«âââ â¡â¤â¥ââââââââ¥ââââââ©âªââ â£â¥â¦ÅÅÅ Å¡Å¸ËËââââââââââââââââ â¡â°â¹âºâ¬";

  var rgx = new RegExp("[" + entities_str + "]", "gm");
  
  var matches = result.match(rgx);
  if (matches != null)
  {
    var processed = new Array();
    
    for (var i=0; i<matches.length; i++)
    {
      if (processed[matches[i]] == null && entities[matches[i]] != null && entities[matches[i]] != undefined)
      {
        // register that the symbol was processed
        processed[matches[i]] = entities[matches[i]];
        var replace_rgx = new RegExp(matches[i],"gm");
        result = result.replace(replace_rgx, entities[matches[i]]);
      }
    }    
  }
  
  return result;   
}
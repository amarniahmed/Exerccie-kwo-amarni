var selectedCell = 0;
var selectedTab  = 0;

function getSelectIndex(_select, _value) {
  for (cpt=0; cpt < _select.options.length; cpt++) { if (_select.options[cpt].value == _value) return cpt; }
  return (0);
}

function switchDisplay() { Element.toggle("tabField", "cellField"); }

function initTabProp() {
  _editor = parent.<?=$params_arr['name']?>;
  cell = findCurrentCell();
  if (!cell) {
    alert("veuillez selectionner une cellule tout d'abord");
    parent.hideAllKwoLayer();
    return false; 
  }  
  selectedCell = cell;
  selectedTab = findParentTable(cell);
  initCellForm();
  initTabForm();
}


function initCellForm() {

  if (Kwo.isIE) {
    padLeft = parseInt(cell.currentStyle.paddingLeft.replace(/px/,""));
    padRight = parseInt(cell.currentStyle.paddingRight.replace(/px/,""));
    if (cell.currentStyle.width == "auto") {
      cellWidth = cell.offsetWidth - (padLeft+padRight) - 2;
      unite = "pixels";
    }
    else {
      if (cell.currentStyle.width.charAt(cell.currentStyle.width.length-1) == "%") {
	cellWidth = cell.currentStyle.width.replace(/%/, "");
	unite = "percent";
      }
      else {
	cellWidth = cell.currentStyle.width.replace(/px/, "");
	unite = "pixels";
      }
    }
    
    if (cell.currentStyle.height == "auto") cellHeight = cell.offsetHeight;
    else cellHeight = cell.currentStyle.height.replace(/px/, "");
  }
  
  else {
    if (cell.style.width == "") {
      cellWidth = cell.offsetWidth - 5;
      unite = "pixels";
    }
    else {
      if (cell.style.width.charAt(cell.style.width.length-1) == "%") {
	cellWidth = cell.style.width.replace(/%/, "");
	unite = "percent";
      }
      else {
	cellWidth = cell.style.width.replace(/px/, "");
	unite = "pixels";
      }
    }
    
    if (cell.style.height == "") cellHeight = cell.offsetHeight;
    else cellHeight = cell.style.height.replace(/px/, "");
  }
  
  $('cellWidth').value = cellWidth;
  $('cellHeight').value = cellHeight; 
  $('cellAlign').selectedIndex = getSelectIndex($('cellAlign'), cell.style.textAlign);
  $('cellVAlign').selectedIndex = getSelectIndex($('cellVAlign'), cell.style.verticalAlign);
  $('unite').selectedIndex = getSelectIndex($('unite'), unite);
  if ($('colorSel')) $('colorSel').style.backgroundColor = cell.style.backgroundColor;
}


function updateCellProp() {
  cell = selectedCell;
  unite = ($('unite').value == "pixels") ? "px" : "%";
  cell.removeAttribute("width");
  cell.removeAttribute("height");
  cell.removeAttribute("align");
  cell.removeAttribute("valign");
 
  if (unite == "px") {
    if (parseInt($F('cellWidth'))>table.offsetWidth) return alert("Taille de cellule trop importante");
  }
  else {
    var tMax = 101 - parseInt(cell.parentNode.childNodes.length);
    if (parseInt($F('cellWidth'))>tMax) return; 
  }


  if (parseInt($F('cellWidth'))!=0 && $F('cellWidth')!="") cell.style.width = $F('cellWidth')+unite;
  if (parseInt($F('cellHeight'))!=0 && $F('cellHeight')!="") cell.style.height = $F('cellHeight')+"px";  
  
  cell.style.textAlign = $F('cellAlign');
  cell.style.verticalAlign = $F('cellVAlign');

  if ($('colorSel')) cell.style.backgroundColor = $('colorSel').style.backgroundColor;
  selectedCell = 0; 
  parent.hideAllKwoLayer();

}

function initTabForm() {
  var table = selectedTab;
  var tableWidth = table.offsetWidth;
  var tableHeight = table.offsetHeight;
  var tableLayout = table.style.tableLayout;
  $('tableWidth').value = tableWidth;
  $('tableHeight').value = tableHeight; 
  $('tableLayout').selectedIndex = getSelectIndex($('tableLayout'), tableLayout);
  $('tablePadding').value = table.cellPadding;
  $('tableSpacing').value = table.cellSpacing;
  $('tableBorder').value = table.border;
}

function updateTabProp() {
  var table = selectedTab;
  var unite = ($F('tabUnite')=="pixels")?"px":"%";
  table.removeAttribute("width");
  table.removeAttribute("height");
  table.style.width       = $F('tableWidth')+unite;
  table.style.height      = $F('tableHeight')+"px";  
  table.style.tableLayout = $F('tableLayout');
  table.cellPadding       = $F('tablePadding');
  table.cellSpacing       = $F('tableSpacing');
  table.border            = $F('tableBorder');
  selectedTab             = 0;
  parent.hideAllKwoLayer();
}

function deleteTable() {
  var curTable = parent.<?=$params_arr['name']?>.selection.moveToAncestorNode('TABLE');
  if (curTable != undefined) curTable.parentNode.removeChild(curTable);
  parent.hideAllKwoLayer();
}

function insertAfter(sourceNode, newNode) {
  if (Kwo.isIE) sourceNode.insertAdjacentElement("AfterEnd", newNode); 
  else { 
    if (sourceNode.nextSibling) sourceNode.parentNode.insertBefore(newNode,sourceNode.nextSibling);
    else sourceNode.parentNode.appendChild(newNode);
  }
}

function findParentTable(cell) {
  var tbl = cell.offsetParent;
  while (tbl) {
    if (tbl.tagName == "TABLE") { return tbl; }
    tbl = tbl.offsetParent;
  }
  return false;
}

function findCurrentCell() {
  var elt = _editor.selection.getParentElement();
  while (elt) {
    if (elt.tagName == "TD") { break; }
    elt = elt.offsetParent;
  }
  
  if (elt && elt.tagName == "TD") { return (elt); }
  else return false;
}

function Table (cell) {
  this.cell      = cell;
  this.table     = findParentTable(this.cell);
  this.tbodyNode = this.table.firstChild;
  this.rowNode   = this.cell.parentNode;

  this.addRow = function() {
    newRow = this.rowNode.cloneNode(true);
    for (i=0; i<newRow.childNodes.length; i++) {
      newRow.childNodes[i].innerHTML = "&nbsp;";
      newRow.childNodes[i].removeAttribute('style');
    }
    insertAfter(this.rowNode, newRow);
  }
  
  this.addCol = function() {
    var index = -1;
    var cell = this.cell; 
    while (cell && cell.tagName == "TD") { cell = cell.previousSibling; index++; }
    for (i = 0; i < this.tbodyNode.childNodes.length; i++) {
      var row = this.tbodyNode.childNodes[i];
      cell = row.childNodes[index];
      insertAfter(cell,  document.createElement("TD"));
      cell.nextSibling.innerHTML = "&nbsp;";
    }
  }
  
  this.delRow = function() {
    if (!confirm("Etes-vous sur de vouloir effacer cette ligne ?")) return;
    var row = this.rowNode;
    var index = -1;
    while (row && row.tagName == "TR") { row = row.previousSibling; index++; }
    this.table.firstChild.removeChild(this.table.firstChild.childNodes[index]);
  }

  this.delCol = function() {
    if (!confirm("Etes-vous sur de vouloir effacer cette colonne ?")) return;
    var index = -1;
    var cell  = this.cell;
    while (cell && cell.tagName=="TD") { cell = cell.previousSibling; index++; }
    row_count = this.tbodyNode.childNodes.length;
    for (i=0; i<row_count; i++) {
      row = this.tbodyNode.childNodes[i];
      row.removeChild(r.childNodes[index]);
    }
  }
  //-----------------------------------------

}

var mytable = new Table(findCurrentCell());

//Event.observe(window, "load", initTabProp, false);
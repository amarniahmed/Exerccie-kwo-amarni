<?php

try {

  $csv = CsvReader::getInstance($req->file_path);

}
catch (Exception $e) {
  $log->error($e);
  $err->add($e);
  exit (0);
}

?>

<style>

  .csv-reader td,
  .csv-reader th {
    border: 1px solid #DDDDDD;
    padding: 8px;
    font-family: Arial;
    text-align: left;
    font-size: 12px;
  }

  .csv-reader th {
    font-size: 14px;
  }

  .csv-reader tbody tr:nth-child(even) {
    background: #E8F3FF
  }

  .csv-reader tbody tr:nth-child(odd) {
    background: #F9F9E7
  }

  .csv-reader thead tr,
  .csv-reader tbody tr th {
    background: #FFFFFF;
  }

  .csv-reader th.row__index {
    text-align: right
  }

  .csv-reader thead th.hidden {
    /*line-height: 0;
    padding: 0;*/
  }

  .csv-reader {
    position: absolute;
    bottom: 0;
    left: 0;
    right: 0;
    top: 60px;
    overflow: hidden;
  }

  .thead__container {
    position: absolute;
    top: 0;
    left: 0;
    right: 0;
    background: #959595;
  }

  /*.thead__container th { border-width: 0 1px; }*/
  .table__container {
    bottom: 0;
    overflow: auto;
    position: absolute;
    top: 0;
    left: 0;
    right: 0;
    background: #959595;
  }

  .csv td {
    position: relative;
  }
  .csv td > div {
    bottom: 0;
    left: 0;
    margin: 8px;
    overflow: hidden;
    position: absolute;
    right: 0;
    text-overflow: ellipsis;
    top: 0;
  }

  .csv td > div:hover {
    background: none repeat scroll 0 0 #ffffff;
    border: 1px solid #DDDDDD;
    margin: 0 !important;
    padding: 7px;
    text-overflow: inherit !important;
    /*width: 200px;*/
    z-index: 100;
    right: inherit;
    bottom: inherit;
  }

</style>
<div>
  <table>
    <tr>
      <th>field_enclosed_by</th>
      <td><?=$csv->field_enclosed_by?></td>
    </tr>
    <tr>
      <th>field_escaped_by</th>
      <td><?=$csv->field_escaped_by?></td>
    </tr>
    <tr>
      <th>field_separated_by</th>
      <td><?=$csv->field_separated_by?></td>
    </tr>
    <tr>
      <th>line_terminated_by</th>
      <td><?=$csv->line_terminated_by?></td>
    </tr>
    <tr>
      <th>fields_count</th>
      <td><?=$csv->fields_count?></td>
    </tr>

  </table>

</div>
<div class="csv-reader" id="csv-reader">
  <table class="csv">
    <thead>
    <tr>
      <th>&nbsp;</th>
      <?php foreach ($csv->getFields() as $field) : ?>
        <th><?= $field ?></th>
      <?php endforeach; ?>
    </tr>
    </thead>
    <tbody>
    <?php foreach ($csv as $i => $row) : if ($i > 20) break; ?>
      <tr>
        <th class="row__index"><?= $i + 1 ?></th>
        <?php foreach ($csv->getFields() as $field) : ?>
          <td>
            <div><?= $row[$field] ?></div>
          </td>
        <?php endforeach; ?>
      </tr>
    <?php endforeach; ?>
    </tbody>
    <tfoot></tfoot>
  </table>
</div>


<script type="text/javascript">
  var CsvReader = Class.create({
    parent: null,
    fields: null,


    initialize: function (parent) {
      this.parent = $(parent);
      this.fields = $H({});
      var table = this.parent.down('table');
      var thead = table.down('thead');

      var widthTotal = 0;
      thead.select('th').each(function (th) {
        var width = th.getWidth();
        th.setStyle({width: width + 'px'});
        widthTotal += 2 + width;
      });
      var theadFixed = thead.clone(true);

      table.setStyle('width:' + widthTotal + 'px');
      table.insert({before: '<div class="table__container"></div><div class="thead__container"><table style="width:' + widthTotal + 'px"></table></div>'});
      var theadContainer = this.parent.down('.thead__container');
      theadContainer.down('table').insert(theadFixed);

      var tableContainer = this.parent.down('.table__container');
      tableContainer.insert(table);


      tableContainer.observe('scroll',
        function (evt) {
          theadContainer.setStyle({left: '-' + tableContainer.scrollLeft + 'px'});
        });
    }
  });
  new CsvReader($('csv-reader'));
</script>
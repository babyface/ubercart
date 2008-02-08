// -*- js-var: set_line_item, base_path, li_titles, li_values, tax_weight; -*-
// $Id: uc_taxes.js,v 1.9.2.3 2008/02/08 22:22:43 rszrama Exp $

var pane = '';
if ($("input[@name*=delivery_]").length){
  pane = 'delivery'
}
else if($("input[@name*=billing_]").length){
  pane = 'billing'
}

$(document).ready(function(){
  getTax();
  $("select[@name*=" + pane + "_country], select[@name*=" + pane + "_zone], input[@name*=" + pane + "_postal_code], select[@name*=address_select]").change(getTax);
});

function getTax(){
  var products = $("[@name=cart_contents]").val();
  var zone = $("select[@name*=" + pane + "_zone]").val();
  if (!zone){
    zone = "0";
  }
  var code = $("input[@name*=" + pane + "_postal_code]").val();
  if (!code){
    code = '';
  }
  var country = $("select[@name*=" + pane + "_country]").val();
  if (!country){
    country = "0";
  }
  var order_size = 5;
  var line_item = '';
  var key;
  var i = 0;
  for (key in li_titles){
    if (key != 'subtotal'){
      line_item = line_item + 'i:' + i + ';a:3:{s:5:"title";s:' + li_titles[key].length + ':"' + li_titles[key] + '";s:4:"type";s:'+ key.length + ':"'+ key + '";s:6:"amount";d:' + li_values[key] + ';}';
      i++;
    }
  }
  line_item = 's:10:"line_items";a:' + i + ':{' + line_item + '}';
  var order = 'O:8:"stdClass":' + order_size + ':{s:8:"products";' + encodeURIComponent(products)
    + 's:13:"delivery_zone";i:' + zone
    + ';s:20:"delivery_postal_code";s:' + code.length +':"' + encodeURIComponent(code)
    + '";s:16:"delivery_country":i:' + country + ';'
    + line_item + '}';
  if (!!products){
    $.ajax({
      type: "POST",
      url: Drupal.settings['base_path'] + "taxes/calculate",
      data: 'order=' + order,
      dataType: "json",
      success: function(taxes){
        var key;
        for (key in li_titles){
          if (key.substr(0, 4) == 'tax_'){
            delete li_titles[key];
            delete li_values[key];
            delete li_weight[key];
          }
        }
        var j;
        for (j in taxes){
          if (taxes[j].id == 'subtotal'){
            summed = 0;
          }
          else {
            summed = 1;
          }
          set_line_item("tax_" + taxes[j].id, taxes[j].name, taxes[j].amount, tax_weight + taxes[j].weight / 10, summed, false);
        }
        render_line_items();
      }
    });
  }
}

// -*- js-var: set_line_item, base_path, li_titles, li_values, tax_weight; -*-
// $Id: uc_taxes.js,v 1.9.2.7 2008/09/17 21:38:43 rszrama Exp $

var pane = '';
if ($("input[@name*=delivery_]").length) {
  pane = 'delivery'
}
else if ($("input[@name*=billing_]").length) {
  pane = 'billing'
}

$(document).ready(function() {
  getTax();
  $("select[@name*=delivery_country], "
    + "select[@name*=delivery_zone], "
    + "input[@name*=delivery_city], "
    + "input[@name*=delivery_postal_code], "
    + "select[@name*=billing_country], "
    + "select[@name*=billing_zone], "
    + "input[@name*=billing_city], "
    + "input[@name*=billing_postal_code]").change(getTax);
});

function getTax() {
  var products = $("[@name=cart_contents]").val();
  var p_email = $("input[@name*=primary_email]").val()
  var s_f_name = $("input[@name*=delivery_first_name]").val();
  var s_l_name = $("input[@name*=delivery_last_name]").val();
  var s_street1 = $("input[@name*=delivery_street1]").val();
  var s_street2 = $("input[@name*=delivery_street2]").val();
  var s_city = $("input[@name*=delivery_city]").val();
  var s_zone = $("select[@name*=delivery_zone]").val();
  if (!s_zone) {
    s_zone = "0";
  }
  var s_code = $("input[@name*=delivery_postal_code]").val();
  if (!s_code) {
    s_code = '';
  }
  var s_country = $("select[@name*=delivery_country]").val();
  if (!s_country) {
    s_country = "0";
  }
  var b_f_name = $("input[@name*=billing_first_name]").val();
  var b_l_name = $("input[@name*=billing_last_name]").val();
  var b_street1 = $("input[@name*=billing_street1]").val();
  var b_street2 = $("input[@name*=billing_street2]").val();
  var b_city = $("input[@name*=billing_city]").val();
  var b_zone = $("select[@name*=billing_zone]").val();
  if (!b_zone) {
    b_zone = "0";
  }
  var b_code = $("input[@name*=billing_postal_code]").val();
  if (!b_code) {
    b_code = '';
  }
  var b_country = $("select[@name*=billing_country]").val();
  if (!b_country) {
    b_country = "0";
  }
  var order_size = 21;
  var line_item = '';
  var key;
  var i = 0;
  for (key in li_titles) {
    if (key != 'subtotal') {
      line_item = line_item + 'i:' + i + ';a:3:{s:5:"title";s:' + li_titles[key].length + ':"' + li_titles[key] + '";s:4:"type";s:'+ key.length + ':"'+ key + '";s:6:"amount";d:' + li_values[key] + ';}';
      i++;
    }
  }
  line_item = 's:10:"line_items";a:' + i + ':{' + line_item + '}';
  var order = 'O:8:"stdClass":' + order_size + ':{s:8:"products";' + encodeURIComponent(products)
    + 's:8:"order_id";i:0;'
    + 's:3:"uid";i:0;'
    + 's:13:"primary_email";s:' + p_email.length + ':"' + encodeURIComponent(p_email)
    + '";s:19:"delivery_first_name";s:' + s_f_name.length + ':"' + encodeURIComponent(s_f_name)
    + '";s:18:"delivery_last_name";s:' + s_l_name.length + ':"' + encodeURIComponent(s_l_name)
    + '";s:16:"delivery_street1";s:' + s_street1.length + ':"' + encodeURIComponent(s_street1)
    + '";s:16:"delivery_street2";s:' + s_street2.length + ':"' + encodeURIComponent(s_street2)
    + '";s:13:"delivery_city";s:' + s_city.length + ':"' + encodeURIComponent(s_city)
    + '";s:13:"delivery_zone";i:' + s_zone
    + ';s:20:"delivery_postal_code";s:' + s_code.length +':"' + encodeURIComponent(s_code)
    + '";s:16:"delivery_country";i:' + s_country + ';'
    + 's:18:"billing_first_name";s:' + b_f_name.length + ':"' + encodeURIComponent(b_f_name)
    + '";s:17:"billing_last_name";s:' + b_l_name.length + ':"' + encodeURIComponent(b_l_name)
    + '";s:15:"billing_street1";s:' + b_street1.length + ':"' + encodeURIComponent(b_street1)
    + '";s:15:"billing_street2";s:' + b_street2.length + ':"' + encodeURIComponent(b_street2)
    + '";s:12:"billing_city";s:' + b_city.length + ':"' + encodeURIComponent(b_city)
    + '";s:12:"billing_zone";i:' + b_zone
    + ';s:19:"billing_postal_code";s:' + b_code.length +':"' + encodeURIComponent(b_code)
    + '";s:15:"billing_country";i:' + b_country + ';'
    + line_item + '}';
  if (!!products) {
    $.ajax({
      type: "POST",
      url: Drupal.settings['base_path'] + "?q=taxes/calculate",
      data: 'order=' + order,
      dataType: "json",
      success: function(taxes) {
        var key;
        for (key in li_titles) {
          if (key.substr(0, 4) == 'tax_') {
            delete li_titles[key];
            delete li_values[key];
            delete li_weight[key];
          }
        }
        var j;
        for (j in taxes) {
          if (taxes[j].id == 'subtotal') {
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

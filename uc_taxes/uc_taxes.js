// $Id: uc_taxes.js,v 1.10.2.5 2009/02/04 13:44:31 islandusurper Exp $

/**
 * Calculate the number of bytes of a Unicode string.
 *
 * Gratefully stolen from http://dt.in.th/2008-09-16.string-length-in-bytes.html.
 * Javascript String.length returns the number of characters, but PHP strlen()
 * returns the number of bytes. When building serialize()d strings in JS,
 * use this function to get the correct string length.
 */
String.prototype.bytes = function() {
  // Drupal.encodeURIComponent() gets around some weirdness in
  // encodeURIComponent(), but encodes some characters twice. The first
  // replace takes care of those while the second lets String.length count
  // the multi-byte characters.
  return Drupal.encodeURIComponent(this).replace(/%252[36F]/g, 'x').replace(/%../g, 'x').length;
};

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

  var p_email = $("input[@name*=primary_email]").val() || '';
  var s_f_name = $("input[@name*=delivery_first_name]").val() || '';
  var s_l_name = $("input[@name*=delivery_last_name]").val() || '';
  var s_street1 = $("input[@name*=delivery_street1]").val() || '';
  var s_street2 = $("input[@name*=delivery_street2]").val() || '';
  var s_city = $("input[@name*=delivery_city]").val() || '';
  var s_zone = $("select[@name*=delivery_zone]").val() || '0';
  var s_code = $("input[@name*=delivery_postal_code]").val() || '';
  var s_country = $("select[@name*=delivery_country]").val() || '0';

  var b_f_name = $("input[@name*=billing_first_name]").val() || '';
  var b_l_name = $("input[@name*=billing_last_name]").val() || '';
  var b_street1 = $("input[@name*=billing_street1]").val() || '';
  var b_street2 = $("input[@name*=billing_street2]").val() || '';
  var b_city = $("input[@name*=billing_city]").val() || '';
  var b_zone = $("select[@name*=billing_zone]").val() || '0';
  var b_code = $("input[@name*=billing_postal_code]").val() || '';
  var b_country = $("select[@name*=billing_country]").val() || '0';

  var order_size = 21;
  var line_item = '';
  var key;
  var i = 0;
  for (key in li_titles) {
    if (key != 'subtotal') {
      line_item = line_item + 'i:' + i + ';a:3:{s:5:"title";s:' + li_titles[key].bytes() + ':"' + li_titles[key] + '";s:4:"type";s:'+ key.bytes() + ':"'+ key + '";s:6:"amount";d:' + li_values[key] + ';}';
      i++;
    }
  }
  line_item = 's:10:"line_items";a:' + i + ':{' + line_item + '}';
  var order = 'O:8:"stdClass":' + order_size + ':{s:8:"products";' + products
    + 's:8:"order_id";i:0;'
    + 's:3:"uid";i:0;'
    + 's:13:"primary_email";s:' + p_email.bytes() + ':"' + p_email
    + '";s:19:"delivery_first_name";s:' + s_f_name.bytes() + ':"' + s_f_name
    + '";s:18:"delivery_last_name";s:' + s_l_name.bytes() + ':"' + s_l_name
    + '";s:16:"delivery_street1";s:' + s_street1.bytes() + ':"' + s_street1
    + '";s:16:"delivery_street2";s:' + s_street2.bytes() + ':"' + s_street2
    + '";s:13:"delivery_city";s:' + s_city.bytes() + ':"' + s_city
    + '";s:13:"delivery_zone";i:' + s_zone
    + ';s:20:"delivery_postal_code";s:' + s_code.bytes() +':"' + s_code
    + '";s:16:"delivery_country";i:' + s_country + ';'
    + 's:18:"billing_first_name";s:' + b_f_name.bytes() + ':"' + b_f_name
    + '";s:17:"billing_last_name";s:' + b_l_name.bytes() + ':"' + b_l_name
    + '";s:15:"billing_street1";s:' + b_street1.bytes() + ':"' + b_street1
    + '";s:15:"billing_street2";s:' + b_street2.bytes() + ':"' + b_street2
    + '";s:12:"billing_city";s:' + b_city.bytes() + ':"' + b_city
    + '";s:12:"billing_zone";i:' + b_zone
    + ';s:19:"billing_postal_code";s:' + b_code.bytes() +':"' + b_code
    + '";s:15:"billing_country";i:' + b_country + ';'
    + line_item + '}';
  if (!!products) {
    $.ajax({
      type: "POST",
      url: Drupal.settings.basePath + "?q=taxes/calculate",
      data: 'order=' + Drupal.encodeURIComponent(order),
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
          set_line_item("tax_" + taxes[j].id, taxes[j].name, taxes[j].amount, Drupal.settings.ucTaxWeight + taxes[j].weight / 10, summed, false);
        }
        render_line_items();
      }
    });
  }
}

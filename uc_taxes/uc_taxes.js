// $Id: uc_taxes.js,v 1.10.2.4 2009/01/02 20:18:39 islandusurper Exp $

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
  var p_email = $("input[@name*=primary_email]").val();
  if (!p_email) {
    p_email = '';
  }
  var s_f_name = $("input[@name*=delivery_first_name]").val();
  if (!s_f_name) {
    s_f_name = '';
  }
  var s_l_name = $("input[@name*=delivery_last_name]").val();
  if (!s_l_name) {
    s_l_name = '';
  }
  var s_street1 = $("input[@name*=delivery_street1]").val();
  if (!s_street1) {
    s_street1 = '';
  }
  var s_street2 = $("input[@name*=delivery_street2]").val();
  if (!s_street2) {
    s_street2 = '';
  }
  var s_city = $("input[@name*=delivery_city]").val();
  if (!s_city) {
    s_city = '';
  }
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
  if (!b_f_name) {
    b_f_name = '';
  }
  var b_l_name = $("input[@name*=billing_last_name]").val();
  if (!b_l_name) {
    b_l_name = '';
  }
  var b_street1 = $("input[@name*=billing_street1]").val();
  if (!b_street1) {
    b_street1 = '';
  }
  var b_street2 = $("input[@name*=billing_street2]").val();
  if (!b_street2) {
    b_street2 = '';
  }
  var b_city = $("input[@name*=billing_city]").val();
  if (!b_city) {
    b_city = '';
  }
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
      line_item = line_item + 'i:' + i + ';a:3:{s:5:"title";s:' + li_titles[key].bytes() + ':"' + li_titles[key] + '";s:4:"type";s:'+ key.bytes() + ':"'+ key + '";s:6:"amount";d:' + li_values[key] + ';}';
      i++;
    }
  }
  line_item = 's:10:"line_items";a:' + i + ':{' + line_item + '}';
  var order = 'O:8:"stdClass":' + order_size + ':{s:8:"products";' + Drupal.encodeURIComponent(products)
    + 's:8:"order_id";i:0;'
    + 's:3:"uid";i:0;'
    + 's:13:"primary_email";s:' + p_email.bytes() + ':"' + Drupal.encodeURIComponent(p_email)
    + '";s:19:"delivery_first_name";s:' + s_f_name.bytes() + ':"' + Drupal.encodeURIComponent(s_f_name)
    + '";s:18:"delivery_last_name";s:' + s_l_name.bytes() + ':"' + Drupal.encodeURIComponent(s_l_name)
    + '";s:16:"delivery_street1";s:' + s_street1.bytes() + ':"' + Drupal.encodeURIComponent(s_street1)
    + '";s:16:"delivery_street2";s:' + s_street2.bytes() + ':"' + Drupal.encodeURIComponent(s_street2)
    + '";s:13:"delivery_city";s:' + s_city.bytes() + ':"' + Drupal.encodeURIComponent(s_city)
    + '";s:13:"delivery_zone";i:' + s_zone
    + ';s:20:"delivery_postal_code";s:' + s_code.bytes() +':"' + Drupal.encodeURIComponent(s_code)
    + '";s:16:"delivery_country";i:' + s_country + ';'
    + 's:18:"billing_first_name";s:' + b_f_name.bytes() + ':"' + Drupal.encodeURIComponent(b_f_name)
    + '";s:17:"billing_last_name";s:' + b_l_name.bytes() + ':"' + Drupal.encodeURIComponent(b_l_name)
    + '";s:15:"billing_street1";s:' + b_street1.bytes() + ':"' + Drupal.encodeURIComponent(b_street1)
    + '";s:15:"billing_street2";s:' + b_street2.bytes() + ':"' + Drupal.encodeURIComponent(b_street2)
    + '";s:12:"billing_city";s:' + b_city.bytes() + ':"' + Drupal.encodeURIComponent(b_city)
    + '";s:12:"billing_zone";i:' + b_zone
    + ';s:19:"billing_postal_code";s:' + b_code.bytes() +':"' + Drupal.encodeURIComponent(b_code)
    + '";s:15:"billing_country";i:' + b_country + ';'
    + line_item + '}';
  if (!!products) {
    $.ajax({
      type: "POST",
      url: Drupal.settings.basePath + "?q=taxes/calculate",
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
          set_line_item("tax_" + taxes[j].id, taxes[j].name, taxes[j].amount, Drupal.settings.ucTaxWeight + taxes[j].weight / 10, summed, false);
        }
        render_line_items();
      }
    });
  }
}

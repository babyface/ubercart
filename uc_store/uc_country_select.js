// $Id: uc_country_select.js,v 1.5.2.1 2008/01/07 20:56:46 rszrama Exp $

$(document).ready(
  function() {
    $('select[@id$=-country]').change(
      function() {
        uc_update_zone_select(this.id, '');
      }
    );
  }
);

function uc_update_zone_select(country_select, default_zone) {
  var zone_select = country_select.substr(0, country_select.length - 8) + '-zone';

  var options = { 'country_id' : $('#' + country_select).val() };

  $.post(Drupal.settings['base_path'] + 'uc_js_util/zone_select', options,
         function (contents) {
           if (contents.match('value="-1"') != null) {
             $('#' + zone_select).attr('disabled', 'disabled');
           }
           else {
             $('#' + zone_select).removeAttr('disabled');
           }
           $('#' + zone_select).empty().append(contents).val(default_zone).change();
         }
  );
}

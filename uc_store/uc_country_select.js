// $Id: uc_country_select.js,v 1.5.2.5 2008/09/17 21:38:42 rszrama Exp $

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

  $('#' + zone_select).parent().siblings('.zone-throbber').attr('style', 'background-image: url(' + Drupal.settings['base_path'] + 'misc/throbber.gif); background-repeat: no-repeat; background-position: 100% -20px;').html('&nbsp;&nbsp;&nbsp;&nbsp;');

  $.post(Drupal.settings['base_path'] + '?q=uc_js_util/zone_select', options,
         function (contents) {
           if (contents.match('value="-1"') != null) {
             $('#' + zone_select).attr('disabled', 'disabled');
           }
           else {
             $('#' + zone_select).removeAttr('disabled');
           }
           $('#' + zone_select).empty().append(contents).val(default_zone).change();
           $('#' + zone_select).parent().siblings('.zone-throbber').removeAttr('style').empty();
         }
  );
}

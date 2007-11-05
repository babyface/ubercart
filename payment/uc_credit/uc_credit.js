// $Id: uc_credit.js,v 1.3 2007/11/05 20:31:53 rszrama Exp $

$(document).ready(
  function () {
    $('#cc_details_title').show(0);
    $('#cc_details').hide(0);
  }
);

/**
 * Toggle credit card details on the order view screen.
 */
function toggle_card_details() {
  $('#cc_details').toggle();
  $('#cc_details_title').toggle();
}


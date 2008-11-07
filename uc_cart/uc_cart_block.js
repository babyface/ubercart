// $Id: uc_cart_block.js,v 1.8.2.2 2008/11/07 21:13:22 islandusurper Exp $

/**
 * Set the behavior to (un)collapse the cart block on a click
 */
Drupal.behaviors.ucCollapseBlock = function(context) {
  $('.cart-block-toggle:not(.ucCollapseBlock-processed)', context).addClass('ucCollapseBlock-processed').click(
    function() {
      cart_block_toggle();
    }
  );
}

/**
 * Collapse the shopping cart block at page load.
 */
$(document).ready(
  function() {
    if (Drupal.settings.ucCollapsedBlock == true) {
      $('#block-cart-contents').hide();
    }
  }
);

/**
 * Toggle the shopping cart block open and closed.
 */
function cart_block_toggle() {
  $('#block-cart-contents').toggle();

  isrc = $('#block-cart-title-arrow').attr('src');
  if (isrc.toLowerCase().match("up") != null) {
    $('#block-cart-title-arrow').attr('src', isrc.split('-up').join('-down'));
  }
  else {
    $('#block-cart-title-arrow').attr('src', isrc.split('-down').join('-up'));
  }
}

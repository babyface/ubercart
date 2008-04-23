// $Id: uc_cart_block.js,v 1.7.2.3 2008/04/23 19:38:07 rszrama Exp $

/**
 * Collapse the shopping cart block at page load.
 */
$(document).ready(function() {
  if (collapsed_block == true) {
    $('#block-cart-contents').hide(0);
  }
  $('.cart-block-toggle').click(function() { cart_block_toggle(); } );
});

/**
 * Toggle the shopping cart block open and closed.
 */
function cart_block_toggle() {
  $('#block-cart-contents').toggle();

  isrc = $('#block-cart-title-arrow').attr('src');
  if (isrc.toLowerCase().match("up") != null) {
    $('#block-cart-title-arrow').attr('src', uc_cart_path + '/images/bullet-arrow-down.gif');
  }
  else {
    $('#block-cart-title-arrow').attr('src', uc_cart_path + '/images/bullet-arrow-up.gif');
  }
}                                                                             

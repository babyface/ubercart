// $Id: uc_cart_block.js,v 1.7 2007/11/05 20:31:57 rszrama Exp $

/**
 * Collapse the shopping cart block at page load.
 */
$(document).ready(function(){
  if (expanded_block == false) {
    $('#block-cart-contents').hide(0);
  }
});

/**
 * Toggle the shopping cart block open and closed.
 */
function cart_toggle(uc_cart_path) {
  $('#block-cart-contents').toggle();

  isrc = $('#block-cart-title-arrow').attr('src');
  if (isrc.toLowerCase().match("up") != null) {
    $('#block-cart-title-arrow').attr('src', uc_cart_path + '/images/bullet-arrow-down.gif');
  }
  else {
    $('#block-cart-title-arrow').attr('src', uc_cart_path + '/images/bullet-arrow-up.gif');
  }
}                                                                             

<?php
// $Id: uc_product_handler_field_price.inc,v 1.1.2.1 2008/10/15 14:47:43 islandusurper Exp $

/**
 * @file
 * Views handler: Product price field.
 */

/**
 * Return a formatted price value to display in the View.
 */
class uc_product_handler_field_price extends views_handler_field {
  function render($values) {
    return uc_currency_format($values->{$this->field_alias});
  }
}

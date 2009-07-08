<?php
// $Id: uc_product_handler_field_price.inc,v 1.1.2.4 2009/07/08 12:56:50 islandusurper Exp $

/**
 * @file
 * Views handler: Product price field.
 */

/**
 * Return a formatted price value to display in the View.
 */
class uc_product_handler_field_price extends views_handler_field {
  function render($values) {
    $context = array(
      'location' => 'views-price-handler',
      'class' => array(
        'product',
        $this->field,
      ),
      'subject' => array(
        'node' => $values,
        'field' => $this->real_field,
      ),
    );
    $options = array('label' => FALSE);

    $table_alias_len = strlen($this->table_alias);
    foreach ($values as $key => $value) {
      if (substr($key, 0, $table_alias_len) == $this->table_alias) {
        $values->{substr($key, $table_alias_len + 1)} = $value;
      }
    }

    return uc_price($values->{$this->field_alias}, $context, $options);
  }
}

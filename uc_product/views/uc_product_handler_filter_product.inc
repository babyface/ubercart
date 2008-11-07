<?php
// $Id: uc_product_handler_filter_product.inc,v 1.1.2.2 2008/11/07 21:13:28 islandusurper Exp $

/**
 * @file
 * Views handler: Node filter on "product-ness".
 */

/**
 * Filter nodes based on whether they have an entry in the uc_products table.
 */
class uc_product_handler_filter_product extends views_handler_filter_boolean_operator {
  function query() {
    $types = module_invoke_all('product_types');
    $this->query->add_field('node', 'type');
    $this->query->add_where($this->options['group'], "node.type ". (empty($this->value) ? "NOT " : "") ."IN (". db_placeholders($types, 'varchar') .")", $types);
  }
}

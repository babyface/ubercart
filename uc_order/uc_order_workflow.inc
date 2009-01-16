<?php
// $Id: uc_order_workflow.inc,v 1.1.2.9 2009/01/16 22:12:38 rszrama Exp $

/**
 * @file
 * This file contains the Workflow-ng hooks and functions necessary to make the
 * order related entity, conditions, events, and actions work.
 */


/******************************************************************************
 * Workflow-ng Hooks                                                          *
 ******************************************************************************/

// Define the order entity for use in Workflow.
function uc_order_entity_info() {
  $entities['order'] = array(
    '#save' => 'uc_order_save',
  );

  return $entities;
}

// Tell Workflow about the various order events.
function uc_order_event_info() {
  $events['order_status_update'] = array(
    '#label' => t('Order status gets updated'),
    '#module' => t('Order'),
    '#arguments' => array(
      'order' => array('#entity' => 'order', '#label' => t('Order')),
      'updated_order' => array('#entity' => 'order', '#label' => t('Updated order')),
    ),
  );

  return $events;
}

// Tell Workflow about the various order conditions.
function uc_order_condition_info() {
  $order_arg = array('#entity' => 'order', '#label' => t('Order'));

  $conditions['uc_order_condition_order_status'] = array(
    '#label' => t('Check the order status'),
    '#arguments' => array('order' => $order_arg),
    '#module' => t('Order'),
  );
  $conditions['uc_order_condition_order_total'] = array(
    '#label' => t('Check the order total'),
    '#arguments' => array('order' => $order_arg),
    '#module' => t('Order'),
  );
  $conditions['uc_order_condition_delivery_postal_code'] = array(
    '#label' => t("Check an order's shipping postal code"),
    '#arguments' => array('order' => $order_arg),
    '#description' => t('Returns TRUE if the shipping postal code is in the specified area.'),
    '#module' => t('Order: Shipping address'),
  );
  $conditions['uc_order_condition_delivery_zone'] = array(
    '#label' => t("Check an order's shipping @zone", array('@zone' => uc_get_field_name('zone'))),
    '#arguments' => array('order' => $order_arg),
    '#description' => t('Returns TRUE if the shipping @zone is in the specified list.', array('@zone' => uc_get_field_name('zone'))),
    '#module' => t('Order: Shipping address'),
  );
  $conditions['uc_order_condition_delivery_country'] = array(
    '#label' => t("Check an order's shipping country"),
    '#arguments' => array('order' => $order_arg),
    '#description' => t('Returns TRUE if the shipping country is in the specified list.'),
    '#module' => t('Order: Shipping address'),
  );
  $conditions['uc_order_condition_billing_postal_code'] = array(
    '#label' => t("Check an order's billing postal code"),
    '#arguments' => array('order' => $order_arg),
    '#description' => t('Returns TRUE if the billing postal code is in the specified area.'),
    '#module' => t('Order: Billing address'),
  );
  $conditions['uc_order_condition_billing_zone'] = array(
    '#label' => t("Check an order's billing @zone", array('@zone' => uc_get_field_name('zone'))),
    '#arguments' => array('order' => $order_arg),
    '#description' => t('Returns TRUE if the billing @zone is in the specified list.', array('@zone' => uc_get_field_name('zone'))),
    '#module' => t('Order: Billing address'),
  );
  $conditions['uc_order_condition_billing_country'] = array(
    '#label' => t("Check an order's billing country"),
    '#arguments' => array('order' => $order_arg),
    '#description' => t('Returns TRUE if the billing country is in the specified list.'),
    '#module' => t('Order: Billing address'),
  );
  $conditions['uc_order_condition_has_products'] = array(
    '#label' => t("Check an order's products"),
    '#arguments' => array('order' => $order_arg),
    '#description' => t('Returns TRUE if the order has any, all, or only the products in the list.'),
    '#module' => t('Order: Product'),
  );
  $conditions['uc_order_condition_count_products'] = array(
    '#label' => t("Check an order's number of products"),
    '#arguments' => array('order' => $order_arg),
    '#description' => t('Determines if the order has the specified number of products, possibly of a certain type.'),
    '#module' => t('Order: Product'),
  );
  $conditions['uc_order_condition_products_weight'] = array(
    '#label' => t("Check an order's total weight"),
    '#arguments' => array('order' => $order_arg),
    '#description' => t('Determines if the order has the specified weight, possibly counting only a certain type of product.'),
    '#module' => t('Order: Product'),
  );
  $conditions['uc_order_condition_is_shippable'] = array(
    '#label' => t('Check if an order can be shipped'),
    '#arguments' => array('order' => $order_arg),
    '#description' => t('Returns TRUE if the order has any shippable products.'),
    '#module' => t('Order'),
  );

  return $conditions;
}

// Tell Workflow about the various order actions.
function uc_order_action_info() {
  $order_arg = array('#entity' => 'order', '#label' => t('Order'));

  $actions['uc_order_action_update_status'] = array(
    '#label' => t('Update the order status'),
    '#arguments' => array('order' => $order_arg),
    '#module' => t('Order'),
  );
  $actions['uc_order_action_add_comment'] = array(
    '#label' => t('Add a comment to the order'),
    '#arguments' => array('order' => $order_arg),
    '#module' => t('Order'),
  );

  return $actions;
}


/******************************************************************************
 * Workflow-ng Condition Callbacks and Forms                                  *
 ******************************************************************************/

// Check the current order status.
function uc_order_condition_order_status($order, $settings) {
  // Return TRUE if the order status matches.
  $status = db_result(db_query("SELECT order_status FROM {uc_orders} WHERE order_id = %d", $order->order_id));
  if ($status == $settings['order_status']) {
    return TRUE;
  }
  return FALSE;
}

function uc_order_condition_order_status_form($settings = array()) {
  foreach (uc_order_status_list('general') as $status) {
    $options[$status['id']] = $status['title'];
  }
  foreach (uc_order_status_list('specific') as $status) {
    $options[$status['id']] = $status['title'];
  }
  $form['order_status'] = array(
    '#type' => 'select',
    '#title' => t('Order status'),
    '#options' => $options,
    '#default_value' => $settings['order_status'],
  );

  return $form;
}

function uc_order_condition_order_status_submit($form_id, $form_values) {
  return array('order_status' => $form_values['order_status']);
}

// Check the current order balance.
function uc_order_condition_order_total($order, $settings) {
  switch ($settings['order_total_comparison']) {
    case 'less':
      return $order->order_total < $settings['order_total_value'];
    case 'less_equal':
      return $order->order_total <= $settings['order_total_value'];
    case 'equal':
      return $order->order_total == $settings['order_total_value'];
    case 'greater_equal':
      return $order->order_total >= $settings['order_total_value'];
    case 'greater':
      return $order->order_total > $settings['order_total_value'];
  }
}

function uc_order_condition_order_total_form($settings = array()) {
  $form['order_total_value'] = array(
    '#type' => 'textfield',
    '#title' => t('Order total value'),
    '#description' => t('Specify a value to compare the order total against.'),
    '#default_value' => $settings['order_total_value'],
    '#size' => 16,
    '#field_prefix' => variable_get('uc_sign_after_amount', FALSE) ? '' : variable_get('uc_currency_sign', '$'),
    '#field_suffix' => variable_get('uc_sign_after_amount', FALSE) ? variable_get('uc_currency_sign', '$') : '',
  );

  $options = array(
    'less' => t('Total is less than specified value.'),
    'less_equal' => t('Total is less than or equal to specified value.'),
    'equal' => t('Total is equal to specified value.'),
    'greater_equal' => t('Total is greater than or equal to specified value.'),
    'greater' => t('Total is greater than specified value.'),
  );
  $form['order_total_comparison'] = array(
    '#type' => 'radios',
    '#title' => t('Order total comparison type'),
    '#options' => $options,
    '#default_value' => isset($settings['order_total_comparison']) ? $settings['order_total_comparison'] : 'greater_equal',
  );

  return $form;
}

function uc_order_condition_order_total_submit($form_id, $form_values) {
  $array = array(
    'order_total_comparison' => $form_values['order_total_comparison'],
    'order_total_value' => $form_values['order_total_value'],
  );
  return $array;
}

// Check an order's delivery postal code.
function uc_order_condition_delivery_postal_code($order, $settings) {
  // Trim the wildcard off the pattern.
  $pattern = rtrim($settings['pattern'], '*');

  // Return TRUE if the delivery postal code begins with the pattern.
  return strpos($order->delivery_postal_code, $pattern) === 0;
}

function uc_order_condition_delivery_postal_code_form($settings = array()) {
  $form['pattern'] = array(
    '#type' => 'textfield',
    '#title' => uc_get_field_name('postal_code'),
    '#default_value' => $settings['pattern'],
    '#description' => t('Specify a postal code or postal code pattern. Use "*" as a wild card to specify a range of postal codes.<br /><b>Example:</b> In the US, 402* represents all areas from 40200 to 40299.'),
    '#size' => 15,
    '#required' => TRUE,
  );

  return $form;
}

function uc_order_condition_delivery_postal_code_submit($form_id, $form_values) {
  return array('pattern' => $form_values['pattern']);
}

// Check an order's delivery zone.
function uc_order_condition_delivery_zone($order, $settings) {
  return in_array($order->delivery_zone, $settings['zones']);
}

function uc_order_condition_delivery_zone_form($settings = array()) {
  $result = db_query("SELECT z.*, c.country_name FROM {uc_zones} AS z LEFT JOIN {uc_countries} AS c ON z.zone_country_id = c.country_id ORDER BY c.country_name, z.zone_name");
  while ($zone = db_fetch_object($result)) {
    $options[$zone->country_name][$zone->zone_id] = $zone->zone_name;
  }

  $form['zones'] = array(
    '#type' => 'select',
    '#title' => uc_get_field_name('zone'),
    '#options' => $options,
    '#default_value' => $settings['zones'],
    '#multiple' => TRUE,
    '#required' => TRUE,
  );

  return $form;
}

function uc_order_condition_delivery_zone_submit($form_id, $form_values) {
  return array('zones' => $form_values['zones']);
}

// Check an order's delivery country.
function uc_order_condition_delivery_country($order, $settings) {
  return in_array($order->delivery_country, $settings['countries']);
}

function uc_order_condition_delivery_country_form($settings = array()) {
  $form['countries'] = uc_country_select(uc_get_field_name('country'));
  $form['countries']['#default_value'] = $settings['countries'];
  $form['countries']['#multiple'] = TRUE;
  $form['countries']['#required'] = TRUE;

  return $form;
}

function uc_order_condition_delivery_country_submit($form_id, $form_values) {
  return array('countries' => $form_values['countries']);
}

// Check an order's billing postal code.
function uc_order_condition_billing_postal_code($order, $settings) {
  // Trim the wildcard off the pattern.
  $pattern = rtrim($settings['pattern'], '*');

  // Return TRUE if the delivery postal code begins with the pattern.
  return strpos($order->billing_postal_code, $pattern) === 0;
}

function uc_order_condition_billing_postal_code_form($settings = array()) {
  $form['pattern'] = array(
    '#type' => 'textfield',
    '#title' => uc_get_field_name('postal_code'),
    '#default_value' => $settings['pattern'],
    '#description' => t('Specify a postal code or postal code pattern. Use "*" as a wild card to specify a range of postal codes.<br /><b>Example:</b> In the US, 402* represents all areas from 40200 to 40299.'),
    '#size' => 15,
    '#required' => TRUE,
  );

  return $form;
}

function uc_order_condition_billing_postal_code_submit($form_id, $form_values) {
  return array('pattern' => $form_values['pattern']);
}

// Check an order's billing zone.
function uc_order_condition_billing_zone($order, $settings) {
  return in_array($order->billing_zone, $settings['zones']);
}

function uc_order_condition_billing_zone_form($settings = array()) {
  $result = db_query("SELECT z.*, c.country_name FROM {uc_zones} AS z LEFT JOIN {uc_countries} AS c ON z.zone_country_id = c.country_id ORDER BY c.country_name, z.zone_name");
  while ($zone = db_fetch_object($result)) {
    $options[$zone->country_name][$zone->zone_id] = $zone->zone_name;
  }

  $form['zones'] = array(
    '#type' => 'select',
    '#title' => uc_get_field_name('zone'),
    '#options' => $options,
    '#default_value' => $settings['zones'],
    '#multiple' => TRUE,
    '#required' => TRUE,
  );

  return $form;
}

function uc_order_condition_billing_zone_submit($form_id, $form_values) {
  return array('zones' => $form_values['zones']);
}

// Check an order's billing country.
function uc_order_condition_billing_country($order, $settings) {
  return in_array($order->billing_country, $settings['countries']);
}

function uc_order_condition_billing_country_form($settings = array()) {
  $form['countries'] = uc_country_select(uc_get_field_name('country'));
  $form['countries']['#default_value'] = $settings['countries'];
  $form['countries']['#multiple'] = TRUE;
  $form['countries']['#required'] = TRUE;

  return $form;
}

function uc_order_condition_billing_country_submit($form_id, $form_values) {
  return array('countries' => $form_values['countries']);
}

function uc_order_condition_has_products($order, $settings) {
  $products = array();
  foreach ($order->products as $product) {
    $products[] = $product->nid;
  }
  $required = array_intersect($settings['products'], $products);
  if ($settings['required']) {
    $required_check = $required == $settings['products'];
  }
  else {
    $required_check = (bool)count($required);
  }
  if ($settings['forbidden']) {
    $forbidden = array_diff($products, $settings['products']);
    $forbidden_check = (bool)count($forbidden);
  }
  else {
    $forbidden_check = false;
  }
  return $required_check && !$forbidden_check;
}

function uc_order_condition_has_products_form($settings = array('required' => 0, 'forbidden' => 0)) {
  $form['required'] = array('#type' => 'radios',
    '#title' => t('Require selected products'),
    '#options' => array(
      0 => t('Order has any of these products.'),
      1 => t('Order has all of these products.'),
    ),
    '#default_value' => $settings['required'],
  );
  $form['forbidden'] = array('#type' => 'radios',
    '#title' => t('Forbid other products'),
    '#options' => array(
      0 => t('Order may have other products.'),
      1 => t('Order has only these products.'),
    ),
    '#default_value' => $settings['forbidden'],
  );
  $options = array();
  $result = db_query("SELECT nid, model FROM {uc_products}");
  while ($product = db_fetch_object($result)) {
    $options[$product->nid] = $product->model;
  }
  $form['products'] = array('#type' => 'select',
    '#title' => t('Products'),
    '#options' => $options,
    '#default_value' => $settings['products'],
    '#multiple' => true,
  );
  return $form;
}

function uc_order_condition_has_products_submit($form_id, $form_values) {
  return array(
    'required' => $form_values['required'],
    'forbidden' => $form_values['forbidden'],
    'products' => $form_values['products'],
  );
}

function uc_order_condition_count_products($order, $settings) {
  $totals = array('all' => 0);
  $total = 0;
  foreach ($order->products as $product) {
    $totals['all'] += $product->qty;
    $totals[$product->nid] = $product->qty;
  }
  if (in_array('all', $settings['products'])) {
    $total = $totals['all'];
  }
  else {
    foreach ($settings['products'] as $product) {
      $total += $totals[$product];
    }
  }
  switch ($settings['product_count_comparison']) {
    case 'less':
      return $total < $settings['product_count_value'];
    case 'less_equal':
      return $total <= $settings['product_count_value'];
    case 'equal':
      return $total == $settings['product_count_value'];
    case 'greater_equal':
      return $total >= $settings['product_count_value'];
    case 'greater':
      return $total > $settings['product_count_value'];
  }
}

function uc_order_condition_count_products_form($settings = array()) {
  $options = array('all' => t('<All products>'));
  $result = db_query("SELECT nid, model FROM {uc_products} ORDER BY model");
  while ($product = db_fetch_object($result)) {
    $options[$product->nid] = $product->model;
  }
  $form['products'] = array('#type' => 'select',
    '#title' => t('Products'),
    '#options' => $options,
    '#default_value' => $settings['products'],
    '#description' => check_plain(t('Selecting "<All products>" will override any other selections and returns the total number of products in the order.')),
    '#multiple' => true,
  );
  $form['product_count_value'] = array(
    '#type' => 'textfield',
    '#title' => t('Product count value'),
    '#description' => t('Specify a value to compare the product count against.'),
    '#default_value' => $settings['product_count_value'],
    '#size' => 16,
  );

  $options = array(
    'less' => t('Total is less than specified value.'),
    'less_equal' => t('Total is less than or equal to specified value.'),
    'equal' => t('Total is equal to specified value.'),
    'greater_equal' => t('Total is greater than or equal to specified value.'),
    'greater' => t('Total is greater than specified value.'),
  );
  $form['product_count_comparison'] = array(
    '#type' => 'radios',
    '#title' => t('Product count comparison type'),
    '#options' => $options,
    '#default_value' => isset($settings['product_count_comparison']) ? $settings['product_count_comparison'] : 'greater_equal',
  );

  return $form;
}

function uc_order_condition_count_products_submit($form_id, $form_values) {
  return array(
    'products' => $form_values['products'],
    'product_count_value' => $form_values['product_count_value'],
    'product_count_comparison' => $form_values['product_count_comparison'],
  );
}

function uc_order_condition_products_weight($order, $settings) {
  $totals = array('all' => 0);
  $total = 0;
  foreach ($order->products as $product) {
    $unit_conversion = uc_weight_conversion($product->weight_units, $settings['weight_units']);
    $totals['all'] += $product->qty * $product->weight * $unit_conversion;
    $totals[$product->model] = $product->qty * $product->weight * $unit_conversion;
  }
  if (in_array('all', $settings['products'])) {
    $total = $totals['all'];
  }
  else {
    foreach ($settings['products'] as $product) {
      $total += $totals[$product];
    }
  }
  switch ($settings['product_weight_comparison']) {
    case 'less':
      return $total < $settings['product_weight_value'];
    case 'less_equal':
      return $total <= $settings['product_weight_value'];
    case 'equal':
      return $total == $settings['product_weight_value'];
    case 'greater_equal':
      return $total >= $settings['product_weight_value'];
    case 'greater':
      return $total > $settings['product_weight_value'];
  }
}

function uc_order_condition_products_weight_form($settings = array()) {
  $options = array('all' => t('<All products>'));
  $result = db_query("SELECT nid, model FROM {uc_products}");
  while ($product = db_fetch_object($result)) {
    $options[$product->nid] = $product->model;
  }
  $form['products'] = array('#type' => 'select',
    '#title' => t('Products'),
    '#options' => $options,
    '#default_value' => $settings['products'],
    '#description' => check_plain(t('Selecting "<All products>" will override any other selections and returns the total number of products in the order.')),
    '#multiple' => true,
  );
  $form['weight_units'] = array('#type' => 'select',
    '#title' => t('Unit of measurement'),
    '#default_value' => $settings['weight_units'] ? $settings['weight_units'] : variable_get('uc_weight_unit', 'lb'),
    '#options' => array(
      'lb' => t('Pounds'),
      'kg' => t('Kilograms'),
      'oz' => t('Ounces'),
      'g' => t('Grams'),
    ),
  );
  $form['product_weight_value'] = array(
    '#type' => 'textfield',
    '#title' => t('Product weight value'),
    '#description' => t('Specify a value to compare the product weight against.'),
    '#default_value' => $settings['product_weight_value'],
    '#size' => 16,
  );

  $options = array(
    'less' => t('Total is less than specified value.'),
    'less_equal' => t('Total is less than or equal to specified value.'),
    'equal' => t('Total is equal to specified value.'),
    'greater_equal' => t('Total is greater than or equal to specified value.'),
    'greater' => t('Total is greater than specified value.'),
  );
  $form['product_weight_comparison'] = array(
    '#type' => 'radios',
    '#title' => t('Product count comparison type'),
    '#options' => $options,
    '#default_value' => isset($settings['product_weight_comparison']) ? $settings['product_weight_comparison'] : 'greater_equal',
  );

  return $form;
}

function uc_order_condition_products_weight_submit($form_id, $form_values) {
  return array(
    'products' => $form_values['products'],
    'weight_units' => $form_values['weight_units'],
    'product_weight_value' => $form_values['product_weight_value'],
    'product_weight_comparison' => $form_values['product_weight_comparison'],
  );
}

function uc_order_condition_is_shippable($order) {
  return uc_order_is_shippable($order);
}

/******************************************************************************
 * Workflow-ng Action Callbacks and Forms                                     *
 ******************************************************************************/

// Update an order's status.
function uc_order_action_update_status($order, $settings) {
  uc_order_update_status($order->order_id, $settings['order_status']);
}

function uc_order_action_update_status_form($settings = array()) {
  foreach (uc_order_status_list('general') as $status) {
    $options[$status['id']] = $status['title'];
  }
  foreach (uc_order_status_list('specific') as $status) {
    $options[$status['id']] = $status['title'];
  }
  $form['order_status'] = array(
    '#type' => 'select',
    '#title' => t('Order status'),
    '#options' => $options,
    '#default_value' => $settings['order_status'],
  );

  return $form;
}

function uc_order_action_update_status_submit($form_id, $form_values) {
  return array('order_status' => $form_values['order_status']);
}

// Add a comment to an order.
function uc_order_action_add_comment($order, $settings) {
  uc_order_comment_save($order->order_id, 0,
    token_replace_multiple($settings['comment'], array('global' => NULL, 'order' => $order)),
    $settings['comment_type'] == 'admin' ? 'admin' : 'order',
    $order->order_status, $settings['comment_type'] == 'notified');
}

function uc_order_action_add_comment_form($settings = array()) {
  $form['comment_type'] = array(
    '#type' => 'radios',
    '#title' => t('Select an order comment type'),
    '#options' => array(
      'admin' => t('Enter this as an admin comment.'),
      'order' => t('Enter this as a customer order comment.'),
      'notified' => t('Enter this as a customer order comment with a notified icon.'),
    ),
    '#default_value' => $settings['comment_type'],
  );
  $form['comment'] = array(
    '#type' => 'textarea',
    '#title' => t('Comment'),
    '#description' => t('Enter the comment message. Uses <a href="!url">order and global tokens</a>.', array('!url' => url('admin/store/help/tokens'))),
    '#default_value' => $settings['comment'],
  );

  return $form;
}

function uc_order_action_add_comment_submit($form_id, $form_values) {
  return array('comment_type' => $form_values['comment_type'], 'comment' => $form_values['comment']);
}


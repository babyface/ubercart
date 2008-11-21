<?php
// $Id: uc_cart.ca.inc,v 1.1.2.7 2008/11/21 21:17:27 islandusurper Exp $

/**
 * @file
 * This file contains the Conditional Actions hooks and functions necessary to make the
 * cart related entity, conditions, events, and actions work.
 */


/******************************************************************************
 * Conditional Actions Hooks                                                  *
 ******************************************************************************/

/**
 * Implementation of hook_ca_condition().
 */
function uc_cart_ca_condition() {
  $conditions['uc_cart_condition_product_class'] = array(
    '#title' => t('Order has a product of a particular class'),
    '#category' => t('Order: Product'),
    '#callback' => 'uc_cart_condition_product_class',
    '#arguments' => array(
      'order' => array(
        '#entity' => 'uc_order',
        '#title' => t('Order'),
      ),
    ),
  );

  return $conditions;
}

/**
 * Implementation of hook_ca_trigger().
 */
function uc_cart_ca_trigger() {
  $triggers['uc_checkout_complete'] = array(
    '#title' => t('Customer completes checkout'),
    '#category' => t('Cart'),
    '#arguments' => array(
      'order' => array(
        '#entity' => 'uc_order',
        '#title' => t('Order'),
      ),
      'account' => array(
        '#entity' => 'user',
        '#title' => t('Customer user account'),
      )
    ),
  );

  return $triggers;
}

/**
 * Implementation of hook_ca_predicate().
 */
function uc_cart_ca_predicate() {
  $predicates['uc_checkout_email_notification'] = array(
    '#title' => t('E-mail checkout notifications'),
    '#description' => t('Send an e-mail about the new order.'),
    '#class' => 'order',
    '#status' => 1,
    '#trigger' => 'uc_checkout_complete',
    '#actions' => array(
      array(
        '#name' => 'uc_order_email_invoice',
        '#title' => t('Send an e-mail to the customer'),
        '#argument_map' => array(
          'order' => 'order',
        ),
        '#settings' => array(
          'from' => uc_store_email_from(),
          'addresses' => '[order-email]',
          'subject' => t('Your Order at [store-name]'),
          'template' => 'customer',
          'view' => 'checkout-mail',
        ),
      ),
      array(
        '#name' => 'uc_order_email_invoice',
        '#title' => t('Send an e-mail to the administrator(s)'),
        '#argument_map' => array(
          'order' => 'order',
        ),
        '#settings' => array(
          'from' => uc_store_email_from(),
          'addresses' => variable_get('uc_store_email', ''),
          'subject' => t('New Order at [store-name]'),
          'template' => 'admin',
          'view' => 'admin-mail',
        ),
      ),
    ),
  );

  return $predicates;
}

/******************************************************************************
 * Condition Callbacks and Forms                                              *
 ******************************************************************************/

function uc_cart_condition_product_class($order, $settings) {
  $result = FALSE;
  $types = array();
  foreach ($order->products as $product) {
    // Ignore "blank line" custom products.
    if ($product->nid) {
      // Cache product types to avoid extra queries.
      if (!isset($types[$product->nid])) {
        if (isset($product->type)) {
          $types[$product->nid] = $product->type;
        }
        else {
          $types[$product->nid] = db_result(db_query("SELECT type FROM {node} WHERE nid = %d", $product->nid));
        }
      }
      if ($types[$product->nid] == $settings['class']) {
        $result = TRUE;
        break;
      }
    }
  }

  return $result;
}

function uc_cart_condition_product_class_form($form_state, $settings = array()) {
  $options = array();
  $types = module_invoke_all('node_info');
  $product_types = module_invoke_all('product_types');
  foreach ($product_types as $id) {
    $options[$id] = $types[$id]['name'];
  }
  $form['class'] = array('#type' => 'select',
    '#title' => t('Product class'),
    '#options' => $options,
    '#default_value' => $settings['class'],
  );

  return $form;
}

function uc_cart_condition_product_class_submit($form, &$form_state) {
  return array('class' => $form_state['values']['class']);
}


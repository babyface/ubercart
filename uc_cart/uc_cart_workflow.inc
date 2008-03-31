<?php
// $Id: uc_cart_workflow.inc,v 1.1.2.3 2008/03/31 20:45:11 rszrama Exp $

/**
 * @file
 * This file contains the Workflow-ng hooks and functions necessary to make the
 * cart related entity, conditions, events, and actions work.
 */


/******************************************************************************
 * Workflow-ng Hooks                                                          *
 ******************************************************************************/

// Tell Workflow about the various cart conditions.
function uc_cart_condition_info() {
  $order_arg = array('#entity' => 'order', '#label' => t('Order'));

  $conditions['uc_cart_condition_product_class'] = array(
    '#label' => t('Order has a product of a particular class'),
    '#arguments' => array('order' => $order_arg),
    '#module' => t('Order: Product'),
  );

  return $conditions;
}

// Tell Workflow about the various cart events.
function uc_cart_event_info() {
  $order_arg = array('#entity' => 'order', '#label' => t('Order'));

  $events['checkout_complete'] = array(
    '#label' => t('Customer completes checkout'),
    '#module' => t('Cart'),
    '#arguments' => array(
      'order' => $order_arg,
      'account' => array('#entity' => 'user', '#label' => t('customer user account'))
    ),
  );

  return $events;
}


/******************************************************************************
 * Workflow-ng Condition Callbacks and Forms                                  *
 ******************************************************************************/

function uc_cart_condition_product_class($order, $settings) {
  $result = FALSE;
  foreach ($order->products as $product) {
    if ($product->type == $settings['class']) {
      $result = TRUE;
      break;
    }
  }

  return $result;
}

function uc_cart_condition_product_class_form($settings = array()) {
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

function uc_cart_condition_product_class_submit($form_id, $form_values) {
  return array('class' => $form_values['class']);
}


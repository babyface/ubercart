<?php
// $Id: uc_payment_workflow.inc,v 1.1.2.2 2008/03/31 20:45:09 rszrama Exp $

/**
 * @file
 * This file contains the Workflow-ng hooks and functions necessary to make the
 * order related entity, conditions, events, and actions work.
 */


/******************************************************************************
 * Workflow-ng Hooks                                                          *
 ******************************************************************************/

// Tell Workflow about default configurations.
function uc_payment_configuration() {
  $configurations['uc_payment_received'] = array(
    '#type' => 'configuration',
    '#label' => t('Update order status on full payment'),
    '#event' => 'payment_entered',
    '#module' => 'uc_payment',
    '#active' => FALSE,
  );
  $configurations['uc_payment_received'][] = array(
    '#type' => 'condition',
    '#name' => 'uc_payment_condition_balance',
    '#argument map' => array(
      'order' => 'order',
    ),
    '#settings' => array(
      'balance_comparison' => 'less_equal',
    ),
  );
  $configurations['uc_payment_received'][] = array(
    '#type' => 'action',
    '#name' => 'uc_order_action_update_status',
    '#argument map' => array(
      'order' => 'order',
    ),
    '#settings' => array(
      'order_status' => 'payment_received',
    ),
  );

  return $configurations;
}

// Tell Workflow about the various payment events.
function uc_payment_event_info() {
  $events['payment_entered'] = array(
    '#label' => t('A payment gets entered for an order'),
    '#module' => t('Payment'),
    '#arguments' => array(
      'order' => array('#entity' => 'order', '#label' => t('Order')),
    ),
  );

  return $events;
}

// Tell Workflow about the various payment conditions.
function uc_payment_condition_info() {
  $order_arg = array('#entity' => 'order', '#label' => t('Order'));

  $conditions['uc_payment_condition_balance'] = array(
    '#label' => t('Check the order balance'),
    '#arguments' => array('order' => $order_arg),
    '#module' => t('Payment'),
  );
  $conditions['uc_payment_condition_method'] = array(
    '#label' => t('Check the order payment method'),
    '#arguments' => array('order' => $order_arg),
    '#module' => t('Payment'),
  );

  return $conditions;
}


/******************************************************************************
 * Workflow-ng Condition Callbacks and Forms                                  *
 ******************************************************************************/

// Check the current order balance.
function uc_payment_condition_balance($order, $settings) {
  $balance = uc_payment_balance($order);

  switch ($settings['balance_comparison']) {
    case 'less':
      return $balance < 0;
    case 'less_equal':
      return $balance <= .01;
    case 'equal':
      return $balance < .01 and $balance > -.01;
    case 'greater':
      return $balance >= .01;
  }
}

function uc_payment_condition_balance_form($settings = array()) {
  $zero = array('!zero' => uc_currency_format(0));
  $options = array(
    'less' => t('Balance is less than !zero.', $zero),
    'less_equal' => t('Balance is less than or equal to !zero.', $zero),
    'equal' => t('Balance is equal to !zero.', $zero),
    'greater' => t('Balance is greater than !zero.', $zero),
  );

  $form['balance_comparison'] = array(
    '#type' => 'radios',
    '#title' => t('Balance comparison type'),
    '#options' => $options,
    '#default_value' => isset($settings['balance_comparison']) ? $settings['balance_comparison'] : 'equal',
  );

  return $form;
}

function uc_payment_condition_balance_submit($form_id, $form_values) {
  return array('balance_comparison' => $form_values['balance_comparison']);
}

// Check the order payment method.
function uc_payment_condition_method($order, $settings) {
  return $order->payment_method == $settings['payment_method'];
}

function uc_payment_condition_method_form($settings = array()) {
  foreach (_payment_method_list() as $method) {
    $options[$method['id']] = $method['title'];
  }

  $form['payment_method'] = array(
    '#type' => 'radios',
    '#title' => t('Payment method'),
    '#options' => $options,
    '#default_value' => $settings['payment_method'],
  );

  return $form;
}

function uc_payment_condition_method_submit($form_id, $form_values) {
  return array('payment_method' => $form_values['payment_method']);
}

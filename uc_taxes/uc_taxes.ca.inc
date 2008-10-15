<?php
// $Id: uc_taxes.ca.inc,v 1.1.2.4 2008/10/15 14:47:36 islandusurper Exp $

/**
 * @file
 * This file contains the Conditional Action hooks and functions necessary to
 * make the tax related entity, conditions, events, and actions work.
 */

/******************************************************************************
 * Conditional Action Hooks                                                   *
 ******************************************************************************/

/**
 * Implementation of hook_ca_entity().
 */
function uc_taxes_ca_entity() {
  $entities['tax'] = array(
    '#title' => t('Tax rule'),
    '#type' => 'object',
  );
  return $entities;
}

/**
 * Implementation of hook_ca_trigger().
 *
 * Register an event for each tax rule in {uc_taxes}.
 */
function uc_taxes_ca_trigger() {
  $triggers = array();

  $triggers['calculate_taxes'] = array(
    '#title' => t('Calculate taxes'),
    '#category' => 'uc_taxes',
    '#arguments' => array(
      'order' => array('#entity' => 'uc_order', '#title' => t('Order')),
      'tax' => array('#entity' => 'tax', '#title' => t('Tax rule')),
      'user' => array('#entity' => 'user', '#title' => t('User account')),
    ),
  );

  return $triggers;
}

/**
 * Implementation of hook_action_info().
 */
function uc_taxes_ca_action() {
  $actions = array();

  $taxes = uc_taxes_get_rates();
  foreach ($taxes as $tax) {
    $actions['uc_taxes_action_apply_tax_'. $tax->id] = array(
      '#title' => t('Charge a tax'),
      '#category' => t('Taxes'),
      '#callback' => 'uc_taxes_action_apply_tax',
      '#arguments' => array(
        'order' => array('#entity' => 'uc_order', '#title' => t('Order')),
        'tax' => array('#entity' => 'tax', '#title' => t('Tax')),
      ),
    );
  }

  return $actions;
}

/**
 * Workflow-ng action callback to calculate a tax.
 *
 * @param $order
 *   The order object being considered.
 * @param $tax
 *   The tax rule calculating the amount.
 * @return
 *   The line item array representing the amount of tax.
 */
function uc_taxes_action_apply_tax($order, $tax) {
  $amount = 0;
  if (is_array($order->products)) {
    foreach ($order->products as $item) {
      $node = node_load($item->nid);
      if (in_array($node->type, $tax->taxed_product_types)) {
        $amount += $item->price * $item->qty * $tax->rate;
      }
    }
  }
  $taxed_line_items = $tax->taxed_line_items;
  if (is_array($order->line_items) && is_array($taxed_line_items)) {
    foreach ($order->line_items as $key => $line_item) {
      if ($line_item['type'] == 'tax' && $line_item['title'] == $tax->name) {
        // Don't tax yourself.
        continue;
      }
      if (in_array($line_item['type'], $taxed_line_items)) {
        $amount += $line_item['amount'] * $tax->rate;
      }
    }
  }
  if (isset($taxed_line_items['tax'])) {
    foreach ($_SESSION['taxes'] as $other_tax) {
      $amount += $other_tax['amount'] * $tax->rate;
    }
  }
  if ($amount) {
    $line_item = (object)array('id' => $tax->id, 'name' => $tax->name, 'amount' => $amount, 'weight' => $tax->weight);
    return $line_item;
  }
}

/**
 * Implementation of hook_ca_predicate().
 *
 * Create a predicate for each event corresponding to a tax rule.
 */
function uc_taxes_ca_predicate() {
  $predicates = array();

  $taxes = uc_taxes_get_rates();
  foreach ($taxes as $tax) {
    $predicates['uc_taxes_'. $tax->id] = array(
      '#title' => $tax->name,
      '#class' => 'uc_taxes',
      '#trigger' => 'calculate_taxes',
      '#status' => 1,
      '#actions' => array(
        array(
          '#name' => 'uc_taxes_action_apply_tax_'. $tax->id,
          '#title' => t('Apply @tax', array('@tax' => $tax->name)),
          '#argument_map' => array(
            'order' => 'uc_order',
            'tax' => 'tax',
          ),
        ),
      ),
    );
  }

  return $predicates;
}

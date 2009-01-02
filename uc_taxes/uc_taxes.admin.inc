<?php
// $Id: uc_taxes.admin.inc,v 1.1.2.4 2009/01/02 20:18:39 islandusurper Exp $

/**
 * @file
 * Taxes administration menu items.
 */

/**
 * Display a list of tax rates.
 */
function uc_taxes_admin_settings() {
  $header = array(t('Name'), t('Rate'), t('Taxed product types'), t('Taxed line items'), t('Weight'), array('data' => t('Operations'), 'colspan' => 4));
  $rows = array();
  $result = db_query("SELECT * FROM {uc_taxes} ORDER BY weight");
  while ($rule = db_fetch_object($result)) {
    $rule->taxed_product_types = (array)unserialize($rule->taxed_product_types);
    $rule->taxed_line_items = (array)unserialize($rule->taxed_line_items);
    $rows[] = array($rule->name, $rule->rate * 100 .'%', implode(', ', $rule->taxed_product_types), implode(', ', $rule->taxed_line_items), $rule->weight, l(t('edit'), 'admin/store/settings/taxes/edit/'. $rule->id), l(t('conditions'), CA_UI_PATH .'/uc_taxes_'. $rule->id .'/edit/conditions'), l(t('copy'), 'admin/store/settings/taxes/copy/'. $rule->id), l(t('delete'), 'admin/store/settings/taxes/delete/'. $rule->id));
  }
  $output = theme('table', $header, $rows);
  $output .= l(t('Make a new tax rule.'), 'admin/store/settings/taxes/edit');
  return $output;
}

/**
 * Create or edit a tax rule.
 *
 * @ingroup forms
 * @see uc_taxes_form_submit()
 */
function uc_taxes_form($form_state, $tax_id = 0) {
  $form = array();

  if ($tax_id != 0) {
    $form_state['values'] = db_fetch_array(db_query("SELECT * FROM {uc_taxes} WHERE id = %d", $tax_id));
    $form_state['values']['taxed_product_types'] = array_values(unserialize($form_state['values']['taxed_product_types']));
    $form_state['values']['taxed_line_items'] = array_values(unserialize($form_state['values']['taxed_line_items']));
  }

  $form['id'] = array('#type' => 'hidden', '#value' => $form_state['values']['id']);

  $form['name'] = array('#type' => 'textfield',
    '#title' => t('Name'),
    '#default_value' => isset($form_state['values']['name']) ? $form_state['values']['name'] : '',
    '#required' => TRUE,
  );

  $form['rate'] = array('#type' => 'textfield',
    '#title' => t('Rate'),
    '#description' => t('The tax rate as a percent or decimal. Examples: 6%, .06'),
    '#default_value' => isset($form_state['values']['rate']) ? $form_state['values']['rate'] : '',
    '#size' => 15,
    '#required' => TRUE,
  );

  $form['taxed_product_types'] = array('#type' => 'checkboxes',
    '#title' => t('Taxed product types'),
    '#multiple' => TRUE,
    '#options' => uc_product_type_names(),
    '#default_value' => isset($form_state['values']['taxed_product_types']) ? $form_state['values']['taxed_product_types'] : array(),
    '#description' => t('Apply taxes to specific product types.'),
  );

  $options = array();

  foreach (_line_item_list() as $line_item) {
    if (!in_array($line_item['id'], array('subtotal', 'tax_subtotal', 'total'))) {
      $options[$line_item['id']] = $line_item['title'];
    }
  }

  $form['taxed_line_items'] = array('#type' => 'checkboxes',
    '#title' => t('Taxed line items'),
    '#multiple' => TRUE,
    '#options' => $options,
    '#default_value' => isset($form_state['values']['taxed_line_items']) ? $form_state['values']['taxed_line_items'] : array(),
    '#description' => t('Adds the checked line item types to the total before applying this tax.'),
  );

  $form['weight'] = array('#type' => 'weight',
    '#title' => t('Weight'),
    '#default_value' => isset($form_state['values']['weight']) ? $form_state['values']['weight'] : 0,
    '#description' => t('Lighter taxes are applied to an order first. This value is unimportant if there are no taxes on tax line items.'),
  );

  $form['submit'] = array('#type' => 'submit', '#value' => t('Submit'));

  return $form;
}

/**
 * Submit handler for uc_taxes_form().
 */
function uc_taxes_form_submit($form, &$form_state) {
  if ($form_state['values']['op'] == t('Submit')) {
    if (strpos($form_state['values']['rate'], '%')) {
      $rate = floatval($form_state['values']['rate']) / 100;
    }
    else {
      $rate = floatval($form_state['values']['rate']);
    }
    if (($form_state['values']['id']) != '') {
      db_query("UPDATE {uc_taxes} SET name = '%s', rate = %f, taxed_product_types = '%s', taxed_line_items = '%s', weight = %d WHERE id = %d",
        $form_state['values']['name'], $rate, serialize(array_filter($form_state['values']['taxed_product_types'])), serialize(array_filter($form_state['values']['taxed_line_items'])), $form_state['values']['weight'], $form_state['values']['id']
      );
    }
    else {
      db_query("INSERT INTO {uc_taxes} (name, rate, taxed_product_types, taxed_line_items, weight) VALUES ('%s', %f, '%s', '%s', %d)",
        $form_state['values']['name'], $rate, serialize(array_filter($form_state['values']['taxed_product_types'])), serialize(array_filter($form_state['values']['taxed_line_items'])), $form_state['values']['weight']
      );
    }

    drupal_set_message(t('%name settings have been saved.', array('%name' => $form_state['values']['name'])));
  }
  $form_state['redirect'] = 'admin/store/settings/taxes';
}

/**
 * Copy a tax rule.
 */
function uc_taxes_copy($tax_id) {
  $rule = db_fetch_array(db_query("SELECT * FROM {uc_taxes} WHERE id = %d", $tax_id));

  db_query("INSERT INTO {uc_taxes} (name, rate, taxed_product_types, taxed_line_items, weight) VALUES ('%s', %f, '%s', '%s', %d)",
    t('Copy of @rule', array('@rule' => $rule['name'])), $rule['rate'], $rule['taxed_product_types'], $rule['taxed_line_items'], $rule['weight']
  );

  drupal_goto('admin/store/settings/taxes');
}

/**
 * Delete a tax rule.
 *
 * @ingroup forms
 * @see uc_taxes_delete_submit()
 */
function uc_taxes_delete($form_state, $tax_id = 0) {
  //drupal_set_message($tax_id);
  if ($tax_id != 0) {
    $rule = db_fetch_object(db_query("SELECT * FROM {uc_taxes} WHERE id = %d", $tax_id));
    $form = array(
      'tax_id' => array('#type' => 'value', '#value' => $tax_id),
    );
    $output = confirm_form($form, t('Delete @rule?', array('@rule' => $rule->name)), 'admin/store/settings/taxes', '', t('Continue'), t('Cancel'));
    return $output;
  }
  else {
    drupal_goto('admin/store/settings/taxes');
  }
}

/**
 * Submit handler for uc_taxes_delete().
 */
function uc_taxes_delete_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    db_query("DELETE FROM {uc_taxes} WHERE id = %d", $form_state['values']['tax_id']);
    ca_delete_predicate('uc_taxes_'. $form_state['values']['tax_id']);
  }

  drupal_set_message(t('Tax rule deleted.'));

  $form_state['redirect'] = 'admin/store/settings/taxes';
}


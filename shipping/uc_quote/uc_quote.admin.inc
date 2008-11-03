<?php
// $Id: uc_quote.admin.inc,v 1.1.2.2 2008/10/24 21:11:32 islandusurper Exp $

/**
 * @file
 * Shipping quotes administration menu items.
 *
 * Coded by Lyle Mantooth
 */

/**
 * Display a summary of the shipping quote settings.
 */
function uc_quote_overview() {
  // Add some Ubercart specific JS for modifying summaries.
  drupal_add_js(drupal_get_path('module', 'uc_store') .'/includes/summaries.js');
  drupal_add_js(array('editIconPath' => base_path() . drupal_get_path('module', 'uc_store') .'/images/order_edit.gif'), 'setting');

  $summaries = array();

  // Load the form summaries for pages beneath this path.
  $summaries = array_merge($summaries, summarize_child_form_pages('admin/store/settings/quotes/edit', FALSE, TRUE));
  $summaries = array_merge($summaries, summarize_child_form_pages('admin/store/settings/quotes/methods', FALSE, TRUE));

  // Theme it all up in a summaries overview.
  return theme('summary_overview', $summaries);
}

/**
 * Default shipping settings.
 *
 * Sets the default shipping location of the store. Allows the user to
 * determine which quotin methods are enabled and which take precedence over
 * the others. Also sets the default quote and shipping types of all products
 * in the store. Individual products may be configured differently.
 *
 * @ingroup forms
 * @see uc_quote_admin_settings_submit
 */
function uc_quote_admin_settings() {
  $address = variable_get('uc_quote_store_default_address', new stdClass());
  $form = array();

  $form['uc_quote_log_errors'] = array(
    '#type' => 'checkbox',
    '#title' => t('Log errors during checkout to watchdog'),
    '#summary callback' => 'summarize_checkbox',
    '#summary arguments' => array(
      t('Quote errors are submitted to watchdog.'),
      t('Quote errors are not submitted to watchdog.'),
    ),
    '#default_value' => variable_get('uc_quote_log_errors', FALSE),
  );
  $form['uc_quote_display_debug'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display debug information to administrators.'),
    '#summary callback' => 'summarize_checkbox',
    '#summary arguments' => array(
      t('Debugging information is displayed to administrators when quotes are generated.'),
      t('Debugging information is not displayed to administrators when quotes are generated.'),
    ),
    '#default_value' => variable_get('uc_quote_display_debug', FALSE),
  );
  $form['uc_quote_require_quote'] = array(
    '#type' => 'checkbox',
    '#title' => t('Prevent the customer from completing an order if a shipping quote is not selected.'),
    '#summary callback' => 'summarize_checkbox',
    '#summary arguments' => array(
      t('Customers cannot complete checkout without selecting a shipping quote.'),
      t('Customers can still checkout without selecting a shipping quote.'),
    ),
    '#default_value' => variable_get('uc_quote_require_quote', TRUE),
  );
  $form['uc_quote_err_msg'] = array(
    '#type' => 'fieldset',
    '#title' => t('Shipping quote error message'),
    '#summary callback' => 'summarize_form',
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['uc_quote_err_msg']['text'] = array(
    '#type' => 'textarea',
    '#title' => t('Message text'),
    '#default_value' => variable_get('uc_quote_err_msg', t("There were problems getting a shipping quote. Please verify the delivery and product information and try again.\nIf this does not resolve the issue, please call in to complete your order.")),
  );
  $form['uc_quote_err_msg']['format'] = filter_form(variable_get('uc_quote_msg_format', FILTER_DEFAULT_FORMAT));
  $form['uc_quote_err_msg']['format']['#summary callback'] = 'summarize_null';

  $form['default_address'] = array(
    '#type' => 'fieldset',
    '#title' => t('Default pickup address'),
    '#description' => t("When delivering products to customers, the original location of the product must be known in order to accurately quote the shipping cost and set up a delivery. This form provides the default location for products across the entire store. If a product's individual pickup address is blank, &Uuml;bercart looks for the manufacturer's. If that is also blank, it uses the store's default pickup address."),
    '#summary' => t('Default pickup address is: <br />!address', array('!address' => uc_address_format(
      $address->first_name,
      $address->last_name,
      $address->company,
      $address->street1,
      $address->street2,
      $address->city,
      $address->zone,
      $address->postal_code,
      $address->country
    ))),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['default_address']['first_name'] = uc_textfield(uc_get_field_name('first_name'), $address->first_name, FALSE);
  $form['default_address']['last_name'] = uc_textfield(uc_get_field_name('last_name'), $address->last_name, FALSE);
  $form['default_address']['company'] = uc_textfield(uc_get_field_name('company'), $address->company, FALSE);
  $form['default_address']['phone'] = uc_textfield(uc_get_field_name('phone'), $address->phone, FALSE, NULL, 32, 16);
  $form['default_address']['street1'] = uc_textfield(uc_get_field_name('street1'), $address->street1, FALSE, NULL, 64);
  $form['default_address']['street2'] = uc_textfield(uc_get_field_name('street2'), $address->street2, FALSE, NULL, 64);
  $form['default_address']['city'] = uc_textfield(uc_get_field_name('city'), $address->city, FALSE);
  if (isset($_POST['country'])) {
    $country = $_POST['country'];
  }
  else {
    $country = $address->country;
  }
  $form['default_address']['country'] = uc_country_select(uc_get_field_name('country'), $address->country);
  $form['default_address']['zone'] = uc_zone_select(uc_get_field_name('zone'), $address->zone, NULL, $country);
  $form['default_address']['postal_code'] = uc_textfield(uc_get_field_name('postal_code'), $address->postal_code, FALSE, NULL, 10, 10);

  $form['buttons']['submit'] = array('#type' => 'submit', '#value' => t('Save configuration') );
  return $form;
}

/**
 * Submit function for uc_quote_admin_settings() in lieu of system_settings_form().
 */
function uc_quote_admin_settings_submit($form, &$form_state) {
  $address = new stdClass();
  $address->first_name = $form_state['values']['first_name'];
  $address->last_name = $form_state['values']['last_name'];
  $address->company = $form_state['values']['company'];
  $address->phone = $form_state['values']['phone'];
  $address->street1 = $form_state['values']['street1'];
  $address->street2 = $form_state['values']['street2'];
  $address->city = $form_state['values']['city'];
  $address->zone = $form_state['values']['zone'];
  $address->postal_code = $form_state['values']['postal_code'];
  $address->country = $form_state['values']['country'];

  variable_set('uc_quote_store_default_address', $address);
  variable_set('uc_quote_log_errors', $form_state['values']['uc_quote_log_errors']);
  variable_set('uc_quote_display_debug', $form_state['values']['uc_quote_display_debug']);
  variable_set('uc_quote_require_quote', $form_state['values']['uc_quote_require_quote']);
  variable_set('uc_quote_err_msg', $form_state['values']['text']);
  variable_set('uc_quote_msg_format', $form_state['values']['format']);

  drupal_set_message(t('The configuration options have been saved.'));
}

/**
 * Settings for the shipping quote methods.
 *
 * Enable and reorder shipping quote methods. Set the default shipping type.
 *
 * @ingroup forms
 * @see theme_uc_quote_method_settings
 * @see uc_quote_method_settings_validate
 * @see uc_quote_method_settings_submit
 */
function uc_quote_method_settings() {
  $form = array();

  $form['methods'] = array(
    '#tree' => TRUE,
    '#summary callback' => 'summarize_form',
  );

  $enabled = variable_get('uc_quote_enabled', array());
  $weight = variable_get('uc_quote_method_weight', array());
  $methods = uc_quote_shipping_method_options();
  if (is_array($methods)) {
    foreach ($methods as $id => $title) {

      $form['methods'][$id]['#summary callback'] = 'summarize_form';
      $form['methods'][$id]['uc_quote_enabled'] = array('#type' => 'checkbox',
        '#default_value' => $enabled[$id],
        '#summary callback' => 'summarize_checkbox',
        '#summary arguments' => array(
          l(t('@method is enabled.', array('@method' => $title)), 'admin/store/settings/quotes/methods/'. $id),
          '',
        ),
      );
      $form['methods'][$id]['uc_quote_method_weight'] = array('#type' => 'weight',
        '#delta' => 5,
        '#default_value' => isset($weight[$id]) ? $weight[$id] : 0,
      );
    }
  }

  $shipping_types = uc_quote_shipping_type_options();
  if (is_array($shipping_types)) {
    $form['uc_quote_type_weight'] = array('#type' => 'fieldset',
      '#title' => t('Type ordering'),
      '#description' => t('Determines which shipping methods are quoted at checkout when products of different shipping types are ordered. Lighter weights are chosen first.'),
      '#collapsible' => TRUE,
      '#summary callback' => 'summarize_null',
      '#tree' => TRUE,
    );
    $weight = variable_get('uc_quote_type_weight', array());
    $shipping_methods = module_invoke_all('shipping_method');
    $method_types = array();
    foreach ($shipping_methods as $method) {
      $method_types[$method['quote']['type']][] = $method['title'];
    }
    if (is_array($method_types['order'])) {
      $form['uc_quote_type_weight']['#description'] .= t('<br />The %list methods are compatible with any shipping type.', array('%list' => implode(', ', $method_types['order'])));
    }
    foreach ($shipping_types as $id => $title) {
      $form['uc_quote_type_weight'][$id] = array('#type' => 'weight',
        '#title' => $title . (is_array($method_types[$id]) ? ' ('. implode(', ', $method_types[$id]) .')' : ''),
        '#delta' => 5,
        '#default_value' => isset($weight[$id]) ? $weight[$id] : 0,
      );
    }
  }
  $form['uc_store_shipping_type'] = array('#type' => 'select',
    '#title' => t('Default order fulfillment type for products'),
    '#options' => $shipping_types,
    '#default_value' => variable_get('uc_store_shipping_type', 'small_package'),
  );

  $form['buttons']['submit'] = array('#type' => 'submit', '#value' => t('Save configuration') );

  return $form;
}

/**
 * Display a formatted list of shipping quote methods and form elements.
 *
 * @ingroup themeable
 * @see uc_quote_method_settings
 */
function theme_uc_quote_method_settings($form) {
  $methods = uc_quote_shipping_method_options();
  $output = '';

  $header = array(t('Enable'), t('Shipping method'), t('Weight'));
  $rows = array();
  foreach (element_children($form['methods']) as $method) {
    $row = array();
    $row[] = drupal_render($form['methods'][$method]['uc_quote_enabled']);
    $row[] = $methods[$method];
    $row[] = drupal_render($form['methods'][$method]['uc_quote_method_weight']);
    $rows[] = $row;
  }
  $output .= theme('table', $header, $rows);

  $output .= drupal_render($form);
  return $output;
}

/**
 * Validation handler for uc_quote_method_settings().
 *
 * Must enable at least one shipping method.
 */
function uc_quote_method_settings_validate($form, &$form_state) {
  $none_enabled = TRUE;
  if (is_array($form_state['values']['methods'])) {
    foreach ($form_state['values']['methods'] as $method) {
      if ($method['uc_quote_enabled']) {
        $none_enabled = FALSE;
      }
    }
  }
  if ($none_enabled) {
    form_set_error('uc_quote_enabled', t('At least one shipping quote method must be enabled.'));
  }
}

/**
 * Submit handler for uc_quote_method_settings().
 */
function uc_quote_method_settings_submit($form, &$form_state) {
  $enabled = array();
  $method_weight = array();
  foreach ($form_state['values']['methods'] as $id => $method) {
    $enabled[$id] = $method['uc_quote_enabled'];
    $method_weight[$id] = $method['uc_quote_method_weight'];
  }

  variable_set('uc_quote_enabled', $enabled);
  variable_set('uc_quote_method_weight', $method_weight);
  variable_set('uc_quote_type_weight', $form_state['values']['uc_quote_type_weight']);
  variable_set('uc_store_shipping_type', $form_state['values']['uc_store_shipping_type']);
  drupal_set_message(t('The configuration options have been saved.'));
}

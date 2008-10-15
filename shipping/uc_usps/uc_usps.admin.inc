<?php
// $Id: uc_usps.admin.inc,v 1.1.2.1 2008/10/15 14:47:41 islandusurper Exp $

/**
 * @file
 * USPS administration menu items.
 *
 * Coded by Lyle Mantooth.
 */

/**
 * Configure USPS settings.
 *
 * @ingroup forms
 */
function uc_usps_admin_settings() {
  $form = array();

  $form['uc_usps_user_id'] = array('#type' => 'textfield',
    '#title' => t('USPS user ID'),
    '#description' => t('To acquire or locate your user ID, refer to the <a href="!url">USPS documentation</a>.', array('!url' => 'http://www.ubercart.org/docs/user/312/usps_web_tool_shipping_quote_settings')),
    '#default_value' => variable_get('uc_usps_user_id', ''),
  );
  $form['uc_usps_services'] = array('#type' => 'checkboxes',
    '#title' => t('USPS services'),
    '#default_value' => variable_get('uc_usps_services', _uc_usps_services()),
    '#options' => _uc_usps_services(),
    '#description' => t('Select the USPS services that are available to customers. Be sure to include the services that the Postal Service agrees are available to you.'),
  );
  $form['uc_usps_intl_services'] = array('#type' => 'checkboxes',
    '#title' => t('USPS international services'),
    '#default_value' => variable_get('uc_usps_intl_services', _uc_usps_intl_services()),
    '#options' => _uc_usps_intl_services(),
    '#description' => t('Select the USPS services that are available to customers. Be sure to include the services that the Postal Service agrees are available to you.'),
  );
  $form['uc_usps_markup_type'] = array('#type' => 'select',
    '#title' => t('Markup type'),
    '#default_value' => variable_get('uc_usps_markup_type', 'percentage'),
    '#options' => array(
      'percentage' => t('Percentage (%)'),
      'multiplier' => t('Multiplier (�)'),
      'currency' => t('Addition (!currency)', array('!currency' => variable_get('uc_currency_sign', '$'))),
    ),
  );
  $form['uc_usps_markup'] = array('#type' => 'textfield',
    '#title' => t('Shipping rate markup'),
    '#default_value' => variable_get('uc_usps_markup', '0'),
    '#description' => t('Markup shipping rate quote by dollar amount, percentage, or multiplier.'),
  );
  $form['uc_usps_all_in_one'] = array('#type' => 'radios',
    '#title' => t('Product packages'),
    '#default_value' => variable_get('uc_usps_all_in_one', 1),
    '#options' => array(
      0 => t('Each in its own package'),
      1 => t('All in one'),
    ),
    '#description' => t('Indicate whether each product is quoted as shipping separately or all in one package.'),
  );

  return system_settings_form($form);
}


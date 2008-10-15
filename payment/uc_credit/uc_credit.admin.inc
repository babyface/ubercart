<?php
// $Id: uc_credit.admin.inc,v 1.1.2.1 2008/10/15 14:47:45 islandusurper Exp $

/**
 * @file
 * Credit administration menu items.
 *
 */

// Displays the credit card terminal page.
function uc_credit_terminal($order) {
  $output = l(t('Return to order view screen.'), 'admin/store/orders/'. $order_id);

  $output .= '<p>'. t('Use this terminal to process credit card payments through your default gateway.') .'</p>';

  if (!variable_get('uc_credit_debug', FALSE) || variable_get('uc_credit_checkout_process', TRUE)) {
    $output .= '<p>'. t('Be warned that credit card data will automatically be converted to the last 4 digits of the card once a transaction has occurred.  As such, subsequent charges after a partial payment will not have any stored credit card information to use.') .'</p>';
  }

  $output .= drupal_get_form('uc_credit_terminal_form', $order, $balance);

  return $output;
}

// Displays the credit card terminal form for administrators.
function uc_credit_terminal_form($form_state, $order, $lock_amount = FALSE) {
  $form['cc_data'] = array(
    '#type' => 'fieldset',
    '#title' => t('Credit card details'),
    '#theme' => 'uc_payment_method_credit_form',
    '#tree' => TRUE,
  );
  $form['cc_data'] += uc_payment_method_credit_form(array(), $order);
  unset($form['cc_data']['cc_policy']);

  $balance = uc_payment_balance($order);
  $form['review'] = array(
    '#value' => '<div><strong>'. t('Order total:') .' </strong>'
              . uc_currency_format($order->order_total) .'</div>'
              .'<div><strong>'. t('Balance:') .' </strong>'
              . uc_currency_format($balance) .'</div>',
  );

  $form['amount'] = array(
    '#type' => 'textfield',
    '#title' => t('Charge Amount'),
    '#default_value' => uc_currency_format($balance, FALSE, FALSE, '.'),
    '#size' => 10,
    '#disabled' => $lock_amount,
    '#field_prefix' => variable_get('uc_sign_after_amount', FALSE) ? '' : variable_get('uc_currency_sign', '$'),
    '#field_suffix' => variable_get('uc_sign_after_amount', FALSE) ? variable_get('uc_currency_sign', '$') : '',
  );
  $form['order_id'] = array(
    '#type' => 'hidden',
    '#value' => $order->order_id,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Charge card'),
    '#weight' => 10,
  );

  return $form;
}

function uc_credit_terminal_form_validate($form, &$form_state) {
  if (!is_numeric($form_state['values']['amount']) || $form_state['values']['amount'] <= 0) {
    form_set_error('amount', t('You must enter a positive number for the amount.'));
  }

  $order = uc_order_load($form_state['values']['order_id']);
  if ($order === FALSE) {
    form_set_error('', t('Invalid order ID.  Unable to process payment.'));
  }
}

function uc_credit_terminal_form_submit($form, &$form_state) {
  // Load the order.
  $order = uc_order_load($form_state['values']['order_id']);

  // Get the data from the form and replace masked data from the order.
  $cc_data = $form_state['values']['cc_data'];

  if (strpos($cc_data['cc_number'], t('(Last 4) ')) === 0) {
    $cc_data['cc_number'] = $order->payment_details['cc_number'];
  }

  if ($cc_data['cc_cvv'] == str_repeat('-', strlen($cc_data['cc_cvv']))) {
    $cc_data['cc_cvv'] = $order->payment_details['cc_cvv'];
  }

  // Cache the values for use during processing.
  uc_credit_cache('save', $cc_data, FALSE);

  $result = uc_payment_process('credit', $form_state['values']['order_id'], $form_state['values']['amount'], NULL, TRUE, NULL, FALSE);

  if ($result) {
    $crypt = new uc_encryption_class;

    // Load up the existing data array.
    $data = db_result(db_query("SELECT data FROM {uc_orders} WHERE order_id = %d", $form_state['values']['order_id']));
    $data = unserialize($data);

    $cache = uc_credit_cache('load');

    if (variable_get('uc_credit_debug', FALSE) && !variable_get('uc_credit_checkout_process', TRUE)) {
      $cc_data = $cache;
    }
    else {
      $cc_data = array(
        'cc_number' => substr($cache['cc_number'], -4),
      );
    }

    // Stuff the serialized and encrypted CC details into the array.
    $data['cc_data'] = $crypt->encrypt(uc_credit_encryption_key(), serialize($cc_data));
    uc_store_encryption_errors($crypt, 'uc_credit');

    // Save it again.
    db_query("UPDATE {uc_orders} SET data = '%s' WHERE order_id = %d", serialize($data), $form_state['values']['order_id']);

    drupal_set_message(t('The credit card payment was successful. See the admin comments for more details.'));
  }
  else {
    if (variable_get('uc_credit_debug', FALSE)) {
      _save_cc_data_to_order(uc_credit_cache('load'), $form_state['values']['order_id']);
    }

    drupal_set_message(t('There was an error processing the credit card.  See the admin comments for details.'), 'error');
  }

  $form_state['redirect'] = 'admin/store/orders/'. $form_state['values']['order_id'];
}


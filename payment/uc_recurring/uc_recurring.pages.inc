<?php
// $Id: uc_recurring.pages.inc,v 1.1.2.1 2008/10/15 14:47:51 islandusurper Exp $

/**
 * @file
 * Recurring payments menu items.
 *
 */

// Displays the confirm form for cancelling a recurring fee.
function uc_recurring_user_cancel_form($form_state, $uid, $rfid) {
  $form['uid'] = array(
    '#type' => 'value',
    '#value' => $uid,
  );

  $form['rfid'] = array(
    '#type' => 'value',
    '#value' => $rfid,
  );

  return confirm_form($form, t('Are you sure you want to cancel your recurring fee?'), 'user/'. $uid, t('This action cannot be undone and may result in the termination of subscription services.'), t('Confirm'), t('Cancel'));
}

function uc_recurring_user_cancel_form_submit($form, &$form_state) {
  uc_recurring_fee_cancel($form_state['values']['rfid']);

  drupal_set_message(t('The recurring fee has been cancelled.'));

  $form_state['redirect'] = 'user/'. $form_state['values']['uid'];
}


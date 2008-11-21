<?php
// $Id: uc_file.ca.inc,v 1.1.2.1 2008/11/21 21:17:26 islandusurper Exp $

/**
 * @file
 * This file contains the Conditional Actions hooks and functions necessary to make the
 * file-related entity, conditions, events, and actions work.
 */


/******************************************************************************
 * Conditional Actions Hooks                                                  *
 ******************************************************************************/

/**
 * Implementation of hook_ca_predicate().
 */
function uc_file_ca_predicate() {

  // Renew all the roles on an order when the status matches what's set in the roles admin settings.
  $configurations['uc_file_renewal'] = array(
    '#title' => t('Renew purchased files'),
    '#description' => t('Renew purchased files if the order status matches.'),
    '#class' => 'renewal',
    '#trigger' => 'uc_order_status_update',
    '#status' => 1,
    '#conditions' => array(
      '#operator' => 'AND',
      '#conditions' => array(
        array(
          '#name' => 'uc_order_status_condition',
          '#title' => t('If the order status is completed.'),
          '#argument_map' => array(
            'order' => 'order',
          ),
          '#settings' => array(
            'order_status' => 'completed',
          ),
        ),
      ),
    ),
    '#actions' => array(
      array(
        '#name' => 'uc_file_order_renew',
        '#title' => t('Update all file expirations for this order.'),
        '#argument_map' => array(
          'order' => 'order',
        ),
        '#settings' => array(
          'order_status' => 'completed',
        ),
      ),
    ),
  );

  return $configurations;
}

/**
 * Implementation of hook_ca_action().
 */
function uc_file_ca_action() {

  $actions['uc_file_order_renew'] = array(
    '#title' => t('Renew the files on an order.'),
    '#category' => t('renewal'),
    '#callback' => 'uc_file_action_order_renew',
    '#arguments' => array(
      'order' => array(
        '#entity' => 'uc_order',
        '#title' => t('Order'),
      ),
    ),
  );

  return $actions;
}

/**
 * Renew an orders product files.
 *
 * @param $order
 *   An Ubercart order object.
 *
 * This function updates access limits on all files found on all products
 * on a given order. First, the order user is loaded, then the order's products
 * are scanned for file product features. An order comment is saved, and the user is
 * notified in Drupal, as well as through the email address associated with the
 * order.
 */
function uc_file_action_order_renew($order) {

  $user_downloads = array();

  // Load user.
  if (!$order->uid || !($order_user = user_load($order->uid))) {
    return;
  }

  // Scan products for models matching downloads.
  foreach ($order->products as $product) {
    $files = db_query("SELECT * FROM {uc_file_products} AS fp ".
      "INNER JOIN {uc_product_features} AS pf ON pf.pfid = fp.pfid ".
      "INNER JOIN {uc_files} as f ON f.fid = fp.fid ".
      "WHERE nid = %d", $product->nid, $order_user->uid);

    while ($file = db_fetch_object($files)) {

      // Either they match, or the file was set to any SKU.
      if (!empty($file->model) && $file->model != $product->model) {
        continue;
      }

      // Grab any existing privilege so we can calculate the new expiration time
      // as an offset of the previous.
      $file_user = _uc_file_user_get($order_user, $file->fid);

      // Get the limit info from the product feature
      $file_modification = array(
        'download_limit' => uc_file_get_download_limit($file),
        'address_limit' => uc_file_get_address_limit($file),
        'expiration' => _uc_file_expiration_date(uc_file_get_time_limit($file), $file_user->expiration),
      );

      // Add file_user(s) for this file/directory. (No overwrite)
      $new_files = uc_file_user_renew($file->fid, $order_user, $file->pfid, $file_modification, FALSE);

      // Save for notification.
      $user_downloads = array_merge($user_downloads, $new_files);

      // Note on the order where the user has gained download permission.
      if (is_dir(uc_file_qualify_file($file->filename))) {
        $comment = t('User can now download files in the directory %dir.', array('%dir' => $file->filename));
      }
      else {
        $comment = t('User can now download the file %file.', array('%file' => basename($file->filename)));
      }
      uc_order_comment_save($order->order_id, $user->uid, $comment);
    }
  }

  // Notify the user of their download(s).
  if (variable_get('uc_file_download_notification', FALSE) && count($user_downloads)) {
    uc_file_email_download($order_user, $order, $user_downloads);
  }

}

function uc_file_action_order_renew_form($form_state, $settings = array()) {
  return uc_order_action_update_status_form($form_state, $settings);
}

function uc_file_action_order_renew_submit($form, &$form_state) {
  return uc_order_action_update_status_submit($form, $form_state);
}


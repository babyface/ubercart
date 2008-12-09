<?php
// $Id: uc_roles.ca.inc,v 1.1.2.1 2008/12/09 15:21:02 islandusurper Exp $

/**
 * @file
 * This file contains the Conditional Actions hooks and functions necessary to make the
 * roles-related entity, conditions, events, and actions work.
 */


/******************************************************************************
 * Conditional Actions Hooks                                                  *
 ******************************************************************************/

/**
 * Implementation of hook_ca_predicate().
 */
function uc_roles_ca_predicate() {
  // Renew all the roles on an order when the status matches what's set in the roles admin settings.
  $configurations['uc_role_renewal'] = array(
    '#title' => t('Renew purchased roles'),
    '#description' => t('Renew purchased roles if the order status matches.'),
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
        '#name' => 'uc_roles_order_renew',
        '#title' => t('Update all role expirations for this order.'),
        '#argument_map' => array(
          'order' => 'order',
        ),
      ),
    ),
  );

  $configurations['uc_roles_revocation_email'] = array(
    '#title' => t('Role revocation email'),
    '#description' => t('Send a notification to the user that a role was revoked.'),
    '#class' => t('Roles'),
    '#status' => 0,
    '#trigger' => 'uc_roles_role_revoked',
    '#actions' => array(
      array(
        '#name' => 'send_email_user',
        '#title' => t('Send role revocation notification email.'),
        '#argument_map' => array(
          'user' => 'user',
        ),
      ),
    ),
  );

  return $configurations;
}

/**
 * Implementation of hook_ca_action().
 */
function uc_roles_ca_action() {

  $actions['uc_roles_order_renew'] = array(
    '#title' => t('Renew the roles on an order.'),
    '#category' => t('renewal'),
    '#callback' => 'uc_roles_action_order_renew',
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
 * Renew an orders product roles.
 *
 * @param $order
 *   An Ubercart order object.
 *
 * This function updates expiration time on all roles found on all products
 * on a given order. First, the order user is loaded, then the order's products
 * are scanned for role product features. If any are found, the expiration time
 * of the role is set using the feature settings to determine the new length of
 * time the new expiration will last. An order comment is saved, and the user is
 * notified in Drupal, as well as through the email address associated with the
 * order.
 */
function uc_roles_action_order_renew($order) {

  global $user;

  // Load the order's user.
  if (!($order_user = user_load($order->uid))) {
    return;
  }

  foreach ($order->products as $product) {

    $roles = db_query("SELECT * FROM {uc_roles_products} WHERE nid = %d", $product->nid);
    while ($role = db_fetch_object($roles)) {

      // Product model matches, or was 'any'.
      if (!empty($role->model) && $role->model != $product->model) {
        continue;
      }

      // Leave an order comment.
      $existing_role = db_fetch_object(db_query("SELECT * FROM {uc_roles_expirations} WHERE uid = %d AND rid = %d", $order_user->uid, $role->rid));
      if (!is_null($existing_role->expiration)) {
        $op = 'renew';
        $comment = t('Customer user role %role renewed.', array('%role' => _uc_roles_get_name($role->rid)));
      }
      else {
        $op = 'grant';
        $comment = t('Customer granted user role %role.', array('%role' => _uc_roles_get_name($role->rid)));
      }
      uc_order_comment_save($order->order_id, $user->uid, $comment);

      // Calc total duration based on if user wanted to multiply by quantity.
      $duration = $role->duration;
      $duration *= $role->by_quantity ? $product->qty : 1;

      // Grant the role to the user.
      uc_roles_grant($order_user, $role->rid, _uc_roles_get_expiration($duration, $role->granularity, $existing_role->expiration));

      // Get the new expiration (if applicable)
      $new_expiration = db_fetch_object(db_query("SELECT * FROM {uc_roles_expirations} WHERE uid = %d AND rid = %d", $order_user->uid, $role->rid));

      // Trigger role update email.
      uc_roles_email_user($op, $order_user, $new_expiration, $order);

    }
  }
}

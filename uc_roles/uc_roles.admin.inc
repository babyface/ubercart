<?php
// $Id: uc_roles.admin.inc,v 1.1.2.3 2008/12/11 15:53:50 islandusurper Exp $

/**
 * @file
 * Roles administration menu items.
 *
 */

function _uc_roles_expiration_header() {

  // Create the header for the table/pager.
  return array(
    array('data' => t('Username'), 'field' => 'u.name'),
    array('data' => t('Role'), 'field' => 'e.rid'),
    array('data' => t('Expiration date'), 'field' => 'e.expiration', 'sort' => 'asc'),
    t('Operations')
  );
}

/**
 * Menu callback for viewing expirations.
 */
function uc_roles_expiration() {

  // Create the header for the pager.
  $header = _uc_roles_expiration_header();

  // Grab all the info to build the pager.
  $sql = 'SELECT * FROM {uc_roles_expirations} AS e INNER JOIN {users} AS u ON e.uid = u.uid';
  $sql .= tablesort_sql($header);
  $result = pager_query($sql, 50, 0, NULL);

  // Stick the expirations into the form.
  while ($row = db_fetch_object($result)) {
    $form['name'][$row->uid .' '. $row->rid] = array('#value' => theme('username', $row));
    $form['role'][$row->uid .' '. $row->rid] = array('#value' => _uc_roles_get_name($row->rid));
    $form['expiration'][$row->uid .' '. $row->rid] = array('#value' => format_date($row->expiration, 'small'));
    $form['operations'][$row->uid .' '. $row->rid] = array('#value' => l(t('delete'), 'admin/user/user/expiration/delete/'. $row->uid .'/'. $row->rid) .' '. l(t('edit'), 'user/'. $row->uid .'/edit', array('fragment' => 'role-expiration-'. $row->rid, 'query' => 'destination=admin%2Fuser%2Fuser%2Fexpiration')));
  }

  return $form;
}

/**
 * Theme user role expiration page.
 */
function theme_uc_roles_expiration($form) {

  $header = _uc_roles_expiration_header();

  if (is_array($form['name'])) {
    foreach (element_children($form['name']) as $key) {
      $rows[] = array(
        drupal_render($form['name'][$key]),
        drupal_render($form['role'][$key]),
        drupal_render($form['expiration'][$key]),
        drupal_render($form['operations'][$key]),
      );
    }
  }
  else  {
    $rows[] = array(array('data' => t('No expirations set to occur'), 'colspan' => '4'));
  }

  // Render everything.
  $output .= theme('table', $header, $rows);
  $output .= theme('pager', NULL, 50, 0);
  $output .= drupal_render($form);

  return $output;
}

/**
 * Form builder for role notification settings.
 */
function uc_roles_notify_settings() {

  // Set Granted Notifications.
  $form['grant'] = array(
    '#type' => 'fieldset',
    '#title' => t('Granted message'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['grant']['uc_roles_grant_notification'] = array(
    '#type' => 'checkbox',
    '#default_value' => variable_get('uc_roles_grant_notification', FALSE),
    '#title' => t('Send email after customer receives new role'),
  );
  $form['grant']['uc_roles_grant_notification_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Message subject'),
    '#default_value' => variable_get('uc_roles_grant_notification_subject', uc_get_message('uc_roles_grant_subject')),
  );
  $form['grant']['uc_roles_grant_notification_message'] = array(
    '#type' => 'textarea',
    '#title' => t('Message text'),
    '#default_value' => variable_get('uc_roles_grant_notification_message', uc_get_message('uc_roles_grant_message')),
    '#description' => t('The message the user receives when granted a new role (<a href="!url">uses order, uc_roles, and global tokens</a>).', array('!url' => url('admin/store/help/tokens'))),
    '#rows' => 10,
  );
  $form['grant']['uc_roles_grant_notification_format'] = filter_form(variable_get('uc_roles_grant_notification_format', FILTER_FORMAT_DEFAULT), NULL, array('uc_roles_grant_notification_format'));

  // Set Revocation Notifications.
  $form['revoke'] = array(
    '#type' => 'fieldset',
    '#title' => t('Expiration message'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['revoke']['uc_roles_revocation_notification'] = array(
    '#type' => 'checkbox',
    '#default_value' => variable_get('uc_roles_revocation_notification', FALSE),
    '#title' => t('Send email after customer loses role'),
  );
  $form['revoke']['uc_roles_revocation_notification_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Message subject'),
    '#default_value' => variable_get('uc_roles_revocation_notification_subject', uc_get_message('uc_roles_revoke_subject')),
  );
  $form['revoke']['uc_roles_revocation_notification_message'] = array(
    '#type' => 'textarea',
    '#title' => t('Message text'),
    '#default_value' => variable_get('uc_roles_revocation_notification_message', uc_get_message('uc_roles_revoke_message')),
    '#description' => t('The message the user receives when a role expires (<a href="!url">uses order, uc_roles, and global tokens</a>).', array('!url' => url('admin/store/help/tokens'))),
    '#rows' => 10,
  );
  $form['revoke']['uc_roles_revocation_notification_format'] = filter_form(variable_get('uc_roles_revocation_notification_format', FILTER_FORMAT_DEFAULT), NULL, array('uc_roles_revocation_notification_format'));

  // Set Renewal Notifications.
  $form['renew'] = array(
    '#type' => 'fieldset',
    '#title' => t('Renewal message'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['renew']['uc_roles_renewal_notification'] = array(
    '#type' => 'checkbox',
    '#default_value' => variable_get('uc_roles_renewal_notification', FALSE),
    '#title' => t('Send email after customer renews existing role'),
  );
  $form['renew']['uc_roles_renewal_notification_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Message subject'),
    '#default_value' => variable_get('uc_roles_renewal_notification_subject', uc_get_message('uc_roles_renew_subject')),
  );
  $form['renew']['uc_roles_renewal_notification_message'] = array(
    '#type' => 'textarea',
    '#title' => t('Message text'),
    '#default_value' => variable_get('uc_roles_renewal_notification_message', uc_get_message('uc_roles_renew_message')),
    '#description' => t('The message the user receives when a role is renewed (<a href="!url">uses order, uc_roles, and global tokens</a>).', array('!url' => url('admin/store/help/tokens'))),
    '#rows' => 10,
  );
  $form['renew']['uc_roles_renewal_notification_format'] = filter_form(variable_get('uc_roles_renewal_notification_format', FILTER_FORMAT_DEFAULT), NULL, array('uc_roles_renewal_notification_format'));

  // Set Reminder Notifications.
  $form['reminder'] = array(
    '#type' => 'fieldset',
    '#title' => t('Expiration reminder message'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['reminder']['uc_roles_reminder_length'] = array(
    '#type' => 'textfield',
    '#title' => t('Time before reminder'),
    '#default_value' => (variable_get('uc_roles_reminder_granularity', 'never') == 'never') ? NULL : variable_get('uc_roles_reminder_length', NULL),
    '#size' => 4,
    '#maxlength' => 4,
    '#prefix' => '<div class="expiration">',
    '#suffix' => '</div>',
  );
  $form['reminder']['uc_roles_reminder_granularity'] = array(
    '#type' => 'select',
    '#default_value' => variable_get('uc_roles_reminder_granularity', 'never'),
    '#options' => array(
      'never' => t('never'),
      'day' => t('day(s)'),
      'week' => t('week(s)'),
      'month' => t('month(s)'),
      'year' => t('year(s)')
    ),
    '#description' => t('The amount of time before a role expiration takes place that a customer is notified of its expiration.'),
    '#prefix' => '<div class="expiration">',
    '#suffix' => '</div>',
  );
  $form['reminder']['uc_roles_reminder_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Message subject'),
    '#default_value' => variable_get('uc_roles_reminder_subject', uc_get_message('uc_roles_reminder_subject')),
  );
  $form['reminder']['uc_roles_reminder_message'] = array(
    '#type' => 'textarea',
    '#title' => t('Message text'),
    '#default_value' => variable_get('uc_roles_reminder_message', uc_get_message('uc_roles_reminder_message')),
    '#description' => t('The message the user receives reminding them of the role expiration (<a href="!url">uses order, uc_roles, and global tokens</a>).', array('!url' => url('admin/store/help/tokens'))),
    '#rows' => 10,
  );
  $form['reminder']['uc_roles_reminder_format'] = filter_form(variable_get('uc_roles_reminder_format', FILTER_FORMAT_DEFAULT), NULL, array('uc_roles_reminder_format'));

  return system_settings_form($form);
}

/**
 * Form builder for role expirations.
 */
function uc_roles_deletion_form($form_id, $account, $rid) {

  $expiration = db_result(db_query("SELECT expiration FROM {uc_roles_expirations} WHERE uid = %d AND rid = %d", $account->uid, $rid));
  if ($expiration) {

    $role_name = _uc_roles_get_name($rid);

    $form['user'] = array('#type' => 'value', '#value' => $account->name);
    $form['uid'] = array('#type' => 'value', '#value' => $account->uid);
    $form['role'] = array('#type' => 'value', '#value' => $role_name);
    $form['rid'] = array('#type' => 'value', '#value' => $rid);

    $form = confirm_form(
      $form, t('Delete expiration of %role_name role for the user %user_name?', array('%user_name' => $account->name, '%role_name' => $role_name)),
      'admin/user/user/expiration',
      t('Deleting the expiration will give %user_name privileges set by the %role_name role indefinitely unless manually removed.', array('%user_name' => $account->name, '%role_name' => $role_name)),
      t('Yes'), t('No')
    );
  }
  else {
    $form['error'] = array(
      '#type' => 'markup',
      '#value' => t('Invalid user id or role id.'),
    );
  }

  return $form;
}

/**
 * Form submit handler for role expirations.
 */
function uc_roles_deletion_form_submit($form, &$form_state) {
  uc_roles_delete(user_load($form_state['values']['uid']), $form_state['values']['rid']);

  drupal_goto('admin/user/user/expiration');
}


<?php
// $Id: uc_order_order_pane.inc,v 1.5 2008/07/10 12:41:02 islandusurper Exp $

/**
 * @file
 * This file contains the callbacks for the default order panes supplied with
 * Ubercart and their corresponding helper functions.
 *
 * Order panes are defined using hook_order_pane() and use a callback to handle
 * the different processes involved in order viewing/editing.  The default order
 * panes are defined in uc_order_order_pane() in uc_order.module.
 */

/**
 * Handle the "Ship to" order pane.
 */
function uc_order_pane_ship_to($op, $arg1) {
  switch ($op) {
    case 'customer':
      if (!uc_order_is_shippable($arg1)) {
        return;
      }
    case 'view':
      $output = uc_order_address($arg1, 'delivery') .'<br />'. check_plain($arg1->delivery_phone);
      return $output;

    case 'edit-form':
      $form['ship_to'] = array(
        '#type' => 'fieldset',
        '#title' => t("Modify 'Ship to' information"),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
      );
      if (uc_address_field_enabled('first_name')) {
        $form['ship_to']['delivery_first_name'] = uc_textfield(uc_get_field_name('first_name'), $arg1->delivery_first_name, FALSE);
      }
      if (uc_address_field_enabled('last_name')) {
        $form['ship_to']['delivery_last_name'] = uc_textfield(uc_get_field_name('last_name'), $arg1->delivery_last_name, FALSE);
      }
      if (uc_address_field_enabled('phone')) {
        $form['ship_to']['delivery_phone'] = uc_textfield(uc_get_field_name('phone'), $arg1->delivery_phone, FALSE, NULL, 32, 16);
      }
      if (uc_address_field_enabled('company')) {
        $form['ship_to']['delivery_company'] = uc_textfield(uc_get_field_name('company'), $arg1->delivery_company, FALSE, NULL, 64);
      }
      if (uc_address_field_enabled('street1')) {
        $form['ship_to']['delivery_street1'] = uc_textfield(uc_get_field_name('street1'), $arg1->delivery_street1, FALSE, NULL, 64);
      }
      if (uc_address_field_enabled('street2')) {
        $form['ship_to']['delivery_street2'] = uc_textfield(uc_get_field_name('street2'), $arg1->delivery_street2, FALSE, NULL, 64);
      }
      if (uc_address_field_enabled('city')) {
        $form['ship_to']['delivery_city'] = uc_textfield(uc_get_field_name('city'), $arg1->delivery_city, FALSE);
      }
      if (uc_address_field_enabled('country')) {
        $form['ship_to']['delivery_country'] = uc_country_select(uc_get_field_name('country'), $arg1->delivery_country);
      }
      if (uc_address_field_enabled('zone')) {
        if (isset($_POST['delivery_country'])) {
          $country_id = intval($_POST['delivery_country']);
        }
        else {
          $country_id = $arg1->delivery_country;
        }
        $form['ship_to']['delivery_zone'] = uc_zone_select(uc_get_field_name('zone'), $arg1->delivery_zone, NULL, $country_id);
      }
      if (uc_address_field_enabled('postal_code')) {
        $form['ship_to']['delivery_postal_code'] = uc_textfield(uc_get_field_name('postal_code'), $arg1->delivery_postal_code, FALSE, NULL, 10, 10);
      }
      return $form;

    case 'edit-title':
      $output .= ' <img src="'. base_path() . drupal_get_path('module', 'uc_store')
                .'/images/address_book.gif" alt="Select from address book." '
                .'title="Select from address book." onclick="load_address_select('. $arg1['order_uid']['#value'] .', \'#delivery_address_select\', \'delivery\');" '
                .'style="position: relative; top: 2px; cursor: pointer;" />';
      return $output;

    case 'edit-theme':
      $output = '<div id="delivery_address_select"></div><table class="order-edit-table">';
      foreach (element_children($arg1['ship_to']) as $field) {
        $title = $arg1['ship_to'][$field]['#title'];
        $arg1['ship_to'][$field]['#title'] = NULL;
        $output .= '<tr><td class="oet-label">'. $title .':</td><td>'
                 . drupal_render($arg1['ship_to'][$field]) .'</td></tr>';
      }
      $output .= '</table>';
      return $output;

    case 'edit-process':
      foreach ($arg1 as $key => $value) {
        if (substr($key, 0, 9) == 'delivery_') {
          if (uc_address_field_enabled(substr($key, 9))) {
            $changes[$key] = $value;
          }
        }
      }
      return $changes;
  }
}

/**
 * Handle the "Bill to" order pane.
 */
function uc_order_pane_bill_to($op, $arg1) {
  switch ($op) {
    case 'view':
    case 'customer':
      $output = uc_order_address($arg1, 'billing') .'<br />'. check_plain($arg1->billing_phone);
      return $output;

    case 'edit-form':
      $form['bill_to'] = array(
        '#type' => 'fieldset',
        '#title' => t("Modify 'Bill to' information"),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
      );
      if (uc_address_field_enabled('first_name')) {
        $form['bill_to']['billing_first_name'] = uc_textfield(uc_get_field_name('first_name'), $arg1->billing_first_name, FALSE);
      }
      if (uc_address_field_enabled('last_name')) {
        $form['bill_to']['billing_last_name'] = uc_textfield(uc_get_field_name('last_name'), $arg1->billing_last_name, FALSE);
      }
      if (uc_address_field_enabled('phone')) {
        $form['bill_to']['billing_phone'] = uc_textfield(uc_get_field_name('phone'), $arg1->billing_phone, FALSE, NULL, 32, 16);
      }
      if (uc_address_field_enabled('company')) {
        $form['bill_to']['billing_company'] = uc_textfield(uc_get_field_name('company'), $arg1->billing_company, FALSE, NULL, 64);
      }
      if (uc_address_field_enabled('street1')) {
        $form['bill_to']['billing_street1'] = uc_textfield(uc_get_field_name('street1'), $arg1->billing_street1, FALSE, NULL, 64);
      }
      if (uc_address_field_enabled('street2')) {
        $form['bill_to']['billing_street2'] = uc_textfield(uc_get_field_name('street2'), $arg1->billing_street2, FALSE, NULL, 64);
      }
      if (uc_address_field_enabled('city')) {
        $form['bill_to']['billing_city'] = uc_textfield(uc_get_field_name('city'), $arg1->billing_city, FALSE);
      }
      if (uc_address_field_enabled('country')) {
        $form['bill_to']['billing_country'] = uc_country_select(uc_get_field_name('country'), $arg1->billing_country);
      }
      if (uc_address_field_enabled('zone')) {
        if (isset($_POST['billing_country'])) {
          $country_id = intval($_POST['billing_country']);
        }
        else {
          $country_id = $arg1->billing_country;
        }
        $form['bill_to']['billing_zone'] = uc_zone_select(uc_get_field_name('zone'), $arg1->billing_zone, NULL, $country_id);
      }
      if (uc_address_field_enabled('postal_code')) {
        $form['bill_to']['billing_postal_code'] = uc_textfield(uc_get_field_name('postal_code'), $arg1->billing_postal_code, FALSE, NULL, 10, 10);
      }
      return $form;

    case 'edit-title':
      $output .= ' <img src="'. base_path() . drupal_get_path('module', 'uc_store')
                .'/images/address_book.gif" alt="'. t('Select from address book.') .'" '
                .'title="'. t('Select from address book.') .'" onclick="load_address_select('. $arg1['order_uid']['#value'] .', \'#billing_address_select\', \'billing\');" '
                .'style="position: relative; top: 2px; cursor: pointer;" />';
      $output .= ' <img src="'. base_path() . drupal_get_path('module', 'uc_store')
               .'/images/copy.gif" alt="'. t('Copy shipping information.') .'" title="'
               . t('Copy shipping information.') .'" onclick="uc_order_copy_shipping_to_billing();" '
               .'style="position: relative; top: 2px; cursor: pointer;" />';
      return $output;

    case 'edit-theme':
      $output = '<div id="billing_address_select"></div><table class="order-edit-table">';
      foreach (element_children($arg1['bill_to']) as $field) {
        $title = $arg1['bill_to'][$field]['#title'];
        $arg1['bill_to'][$field]['#title'] = NULL;
        $output .= '<tr><td class="oet-label">'. $title .':</td><td>'
                 . drupal_render($arg1['bill_to'][$field]) .'</td></tr>';
      }
      $output .= '</table>';
      return $output;

    case 'edit-process':
      foreach ($arg1 as $key => $value) {
        if (substr($key, 0, 8) == 'billing_') {
          if (uc_address_field_enabled(substr($key, 8))) {
            $changes[$key] = $value;
          }
        }
      }
      return $changes;
  }
}

/**
 * Handle the "Customer Info" order pane.
 */
function uc_order_pane_customer($op, $arg1) {
  switch ($op) {
    case 'view':
      $output = t('Customer number: ') . l($arg1->uid, 'user/'. $arg1->uid)
               .'<br />'. t('Primary e-mail:') .'<br />'
              . check_plain($arg1->primary_email);
      return $output;

    case 'edit-form':
      $form['customer'] = array(
        '#type' => 'fieldset',
        '#title' => t("Modify 'Customer info'"),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE
      );
      $form['customer']['uid'] = array(
        '#type' => 'hidden',
        '#default_value' => $arg1->uid,
      );
      $form['customer']['text']['uid_text'] = array(
        '#type' => 'textfield',
        '#title' => t('Customer number'),
        '#default_value' => $arg1->uid,
        '#maxlength' => 10,
        '#size' => 10,
        '#disabled' => TRUE,
      );
      $form['customer']['primary_email'] = array(
        '#type' => 'hidden',
        '#default_value' => $arg1->primary_email,
      );
      $form['customer']['text']['primary_email_text'] = array(
        '#type' => 'textfield',
        '#title' => t('Primary e-mail'),
        '#default_value' => $arg1->primary_email,
        '#maxlength' => 64,
        '#size' => 32,
        '#disabled' => TRUE,
      );
      return $form;

    case 'edit-title':
      $output .= ' <img src="'. base_path() . drupal_get_path('module', 'uc_store')
                .'/images/order_view.gif" alt="'. t('Search for an existing customer.') .'" '
                .'title="'. t('Search for an existing customer.') .'" onclick="load_customer_search();" '
                .'style="position: relative; top: 2px; cursor: pointer;" />';
      $output .= ' <img src="'. base_path() . drupal_get_path('module', 'uc_store')
                .'/images/menu_customers_small.gif" alt="'. t('Create a new customer.') .'" '
                .'title="'. t('Create a new customer.') .'" onclick="load_new_customer_form();" '
                .'style="position: relative; top: 2px; cursor: pointer;" />';
      return $output;

    case 'edit-theme':
      $output = '<div id="customer-select"></div><table class="order-edit-table">';
      foreach (element_children($arg1['customer']['text']) as $field) {
        $title = $arg1['customer']['text'][$field]['#title'];
        $arg1['customer']['text'][$field]['#title'] = NULL;
        $output .= '<tr><td class="oet-label">'. $title .':</td><td>'
                 . drupal_render($arg1['customer']['text'][$field]) .'</td></tr>';
      }
      $output .= '</table>'. drupal_render($arg1['customer']['primary_email'])
               . drupal_render($arg1['customer']['uid']);
      return $output;

    case 'edit-process':
      $changes['uid'] = $arg1['uid'];
      $changes['primary_email'] = $arg1['primary_email'];
      return $changes;
  }
}

/**
 * Handle the "Products" order pane.
 */
function uc_order_pane_products($op, $arg1) {
  switch ($op) {
    case 'view':
      return tapir_get_table('op_products_view_table', $arg1->products);

    case 'customer':
      return tapir_get_table('op_products_customer_table', $arg1->products);

    case 'edit-theme':
      uc_add_js('uc_order_load_product_edit_div('. $arg1['order_id']['#value'] .');', 'inline');

      $output = '<div id="products-selector"></div>'
               .'<div id="products-container">'. t('Loading product information... (<em>If nothing happens, make sure you have Javascript enabled.</em>)') .'</div>';
      return $output;

    case 'edit-title':
      $settings = array(
        'div' => '#products-selector',
        'class' => 'product-select-box',
        'vid' => variable_get('uc_catalog_vid', 0),
        'filter' => implode(',', module_invoke_all('product_types')),
        'search' => 'true',
        'nids' => 'true',
        'close' => 'true',
        'nodesg' => 'product',
        'nodepl' => 'products',
        'select' => 'add_product_form();',
      );

      if (module_exists('ubrowser') && module_exists('uc_catalog') && variable_get('uc_ubrowser_product_select', TRUE)) {
        $output = ubrowser_button(t('Add product'), $settings, NULL, 'add-product-button') .' ';
      }
      else {
        $output = '<input type="button" value="'. t('Add product')
                 .'" onclick="load_product_select('. $arg1['order_id']['#value']
                 .', false);" id="add-product-button" />';
      }
      $output .= '<input type="button" value="'. t('Add blank line')
                .'" onclick="add_blank_line_button('. $arg1['order_id']['#value']
                .');" />';
      $output .= ' <span id="product-div-throbber"></span>';
      return $output;
  }
}

/**
 * Handle the "Line Items" order pane.
 */
function uc_order_pane_line_items($op, $arg1) {
  switch ($op) {
    case 'view':
    case 'customer':
      $line_items = $arg1->line_items;
      $items = _line_item_list();
      foreach ($items as $item) {
        if (isset($item['display_only']) && $item['display_only'] == TRUE) {
          $result = $item['callback']('display', $arg1);
          if (is_array($result)) {
            foreach ($result as $line) {
              $line_items[] = array(
                'title' => $line['title'],
                'amount' => $line['amount'],
                'weight' => $item['weight']
              );
            }
          }
        }
      }
      usort($line_items, 'uc_weight_sort');

      $output = '<table class="line-item-table">';
      foreach ($line_items as $item) {
        $output .= '<tr><td class="li-title">'. check_plain($item['title']) .':</td>'
                  .'<td class="li-amount" nowrap="nowrap">'
                 . uc_currency_format($item['amount']) .'</td></tr>';
      }
      $output .= '</table>';
      return $output;

    case 'show-title':
      return FALSE;

    case 'edit-form':
      $options = array();
      $items = _line_item_list();
      $line_items = $arg1->line_items;
      foreach ($items as $item) {
        if ($item['add_list'] === TRUE) {
          $options[$item['id']] = check_plain($item['title']);
        }
        if (isset($item['display_only']) && $item['display_only'] == TRUE) {
          $result = $item['callback']('display', $arg1);
          if (is_array($result)) {
            foreach ($result as $line) {
              $line_items[] = array(
                'line_item_id' => $line['id'],
                'title' => $line['title'],
                'amount' => $line['amount'],
                'weight' => $item['weight'],
              );
            }
          }
        }
      }
      usort($line_items, 'uc_weight_sort');

      $form['add_line_item'] = array(
        '#type' => 'fieldset',
        '#title' => t('Add line item'),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
      );
      $form['add_line_item']['li_type_select'] = array(
        '#type' => 'select',
        '#title' => t('Select a type'),
        '#options' => $options,
      );
      $form['add_line_item']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Add line'),
        '#attributes' => array('class' => 'save-button'),
        '#disabled' => TRUE,
      );
      $form['li_delete_id'] = array(
        '#type' => 'hidden',
      );
      $form['line_items'] = array(
        '#type' => 'fieldset',
        '#title' => t("Modify 'line items'"),
        '#collapsible' => TRUE,
        '#collapsed' => FALSE,
        '#tree' => TRUE,
      );
      foreach ($line_items as $item) {
        $form['line_items'][$item['line_item_id']]['li_id'] = array(
          '#type' => 'hidden',
          '#value' => $item['line_item_id'],
        );
        if (_line_item_data($item['type'], 'stored') == TRUE) {
          $form['line_items'][$item['line_item_id']]['title'] = array(
            '#type' => 'textfield',
            '#title' => t('Title'),
            '#default_value' => $item['title'],
            '#size' => 40,
            '#maxlength' => 128,
            '#prefix' => '<img id="li-'. $item['line_item_id'] .'" src="'
                       . base_path() . drupal_get_path('module', 'uc_store')
                        .'/images/error.gif" alt="'. t('Remove line item.')
                        .'" title="'. t('Remove line item.') .'" style="'
                        .'cursor: pointer; position: relative; top: 3px;" '
                        .'onclick="confirm_line_item_delete(\''
                       . addslashes(t('Remove this line item?')) .'\', this.id);" />',
          );
          $form['line_items'][$item['line_item_id']]['amount'] = array(
            '#type' => 'textfield',
            '#title' => t('Amount'),
            '#default_value' => $item['amount'],
            '#size' => 6,
            '#maxlength' => 13,
            '#field_prefix' => variable_get('uc_sign_after_amount', FALSE) ? '' : variable_get('uc_currency_sign', '$'),
            '#field_suffix' => variable_get('uc_sign_after_amount', FALSE) ? variable_get('uc_currency_sign', '$') : '',
          );
        }
        else {
          $form['line_items'][$item['line_item_id']]['title'] = array(
            '#value' => check_plain($item['title']),
          );
          $form['line_items'][$item['line_item_id']]['amount'] = array(
            '#value' => uc_currency_format($item['amount']),
          );
        }
      }
      return $form;

    case 'edit-theme':
      $arg1['add_line_item']['li_type_select']['#title'] = '';
      $output = '<table class="full-width"><tr><td>';
      $output .= '<table><tr><td colspan="2"><b>'. t('Add a line item')
                .': </b></td></tr><tr><td>'. drupal_render($arg1['add_line_item']['li_type_select'])
                .' </td><td>'. drupal_render($arg1['add_line_item']['submit'])
                .'</td></tr></table>';
      $output .= '</td><td>'. drupal_render($arg1['li_delete_id'])
                .'<table class="line-item-table">';
      foreach (element_children($arg1['line_items']) as $field) {
        $arg1['line_items'][$field]['title']['#title'] = '';
        $arg1['line_items'][$field]['amount']['#title'] = '';
        $output .= '<tr><td class="li-title">'
                 . drupal_render($arg1['line_items'][$field]['li_id'])
                 . drupal_render($arg1['line_items'][$field]['title'])
                  .':</td><td class="li-amount" nowrap>'
                 . drupal_render($arg1['line_items'][$field]['amount'])
                  .'</td></tr>';
      }
      $output .= '</table></td></tr></table>';
      return $output;

    case 'edit-process':
      if (is_array($arg1['line_items'])) {
        foreach ($arg1['line_items'] as $line) {
          if (is_numeric($line['li_id']) && intval($line['li_id']) > 0) {
            uc_order_update_line_item($line['li_id'], $line['title'], $line['amount']);
          }
        }
      }
      if (intval($arg1['li_delete_id']) > 0) {
        uc_order_delete_line_item($arg1['li_delete_id']);
        drupal_set_message(t('Line item removed.'));
      }
      return;

    case 'edit-ops':
      return array(t('Add line'));

    case t('Add line'):
      drupal_goto('admin/store/orders/'. $arg1['order_id']
                 .'/add_line_item/'. $arg1['li_type_select']);
  }
}

/**
 * Handle the "Order Comments" order pane.
 */
function uc_order_pane_order_comments($op, $arg1) {
  switch ($op) {
    case 'view':
      $comments = uc_order_comments_load($arg1->order_id);
      return tapir_get_table('op_order_comments_view_table', $comments);

    case 'customer':
      $comments = uc_order_comments_load($arg1->order_id);
      $header = array(t('Date'), t('Status'), array('data' => t('Message'), 'width' => '100%'));
      $rows[] = array(
        format_date($arg1->created, 'custom', variable_get('uc_date_format_default', 'm/d/Y')),
        array('data' => '-', 'align' => 'center'),
        t('Order created.')
      );
      if (count($comments) > 0) {
        foreach ($comments as $comment) {
          $rows[] = array(
            'data' => array(
              array('data' => format_date($comment->created, 'custom', variable_get('uc_date_format_default', 'm/d/Y')), 'align' => 'center'),
              array('data' => $comment->title, 'align' => 'center', 'nowrap' => 'nowrap'),
              check_plain($comment->message),
            ),
            'valign' => 'top'
          );
        }
      }
      $output = theme('table', $header, $rows);

      return $output;
  }
}

/**
 * Handle the "Admin Comments" order pane.
 */
function uc_order_pane_admin_comments($op, $arg1) {
  switch ($op) {
    case 'view':
      $comments = uc_order_comments_load($arg1->order_id, TRUE);
      return tapir_get_table('op_admin_comments_view_table', $comments);

    case 'edit-form':
      $form['admin_comment_field'] = array(
        '#type' => 'fieldset',
        '#title' => t('Add an admin comment'),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
      );
      $form['admin_comment_field']['admin_comment'] = array(
        '#type' => 'textarea',
        '#description' => t('Admin comments are only seen by store administrators.'),
      );
      return $form;

    case 'edit-theme':
      $comments = uc_order_comments_load($arg1['order_id']['#value'], TRUE);
      if (is_array($comments) && count($comments) > 0) {
        foreach ($comments as $comment) {
          $items[] = '['. uc_get_initials($comment->uid) .'] '. filter_xss_admin($comment->message);
        }
      }
      else {
        $items = array(t('No admin comments have been entered for this order.'));
      }
      $output = theme('item_list', $items) . drupal_render($arg1['admin_comment_field']);
      return $output;

    case 'edit-process':
      if (!is_null($arg1['admin_comment']) && strlen(trim($arg1['admin_comment'])) > 0) {
        global $user;
        uc_order_comment_save($arg1['order_id'], $user->uid, $arg1['admin_comment']);
      }
      return;
  }
}

/**
 * Handle the "Update" order pane.
 */
function uc_order_pane_update($op, $arg1) {
  switch ($op) {
    case 'view':
      return drupal_get_form('uc_order_view_update_form', $arg1);
  }
}

function uc_order_view_update_form($form_state, $order) {
  $form['order_comment_field'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add an order comment'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['order_comment_field']['order_comment'] = array(
    '#type' => 'textarea',
    '#description' => t('Order comments are used primarily to communicate with the customer.'),
  );

  $form['admin_comment_field'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add an admin comment'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['admin_comment_field']['admin_comment'] = array(
    '#type' => 'textarea',
    '#description' => t('Admin comments are only seen by store administrators.'),
  );

  $form['current_status'] = array(
    '#type' => 'hidden',
    '#value' => $order->order_status,
  );

  $form['order_id'] = array(
    '#type' => 'hidden',
    '#value' => $order->order_id,
  );

  $form['controls'] = array(
    '#theme' => 'uc_order_view_update_controls',
    '#weight' => 10,
  );

  foreach (uc_order_status_list('general') as $status) {
    $options[$status['id']] = $status['title'];
  }
  foreach (uc_order_status_list('specific') as $status) {
    $options[$status['id']] = $status['title'];
  }
  $form['controls']['status'] = array(
    '#type' => 'select',
    '#title' => t('Order status'),
    '#default_value' => $order->order_status,
    '#options' => $options,
    '#attributes' => array('style' => 'display: inline;'),
    '#weight' => -10,
  );

  $form['controls']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
    '#weight' => 10,
  );

  return $form;
}

function theme_uc_order_view_update_controls($form) {
  return '<div class="update-controls">'. drupal_render($form) .'</div>';
}

function uc_order_view_update_form_submit($form_id, &$form_state) {
  global $user;

  if ($form_state['values']['status'] != $form_state['values']['current_status']) {
    if (uc_order_update_status($form_state['values']['order_id'], $form_state['values']['status'])) {
      if (is_null($form_state['values']['order_comment']) || strlen($form_state['values']['order_comment']) == 0) {
        uc_order_comment_save($form_state['values']['order_id'], $user->uid, '-', 'order', $form_state['values']['status'], $form_state['values']['notify']);
      }
    }
  }

  if (!is_null($form_state['values']['order_comment']) && strlen(trim($form_state['values']['order_comment'])) > 0) {
    uc_order_comment_save($form_state['values']['order_id'], $user->uid, $form_state['values']['order_comment'], 'order', $form_state['values']['status'], $form_state['values']['notify']);
  }

  if (!is_null($form_state['values']['admin_comment']) && strlen(trim($form_state['values']['admin_comment'])) > 0) {
    uc_order_comment_save($form_state['values']['order_id'], $user->uid, $form_state['values']['admin_comment']);
  }

  drupal_set_message(t('Order updated.'));
}

/**
 * Build the order view products table.
 */
function op_products_view_table($form_state, $products) {
  $table = array();

  $fields = array();
  $fields[] = array(
    'id' => 'qty',
    'cell' => array(
      'data' => t('Qty'),
      'class' => 'text-right',
    ),
    'weight' => 0,
  );
  $fields[] = array(
    'id' => 'product',
    'cell' => array(
      'data' => t('Product'),
      'width' => '80%',
    ),
    'weight' => 1,
  );
  $fields[] = array(
    'id' => 'model',
    'cell' => array(
      'data' => t('Model'),
      'class' => 'text-center',
      'nowrap' => 'nowrap',
    ),
    'weight' => 2,
  );
  if (user_access('administer products')) {
    $fields[] = array(
      'id' => 'cost',
      'cell' => array(
        'data' => t('Cost'),
        'class' => 'text-right',
      ),
      'weight' => 3,
    );
  }
  $fields[] = array(
    'id' => 'price',
    'cell' => array(
      'data' => t('Price'),
      'class' => 'text-right',
    ),
    'weight' => 4,
  );
  $fields[] = array(
    'id' => 'total',
    'cell' => array(
      'data' => t('Total'),
      'class' => 'text-right',
    ),
    'weight' => 5,
  );

  $table['columns'] = $fields;

  if (is_array($products)) {
    foreach ($products as $product) {
      $data = array();
      $data['qty']['cell'] = array('data' => $product->qty .'x', 'align' => 'right');
      $options = $product->data['attributes'];
      $option_rows = array();
      if (module_exists('uc_attribute') && is_array($options)) {
        foreach ($options as $attribute => $option) {
          $option_rows[] = t('@attribute: @option', array('@attribute' => $attribute, '@option' => $option));
        }
      }
      $data['product']['cell'] = check_plain($product->title) . theme('item_list', $option_rows, NULL, 'ul', array('class' => 'product-options'));
      $data['model']['cell'] = array('data' => check_plain($product->model), 'align' => 'center', 'nowrap' => 'nowrap');
      if (user_access('administer products')) {
        $data['cost']['cell'] = array('data' => uc_currency_format($product->cost), 'align' => 'right', 'nowrap' => 'nowrap');
      }
      $data['price']['cell'] = array('data' => uc_currency_format($product->price), 'align' => 'right', 'nowrap' => 'nowrap');
      $data['total']['cell'] = array('data' => uc_currency_format($product->qty * $product->price), 'align' => 'right', 'nowrap' => 'nowrap');
      $table['rows'][] = array('data' => $data, 'valign' => 'top');
    }
  }
  else {
    $table['rows'][]['product']['cell'] = array('data' => t('This order contains no products.'), 'colspan' => 10);
  }

  $attributes = array('class' => 'order-pane-table');

  $form['table'] = array(
    '#value' => $table,
    '#theme' => 'tapir_table',
    '#attributes' => $attributes,
  );

  return $form;
}

function op_products_customer_table($form_state, $products) {
  $table = array();

  $fields = array();
  $fields[] = array(
    'id' => 'qty',
    'cell' => array('data' => t('Qty'), 'class' => 'text-right'),
    'weight' => 0,
  );
  $fields[] = array(
    'id' => 'product',
    'cell' => array('data' => t('Product'), 'width' => '80%'),
    'weight' => 1,
  );
  $fields[] = array(
    'id' => 'model',
    'cell' => array('data' => t('Model'), 'class' => 'text-center', 'nowrap' => 'nowrap'),
    'weight' => 2,
  );
  $fields[] = array(
    'id' => 'cost',
    'cell' => array('data' => t('Cost'), 'class' => 'text-right'),
    'weight' => 3,
  );
  $fields[] = array(
    'id' => 'price',
    'cell' => array('data' => t('Price'), 'class' => 'text-right'),
    'weight' => 4,
  );
  $fields[] = array(
    'id' => 'total',
    'cell' => array('data' => t('Total'), 'class' => 'text-right'),
    'weight' => 5,
  );

  $table['columns'] = $fields;

  if (is_array($products)) {
    foreach ($products as $product) {
      $data = array();
      $data['qty']['cell'] = array('data' => $product->qty .'x', 'align' => 'right');
      $options = $product->data['attributes'];
      $option_rows = array();
      if (is_array($options)) {
        foreach ($options as $attribute => $option) {
          $option_rows[] = t('@attribute: @option', array('@attribute' => $attribute, '@option' => $option));
        }
      }
      $data['product']['cell'] = check_plain($product->title) . theme('item_list', $option_rows, NULL, 'ul', array('class' => 'product-options'));
      $data['model']['cell'] = array('data' => check_plain($product->model), 'align' => 'center', 'nowrap' => 'nowrap');
      $data['cost']['cell'] = array('data' => uc_currency_format($product->cost), 'align' => 'right', 'nowrap' => 'nowrap');
      $data['price']['cell'] = array('data' => uc_currency_format($product->price), 'align' => 'right', 'nowrap' => 'nowrap');
      $data['total']['cell'] = array('data' => uc_currency_format($product->qty * $product->price), 'align' => 'right', 'nowrap' => 'nowrap');
      $table['rows'][] = array('data' => $data, 'valign' => 'top');
    }
  }
  else {
    $data['product']['cell'] = array('data' => t('This order contains no products.'), 'colspan' => 10);
    $table['rows'][] = $data;
  }

  $attributes = array('class' => 'order-pane-table');

  $form['table'] = array(
    '#value' => $table,
    '#theme' => 'tapir_table',
    '#attributes' => $attributes,
  );

  return $form;
}

function op_order_comments_view_table($form_state, $comments) {
  $table = array();

  $fields = array();
  $fields[] = array(
    'id' => 'date',
    'cell' => array('data' => t('Date'), 'class' => 'text-center'),
    'weight' => 0,
  );
  $fields[] = array(
    'id' => 'user',
    'cell' => t('User'),
    'weight' => 1,
  );
  $fields[] = array(
    'id' => 'notified',
    'cell' => t('Notified'),
    'weight' => 2,
  );
  $fields[] = array(
    'id' => 'status',
    'cell' => array('data' => t('Status'), 'class' => 'text-center'),
    'weight' => 3,
  );
  $fields[] = array(
    'id' => 'comment',
    'cell' => array('data' => t('Comment'), 'width' => '80%'),
    'weight' => 4,
  );

  $table['columns'] = $fields;

  if (is_array($comments)) {
    foreach ($comments as $comment) {
      $data = array();
      $data['date']['cell'] = array('data' => format_date($comment->created, 'custom', variable_get('uc_date_format_default', 'm/d/Y') .'<\b\r />g:i:s A'), 'align' => 'center', 'nowrap' => 'true');
      $data['user']['cell'] = array('data' => uc_get_initials($comment->uid), 'align' => 'center');
      $data['notified']['cell'] = array('data' => (($comment->notified == 1) ? '<img src="'. base_path() . drupal_get_path('module', 'uc_order') .'/images/true-icon.gif" />': '<img src="'. base_path() . drupal_get_path('module', 'uc_order') .'/images/false-icon.gif" />'), 'align' => 'center');
      $data['status']['cell'] = array('data' => $comment->title, 'align' => 'center');
      $data['comment']['cell'] = array('data' => filter_xss_admin($comment->message));
      $table['rows'][] = $data;
    }
  }
  else {
    $data['comment']['cell'] = array('data' => t('This order has no comments associated with it.'), 'colspan' => 10);
    $table['rows'][] = $data;
  }

  $attributes = array('class' => 'order-pane-table');

  $form['table'] = array(
    '#value' => $table,
    '#theme' => 'tapir_table',
    '#attributes' => $attributes,
  );

  return $form;
}

function op_admin_comments_view_table($form_state, $comments) {
  $table = array();

  $fields = array();
  $fields[] = array(
    'id' => 'date',
    'cell' => array('data' => t('Date'), 'class' => 'text-center'),
    'weight' => 0,
  );
  $fields[] = array(
    'id' => 'user',
    'cell' => array('data' => t('User'), 'class' => 'text-center'),
    'weight' => 1,
  );
  $fields[] = array(
    'id' => 'comment',
    'cell' => array('data' => t('Comment'), 'width' => '80%'),
    'weight' => 2,
  );

  $table['columns'] = $fields;

  if (is_array($comments)) {
    foreach ($comments as $comment) {
      $data = array();
      $data['date']['cell'] = array('data' => format_date($comment->created, 'custom', variable_get('uc_date_format_default', 'm/d/Y') .'<\b\r />g:i:s A'), 'align' => 'center', 'nowrap' => 'true', 'valign' => 'top');
      $data['user']['cell'] = array('data' => uc_get_initials($comment->uid), 'align' => 'center', 'valign' => 'top');
      $data['comment']['cell'] = array('data' => filter_xss_admin($comment->message), 'valign' => 'top');
      $table['rows'][] = $data;
    }
  }
  else {
    $data['comment']['cell'] = array('data' => t('This order has no admin comments associated with it.'), 'colspan' => 10);
    $table['rows'][] = $data;
  }

  $attributes = array('class' => 'order-pane-table');

  $form['table'] = array(
    '#value' => $table,
    '#theme' => 'tapir_table',
    '#attributes' => $attributes,
  );

  return $form;
}

/**
 * Build a list of order panes defined in the enabled modules.
 */
function _order_pane_list($view = 'view') {
  static $panes;

  if (count($panes) > 0 && $action !== 'rebuild') {
    return $panes;
  }

  $panes = module_invoke_all('order_pane', NULL);
  foreach ($panes as $i => $value) {
    $panes[$i]['enabled'] = variable_get('uc_order_pane_'. $panes[$i]['id'] .'_enabled', (!isset($panes[$i]['enabled']) ? TRUE : $panes[$i]['enabled']));
    $panes[$i]['weight'] = variable_get('uc_order_pane_'. $panes[$i]['id'] .'_weight_'. $view, (!isset($panes[$i]['weight']) ? 0 : $panes[$i]['weight']));
  }
  usort($panes, 'uc_weight_sort');

  return $panes;
}

/**
 * Return data from an order pane by pane ID and the array key.
 */
function _order_pane_data($pane_id, $key) {
  $panes = _order_pane_list();
  foreach ($panes as $pane) {
    if ($pane['id'] == $pane_id) {
      return $pane[$key];
    }
  }
}

/**
 * Call an order pane function with the argument passed by reference.
 */
function _call_order_pane_byref($func, $op, &$form) {
  return $func($op, $form);
}


<?php
// $Id: uc_payment_checkout_pane.inc,v 1.5 2008/07/10 12:40:57 islandusurper Exp $

function uc_checkout_pane_payment($op, &$arg1, $arg2) {
  switch ($op) {
    case 'view':
      if (variable_get('uc_payment_show_order_total_preview', TRUE)) {
        uc_add_js('misc/progress.js');
        uc_add_js("var def_payment_msg = '". addslashes(variable_get('uc_default_payment_msg', t('Continue with checkout to complete payment.'))) ."';", 'inline');

        $contents['current_total'] = array(
          '#type' => 'hidden',
          '#value' => $arg1->order_total > 0 ? $arg1->order_total : NULL,
        );
        $contents['shown_total'] = array(
          '#value' => '<div style="padding: .5em 1em; margin-bottom: 1em; border: dashed 1px #bbb;" id="line-items-div"><em>'. t('Javascript must be enabled to view the order total preview.') .'</em></div>',
          '#weight' => -20,
        );
        uc_add_js("\$(document).ready(function() { show_progressBar('#line-items-div'); } );", 'inline');
      }

      $description = t('Select a payment method from the following options.');

      $methods = _payment_method_list();

      foreach ($methods as $method) {
        if ($method['checkout']) {
          $options[$method['id']] = $method['title'];
        }
      }

      if (is_array($options)) {
        if (isset($_POST['panes']) && in_array($_POST['panes']['payment']['payment_method'], array_keys($options))) {
          $default = $_POST['panes']['payment']['payment_method'];
        }
        else {
          $default = (count($options) == 1 || !isset($arg1->payment_method)) ? key($options) : $arg1->payment_method;
        }
      }

      $contents['payment_method'] = array(
        '#type' => 'radios',
        '#title' => t('Payment method'),
        '#options' => $options,
        '#default_value' => $default,
        '#disabled' => count($options) == 1 ? TRUE : FALSE,
        '#required' => TRUE,
        '#attributes' => array('onclick' => "get_payment_details('cart/checkout/payment_details/' + this.value);"),
        '#theme' => 'uc_payment_method_select',
      );
      $contents['details'] = array(
        '#value' => '<div id="payment_details" class="solid-border display-none"></div>',
      );

      return array('description' => $description, 'contents' => $contents);

    case 'process':
      $arg1->payment_method = $arg2['payment_method'];
      $func = _payment_method_data($arg1->payment_method, 'callback');
      if (function_exists($func)) {
        $result = $func('cart-process', $arg1);
        if ($result === FALSE) {
          return FALSE;
        }
      }
      return TRUE;

    case 'review':
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
      foreach ($line_items as $line_item) {
        $review[] = array('title' => $line_item['title'], 'data' => uc_currency_format($line_item['amount']));
      }
      $review_data = _payment_method_data($arg1->payment_method, 'review');
      if (empty($review_data)) {
        $review_data = _payment_method_data($arg1->payment_method, 'name');
      }
      $review[] = array('border' => 'top', 'title' => t('Paying by'), 'data' => $review_data);
      $func = _payment_method_data($arg1->payment_method, 'callback');
      if (function_exists($func)) {
        $result = $func('cart-review', $arg1);
        if (is_array($result)) {
          $review = array_merge($review, $result);
        }
      }
      return $review;

    case 'settings':
      $form['uc_payment_show_order_total_preview'] = array(
        '#type' => 'checkbox',
        '#title' => t('Show the order total preview on the payment pane.'),
        '#default_value' => variable_get('uc_payment_show_order_total_preview', TRUE),
      );
      return $form;
  }
}

/**
 * We need a theme function for the radios element in case another module alters
 * the default or available payment methods... we need the JS to grab the right
 * default payment details.
 */
function theme_uc_payment_method_select($form) {
  if (empty($form['#options'])) {
    drupal_set_message(t('Checkout cannot be completed without any payment methods enabled. Please contact an administrator to resolve the issue.'), 'error');
    return;
  }

  // Perhaps instead this should be a normal JS function and we just print the
  // default payment method to a JS variable or use a selector.  -RS
  uc_add_js("\$(document).ready( function () { init_payment_details('". $form['#default_value'] ."'); } );", 'inline');

  return drupal_render($form);
}

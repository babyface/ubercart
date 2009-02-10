<?php
// $Id: ca.ca.inc,v 1.1.2.3 2009/02/10 21:58:14 islandusurper Exp $

/**
 * @file: This file includes some generic conditions and actions.
 */


/*******************************************************************************
 * Conditional Actions Hooks
 ******************************************************************************/

/**
 * Implementation of hook_ca_entity().
 *
 * Demonstrates defining a data object that may be passed in to ca_pull_trigger
 * and mapped to a predicate's arguments.
 */
function ca_ca_entity() {
  $entities = array();

  $entities['user'] = array(
    '#title' => t('Drupal user'),
    '#type' => 'object',
    '#load' => 'user_load',
    '#save' => 'user_save',
  );

  return $entities;
}

/**
 * Implementation of hook_ca_condition().
 */
function ca_ca_condition() {
  $conditions = array();

  $conditions['ca_condition_date'] = array(
    '#title' => t('Check the current date'),
    '#description' => t('Used to determine if the action should be performed on the current date.'),
    '#category' => t('Drupal'),
    '#callback' => 'ca_condition_date',
    '#arguments' => array(),
  );

  return $conditions;
}

/**
 * Implementation of hook_ca_action().
 *
 * Demonstrates defining an action for predicates to use; primarily specifies a
 * callback function to perform the action and an array that specifies arguments
 * and their data types.
 */
function ca_ca_action() {
  $actions['ca_drupal_set_message'] = array(
    '#title' => t('Display a message to the user'),
    '#category' => t('Drupal'),
    '#callback' => 'ca_action_drupal_set_message',
    '#arguments' => array(),
  );

  return $actions;
}

function ca_condition_date($settings) {
  $set_date_start = gmmktime(0, 0, 0, $settings['date']['month'], $settings['date']['day'], $settings['date']['year']);
  $set_date_end = gmmktime(23, 59, 59, $settings['date']['month'], $settings['date']['day'], $settings['date']['year']);

  $curr_time = time();

  switch ($settings['operator']) {
    case 'before':
      return $curr_time < $set_date_start;
    case 'only':
      return ($set_date_start < $curr_time) && ($curr_time < $set_date_end);
    case 'after':
      return $set_date_end < $curr_time;
    default:
      return FALSE;
  }
}

function ca_condition_date_form($form_state, $settings = array()) {
  $form = array();

  $form['operator'] = array(
    '#type' => 'radios',
    '#title' => t('Comparison'),
    '#options' => array(
      'before' => t('Before'),
      'only' => t('Only'),
      'after' => t('After'),
    ),
    '#default_value' => $settings['operator'],
    '#description' => t('Example: "The current date is before the date below."'),
  );

  $form['date'] = array(
    '#type' => 'date',
    '#title' => t('Date'),
    '#default_value' => $settings['date'],
    '#description' => t('When the predicate is evaluated, the current date is compared to this date.'),
  );

  return $form;
}

// Display a message to the user.
function ca_action_drupal_set_message($settings) {
  // Get the specified input format or use the default.
  /*if (!empty($settings['message_format'])) {
    $format = $settings['message_format'];
  }
  else {
    $format = FILTER_FORMAT_DEFAULT;
  }*/

  // Filter the text using the format.
  // $message = check_markup($settings['message_text'], $format, FALSE);
  $message = check_plain($settings['message_text']);

  // Return if there's nothing to display.
  if (empty($message)) {
    return;
  }

  // Make sure we have a valid message type.
  if ($settings['message_type'] == 'error') {
    $type = $settings['message_type'];
  }
  else {
    $type = 'status';
  }

  // Output the message using the Drupal message API.
  drupal_set_message($message, $type);
}


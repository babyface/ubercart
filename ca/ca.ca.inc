<?php
// $Id: ca.ca.inc,v 1.1.2.2 2008/11/07 21:13:20 islandusurper Exp $

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


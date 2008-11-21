<?php
// $Id: tapir.inc,v 1.1.2.1 2008/11/21 21:17:30 islandusurper Exp $

/**
 * @file
 * Contains the TAPIr code rolled into Ubercart core from Drupal contrib.
 */


/**
 * Loads and renders a table for display through the Forms API.
 *
 * @param $table_id
 *   The string identifying the table you want to display, like a form ID.
 * @param ...
 *   Any additional arguments will be passed on to the table builder function
 *     specified as the $table_id.
 * @return
 *   The rendered table.
 */
function tapir_get_table($table_id) {
  // Get any additional arguments.
  $args = func_get_args();

  // Replace the first argument with a basic $form_state array.
  $args[0] = array('storage' => NULL, 'submitted' => FALSE, 'post' => $_POST);

  // Add the table ID to the argument array.
  array_unshift($args, $table_id);

  // Retrieve the table data through FAPI and allow modules to alter it.
  $form = call_user_func_array('drupal_retrieve_form', $args);
  drupal_prepare_form($table_id, $form, $args[1]);

  // Return the rendered version of the table.
  return drupal_render_form($table_id, $form);
}

/**
 * Themes form data into a table for display.
 *
 * @param $form
 *   The array of form information needing to be rendered into the table.
 * @return
 *   The table output rendered in HTML.
 * @ingroup themeable
 */
function theme_tapir_table($form) {
  $header = array();
  $rows = array();

  // First sort the columns by weight.
  uasort($form['#value']['columns'], 'uc_weight_sort');

  // Loop through the columns and create the header array.
  foreach ($form['#value']['columns'] as $col_id => $col_data) {
    // Add the cell if available.
    if ($col_data['access'] !== FALSE) {
      $header[] = $col_data['cell'];
    }
  }

  // Loop through the row data and create rows with the data in the right order.
  foreach ((array) $form['#value']['rows'] as $data) {
    $attributes = array();
    $row = array();

    // If the row combines cell data with attributes...
    if (isset($data['data'])) {
      // Set the attributes array to be everything in the row array except the
      // data array.
      $attributes = $data;
      unset($attributes['data']);

      // Filter the data array down to just the cell data.
      $data = $data['data'];
    }

    // Loop through each column in the header.
    foreach ($form['#value']['columns'] as $col_id => $col_data) {
      // If this row defines cell data for the current column...
      if ($col_data['access'] !== FALSE && isset($data[$col_id])) {
        // Add it to the row array.
        $row[] = $data[$col_id]['cell'];
      }
    }

    // Merge the row data into a single row array along with the attributes.
    $row = array_merge(array('data' => $row), $attributes);

    // Add the current row to the table rows array.
    $rows[] = $row;
  }

  // Return the rendered table.
  return theme('table', $header, $rows, is_array($form['#attributes']) ? $form['#attributes'] : array(), $form['#title']);
}

/**
 * Builds an ORDER BY clause for a table's SELECT query based on the table's
 *   columns.
 *
 * @param $columns
 *   The column array from a TAPIr table array.
 * @return
 *   The result of a tablesort_sql() call on the header info for this table.
 */
function tapirsort_sql($columns) {
  $header = array();

  // Loop through the columns array...
  foreach ($columns as $column) {
    // And build a header array based on column information.
    $header[] = $column['cell'];
  }

  // Return the result of a tablesort on our header.
  return tablesort_sql($header);
}

function uc_store_form_alter(&$form, &$form_state, $form_id) {
  if ($form_id == 'uc_product_table') {
    unset($form['table']['#value']['columns']['list_price']);
  }
}


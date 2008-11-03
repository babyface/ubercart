<?php
// $Id: uc_product.admin.inc,v 1.1.2.3 2008/11/03 21:33:53 islandusurper Exp $

/**
 * @file
 * Product administration menu items.
 *
 * Coded by: Lyle Mantooth
 * Product Features by: Ryan Szrama
 */

/**
 * List all products.
 */
function uc_product_administration() {
  $header = array();
  $tbl_columns = uc_product_table_header();
  foreach ($tbl_columns as $column) {
    $header[] = $column['cell'];
  }
  $order = substr(tablesort_sql($header), 10);
  if (empty($_REQUEST['order'])) {
    $order = 'p.ordering, n.title, n.nid';
  }

  $nids = array();
  // Display 50 products per page.
  $result = pager_query("SELECT n.nid FROM {node} AS n INNER JOIN {uc_products} AS p ON n.vid = p.vid ORDER BY ". $order, 50, 0, NULL);
  while ($product = db_fetch_object($result)) {
    $nids[] = $product->nid;
  }

  // Setup the arguments to pass to the table builder function.
  $args = array(
    'nids' => $nids,
  );

  // Display the product table and pager if necessary.
  $output .= tapir_get_table('uc_product_table', $args) . theme('pager');

  return $output;
}

/**
 * Display a list of product classes.
 */
function uc_product_class_default() {
  $result = db_query("SELECT * FROM {uc_product_classes}");
  $header = array(t('Class ID'), t('Name'), t('Description'), t('Operations'));
  $rows = array();
  while ($class = db_fetch_object($result)) {
    $ops = array(
      l(t('edit'), 'admin/store/products/classes/'. $class->pcid .'/edit'),
      l(t('delete'), 'admin/store/products/classes/'. $class->pcid .'/delete'),
    );
    $rows[] = array(
      $class->pcid,
      $class->name,
      $class->description,
      implode(' ', $ops),
    );
  }
  if (count($rows) == 0) {
    $rows[] = array(array('data' => t('No product classes have been defined yet.'), 'colspan' => '5'));
  }

  $output = theme('table', $header, $rows);
  $output .= '<h2>'. t('Add a class') .'</h2>';
  $output .= drupal_get_form('uc_product_class_form');

  return $output;
}

/**
 * Display overview of product settings.
 *
 * @see uc_product_settings_form
 * @see uc_product_field_settings_form
 */
function uc_product_settings_overview() {

  // Add some Ubercart specific JS for modifying summaries.
  drupal_add_js(drupal_get_path('module', 'uc_store') .'/includes/summaries.js');
  drupal_add_js(array('editIconPath' => base_path() . drupal_get_path('module', 'uc_store') .'/images/order_edit.gif'), 'setting');

  // Load the form summaries for pages beneath this path.
  $summaries = summarize_child_form_pages('admin/store/settings/products/edit');

  // Theme it all up in a summaries overview.
  return theme('summary_overview', $summaries);

}

/**
 * Form to change product settings.
 *
 * @ingroup forms
 * @see uc_product_settings_overview
 */
function uc_product_settings_form() {
  $form['uc_product_nodes_per_page'] = array(
    '#type' => 'select',
    '#title' => t('Products per page'),
    '#description' => t('The number of products per page in a list.'),
    '#default_value' => variable_get('uc_product_nodes_per_page', 10),
    '#options' => drupal_map_assoc(uc_range(10, 100, 10)),
    '#summary' => t('Showing %products products per page.', array('%products' => variable_get('uc_product_nodes_per_page', 10))),
    '#summary arguments' => array(FALSE),
  );

  $form['uc_product_add_to_cart_qty'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display an optional quantity field in the <em>Add to Cart</em> form.'),
    '#default_value' => variable_get('uc_product_add_to_cart_qty', FALSE),
    '#summary arguments' => array(
      t('The Quantity field in the <em>Add to Cart</em> form is enabled.'),
      t('The Quantity field in the <em>Add to Cart</em> form is disabled.'),
    ),
  );
  $form['uc_product_add_to_cart_teaser'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable <em>Add to cart</em> forms in product node teasers.'),
    '#default_value' => variable_get('uc_product_add_to_cart_teaser', TRUE),
    '#summary arguments' => array(
      t('The <em>Add to Cart</em> form is enabled in product teasers.'),
      t('The <em>Add to Cart</em> form is disabled in product teasers.'),
    ),
  );

  $form['uc_add_to_cart_text'] = array(
    '#type' => 'fieldset',
    '#title' => t('<em>Add to cart</em> button text'),
    '#description' => t('Use the textboxes to adjust the text of the submit button for <em>Add to Cart</em> forms in various places on the site.'),
    '#collapsed' => FALSE,
    '#collapsible' => FALSE,
    '#summary callback' => '_uc_submit_text_summarize',
  );
  $form['uc_add_to_cart_text']['uc_teaser_add_to_cart_text'] = array(
    '#type' => 'textfield',
    '#title' => t('Teaser forms'),
    '#description' => t('For the form displayed on teasers and catalog pages.'),
    '#default_value' => variable_get('uc_teaser_add_to_cart_text', t('Add to cart')),
    '#summary' => t('Teaser and catalog pages: %text', array('%text' => variable_get('uc_teaser_add_to_cart_text', t('Add to cart')))),
    '#summary arguments' => array(FALSE),
  );
  $form['uc_add_to_cart_text']['uc_product_add_to_cart_text'] = array(
    '#type' => 'textfield',
    '#title' => t('Product view'),
    '#description' => t('For the form displayed on the product view page.'),
    '#default_value' => variable_get('uc_product_add_to_cart_text', t('Add to cart')),
    '#summary' => t('Product view pages: %text', array('%text' => variable_get('uc_teaser_add_to_cart_text', t('Add to cart')))),
    '#summary arguments' => array(FALSE),
  );

  return system_settings_form($form);
}

function _uc_submit_text_summarize($form) {

  $item = summarize_form($form);

  $items[] = array(
    'data' => t('<em>Add to Cart</em> submit button text:'),
    'children' => $item,
  );

  return $items;

}

/**
 * Allows store administrators to control what product information is relavent to their store.
 *
 * @ingroup forms
 * @see uc_product_settings_overview
 * @see theme_uc_product_field_settings_form
 * @see uc_product_field_settings_form_submit
 */
function uc_product_field_settings_form() {
  $form = array();

  $options = array(
    'model' => t('Model'),
    'image' => t('Image'),
    'display_price' => t('Display price'),
    'list_price' => t('List price'),
    'cost' => t("Cost (seen only by 'administer products' permission)"),
    'sell_price' => t('Sell price'),
    'weight' => t('Weight'),
    'dimensions' => t('Dimensions'),
    'add_to_cart' => variable_get('uc_product_add_to_cart_text', t('Add to cart')),
  );
  $enabled = variable_get('uc_product_field_enabled', array(
    'model' => 'model',
    'image' => 'image',
    'display_price' => 'display_price',
    'sell_price' => 'sell_price',
    'add_to_cart' => 'add_to_cart',
  ));
  $weight = variable_get('uc_product_field_weight', array(
    'image' => -2,
    'display_price' -1,
    'model' => 0,
    'list_price' => 2,
    'cost' => 3,
    'sell_price' => 4,
    'weight' => 5,
    'dimensions' => 6,
    'add_to_cart' => 10,
  ));
  $fields = array();
  foreach ($options as $field => $value) {
    $fields[$field] = array(
      'title' => $value,
      'enabled' => $enabled[$field],
      'weight' => $weight[$field],
    );
  }
  uasort($fields, 'uc_weight_sort');
  $form['fields'] = array('#tree' => TRUE);
  $form['fields']['#summary callback'] = '_uc_product_fields_summarize';
  $form['fields']['#summary arguments'] = array($options, $enabled, $weight);

  foreach ($fields as $label => $field) {
    $form['fields'][$label]['enabled'] = array('#type' => 'checkbox',
      '#default_value' => $field['enabled'],
    );
    $form['fields'][$label]['title'] = array('#type' => 'markup',
      '#value' => $field['title'],
    );
    $form['fields'][$label]['weight'] = array('#type' => 'weight',
      '#delta' => 10,
      '#default_value' => $field['weight'],
    );
  }

  $form['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );
  $form['buttons']['reset'] = array('#type' => 'submit',
    '#value' => t('Reset to defaults'),
    '#submit' => array('uc_product_field_settings_form_reset'),
  );

  return $form;
}

function _uc_product_fields_summarize($form, $options, $enabled, $weight) {

  $fields = array();
  foreach ($options as $field => $label) {
    if ($enabled[$field]) {
      $fields[$field] = array('enabled' => $enabled[$field],
        'weight' => $weight[$field],
        'data' => $label,
      );
    }
  }
  uasort($fields, 'uc_weight_sort');

  foreach ($fields as $field) {
    $item[] = $field['data'];
  }

  $items[] = array(
    'data' => t('Displayed product fields:'),
    'children' => $item,
  );

  return $items;
}

/**
 * Display the product field settings form.
 *
 * @ingroup themeable
 * @see uc_product_field_settings_form
 */
function theme_uc_product_field_settings_form($form) {
  $header = array(t('Enable'), t('Product field'), t('Weight'));
  $rows = array();
  foreach (element_children($form['fields']) as $field) {
    $row = array();
    $row[] = drupal_render($form['fields'][$field]['enabled']);
    $row[] = drupal_render($form['fields'][$field]['title']);
    $row[] = drupal_render($form['fields'][$field]['weight']);
    $rows[] = $row;
  }
  $output .= theme('table', $header, $rows);

  $output .= drupal_render($form);
  return $output;
}

function uc_product_field_settings_form_reset($form, &$form_state) {
  variable_del('uc_product_field_enabled');
  variable_del('uc_product_field_weight');

  drupal_set_message(t('The configuration options have been reset to their default values.'));
}

/**
 * Submit handler for uc_product_field_settings_form.
 */
function uc_product_field_settings_form_submit($form, &$form_state) {
  $enabled = array();
  $weight = array();
  foreach ($form_state['values']['fields'] as $id => $field) {
    $enabled[$id] = $field['enabled'];
    $weight[$id] = $field['weight'];
  }
  variable_set('uc_product_field_enabled', $enabled);
  variable_set('uc_product_field_weight', $weight);

  drupal_set_message(t('The configuration options have been saved.'));
}

/**
 * Display the settings form for all product features.
 *
 * @ingroup forms
 */
function uc_product_feature_settings_form() {
  $titles = array();
  $features = module_invoke_all('product_feature');
  foreach ($features as $feature) {
    $titles[] = $feature['title'];
  }
  if (empty($titles)) {
    $titles[] = '<em>'. t('No product features found.') .'</em>';
  }
  $form['features_list'] = array(
    '#value' => '<div><b>'. t('The following product features are enabled')
               .':</b><br />'. implode(', ', $titles) .'</div><br />',
    '#summary callback' => '_uc_product_features_summarize',
    '#summary arguments' => array($titles),
  );

  foreach ($features as $feature) {
    if (function_exists($feature['settings'])) {
      $form[$feature['id']] = array(
        '#type' => 'fieldset',
        '#title' => t('!feature settings', array('!feature' => $feature['title'])),
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#summary callback' => 'summarize_null',
      );
      $form[$feature['id']] = array_merge($form[$feature['id']], $feature['settings']());
    }
  }

  return system_settings_form($form);
}

function _uc_product_features_summarize($form, $titles) {

  foreach ($titles as $title) {
    $item[] = $title;
  }

  $items[] = array(
    'data' => t('The following features are enabled:'),
    'children' => $item,
  );

  return $items;

}

/**
 * Displays the project features tab on a product node edit form.
 */
function uc_product_features($node) {
  drupal_set_title(check_plain($node->title));

  if (arg(4)) {
    // First check to see if we're trying to remove a feature.
    if (intval(arg(5)) > 0 && arg(6) == 'delete') {
      $result = db_query("SELECT * FROM {uc_product_features} WHERE pfid = %d AND fid = '%s'", intval(arg(5)), arg(4));
      if ($feature = db_fetch_array($result)) {
        // If the user confirmed the delete, process it!
        if ($_POST['pf_delete']) {
          // Call the delete function for this product feature if it exists.
          $func = uc_product_feature_data($feature['fid'], 'delete');
          if (function_exists($func)) {
            $func($feature);
          }

          // Remove the product feature data from the database.
          db_query("DELETE FROM {uc_product_features} WHERE pfid = %d", intval(arg(5)));

          drupal_set_message(t('The product feature has been deleted.'));
          drupal_goto('node/'. arg(1) .'/edit/features');
        }

        // Show the confirmation form for deleting this feature.
        $question = $node->title;
        $description = t('Are you sure you wish to delete this %feature?', array('%feature' => uc_product_feature_data($feature['fid'], 'title')))
                      .'<div><b>'. t('Description') .':</b><br />'. $feature['description'] .'</div><br />';
        return drupal_get_form('confirm_form', $question, 'node/'. arg(1) .'/edit/features', $description, t('Delete'), t('Cancel'), 'pf_delete');
      }
      else {
        drupal_set_message(t("That product feature doesn't exist."), 'error');
        drupal_goto('node/'. arg(1) .'/edit/features');
      }
    }

    // Handle adding or editing product features.
    $func = uc_product_feature_data(arg(4), 'callback');
    if (function_exists($func)) {
      if (arg(5) == 'add') {
        $output = drupal_get_form($func, $node, array());
      }
      elseif (intval(arg(5)) > 0) {
        $result = db_query("SELECT * FROM {uc_product_features} WHERE pfid = %d AND fid = '%s'", intval(arg(5)), arg(4));
        if ($feature = db_fetch_array($result)) {
          $output = drupal_get_form($func, $node, $feature);
        }
      }
    }
    else {
      drupal_set_message(t('Error: Attempted to add a non-existent product feature type.'), 'error');
      drupal_goto('node/'. $node->nid .'/edit/features');
    }

    if (empty($output)) {
      drupal_set_message(t('Error: No form data was returned for that operation.'), 'error');
      drupal_goto('node/'. $node->nid .'/edit/features');
    }

    return $output;
  }

  $header = array(t('Type'), t('Description'), t('Operations'));

  $result = db_query("SELECT * FROM {uc_product_features} WHERE nid = %d ORDER BY pfid ASC", $node->nid);
  while ($feature = db_fetch_object($result)) {
    $operations = array(
      l(t('edit'), 'node/'. $node->nid .'/edit/features/'. $feature->fid .'/'. $feature->pfid),
      l(t('delete'), 'node/'. $node->nid .'/edit/features/'. $feature->fid .'/'. $feature->pfid .'/delete'),
    );
    $rows[] = array(
      'data' => array(
        array('data' => uc_product_feature_data($feature->fid, 'title'), 'nowrap' => 'nowrap'),
        array('data' => $feature->description, 'width' => '100%'),
        array('data' => implode(' ', $operations), 'nowrap' => 'nowrap'),
      ),
      'valign' => 'top',
    );
  }

  if (empty($rows)) {
    $rows[] = array(
      array('data' => t('No features found for this product.'), 'colspan' => 3),
    );
  }

  $output = theme('table', $header, $rows)
          . drupal_get_form('uc_product_feature_add_form');

  return $output;
}

/**
 * Add the form for adding a product feature to the features tab.
 *
 * @ingroup forms
 * @see theme_uc_product_feature_add_form
 */
function uc_product_feature_add_form() {
  foreach (module_invoke_all('product_feature') as $feature) {
    $options[$feature['id']] = $feature['title'];
  }
  ksort($options);

  $form['feature'] = array(
    '#type' => 'select',
    '#title' => t('Add a new feature'),
    '#options' => $options,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add'),
  );

  return $form;
}

/**
 * @ingroup themeable
 */
function theme_uc_product_feature_add_form($form) {
  $output = '<table class="add-feature"><tr><td>'. drupal_render($form) .'</td></tr></table>';
  return $output;
}

/**
 * Submit handler for uc_product_feature_add_form_submit.
 */
function uc_product_feature_add_form_submit($form, &$form_state) {
  $form_state['redirect'] = 'node/'. arg(1) .'/edit/features/'. $form_state['values']['feature'] .'/add';
}

/**
 * Set up imagefield and imagecache for products.
 *
 * @param $checks A bitmap describing the state of the image modules. Four
 *   checks are made to fill this bitmap: an imagefield exists, an imagefield
 *   is attached to product, the imagecache presets exist, and each preset has
 *   at least one action.
 * @see uc_product_store_status
 */
function uc_product_image_defaults() {
  uc_product_add_default_image_field();

  $presets = array('product', 'product_list', 'uc_thumbnail');
  if (module_exists('uc_catalog')) {
    $presets[] = 'uc_category';
  }
  if (module_exists('uc_cart')) {
    $presets[] = 'cart';
  }

  foreach ($presets as $preset_name) {
    $preset = imagecache_preset_by_name($preset_name);
    if (empty($preset)) {
      // No preset of that name, so make a new one.
      $preset = imagecache_preset_save(array('presetname' => $preset_name));
    }

    $actions = imagecache_preset_actions($preset);
    if (empty($actions)) {
      // The preset has no actions, so scale the image.
      $action = array(
        'action' => 'imagecache_scale',
        'presetid' => $preset['presetid'],
        'weight' => 0,
        'data' => array(
          'upscale' => 0,
        ),
      );
      switch ($preset_name) {
        case 'product':
        case 'product_list':
          $action['data']['width'] = '100';
          $action['data']['height'] = '100';
        break;
        case 'cart':
          $action['data']['width'] = '50';
          $action['data']['height'] = '50';
        break;
        case 'uc_category':
          $action['data']['width'] = '96';
          $action['data']['height'] = '96';
        break;
        case 'uc_thumbnail':
          $action['data']['width'] = '35';
          $action['data']['height'] = '35';
        break;
        default:
          // Do not save the action and get the next preset.
          // Continue thinks swtich() is a loop, so continue it and the
          // foreach().
        continue 2;
      }
      imagecache_action_save($action);
    }
  }
  drupal_goto('admin/store');
}

/**
 *  Form builder for product classes.
 *
 * @ingroup forms
 * @see uc_product_class_form_submit
 */
function uc_product_class_form($form_state, $class = NULL) {
  if ($class) {
    drupal_set_title(check_plain($class->name));
    $form['pcid'] = array('#type' => 'hidden', '#value' => $class->pcid);
  }
  else {
    $form['pcid'] = array('#type' => 'textfield',
      '#title' => t('Class ID'),
      '#required' => TRUE,
      '#max_length' => 32,
      '#description' => t('The machine-readable name of this content type. This text will be used for constructing the URL of the <em>create content</em> page for this content type. This name may consist only of lowercase letters, numbers, and underscores. Dashes are not allowed. Underscores will be converted into dashes when constructing the URL of the <em>create content</em> page. This name must be unique to this content type.'),
    );
  }
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Class name'),
    '#default_value' => $class->name,
    '#required' => TRUE,
  );
  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => t('This text describes the content type created for this product class to administrators.'),
    '#default_value' => $class->description,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  return $form;
}

/**
 * Submit handler for uc_product_class_form.
 */
function uc_product_class_form_submit($form_id, &$form_state) {
  if (uc_product_class_load($form_state['values']['pcid'])) {
    $is_new = FALSE;
    $pcid = $form_state['values']['pcid'];
    db_query("UPDATE {uc_product_classes} SET name = '%s', description = '%s' WHERE pcid = '%s'", $form_state['values']['name'], $form_state['values']['description'], $pcid);
    uc_product_node_info(TRUE);
    module_invoke_all('product_class', $pcid, 'update');
  }
  else {
    $is_new = TRUE;
    // Convert whitespace to underscores, and remove other non-alphanumeric characters.
    $pcid = preg_replace(array('/\s+/', '/\W/'), array('_', ''), strtolower($form_state['values']['pcid']));
    // Search for class ids of the form $pcid_# where # is any character.
    $similar = db_result(db_query("SELECT COUNT(pcid) FROM {uc_product_classes} WHERE pcid LIKE '%s\\__' OR pcid LIKE '%s' UNION SELECT type FROM {node_type} WHERE type = '%s' AND custom = 0", $pcid, $pcid, $pcid));
    if ($similar) {
      $pcid = $pcid .'_'. $similar;
    }
    db_query("INSERT INTO {uc_product_classes} (pcid, name, description) VALUES ('%s', '%s', '%s')", $pcid, $form_state['values']['name'], $form_state['values']['description']);
    uc_product_node_info(TRUE);
    variable_set('node_options_'. $pcid, variable_get('node_options_product', array('status', 'promote')));
    if (module_exists('comment')) {
      variable_set('comment_'. $pcid, variable_get('comment_product', COMMENT_NODE_READ_WRITE));
    }
    module_invoke_all('product_class', $pcid, 'insert');
  }

  node_types_rebuild();
  if ($is_new && module_exists('filefield')) {
    uc_product_add_default_image_field($pcid);
  }
  menu_rebuild();

  $form_state['redirect'] = 'admin/store/products/classes';
}

/**
 * Confirm the deletion of a product class.
 *
 * @see uc_product_class_delete_confirm_submit
 */
function uc_product_class_delete_confirm($form_state, $class) {
  $result = db_query("SELECT COUNT(*) AS number FROM {node} WHERE type = '%s'", $class->pcid);
  $products = db_fetch_object($result);
  $form['pcid'] = array('#type' => 'value', '#value' => $class->pcid);
  $form['#redirect'] = 'admin/store/products/classes';
  $output = confirm_form($form, t('Be very sure you want to delete the %type product class. It will become a standard node type.', array('%type' => $class->pcid)), 'admin/store/products/classes',
    format_plural($products->number, 'There is @count node of this type.', 'There are @count nodes of this type.'),
    t('Continue'), t('Cancel'));

  return $output;
}

/**
 * Submit handler for uc_product_class_delete_confirm.
 */
function uc_product_class_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $type = node_get_types('type', $form_state['values']['pcid']);
    $type->module = 'node';
    $type->custom = 1;
    node_type_save($type);

    db_query("DELETE FROM {uc_product_classes} WHERE pcid = '%s'", $form_state['values']['pcid']);
    module_invoke_all('product_class', $form_state['values']['pcid'], 'delete');
    uc_product_node_info(TRUE);
    node_types_rebuild();
    menu_rebuild();

    drupal_set_message(t('Product class %type deleted.', array('%type' => $form_state['values']['pcid'])));
  }
}


<?php
// $Id: uc_catalog.admin.inc,v 1.1.2.3 2008/11/03 21:33:51 islandusurper Exp $

/**
 * @file
 * Catalog administration menu items.
 *
 */

/**
 * Display an overview of all catalog settings.
 */
function uc_catalog_settings_overview() {
  // Add some Ubercart specific JS for modifying summaries.
  drupal_add_js(drupal_get_path('module', 'uc_store') .'/includes/summaries.js');
  drupal_add_js(array('editIconPath' => base_path() . drupal_get_path('module', 'uc_store') .'/images/order_edit.gif'), 'setting');

  // Load the form summaries for pages beneath this path.
  $summaries = summarize_child_form_pages('admin/store/settings/catalog/edit');

  // Theme it all up in a summaries overview.
  return theme('summary_overview', $summaries);
}

/**
 * Catalog settings form.
 *
 * Configure the display of the catalog breadcrumb.
 *
 * @ingroup forms
 */
function uc_catalog_settings_form() {
  $form = array();

  $vid = variable_get('uc_catalog_vid', NULL);
  if ($vid) {
    $catalog = taxonomy_vocabulary_load($vid);

    $form['catalog-vid'] = array(
      '#prefix' => '<div>',

      '#summary' => t('Vocabulary %name is set as the product catalog.', array('%name' => $catalog->name)) .' '. l(t('View the catalog here.'), 'admin/content/taxonomy/'. $vid),

      '#suffix' => '</div>',
    );
    $form['catalog-vid']['#value'] = $form['catalog-vid']['#summary'];
  }

  $vocabs = array();
  $vocabularies = taxonomy_get_vocabularies();
  foreach ($vocabularies as $vid => $vocabulary) {
    $vocabs[$vid] = $vocabulary->name;
  }

  // JTR - catalog-top-level sub-textfield
  $form['catalog-top-level'] = array('#type' => 'fieldset',
    '#title' => t('Catalog top level'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#summary callback' => 'summarize_form',
    '#attributes' => array('class' => 'catalog-top-level'),
  );
  $form['catalog-top-level']['uc_catalog_vid'] = array('#type' => 'select',
    '#title' => t('Catalog vocabulary'),
    '#description' => t("The taxonomy vocabulary that will be considered the product catalog."),
    '#default_value' => variable_get('uc_catalog_vid', 0),
    '#summary callback' => 'summarize_null',
    '#options' => $vocabs,
  );
  $form['catalog-top-level']['uc_catalog_breadcrumb'] = array('#type' => 'checkbox',
    '#title' => t('Display the catalog breadcrumb'),
    '#summary callback' => 'summarize_checkbox',
    '#summary arguments' => array(
      t('Catalog breadcrumb is being displayed.'),
      t('Catalog breadcrumb is hidden.'),
    ),
    '#default_value' => variable_get('uc_catalog_breadcrumb', TRUE),
  );
  $form['catalog-top-level']['uc_catalog_breadcrumb_nodecount'] = array('#type' => 'checkbox',
    '#title' => t('Display node counts in the catalog breadcrumb'),
    '#summary callback' => 'summarize_checkbox',
    '#summary arguments' => array(
      t('Node count is being displayed in the catalog breadcrumb.'),
      t('Node count is not being displayed in the catalog breadcrumb.'),
    ),
    '#default_value' => variable_get('uc_catalog_breadcrumb_nodecount', FALSE),
  );
  $form['catalog-top-level']['uc_catalog_show_subcategories'] = array('#type' => 'checkbox',
    '#title' => t('Display subcategories in the catalog view'),
    '#summary callback' => 'summarize_checkbox',
    '#summary arguments' => array(
      t('The catalog view is displaying subcategories.'),
      t('The catalog view is not displaying subcategories.'),
    ),
    '#default_value' => variable_get('uc_catalog_show_subcategories', TRUE),
  );
  $form['catalog-top-level']['uc_catalog_category_columns'] = array('#type' => 'select',
    '#title' => t('Number of columns in the grid of categories'),
    '#default_value' => variable_get('uc_catalog_category_columns', 3),
    '#summary' => t('Subcategories are being displayed in @columns columns.', array('@columns' => variable_get('uc_catalog_category_columns', 3))),
    '#options' => drupal_map_assoc(uc_range(1, 5)),
  );

  $form['catalog-products-list'] = array('#type' => 'fieldset',
    '#title' => t('Catalog products list'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#summary callback' => 'summarize_form',
    '#attributes' => array('class' => 'catalog-products-list'),
  );
  $form['catalog-products-list']['uc_product_nodes_per_page'] = array(
    '#type' => 'textfield',
    '#title' => t('Product nodes per page'),
    '#summary' => t('There are @nodes product nodes displayed per page.', array('@nodes' => variable_get('uc_product_nodes_per_page', 12))),
    '#default_value' => variable_get('uc_product_nodes_per_page', 12),
    '#description' => t("Determines how many products will be listed on every catalog category. Notice that if you are using grid display it must be multiple of the grid width value, otherwise the last row will not match."),
    '#size' => 2,
  );

  $form['block-display'] = array('#type' => 'fieldset',
    '#title' => t('Catalog block settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#summary callback' => '_uc_catalog_block_summarize',
    '#attributes' => array('class' => 'block-display'),
  );
  $form['block-display']['uc_catalog_block_title'] = array('#type' => 'checkbox',
    '#title' => t('Block title links to top-level catalog page'),
    '#summary callback' => 'summarize_checkbox',
    '#summary arguments' => array(
      t('Block title is pointing to the top-level catalog page.'),
      t('Block title is not pointing to the top-level catalog page.'),
    ),
    '#default_value' => variable_get('uc_catalog_block_title', TRUE),
    '#description' => t("If selected, the block title is a link, otherwise plain text."),
  );
  $form['block-display']['uc_catalog_expand_categories'] = array('#type' => 'checkbox',
    '#title' => t('Always expand categories in the catalog block'),
    '#summary callback' => 'summarize_checkbox',
    '#summary arguments' => array(
      t('Expanding categories in the catalog block.'),
      t('Not expanding categories in the catalog block.'),
    ),
    '#default_value' => variable_get('uc_catalog_expand_categories', FALSE),
  );
  $form['block-display']['uc_catalog_block_nodecount'] = array('#type' => 'checkbox',
    '#title' => t('Display node counts in the catalog block'),
    '#summary callback' => 'summarize_checkbox',
    '#summary arguments' => array(
      t('The number of nodes in a category are being shown in the catalog block.'),
      t('The number of nodes in a category are not being shown in the catalog block.'),
    ),
    '#default_value' => variable_get('uc_catalog_block_nodecount', TRUE),
  );

  return system_settings_form($form);
}

/**
 * Catalog settings form.
 *
 * Configure the display of the catalog breadcrumb.
 *
 * @ingroup forms
 */
function uc_catalog_grid_admin_form() {
  $form = array();

  // JTR - elements for products grid view administration
  $form['catalog-grid'] = array('#type' => 'fieldset',
    '#title' => t('Products grid display'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#summary callback' => 'summarize_form',
    '#weight' => -3,
    '#attributes' => array('class' => 'catalog-grid'),
  );
  $form['catalog-grid']['uc_catalog_grid_display'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display products in grid'),
    '#summary callback' => 'summarize_checkbox',
    '#summary arguments' => array(
      t('Products are displayed in a grid.'),
      t('Products are displayed on a list table.'),
    ),
    '#default_value' => variable_get('uc_catalog_grid_display', FALSE),
    '#description' => t("Select it if you want the products to be displayed on a grid instead of a list table."),
  );
  $form['catalog-grid']['uc_catalog_grid_display_width'] = array(
    '#type' => 'textfield',
    '#title' => t('Width of grid'),
    '#default_value' => variable_get('uc_catalog_grid_display_width', 3),
    '#summary' => t('The grid will be displayed in @columns columns', array('@columns' => variable_get('uc_catalog_grid_display_width', 3))),
    '#size' => 2,
  );
  $form['catalog-grid']['uc_catalog_grid_display_title'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display product title'),
    '#summary callback' => 'summarize_checkbox',
    '#summary arguments' => array(
      t('Every cell is displaying product titles.'),
      t('Every cell is hiding product titles.'),
    ),
    '#default_value' => variable_get('uc_catalog_grid_display_title', TRUE),
  );
  $form['catalog-grid']['uc_catalog_grid_display_model'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display product model (SKU)'),
    '#summary callback' => 'summarize_checkbox',
    '#summary arguments' => array(
      t('Every cell is displaying product model (SKU).'),
      t('Every cell is hiding product model (SKU).'),
    ),
    '#default_value' => variable_get('uc_catalog_grid_display_model', TRUE),
  );
  $form['catalog-grid']['uc_catalog_grid_display_sell_price'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display product sell price'),
    '#summary callback' => 'summarize_checkbox',
    '#summary arguments' => array(
      t('Every cell is displaying product sell price.'),
      t('Every cell is hiding product sell price.'),
    ),
    '#default_value' => variable_get('uc_catalog_grid_display_sell_price', TRUE),
  );
  $form['catalog-grid']['uc_catalog_grid_display_add_to_cart'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display product add To cart'),
    '#summary callback' => 'summarize_checkbox',
    '#summary arguments' => array(
      t('Every cell is displaying product "Add to cart".'),
      t('Every cell is hiding product "Add to cart".'),
    ),
    '#default_value' => variable_get('uc_catalog_grid_display_add_to_cart', TRUE),
  );
  $form['catalog-grid']['uc_catalog_grid_display_attributes'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display attributes'),
    '#summary callback' => 'summarize_checkbox',
    '#summary arguments' => array(
      t('Every cell is displaying attributes when "Add to cart button" is selected.'),
      t('Every cell is hiding attributes when "Add to cart button" is selected.'),
    ),
    '#default_value' => variable_get('uc_catalog_grid_display_attributes', TRUE),
    '#description' => t('Allows displaying attributes when "Add to cart button" is selected.'),
  );

  return system_settings_form($form);
}

/**
 * Display links to all products that have not been categorized.
 */
function uc_catalog_orphaned_products() {
  $output = '<p>'. t('Orphaned products are products that you have created but
     not yet assigned to a category in your product catalog. All such
     products will appear as links below that you can follow to edit
     the product listings to assign them to categories.') .'</p>';

  $query = "SELECT DISTINCT n.nid, n.title FROM {node} AS n LEFT JOIN {term_node} AS tn ON n.nid = tn.nid LEFT JOIN {vocabulary_node_types} AS vnt ON n.type = vnt.type WHERE n.type <> 'image' AND tn.tid IS NULL AND vnt.vid = %d";
  $vid = variable_get('uc_catalog_vid', 0);
  $result = db_query($query, $vid);

  $rows = array();
  while ($node = db_fetch_object($result)) {
    $rows[] = l($node->title, 'node/'. $node->nid .'/edit', array('query' => array('destination' => 'admin/store/products/orphans')));
  }

  if (count($rows) > 0) {
    $output .= theme('item_list', $rows);
  }
  else {
    $output .= '<p>'. t('All products are currently listed in the catalog.') .'</p>';
  }

  return $output;
}


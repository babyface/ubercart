<?php

/**
 * Implementation of hook_views_data()
 *
 * @see uc_views_handler_price
 * @see uc_views_handler_weight
 * @see uc_product_views_handler_add_to_cart_link
 * @see uc_product_views_handler_filter_product
 */
function uc_product_views_data() {
  $data['uc_products']['table']['group'] = t('Product');

  $data['uc_products']['table']['join'] = array(
    'node' => array(
      'left_field' => 'vid',
      'field' => 'vid',
    ),
    'node_revisions' => array(
      'left_field' => 'vid',
      'field' => 'vid',
    ),
  );

  $data['uc_products']['model'] = array(
    'title' => t('SKU'),
    'help' => t('The model number.'),
    'field' => array(
      'handler' => 'views_handler_field_node',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $data['uc_products']['list_price'] = array(
    'title' => t('List price'),
    'help' => t("The manufacturer's suggested price."),
    'field' => array(
      'handler' => 'uc_views_handler_field_price',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );

  $data['uc_products']['cost'] = array(
    'title' => t('Cost'),
    'help' => t('The cost of the product to the store.'),
    'field' => array(
      'handler' => 'uc_views_handler_field_price',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );

  $data['uc_products']['sell_price'] = array(
    'title' => t('Sell price'),
    'help' => t('The price for which the product is sold.'),
    'field' => array(
      'handler' => 'uc_views_handler_field_price',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );

  $data['uc_products']['weight'] = array(
    'title' => t('Weight'),
    'help' => t('The physical weight.'),
    'field' => array(
      'handler' => 'uc_views_handler_field_weight',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
  );

  $data['uc_products']['ordering'] = array(
    'title' => t('Ordering'),
    'help' => t('The default sort criteria in the catalog.'),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['uc_products']['is_product'] = array(
    'title' => t('Is a product'),
    'help' => t('Check for the fields provided by the Ubercart product module.'),
    'group' => t('Node'),
    'filter' => array(
      'handler' => 'uc_views_handler_filter_product',
      'label' => t('Is a product'),
    ),
  );

  $data['uc_products']['addtocartlink'] = array(
    'title' => t('Add to cart form'),
    'help' => t("Form to put the product in the customer's cart."),
    'group' => t('Product'),
    'field' => array(
      'additional fields' => array(
        'nid' => array(
          'table' => 'node',
          'field' => 'nid',
        ),
        'type' => array(
          'table' => 'node',
          'field' => 'type',
        ),
      ),
      'handler' => 'uc_views_handler_field_addtocart',
    ),
  );

  $data['uc_products']['buyitnowbutton'] = array(
    'title' => t('Buy it now button'),
    'help' => t('A button to add a product to the cart without quantity or attribute fields.'),
    'group' => t('Product'),
    'field' => array(
      'table' => 'node',
      'additional fields' => array(
        'nid' => array(
          'table' => 'node',
          'field' => 'nid',
        ),
      ),
      'handler' => 'uc_views_handler_field_buyitnow',
    ),
  );

  return $data;
}

/**
 * Return a formatted price value to display in the View.
 */
class uc_views_handler_field_price extends views_handler_field {
  function render($values) {
    return uc_currency_format($values->{$this->field_alias});
  }
}

/**
 * Return a formatted weight value to display in the View.
 */
class uc_views_handler_field_weight extends views_handler_field {
  function render($values) {
    return uc_weight_format($values->{$this->field_alias});
  }
}

/**
 * Filter nodes based on whether they have an entry in the uc_products table.
 */
class uc_views_handler_filter_product extends views_handler_filter_boolean_operator {
  function query() {
    $types = module_invoke_all('product_types');
    $this->query->add_field('node', 'type');
    $this->query->add_where($this->options['group'], "node.type ". (empty($this->value) ? "NOT " : "") ."IN (". db_placeholders($types, 'varchar') .")", $types);
  }
}

class uc_views_handler_field_addtocart extends views_handler_field {
  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function render($values) {
    $type = node_get_types('type', $values->{$this->aliases['type']});
    $module = $type->module;
    $product = node_load($values->{$this->aliases['nid']});
    if (function_exists('theme_'. $module .'_add_to_cart')) {
      return theme($module .'_add_to_cart', $product);
    }
    else if (function_exists('theme_uc_product_add_to_cart')) {
      return theme('uc_product_add_to_cart', $product);
    }
  }
}

class uc_views_handler_field_buyitnow extends views_handler_field {
  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function render($values) {
    $product = node_load($values->{$this->aliases['nid']});
    if (uc_product_is_product($product)) {
      return drupal_get_form('uc_catalog_buy_it_now_form_'. $values->{$this->aliases['nid']}, $product);
    }
  }
}

/**
 * Conditionally add editablefields support.
 */
function uc_product_views_tables_alter(&$tables) {
  if (module_exists('editablefields')) {
    if (is_array($tables['uc_products']['fields']['model']['option'])) {
      $tables['uc_products']['fields']['model']['option']['#options']['editable'] = t('Editable');
    }
    else {
      $tables['uc_products']['fields']['model']['option'] = array(
        '#type' => 'select',
        '#options' => array(
          'display' => t('Display'),
          'editable' => t('Editable'),
        ),
      );
    }
    $tables['uc_products']['fields']['model']['form_parents'] = 'base][model';
    if (is_array($tables['uc_products']['fields']['list_price']['option'])) {
      $tables['uc_products']['fields']['list_price']['option']['#options']['editable'] = t('Editable');
    }
    else {
      $tables['uc_products']['fields']['list_price']['option'] = array(
        '#type' => 'select',
        '#options' => array(
          'display' => t('Display'),
          'editable' => t('Editable'),
        ),
      );
    }
    $tables['uc_products']['fields']['list_price']['form_parents'] = 'base][prices][list_price';
    if (is_array($tables['uc_products']['fields']['cost']['option'])) {
      $tables['uc_products']['fields']['cost']['option']['#options']['editable'] = t('Editable');
    }
    else {
      $tables['uc_products']['fields']['cost']['option'] = array(
        '#type' => 'select',
        '#options' => array(
          'display' => t('Display'),
          'editable' => t('Editable'),
        ),
      );
    }
    $tables['uc_products']['fields']['cost']['form_parents'] = 'base][prices][cost';
    if (is_array($tables['uc_products']['fields']['sell_price']['option'])) {
      $tables['uc_products']['fields']['sell_price']['option']['#options']['editable'] = t('Editable');
    }
    else {
      $tables['uc_products']['fields']['sell_price']['option'] = array(
        '#type' => 'select',
        '#options' => array(
          'display' => t('Display'),
          'editable' => t('Editable'),
        ),
      );
    }
    $tables['uc_products']['fields']['sell_price']['form_parents'] = 'base][prices][sell_price';
    if (is_array($tables['uc_products']['fields']['weight']['option'])) {
      $tables['uc_products']['fields']['weight']['option']['#options']['editable'] = t('Editable');
    }
    else {
      $tables['uc_products']['fields']['weight']['option'] = array(
        '#type' => 'select',
        '#options' => array(
          'display' => t('Display'),
          'editable' => t('Editable'),
        ),
      );
    }
    $tables['uc_products']['fields']['weight']['form_parents'] = 'base][weight';
  }
}

/**
 * Implementation of hook_default_view_views().
 */
function uc_product_views_default_views() {
  $view = new view;
  $view->name = 'products';
  $view->description = 'List of products.';
  $view->tag = 'Ubercart';
  $view->view_php = '';
  $view->base_table = 'node';
  $view->is_cacheable = FALSE;
  $view->api_version = 2;
  $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
  $handler = $view->new_display('default', 'Defaults', 'default');
  $handler->override_option('fields', array(
    'field_image_cache_fid' => array(
      'label' => '',
      'link_to_node' => 1,
      'label_type' => 'widget',
      'format' => 'product_list_linked',
      'multiple' => array(
        'group' => 0,
        'multiple_number' => '1',
        'multiple_from' => '0',
        'multiple_reversed' => 0,
      ),
      'exclude' => 0,
      'id' => 'field_image_cache_fid',
      'table' => 'node_data_field_image_cache',
      'field' => 'field_image_cache_fid',
      'relationship' => 'none',
      'override' => array(
        'button' => 'Override',
      ),
    ),
    'title' => array(
      'label' => 'Title',
      'link_to_node' => 1,
      'exclude' => 0,
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'none',
    ),
    'sell_price' => array(
      'label' => 'Sell price',
      'exclude' => 0,
      'id' => 'sell_price',
      'table' => 'uc_products',
      'field' => 'sell_price',
      'relationship' => 'none',
    ),
    'buyitnowbutton' => array(
      'label' => 'Buy it now button',
      'exclude' => 0,
      'id' => 'buyitnowbutton',
      'table' => 'uc_products',
      'field' => 'buyitnowbutton',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('sorts', array(
    'ordering' => array(
      'order' => 'ASC',
      'id' => 'ordering',
      'table' => 'uc_products',
      'field' => 'ordering',
      'relationship' => 'none',
    ),
    'title' => array(
      'order' => 'ASC',
      'id' => 'title',
      'table' => 'node',
      'field' => 'title',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('filters', array(
    'status' => array(
      'operator' => '=',
      'value' => 1,
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'status',
      'table' => 'node',
      'field' => 'status',
      'relationship' => 'none',
    ),
    'is_product' => array(
      'operator' => '=',
      'value' => 1,
      'group' => '0',
      'exposed' => FALSE,
      'expose' => array(
        'operator' => FALSE,
        'label' => '',
      ),
      'id' => 'is_product',
      'table' => 'uc_products',
      'field' => 'is_product',
      'relationship' => 'none',
    ),
  ));
  $handler->override_option('access', array(
    'type' => 'none',
    'role' => array(),
    'perm' => '',
  ));
  $handler->override_option('title', 'Products');
  $handler->override_option('use_pager', '1');
  $handler->override_option('style_plugin', 'table');
  $handler->override_option('style_options', array(
    'grouping' => '',
    'override' => 1,
    'sticky' => 0,
    'order' => 'asc',
    'columns' => array(
      'title' => 'title',
      'sell_price' => 'sell_price',
    ),
    'info' => array(
      'title' => array(
        'sortable' => 1,
        'separator' => '',
      ),
      'sell_price' => array(
        'sortable' => 1,
        'separator' => '',
      ),
    ),
    'default' => 'title',
  ));
  $handler = $view->new_display('page', 'Page', 'page_1');
  $handler->override_option('path', 'products');
  $handler->override_option('menu', array(
    'type' => 'none',
    'title' => '',
    'weight' => 0,
  ));
  $handler->override_option('tab_options', array(
    'type' => 'none',
    'title' => '',
    'weight' => 0,
  ));
  $views[$view->name] = $view;

  return $views;
}

<?php
// $Id: uc_file.admin.inc,v 1.1.2.2 2008/11/07 21:13:23 islandusurper Exp $

/**
 * @file
 * File administration menu items.
 *
 */

function uc_file_files_admin() {
  _file_table_action('insert');
  return drupal_get_form('uc_file_files_form');
}

/**
 * Implementation of theme_form($form)
 */
function theme_uc_file_files_form($form) {
  $output = '';

  //Only display files on 1st form step
  if ($form['step']['#value'] == 1) {
    $files = array();
    $args = array('form' => $form);
    $header = tablesort_sql(tapir_get_header('uc_file_files_table', array()));
    $order = (empty($header)) ? "ORDER BY f.filename ASC" : $header .", f.filename ASC";
    $count_query = "SELECT COUNT(*) FROM {uc_files}";
    $query = pager_query("SELECT n.nid, f.filename, n.title, fp.model, f.fid, pf.pfid FROM {uc_files} as f LEFT JOIN {uc_file_products} as fp ON (f.fid = fp.fid) LEFT JOIN {uc_product_features} as pf ON (fp.pfid = pf.pfid) LEFT JOIN {node} as n ON (pf.nid = n.nid) ". $order, UC_FILE_PAGER_SIZE, 0, $count_query);
    while ($file = db_fetch_object($query)) {
      $files[] = $file;
    }
    $args['files'] = $files;

    $output .= '<p>'. t('File downloads can be attached to any Ubercart product as a product feature. For security reasons the <a href="!download_url">file downloads directory</a> is separated from the Drupal <a href="!file_url">file system</a>. Here are the list of files (and their associated Ubercart products) that can be used for file downloads.', array('!download_url' => url('admin/store/settings/products/edit/features'), '!file_url' => url('admin/settings/file-system'))) .'</p>';
    $output .= drupal_render($form['uc_file_action']);
    $output .= tapir_get_table('uc_file_files_table', $args);
    $output .= theme('pager', NULL, UC_FILE_PAGER_SIZE, 0);
  }

  //Checkboxes already rendered in uc_file_files_table
  foreach ($form as $form_element => $form_data) {
    if (strpos($form_element, 'file_select_') !== FALSE) {
      unset($form[$form_element]);
    }
  }
  $output .= drupal_render($form);

  return $output;
}

/**
 * Form builder for file products admin
 */
function uc_file_files_form($form_state) {
  if (!isset($form_state['step']) || $form_state['step'] < 2) {
    //Display File Options and File checkboxes
    $files = db_query("SELECT * FROM {uc_files}");
    $file_actions = array(
      'uc_file_delete' => t('Delete file(s)'),
      'uc_file_upload' => t('Upload file')
    );

    //Check any if any hook_file_action('info', $args) are implemented
    foreach (module_implements('file_action') as $module) {
      $name = $module .'_file_action';
      $result = $name('info', NULL);
      if (is_array($result)) {
        foreach ($result as $key => $action) {
          if ($key != 'uc_file_delete' && $key != 'uc_file_upload') {
            $file_actions[$key] = $action;
          }
        }
      }
    }

    while ($file = db_fetch_object($files)) {
      $form['file_select_'. $file->fid] = array('#type' => 'checkbox');
    }
    $form['uc_file_action'] = array(
      '#type' => 'fieldset',
      '#title' => t('File options'),
      '#collapsible' => FALSE,
      '#collapsed' => FALSE,
    );
    $form['uc_file_action']['action'] = array(
      '#type' => 'select',
      '#title' => t('Action'),
      '#options' => $file_actions,
      '#prefix' => '<div class="duration">',
      '#suffix' => '</div>',
    );
    $form['uc_file_action']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Perform action'),
      '#prefix' => '<div class="duration">',
      '#suffix' => '</div>',
    );
  }
  else {
    //Perform File Action (Upload, Delete, hooked in actions)
    $file_ids = array();
    foreach ($form_state['values'] as $name => $form_value) {
      if (strpos($name, 'file_select_') !== FALSE) {
        $file_ids[] = intval(str_replace('file_select_', '', $name));
      }
    }
    $form['file_ids'] = array('#type' => 'value', '#value' => $file_ids);
    $form['action'] = array('#type' => 'value', '#value' => $form_state['values']['action']);

    //Switch to an action to perform
    switch ($form_state['values']['action']) {
      case 'uc_file_delete': //Delete selected files
        foreach ($file_ids as $file_id) {
          $filename = db_result(db_query("SELECT filename FROM {uc_files} WHERE fid = %d", $file_id));
          $filename = (substr($filename, -1) == "/") ? $filename .' ('. t('directory') .')' : $filename;
          $file_list[] = $filename;
        }
        $form['files'] = array(
          '#type' => 'markup',
          '#value' => theme_item_list($file_list, NULL, 'ul', array('class' => 'file-name')),
        );
        $form['recurse_directories'] = array(
          '#type' => 'checkbox',
          '#title' => t('Delete selected directories and their sub directories'),
        );
        $form = confirm_form($form, t('Delete the following file(s)?'), 'admin/store/products/files', t('Deleting a file will remove all its associated file downloads and product features. Removing a directory will remove any files it contains and their associated file downloads and product features.'), t('Yes'), t('No'));
        break;
      case 'uc_file_upload': //Upload file
        drupal_set_title(t('Upload File'));
        $max_bytes = trim(ini_get('post_max_size'));
        $directories = array('' => '/');

        switch (strtolower($max_bytes{strlen($max_bytes)-1})) {
          case 'g':
            $max_bytes *= 1024;
          case 'm':
            $max_bytes *= 1024;
          case 'k':
            $max_bytes *= 1024;
        }

        $files = db_query("SELECT * FROM {uc_files}");
        while ($file = db_fetch_object($files)) {
          if (is_dir(variable_get('uc_file_base_dir', NULL) ."/". $file->filename)) {
            $directories[$file->filename] = $file->filename;
          }
        }

        $form['#attributes']['enctype'] = 'multipart/form-data';
        $form['upload_dir'] = array(
          '#type' => 'select',
          '#title' => t('Directory'),
          '#description' => t('The directory to upload the file to. The default directory is the root of the file downloads directory.'),
          '#options' => $directories,
        );
        $form['upload'] = array(
          '#type' => 'file',
          '#title' => t('File'),
          '#description' => t('The maximum file size that can be uploaded is %size bytes. You will need to use a different method to upload the file to the directory (e.g. FTP, SSH) if your file exceeds this size.', array('%size' => number_format($max_bytes))),
        );
        $form['submit'] = array(
          '#type' => 'submit',
          '#value' => t('Upload'),
        );
        break;
      default:
        //Check any if any hook_file_action('form', $args) are implemented
        foreach (module_implements('file_action') as $module) {
          $name = $module .'_file_action';
          $result = $name('form', array('action' => $form_state['values']['action'], 'file_ids' => $file_ids));
          $form = (is_array($result)) ? array_merge($form, $result) : $form;
        }
        break;
    }
  }

  return $form;
}

function uc_file_files_form_validate($form, &$form_state) {
  switch ($form_state['step']) {
    case 2:
      switch ($form_state['values']['action']) {
        case 'uc_file_delete': //Nothing to validate for file delete
          break;
        case 'uc_file_upload':
          //Check any if any hook_file_action('validate', $args) are implemented
          if ($temp_file = file_save_upload('upload', array())) {
            foreach (module_implements('file_action') as $module) {
              $name = $module .'_file_action';
              $result = $name('upload_validate', array('file_object' => $temp_file, 'form_id' => $form_id, 'form_state' => $form_state));
            }
            $form_state['storage']['temp_file'] = $temp_file;
          }
          else {
            form_set_error('', t('An error occurred while uploading the file'));
          }
          break;
        default:
          //Check any if any hook_file_action('validate', $args) are implemented
          foreach (module_implements('file_action') as $module) {
            $name = $module .'_file_action';
            $result = $name('validate', array('form_id' => $form_id, 'form_state' => $form_state));
          }
          break;
      }
      break;
    default:
      break;
  }
}

function uc_file_files_form_submit($form, &$form_state) {
  switch ($form_state['step']) {
    case 2:
      switch ($form_state['values']['action']) {
        case 'uc_file_delete':
          foreach ($form_state['values']['file_ids'] as $file_id) {
            _file_table_action('remove', $file_id, $form_state['values']['recurse_directories'], TRUE);
          }
          drupal_set_message(t('The select file(s) have been deleted.'));
          break;
        case 'uc_file_upload':
          $dir = variable_get('uc_file_base_dir', NULL) .'/';
          $dir = (is_null($form_state['values']['upload_dir'])) ? $dir : $dir . $form_state['values']['upload_dir'];
          if (is_dir($dir)) {
            $file_object = $form_state['storage']['temp_file'];
            if (file_copy($file_object, $dir, FILE_EXISTS_REPLACE)) {
              //Check any if any hook_file_action('upload', $args) are implemented
              foreach (module_implements('file_action') as $module) {
                $name = $module .'_file_action';
                $result = $name('upload', array('file_object' => $file_object, 'form_id' => $form_id, 'form_state' => $form_state));
              }

              _file_table_action('insert');
              drupal_set_message(t('The %file has been uploaded to %dir', array('%file' => basename($file_object->filepath), '%dir' => $dir)));
            }
            else {
              drupal_set_message(t('An error occurred while copying the file to %dir', array('%dir' => $dir)));
            }
          }
          else {
            drupal_set_message(t('Can not move file to %dir', array('%dir' => $dir)));
          }
          break;
        default:
          //Check any if any hook_file_action('validate', $args) are implemented
          foreach (module_implements('file_action') as $module) {
            $name = $module .'_file_action';
            $result = $name('submit', array('form_id' => $form_id, 'form_state' => $form_state));
          }
          break;
      }
      $form_state['rebuild'] = FALSE;
      drupal_goto('admin/store/products/files');
      break;
    default:
      $form_state['step'] = 2;
      $form_state['rebuild'] = TRUE;
    break;
  }
}

 /**
 * Form builder for file download notification settings.
 */
function uc_file_notify_settings() {
  $form['uc_file_download_notification'] = array(
    '#type' => 'checkbox',
    '#title' => t('Send email to customer with file download link(s).'),
    '#default_value' => variable_get('uc_file_download_notification', FALSE),
  );
  $form['uc_file_download_notification_subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Message subject'),
    '#default_value' => variable_get('uc_file_download_notification_subject', uc_get_message('uc_file_download_subject')),
  );
  $form['uc_file_download_notification_message'] = array(
    '#type' => 'textarea',
    '#title' => t('Message text'),
    '#default_value' => variable_get('uc_file_download_notification_message', uc_get_message('uc_file_download_message')),
    '#description' => t('The message the user receives after purchasing products with file downloads (<a href="!token_url">uses order, uc_file, and global tokens</a>)', array('!token_url' => url('admin/store/help/tokens'))),
    '#rows' => 10,
  );
  $form['uc_file_download_notification_format'] = filter_form(variable_get('uc_file_download_notification_format', FILTER_FORMAT_DEFAULT), NULL, array('uc_file_download_notification_format'));

  return system_settings_form($form);
}

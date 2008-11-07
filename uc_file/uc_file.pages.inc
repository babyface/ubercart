<?php
// $Id: uc_file.pages.inc,v 1.1.2.3 2008/11/07 21:13:23 islandusurper Exp $

/**
 * @file
 * File menu items.
 *
 */

/**
 * Table builder for user downloads
 */
function uc_file_user_downloads($account) {
  drupal_set_title(t('File downloads'));
  drupal_add_js(drupal_get_path('module', 'uc_file') .'/uc_file.js');

  $header = array(
    array('data' => t('Purchased'), 'field' => 'u.granted', 'sort' => 'desc'),
    array('data' => t('Filename'), 'field' => 'f.filename'),
    array('data' => t('Description'), 'field' => 'p.description'),
    array('data' => t('Downloads'), 'field' => 'u.accessed'),
  );

  $sql = "SELECT granted, filename, accessed, description, `key`, f.fid FROM {uc_file_users} as u LEFT JOIN {uc_files} as f ON u.fid = f.fid LEFT JOIN {uc_file_products} as p ON p.pfid = u.pfid WHERE uid = %d";
  $count_query = "SELECT COUNT(*) FROM {uc_file_users} WHERE uid = %d";
  $download_limit = variable_get('uc_file_download_limit_number', NULL);
  $file_ids = array();
  $rows = array();
  $files = pager_query($sql . tablesort_sql($header), UC_FILE_PAGER_SIZE, 0, $count_query, $account->uid);

  while ($file = db_fetch_object($files)) {
    $row = count($rows);
    $expiration = _file_expiration_date($file->granted);
    $onclick = array('onclick' => 'uc_file_update_download('. $row .', '. $file->accessed .', '. ((empty($download_limit)) ? -1 : $download_limit) .');', 'id' => 'link-'. $row);

    if (!$expiration) {
      $file_link = l(basename($file->filename), 'download/'. $file->fid .'/'. $file->key, $onclick);
    }
    else {
      if (time() > $expiration) {
        $file_link = basename($file->filename);
      }
      else {
        $file_link = l(basename($file->filename), 'download/'. $file->fid .'/'. $file->key, $onclick) .' ('. t('expires on @date', array('@date' => format_date($expiration, 'custom', variable_get('uc_date_format_default', 'm/d/Y')))) .')';
      }
    }

    $rows[] = array(
      array('data' => format_date($file->granted, 'custom', variable_get('uc_date_format_default', 'm/d/Y')), 'class' => 'date-row', 'id' => 'date-'. $row),
      array('data' => $file_link, 'class' => 'filename-row', 'id' => 'filename-'. $row),
      array('data' => $file->description, 'class' => 'description-row', 'id' => 'description-'. $row),
      array('data' => $file->accessed, 'class' => 'download-row', 'id' => 'download-'. $row),
    );
  }
  if (empty($rows)) {
    $rows[] = array(array('data' => t('No downloads found'), 'colspan' => 4));
  }

  $output = theme('table', $header, $rows) . theme('pager', NULL, UC_FILE_PAGER_SIZE, 0);
  return $output;
}

/**
 * Perform first-pass authorization. Call authorization hooks afterwards.
 *
 * Called when a user requests a file download, function checks download
 * limits then checks for any implementation of hook_download_authorize.
 * Passing that, the function _file_download_transfer is called.
 *
 * @param $fid
 *   The fid of the file specified to download.
 * @param $key
 *   The hash key of a user's download
 */
function _file_download($fid, $key) {
  global $user;
  $ip = ip_address();
  $message_admin = t('Please contact the site administrator if this message has been received in error.');

  $message_user = ($user->uid) ? t('The user %username ', array('%username' => $user->name)) : t('The IP address %ip ', array('%ip' => $ip));
  $file_download = db_fetch_object(db_query("SELECT * FROM {uc_file_users} WHERE fid = %d AND `key` = '%s'", $fid, $key));
  $request_cache = cache_get('uc_file_'. $ip);
  $requests = ($request_cache) ? $request_cache->data + 1 : 1;

  if ($requests > UC_FILE_REQUEST_LIMIT) {
    _file_download_deny($user->uid, t('You have attempted to download an incorrect file URL too many times. ') . $message_admin);
  }

  if (!$file_download) {
    cache_set('uc_file_'. $ip, $requests, 'cache', time() + 86400);
    if ($requests == UC_FILE_REQUEST_LIMIT) {
      watchdog('uc_file', '%username has been temporarily banned from file downloads.', array('%username' => $message_user), WATCHDOG_WARNING);
    }
    _file_download_deny($user->uid, t("The following URL is not a valid download link. ") . $message_admin);
  }
  else {
    $ip_limit = variable_get('uc_file_download_limit_addresses', NULL);
    $addresses = unserialize($file_download->addresses);

    if (!empty($ip_limit) && !in_array($ip, $addresses) && count($addresses) >= $ip_limit) {
      watchdog('uc_file', '%username has been denied a file download by downloading it from too many IP addresses.', array('%username' => $message_user), WATCHDOG_WARNING);
      _file_download_deny($user->uid, t('You have downloaded this file from too many different locations. ') . $message_admin);
    }
    else {
      $download_limit = variable_get('uc_file_download_limit_number', NULL);

      if (!empty($download_limit) && $file_download->accessed >= $download_limit) {
        watchdog('uc_file', '%username has been denied a file download by downloading it too many times.', array('%username' => $message_user), WATCHDOG_WARNING);
        _file_download_deny($user->uid, t('You have downloaded this file too many times. ') . $message_admin);
      }
      else {
        $duration_limit = _file_expiration_date($file_download->granted);

        if ($duration_limit !== FALSE && time() >= $duration_limit) {
          watchdog('uc_file', '%username has been denied an expired file download.', array('%username' => $message_user), WATCHDOG_WARNING);
          _file_download_deny($user->uid, t("This file download has expired. ") . $message_admin);
        }
        else {
          //Check any if any hook_download_authorize calls deny the download
          foreach (module_implements('download_authorize') as $module) {
            $name = $module .'_download_authorize';
            $result = $name($user, $file_download);
            if (!$result) {
              _file_download_deny($user->uid);
            }
          }

          $filename = db_result(db_query("SELECT filename FROM {uc_files} WHERE fid = %d", $fid));
          watchdog('uc_file', '%username has started download of the file %filename.', array('%username' => $message_user, '%filename' => basename($filename)), WATCHDOG_NOTICE);
          _file_download_transfer($file_download, $ip, $file_download->fid);
        }
      }
    }
  }
}

/**
 * Deny a file download
 *
 * @param $uid
 *   The user id of the person attempting the download
 * @param $message
 *   The optional message to send to the user
 */
function _file_download_deny($uid = NULL, $message = NULL) {
  if (!is_null($message)) {
    drupal_set_message($message, 'error');
  }
  if (is_null($uid) || $uid == 0) {
    drupal_access_denied();
    exit();
  }
  else {
    drupal_goto('user/'. $uid .'/files');
  }
}

/**
 * Send the file's binary data to a user via HTTP and update the uc_file_users table.
 *
 * @param $file_user
 *   The file_user object from the uc_file_users
 * @param $ip
 *   The string containing the ip address the download is going to
 * @param $fid
 *   The file id of the file to transfer
 */
function _file_download_transfer($file_user, $ip, $fid) {
  $file = db_result(db_query("SELECT filename FROM {uc_files} WHERE fid = %d", $fid));
  $file_path = variable_get('uc_file_base_dir', NULL) .'/'. $file;

  if (!is_file($file_path)) {
    drupal_set_message(t('The file %filename could not be found. Please contact the site administrator.', array('%filename' => basename($file))), 'error');
    drupal_not_found();
    exit();
  }
  else {
    //Check any if any hook_file_transfer_alter calls alter the download
    foreach (module_implements('file_transfer_alter') as $module) {
      $name = $module .'_file_transfer_alter';
      $file_path = $name($file_user, $ip, $fid, $file_path);
    }
    $data = fopen($file_path, 'rb');

    ob_end_clean();
    $http_headers = array(
      'Pragma: no-cache',
      'Expires: 0',
      'Cache-Control: no-cache, must-revalidate',
      'Cache-Control: private',
      'Content-Transfer-Encoding: binary',
      'Content-Length:'. filesize($file_path),
      'Content-Disposition: attachment; filename="'. basename($file) .'"',
      'Content-Type: application/octet-stream'
    );
    foreach ($http_headers as $header) {
      $header = preg_replace('/\r?\n(?!\t| )/', '', $header);
      drupal_set_header($header);
    }

    while (!feof($data)) {
      print fread($data, UC_FILE_BYTE_SIZE);
    }
    fclose($data);
    _user_table_action('download', $file_user, $ip);
    exit();
  }
}


// $Id: ca.js,v 1.1.2.1 2008/09/22 21:07:41 islandusurper Exp $

/**
 * @file
 *   Adds some helper JS to the conditional actions forms.
 */

// Adds confirmation prompts to remove buttons.
Drupal.behaviors.caRemoveConfirm = function(context) {
  $('.ca-remove-confirm:not(.caRemoveConfirm-processed)', context).addClass('caRemoveConfirm-processed').click(function() {
    return confirm(Drupal.t('Are you sure you want to remove this item?'));
  });
}


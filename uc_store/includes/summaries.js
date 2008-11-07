// $Id: summaries.js,v 1.1.2.3 2008/11/07 21:13:26 islandusurper Exp $

/**
 * @file
 *   Adds some helper JS to summaries.
 */

// Modifies the summary overviews to have onclick functionality.
Drupal.behaviors.summaryOnclick = function(context) {
  $('.summary-overview:not(.summaryOnclick-processed)', context).prepend('<img src="' + Drupal.settings.editIconPath + '" class="summary-edit-icon" />');

  $('.summary-overview:not(.summaryOnclick-processed)', context).addClass('summaryOnclick-processed').click(function() {
    window.location = Drupal.settings.basePath + this.id;
  });
}


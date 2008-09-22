// $Id: summaries.js,v 1.1.2.1 2008/09/22 21:07:45 islandusurper Exp $

/**
 * @file
 *   Adds some helper JS to summaries.
 */

// Modifies the summary overviews to have onclick functionality.
Drupal.behaviors.summaryOnclick = function(context) {
  $('.summary-overview:not(.summaryOnclick-processed)', context).prepend('<img src="' + edit_icon_path + '" class="summary-edit-icon" />');

  $('.summary-overview:not(.summaryOnclick-processed)', context).addClass('summaryOnclick-processed').click(function() {
    window.location = Drupal.settings.basePath + this.id;
  });
}


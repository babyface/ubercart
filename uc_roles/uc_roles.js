function _uc_role_expiration_disable_check(granularity, quantity) {
  // 'never' means there's no point in setting a duration.
  if ($(granularity).val() == 'never') {
    $(quantity).attr('disabled', 'disabled').val('');
  }
  // Anything besides 'never' should enable setting a duration.
  else {
    $(quantity).removeAttr('disabled');
  }
}

$(document).ready(
  function() {
    _uc_role_expiration_disable_check('#edit-uc-roles-granularity', '#edit-uc-roles-qty');
    _uc_role_expiration_disable_check('#edit-uc-roles-default-granularity', '#edit-uc-roles-default-length');
    _uc_role_expiration_disable_check('#edit-uc-roles-reminder-granularity', '#edit-uc-roles-reminder-length');
  }
);

// When you change the role expiration granularity select.
Drupal.behaviors.ucRoleExpirationGranularity = function(context) {
  $('#edit-uc-roles-granularity:not(.ucRoleExpirationGranularity-processed)', context).addClass('ucRoleExpirationGranularity-processed').change(
    function() {
    _uc_role_expiration_disable_check('#edit-uc-roles-granularity', '#edit-uc-roles-qty');
    }
  );
}

// When you change the default role expiration granularity select.
Drupal.behaviors.ucRoleDefaultExpirationGranularity = function(context) {
  $('#edit-uc-roles-default-granularity:not(.ucRoleDefaultExpirationGranularity-processed)', context).addClass('ucRoleDefaultExpirationGranularity-processed').change(
    function() {
    _uc_role_expiration_disable_check('#edit-uc-roles-default-granularity', '#edit-uc-roles-default-length');
    }
  );
}

// When you change the default role expiration granularity select.
Drupal.behaviors.ucRoleReminderExpirationGranularity = function(context) {
  $('#edit-uc-roles-reminder-granularity:not(.ucRoleReminderExpirationGranularity-processed)', context).addClass('ucRoleReminderExpirationGranularity-processed').change(
    function() {
    _uc_role_expiration_disable_check('#edit-uc-roles-reminder-granularity', '#edit-uc-roles-reminder-length');
    }
  );
}


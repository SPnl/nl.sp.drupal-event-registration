<?php

/**
 * Class SPEventReg_Civi
 * Functies voor CiviCRM-integratie.
 */
class SPEventReg_Civi {

  private static $instance;

  /**
   * Get instance
   * @return bool|\SPEventReg_Civi Instance
   */
  public static function getInstance() {
    if (!self::$instance) {
      self::$instance = new self;
    }

    if (!civicrm_initialize()) {
      drupal_set_message('Could not initialize CiviCRM.');
      return FALSE;
    }

    return self::$instance;
  }

}
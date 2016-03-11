<?php

/**
 * Page callback: deelnemerslijst voor een of meer evenementen,
 * met de mogelijkheid die aan- of af te melden e.d.
 */
function speventreg_page_participants() {

  // Check session variables
  $event_ids = $_SESSION['speventreg_event_ids'];
  $location_type = $_SESSION['speventreg_location_type'];

  if (!$event_ids || !is_array($event_ids)) {
    drupal_goto('evenementenregistratie');
  }

  $event_ids = $_SESSION['speventreg_event_ids'];
  if (!is_array($event_ids) && !is_numeric($event_ids)) {
    drupal_set_message('Ongeldige evenementen-id (' . $event_ids . ').');
    drupal_goto('evenementregistratie');
  }

  require_once 'speventreg.civicrm.inc';

  // Fetch data and set view parameters
  $spcivi = \SPEventreg_Civi::getInstance();
  $params = [
    'events'       => $spcivi->getEventKV($event_ids),
    'participants' => $spcivi->getParticipants($event_ids, TRUE, $location_type),
    'location_type' => $location_type,
  ];

  // Add module CSS / JS
  drupal_add_js(drupal_get_path('module', 'speventreg') . '/speventreg.js');
  drupal_add_css('https://maxcdn.bootstrapcdn.com/font-awesome/4.5.0/css/font-awesome.min.css', ['type' => 'external']);
  drupal_add_css(drupal_get_path('module', 'speventreg') . '/speventreg.css');

// Return view
  return theme('speventreg-tpl-participants', $params);
}

/**
 * Theme function voor speventreg-tpl-participants.tpl.php.
 * Dit is blijkbaar nodig. Leve Drupal.
 */
function theme_speventreg_tpl_participants($vars) {
  return $vars;
}

/**
 * Formulier voor nieuwe deelnemers toevoegen, met autocomplete functionaliteit
 * @param array $form Form
 * @return array Form
 */
function speventreg_addparticipant_form($form) {

  $spcivi = \SPEventreg_Civi::getInstance();

  $form['contact'] = [
    '#type'              => 'textfield',
    '#required'          => TRUE,
    '#autocomplete_path' => 'evenementregistratie/autocomplete',
    '#attributes'        => ['placeholder' => 'Zoek een contact...', 'required' => 'required'],
  ];

  $form['role'] = [
    '#type'       => 'select',
    '#options'    => $spcivi->getAvailableRoles(),
    '#attributes' => ['required' => 'required'],
  ];

  $form['event'] = [
    '#type'       => 'select',
    '#options'    => $spcivi->getEventKV($_SESSION['speventreg_event_ids']),
    '#attributes' => ['required' => 'required'],
  ];

  $form['submit'] = [
    '#type'  => 'submit',
    '#value' => 'Toevoegen',
    '#ajax'  => ['callback' => 'speventreg_addparticipant_callback', 'wrapper' => 'speventreg-new-inner'],
  ];

  return $form;
}

/**
 * Callback voor het bovenstaande formulier - voegt een participant toe en ververst voor nu gewoon de pagina.
 * @param array $form Form
 * @param array $form_state Form state
 * @return mixed content
 */
function speventreg_addparticipant_callback($form, &$form_state) {

  /** @var \SPEventreg_Civi $spcivi */
  $spcivi = \SPEventreg_Civi::getInstance();

  $statuses = $spcivi->getAvailableStatuses();
  $this_status = array_search('Attended', $statuses);

  if (!empty($form_state['values']['event']) && !empty($form_state['values']['contact']) && !empty($form_state['values']['role'])) {

    $success = $spcivi->addParticipant($form_state['values']['event'], $form_state['values']['contact'], $this_status, $form_state['values']['role']);

    if ($success) {
      $form_state['rebuild'] = TRUE;
      return '<p class="success"><i class="fa fa-check-circle"></i>  Nieuwe deelnemer toegevoegd! <a class="speventreg-refresh" href="#">Ververs de pagina</a> om het record te zien.</p>' . drupal_render($form);
    } else {
      return '<p class="error"><i class="fa fa-times-circle"></i> Deze deelnemer kon niet aangemeld worden. Mogelijk is hij/zij al aangemeld.</p>' . drupal_render($form);
    }
  } else {
    return '<p class="error"><i class="fa fa-times-circle"></i>  Je hebt een of meer verplichte velden niet ingevuld.</p>' . drupal_render($form);
  }
}

/**
 * Page callback voor aanmelden/afmelden van mensen. Via ajax.
 * @param int $participant_id Participant ID (!)
 * @param string $action Action (register/unregister)
 */
function speventreg_page_partstatus($participant_id, $action) {

  /** @var \SPEventreg_Civi $spcivi */
  $spcivi = \SPEventreg_Civi::getInstance();
  $statuses = $spcivi->getAvailableStatuses();

  if ($action == 'register') {
    $ret = $spcivi->updateParticipantStatus($participant_id, array_search('Attended', $statuses));
  } else {
    $ret = $spcivi->updateParticipantStatus($participant_id, array_search('Cancelled', $statuses));
  }

  drupal_json_output(['status' => ($ret ? 'ok' : 'error')]);
}

/**
 * Autocomplete voor ledengegevens
 * @param string $string
 */
function speventreg_page_autocomplete($string = "") {

  /** @var \SPEventreg_Civi $spcivi */
  $spcivi = \SPEventreg_Civi::getInstance();

  drupal_json_output($spcivi->getContactAutocompleteData($string));
}

<?php

/**
 * Initialiseer CiviCRM logic / API
 * @return bool Success
 */
function speventreg_civi_initialize() {
  if (!civicrm_initialize()) {
    drupal_set_message('Could not initialize CiviCRM.');

    return FALSE;
  }

  return TRUE;
}

/**
 * Geeft alle toekomstige evenementen als key-value-array terug
 * @param int|array|null $event_ids Event IDs (optional)
 * @return array Evenementen
 * @throws CiviCRM_API3_Exception Exception
 */
function speventreg_civi_eventkv($event_ids = NULL) {

  $params = [
    'sequential' => 1,
    'start_date' => ['>=' => date('Y-m-d')],
    'options'    => ['sort' => "start_date", 'limit' => 10],
    'return'     => "id,title,start_date,end_date",
  ];

  speventreg_civi_initialize();
  if(is_numeric($params['event_id'])) {
    $params['event_id'] = $event_ids;
  } elseif(is_array($event_ids) && !is_null($event_ids)) {
    $params['event_id'] = ['IN' => $event_ids];
  }
  $events = civicrm_api3('Event', 'get', $params);

  $ret = [];
  if (!$events['is_error']) {
    foreach ($events['values'] as $event) {
      $eventDate = new \DateTime($event['start_date']);
      $ret[ $event['id'] ] = $event['title'] . ' (' . $eventDate->format('d-m-Y') . ')';
    }
  }

  return $ret;

}

/**
 * Geeft alle deelnemers voor een specifiek event terug
 * @param int|array $event_ids Event ID(s)
 * @return array Participants
 * @throws CiviCRM_API3_Exception Exception
 */
function speventreg_civi_participants($event_ids = []) {

  speventreg_civi_initialize();
  if (is_array($event_ids)) {
    $event_ids = array_values($event_ids);
  } elseif(is_numeric($event_ids)) {
    $event_ids = [$event_ids];
  }

  // Getting participants for each event. event_id => ['IN' => [1, 2]] doesn't work - may be a 4.4 bug
  $participants = [];
  foreach($event_ids as $event_id) {
    $res = civicrm_api3('Participant', 'get', [
      'sequential' => 1,
      'event_id' => $event_id,
    ]);
    if (!$res['is_error']) {
      $participants = array_merge($participants, $res['values']);
    }
  }

  return $participants;
}
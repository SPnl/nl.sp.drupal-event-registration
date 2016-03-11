<?php

require_once 'speventreg.civicrm.inc';

/**
 * Page callback: overzicht met selectie van evenementen
 */
function speventreg_page_overview() {
  return drupal_get_form('speventreg_form_overview');
}

/**
 * Formulier: overzicht met selectie van evenementen
 * @param mixed $form
 * @param mixed $form_state
 * @return mixed $form
 */
function speventreg_form_overview($form, &$form_state) {

  /** @var \SPEventreg_Civi $spcivi */
  $spcivi = \SPEventreg_Civi::getInstance();

  $form = [
    '#prefix' => '<div class="content" id="eventreg_form"><p>Vink één of meer bijeenkomsten aan waarvoor je de aanwezigheid wilt registreren.</p>',
    '#suffix' => '<hr /></div>',
  ];

  $form['events'] = [
    '#type'          => 'checkboxes',
    '#options'       => $spcivi->getEventKV(),
    '#default_value' => (!empty($_SESSION['speventreg_event_ids']) ? $_SESSION['speventreg_event_ids'] : []),
  ];

  $form['location_type'] = [
    '#title' => 'Toon afdelingen...<br>',
    '#type'          => 'select',
    '#options'       => [
      'geo' => 'Op basis van geostelsel',
      'rel' => 'Op basis van kaderfunctie',
    ],
    '#default_value' => (!empty($_SESSION['speventreg_location_type']) ? $_SESSION['speventreg_location_type'] : ''),
  ];

  $form['submit'] = [
    '#type'       => 'submit',
    '#value'      => 'Verder >',
    '#attributes' => ['class' => ['btn', 'submit-btn']],
  ];

  drupal_add_css(drupal_get_path('module', 'speventreg') . '/speventreg.css');

  return $form;
}

function speventreg_form_overview_validate($form, &$form_state) {

  $event_ids = array_filter($form_state['values']['events']);
  if(!$event_ids || count($event_ids) == 0) {
    form_set_error('events', 'Selecteer één of meer evenementen.');
  }
}

function speventreg_form_overview_submit($form, &$form_state) {

  $_SESSION['speventreg_event_ids'] = array_values(array_filter($form_state['values']['events']));
  $_SESSION['speventreg_location_type'] = $form_state['values']['location_type'];
  drupal_goto('evenementregistratie/participants');
}
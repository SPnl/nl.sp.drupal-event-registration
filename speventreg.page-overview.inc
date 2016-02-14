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

  $form = [
    '#prefix' => '<div class="content" id="eventreg_form"><p>Vink één of meer evenementen aan waarvoor je de deelnemers wilt registreren.</p>',
    '#suffix' => '</div>',
  ];

  $form['events'] = [
    '#type'    => 'checkboxes',
    '#options' => speventreg_civi_eventkv(),
    '#default_value' => (!empty($_SESSION['speventreg_event_ids']) ? $_SESSION['speventreg_event_ids'] : null),
  ];

  $form['submit'] = [
    '#type'       => 'submit',
    '#value'      => 'Verder >',
    '#attributes' => ['class' => ['btn', 'submit-btn']],
  ];

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
  drupal_goto('evenementregistratie/participants');
}
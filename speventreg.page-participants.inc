<?php

/**
 * Page callback: deelnemerslijst voor een of meer evenementen,
 * met de mogelijkheid die aan- of af te melden e.d.
 */
function speventreg_page_participants() {

  $event_ids = $_SESSION['speventreg_event_ids'];
  if(!$event_ids || !is_array($event_ids)) {
    drupal_goto('evenementenregistratie');
  }

  $content = '<div class="content">';

  $event_ids = $_SESSION['speventreg_event_ids'];
  if(!is_array($event_ids) && !is_numeric($event_ids)) {
    drupal_set_message('Ongeldige evenementen-id (' . $event_ids . ').');
    drupal_goto('evenementregistratie');
  }

  require_once 'speventreg.civicrm.inc';

  $events = speventreg_civi_eventkv($event_ids);
  $participants = speventreg_civi_participants($event_ids);

  $content .= "<p>Je gaat nu aanwezigheid registreren voor de volgende evenementen:<br>\n";
  foreach($events as $event) {
    $content .= "- " . $event . "<br>\n";
  }
  $content .= "</p>\n";

  $content .= "<p><a href=\"#\">Nieuwe deelnemer [slidedown autocomplete]</a></p>\n";

  $rows = [];
  foreach($participants as $p) {
    $rows[] = [$p['event_title'],
              '<a href="/civicrm/contact/view?cid=' . $p['contact_id'] . '">' . $p['display_name'] . '</a>',
               $p['participant_status'],
               '<a href="#' . $p['participant_id'] . '">[Aan|Af]wezig</a>',
              '<a href="#">Vervangen</a>',
    ];
  }

  $content .= theme_table([
    'header' => ['Evenement', 'Naam', 'Status', ['data' => 'Acties', 'colspan' => 2]],
    'rows' => $rows,
    'attributes' => ['width' => '100%'],
  ]);

  $content .= '</div>';
  return $content;
}

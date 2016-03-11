<?php

/**
 * Class SPEventreg_Civi
 * Functies voor CiviCRM-integratie.
 */
class SPEventreg_Civi extends \SPCivi {

  /** \SPEventreg_Civi @method getInstance() */

  /**
   * @var array $afdelingen Gecachte array met afdelingen
   */
  private $afdelingen = [];

  /**
   * @var array $relTypes Gecachte array met relationship types
   */
  private $relTypes = [];

  /**
   * Relatietypes die getoond worden op de pagina. Drie groepen: voor iedere groep geldt dat de eerstgevonden functie getoond wordt.
   * @var array $allowedRelationships Array met sub-arrays met relationship types
   */
  private $allowedRelationships = [
    ['sprel_liddagelijksbestuur_landelijk', 'sprel_partijbestuurslid_landelijk', 'sprel_regiobestuurder_landelijk', 'Waarnemer PB','sprel_tweede_kamerlid_landelijk', 'sprel_eerste_kamerlid_landelijk', 'sprel_europarlementarier_landelijk', 'sprel_personeelslid_amersfoort_landelijk', 'sprel_personeelslid_denhaag_landelijk', 'sprel_personeelslid_brussel_landelijk', 'sprel_bestuurslidrood_landelijk'],
    ['sprel_voorzitter_afdeling', 'sprel_vervangendvoorzitter_afdeling', 'sprel_organisatiesecretaris_afdeling', 'sprel_penningmeester_afdeling', 'sprel_wethouder_afdeling', 'sprel_statenlid_provincie', 'sprel_gedeputeerde_provincie', 'sprel_fractievoorzitter_afdeling', 'sprel_bestuurslid_afdeling', 'sprel_fractieraadslid_afdeling'],
  ];

  /**
   * Geeft alle toekomstige evenementen als key-value-array terug
   * @param int|array|null $event_ids Event IDs (optional)
   * @return array Evenementen
   * @throws CiviCRM_API3_Exception Exception
   */
  public function getEventKV($event_ids = NULL) {

    $params = [
      'sequential' => 1,
      // 'start_date' => ['>=' => date('Y-m-d')],  // voor live
      'start_date' => ['>=' => '2016-01-01'],  // voor testen, in combinatie met limiet 1000
      'options'    => ['sort' => "start_date", 'limit' => 1000],
      'return'     => "id,title,start_date,end_date",
    ];

    if (is_numeric($params['event_id'])) {
      $params['event_id'] = $event_ids;
    } elseif (is_array($event_ids) && !is_null($event_ids)) {
      $params['event_id'] = ['IN' => $event_ids];
    }
    $events = $this->api('Event', 'get', $params);

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
   * @param bool $extra Extra data (calls addExtraData to get afdelingsnaam and custom status/role descriptions)
   * @param string $locationType Type of afdeling-lookup: 'geo' for geostelsel or 'rel' for relationships
   * @return array Participants
   * @throws CiviCRM_API3_Exception Exception
   */
  public function getParticipants($event_ids = [], $extra = FALSE, $locationType = 'geo') {

    if (is_array($event_ids)) {
      $event_ids = array_values($event_ids);
    } elseif (is_numeric($event_ids)) {
      $event_ids = [$event_ids];
    }

    // Getting participants for each event. event_id => ['IN' => [1, 2]] doesn't work - may be a 4.4 bug
    $participants = [];
    foreach ($event_ids as $event_id) {
      $res = $this->api('Participant', 'get', [
        'sequential' => FALSE,
        'event_id'   => $event_id,
        'options'    => ['limit' => 100000],
      ]);
      if ($res) {
        $participants = array_merge($participants, $res['values']);
      }
    }

    // Sort merged participants array by sort name
    uasort($participants, function ($a, $b) {
      return strcasecmp($a['sort_name'], $b['sort_name']);
    });

    // Add extra data if $extra is true
    if ($extra) {
      $participants = $this->addExtraData($participants, $locationType);
    }

    return $participants;
  }

  /**
   * Voegt extra data (status/rol-informatie en afdeling) toe aan een array met participant-records
   * @param array $participants Participants
   * @param string $locationType Type of afdeling-lookup: 'geo' for geostelsel or 'rel' for relationships
   * @return array Enriched participants array
   */
  private function addExtraData($participants, $locationType = 'geo') {

    // Status / rolinformatie opvragen, en vervolgens acties toevoegen en statusnamen aanpassen
    $statuses = $this->getAvailableStatuses();
    $roles = $this->getAvailableRoles();

    foreach ($participants as &$p) {

      // Statusnamen + acties aanpassen
      $p['participant_status_action'] = 'register';
      $p['participant_status_this'] = $statuses[ $p['participant_status_id'] ];

      switch ($p['participant_status_this']) {
        case 'Registered':
          $p['participant_status_this'] = 'Uitgenodigd';
          break;
        case 'Attended':
          $p['participant_status_this'] = 'Aanwezig';
          $p['participant_status_action'] = 'unregister';
          break;
        case 'No-show':
          $p['participant_status_this'] = 'Afwezig';
          break;
        case 'Cancelled':
        case 'Komt niet':
          $p['participant_status_this'] = 'Afgemeld';
          break;
      }

      // Rolnaam bij role id
      $p['participant_role_this'] = $roles[ $p['participant_role_id'] ];
    }

    // Afdelingscontacten ophalen
    $afdelingen_kv = $this->getAfdelingenKV();

    // Contactinfo ophalen obv contact-ids
    $contactIds = array_column($participants, 'contact_id');
    $afdelingField = 'custom_' . $this->getCustomFieldId('Geostelsel', 'Afdeling');
    $contacts = $this->api('Contact', 'get', [
      'sequential'   => FALSE,
      'contact_id'   => ['IN' => $contactIds],
      'return'       => ['first_name', 'middle_name', 'last_name', $afdelingField],
      'option.limit' => 1000,
    ]);

    if ($locationType == 'rel') {
      $relTypes = $this->getRelationshipTypes();
      $allowedRelTypes = $this->getAllowedRelationshipsKV();
      $crelinfo = $this->api('Relationship', 'get', [
        'sequential'           => FALSE,
        'is_active'            => 1,
        'contact_id_a'         => ['IN' => $contactIds],
        'relationship_type_id' => ['IN' => array_keys($allowedRelTypes)],
        'option.limit'         => 1000,
      ]);
      $relationships = [];
      foreach ($crelinfo['values'] as $crow) {
        if (!isset($relationships[ $crelinfo['contact_id_a'] ])) {
          $relationships[ $crelinfo['contact_id_a'] ] = [];
        }
        $crow['name_a_b'] = $relTypes[ $crow['relationship_type_id'] ]['name_a_b'];
        $crow['label_a_b'] = $relTypes[ $crow['relationship_type_id'] ]['label_a_b'];
        $relationships[ $crow['contact_id_a'] ][ $crow['name_a_b'] . '_' . $crow['contact_id_b'] ] = $crow;
      }
    }

    // Array verrijken met de contactdata, en deze teruggeven
    if (!$contacts['is_error'] && $contacts['count'] > 0) {
      foreach ($participants as &$p) {

        if (!array_key_exists($p['contact_id'], $contacts['values'])) {
          continue;
        }
        
        $crec = &$contacts['values'][ $p['contact_id'] ];
        $crel = &$relationships[ $p['contact_id'] ];
        $p['sort_name_this'] = trim($crec['last_name'] . ', ' . $crec['first_name'] . ' ' . $crec['middle_name']);

        // Locatienaam - beetje lang blok, toont een selectie van functies in 'rel'-modus, of de woonafdeling in 'geo'-modus
        if ($locationType == 'rel') {
          // Type rel
          if (isset($crel) && count($crel) > 0) {
            $location = [];
            foreach ($this->allowedRelationships as $allowedGroup) {
              foreach ($allowedGroup as $allowedName) {
                foreach ($crel as $rel) {
                  if ($rel['name_a_b'] != $allowedName || isset($location[$rel['contact_id_b']])) {
                    continue;
                  }

                  $location[$rel['contact_id_b']] = $rel['label_a_b'] . (array_key_exists($rel['contact_id_b'], $afdelingen_kv) ? ' ' . $afdelingen_kv[ $rel['contact_id_b'] ] : '');
                }
              }
            }

            if(count($location) > 0) {
              $location = implode("<br>\n", $location);
            } else {
              $location = '-';
            }
          } else {
            $location = '[Geen, woonafdeling: ' . $afdelingen_kv[ $crec[ $afdelingField ] ] . ']';
          }
        } else {
          // Type geo
          $location = $afdelingen_kv[ $crec[ $afdelingField ] ];
        }

        if (!empty($location)) {
          $location = str_replace(['SP-afdeling ', 'SP-regio ', 'SP-werkgroep ', 'afd '], '', $location);
          $location = str_replace(['Tweede Kamerfractie', 'Europese fractie', 'Personeelslid', 'Organisatiesecretaris'], ['Kamerfractie', 'Brussel', 'Personeel', 'Organisatiesecretaris', ''], $location);
          $p['location'] = $location;
        } else {
          $p['location'] = '-';
        }
      }
    }

    return $participants;
  }

  /**
   * Update participant status
   * @param int $participant_id Participant record ID
   * @param int $status_id Participant status ID
   * @return bool Success
   */
  public function updateParticipantStatus($participant_id, $status_id) {

    $res = $this->api('Participant', 'create', [
      'participant_id' => $participant_id,
      'status_id'      => $status_id,
    ]);

    return (bool) !$res['is_error'];
  }

  /**
   * Get single participant record from params
   * @param int $event_id Event ID
   * @param int $contact_id Contact ID
   * @param int $role_id Participant Role ID
   * @return array|bool Record or false
   */
  public function getParticipant($event_id, $contact_id, $role_id) {
    try {
      $participant = $this->api('Participant', 'getsingle', [
        'event_id'   => $event_id,
        'contact_id' => $contact_id,
        'role_id'    => $role_id,
      ]);

      return $participant['participant_id'];
    } catch (\CiviCRM_API3_Exception $e) {
      return FALSE;
    }
  }

  /**
   * Add participant
   * Controleert nu eerst of een aanmelding met zelfde event/contact/rol al bestaat, regardless of participant status.
   * Overschrijft deze als dit het geval is, maakt anders nieuw record aan.
   * @param int $event_id Event ID
   * @param int $contact_id Contact ID
   * @param int $role_id Participant Role ID
   * @return array|bool Return array or false
   */
  public function addParticipant($event_id, $contact_id, $status_id, $role_id) {
    try {
      $chk = $this->getParticipant($event_id, $contact_id, $role_id);
      if ($chk) {
        return FALSE;
      }

      return $this->api('Participant', 'create', [
        'event_id'   => $event_id,
        'contact_id' => $contact_id,
        'status_id'  => $status_id,
        'role_id'    => $role_id,
      ]);
    } catch (\CiviCRM_API3_Exception $e) {
      drupal_set_message('Kon deelnemer niet aanmelden: ' . $e->getMessage(), 'error');

      return FALSE;
    }
  }

  /**
   * Beschikbare participant roles ophalen - lekker makkelijk via de civi-functie
   * @return array Available roles
   */
  public function getAvailableRoles() {
    return \CRM_Event_PseudoConstant::participantRole();
  }

  /**
   * Beschikbare participant statuses ophalen - lekker makkelijk via de civi-functie
   * @return array Available statuses
   */
  public function getAvailableStatuses() {
    return \CRM_Event_PseudoConstant::participantStatus();
  }

  /**
   * Haalt alle contacten van het type SP_Afdeling op en cached deze.
   * @return array Alle afdelingen
   */
  private function getAfdelingen() {
    if (count($this->afdelingen) == 0) {
      $afdelingen = $this->api('Contact', 'get', [
        'contact_sub_type' => 'SP_Afdeling',
        'option.limit'     => 1000,
      ]);
      if ($afdelingen['is_error']) {
        return FALSE;
      }

      $this->afdelingen = $afdelingen['values'];
    }

    return $this->afdelingen;
  }

  /**
   * Geeft een K/V-array terug met alle afdelingen
   * @return array Alle afdelingen als K/V-array
   */
  private function getAfdelingenKV() {
    $afdelingen = $this->getAfdelingen();

    return array_column($afdelingen, 'display_name', 'contact_id');
  }

  /**
   * Haalt alle relatietypes op en cached deze.
   * @return array All relationship types
   */
  private function getRelationshipTypes() {
    if (count($this->relTypes) == 0) {
      $relTypes = $this->api('RelationshipType', 'get', [
        'sequential'   => FALSE,
        'is_active'    => 1,
        'option.limit' => 1000,
      ]);
      if ($relTypes['is_error']) {
        return FALSE;
      }

      $this->relTypes = $relTypes['values'];
    }

    return $this->relTypes;
  }

  /**
   * Retourneert relatietypes die gebruikt worden om een contact te koppelen aan een afdeling.
   * @return array Allowed relationship types (as K/V)
   */
  public function getAllowedRelationshipsKV() {

    $relTypes = $this->getRelationshipTypes();
    $ret = [];

    foreach ($relTypes as $r) {
      foreach ($this->allowedRelationships as $set) {
        if (in_array($r['name_a_b'], $set)) {
          $ret[ $r['id'] ] = $r['name_a_b'];
        }
      }
    }

    return $ret;
  }

}
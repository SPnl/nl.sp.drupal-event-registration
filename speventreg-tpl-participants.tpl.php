<div class="content" id="speventreg-content">

  <?php
  $ecount = count($events); // Get counts
  $pcount = count($participants); // Get counts
  $acount = 0; // Count attending in foreach loop
  $prev = NULL; // Check for doubles in loop
  ?>

  <div class="speventreg-blocks">

    <div id="speventreg-eventoverview">
      <p>Je kunt nu de aanwezigen inschrijven voor de volgende bijeenkomsten:<br/>
        <?php foreach ($events as $event): ?>
          - <?= $event; ?><br/>
        <?php endforeach; ?>
      </p>
    </div>

    <div id="speventreg-eventlinks">
      <p>
        <a href="/evenementregistratie" class="rood"><i class="fa fa-fw fa-pencil"></i> Instellingen wijzigen</a><br/>
        <br/>
        <a href="#" class="rood speventreg-refresh"><i class="fa fa-fw fa-refresh"></i> Lijst verversen</a>
      </p>
    </div>
  </div>

  <div class="clear"></div>

  <hr/>

  <div class="speventreg-blocks">

    <div id="speventreg-search">
      <form id="speventreg-search-form" action="#">
        <label for="speventreg-search-text"><strong>Lijst doorzoeken:</strong></label>
        <input type="text" id="speventreg-search-text" value="" placeholder="Zoek op lidnummer, naam of afdeling..."/>
        <input type="submit" id="speventreg-search-submit" value="Zoek"/>
        <input type="button" id="speventreg-search-reset" value="Wissen"/>
      </form>
    </div>

    <div id="speventreg-new-block">
      <a href="#" id="speventreg-new-open"><i class="fa fa-fw fa-user-plus"></i> Nieuwe deelnemer&hellip;</a>

      <div id="speventreg-new">
        <label for="speventreg-search-text" class="hide"><strong>Nieuwe deelnemer aanmelden:</strong></label>
        <a href="#" id="speventreg-new-close" class="fa fa-times"></a>

        <div id="speventreg-new-inner">
          <?php
          $form = drupal_get_form('speventreg_addparticipant_form');
          print drupal_render($form);
          ?>
        </div>
      </div>

      <div class="clear"></div>

    </div>

    <div class="clear"></div>
  </div>

  <?php if (count($participants) > 0): ?>
    <table id="speventreg-participants">
      <thead>
      <tr>
        <td>Regnr</td>
        <td>Deelnemer</td>
        <td><?=($location_type == 'rel' ? 'Kaderfunctie + afdeling' : 'Woonafdeling');?></td>
        <td class="mright">Status</td>
        <td class="mright">Rol</td>
        <td>Evenement</td>
        <td colspan="2">Acties</td>
      </tr>
      </thead>
      <tbody>
      <?php foreach ($participants as $p):
        $is_double = (is_array($prev) && $prev['contact_id'] == $p['contact_id']);
        ?>
        <tr
          data-searchdata="<?= strtolower($p['contact_id'] . ' ' . $p['display_name'] . ' ' . $p['location']); ?>"
          data-pid="<?= $p['participant_id']; ?>">
          <td class="regnr">
            <?php if ($is_double): ?>
              <i class="fa fa-level-up fa-flip-horizontal"></i> <i class="fa fa-fw fa-exclamation-circle"></i>
            <?php else: ?>
              <a class="infolink" href="/civicrm/contact/view?reset=1&cid=<?= $p['contact_id']; ?>" target="_blank">
                <?= $p['contact_id']; ?></a>
            <?php endif; ?>
          </td>
          <td class="bold">
            <a class="infolink"
               href="/civicrm/contact/view/participant?reset=1&id=<?= $p['participant_id']; ?>&cid=<?= $p['contact_id']; ?>&action=view&context=participant"
               target="_blank"><?= $p['sort_name_this']; ?></a>
          </td>
          <td><?=($is_double ? '' : $p['location']);?></td>
          <td class="status nowrap padright">
            <?php if ($p['participant_status_this'] == 'Aanwezig'): ?>
              <i class="fa fa-check fa-fw"></i>
            <?php elseif (in_array($p['participant_status_this'], ['Afgemeld', 'Afwezig'])): ?>
              <i class="fa fa-times fa-fw"></i>
            <?php else: ?>
              <i class="fa fa-question fa-fw"></i>
            <?php endif; ?>
            <?= $p['participant_status_this']; ?>
          </td>
          <td class="nowrap padright"><?= $p['participant_role_this']; ?></td>
          <?php if ($ecount > 1): /* Event title bij meer dan 1 event, zou eventueel ook nog met code kunnen. */ ?>
            <td>
            <?= str_replace(['Partijraad:', 'gasten en genodigden', 'partijbestuur'], ['PR', 'gasten', 'PB'], $p['event_title']); ?>
            </td><?php endif; ?>
          <td class="nowrap">
            <a href='#' class='speventreg-action-status nowrap' data-participantid="<?= $p['participant_id']; ?>"
               data-action="<?= $p['participant_status_action']; ?>">
              <?php if ($p['participant_status_action'] == 'register'): ?>
                <i class="fa fa-fw fa-user-plus"></i>&nbsp;Aanmelden
              <?php else: ?>
                <i class="fa fa-fw fa-user-times"></i>&nbsp;Afmelden
              <?php endif; ?>
            </a>
            <span class="hide wait"><i class="fa fa-cog fa-spin"></i> Bezig&hellip;</span>
          </td>
          <td>
            <a
              href='/civicrm/contact/view/participant?reset=1&id=<?= $p['participant_id']; ?>&cid=<?= $p['contact_id']; ?>&action=update&context=participant&selectedChild=event'
              class='nowrap' target='_blank'>
              <i class="fa fa-fw fa-pencil"></i> Bewerken</a>
          </td>
        </tr>
        <?php
        if ($p['participant_status_this'] == 'Aanwezig') {
          $acount ++;
        }
        $prev = $p;
        ?>
      <?php endforeach; ?>
      </tbody>
    </table>
  <?php endif; ?>

  <p><strong class="speventreg-foot" data-origtxt="Totaal <?= $pcount; ?> geregistreerde deelnemers, waarvan <?= $acount; ?> aanwezig (bij laatste refresh).">
      Totaal <?= $pcount; ?> geregistreerde deelnemers, waarvan <?= $acount; ?> aanwezig (bij laatste refresh).</strong></p>

  <hr/>

</div>
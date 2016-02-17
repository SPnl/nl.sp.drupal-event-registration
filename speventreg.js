/**
 * nl.sp.drupal-event-registration | Custom JS
 */

Drupal.behaviors.speventreg = {

    attach: function (context, settings) {

        jQuery(function ($) {

            /* Algemene ajax-errors afvangen (dit is nogal een oude JQuery-versie blijkbaar */
            $(document).ajaxError(function() {
                alert('Er is een fout opgetreden bij het verwerken van dit verzoek. Controleer de verbinding en probeer het opnieuw.');
                return false;
            });

            /* Refresh page */
            $('.speventreg-refresh').click(function(ev) {
                ev.preventDefault();
                location.reload();
            });

            /* Toggle zichtbaarheid zoekformulier, is verder een Drupal ajax form */
            $('#speventreg-new-open').click(function (ev) {
                ev.preventDefault();
                $('#speventreg-new-block').addClass('active');
                $('#speventreg-new').show();
                $(this).hide();
            });

            $('#speventreg-new-close').click(function(ev) {
                ev.preventDefault();
                $('#speventreg-new').hide();
                $('#speventreg-new-block').removeClass('active');
                $('#speventreg-new-open').show();
            });

            /* Links om aan / af te melden */
            $('.speventreg-action-status').click(function(ev) {

                ev.preventDefault();
                $link = $(this);

                var participant_id = $link.attr('data-participantid');
                var action = $link.attr('data-action');
                if(!participant_id || !action) {
                    alert('Ongeldige deelnemer-id of actie gespecificeerd.');
                    return false;
                }

                $link.hide().next('.wait').show();

                $.get('/evenementregistratie/status/' + participant_id + '/' + action + '/', function(data) {

                    // Dit is niet de aller-strakste oplossing, geef ik toe. Maar voor nu is het wel even goed...

                    if(data.error) {
                        $link.html('<i class="fa fa-exclamation-triangle"></i> Error');
                    } else if(action == 'register') {
                        $link.closest('tr').find('.status').html('<i class="fa fa-check fa-fw"></i> Aanwezig');
                        $link.closest('tr').find('.speventreg-action-replace').hide();
                        $link.html('<i class="fa fa-user-times"></i> Afmelden');
                        $link.attr('data-action', 'unregister');
                    } else {
                        $link.closest('tr').find('.status').html('<i class="fa fa-times fa-fw"></i> Afgemeld');
                        $link.closest('tr').find('.speventreg-action-replace').show();
                        $link.html('<i class="fa fa-user-plus"></i> Aanmelden');
                        $link.attr('data-action', 'register');
                    }

                    $link.show().next('.wait').hide();
                });
            });

            /* Links om iemand te vervangen, wordt wellicht een Drupal ajax form */
            $('.speventreg-action-replace').click(function(ev) {
                ev.preventDefault();
                alert('TODO ACTION REPLACE');
            });

            /* En de in page search-implementatie */
            var filterParticipants = function (val) {

                val = val.toLowerCase().trim();
                if(!val || val == '') {
                    $('#speventreg-participants tbody tr').show();
                    $('.speventreg-foot').html($('.speventreg-foot').attr('data-origtxt'));
                    return true;
                }

                $('#speventreg-participants tbody tr').hide();

                var foundRows = $('#speventreg-participants tbody tr[data-searchdata*=\"' + val + '\"]');
                foundRows.show();

                $('.speventreg-foot').html('Je zoekopdracht heeft ' + foundRows.length +
                    (foundRows.length == 1 ? ' resultaat ' : ' resultaten ') + 'opgeleverd.');

                return true;
            };

            $('#speventreg-search-text').keyup(function (ev) {
                filterParticipants($(this).val());
            });

            $('#speventreg-search-submit').click(function(ev) {
                filterParticipants($('#speventreg-search-text').val());
                ev.preventDefault();
            });

            $('#speventreg-search-form').submit(function (ev) {
                filterParticipants($('#speventreg-search-text').val());
                ev.preventDefault();
            });

            $('#speventreg-search-reset').click(function (ev) {
                filterParticipants('');
                $('#speventreg-search-text').val('');
                ev.preventDefault();
            });

        });
    }
};
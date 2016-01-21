nl.sp.drupal-event-registration
===============================

Deze module bevat views en formulieren om bij bijeenkomsten, zoals een partijraad, conferentie of debat,
ter plekke digitaal de aanwezigheid van deelnemers te registreren, nieuwe deelnemers toe te voegen of deelnemers
te laten vervangen. 

Deze module is in ontwikkeling. Geplande opzet (tevens TODO):
- Eén overzichtsformulier (dat moet dus helemaal geen view zijn) waarop de gebruiker 
  een of meer evenementen kan selecteren.
- Een detailpagina die de aangemelde deelnemers voor deze evenementen toont.
- Naast de gegevens van iedere deelnemer zit een knop Vervangen en een knop Afmelden, die (bij voorkeur via ajax)
  de betreffende handeling uitvoeren.
- Er komt nog een aparte knop Nieuwe deelnemer om deelnemers of gasten toe te voegen die nog niet eerder op de lijst
  van deelnemers stonden.
  
http://civicrmacc.sp.nl/evenementregistratie

<hr/>

### Is dat alles?

Nee, dan hebben we het volgende verhaal van Mathijs, om het makkelijker te maken:

Ik gebruikte de 3 evenementen om dubbelrollen eruit te halen: afdelingsvoorzitters die ook personeelslid en/of partijbestuurslid zijn. Voor de stemming ga ik er trouwens ook tegenwoordig gemakkelijk aan voorbij (een afdelingslijst minus de IO, en met gewicht 1/50 van ledental - en 1 stem per PB-lid is gemakkelijk in een excel te maken).

Van belang is dat we tijdens de inschrijving verschillende zaken eenvoudig kunnen zien (en eventueel gemakkelijk kunnen wijzigen):

1) dat en afdelingsvoorzitter die ook in het PB zit (en dus twee stemkaarten moet krijgen).  
2) dat een afdelingsvoorzitter of vervanger horen bij een afdeling die erkend is of die io is.  
3) dat als er al iemand zich namens een afdeling heeft gemeld en met stemrecht is ingeschreven, een tweede persoon uit die afdeling sowieso als gast ingeschreven moet worden

Op 3 hebben we als uitzonderingen: voorzitters die een aanpalende afdeling waarnemen o.a. (...)
<!-- Arie-Jan Boer (780710, voorzitter Maassluis, maar woont in Vlaardingen); Anna de Groot (437336, voorzitter Wormerland, maar woont in Zaanstreek); Maarten de Pijper (naast voorzitter Brielle neemt hij waar voor Hellevoetsluis); Vincent Mulder (voorzitter Hengelo, en neemt waar in Deventer), Wim Stolk (726087, neemt waar in Alkmaar, en Stede Broec, zit in het PB en woont in de afdeling SP-afdeling West-Friesland). En ik zal er nog we een paar vergeten zijn. -->

Probleem met enkelen van deze lui zal zijn, dat ze zich op de partijraad door een bestuurslid uit die aanpalende afdeling laten vervangen, terwijl ze wel voor hun eigen afdeling of als PB-lid aanwezig zijn. Voor de goede orde: voorzitters kunnen zich laten vervangen, maar PB-leden en gasten/waarnemers niet.

Ik hoop dat ik het inspirerend gecompliceerd maak, en jij het weer eenvoudig tot de juiste essentie weet terug te brengen.

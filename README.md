# Bom-rakkers

## Hardware
Er zijn drie ontmantel mechanismen: bananenklemmen, kleur draadjes en cijfer code.
-	De cijfer code zijn 10 knoppen van 0 t.e.m. 9 als ook een extra knop “OK”.
-	Er zijn 6 verschillende kleuren draadjes die ze moeten “doorknippen” of simpelweg uittrekken
-	Met 3 banaanklemdraden kan met in totaal 15 mogelijke combinaties maken door deze in 6 gaten te steken. 

### Cijfer code
![image](https://user-images.githubusercontent.com/86438012/163394508-205f8d51-a7b8-4f89-bd94-60aecbea3265.png)

Door de verschillende spanningsdelers zal er altijd een andere spanning over R15 gelezen worden. 
Vb. C5 is ingedrukt dus staat er nog maar 2.5 volt over R15. Dit lezen we uit met A5 op de arduino. Dat dan digitaal omgezet wordt naar een waarde van 0-1023. 

### Kleuren draadjes 
![image](https://user-images.githubusercontent.com/86438012/163394743-5a508a85-de5a-47c9-acad-4a3fdf4e0aa3.png)

Het principe is vrij eenvoudig. Als er een draadje verwijdert wordt (of in ideale doorgeknipt) zoals hier de rode draad (schakelaar = open), dan wordt de weerstand niet meer langer kortgesloten en is er dus wel een spanningsdeler met bovenste weerstand gelijk aan 0 Ohm. De spanning lezen we opnieuw af in een analoge input A4.

### Bananenklemmen
![image](https://user-images.githubusercontent.com/86438012/163394933-ca2be7c5-6bcd-40be-be04-6cf90e1b16da.png)

De groene gaten zijn de connecties waar je de klemmen kan insteken. Hierdoor sluit je dus een aantal weerstanden kort, waardoor de uiteindelijke spanningen op de 4 bovenste groene bollen telkens zal veranderen. 

![image](https://user-images.githubusercontent.com/86438012/163403146-cb3d7c0c-d159-4b3e-b0d6-939e4c4790e8.png)


Uit simulaties, en getest in de praktijk, is gebleken dat wanneer we slechts 4 spanningen meten (dus de bovenste groene bollen) we 100% zeker een onderscheid kunnen maken in de 15 verschillende combinaties.  De 4 spanningen lezen we af met A0, A1, A2, A3, vier analoge ingangen, zoals de vorige 2 mechanismen. 

De 15 verschillende combinaties zijn:
![image](https://user-images.githubusercontent.com/86438012/163395024-924cecdd-82e7-492b-be83-ca1b9b086308.png)

### 3 Groene LED’s
Er zullen nog 3 groene lichtjes zijn die aanduiden als een ontmanteling mechanisme correct is gedaan (een van de bovenstaande drie). Als alle drie de lichtjes dus branden zal de bom ontmanteld zijn.
Deze worden aangesloten op een digitale output: D3, D9, D10. En staan in serie met een weerstand van 220 Ohm. Moet nog getest worden van helderheid!

### Rode LED + buzzer
Voor de tikkende tijdbom aan te geven, zal gebruikt worden gemaakt van een rode LED parallel over een buzzer. Dit kan zo een korte hoge piep veroorzaken tegelijk met een rood lichtje als indicator dat de bom aan het aftellen is. Hier is 1 digitale uitgang voor nodig. Welke?

### LCD Display
Hierop kunnen we tekst printen 16x2 van karakter dimensies. Hier zijn 6 digitale poorten voor nodig.

---

## Software
Bij het opstarten van de Arduino moet eerst de Setup gebeuren voor dat de bom begint af te tellen. Dit gebeurt in een aantal stappen:

-	“Select Game Mode”
Druk 1 voor “Defuse” : dit kan voor een soort escape room gebruikt worden, waar de bom direct begint af te tellen en ze al de drie mechanismen correct moeten uitvoeren om de bom te ontmantelen. 
Druk 2 voor “Plant & Defuse” : dit is zoals CSGO waar eerst nog een team de bom moet planten alvorens het andere team de bom kan ontmantelen. 

#### INDIEN 1 “Defuse” geselecteerd is:
-	“Select Time”
Hier geef je het aantal minuten in dat je het spel wilt spelen (max 99 minuten). En daarna druk je op “OK” om verder te gaan. 
-	“Kies Preset”
Hier kiest men de preset (die gehard code ingebakken is). Dit is een oplossing, dus code + kleur draadje + bananenklem combinatie. Zo kan men kiezen tussen 10 verschillende oplossingen. Indien je er specifiek wil, kan je ze altijd zelf her programmeren.
-	“Game starts in:”
De teller begint af te tellen van 10 naar 1, en hierna start de echte timer (op voorhand aan gegeven in Select Time) die dan naar 0 telt, met de buzzer en rode LED om het tikken aan te geven. Vanaf dan kan het spel echt beginnen.

#### INDIEN 2 “PLANT & DEFUSE” geselecteerd is:
-	“Random code” -> geeft random getal (=activatie code) terug. Dit onthoudt de leiding.
-	“Select time”
Ook nog is tijd selecteren hoe lang het duurt voor bom ontploft. 
Kan misschien in één stap gebeuren voor de game mode geselecteerd wordt? 
-	“Code to plant”
Om de bom te planten moet je eerst de activatie code invoeren. Deze code ligt op de plaats om de bom te planten. Zo kan men deze nog niet activeren al vorens de bom op de plaats delict is. 
-	“Press OK for 20 seconds”
Dit is nodig om te bom echt te planten en de timer te starten. Deze reset altijd als je nooit volledig tot 20 bent gegaan in één druk. Hierna is de bom geplant en kan men deze defusen.
-	“Planted”
-	Timer die aftikt!
-	“Press OK for 10 seconds to defuse”
Zelfde principe om de ontmanteling te starten. 
-	Vanaf dan kan men via instructies of wiskunde sommetjes de bom terug ontmantelen. Als ze dit binnen de tijd kunnen doen, is de bom ontmanteld en anders ontploft ze.

# Servo aansturingstest

Deze test heeft als doel een functie schrijven die op commanodo de arm van uitgestrekte naar ingeplooide toestand kan verplaatsen volgens een gecontroleerd pad. Er wordt gewerkt met een duuwknop om de trigger te initiëren.

**V2:**
De tweede versie werd de code aangepast om dezelfde functie te kunnen gebruiken voor twee of meerdere servo's. Dit werd gedaan door het servo en ramp object te importeren in de functie. 
Ook de in en uit posititie werd als input van de functie geschreven om fouten in de servo aansturing te corrigeren.
Extra werd de start postitie geschreven m.b.v. de Ramp functie's.

**In deze code werden volgende librarys gebruikt:**
- **Servo library[^1]**
De Arduino Servo library maakt het eenvoudig om servomotoren aan te sturen via een PWM-signaal (Pulse Width Modulation). Met functies zoals `attach()` en `write()` kun je een servo koppelen aan een pin en de positie instellen in graden (0–180°). Intern beheert de library de pulsbreedte (500–2500 µs) die bepaalt naar welke hoek de servomotor draait.

- **Ramp library[^2]**
De Arduino Ramp library zorgt voor vloeiende, geïnterpoleerde overgangen tussen twee waarden over een bepaalde tijd.Dit wordt vaak gebruikt om vloeiende bewegingen mogelijk te maken. Met functies zoals go() stel je een doelwaarde en duur in, waarna de library automatisch de tussenliggende waarden berekent op basis van verschillende easing-functies (zoals lineair, sinus of exponentieel). Je roept simpelweg update() aan in je loop() om de huidige waarde op te halen, zonder zelf timers of wiskunde te hoeven beheren.



[^1]:https://github.com/arduino-libraries/servo
[^2]:https://github.com/siteswapjuggler/RAMP



 ## Schakeling
![Schema](servo_schakeling.png)

 ## Blokschema
![Schema](Servo_BlokSchema.jpg)







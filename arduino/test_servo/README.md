# Servo aansturingstest

Deze test heeft als doel een functie schrijven die op commanodo de arm van uitgestrekte naar ingeplooide toestand kan verplaatsen volgens een gecontroleerd pad. Er wordt gewerkt met een duuwknop om de trigger te initiëren.

**V2:**
De tweede versie werd de code aangepast om dezelfde functie te kunnen gebruiken voor twee of meerdere servo's. Dit werd gedaan door het servo en ramp object te importeren in de functie. 
Ook de in en uit posititie werd als input van de functie geschreven om fouten in de servo aansturing te corrigeren.
Extra werd de start postitie geschreven m.b.v. de Ramp functie's.

**In deze code werden volgende librarys gebruikt:**
- Servo library[^1]
- Ramp library[^2]
[^1]:https://github.com/arduino-libraries/servo
[^2]:https://github.com/siteswapjuggler/RAMP

 ## Schakeling
![Schema](servo_schakeling.png)

 ## Blokschema
![Schema](Servo_BlokSchema.jpg)







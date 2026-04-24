# Pommeldoro timer

Dit project is een pommodoro timer met een hoekje af, eerder een vlakje af. Deze pommeldoro timer toont zijn status niet met flitsende lampjes, neen, het toont het door te schommelen naar zijn andere vlak wanneer de timer afloopt.

*De volledige demo is bekijken via deze link.*
[![Pommeldoro demo](/imgs/Pommeldoro_v1.gif)](/imgs/Pommeldoro_v1.mp4)

## Contributors
- Otto Vandroogenbroeck

## Inhoud:
- Test scripts met servo en ramp library [Servo Test](/arduino/test_servo/README.md)
- Test scripts met timer logica [Timer test](/arduino/timer_test/README.md)

## Gebruikte libraries
- **Servo library[^1]**
De Arduino Servo library maakt het eenvoudig om servomotoren aan te sturen via een PWM-signaal (Pulse Width Modulation). Met functies zoals `attach()` en `write()` kun je een servo koppelen aan een pin en de positie instellen in graden (0–180°). Intern beheert de library de pulsbreedte (500–2500 µs) die bepaalt naar welke hoek de servomotor draait.

- **Ramp library[^2]**
De Arduino Ramp library zorgt voor vloeiende, geïnterpoleerde overgangen tussen twee waarden over een bepaalde tijd.Dit wordt vaak gebruikt om vloeiende bewegingen mogelijk te maken. Met functies zoals go() stel je een doelwaarde en duur in, waarna de library automatisch de tussenliggende waarden berekent op basis van verschillende easing-functies (zoals lineair, sinus of exponentieel). Je roept simpelweg update() aan in je loop() om de huidige waarde op te halen, zonder zelf timers of wiskunde te hoeven beheren.



[^1]:https://github.com/arduino-libraries/servo
[^2]:https://github.com/siteswapjuggler/RAMP
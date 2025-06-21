# Beleg_mc_Cubefield

Das Ziel von Cubefield ist es, den Würfeln, die auf den Spieler zu kommen, auszuweichen und so eine mögichst weite Distanz zurückzulegen.

Während des Spiels wird die Geschwindigkeit, mit der der Sieler sich bewegt, immer weiter erhöht (bis sie ein Maximum erreicht). Wenn es ihm zu schnell wird kann er die Fähigkeit verwenden (sofern sie aufgeladen ist), mit der die Geschwindigkeit für eine kurze Zeit (4,8s) verringert werden kann.

## Funktionsweise
Das Timing der ganzen Aktionen wir über Timer-Interrupts gelöst, die alle 20ms ausgelöst werden und somit die "Spielticks" hochzählen. Wenn ein Zähler einen bestimmten Wert, der zum erhöhen der Geschwindigkeit verändert werden kann, erreicht, wird ein neues Bild berechnet und angezeigt. Die Positionen der Würfel sind in einem Array gespeichert, das immer wieder von vorne durchlaufen wird, wodurch auch die Würfel immer wieder an den gleichen Positionen erscheinen. Um das zu beheben gibt es im [Demoprogramm](src/random_modules_demo.c) mehrere handkonstruierte Module, die zufällig aneinandergereiht werden können und in der Konsole als Spielfeld ausgegeben werden.


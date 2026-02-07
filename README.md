## DETTAGLI IMPORTANTI SULLO STATO ATTUALE DEL PROGETTO
### BUG
+ `CTRL+C` invia un segnale di kill al processo padre che esegue il nodo `JoyActionClient`, ma la chiusura del terminale da UI no e in questo caso ritroviamo `/joypad_actionclient` come nodo attivo (`ros2 node list`)
+ La tabella della mappatura dei tasti alle azioni viene tagliata a metà da altri print di log
  
  Sarebbe il caso di salvare la tabella in una stringa e stamparla tutta insieme, invece di fare delle print separate

### MODIFICHE
+ Modifica il gestore dei parametri in modo che button sia una stringa e non un intero
  
  Prima di fare questo sarebbe il caso capire se diversi controllori avessero diverse mappature di tasti

+ Alla pressione di un bottone corrisponde lo spawn di un thread per l'action client associato all'azione corrispondente a quel bottone
  
  Tendenzialmente preferiremmo avere un numero limitato di thread spawnabili oppure un numero fissato a compilazione e che matchi sufficentemente bene quello degli actionclient da gestire

+ Fare in modo che premendo START venga stampata a schermo la tabella della mappatura dei tasti (indice in `buttons` vs. bottone), mentre premendo SELECT venga stampata solo la mappa dei bottoni mappati per il nodo `JoyActionClient`
  
  Potremmo anche usare SELECT per rimappare i bottoni con uno schema di questo tipo:
  - premi SELECT
  - premi il tasto dell'azione `<act>` da rimappare (e.g. di default `arm` è 0, cioè A)
  - premi il tasto `<btn>` a cui rimappare l'azione (e.g. X, cioè 1)
  - invoca `param set <self node name> <act>.button <btn>`

  Rimappare un bottone a 2 azioni distinte non crea conflitti e in quel caso verranno azionati ambi client (leggere i vari `NOTE:` nei sorgenti per qualche dettaglio o nota in più)

### OSSERVAZIONI GENERALI
+ La lunghezza della coda dei "messaggi" in entrata ad un nodo è impostabile tramite la `QoS`: `QoS = 10` significa che più di 10 "messaggi" concorrentemente non è in grado di gestirli
    
  Se spammando tasti con il controller qualche action client dovesse andare in cancelling è perchè è scaduto il timeout o perchè la coda è piena

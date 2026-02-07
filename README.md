## DETTAGLI IMPORTANTI SULLO STATO ATTUALE DEL PROGETTO
### BUG
+ `CTRL+C` invia un segnale di kill al processo padre che esegue il nodo `JoyActionClient`, ma la chiusura del terminale da UI no e in questo caso ritroviamo `/joypad_actionclient` come nodo attivo (`ros2 node list`)

### MODIFICHE
+ Alla pressione di un bottone corrisponde lo spawn di un thread per l'action client associato all'azione corrispondente a quel bottone
  
  Tendenzialmente preferiremmo avere un numero limitato di thread spawnabili oppure un numero fissato a compilazione e che matchi sufficentemente bene quello degli actionclient da gestire

+ Modificare il gestore dei parametri in modo che button sia una stringa e non un intero potrebbe essere tosta: diversi controller hanno diverse mappature di tasti (e.g. il controller PS3 di L ha all'indice 5 START, F parrebbe avere L1)
  
  Un config yaml (non necessariamente gestiti da ROS2) di questo tipo `vvv` potrebbe implementare questo con la giusta logica:  
  
  ```yaml
  buttons:
    - "A"
    - "X"
    - "B"
    - "Y"
    - "START"
    ...
  ```

### OSSERVAZIONI GENERALI
+ La lunghezza della coda dei "messaggi" in entrata ad un nodo è impostabile tramite la `QoS`: `QoS = 10` significa che più di 10 "messaggi" concorrentemente non è in grado di gestirli
    
  Se spammando tasti con il controller qualche action client dovesse andare in cancelling è perchè è scaduto il timeout o perchè la coda è piena

+ Premendo START viene stampata a schermo la tabella della mappatura dei tasti (indice in `buttons` vs. bottone), mentre premendo SELECT venga stampata solo la mappa dei bottoni mappati per il nodo `JoyActionClient`
  
  Potremmo anche usare SELECT per rimappare i bottoni con uno schema di questo tipo:
  - premi SELECT
  - premi il tasto dell'azione `<act>` da rimappare (e.g. di default `arm` è 0, cioè A)
  - premi il tasto `<btn>` a cui rimappare l'azione (e.g. X, cioè 1)
  - invoca `param set <self node name> <act>.button <btn>`

  Rimappare un bottone a 2 azioni distinte non crea conflitti e in quel caso verranno azionati ambi client (leggere i vari `NOTE:` nei sorgenti per qualche dettaglio o nota in più)

NOTE & TODO & ROADMAP:
+ l' "action client" che dobbiamo implementare è un nodo che gestisce 4 azioni
+ le azioni sono descritte nella lezione 5 di ROS2 e sono un paradigma di comunicazione client-server in cui il client richiede il compimento di un goal al server e riceve da quest ultimo risultati intermedi (i feedback) o risultato completo del goal (result)
+ una azione è un singolo diagramma a stati che descrive come queste comunicazioni vengono portate avanti, il che vuol dire che noi dobbiamo implementarne ben 4 di questi 
+ in realtà non dobbiamo implementare proprio tutte le possibili interazioni client-server in quanto 1. il server già lo abbiamo (è il test_server che i ragazzi ci hanno fornito) 2. una data azione potrebbe essere interessata ad una parte del diagramma a stati: leggendo, per esempio il sorgente di Arm.action si vede che solo la sezione RESULT ha qualcosa (quindi per l'azione Arm il client dovrà soltanto ricevere dal server il risultato finale senza che richieda il goal, richieda di cancellarlo e così via; mentre Landing e Takeoff sono più complicati e un diagramma a stati più completo lo inventiamo noi [con un po' di raziocinio] o lo chiediamo ai ragazzi del lab)
+ complessivamente per la parte azioni noi dovremmo scrivere tutte le callbacks di cui abbiamo bisogno, ciascuna per una possibile interazione client->server e un codice sorgente che ccommuti tutte le altre in unico eseguibile
+ per quanto riguarda l'impostare l'altitudine nel Takeoff e Landing, in Landing.action e Takeoff.Action ci sono due messaggi con formato geometry_msgs/msg/PoseStamped e questo ha campi 
		std_msgs/Header header
		Pose pose
e Pose a sua volta è
		Point position
		Quaternion orientation
e Point è
		float64 x
		float64 y
		float64 z
quindi abbiamo già tutto quello che ci serve (quando i ragazzi del lab hanno scritto nella traccia di "aggiungere l'altitudine" intendevano solo nel file config/joypad_actionclinet_parameters.yaml)
+ il CMakeLists.txt in src/joypad_actionclient dovrebbe andar bene così com'è e lavoreremo quasi solo esclusivamente in src/joypad_actionclient/src/\*.cpp e/o src/joypad_actionclient/include/\*.hpp (se definiremo degli header. In tal caso potremmo dover aggiungere una riga in CMakeLists.txt... questa cosa la faremo solo se avremmo errori di compilazione)
+ per quanto riguarda la parte subscriber, ho visto che il codice che GPT ci ha dato già ha qualcosa per il subscribing a /joy ma è da investigare (sarebbe molto figo e comodo se fosse già così, bell'e fatto ahahah)
+ in ogni caso dobbiamo capire, per questa parte del subscriber quali sono gli indici dei bottoni che vogliamo rimappare (se facessimo la subscription separata potremmo automatizzare il processo di selezione de-l/-i botton-e/-i

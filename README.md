# STM32 HAL Modular Drivers (LED, Button & Queue)

Una raccolta di driver modulari in C, scritti secondo i paradigmi della programmazione orientata agli oggetti e dell'astrazione hardware, pensati per lo sviluppo embedded su microcontrollori STM32 tramite le librerie HAL (Hardware Abstraction Layer).

Questa libreria offre soluzioni robuste, deterministiche e totalmente non bloccanti (senza l'uso di HAL_Delay).

---

## Struttura del Progetto
La libreria è organizzata secondo il layout standard dei progetti STM32Cube:

```text
libreria/
├── Inc/
│   ├── led.h         # Dichiarazione dell'oggetto led_t e delle API di controllo
│   ├── button.h      # Dichiarazione dell'oggetto button_t e delle API di lettura
│   ├── queue.h       # Dichiarazione dell'oggetto queue_t (Buffer Circolare FIFO)
│   ├── timer.h       # Dichiarazione dell'oggetto timer_t (Timer/PWM/Duty Helper)
│   ├── servo.h       # Dichiarazione dell'oggetto servo_t e delle API del servomotore
│   ├── fsm_logic.h   # Dichiarazione dello scheletro della Macchina a Stati (FSM)
│   └── ds3231_oop.h  # Driver RTC DS3231 ad Oggetti (Lettura atomica ed efficiente)
├── Src/
│   ├── led.c         # Implementazione del driver LED con astrazione logica
│   ├── button.c      # Implementazione del driver Button con debounce non bloccante
│   ├── queue.c       # Implementazione della coda FIFO generica e thread-safe
│   ├── timer.c       # Implementazione delle API di configurazione dei timer hardware
│   ├── servo.c       # Implementazione del driver servo (indipendente da ARR)
│   ├── fsm_logic.c   # Template per l'evoluzione degli stati FSM
│   └── ds3231_oop.c  # Driver RTC DS3231 con gestione I2C non bloccante ad oggetti
└── README.md         # Questa documentazione
```

---

## Caratteristiche Principali

### 1. Astrazione Hardware Totale (Logical Mapping)
Il codice separa il concetto logico (es. pulsante premuto) dal livello elettrico del pin fisco (es. 0 Volt / RESET). 
* **LED Attivi Bassi o Attivi Alti:** Se il tuo LED si accende a livello basso (0V), ti basta cambiare una sola riga di mappatura in `led.c` senza toccare la logica applicativa nel `main.c`.
* **Pulsanti in Pull-Up o Pull-Down:** La lettura fisica del pulsante in Pull-Up (0V = premuto, 3.3V = a riposo) viene raddrizzata automaticamente in uno stato logico lineare (`BUTTON_PRESSED` = 1, `BUTTON_RELEASED` = 0).

### 2. Debounce Software Non Bloccante
Il driver del pulsante implementa un algoritmo antirimbalzo (debounce) asincrono basato sulla funzione `HAL_GetTick()`. Le oscillazioni elettriche iniziali dei contatti metallici vengono filtrate via software senza congelare la CPU.

### 3. Sicurezza ed Incapsulamento (MISRA C Friendly)
* **Controllo dei puntatori:** Tutte le funzioni controllano preventivamente se i puntatori ad oggetto passati sono `NULL`, prevenendo crash catastrofici del firmware (HardFault).
* **Determinismo all'avvio:** Nessuna variabile locale viene lasciata non inizializzata, eliminando comportamenti imprevedibili dovuti a residui casuali nella RAM all'accensione della scheda.

### 4. Coda FIFO Asincrona Sicura e Configurabile (Thread-Safe)
Il modulo `queue` implementa un buffer circolare generico (tramite `memcpy` e puntatori `void*`) con protezione concorrente per l'uso ad interrupt. Supporta due modalità operative configurabili tramite `queue_set_mode()`:
* **`QUEUE_MODE_REJECT` (Default):** Se la coda è piena, rifiuta i nuovi inserimenti restituendo `QUEUE_FULL`. Ideale per non perdere messaggi o comandi di controllo (es. gli override del semaforo).
* **`QUEUE_MODE_OVERWRITE`:** Se la coda è piena, il nuovo dato inserito sovrascrive automaticamente quello più vecchio (facendo avanzare la testa `head` di conseguenza). Ideale per buffer di dati continui (es. medie mobili di sensori) in cui contano solo le letture più fresche.
* **Sicurezza nei contesti ad Interrupt:** Tutte le funzioni critiche disabilitano temporaneamente gli interrupt (`__disable_irq()`), rendendo le modifiche degli indici atomiche e protette da preemption concorrente.

### 5. Controllo Servomotore Autoadattativo (SG90)
Il modulo `servo` implementa la gestione del servomotore SG90 tramite segnale PWM a 50 Hz.
* **Indipendenza dall'ARR hardware:** Il calcolo del ciclo di lavoro (Duty Cycle) per impostare i gradi di rotazione (da 0° a 180°, mappati su impulsi da 0.5 ms a 2.5 ms) legge a runtime il registro di auto-reload del timer (`ARR`) tramite la macro `__HAL_TIM_GET_AUTORELOAD`. Modifiche della frequenza di clock o della risoluzione del timer in CubeMX non richiedono quindi alcuna modifica al codice.
* **Limitazione software e sicurezza:** Il driver impone limiti software configurabili (`servo_set_bounds()`) per prevenire rotazioni oltre i limiti meccanici del servo, riducendo lo stress meccanico e i picchi di corrente.

---

## Integrazione in STM32CubeIDE

1. Copia la cartella `libreria` all'interno dell'albero delle directory del tuo progetto STM32.
2. In STM32CubeIDE, fai tasto destro sulla cartella di progetto -> **Properties**.
3. Naviga su **C/C++ Build** -> **Settings** -> **Tool Settings** -> **MCU GCC Compiler** -> **Include paths**.
4. Clicca sull'icona **Add...** e aggiungi il percorso relativo della cartella `libreria/Inc` (es. `../libreria/Inc`).
5. Assicurati che la cartella `libreria/Src` sia inclusa tra le cartelle sorgenti per la compilazione (solitamente STM32CubeIDE la rileva automaticamente).

---

## Crediti e Riconoscimenti

* **Driver RTC DS3231 (`ds3231_for_stm32_hal`):** Questo modulo è basato sulla libreria originale sviluppata da `@eepj` (disponibile su [GitHub/eepj](https://github.com/eepj)). È stata adattata e migliorata dall'autore per supportare maschere specifiche per i registri degli allarmi (Alarm1, Alarm2) e per l'abilitazione del pin a 32kHz. Queste ottimizzazioni sono state proposte dall'autore sotto forma di Pull Request nel repository originale e integrate in questa raccolta in attesa di un'eventuale approvazione (data la potenziale inattività del maintainer originale).
* **Driver LED, Button, Queue, Timer e template FSM:** Questi moduli derivano dal materiale didattico fornito dall'**Università degli Studi di Salerno (UNISA)**. I driver sono ottimizzati, integrati ed estesi dall'autore per la risoluzione dei compiti d'esame e lo sviluppo dei vari progetti accademici.

---

## Licenza
Questo progetto è rilasciato sotto licenza MIT. Sentiti libero di usarlo, modificarlo e distribuirlo nei tuoi progetti commerciali o personali.

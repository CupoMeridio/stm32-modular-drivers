# STM32 HAL Modular Drivers (LED, Button & Queue) 🚀

Una raccolta di driver modulari in C, scritti secondo i paradigmi della **programmazione orientata agli oggetti** e dell'**astrazione hardware**, pensati per lo sviluppo embedded su microcontrollori STM32 tramite le librerie **HAL (Hardware Abstraction Layer)**.

Questa libreria nasce per superare le comuni "code smell" dello sviluppo didattico, offrendo soluzioni robuste, deterministiche e totalmente **non bloccanti** (senza l'uso di `HAL_Delay`).

---

## 📂 Struttura del Progetto
La libreria è organizzata secondo il layout standard dei progetti STM32Cube:

```text
libreria/
├── Inc/
│   ├── led.h         # Dichiarazione dell'oggetto led_t e delle API di controllo
│   ├── button.h      # Dichiarazione dell'oggetto button_t e delle API di lettura
│   ├── queue.h       # Dichiarazione dell'oggetto queue_t (Buffer Circolare FIFO)
│   ├── timer.h       # Dichiarazione dell'oggetto timer_t (Timer/PWM/Duty Helper)
│   ├── fsm_logic.h   # Dichiarazione dello scheletro della Macchina a Stati (FSM)
│   └── ds3231_oop.h  # Driver RTC DS3231 ad Oggetti (Lettura atomica ed efficiente)
├── Src/
│   ├── led.c         # Implementazione del driver LED con astrazione logica
│   ├── button.c      # Implementazione del driver Button con debounce non bloccante
│   ├── queue.c       # Implementazione della coda FIFO generica e thread-safe
│   ├── timer.c       # Implementazione delle API di configurazione dei timer hardware
│   ├── fsm_logic.c   # Template per l'evoluzione degli stati FSM
│   └── ds3231_oop.c  # Driver RTC DS3231 con gestione I2C non bloccante ad oggetti
└── README.md         # Questa documentazione
```

---

## 🌟 Caratteristiche Principali

### 1. Astrazione Hardware Totale (Logical Mapping)
Il codice separa il concetto logico (es. *pulsante premuto*) dal livello elettrico del pin fisco (es. *0 Volt / RESET*). 
*   **LED Attivi Bassi o Attivi Alti:** Se il tuo LED si accende a livello basso (0V), ti basta cambiare una sola riga di mappatura in `led.c` senza toccare la logica applicativa nel `main.c`.
*   **Pulsanti in Pull-Up o Pull-Down:** La lettura fisica del pulsante in Pull-Up (0V = premuto, 3.3V = a riposo) viene raddrizzata automaticamente in uno stato logico lineare (`BUTTON_PRESSED` = 1, `BUTTON_RELEASED` = 0).

### 2. Debounce Software Non Bloccante
Il driver del pulsante implementa un algoritmo antirimbalzo (debounce) asincrono basato sulla funzione `HAL_GetTick()`. Le oscillazioni elettriche iniziali dei contatti metallici vengono filtrate via software senza congelare la CPU.

### 3. Sicurezza ed Incapsulamento (MISRA C Friendly)
*   **Controllo dei puntatori:** Tutte le funzioni controllano preventivamente se i puntatori ad oggetto passati sono `NULL`, prevenendo crash catastrofici del firmware (**HardFault**).
*   **Determinismo all'avvio:** Nessuna variabile locale viene lasciata non inizializzata, eliminando comportamenti imprevedibili dovuti a residui casuali nella RAM all'accensione della scheda.

### 4. Coda FIFO Asincrona Sicura e Configurabile (Thread-Safe)
Il modulo `queue` implementa un buffer circolare generico (tramite `memcpy` e puntatori `void*`) con protezione concorrente per l'uso ad interrupt. Supporta due modalità operative configurabili tramite `queue_set_mode()`:
*   **`QUEUE_MODE_REJECT` (Default):** Se la coda è piena, rifiuta i nuovi inserimenti restituendo `QUEUE_FULL`. Ideale per non perdere messaggi o comandi di controllo (es. gli override del semaforo).
*   **`QUEUE_MODE_OVERWRITE`:** Se la coda è piena, il nuovo dato inserito sovrascrive automaticamente quello più vecchio (facendo avanzare la testa `head` di conseguenza). Ideale per buffer di dati continui (es. medie mobili di sensori) in cui contano solo le letture più fresche.
*   **Sicurezza nei contesti ad Interrupt:** Tutte le funzioni critiche disabilitano temporaneamente gli interrupt (`__disable_irq()`), rendendo le modifiche degli indici atomiche e protette da preemption concorrente.

---

## 🔧 Integrazione in STM32CubeIDE

1. Copia la cartella `libreria` all'interno dell'albero delle directory del tuo progetto STM32.
2. In STM32CubeIDE, fai tasto destro sulla cartella di progetto ➡️ **Properties**.
3. Naviga su **C/C++ Build** ➡️ **Settings** ➡️ **Tool Settings** ➡️ **MCU GCC Compiler** ➡️ **Include paths**.
4. Clicca sull'icona **Add...** e aggiungi il percorso relativo della cartella `libreria/Inc` (es. `../libreria/Inc`).
5. Assicurati che la cartella `libreria/Src` sia inclusa tra le cartelle sorgenti per la compilazione (solitamente STM32CubeIDE la rileva automaticamente).

---

## 💻 Esempio di Utilizzo (Quick Start)

Ecco un esempio completo di come orchestrare un LED ed un pulsante all'interno del file `main.c`:

```c
#include "main.h"
#include "gpio.h"
#include "led.h"
#include "button.h"

int main(void)
{
  // 1. Inizializzazioni Hardware generate da CubeMX
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  // 2. Dichiarazione e Inizializzazione degli oggetti driver
  led_t green_led;
  button_t blue_button;

  // Inizializziamo in modo sicuro lo stato logico di partenza
  button_state_t button_state = BUTTON_RELEASED;
  button_state_t last_button_state = BUTTON_RELEASED;

  // Associazione dei pin fisici e impostazione dei parametri di funzionamento
  led_init(&green_led, LED_GPIO_Port, LED_Pin, LED_OFF);
  button_init(&blue_button, BUTTON_GPIO_Port, BUTTON_Pin, BUTTON_RELEASED);
  button_set_delay(&blue_button, 100); // 100 ms di tempo di debounce

  // 3. Loop principale (Non bloccante)
  while (1)
  {
    // Memorizziamo lo stato del ciclo precedente
    last_button_state = button_state;

    // Lettura filtrata del pulsante (non bloccante)
    button_read(&blue_button, &button_state);

    // Rilevamento del fronte di salita (Trigger all'istante esatto del click fisco)
    if ((button_state == BUTTON_PRESSED) && (last_button_state != button_state))
    {
      led_toggle(&green_led); // Esegue il toggle del LED
    }
  }
}
```

---

## 📝 Licenza
Questo progetto è rilasciato sotto licenza MIT. Sentiti libero di usarlo, modificarlo e distribuirlo nei tuoi progetti commerciali o personali.

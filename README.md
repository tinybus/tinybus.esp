<!-- SPDX-FileCopyrightText: Copyright 2025 Clever Design (Switzerland) GmbH ;
SPDX-License-Identifier: GPL-3.0-only OR LicenseRef-TinyBus-Software-1.0 -->
# TinyBus

TinyBus is a lightweight library that combines a **state machine** for managing system behavior with a **message broker** for inter-component communication. This approach enhances modularity and simplifies the development of complex embedded systems. Based on 20+ years of experience developing microcontroller projects (ESP32, STM32, nRF) with frameworks like ESP-IDF and Zephyr, TinyBus offers a proven and scalable architecture.

This repository provides the **ESP-IDF implementation**, which can be installed via the Espressif Component Registry or by cloning it into your components directory.

## How It Works

TinyBus is implemented as a singleton and internally uses a **FreeRTOS** queue and task for event handling.

### Example: Controlling an LED with a Button Press

Assume you have a development kit where you want to turn on an LED (`led.c`) when a button is pressed (`button.c`).

#### `led.h`
Define the **LED_EVENT_LED1_ON** event in the header file to allow `button.c` to trigger it:

```c
// Include TinyBus
#include <tinybus/tinybus.h>

// Define the event
TB_EVENT_NAME(LED, LED1_ON, "led1_on");
```

#### `led.c`

```c
// Define the action
static void onLedOn(Event *apEvent);

// Define the state table
static const StateTableRow stateTable[] = {
    {
        .state = TB_ANY_STATE,  // Matches any state
        .event = LED_EVENT_LED1_ON,
        .entryAction = onLedOn, // Function to call when the event occurs
        .nextState = TB_STATE_KEEP // Keep the current state
    }
};

// Handle the LED event action
static void onLedOn(Event *apEvent) {
    gpio_set_level(GPIO_LED, 0); // Turn LED on
}

void LedModule() {
    // Subscribe to TinyBus
    tb_subscribe(TB_SUBSCRIBER("led", stateTable, TB_TABLE_ROW_COUNT(stateTable)));
}
```

#### `button.c`

```c
// Include the LED event definition
#include "led.h"
#include <tinybus/tinybus.h>

// Define events and states
TB_EVENT_NAME(BUTTON, INITIALIZE, "initialize");
TB_EVENT_NAME(BUTTON, PRESSED, "button_short_pressed");
TB_STATE_NAME(BUTTON, IDLE, "idle");

// Define actions
static void initModule(Event *apEvent);
static void onButtonPress(Event *apEvent);

// Define the state table
static const StateTableRow stateTable[] = {
    // Initialize the button module
    {
        .state = TB_STATE_INITIAL,
        .event = BUTTON_EVENT_INITIALIZE,
        .entryAction = initModule,
        .nextState = BUTTON_STATE_POLLING,
    },
    // Handle button press event
    {
        .state = BUTTON_STATE_IDLE,
        .event = BUTTON_EVENT_PRESSED,
        .entryAction = onButtonPress,
        .nextState = TB_STATE_KEEP // Maintain the current state
    }
};

// Subscribe the button module to TinyBus
void ButtonModule() {
    tb_subscribe(TB_SUBSCRIBER("button", stateTable, TB_TABLE_ROW_COUNT(stateTable)));
}

// Handle the button press event and trigger the LED module
static void onButtonPress(Event *apEvent) {
    tb_publish(TB_EVENT(LED_EVENT_LED1_ON, NULL, 0)); // Send event to LED module
}
```

## Contributing

We welcome contributions in the form of code, bug reports, and feedback.

- See [CONTRIBUTING.md](https://github.com/tinybus/tinybus.esp/blob/main/CONTRIBUTING.md) for guidelines.

## Contact

Join the conversation on:
- **[GitHub Discussions](https://github.com/tinybus/tinybus.esp/discussions)** – For general chat or questions.
- **[GitHub Issues](https://github.com/tinybus/tinybus.esp/issues)** – To report bugs or suggest features.

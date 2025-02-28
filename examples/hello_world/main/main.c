/*
 *  TinyBus simplest example: Hello World
 *
 *  SPDX-FileCopyrightText: Copyright 2025 Clever Design (Switzerland) GmbH
 *  SPDX-License-Identifier: GPL-3.0-only OR OR LicenseRef-TinyBus-Software-1.0
 */

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <tinybus/tinybus.h>

const char *TAG = "main";

/*****************************************************************************
 * Tiny Bus Definitions for the main Module
 *****************************************************************************/

// 1. Define the events using in the state table
TB_EVENT_NAME(MAIN, HELLO_WORLD, "HelloWorld");

// 2. Declare the actions using in the state table
static void onHelloWorld(Event *apEvent);

// 3. Glue everything together
static const StateTableRow stateTable[] = {{
    .state = TB_STATE_INITIAL,                // only process the event, if the current state
                                              // match. TB_STATE_INITIAL is the default state.
                                              // use TB_STATE_ANY, if this should be ignored
    .event          = MAIN_EVENT_HELLO_WORLD, // process if the event matches
    .conditionCheck = NULL,                   // No condition function.  The action is always executed.
    .entryAction    = onHelloWorld,           // The function to call when the event occurs.
    .nextState      = TB_STATE_KEEP,          // Keep the current state after the action.
    .exitAction     = NULL,                   // No exit function.
    .stop           = true                    // Stop processing after this entry (this is the default case)
}};

/*****************************************************************************/
// Implement the Actions
static void onHelloWorld(Event *apEvent) { ESP_LOGI(TAG, "Action onHellowWorld() called from TinyBus"); }

void app_main()
{
    // Subscribe to the TinyBus using the state table. This registers the module
    // to receive and process events according to the defined rules.
    ESP_LOGI(TAG, "Subscribe module '%s' to TinyBus", TAG);
    tb_subscribe(TB_SUBSCRIBER(TAG, stateTable, TB_TABLE_ROW_COUNT(stateTable)));

    while (true)
    {
        ESP_LOGI(TAG, "Publish HELLO_WORLD event");
        // Publish the HELLO_WORLD event.  The TinyBus will then look up this
        // event in the state table and execute the corresponding action
        // (onHelloWorld in this case).  The NULL and 0 arguments indicate no data
        // is being sent with the event.    // publish an event to the TinyBus
        tb_publish(TB_EVENT(MAIN_EVENT_HELLO_WORLD, NULL, 0));

        // next event in 1 second
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

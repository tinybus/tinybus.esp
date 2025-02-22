# Nonsense Hello World Logger  

This is a **rather nonsense example** that logs `"Hello World"` to the console.  
Why? Because we can 🚀 —and because it demonstrates the basic usage of TinyBus.  

## Running the Example  

To run this masterpiece on a Linux host, simply use:  

```sh
make linux
```

## Code  

``` c
// 1. Define the events using in the state table
TB_EVENT_NAME(MAIN, HELLO_WORLD, "HelloWorld");

// 2. Declare the actions using in the state table
static void onHelloWorld(Event *apEvent);

// 3. Glue everything togther
static const StateTableRow stateTable[] = {{
    .state = TB_STATE_INITIAL, // only process the event, if the current state
                               // match. TB_STATE_INITIAL is the default state.
                               // use TB_STATE_ANY, if this should be ignored
    .event = MAIN_EVENT_HELLO_WORLD, // process if the event matches
    .conditionCheck =
        NULL, // No condition function.  The action is always executed.
    .entryAction = onHelloWorld, // The function to call when the event occurs.
    .nextState = TB_STATE_KEEP,  // Keep the current state after the action.
    .exitAction = NULL,          // No exit function.
    .stop = true // Stop processing after this entry (this is the default case)
}};

// Implement Action
static void onHelloWorld(Event *apEvent) {
  ESP_LOGI(TAG, "Action onHellowWorld() called from TinyBus");
}

void app_main() {
  ESP_LOGI(TAG, "Subscribe module '%s' to TinyBus", TAG);
  tb_subscribe(TB_SUBSCRIBER(TAG, stateTable, TB_TABLE_ROW_COUNT(stateTable)));

  while (true) {
    ESP_LOGI(TAG, "Publish HELLO_WORLD event");
    tb_publish(TB_EVENT(MAIN_EVENT_HELLO_WORLD, NULL, 0));
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}
```

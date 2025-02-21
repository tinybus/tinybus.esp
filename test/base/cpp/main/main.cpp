/* SPDX-License-Identifier: GPL-3.0-or-later */

// global includes
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <tinybus/tinybus.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
const char *TAG = "main";

namespace {
void print_heap_stats() {
  static multi_heap_info_t heap_info;
  heap_caps_get_info(&heap_info, MALLOC_CAP_INTERNAL);
  ESP_LOGI(TAG, "------------------------");
  ESP_LOGI(TAG, "total_free_bytes      %zu", heap_info.total_free_bytes);
  ESP_LOGI(TAG, "total_allocated_bytes %zu", heap_info.total_allocated_bytes);
  ESP_LOGI(TAG, "largest_free_block    %zu", heap_info.largest_free_block);
  ESP_LOGI(TAG, "minimum_free_bytes    %zu", heap_info.minimum_free_bytes);
  ESP_LOGI(TAG, "allocated_blocks      %zu", heap_info.allocated_blocks);
  ESP_LOGI(TAG, "free_blocks           %zu", heap_info.free_blocks);
  ESP_LOGI(TAG, "total_blocks          %zu", heap_info.total_blocks);
  ESP_LOGI(TAG, "------------------------");
}

void onButtonPress(Event *apEvent) { ESP_LOGI(TAG, "Button pressed"); }
// Inializing std::map using initializer list
TB_EVENT(MAIN, BUTTON_PRESS1, "ButtonPress1");
const StateTableRow stateTable[] = {{.event = TB_MAIN_EVENT_BUTTON_PRESS1,
                                     .state = TB_STATE_INITIAL,
                                     .conditionFn = nullptr,
                                     .entryFn = onButtonPress,
                                     .nextState = TB_STATE_KEEP,
                                     .exitFn = nullptr,
                                     .stop = true}};

} // namespace

extern "C" void app_main() {
  ESP_LOGI(TAG, "Starting app");

  tb_subscribe("main", stateTable, ARRAY_SIZE(stateTable));

  while (true) {
    print_heap_stats();
    vTaskDelay(10000 / portTICK_PERIOD_MS);
  }
}

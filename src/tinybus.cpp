/*
 *  TinyBus implementation
 *
 *  SPDX-FileCopyrightText: Copyright 2025 Clever Design (Switzerland) GmbH
 *  SPDX-License-Identifier: GPL-3.0-only OR OR LicenseRef-TinyBus-Software-1.0
 */

/**
 * @file
 *   Implementation of the TinyBus class
 */

#include "tinybus.hpp"
#include "tb_config.hpp"
#include <cstring>

namespace {
extern "C" void ebTask(void *aParameters) {
  tb::TinyBus::Instance().RunMainLoop();
}
} // namespace

namespace tb {

bool handleEvent(const Subscriber *apSubscriber, Event &arEvent) {
  uint8_t i;
  bool eventHandled = false;

  for (i = 0; i < apSubscriber->tableRowCount; i++) {
    const StateTableRow *row = &apSubscriber->table[i];
    if ((arEvent.event == row->event) &&
        ((apSubscriber->currentState == row->state) ||
         (row->state == TB_STATE_ANY)) &&
        ((row->conditionCheck == nullptr || (row->conditionCheck)()))) {

      // call exit action from last state (only, if the next state is new)
      if (apSubscriber->exitAction != nullptr &&
          row->nextState != TB_STATE_KEEP) {
        (apSubscriber->exitAction)(&arEvent);
      }

      // call entry action from new state
      if (row->entryAction != nullptr) {
        (row->entryAction)(&arEvent);
      }

      // set current state
      if (row->nextState != TB_STATE_KEEP) {
        *(const char **)apSubscriber->currentState = row->nextState;
      }

      // set exit action for next state (only, if the next state is new)
      if (row->nextState != TB_STATE_KEEP) {
        *(StateActionFn *)apSubscriber->exitAction = row->exitAction;
      }
      eventHandled = true;
      // break if needed
      if (row->stop) {
        break;
      }
    }
  }

  return eventHandled;
}

extern "C" tbError tb_subscribe(Subscriber *apSubscriber) {
  return TinyBus::Instance().Subscribe(apSubscriber);
}

extern "C" tbError tb_publish(const Event *apEvent) {
  return TinyBus::Instance().Publish(apEvent);
}

void TinyBus::RunMainLoop() {
  Event event;
  for (;;) {
    while (xQueueReceive(mBacklogQueue, &event, portMAX_DELAY) != pdPASS) {
    }
    bool eventHandled = false;
    for (size_t i = 0; i < mSubscriberCount; i++) {
      eventHandled |= handleEvent(mpSubscriber[i], event);
    }
    // Free event slot
    if (event.data != NULL) {
      tb_free((uint8_t *)event.data);
      event.data = NULL;
    }
    if (!eventHandled) {
      TB_WARN("No match for event '%s' found", event.event);
    }
  }
}

TinyBus::TinyBus() {
  // initialize event matrix
  mSubscriberCount = 0;
  mpSubscriber = (Subscriber **)tb_malloc(CONFIG_TINYBUS_MAX_SUBSCRIBERS *
                                          sizeof(Subscriber *));
  // create event queue
  mBacklogQueue = xQueueCreate(CONFIG_TINYBUS_MAX_BACKLOG, sizeof(Event));

  xTaskCreate(ebTask, "eb_main", 2048, this, 1, nullptr);
}

tbError TinyBus::Publish(const Event *apEvent) {
  // we need to copy the data within the event to the heap
  // it will be freed, after the event is processed
  void *dataCopy = tb_malloc(apEvent->dataLen);
  memcpy(dataCopy, apEvent->data, apEvent->dataLen);
  Event event = {
      .event = apEvent->event, .data = dataCopy, .dataLen = apEvent->dataLen};
  // FreeRTOS queue makes a copy of the data, it's save to use
  // a local variable (event)
  xQueueSendToBack(mBacklogQueue, &event, (TickType_t)10);

  return tbError::TB_ERROR_NONE;
}

tbError TinyBus::Subscribe(Subscriber *apSubscriber) {
  if (apSubscriber == NULL || apSubscriber->tableRowCount == 0) {
    TB_DEBUG("Subscriber NULL");
    return tbError::TB_ERROR_NONE;
  }
  if (apSubscriber->tableRowCount == 0) {
    TB_DEBUG("tableRowCount == 0");
    return tbError::TB_ERROR_NONE;
  }
  // test if we have slots left
  if (mSubscriberCount + 1 >= CONFIG_TINYBUS_MAX_SUBSCRIBERS) {
    TB_ERROR("Max count for subscribers reached");
    return tbError::TB_ERROR_SUBSCRIBER_COUNT_EXCEEDED;
  }
  mpSubscriber[mSubscriberCount] = apSubscriber;
  mpSubscriber[mSubscriberCount]->currentState = TB_STATE_INITIAL;
  mpSubscriber[mSubscriberCount]->exitAction = NULL;

  mSubscriberCount++;
  return tbError::TB_ERROR_NONE;
};

} // namespace tb

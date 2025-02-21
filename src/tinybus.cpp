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

namespace {
extern "C" void ebTask(void *aParameters) {
  tb::TinyBus::Instance().RunMainLoop();
}
} // namespace

namespace tb {

bool handleEvent(const TinyBus::Subscriber *apSubscriber, Event &arEvent) {
  uint8_t i;
  bool eventHandled = false;

  for (i = 0; i < apSubscriber->eventCount; i++) {
    const StateTableRow *row = &apSubscriber->stateTable[i];
    if ((arEvent.event == row->event) &&
        ((apSubscriber->currentState == row->state) ||
         (row->state == TB_STATE_ANY)) &&
        ((row->conditionFn == nullptr || (row->conditionFn)()))) {

      // call exit action from last state (only, if the next state is new)
      if (apSubscriber->exitFn != nullptr && row->nextState != TB_STATE_KEEP) {
        (apSubscriber->exitFn)(&arEvent);
      }

      // call entry action from new state
      if (row->entryFn != nullptr) {
        (row->entryFn)(&arEvent);
      }

      // set current state
      if (row->nextState != TB_STATE_KEEP) {
        *(const char **)apSubscriber->currentState = row->nextState;
      }

      // set exit action for next state (only, if the next state is new)
      if (row->nextState != TB_STATE_KEEP) {
        *(StateActionFn *)apSubscriber->exitFn = row->exitFn;
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

extern "C" tbError tb_subscribe(const char *pSubscriberName,
                                const StateTableRow *apStateTable,
                                size_t aCount) {
  return TinyBus::Instance().Subscribe(pSubscriberName, apStateTable, aCount);
}

void TinyBus::RunMainLoop() {
  Event event;
  for (;;) {
    while (xQueueReceive(mBacklogQueue, &event, portMAX_DELAY) != pdPASS) {
    }
    bool eventHandled = false;
    for (size_t i = 0; i < mSubscriberCont; i++) {
      eventHandled |= handleEvent(mpSubscriber[i], event);
    }
  }
}

TinyBus::TinyBus() {
  // initialize event matrix
  mSubscriberCont = 0;
  for (int i = 0; i < CONFIG_TINYBUS_MAX_SUBSCRIBERS; i++) {
    mpSubscriber[i] = nullptr;
  }
  // create event queue
  mBacklogQueue = xQueueCreate(CONFIG_TINYBUS_MAX_BACKLOG, sizeof(Event));

  xTaskCreate(ebTask, "eb_main", 2048, this, 1, nullptr);
}

void TinyBus::Publish() {}
tbError TinyBus::Subscribe(const char *apSubscriberName,
                           const StateTableRow *apStateTable,
                           const size_t aCount) {
  if (mSubscriberCont == 0) {
    // nothing to do
    return tbError::TB_ERROR_NONE;
  }
  if (mSubscriberCont + aCount > CONFIG_TINYBUS_MAX_SUBSCRIBERS) {
    // ASSERT(false);
    return tbError::TB_ERROR_SUBSCRIBER_COUNT_EXCEEDED;
  }
  *(int *)mpSubscriber[mSubscriberCont]->eventCount = aCount;
  *(const StateTableRow **)mpSubscriber[mSubscriberCont]->stateTable =
      apStateTable;
  *(const char **)mpSubscriber[mSubscriberCont]->name = apSubscriberName;
  mpSubscriber[mSubscriberCont]->currentState = TB_STATE_INITIAL;

  mSubscriberCont++;
  return tbError::TB_ERROR_NONE;
};

} // namespace tb

/*
 *  Main include for C++ based applications
 *
 *  SPDX-FileCopyrightText: Copyright 2025 Clever Design (Switzerland) GmbH
 *  SPDX-License-Identifier: GPL-3.0-only OR OR LicenseRef-TinyBus-Software-1.0
 */

#ifndef TB_TINYBUS_HPP
#define TB_TINYBUS_HPP

#include "tb_config.hpp"
#include <tinybus/tinybus.h>

namespace tb {

/** @brief The TinyBus class is a singleton class that manages the event
 *  subscription and publishing.
 *
 *  The TinyBus class is a singleton class that manages the event
 *  subscription and publishing. The class is designed to be used in
 *  embedded systems with limited resources. The class uses a static
 *  memory allocation for the event queue and the state table
 */
class TinyBus {
public:
  struct Subscriber {
    const char *const name;
    const StateTableRow *const stateTable;
    const size_t eventCount;
    const char *currentState;
    const StateActionFn exitFn;
  };
  /** @brief Get the instance of the TinyBus (singleton)class.
   *
   *  Get the instance of the TinyBus class.
   *
   *  @return The instance of the TinyBus class.
   */
  static TinyBus &Instance() {
    static TinyBus sInstance;
    return sInstance;
  }
  void RunMainLoop();

  TinyBus(const TinyBus &) = delete;
  TinyBus &operator=(const TinyBus &) = delete;

  tbError Subscribe(const char *apName, const StateTableRow *apStateTable,
                    size_t aCount);
  void Publish();

private:
  // private constructor for singleton class
  TinyBus();
  // private destructor for singleton class
  ~TinyBus() {}

  Subscriber *mpSubscriber[CONFIG_TINYBUS_MAX_SUBSCRIBERS];
  size_t mSubscriberCont;
  QueueHandle_t mBacklogQueue;
};
} // namespace tb

#endif // TB_TINYBUS_HPP

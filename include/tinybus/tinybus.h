/*
 *  Main include for C based applications
 *
 *  SPDX-FileCopyrightText: Copyright 2025 Clever Design (Switzerland) GmbH
 *  SPDX-License-Identifier: GPL-3.0-only OR OR LicenseRef-TinyBus-Software-1.0
 */

#ifndef TB_TINYBUS_H
#define TB_TINYBUS_H

#include <stddef.h>

#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif

const char *const TB_STATE_INITIAL = "TB_initial";
const char *const TB_STATE_ANY = "TB_any";
const char *const TB_STATE_KEEP = "TB_keep";

// Macro for genereting a const char* from a string literal
#define TB_EVENT_NAME(tag, name, value)                                        \
  const char *const TB_##tag##_EVENT_##name = value;
#define TB_STATE_NAME(tag, name, value)                                        \
  const char *const TB_##tag##_STATE_##name = value;
#define TB_EVENT(event, data, len) (&(Event){event, data, len})
#define TB_SUBSCRIBER(module, table, tableRowCount)                            \
  (&(Subscriber){module, table, tableRowCount, NULL, NULL})

// ARRAY_SIZE ... (be careful, this is base on sizeof, only use on arrays)
#define TB_TABLE_ROW_COUNT(x) (sizeof(x) / sizeof((x)[0]))

typedef struct Event {
  const char *event;
  void *data;
  size_t dataLen;
} Event;

typedef void (*StateActionFn)(Event *apEvent);

typedef bool (*StateConditionFn)();

typedef struct StateTableRow {
  const char *event;
  const char *state;
  StateConditionFn conditionCheck;
  StateActionFn entryAction;
  const char *nextState;
  StateActionFn exitAction;
  bool stop;
} StateTableRow;

typedef struct Subscriber {
  const char *const name;
  const StateTableRow *const table;
  const size_t tableRowCount;
  const char *currentState;
  StateActionFn exitAction;
} Subscriber;

tbError tb_subscribe(Subscriber *apSubscriber);

tbError tb_publish(const Event *apEvent);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TB_TINYBUS_H

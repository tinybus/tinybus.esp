/*
 *  Main include for C based applications
 *
 *  SPDX-FileCopyrightText: Copyright 2025 Clever Design (Switzerland) GmbH
 *  SPDX-License-Identifier: GPL-3.0-only OR OR LicenseRef-TinyBus-Software-1.0
 */

#ifndef TB_TINYBUS_H
#define TB_TINYBUS_H

#include <stddef.h>

#include "tinybus/error.h"

#ifdef __cplusplus
extern "C" {
#endif

// Macro for genereting a const char* from a string literal
#define TB_EVENT(tag, name, value)                                             \
  const char *const TB_##tag##_EVENT_##name = value;
#define TB_STATE(tag, name, value)                                             \
  const char *const TB_##tag##_STATE_##name = value;

const char *const TB_STATE_INITIAL = "TB_initial";
const char *const TB_STATE_ANY = "TB_any";
const char *const TB_STATE_KEEP = "TB_keep";

typedef struct Event {
  char *event;
  size_t dataLen;
  void *data;
} Event;

typedef void (*StateActionFn)(Event *apEvent);
typedef bool (*StateConditionFn)();
typedef struct StateTableRow {
  const char *event;
  const char *state;
  StateConditionFn conditionFn;
  StateActionFn entryFn;
  const char *nextState;
  StateActionFn exitFn;
  bool stop;
} StateTableRow;

tbError tb_subscribe(const char *pSubscriberName,
                     const StateTableRow *apStateTable, size_t aCount);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TB_TINYBUS_H

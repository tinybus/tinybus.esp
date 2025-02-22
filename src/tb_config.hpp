/*
 *  TinyBus global include
 *
 *  SPDX-FileCopyrightText: Copyright 2025 Clever Design (Switzerland) GmbH
 *  SPDX-License-Identifier: GPL-3.0-only OR OR LicenseRef-TinyBus-Software-1.0
 */

#ifndef TB_CONFIG_H
#define TB_CONFIG_H

// System includes
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "sdkconfig.h"
#include <cstddef>

#if defined(CONFIG_TINYBUS_MALLOC_STRATEGY_DEFAULT) ||                         \
    defined(CONFIG_TINYBUS_MALLOC_STRATEGY_INTERNAL) ||                        \
    defined(CONFIG_TINYBUS_MALLOC_STRATEGY_SPIRAM)
#include "esp_heap_caps.h"
#include <stdlib.h>
#endif

#ifdef CONFIG_TB_ASSERTS
#include <assert.h>
#endif

#if !defined(TB_NO_DEBUG) || !defined(TB_NO_WARN) || !defined(TB_NO_ERROR) ||  \
    defined(TB_YES_TRACE)
#include <stdio.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern const char TINYBUS_TAG[];

// Logging functions
#ifndef TB_TRACE
#ifdef TB_YES_TRACE
#define TB_TRACE_(fmt, ...)                                                    \
  ESP_LOGV(TINYBUS_TAG, "%s:%d:trace: " fmt "%s", __FILE_NAME__, __LINE__,     \
           __VA_ARGS__)
#define TB_TRACE(...) TB_TRACE_(__VA_ARGS__, "")
#else
#define TB_TRACE(...)
#endif
#endif

#ifndef TB_DEBUG
#ifndef TB_NO_DEBUG
#define TB_DEBUG_(fmt, ...)                                                    \
  ESP_LOGD(TINYBUS_TAG, "%s:%d:debug: " fmt "%s", __FILE_NAME__, __LINE__,     \
           __VA_ARGS__)
#define TB_DEBUG(...) TB_DEBUG_(__VA_ARGS__, "")
#else
#define TB_DEBUG(...)
#endif
#endif

#ifndef TB_WARN
#ifndef TB_NO_WARN
#define TB_WARN_(fmt, ...)                                                     \
  ESP_LOGW(TINYBUS_TAG, "%s:%d:warn: " fmt "%s", __FILE_NAME__, __LINE__,      \
           __VA_ARGS__)
#define TB_WARN(...) TB_WARN_(__VA_ARGS__, "")
#else
#define TB_WARN(...)
#endif
#endif

#ifndef TB_ERROR
#ifndef TB_NO_ERROR
#define TB_ERROR_(fmt, ...)                                                    \
  ESP_LOGE(TINYBUS_TAG, "%s:%d:error: " fmt "%s", __FILE_NAME__, __LINE__,     \
           __VA_ARGS__)
#define TB_ERROR(...) TB_ERROR_(__VA_ARGS__, "")
#else
#define TB_ERROR(...)
#endif
#endif

// Runtime assertions
#ifdef CONFIG_TINYBUS_ASSERTS
#define TB_ASSERT(test) assert(test)
#else
#define TB_ASSERT(test)
#endif

// Allocate memory, only used if buffers are not provided to tinybus
// For the lookahead buffer, memory must be 32-bit aligned
static inline void *tb_malloc(size_t size) {
#if defined(CONFIG_TINYBUS_MALLOC_STRATEGY_DEFAULT)
  return malloc(size); // Equivalent to heap_caps_malloc_default(size);
#elif defined(CONFIG_TINYBUS_MALLOC_STRATEGY_INTERNAL)
  return heap_caps_malloc(size, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
#elif defined(CONFIG_TINYBUS_MALLOC_STRATEGY_SPIRAM)
  return heap_caps_malloc(size, MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
#else // CONFIG_TINYBUS_MALLOC_STRATEGY_DISABLE or not defined
  (void)size;
  return NULL;
#endif
}

// Deallocate memory, only used if buffers are not provided to tinybus
static inline void tb_free(void *p) {
#if defined(CONFIG_TINYBUS_MALLOC_STRATEGY_DEFAULT) ||                         \
    defined(CONFIG_TINYBUS_MALLOC_STRATEGY_INTERNAL) ||                        \
    defined(CONFIG_TINYBUS_MALLOC_STRATEGY_SPIRAM)
  free(p);
#else // CONFIG_TINYBUS_MALLOC_STRATEGY_DISABLE or not defined
  (void)p;
#endif
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // TB_CONFIG_H

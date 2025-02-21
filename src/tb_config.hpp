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

// Macros, may be replaced by system specific wrappers. Arguments to these
// macros must not have side-effects as the macros can be removed for a smaller
// code footprint
extern const char TINYBUS_TAG[];

// Logging functions
#ifndef TB_TRACE
#ifdef TB_YES_TRACE
#define TB_TRACE_(fmt, ...)                                                    \
  ESP_LOGV(ESP_TINYBUS_TAG, "%s:%d:trace: " fmt "%s\n", __FILE__, __LINE__,    \
           __VA_ARGS__)
#define TB_TRACE(...) TB_TRACE_(__VA_ARGS__, "")
#else
#define TB_TRACE(...)
#endif
#endif

#ifndef TB_DEBUG
#ifndef TB_NO_DEBUG
#define TB_DEBUG_(fmt, ...)                                                    \
  ESP_LOGD(ESP_TINYBUS_TAG, "%s:%d:debug: " fmt "%s\n", __FILE__, __LINE__,    \
           __VA_ARGS__)
#define TB_DEBUG(...) TB_DEBUG_(__VA_ARGS__, "")
#else
#define TB_DEBUG(...)
#endif
#endif

#ifndef TB_WARN
#ifndef TB_NO_WARN
#define TB_WARN_(fmt, ...)                                                     \
  ESP_LOGW(ESP_TINYBUS_TAG, "%s:%d:warn: " fmt "%s\n", __FILE__, __LINE__,     \
           __VA_ARGS__)
#define TB_WARN(...) TB_WARN_(__VA_ARGS__, "")
#else
#define TB_WARN(...)
#endif
#endif

#ifndef TB_ERROR
#ifndef TB_NO_ERROR
#define TB_ERROR_(fmt, ...)                                                    \
  ESP_LOGE(ESP_TINYBUS_TAG, "%s:%d:error: " fmt "%s\n", __FILE__, __LINE__,    \
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

// Builtin functions, these may be replaced by more efficient
// toolchain-specific implementations. TB_NO_INTRINSICS falls back to a more
// expensive basic C implementation for debugging purposes

// Min/max functions for unsigned 32-bit numbers
static inline uint32_t tb_max(uint32_t a, uint32_t b) {
  return (a > b) ? a : b;
}

static inline uint32_t tb_min(uint32_t a, uint32_t b) {
  return (a < b) ? a : b;
}

// Align to nearest multiple of a size
static inline uint32_t tb_aligndown(uint32_t a, uint32_t alignment) {
  return a - (a % alignment);
}

static inline uint32_t tb_alignup(uint32_t a, uint32_t alignment) {
  return tb_aligndown(a + alignment - 1, alignment);
}

// Find the smallest power of 2 greater than or equal to a
static inline uint32_t tb_npw2(uint32_t a) {
#if !defined(TB_NO_INTRINSICS) && (defined(__GNUC__) || defined(__CC_ARM))
  return 32 - __builtin_clz(a - 1);
#else
  uint32_t r = 0;
  uint32_t s;
  a -= 1;
  s = (a > 0xffff) << 4;
  a >>= s;
  r |= s;
  s = (a > 0xff) << 3;
  a >>= s;
  r |= s;
  s = (a > 0xf) << 2;
  a >>= s;
  r |= s;
  s = (a > 0x3) << 1;
  a >>= s;
  r |= s;
  return (r | (a >> 1)) + 1;
#endif
}

// Count the number of trailing binary zeros in a
// tb_ctz(0) may be undefined
static inline uint32_t tb_ctz(uint32_t a) {
#if !defined(TB_NO_INTRINSICS) && defined(__GNUC__)
  return __builtin_ctz(a);
#else
  return tb_npw2((a & -a) + 1) - 1;
#endif
}

// Count the number of binary ones in a
static inline uint32_t tb_popc(uint32_t a) {
#if !defined(TB_NO_INTRINSICS) && (defined(__GNUC__) || defined(__CC_ARM))
  return __builtin_popcount(a);
#else
  a = a - ((a >> 1) & 0x55555555);
  a = (a & 0x33333333) + ((a >> 2) & 0x33333333);
  return (((a + (a >> 4)) & 0xf0f0f0f) * 0x1010101) >> 24;
#endif
}

// Find the sequence comparison of a and b, this is the distance
// between a and b ignoring overflow
static inline int tb_scmp(uint32_t a, uint32_t b) {
  return (int)(unsigned)(a - b);
}

// Convert between 32-bit little-endian and native order
static inline uint32_t tb_fromle32(uint32_t a) {
#if !defined(TB_NO_INTRINSICS) &&                                              \
    ((defined(BYTE_ORDER) && defined(ORDER_LITTLE_ENDIAN) &&                   \
      BYTE_ORDER == ORDER_LITTLE_ENDIAN) ||                                    \
     (defined(__BYTE_ORDER) && defined(__ORDER_LITTLE_ENDIAN) &&               \
      __BYTE_ORDER == __ORDER_LITTLE_ENDIAN) ||                                \
     (defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) &&           \
      __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__))
  return a;
#elif !defined(TB_NO_INTRINSICS) &&                                            \
    ((defined(BYTE_ORDER) && defined(ORDER_BIG_ENDIAN) &&                      \
      BYTE_ORDER == ORDER_BIG_ENDIAN) ||                                       \
     (defined(__BYTE_ORDER) && defined(__ORDER_BIG_ENDIAN) &&                  \
      __BYTE_ORDER == __ORDER_BIG_ENDIAN) ||                                   \
     (defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) &&              \
      __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__))
  return __builtin_bswap32(a);
#else
  return (((uint8_t *)&a)[0] << 0) | (((uint8_t *)&a)[1] << 8) |
         (((uint8_t *)&a)[2] << 16) | (((uint8_t *)&a)[3] << 24);
#endif
}

static inline uint32_t tb_tole32(uint32_t a) { return tb_fromle32(a); }

// Convert between 32-bit big-endian and native order
static inline uint32_t tb_frombe32(uint32_t a) {
#if !defined(TB_NO_INTRINSICS) &&                                              \
    ((defined(BYTE_ORDER) && defined(ORDER_LITTLE_ENDIAN) &&                   \
      BYTE_ORDER == ORDER_LITTLE_ENDIAN) ||                                    \
     (defined(__BYTE_ORDER) && defined(__ORDER_LITTLE_ENDIAN) &&               \
      __BYTE_ORDER == __ORDER_LITTLE_ENDIAN) ||                                \
     (defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) &&           \
      __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__))
  return __builtin_bswap32(a);
#elif !defined(TB_NO_INTRINSICS) &&                                            \
    ((defined(BYTE_ORDER) && defined(ORDER_BIG_ENDIAN) &&                      \
      BYTE_ORDER == ORDER_BIG_ENDIAN) ||                                       \
     (defined(__BYTE_ORDER) && defined(__ORDER_BIG_ENDIAN) &&                  \
      __BYTE_ORDER == __ORDER_BIG_ENDIAN) ||                                   \
     (defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) &&              \
      __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__))
  return a;
#else
  return (((uint8_t *)&a)[0] << 24) | (((uint8_t *)&a)[1] << 16) |
         (((uint8_t *)&a)[2] << 8) | (((uint8_t *)&a)[3] << 0);
#endif
}

static inline uint32_t tb_tobe32(uint32_t a) { return tb_frombe32(a); }

// Calculate CRC-32 with polynomial = 0x04c11db7
uint32_t tb_crc(uint32_t crc, const void *buffer, size_t size);

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

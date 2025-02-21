/**
 * @file
 * @brief
 *  This file defines the errors used in the OpenThread.
 */

#ifndef TB_ERROR_H_
#define TB_ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup api-error
 *
 * @brief
 *   This module includes error definitions used in OpenThread.
 *
 * @{
 */

/**
 * Represents error codes used throughout OpenThread.
 */
typedef enum tbError {
  /**
   * No error.
   */
  TB_ERROR_NONE = 0,

  /**
   * Operational failed.
   */
  TB_ERROR_SUBSCRIBER_COUNT_EXCEEDED = 1,

  /**
   * The number of defined errors.
   */
  TB_NUM_ERRORS,

  /**
   * Generic error (should not use).
   */
  TB_ERROR_GENERIC = 255,
} tbError;

/**
 * @}
 */

#ifdef __cplusplus
} // extern "C"
#endif

#endif // TB_ERROR_H_

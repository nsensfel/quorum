/**
 * Copyright (c) 2024 https://segfault.tech
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#ifndef QUORUM_INCLUDE_PERVASIVE_H
#define QUORUM_INCLUDE_PERVASIVE_H

#include <stdint.h>

#ifdef QUORUM_CONFIGURATION_HEADER
	#include QUORUM_CONFIGURATION_HEADER
#endif

/******************************************************************************/
/**** CONFIGURATION ***********************************************************/
/******************************************************************************/

#ifndef QUORUM_CORE_COUNT_MAX
	#error "QUORUM_CORE_COUNT_MAX must be defined."
#endif

#ifndef QUORUM_CORE_INDEX_TYPE
	#define QUORUM_CORE_INDEX_TYPE uint8_t

	#if ((QUORUM_CORE_COUNT_MAX) > UINT8_MAX)
		#error "QUORUM_CORE_COUNT_MAX is set too high for the default quorum_core_index type."
	#endif
#endif

#ifndef QUORUM_MECHANISM_INDEX_TYPE
	#define QUORUM_MECHANISM_INDEX_TYPE uint16_t

	#if (defined(QUORUM_MECHANISM_INDEX_MAX) && (QUORUM_MECHANISM_INDEX_MAX > UINT16_MAX))
		#error "QUORUM_MECHANISM_INDEX_MAX is set too high for quorum_mechanism_index type."
	#endif

	#ifndef QUORUM_MECHANISM_INDEX_MAX
		#define QUORUM_MECHANISM_INDEX_MAX UINT16_MAX
	#endif
#else
	#ifndef QUORUM_MECHANISM_INDEX_MAX
		#error "quorum_mechanism_index type has been redefined, but not QUORUM_MECHANISM_INDEX_MAX"
	#endif
#endif

#ifndef QUORUM_ENABLE_BARRIER
	#pragma message "QUORUM_ENABLE_BARRIER implicitly set to 0."
	#define QUORUM_ENABLE_BARRIER 0
#endif

#ifndef QUORUM_ENABLE_MAILBOX
	#pragma message "QUORUM_ENABLE_MAILBOX implicitly set to 0."
	#define QUORUM_ENABLE_MAILBOX 0
#endif

#ifndef QUORUM_ENABLE_MUTEX
	#pragma message "QUORUM_ENABLE_MUTEX implicitly set to 0."
	#define QUORUM_ENABLE_MUTEX 0
#endif

#ifndef QUORUM_ENABLE_SEMAPHORE
	#pragma message "QUORUM_ENABLE_SEMAPHORE implicitly set to 0."
	#define QUORUM_ENABLE_SEMAPHORE 0
#endif

#ifndef QUORUM_ENABLE_SIGNAL
	#pragma message "QUORUM_ENABLE_SIGNAL implicitly set to 0."
	#define QUORUM_ENABLE_SIGNAL 0
#endif

#ifndef QUORUM_ENABLE_ASSERT
	#pragma message "QUORUM_ENABLE_ASSERT implicitly set to 0."
	#define QUORUM_ENABLE_ASSERT 0
#endif

#ifndef QUORUM_ENABLE_QUORUM_DEBUG_ASSERT
	#pragma message "QUORUM_ENABLE_QUORUM_DEBUG_ASSERT implicitly set to 0."
	#define QUORUM_ENABLE_QUORUM_DEBUG_ASSERT 0
#endif

/******************************************************************************/
/**** CONFIGURATION SANITY CHECKS *********************************************/
/******************************************************************************/
/**** Mechanism enabling is 1 or 0 ********************************************/
#if ((QUORUM_ENABLE_BARRIER != 0) && (QUORUM_ENABLE_BARRIER != 1))
	#error "Invalid value for QUORUM_ENABLE_BARRIER. Allowed values: 0, 1."
#endif

#if ((QUORUM_ENABLE_MAILBOX != 0) && (QUORUM_ENABLE_MAILBOX != 1))
	#error "Invalid value for QUORUM_ENABLE_MAILBOX. Allowed values: 0, 1."
#endif

#if ((QUORUM_ENABLE_MUTEX != 0) && (QUORUM_ENABLE_MUTEX != 1))
	#error "Invalid value for QUORUM_ENABLE_MUTEX. Allowed values: 0, 1."
#endif

#if ((QUORUM_ENABLE_SEMAPHORE != 0) && (QUORUM_ENABLE_SEMAPHORE != 1))
	#error "Invalid value for QUORUM_ENABLE_SEMAPHORE. Allowed values: 0, 1."
#endif

#if ((QUORUM_ENABLE_SIGNAL != 0) && (QUORUM_ENABLE_SIGNAL != 1))
	#error "Invalid value for QUORUM_ENABLE_SIGNAL. Allowed values: 0, 1."
#endif

/**** Debug enabling is 1 or 0 ************************************************/
#if ((QUORUM_ENABLE_ASSERT != 0) && (QUORUM_ENABLE_ASSERT != 1))
	#error "Invalid value for QUORUM_ENABLE_ASSERT. Allowed values: 0, 1."
#endif

#if ((QUORUM_ENABLE_QUORUM_DEBUG_ASSERT != 0) && (QUORUM_ENABLE_QUORUM_DEBUG_ASSERT != 1))
	#error "Invalid value for QUORUM_ENABLE_QUORUM_DEBUG_ASSERT. Allowed values: 0, 1."
#endif

#if ((QUORUM_ENABLE_ASSERT == 0) && (QUORUM_ENABLE_QUORUM_DEBUG_ASSERT == 1))
	#warning "QUORUM_ENABLE_QUORUM_DEBUG_ASSERT is 1, but QUORUM_ENABLE_ASSERT is 0."
#endif

/******************************************************************************/
/**** GLOBAL TYPES ************************************************************/
/******************************************************************************/
typedef QUORUM_CORE_INDEX_TYPE quorum_core_index;
typedef QUORUM_MECHANISM_INDEX_TYPE quorum_mechanism_index;

enum quorum_result
{
	/* This can never succeed (programming error). */
	QUORUM_IMPOSSIBLE = -4,

	/* An error occurred */
	QUORUM_ERROR = -3,

	/* Locking the council timed out. */
	QUORUM_COUNCIL_LOCK_TIMEOUT = -2,

	/* Something prevented the operation from succeeding. */
	QUORUM_FAILURE = -1,

	/* The operation succeeded. */
	QUORUM_SUCCESS = 0,

	/* The operation has started (e.g. joined a waiting list). */
	QUORUM_PENDING = 0,

	/* The requested permission was granted. */
	QUORUM_GRANTED = 1,
};

typedef enum quorum_result quorum_result;

enum quorum_attribute
{
	QUORUM_DEFAULT_BEHAVIOR = 0,

	/*
	 * Do not join waiting lists. Fail if the permission cannot be granted
	 * immediately.
	 */
	QUORUM_IMMEDIATE = 1,

	/*
	 * Do not attempt multiple council locks. Timeout the council lock
	 * immediately if it fails.
	 */
	QUORUM_IMMEDIATE_COUNCIL = 2,

	/*
	 * Request a timeout on the council lock. Meaning is up to the machine
	 * specific implementation.
	 * This option is intended to provide an option that ensures all Quorum calls
	 * complete. It is recommended for 'quorum_core.opaque' to hold all
	 * information regarding how long the timeout should last.
	 */
	QUORUM_ENABLE_COUNCIL_TIMEOUT = 4,

	/*
	 * Ignores the FIFO order of this mechanism if doing so makes the operation
	 * succeed.
	 */
	QUORUM_ALLOW_OUT_OF_ORDER = 8
};

typedef uint32_t quorum_attributes;

/******************************************************************************/
/**** ERROR HANDLING **********************************************************/
/******************************************************************************/

#ifndef QUORUM_ERROR
	#include <stdio.h>

	#define QUORUM_ERROR(message, ...) \
		fprintf \
		( \
			stderr, \
			"[E][QUORUM][%s][l.%d]" message "\n", \
			__FILE__, \
			__LINE__, \
			__VA_ARGS__ \
		)
#endif

#if (QUORUM_ENABLE_ASSERT == 1)
	#ifndef QUORUM_ASSERT
		#define QUORUM_ASSERT(test, message, ...) \
			if (!(test)) \
			{ \
				QUORUM_ERROR("[ASSERT]" message, __VA_ARGS__); \
			}
	#endif
#else
	#define QUORUM_ASSERT(test, message, ...)
#endif

#if (QUORUM_ENABLE_QUORUM_DEBUG_ASSERT == 1)
	#ifndef QUORUM_DEBUG_ASSERT
		#define QUORUM_DEBUG_ASSERT(test, message, ...) \
			QUORUM_ASSERT(test, "[QUORUM-DEBUG]" message, __VA_ARGS__)
	#endif
#else
	#define QUORUM_DEBUG_ASSERT(...)
#endif

/*
 * Relationship between quorum_core_index and quorum_core_indent:
 * - quorum_core_indent(0) indicates no core.
 * - quorum_core_indent(X) indicates core (X - 1).
 */
typedef quorum_core_index quorum_core_ident;

#endif

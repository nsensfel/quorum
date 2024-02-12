/**
 * Copyright (c) 2024 https://segfault.tech
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#ifndef QUORUM_INCLUDE_COUNCIL_TYPE_H
#define QUORUM_INCLUDE_COUNCIL_TYPE_H

#include "pervasive.h"

typedef struct
{
	/* Index of the core in this council. 0 =< index < QUORUM_CORE_COUNT_MAX. */
	quorum_core_index index;

	/* Pointer provided for the integrator to pass extra information. */
	void * opaque;
} quorum_core;

typedef struct
{
	#if (QUORUM_ENABLE_BARRIER == 1)
		/* Number of barriers the core should acknowledge the opening of. */
		quorum_mechanism_index pending_barrier_updates;
	#endif
	#if (QUORUM_ENABLE_MAILBOX == 1)
		/* Number of mailboxes the core should acknowledge the ??? of. */
		quorum_mechanism_index pending_mailbox_updates;
	#endif
	#if (QUORUM_ENABLE_MUTEX == 1)
		/* Number of mutexes the core should acknowledge the acquisition of. */
		quorum_mechanism_index pending_mutex_updates;
	#endif
	#if (QUORUM_ENABLE_SEMAPHORE == 1)
		/**
		 * Number of semaphores the core should acknowledge the acquisition from.
		 **/
		quorum_mechanism_index pending_semaphore_updates;
	#endif
	#if (QUORUM_ENABLE_SIGNAL == 1)
		/* Number of signals the core should acknowledge the happening of. */
		quorum_mechanism_index pending_signal_updates;
	#endif
} quorum_core_status;

typedef struct
{
	/* Status of each core in the council. */
	quorum_core_status core_statuses[QUORUM_CORE_COUNT_MAX];
	#if (QUORUM_ENABLE_BARRIER == 1)
		quorum_mechanism_index barriers_count;
		quorum_barrier * barriers;
	#endif
	#if (QUORUM_ENABLE_MAILBOX == 1)
		quorum_mechanism_index mailboxes_count;
		quorum_mailbox * mailboxes;
	#endif
	#if (QUORUM_ENABLE_MUTEX == 1)
		quorum_mechanism_index mutexes_count;
		quorum_mutex * mutexes;
	#endif
	#if (QUORUM_ENABLE_SEMAPHORE == 1)
		quorum_mechanism_index semaphores_count;
		quorum_semaphore * semaphores;
	#endif
	#if (QUORUM_ENABLE_SIGNAL == 1)
		quorum_mechanism_index signals_count;
		quorum_signal * signals;
	#endif
	/* Has one of the cores initialized the council yet? */
	bool is_initialized;
} quorum_council;

#endif

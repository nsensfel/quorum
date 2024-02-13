/**
 * Copyright (c) 2024 https://segfault.tech
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#ifndef QUORUM_INCLUDE_COUNCIL_H
#define QUORUM_INCLUDE_COUNCIL_H

#include "pervasive.h"

#if (QUORUM_ENABLE_BARRIER == 1)
	#include "barrier/types.h"
#endif

#if (QUORUM_ENABLE_MAILBOX == 1)
	#include "mailbox/types.h"
#endif

#if (QUORUM_ENABLE_MUTEX == 1)
	#include "mutex/types.h"
#endif

#if (QUORUM_ENABLE_SEMAPHORE == 1)
	#include "semaphore/types.h"
#endif

#if (QUORUM_ENABLE_SIGNAL == 1)
	#include "signal/types.h"
#endif

#include "council/types.h"

/*
 * First CPU to get the HW mutex initializes.
 * Others don't.
 * Role is indicated by return value.
 */
quorum_result quorum_council_initialize_preamble
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	#if (QUORUM_ENABLE_BARRIER == 1)
		const quorum_mechanism_index barriers_count,
		quorum_barrier barriers [const static barriers_count],
	#endif
	#if (QUORUM_ENABLE_MAILBOX == 1)
		const quorum_mechanism_index mailboxes_count,
		quorum_mailbox mailboxes [const static mailboxes_count],
	#endif
	#if (QUORUM_ENABLE_MUTEX == 1)
		const quorum_mechanism_index mutexes_count,
		quorum_mutex mutexes [const static mutexes_count],
	#endif
	#if (QUORUM_ENABLE_SEMAPHORE == 1)
		const quorum_mechanism_index semaphores_count,
		quorum_semaphore semaphores [const static semaphores_count],
	#endif
	#if (QUORUM_ENABLE_SIGNAL == 1)
		const quorum_mechanism_index signals_count,
		quorum_signal signals [const static signals_count],
	#endif
	const quorum_attributes attributes
);

void quorum_council_initialize_epilogue
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_attributes attributes
);

quorum_result quorum_council_lock
(
	const quorum_core core [const restrict static 1],
	const quorum_attributes attributes
);

void quorum_council_unlock
(
	const quorum_core core [const restrict static 1],
	const quorum_attributes attributes
);

void quorum_council_notify
(
	const quorum_core core [const restrict static 1],
	const quorum_attributes attributes,
	const quorum_core_index
);

#endif

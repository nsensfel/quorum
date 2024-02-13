/**
 * Copyright (c) 2024 https://segfault.tech
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#include <quorum/council.h>

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
)
{
	if (quorum_council_lock(core, attributes) != QUORUM_GRANTED)
	{
		return QUORUM_COUNCIL_LOCK_TIMEOUT;
	}

	if (council->is_initialized)
	{
		quorum_council_unlock(core, attributes);

		return QUORUM_SUCCESS;
	}

	#if (QUORUM_ENABLE_BARRIER == 1)
		council->barriers_count = barriers_count;
		council->barriers = barriers;
	#endif
	#if (QUORUM_ENABLE_MAILBOX == 1)
		council->mailboxes_count = mailboxes_count;
		council->mailboxes = mailboxes;
	#endif
	#if (QUORUM_ENABLE_MUTEX == 1)
		council->mutexes_count = mutexes_count;
		council->mutexes = mutexes;
	#endif
	#if (QUORUM_ENABLE_SEMAPHORE == 1)
		council->semaphores_count = semaphores_count;
		council->semaphores = semaphores;
	#endif
	#if (QUORUM_ENABLE_SIGNAL == 1)
		council->signals_count = signals_count;
		council->signals = signals;
	#endif

	/* Do NOT unlock yet. */
	return QUORUM_GRANTED;
}

void quorum_council_initialize_epilogue
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_attributes attributes
)
{
	council->is_initialized = true;

	quorum_council_unlock(core, attributes);
}

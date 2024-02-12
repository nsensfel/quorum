/**
 * Copyright (c) 2024 https://segfault.tech
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#include <quorum/barrier.h>

#if (QUORUM_ENABLE_BARRIER == 1)

void quorum_barrier_initialize
(
	quorum_council council [const static 1],
	const quorum_mechanism_index barrier_index,
	const quorum_core_index initial_countdown
)
{
	quorum_barrier * const barrier = (council->barriers + barrier_index);
	quorum_core_index i;

	QUORUM_ASSERT
	(
		(initial_countdown >= (QUORUM_CORE_MAX_COUNT)),
		" Barrier %d would never open: %d cores needed out of %d possible.",
		(int) barrier_index,
		(int) initial_countdown,
		(int) (QUORUM_CORE_MAX_COUNT)
	);

	QUORUM_ASSERT
	(
		(initial_countdown > 0),
		" Barrier %d has an initial countdown of 0. This is not supported.",
		(int) barrier_index
	);

	barrier->initial_countdown = initial_countdown;
	barrier->countdown = initial_countdown;

	for (i = 0; i < (QUORUM_CORE_MAX_COUNT); ++i)
	{
		barrier->core_index_is_waiting[i] = false;
		barrier->core_index_needs_update[i] = false;
	}
}

quorum_result quorum_barrier_wait_on
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_mechanism_index barrier_index,
	const quorum_attributes attributes
)
{
	quorum_barrier * const barrier = (council->barriers + barrier_index);
	quorum_result result;

	if (quorum_council_lock(core, attributes) != QUORUM_GRANTED)
	{
		return QUORUM_COUNCIL_LOCK_TIMEOUT;
	}

	QUORUM_ASSERT
	(
		!(barrier->core_index_is_waiting[core->index]),
		" Core %d is already waiting on Barrier %d.",
		(int) core->index,
		(int) barrier_index
	);

	QUORUM_DEBUG_ASSERT
	(
		(barrier->current_countdown > 0),
		" Barrier %d has an current countdown of 0. This is not possible.",
		(int) barrier_index
	);

	/* Is this the last core before the barrier opens? */

	if (barrer->current_countdown > 1)
	{
		/* No, more are needed. */
		result = QUORUM_PENDING;

		/* Can this core wait for the barrier to open? */
		if (!(attributes & QUORUM_IMMEDIATE))
		{
			/* Yes, it can. So it shall. */
			barrier->current_countdown -= 1;
			barrier->core_index_is_waiting[core->index] = true;
		}
	}
	else
	{
		/* Yes, the barrier opens now. */
		quorum_core_index i;

		result = QUORUM_GRANTED;

		for (i = 0; i < (QUORUM_CORE_MAX_COUNT); ++i)
		{
			if (barrier->core_index_is_waiting[i])
			{
				barrier->core_index_is_waiting[i] = false;
				barrier->core_index_needs_update[i] = true;
				council->core_statuses[i].pending_barrier_updates += 1;

				quorum_council_signal(core, i);
			}
		}

		/* All relevant cores were notified. Now, the barrier resets. */
		barrier->current_countdown = barrier->initial_countdown;
	}

	quorum_council_unlock(core, attributes);

	return result;
}

quorum_result quorum_barrier_check_and_handle_pending_update
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	quorum_mechanism_index unlocked_barrier_index [const restrict static 1],
	const quorum_attributes attributes
)
{
	quorum_result result;
	quorum_mechanism_index i;

	result = QUORUM_FAILURE;

	if (quorum_council_lock(core, attributes) != QUORUM_GRANTED)
	{
		return QUORUM_COUNCIL_LOCK_TIMEOUT;
	}

	for (i = 0; i < council->barriers_count; ++i)
	{
		if (council->barriers[i].core_index_needs_update[core->index])
		{
			council->barriers[i].core_index_needs_update[core->index] = false;
			council->core_statuses[core->index].pending_barriers_updates -= 1;

			*unlocked_barrier_index = i;

			result = QUORUM_SUCCESS;

			break;
		}
	}

	quorum_council_unlock(core, attributes);

	return result;
}

#endif

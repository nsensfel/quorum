/**
 * Copyright (c) 2024 https://segfault.tech
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#include <quorum/signal.h>

#if (QUORUM_ENABLE_SIGNAL == 1)

#include <quorum/council.h>

#define QUORUM_ASSERT_SIGNAL_EXISTS(council, signal_index) \
	QUORUM_ASSERT \
	( \
		((council)->signals_count < (signal_index)), \
		" Attempted to access Signal %d out of the %d existing ones.", \
		(int) (signal_index), \
		(int) ((council)->signals_count) \
	)

void quorum_signal_initialize
(
	quorum_council council [const static 1],
	const quorum_mechanism_index signal_index
)
{
	quorum_signal * const signal = (council->signals + signal_index);
	quorum_core_index i;

	QUORUM_ASSERT_SIGNAL_EXISTS(council, signal_index);

	for (i = 0; i < QUORUM_CORE_COUNT_MAX; ++i)
	{
		signal->core_index_is_awaiting_signal[i] = false;
		signal->core_index_has_pending_signal[i] = false;
	}
}

void quorum_signal_initialize_all (quorum_council council [const static 1])
{
	quorum_mechanism_index i;

	for (i = 0; council->signals_count; ++i)
	{
		quorum_signal_initialize(council, i);
	}
}

static inline void signal_core
(
	quorum_council council [const static 1],
	quorum_signal signal [const restrict static 1],
	const quorum_core core [const restrict static 1],
	const quorum_core_index i,
	const quorum_attributes attributes
)
{
	signal->core_index_is_awaiting_signal[i] = false;
	signal->core_index_has_pending_signal[i] = true;
	council->core_statuses[i].pending_signal_updates += 1;

	quorum_council_notify(core, attributes, i);
}

quorum_result quorum_signal_one
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_mechanism_index signal_index,
	const quorum_attributes attributes
)
{
	quorum_signal * const signal = (council->signals + signal_index);
	quorum_result result;
	quorum_mechanism_index i;

	if (quorum_council_lock(core, attributes) != QUORUM_GRANTED)
	{
		return QUORUM_COUNCIL_LOCK_TIMEOUT;
	}

	result = QUORUM_SUCCESS;

	for (i = 0; i < QUORUM_CORE_COUNT_MAX; ++i)
	{
		if (signal->core_index_is_awaiting_signal[i])
		{
			signal_core(council, signal, core, i, attributes);

			result = QUORUM_GRANTED;

			break;
		}
	}

	quorum_council_unlock(core, attributes);

	return result;
}

quorum_result quorum_signal_all
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_mechanism_index signal_index,
	const quorum_attributes attributes
)
{
	quorum_signal * const signal = (council->signals + signal_index);
	quorum_result result;
	quorum_mechanism_index i;

	if (quorum_council_lock(core, attributes) != QUORUM_GRANTED)
	{
		return QUORUM_COUNCIL_LOCK_TIMEOUT;
	}

	result = QUORUM_SUCCESS;

	for (i = 0; i < QUORUM_CORE_COUNT_MAX; ++i)
	{
		if (signal->core_index_is_awaiting_signal[i])
		{
			signal_core(council, signal, core, i, attributes);

			result = QUORUM_GRANTED;
		}
	}

	quorum_council_unlock(core, attributes);

	return result;
}

quorum_result quorum_signal_check_and_handle_pending_update
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_attributes attributes,
	quorum_mechanism_index result_index [const restrict static 1]
)
{
	quorum_result result;
	quorum_mechanism_index i;

	result = QUORUM_FAILURE;

	if (quorum_council_lock(core, attributes) != QUORUM_GRANTED)
	{
		return QUORUM_COUNCIL_LOCK_TIMEOUT;
	}

	for (i = 0; i < council->signals_count; ++i)
	{
		if (council->signals[i].core_index_has_pending_signal[core->index])
		{
			council->signals[i].core_index_has_pending_signal[core->index] = false;
			council->core_statuses[core->index].pending_signal_updates -= 1;

			*result_index = i;

			result = QUORUM_SUCCESS;

			break;
		}
	}

	quorum_council_unlock(core, attributes);

	return result;
}

#endif

/**
 * Copyright (c) 2024 https://segfault.tech
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#include <quorum/mutex.h>

#if (QUORUM_ENABLE_MUTEX == 1)

#include <quorum/council.h>

#define QUORUM_ASSERT_MUTEX_EXISTS(council, mutex_index) \
	QUORUM_ASSERT \
	( \
		((council)->mutexes_count < (mutex_index)), \
		" Attempted to access Mutex %d out of the %d existing ones.", \
		(int) (mutex_index), \
		(int) ((council)->mutexes_count) \
	)

void quorum_mutex_initialize
(
	quorum_council council [const static 1],
	const quorum_mechanism_index mutex_index
)
{
	quorum_mutex * const mutex = (council->mutexes + mutex_index);
	quorum_core_index i;

	QUORUM_ASSERT_MUTEX_EXISTS(council, mutex_index);

	mutex->is_waiting_for_owner = false;
	mutex->owner_ident = false;
	mutex->next_waiting_core_ident = 0;

	for (i = 0; i < (QUORUM_CORE_COUNT_MAX); ++i)
	{
		mutex->waiting_core_idents[i] = 0;
	}
}

void quorum_mutex_initialize_all (quorum_council council [const static 1])
{
	quorum_mechanism_index i;

	for (i = 0; i < council->mutexes_count; ++i)
	{
		quorum_mutex_initialize(council, i);
	}
}

quorum_result quorum_mutex_lock
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_mechanism_index mutex_index,
	const quorum_attributes attributes
)
{
	const quorum_core_ident core_ident = (core->index + 1);
	quorum_mutex * const mutex = (council->mutexes + mutex_index);
	quorum_result result;
	quorum_core_ident owner_ident;

	if (quorum_council_lock(core, attributes) != QUORUM_GRANTED)
	{
		return QUORUM_COUNCIL_LOCK_TIMEOUT;
	}

	QUORUM_ASSERT_MUTEX_EXISTS(council, mutex_index);

	owner_ident = mutex->owner_ident;

	QUORUM_DEBUG_ASSERT
	(
		(owner_ident <= (QUORUM_CORE_COUNT_MAX)),
		" Mutex %d is owned by supposed Core %d, despite max core ident being %d.",
		(int) mutex_index,
		(int) owner_ident,
		(int) (QUORUM_CORE_COUNT_MAX)
	);

	QUORUM_ASSERT
	(
		(owner_ident != core_ident),
		" Trying to lock Mutex %d a second time.",
		(int) mutex_index
	);

	if (owner_ident == 0)
	{
		result = QUORUM_GRANTED;
		mutex->owner_ident = core_ident;

		QUORUM_DEBUG_ASSERT
		(
			(mutex->next_waiting_core_ident == 0),
			" Mutex %d was free, yet its waiting list had %d elements.",
			(int) mutex_index,
			(int) mutex->next_waiting_core_ident
		);
	}
	else
	{
		result = QUORUM_PENDING;

		if (!(attributes & QUORUM_IMMEDIATE))
		{
			mutex->waiting_core_idents[mutex->next_waiting_core_ident] =
				core_ident;
			mutex->next_waiting_core_ident += 1;
		}
	}

	quorum_council_unlock(core, attributes);

	return result;
}

quorum_result quorum_mutex_unlock
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_mechanism_index mutex_index,
	const quorum_attributes attributes
)
{
	const quorum_core_ident core_ident = (core->index + 1);
	quorum_mutex * const mutex = (council->mutexes + mutex_index);
	quorum_result result;

	if (quorum_council_lock(core, attributes) != QUORUM_GRANTED)
	{
		return QUORUM_COUNCIL_LOCK_TIMEOUT;
	}

	QUORUM_ASSERT_MUTEX_EXISTS(council, mutex_index);

	QUORUM_ASSERT
	(
		(mutex->owner_ident == core_ident),
		" Trying to unlock Mutex %d despite now owning it.",
		(int) mutex_index
	);

	if (mutex->next_waiting_core_ident == 0)
	{
		result = QUORUM_SUCCESS;
		mutex->owner_ident = 0;
	}
	else
	{
		const quorum_core_ident next_owner_ident = mutex->waiting_core_idents[0];
		const quorum_core_index next_owner_index = (next_owner_ident - 1);
		quorum_core_ident i;

		mutex->owner_ident = next_owner_ident;
		mutex->is_waiting_for_owner = true;

		for (i = 1; i < mutex->next_waiting_core_ident; ++i)
		{
			mutex->waiting_core_idents[i - 1] = mutex->waiting_core_idents[i];
		}

		mutex->next_waiting_core_ident -= 1;

		council->core_statuses[next_owner_index].pending_mutex_updates += 1;

		quorum_council_notify(core, attributes, next_owner_index);

		result = QUORUM_GRANTED;
	}

	quorum_council_unlock(core, attributes);

	return result;
}

quorum_result quorum_mutex_check_and_handle_pending_update
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_attributes attributes,
	quorum_mechanism_index result_index [const restrict static 1]
)
{
	const quorum_core_ident core_ident = (core->index + 1);
	quorum_result result;
	quorum_mechanism_index i;

	result = QUORUM_FAILURE;

	if (quorum_council_lock(core, attributes) != QUORUM_GRANTED)
	{
		return QUORUM_COUNCIL_LOCK_TIMEOUT;
	}

	for (i = 0; i < council->mutexes_count; ++i)
	{
		quorum_mutex * const mutex = (council->mutexes + i);

		if
		(
			(mutex->owner_ident == core_ident)
			&& mutex->is_waiting_for_owner
		)
		{
			mutex->is_waiting_for_owner = false;
			council->core_statuses[core->index].pending_mutex_updates -= 1;

			*result_index = i;

			result = QUORUM_SUCCESS;

			break;
		}
	}

	quorum_council_unlock(core, attributes);

	return result;
}

#endif

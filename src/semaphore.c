/**
 * Copyright (c) 2024 https://segfault.tech
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#include <quorum/semaphore.h>

#if (QUORUM_ENABLE_SEMAPHORE == 1)

#include <quorum/council.h>

#define QUORUM_ASSERT_SEMAPHORE_EXISTS(council, semaphore_index) \
	QUORUM_ASSERT \
	( \
		((council)->semaphores_count < (semaphore_index)), \
		" Attempted to access Semaphore %d out of the %d existing ones.", \
		(int) (semaphore_index), \
		(int) ((council)->semaphores_count) \
	)

void quorum_semaphore_initialize
(
	quorum_council council [const static 1],
	const quorum_mechanism_index semaphore_index,
	const quorum_semaphore_tokens initial_tokens
)
{
	quorum_semaphore * const semaphore = (council->semaphores + semaphore_index);
	quorum_core_ident i;

	QUORUM_ASSERT_SEMAPHORE_EXISTS(council, semaphore_index);

	semaphore->tokens = initial_tokens;

	semaphore->next_waiting_core_ident = 0;

	for (i = 0; i < QUORUM_CORE_COUNT_MAX; ++i)
	{
		semaphore->waiting_core_idents[i] = 0;
		semaphore->core_index_requested_tokens[i] = 0;
		semaphore->core_index_needs_update[i] = false;
	}
}

quorum_result quorum_semaphore_take
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_mechanism_index semaphore_index,
	const quorum_semaphore_tokens tokens,
	const quorum_attributes attributes
)
{
	quorum_semaphore * const semaphore = (council->semaphores + semaphore_index);
	quorum_result result;
	quorum_core_ident i;

	if (quorum_council_lock(core, attributes) != QUORUM_GRANTED)
	{
		return QUORUM_COUNCIL_LOCK_TIMEOUT;
	}

	QUORUM_ASSERT_SEMAPHORE_EXISTS(council, semaphore_index);

	if
	(
		(semaphore->tokens >= tokens)
		&&
		(
			(semaphore->next_waiting_core_ident == 0)
			||
			(attributes & QUORUM_ALLOW_OUT_OF_ORDER)
		)
	)
	{
		result = QUORUM_GRANTED;
	}
	else
	{
		result = QUORUM_PENDING;

		if (!(attributes & QUORUM_IMMEDIATE))
		{
			semaphore->waiting_core_idents[semaphore->next_waiting_core_ident] =
				(core->index + 1);

			semaphore->core_index_requested_tokens[core->index] = tokens;

			semaphore->next_waiting_core_ident += 1;
		}
	}

	quorum_council_unlock(core, attributes);

	return result;
}

quorum_result quorum_semaphore_give
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_mechanism_index semaphore_index,
	const quorum_semaphore_tokens tokens,
	const quorum_attributes attributes
)
{
	quorum_semaphore * const semaphore = (council->semaphores + semaphore_index);
	quorum_result result;

	if (quorum_council_lock(core, attributes) != QUORUM_GRANTED)
	{
		return QUORUM_COUNCIL_LOCK_TIMEOUT;
	}

	QUORUM_ASSERT_SEMAPHORE_EXISTS(council, semaphore_index);

	QUORUM_ASSERT
	(
		(
			(QUORUM_SEMAPHORE_TOKENS_TYPE_MAX - semaphore->tokens)
			>= tokens
		),
		" Adding %d tokens to Semaphore %d would cause an overflow (%d current"
		" tokens, %d QUORUM_SEMAPHORE_TOKENS_TYPE_MAX).",
		(int) tokens,
		(int) semaphore_index,
		(int) semaphore->tokens,
		(int) QUORUM_SEMAPHORE_TOKENS_TYPE_MAX
	);

	result = QUORUM_SUCCESS;

	if (semaphore->next_waiting_core_ident == 0)
	{
		semaphore->tokens += tokens;
	}
	else
	{
		const quorum_core_ident max_candidate =
			(
				(attributes & QUORUM_ALLOW_OUT_OF_ORDER) ?
				semaphore->next_waiting_core_ident
				:
				1
			);
		quorum_semaphore_tokens remaining_tokens;
		quorum_core_ident i;


		remaining_tokens = semaphore->tokens + tokens;

		i = 0;

		while (i < max_candidate)
		{
			const quorum_core_index candidate_index =
				(semaphore->waiting_core_idents[i] - 1);
			const quorum_semaphore_tokens candidate_request =
				semaphore->core_index_requested_tokens[candidate_index];

			if (candidate_request <= remaining_tokens)
			{
				quorum_core_ident j;

				remaining_tokens -= candidate_request;
				semaphore->core_index_requested_tokens[candidate_index] = 0;
				council->core_statuses[candidate_index].pending_semaphore_updates += 1;

				for (j = i + 1; j < semaphore->next_waiting_core_ident; ++j)
				{
					semaphore->waiting_core_idents[i] =
						semaphore->waiting_core_idents[j];
				}

				/* Could not be zero if this code was reached. */
				semaphore->next_waiting_core_ident--;

				quorum_council_notify(core, attributes, candidate_index);

				result = QUORUM_GRANTED;
			}

			++i;
		}

		semaphore->tokens = remaining_tokens;
	}

	quorum_council_unlock(core, attributes);

	return result;
}

quorum_result quorum_semaphore_check_and_handle_pending_update
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

	for (i = 0; i < council->semaphores_count; ++i)
	{
		quorum_semaphore * const semaphore = (council->semaphores + i);

		if (semaphore->core_index_needs_update[core->index])
		{
			semaphore->core_index_needs_update[core->index] = false;
			council->core_statuses[core->index].pending_semaphore_updates -= 1;

			*result_index = i;

			result = QUORUM_SUCCESS;

			break;
		}
	}

	quorum_council_unlock(core, attributes);

	return result;
}

#endif

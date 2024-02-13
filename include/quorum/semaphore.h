/**
 * Copyright (c) 2024 https://segfault.tech
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#ifndef QUORUM_INCLUDE_SEMAPHORE_H
#define QUORUM_INCLUDE_SEMAPHORE_H

#include "pervasive.h"

#if (QUORUM_ENABLE_SEMAPHORE == 1)

#include "semaphore/types.h"
#include "council/types.h"

void quorum_semaphore_initialize
(
	quorum_council council [const static 1],
	const quorum_mechanism_index semaphore_index,
	const quorum_semaphore_tokens initial_tokens
);

quorum_result quorum_semaphore_take
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_mechanism_index semaphore_index,
	const quorum_semaphore_tokens tokens,
	const quorum_attributes attributes
);

quorum_result quorum_semaphore_give
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_mechanism_index semaphore_index,
	const quorum_semaphore_tokens tokens,
	const quorum_attributes attributes
);

quorum_result quorum_semaphore_check_and_handle_pending_update
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_attributes attributes,
	quorum_mechanism_index result_index [const restrict static 1]
);

#endif

#endif

/**
 * Copyright (c) 2024 https://segfault.tech
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#ifndef QUORUM_INCLUDE_BARRIER_H
#define QUORUM_INCLUDE_BARRIER_H

#include "pervasive.h"

#if (QUORUM_ENABLE_BARRIER == 1)

#include <stdbool.h>

#include "barrier/types.h"
#include "council/types.h"

void quorum_barrier_initialize
(
	quorum_council council [const static 1],
	const quorum_mechanism_index barrier_index,
	const quorum_core_index initial_countdown
);

quorum_result quorum_barrier_wait_on
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_mechanism_index barrier_index,
	const quorum_attributes attributes
);

quorum_result quorum_barrier_check_and_handle_pending_update
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	quorum_mechanism_index unlocked_barrier_index [const restrict static 1],
	const quorum_attributes attributes
);

#endif

#endif

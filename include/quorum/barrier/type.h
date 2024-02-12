/**
 * Copyright (c) 2024 https://segfault.tech
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#ifndef QUORUM_INCLUDE_BARRIER_TYPE_H
#define QUORUM_INCLUDE_BARRIER_TYPE_H

#include "pervasive.h"

#if (QUORUM_ENABLE_BARRIER == 1)

#include <stdbool.h>

typedef struct
{
	/*
	 * Number of additional cores needed for the barrier to open.
	 */
	quorum_core_index current_countdown;

	/*
	 * Number of cores required for the barrier to open after it closes again.
	 */
	quorum_core_index initial_countdown;

	/* Is [core_index] waiting for the barrier to open? */
	bool core_index_is_waiting[QUORUM_CORE_COUNT_MAX];

	/* Is [core_index] needing to be informed the barrier opened? */
	bool core_index_needs_update[QUORUM_CORE_COUNT_MAX];
} quorum_barrier;

#endif

#endif

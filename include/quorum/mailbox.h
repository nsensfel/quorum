/**
 * Copyright (c) 2024 https://segfault.tech
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#ifndef QUORUM_INCLUDE_SEMAPHORE_H
#define QUORUM_INCLUDE_SEMAPHORE_H

#include "pervasive.h"

#if (QUORUM_ENABLE_SEMAPHORE == 1)

quorum_result quorum_semaphore_initialize
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_mechanism_index barrier_index,
	const quorum_semaphore_token_count initial_count,
	const quorum_attributes attributes
);

#endif

#endif

/**
 * Copyright (c) 2024 https://segfault.tech
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#ifndef QUORUM_INCLUDE_MUTEX_H
#define QUORUM_INCLUDE_MUTEX_H

#include "pervasive.h"

#if (QUORUM_ENABLE_MUTEX == 1)

#include "mutex/types.h"
#include "council/types.h"

void quorum_mutex_initialize
(
	quorum_council council [const static 1],
	const quorum_mechanism_index mutex_index
);

void quorum_mutex_initialize_all (quorum_council council [const static 1]);

quorum_result quorum_mutex_lock
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_mechanism_index mutex_index,
	const quorum_attributes attributes
);

quorum_result quorum_mutex_unlock
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_mechanism_index mutex_index,
	const quorum_attributes attributes
);

quorum_result quorum_mutex_check_and_handle_pending_update
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_attributes attributes,
	quorum_mechanism_index result_index [const restrict static 1]
);

#endif

#endif

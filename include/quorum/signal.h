/**
 * Copyright (c) 2024 https://segfault.tech
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#ifndef QUORUM_INCLUDE_SIGNAL_H
#define QUORUM_INCLUDE_SIGNAL_H

#include "pervasive.h"

#if (QUORUM_ENABLE_SIGNAL == 1)

#include "signal/types.h"
#include "council/types.h"

void quorum_signal_initialize
(
	quorum_council council [const static 1],
	const quorum_mechanism_index signal_index
);

void quorum_signal_initialize_all (quorum_council council [const static 1]);

quorum_result quorum_signal_one
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_mechanism_index signal_index,
	const quorum_attributes attributes
);

quorum_result quorum_signal_all
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_mechanism_index signal_index,
	const quorum_attributes attributes
);

quorum_result quorum_signal_check_and_handle_pending_update
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_attributes attributes,
	quorum_mechanism_index result_index [const restrict static 1]
);

#endif

#endif

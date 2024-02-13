/**
 * Copyright (c) 2024 https://segfault.tech
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#ifndef QUORUM_INCLUDE_SIGNAL_TYPES_H
#define QUORUM_INCLUDE_SIGNAL_TYPES_H

#include "../pervasive.h"

#if (QUORUM_ENABLE_SIGNAL == 1)

#include <stdbool.h>

typedef struct
{
	bool core_index_is_awaiting_signal[QUORUM_CORE_COUNT_MAX];
	bool core_index_has_pending_signal[QUORUM_CORE_COUNT_MAX];
} quorum_signal;

#endif

#endif

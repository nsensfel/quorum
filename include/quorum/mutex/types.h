/**
 * Copyright (c) 2024 https://segfault.tech
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#ifndef QUORUM_INCLUDE_MUTEX_TYPES_H
#define QUORUM_INCLUDE_MUTEX_TYPES_H

#include "../pervasive.h"

#if (QUORUM_ENABLE_MUTEX == 1)

#include <stdbool.h>

typedef struct
{
	bool is_waiting_for_owner;

	quorum_core_ident owner_ident;

	quorum_core_index next_waiting_core_ident;
	quorum_core_ident waiting_core_idents[QUORUM_CORE_COUNT_MAX];
} quorum_mutex;

#endif

#endif

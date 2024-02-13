/**
 * Copyright (c) 2024 https://segfault.tech
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#ifndef QUORUM_INCLUDE_SEMAPHORE_TYPES_H
#define QUORUM_INCLUDE_SEMAPHORE_TYPES_H

#include "../pervasive.h"

#if (QUORUM_ENABLE_SEMAPHORE == 1)

#include <stdint.h>
#include <stdbool.h>

#ifndef QUORUM_SEMAPHORE_TOKENS_TYPE
	#pragma message "Using default QUORUM_SEMAPHORE_TOKENS_TYPE: uint8_t."
	#define QUORUM_SEMAPHORE_TOKENS_TYPE uint8_t
	#define QUORUM_SEMAPHORE_TOKENS_TYPE_MAX UINT8_MAX
#endif

#ifndef QUORUM_SEMAPHORE_TOKENS_TYPE_MAX
	#error "Please define QUORUM_SEMAPHORE_TOKENS_TYPE_MAX for your custom QUORUM_SEMAPHORE_TOKENS_TYPE."
#endif

typedef QUORUM_SEMAPHORE_TOKENS_TYPE quorum_semaphore_tokens;

typedef struct
{
	quorum_semaphore_tokens tokens;

	quorum_core_ident next_waiting_core_ident;
	quorum_core_ident waiting_core_idents[QUORUM_CORE_COUNT_MAX];
	quorum_semaphore_tokens core_index_requested_tokens[QUORUM_CORE_COUNT_MAX];

	bool core_index_needs_update[QUORUM_CORE_COUNT_MAX];
} quorum_semaphore;

#endif

#endif

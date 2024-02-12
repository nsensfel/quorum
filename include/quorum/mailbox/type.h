/**
 * Copyright (c) 2024 https://segfault.tech
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#ifndef QUORUM_INCLUDE_MAILBOX_TYPE_H
#define QUORUM_INCLUDE_MAILBOX_TYPE_H

#include "pervasive.h"

#if (QUORUM_ENABLE_MAILBOX == 1)

#include <stdbool.h>
#include <stdint.h>

typedef struct
{
	quorum_core_index owner;

	quorum_mailbox_size size;
	uint8_t * data;

	quorum_mailbox_size next_free_byte;
	quorum_mailbox_size reserved_space_size;

	quorum_core_index next_waiting_core_index;
	quorum_core_ident waiting_core_idents[QUORUM_CORE_COUNT_MAX];
	quorum_core_ident waiting_core_requests[QUORUM_CORE_COUNT_MAX];

	quorum_mailbox_size pending_read_buffer_size;

	bool pending_read_should_update;

	bool core_index_has_pending_updates[QUORUM_CORE_COUNT_MAX];
} quorum_mailbox;

#endif

#endif

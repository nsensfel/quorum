/**
 * Copyright (c) 2024 https://segfault.tech
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#ifndef QUORUM_INCLUDE_MAILBOX_TYPES_H
#define QUORUM_INCLUDE_MAILBOX_TYPES_H

#include "../pervasive.h"

#if (QUORUM_ENABLE_MAILBOX == 1)

#include <stdbool.h>
#include <stdint.h>

#ifndef QUORUM_MAILBOX_SIZE_TYPE
	#pragma message "Using default QUORUM_MAILBOX_SIZE_TYPE: uint32_t"
	#define QUORUM_MAILBOX_SIZE_TYPE uint32_t
#endif

#ifndef QUORUM_MESSAGE_SIZE_TYPE
	#pragma message "Using default QUORUM_MESSAGE_SIZE_TYPE: uint8_t"
	#define QUORUM_MESSAGE_SIZE_TYPE uint8_t
	#define QUORUM_MESSAGE_SIZE_MAX UINT8_MAX
#endif

typedef QUORUM_MAILBOX_SIZE_TYPE quorum_mailbox_size;
typedef QUORUM_MESSAGE_SIZE_TYPE quorum_message_size;

#ifndef QUORUM_MESSAGE_SIZE_MAX
	#error \
		"Please define QUORUM_MESSAGE_SIZE_MAX according to" \
		" QUORUM_MESSAGE_SIZE_TYPE. Bear in mind that the largest possible" \
		" message will then be " \
		" (QUORUM_MESSAGE_SIZE_MAX - sizeof(QUORUM_MESSAGE_SIZE_TYPE)) bytes."
#endif

typedef struct
{
	quorum_core_index owner;

	quorum_mailbox_size size;
	uint8_t * data;

	quorum_mailbox_size next_free_byte;
	quorum_mailbox_size reserved_space_size;

	quorum_core_index next_waiting_core_index;
	quorum_core_ident waiting_core_idents[QUORUM_CORE_COUNT_MAX];
	quorum_message_size core_index_requested_space[QUORUM_CORE_COUNT_MAX];

	quorum_mailbox_size pending_read_buffer_size;

	bool owner_has_pending_read_update;
	bool core_index_has_reserved_space_update[QUORUM_CORE_COUNT_MAX];
} quorum_mailbox;

#endif

#endif

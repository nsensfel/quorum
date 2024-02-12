/**
 * Copyright (c) 2024 https://segfault.tech
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#ifndef QUORUM_INCLUDE_MAILBOX_H
#define QUORUM_INCLUDE_MAILBOX_H

#include "pervasive.h"

#if (QUORUM_ENABLE_MAILBOX == 1)

#include <stdint.h>

quorum_result quorum_mailbox_initialize
(
	quorum_council council [const static 1],
	const quorum_mechanism_index mailbox_index,
	const quorum_mailbox_size mailbox_size,
	uint8_t mailbox_data [const static mailbox_size],
	const quorum_core_index owner_core_index
);

quorum_result quorum_mailbox_send
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_mechanism_index mailbox_index,
	const quorum_message_size message_size,
	const uint8_t message [const restrict static message_size],
	const quorum_attributes attributes
);

quorum_result quorum_mailbox_send_to_allocated
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_mechanism_index mailbox_index,
	const quorum_message_size message_size,
	const uint8_t message [const static message_size],
	const quorum_attributes attributes
);

quorum_result quorum_mailbox_receive
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_message_size buffer_size,
	uint8_t buffer [const restrict static buffer_size],
	quorum_message_size output_size [const restrict static 1],
	const quorum_attributes attributes
);
#endif

#endif

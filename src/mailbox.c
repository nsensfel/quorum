/**
 * Copyright (c) 2024 https://segfault.tech
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#include <quorum/mailbox.h>

#if (QUORUM_ENABLE_MAILBOX == 1)

#define QUORUM_ASSERT_MAILBOX_EXISTS(council, mailbox_index) \
	QUORUM_ASSERT \
	( \
		((council)->mailboxes_count < (mailbox_index)), \
		" Attempted to access Mailbox %d out of the %d existing ones.", \
		(int) (mailbox_index), \
		(int) ((council)->mailboxes_count) \
	)

quorum_result quorum_mailbox_initialize
(
	quorum_council council [const static 1],
	const quorum_mechanism_index mailbox_index,
	const quorum_mailbox_size mailbox_size,
	uint8_t mailbox_data [const static mailbox_size],
	const quorum_core_index owner_core_index
)
{
	QUORUM_ASSERT_MAILBOX_EXISTS(council, mailbox_index);

	quorum_mailbox * const mailbox = (council->mailboxes + mailbox_index);

}

quorum_result quorum_mailbox_send
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_mechanism_index mailbox_index,
	const quorum_message_size message_size,
	const uint8_t message [const restrict static message_size],
	const quorum_attributes attributes
)
{

}

quorum_result quorum_mailbox_send_to_allocated
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_mechanism_index mailbox_index,
	const quorum_message_size message_size,
	const uint8_t message [const static message_size],
	const quorum_attributes attributes
)
{

}

quorum_result quorum_mailbox_receive
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_message_size buffer_size,
	uint8_t buffer [const restrict static buffer_size],
	quorum_message_size output_size [const restrict static 1],
	const quorum_attributes attributes
)
{

}

#endif

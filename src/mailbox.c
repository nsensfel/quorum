/**
 * Copyright (c) 2024 https://segfault.tech
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#include <quorum/mailbox.h>

#if (QUORUM_ENABLE_MAILBOX == 1)

#include <string.h>

#include <quorum/council.h>

#define QUORUM_ASSERT_MAILBOX_EXISTS(council, mailbox_index) \
	QUORUM_ASSERT \
	( \
		((council)->mailboxes_count < (mailbox_index)), \
		" Attempted to access Mailbox %d out of the %d existing ones.", \
		(int) (mailbox_index), \
		(int) ((council)->mailboxes_count) \
	)

void quorum_mailbox_initialize
(
	quorum_council council [const static 1],
	const quorum_mechanism_index mailbox_index,
	const quorum_mailbox_size mailbox_size,
	uint8_t mailbox_data [const static mailbox_size],
	const quorum_core_index owner_core_index
)
{
	quorum_mailbox * const mailbox = (council->mailboxes + mailbox_index);
	quorum_core_index i;

	QUORUM_ASSERT_MAILBOX_EXISTS(council, mailbox_index);

	mailbox->owner = owner_core_index;
	mailbox->data = mailbox_data;
	mailbox->size = mailbox_size;

	mailbox->next_free_byte = 0;
	mailbox->reserved_space_size = 0;

	mailbox->next_waiting_core_index = 0;

	for (i = 0; i < (QUORUM_CORE_COUNT_MAX); ++i)
	{
		mailbox->waiting_core_idents[i] = 0;
		mailbox->core_index_requested_space[i] = 0;
		mailbox->core_index_has_reserved_space_update[i] = false;
	}

	mailbox->pending_read_buffer_size = 0;

	mailbox->owner_has_pending_read_update = false;

	memset(mailbox_data, /* value = */ 0, mailbox_size);
}

static inline void copy_to_mailbox
(
	quorum_council council [const static 1],
	quorum_mailbox mailbox [const restrict static 1],
	const quorum_core core [const restrict static 1],
	const quorum_message_size message_size,
	const uint8_t message [const restrict static message_size],
	const quorum_attributes attributes
)
{
	uint8_t * data_destination;

	/* Write message size */
	*((quorum_message_size *) (mailbox->data + mailbox->next_free_byte)) =
		message_size;

	data_destination =
		(
			mailbox->data
			+ mailbox->next_free_byte
			+ sizeof(quorum_message_size)
		);

	memcpy(data_destination, message, message_size);

	mailbox->next_free_byte += sizeof(quorum_message_size) + message_size;

	if (message_size <= mailbox->pending_read_buffer_size)
	{
		mailbox->pending_read_buffer_size = 0;
		mailbox->owner_has_pending_read_update = true;

		council->core_statuses[mailbox->owner].pending_mailbox_updates += 1;

		quorum_council_notify(core, attributes, mailbox->owner);
	}
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
	const quorum_message_size frame_size =
		(sizeof(quorum_message_size) + message_size);
	quorum_mailbox * const mailbox = (council->mailboxes + mailbox_index);
	quorum_result result;

	QUORUM_ASSERT
	(
		(
			(QUORUM_MESSAGE_SIZE_MAX - sizeof(quorum_message_size))
			>= message_size
		),
		" Cannot create a message of size %d. With the defined message types,"
		" the maximum is %d bytes.",
		(int) message_size,
		(int) (QUORUM_MESSAGE_SIZE_MAX - sizeof(quorum_message_size))
	);

	if (quorum_council_lock(core, attributes) != QUORUM_GRANTED)
	{
		return QUORUM_COUNCIL_LOCK_TIMEOUT;
	}

	QUORUM_ASSERT_MAILBOX_EXISTS(council, mailbox_index);

	if (!(attributes & QUORUM_IMMEDIATE))
	{
		QUORUM_ASSERT
		(
			(mailbox->core_index_requested_space[core->index] == 0),
			" Attempting to send multiple non-immediate messages at once, on"
			" Mailbox %d.",
			(int) mailbox_index
		);
	}

	QUORUM_ASSERT
	(
		(mailbox->size >= frame_size),
		" Mailbox %d has a size of %d bytes. It will never be able to receive"
		" the %d bytes being sent. Note: the message being sent includes an"
		" overhead of %d bytes.",
		(int) mailbox_index,
		(int) mailbox->size,
		(int) frame_size,
		(int) sizeof(quorum_message_size)
	);

	/* Can the mailbox receive this message immediately? */
	if
	(
		( /* No because not enough room right now. */
			(
				mailbox->size
				- mailbox->next_free_byte
				- mailbox->reserved_space_size
			)
			< frame_size
		)
		||
		( /* No because strict FIFO order imposed + someone is already waiting. */
			(mailbox->next_waiting_core_index > 0)
			&& !(attributes & QUORUM_ALLOW_OUT_OF_ORDER)
		)
	)
	{
		/* No, it cannot. */

		result = QUORUM_PENDING;

		/* Do we join the waiting queue or leave immediately? */
		if (!(attributes & QUORUM_IMMEDIATE))
		{
			/* Join the waiting queue. */
			mailbox->core_index_requested_space[core->index] = frame_size;
			mailbox->waiting_core_idents[mailbox->next_waiting_core_index] =
				(core->index + 1);

			mailbox->next_waiting_core_index += 1;
		}
	}
	else
	{
		/* Yes, it can. */

		copy_to_mailbox(council, mailbox, core, message_size, message, attributes);

		result = QUORUM_GRANTED;
	}

	quorum_council_unlock(core, attributes);

	return result;
}

quorum_result quorum_mailbox_send_to_reserved_space
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_mechanism_index mailbox_index,
	const quorum_message_size message_size,
	const uint8_t message [const restrict static message_size],
	const quorum_attributes attributes
)
{
	quorum_mailbox * const mailbox = (council->mailboxes + mailbox_index);
	quorum_result result;

	if (quorum_council_lock(core, attributes) != QUORUM_GRANTED)
	{
		return QUORUM_COUNCIL_LOCK_TIMEOUT;
	}

	QUORUM_ASSERT_MAILBOX_EXISTS(council, mailbox_index);
	QUORUM_ASSERT
	(
		(
			mailbox->core_index_requested_space[core->index]
			>=
			(message_size + sizeof(quorum_message_size))
		),
		" Attempting to send more than the reserve space on Mailbox %d"
		" [%d/%d]. Note: both values include an overhead of %d bytes.",
		(int) mailbox_index,
		(int) (message_size + sizeof(quorum_message_size)),
		(int) mailbox->core_index_requested_space[core->index],
		(int) sizeof(quorum_message_size)
	);

	copy_to_mailbox(council, mailbox, core, message_size, message, attributes);

	mailbox->reserved_space_size -= mailbox->core_index_requested_space[core->index];
	mailbox->core_index_requested_space[core->index] = 0;

	result = QUORUM_GRANTED;

	quorum_council_unlock(core, attributes);

	return result;
}

quorum_result quorum_mailbox_receive
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_mechanism_index mailbox_index,
	const quorum_message_size buffer_size,
	uint8_t buffer [const restrict static buffer_size],
	quorum_message_size output_size [const restrict static 1],
	const quorum_attributes attributes
)
{
	quorum_mailbox * const mailbox = (council->mailboxes + mailbox_index);
	quorum_result result;
	quorum_mailbox_size i;

	if (quorum_council_lock(core, attributes) != QUORUM_GRANTED)
	{
		return QUORUM_COUNCIL_LOCK_TIMEOUT;
	}

	QUORUM_ASSERT_MAILBOX_EXISTS(council, mailbox_index);

	i = 0;

	result = QUORUM_PENDING;

	while (i < mailbox->next_free_byte)
	{
		quorum_message_size * next_message_size;

		next_message_size = (quorum_message_size *) (mailbox->data + i);

		if ((*next_message_size) > buffer_size)
		{
			i += sizeof(quorum_message_size) + (*next_message_size);

			continue;
		}
		else
		{
			const quorum_message_size frame_size =
				sizeof(quorum_message_size) + *next_message_size;
			quorum_core_index j, k;
			quorum_core_index max_candidate;
			quorum_mailbox_size available_space;

			*output_size = *next_message_size;

			/* Copies into output buffer. */
			memcpy
			(
				buffer,
				(mailbox->data + i + sizeof(quorum_message_size)),
				*next_message_size
			);

			if ((i + frame_size) < mailbox->next_free_byte)
			{
				/*
				 * Shifts all following messages, to remove the one that was just
				 * read.
				 */
				memmove
				(
					(mailbox->data + i),
					(mailbox->data + i + frame_size),
					mailbox->next_free_byte - i - frame_size
				);
			}

			mailbox->next_free_byte -= frame_size;

			available_space =
				(
					mailbox->size
					- mailbox->next_free_byte
					- mailbox->reserved_space_size
				);

			max_candidate = mailbox->next_waiting_core_index;

			/*
			 * If we allow out of order, then we read the next message that fits
			 * the buffer, in order of arrival. Otherwise, we only read the next
			 * message (also in order of arrival) only if that message fits the
			 * buffer.
			 */
			if ((max_candidate > 0) && (attributes & QUORUM_ALLOW_OUT_OF_ORDER))
			{
				max_candidate = mailbox->next_waiting_core_index;
			}

			j = 0;

			while ((j < max_candidate))
			{
				const quorum_core_index waiting_core_index =
					mailbox->waiting_core_idents[j] - 1;

				const quorum_message_size requested_space =
					mailbox->core_index_requested_space[waiting_core_index];

				if (requested_space < available_space)
				{
					available_space -= requested_space;
					mailbox->reserved_space_size += requested_space;
					mailbox->core_index_has_reserved_space_update[waiting_core_index] = true;
					council->core_statuses[waiting_core_index].pending_mailbox_updates += 1;

					for (k = j + 1; k < mailbox->next_waiting_core_index; ++k)
					{
						mailbox->waiting_core_idents[j] =
							mailbox->waiting_core_idents[k];
					}

					/* Could not be zero if this code was reached. */
					mailbox->next_waiting_core_index--;

					quorum_council_notify(core, attributes, waiting_core_index);
				}
				else
				{
					j++;
				}
			}

			result = QUORUM_GRANTED;

			break;
		}
	}

	if (result == QUORUM_PENDING)
	{
		if (!(attributes & QUORUM_IMMEDIATE))
		{
			mailbox->pending_read_buffer_size = buffer_size;
		}
	}

	quorum_council_unlock(core, attributes);

	return result;
}

quorum_result quorum_mailbox_check_and_handle_pending_update
(
	quorum_council council [const static 1],
	const quorum_core core [const restrict static 1],
	const quorum_attributes attributes,
	quorum_mechanism_index result_index [const restrict static 1],
	bool update_indicates_writing [const restrict static 1]
)
{
	quorum_result result;
	quorum_mechanism_index i;

	result = QUORUM_FAILURE;

	if (quorum_council_lock(core, attributes) != QUORUM_GRANTED)
	{
		return QUORUM_COUNCIL_LOCK_TIMEOUT;
	}

	for (i = 0; council->mailboxes_count; ++i)
	{
		QUORUM_ASSERT_MAILBOX_EXISTS(council, i);

		quorum_mailbox * const mailbox = (council->mailboxes + i);

		if
		(
			(mailbox->owner == core->index)
			&& mailbox->owner_has_pending_read_update
		)
		{
			mailbox->owner_has_pending_read_update = false;
			*update_indicates_writing = false;

			council->core_statuses[core->index].pending_mailbox_updates -= 1;

			*result_index = i;
			result = QUORUM_SUCCESS;

			break;
		}
		else if (mailbox->core_index_has_reserved_space_update[core->index])
		{
			mailbox->core_index_has_reserved_space_update[core->index] = false;
			*update_indicates_writing = true;
			council->core_statuses[core->index].pending_mailbox_updates -= 1;

			*result_index = i;
			result = QUORUM_SUCCESS;

			break;
		}
	}

	quorum_council_unlock(core, attributes);

	return result;
}

#endif

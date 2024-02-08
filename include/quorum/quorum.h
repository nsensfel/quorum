/**
 * Copyright (c) 2024 https://segfault.tech
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#ifndef QUORUM_INCLUDE_QUORUM_H
#define QUORUM_INCLUDE_QUORUM_H

#include "pervasive.h"
#include "council.h"

#if (QUORUM_ENABLE_MUTEX == 1)
	#include "mutex.h"
#endif

#if (QUORUM_ENABLE_BARRIER == 1)
	#include "barrier.h"
#endif

#if (QUORUM_ENABLE_SEMAPHORE == 1)
	#include "semaphore.h"
#endif

#if (QUORUM_ENABLE_SIGNAL == 1)
	#include "signal.h"
#endif

#if (QUORUM_ENABLE_MAILBOX == 1)
	#include "mailbox.h"
#endif

#endif

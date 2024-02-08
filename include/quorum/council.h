/**
 * Copyright (c) 2024 https://segfault.tech
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **/

#ifndef QUORUM_INCLUDE_COUNCIL_H
#define QUORUM_INCLUDE_COUNCIL_H

#include "pervasive.h"

/*
 * First CPU to get the HW mutex initializes.
 * Others don't.
 * Role is indicated by return value.
 */
quorum_result quorum_council_initialize_preamble
(
	quorum_council [const static 1]
);

quorum_result quorum_council_initialize_epilogue
(
	quorum_council [const static 1]
);

#endif

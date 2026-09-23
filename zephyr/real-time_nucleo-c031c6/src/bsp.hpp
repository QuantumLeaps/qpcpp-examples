//============================================================================
// "real-time" example to demonstrate timing in QP/C++
//
// Copyright (C) 2005-2026 Quantum Leaps, LLC. All rights reserved.
//
//                    Q u a n t u m  L e a P s
//                    ------------------------
//                    Modern Embedded Software
//
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-QL-commercial
//
// This software is dual-licensed under the terms of the open-source GNU
// General Public License, version 3 or later (GPL-3.0-or-later), or,
// alternatively, under the terms of one of the closed-source Quantum Leaps
// commercial licenses.
//
// Redistributions in source code must retain this top-level comment block.
//
// NOTE:
// The GPL does NOT permit distributing this code as part of proprietary
// (closed-source) programs. For such use, please contact Quantum Leaps for
// a commercial license, which is an alternative to the GPL designed
// explicitly for closed-source distribution.
//
// Quantum Leaps contact information:
// <www.state-machine.com/licensing>
// <info@state-machine.com>
//============================================================================
#ifndef BSP_HPP_
#define BSP_HPP_

namespace BSP {

constexpr std::uint32_t TICKS_PER_SEC {CONFIG_SYS_CLOCK_TICKS_PER_SEC};

void init(void const * const arg);

void d1on();
void d1off();

void d2on();
void d2off();

void d3on();
void d3off();

void d4on();
void d4off();

void d5on();
void d5off();

void d6on();
void d6off();

void d7on();
void d7off();

// immutable events for Periodic active objects
QP::QEvt const *getEvtPeriodic1(std::uint8_t num);
QP::QEvt const *getEvtPeriodic4(std::uint8_t num);

} // namespace BSP

#endif // BSP_HPP_

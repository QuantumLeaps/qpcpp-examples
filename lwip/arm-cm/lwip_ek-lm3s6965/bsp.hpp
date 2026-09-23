//============================================================================
// Product: BSP for DPP application with lwIP on EV-LM3S9665 board
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

#define BSP_TICKS_PER_SEC    100U

void BSP_init(void const * const arg);

// RITEK 128x96x4 OLED used in Rev C-D boards
#define RITEK_OLED

// OSRAM 128x64x4 OLED used in REV A-B boards
//#define OSRAM_OLED

extern "C" {

void ISR_SysTick(void);
void ISR_Ethernet(void);
void ISR_Nmi(void);
void ISR_Fault(void);
void ISR_DefaultHandler(void);

}

#endif // BSP_HPP_

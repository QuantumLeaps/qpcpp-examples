//============================================================================
// Product: DPP example (console)
// Last updated for version 8.0.0
// Last updated on  2024-07-31
//
//                   Q u a n t u m  L e a P s
//                   ------------------------
//                   Modern Embedded Software
//
// Copyright (C) 2005 Quantum Leaps, LLC <state-machine.com>.
//
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-QL-commercial
//
// This software is dual-licensed under the terms of the open source GNU
// General Public License version 3 (or any later version), or alternatively,
// under the terms of one of the closed source Quantum Leaps commercial
// licenses.
//
// The terms of the open source GNU General Public License version 3
// can be found at: <www.gnu.org/licenses/gpl-3.0>
//
// The terms of the closed source Quantum Leaps commercial licenses
// can be found at: <www.state-machine.com/licensing>
//
// Redistributions in source code must retain this top-level comment block.
// Plagiarizing this software to sidestep the license obligations is illegal.
//
// Contact information:
// <www.state-machine.com>
// <info@state-machine.com>
//============================================================================
#ifndef BSP_HPP_
#define BSP_HPP_

namespace BSP {

constexpr std::uint32_t TICKS_PER_SEC {100U};

void init();
void start();
void displayPaused(std::uint8_t const paused);
void displayPhilStat(std::uint8_t const n, char const *stat);
void terminate(std::int16_t const result);

void randomSeed(std::uint32_t const seed); // random seed
std::uint32_t random(void); // pseudo-random generator

// for testing...
void wait4SW1(void);
void ledOn(void);
void ledOff(void);

} // namespace BSP

#endif // BSP_HPP_


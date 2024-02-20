//***************************************************************************
// Product: BSP for "Blinky" example
// Last updated for version 6.3.1
// Last updated on  2018-05-21
//
//                    Q u a n t u m     L e a P s
//                    ---------------------------
//                    innovating embedded systems
//
// Copyright (C) Quantum Leaps, LLC. All rights reserved.
//
// This program is open source software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Alternatively, this program may be distributed and modified under the
// terms of Quantum Leaps commercial licenses, which expressly supersede
// the GNU General Public License and are specifically designed for
// licensees interested in retaining the proprietary status of their code.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <www.gnu.org/licenses/>.
//
// Contact information:
// <www.state-machine.com/licensing>
// <info@state-machine.com>
//============================================================================
#ifndef BSP_HPP
#define BSP_HPP

// a very simple Board Support Package (BSP) -----========--------------------
class BSP {
public:
    enum { TICKS_PER_SEC = 100 }; // number of clock ticks in a second
    static void init(void);
    static void ledOff(void);
    static void ledOn(void);
};

// active object(s) used in this application -------------------------------
extern QP::QActive * const AO_Blinky; // opaque pointer to the Blinky AO

#endif // BSP_HPP

//============================================================================
// Product: Console-based BSP
// Last Updated for Version: 7.4.0
// Date of the Last Update:  2024-06-05
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
#include "qpcpp.hpp"
#include "bsp.hpp"

#include <iostream>
#include <cstdlib>  // for exit()

//............................................................................
void BSP::init(int /*argc*/, char * /*argv*/[]) {
}
//............................................................................
void QP::QF::onStartup(void) {
    QP::QF::setTickRate(BSP::TICKS_PER_SEC, 30); // set the desired tick rate
    QP::QF::consoleSetup();
}
//............................................................................
void QP::QF::onCleanup(void) {
    std::cout << "\nBye!Bye!\n";
    QP::QF::consoleCleanup();
}
//............................................................................
void QP::QF::onClockTick(void) {
    QP::QTimeEvt::TICK_X(0U, nullptr); // the QF clock tick processing
    int key = QP::QF::consoleGetKey();
    if (key != 0U) { // any key pressed?
        BSP::onKeyboardInput((uint8_t)key);
    }
}
//............................................................................
extern "C" Q_NORETURN Q_onError(char const * const module, int_t const id) {
    std::cerr << "Assertion failed in " << module << ":" << id << std::endl;
    QP::QF::onCleanup();
    exit(-1);
}

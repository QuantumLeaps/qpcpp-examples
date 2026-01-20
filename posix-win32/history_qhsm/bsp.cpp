//============================================================================
// BSP for the "history" example
//
// Copyright (C) 2005 Quantum Leaps, LLC. All rights reserved.
//
//                    Q u a n t u m  L e a P s
//                    ------------------------
//                    Modern Embedded Software
//
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-QL-commercial
//
// This software is dual-licensed under the terms of the open-source GNU
// General Public License (GPL) or under the terms of one of the closed-
// source Quantum Leaps commercial licenses.
//
// Redistributions in source code must retain this top-level comment block.
// Plagiarizing this software to sidestep the license obligations is illegal.
//
// NOTE:
// The GPL does NOT permit the incorporation of this code into proprietary
// programs. Please contact Quantum Leaps for commercial licensing options,
// which expressly supersede the GPL and are designed explicitly for
// closed-source distribution.
//
// Quantum Leaps contact information:
// <www.state-machine.com/licensing>
// <info@state-machine.com>
//============================================================================
#include "qpcpp.hpp"        // QP/C++ real-time event framework
#include "bsp.hpp"          // Board Support Package
#include "app.hpp"          // Application

#include <iostream>         // for cout/cerr
#include <iomanip>          // for std::setw
#include <stdlib.h>         // for exit()

//============================================================================
namespace { // unnamed namespace for local stuff with internal linkage

Q_DEFINE_THIS_MODULE("bsp")

// Local objects -------------------------------------------------------------
static QP::QTicker l_Ticker0_inst(0U); // "ticker" AO for tick rate 0
QP::QTicker * const the_Ticker0 = &l_Ticker0_inst;


} // unnamed namespace

#ifdef Q_SPY
    #error This application does not provide Spy build configuration
#endif

//============================================================================
extern "C" {

//............................................................................
Q_NORETURN Q_onError(char const * const module, int_t id) {
    std::cerr << "ERROR in " << module << ':' << id << std::endl;
    QP::QF::onCleanup();
    exit(-1);
}

} // extern "C"

//============================================================================
namespace BSP {

void init(void const * const arg) {
    Q_UNUSED_PAR(arg);

    QP::QF::consoleSetup();
    std::cout << "History state pattern\n"
           "QP/C++ version: " QP_VERSION_STR "\n"
           "Press 'o' to OPEN  the door\n"
           "Press 'c' to CLOSE the door\n"
           "Press 't' to start TOASTING\n"
           "Press 'b' to start BAKING\n"
           "Press 'f' to turn the oven OFF\n"
           "Press ESC to quit..." << std::endl;

    // dynamic events not used, no need to call QF::poolInit()
    // publish-subscribe not used, no call to QActive::psInit()
}
//............................................................................
void showMsg(char const * const msg) {
    std::cout << msg << std::endl;
}

} // namespace BSP

//============================================================================
namespace QP {

// QF callbacks...
void QF::onStartup() {
    SM_ToastOven->init(nullptr, 0U);

    for (;;) {
        std::cout << std::endl;
        std::uint8_t c = static_cast<std::uint8_t>(QF::consoleWaitForKey());
        std::cout << static_cast<char>((c >= ' ') ? c : 'X') << ':';

        QSignal sig = 0U;
        switch (c) {
            case 'o':  sig = OPEN_SIG;        break;
            case 'c':  sig = CLOSE_SIG;       break;
            case 't':  sig = TOAST_SIG;       break;
            case 'b':  sig = BAKE_SIG;        break;
            case 'f':  sig = OFF_SIG;         break;
            case 0x1B: sig = TERMINATE_SIG;   break;
            default: std::cout << "unrecognized" << std::endl; break;
        }

        // dispatch the event into the state machine
        QP::QEvt e {sig}; // automatic event (on the stack)
        SM_ToastOven->dispatch(&e, 0U); // synchronously dispatch
    }
}
//............................................................................
void QF::onCleanup() {
    std::cout << "Bye! Bye!" << std::endl;
    QF::consoleCleanup();
}
//............................................................................
void QF::onClockTick() {
}

} // namespace QP

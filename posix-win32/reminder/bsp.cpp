//============================================================================
// BSP for the "reminder" example
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

namespace {
//Q_DEFINE_THIS_FILE
}

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
    std::cout << "Orthogonal Component pattern\n"
           "QP/C++ version: " QP_VERSION_STR "\n"
           "Press ESC to quit..." << std::endl;

    // dynamic event allocation not used, no call to QF::poolInit()
    // publish-subscribe not used, no call to QActive::psInit()
}
//............................................................................
void showMsg(char const * const msg, std::uint8_t const num) {
    if (num == 0U) {
        std::cout << msg << std::endl;
    }
    else {
        std::cout << msg << static_cast<unsigned>(num) << std::endl;
    }
}

} // namespace BSP

//============================================================================
namespace QP {

// QF callbacks...
void QF::onStartup() {
    // start the active objects...
    static QP::QEvtPtr sensorQSto[10]; // event queue storage for Sensor AO
    AO_Sensor->start(
        1U,
        sensorQSto, Q_DIM(sensorQSto),
        nullptr, 0U);

    QF::setTickRate(BSP::TICKS_PER_SEC, 50U); // desired tick rate/ticker-prio
}
//............................................................................
void QF::onCleanup() {
    std::cout << "Bye! Bye!" << std::endl;
    QF::consoleCleanup();
}
//............................................................................
void QF::onClockTick() {
    QTimeEvt::TICK_X(0U, nullptr); // clock tick processing for rate 0

    int key = QF::consoleGetKey();
    if (key != 0) { // any key pressed?
        switch (key) {
            case '\033': { // ESC pressed?
                static QP::QEvt const terminateEvt {TERMINATE_SIG};
                AO_Sensor->POST(&terminateEvt, nullptr);
                break;
            }
        }
    }
}

} // namespace QP

//============================================================================
// BSP for the "reminder2" example
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
           "Press 'e' to echo the current value...\n"
           "Press ESC to quit..." << std::endl;

    // initialize event pool(s) for dynamic events...
    static QF_MPOOL_EL(ReminderEvt) smlPoolSto[20]; // storage for small pool
    QP::QF::poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

    // publish-subscribe not used, no call to QActive::psInit()
}
//............................................................................
void showMsg(char const * const msg, double x) {
    if (x == 0.0) {
        std::cout << msg << std::endl;
    }
    else {
        std::cout << msg << std::setprecision(16) << x << std::endl;
    }
}

} // namespace BSP

//============================================================================
namespace QP {

// QF callbacks...
void QF::onStartup() {
    // start the active objects...
    static QP::QEvtPtr cruncherQSto[10]; // event queue storage for Cruncher AO
    AO_Cruncher->start(
        1U,
        cruncherQSto, Q_DIM(cruncherQSto),
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
            case 'e': { // 'e' pressed -> post the ECHO event
                // NOTE:
                // Posting the ECHO event would be best handled with an
                // immutable event, as it is illustrated in the next case
                // of the TERMINATE event below (case '\033').
                //
                // However, for the sake of demonstrating the q_new_x<>
                // facility, the ECHO event is allocated dynamically, which
                // is allowed to fail because the ECHO event is not
                // considered critical.
#ifdef QEVT_PAR_INIT
                QEvt const * const echoEvt = QF::q_new_x<QEvt>(2U, ECHO_SIG, QEvt::DYNAMIC);
#else
                QEvt * const echoEvt = QF::q_new_x<QEvt>(2U, ECHO_SIG);
#endif
                if (echoEvt != nullptr) { // event allocated successfully?
                    AO_Cruncher->POST_X(echoEvt, 2U, nullptr);
                }
                break;
            }
            case '\033': { // ESC pressed?
                static QP::QEvt const terminateEvt {TERMINATE_SIG};
                AO_Cruncher->POST(&terminateEvt, nullptr);
                break;
            }
        }
    }
}

} // namespace QP

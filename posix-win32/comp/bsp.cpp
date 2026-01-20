//============================================================================
// BSP for the "comp" example
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
           "Press 'o' to turn the Alarm ON\n"
           "Press 'f' to turn the Alarm OFF\n"
           "Press '0'..'9' to set the Alarm time\n"
           "Press 'a' to set the Clock in 12-hour mode\n"
           "Press 'b' to set the Clock in 24-hour mode\n"
           "Press ESC to quit..." << std::endl;

    // initialize event pools...
    static QF_MPOOL_EL(TimeEvt) smlPoolSto[10]; // storage for small pool
    QP::QF::poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

    // publish-subscribe not used, no call to QActive::psInit()
}
//............................................................................
void showMsg(char const *str) {
    std::cout << str << std::endl;
}
//............................................................................
void showTime24H(char const *str, std::uint32_t time, std::uint32_t base) {
    std::cout << str
        << std::setw(2) << std::setfill('0') << (time / base) << ':'
        << std::setw(2) << std::setfill('0') << (time % base) << std::endl;
}
//............................................................................
void showTime12H(char const *str, std::uint32_t time, std::uint32_t base) {
    std::uint32_t h = time / base;

    std::cout << str
        << std::setw(2) << std::setfill('0')
        << (((h % 12U) != 0U) ? (h % 12U) : 12) << ':'
        << std::setw(2) << std::setfill('0') << (time % base)
        << (((h / 12U) != 0U) ? " PM" : " AM")
        <<  std::endl;
}

} // namespace BSP

//============================================================================
namespace QP {

// QF callbacks...
void QF::onStartup() {
    // start AOs...
    static QP::QEvtPtr alarmClockQSto[10]; // queue storage for AlarmClock
    APP_alarmClock->start(
        1U,
        alarmClockQSto, Q_DIM(alarmClockQSto),
        nullptr, 0U);

    QF::setTickRate(BSP::TICKS_PER_SEC, 50U); // desired tick rate/ticker-prio
}
//............................................................................
void QF::onCleanup() {
    std::cout << "Bye! Bye!" << std::endl;
    QF::consoleCleanup();
}
//............................................................................
void QF::onClockTick(void) {
    QTimeEvt::TICK_X(0U, nullptr); // clock tick processing for rate 0

    int key = QF::consoleGetKey();
    if (key != 0) { // any key pressed?
        switch (key) {
            case 'o': { // 'o': Alarm on event?
                APP_alarmClock->POST(QF::q_new<QEvt>(ALARM_ON_SIG), nullptr);
                break;
            }
            case 'f': { // 'f': Alarm off event?
                APP_alarmClock->POST(QF::q_new<QEvt>(ALARM_OFF_SIG), nullptr);
                break;
            }
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': {
                SetEvt * const e = QF::q_new<SetEvt>(ALARM_SET_SIG);
                e->digit = (uint8_t)(key - '0');
                APP_alarmClock->POST(e, nullptr);
                break;
            }
            case '0': {
                SetEvt * const e = QF::q_new<SetEvt>(ALARM_SET_SIG);
                e->digit = 0;
                APP_alarmClock->POST(e, nullptr);
                break;
            }
            case 'a': { // 'a': Clock 12H event?
                APP_alarmClock->POST(QF::q_new<QEvt>(CLOCK_12H_SIG), nullptr);
                break;
            }
            case 'b': { // 'b': Clock 24H event?
                APP_alarmClock->POST(QF::q_new<QEvt>(CLOCK_24H_SIG), nullptr);
                break;
            }
            case '\33': { // ESC pressed?
                APP_alarmClock->POST(QF::q_new<QEvt>(TERMINATE_SIG), nullptr);
                break;
            }
        }
    }
}

} // namespace QP

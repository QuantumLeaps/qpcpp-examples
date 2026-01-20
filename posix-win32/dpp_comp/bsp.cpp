//============================================================================
// BSP for the "DPP-comp" example
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
#include <cstdlib>          // for exit()

//============================================================================
namespace { // unnamed namespace for local stuff with internal linkage

Q_DEFINE_THIS_FILE  // file name for assertions

// Local objects -------------------------------------------------------------
static QP::QTicker l_Ticker0_inst(0U); // "ticker" AO for tick rate 0
QP::QTicker * const the_Ticker0 = &l_Ticker0_inst;

static std::uint32_t l_rnd; // random seed

#ifdef Q_SPY
    enum AppRecords { // application-specific trace records
        PHILO_STAT = QP::QS_USER,
        PAUSED_STAT,
    };

    // QSpy source IDs
    static QP::QSpyId const l_clock_tick = { QP::QS_ID_AP };
#endif // Q_SPY

} // unnamed namespace

//============================================================================
// Error handler

extern "C" {

Q_NORETURN Q_onError(char const * const module, int_t const id) {
    // NOTE: this implementation of the error handler is intended only
    // for debugging and MUST be changed for deployment of the application.
    Q_UNUSED_PAR(module);
    Q_UNUSED_PAR(id);
    QS_ASSERTION(module, id, 10000U); // report assertion to QS

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
    std::cout << "DPP-comp example\n"
           "QP/C++ version: " QP_VERSION_STR "\n"
            "Press 'p' to pause\n"
           "Press 's' to serve\n"
           "Press ESC to quit..." << std::endl;

    // initialize QS software tracing...
    if (!QS_INIT(arg)) {
        Q_ERROR();
    }

    // QS dictionaries...
    QS_OBJ_DICTIONARY(&l_clock_tick);
    QS_OBJ_DICTIONARY(the_Ticker0);

    QS_USR_DICTIONARY(PHILO_STAT);
    QS_USR_DICTIONARY(PAUSED_STAT);
    QS_ONLY(APP::produce_sig_dict());

    // setup QS filters...
    QS_GLB_FILTER(QP::QS_GRP_ALL);  // enable all QS trace records
    QS_GLB_FILTER(-QP::QS_QF_TICK); // exclude the tick record
    QS_LOC_FILTER(-(APP::N_PHILO + 4)); // exclude the "ticker" prio

    // initialize event pools for mutable events
    static QF_MPOOL_EL(APP::TableEvt) smlPoolSto[2*APP::N_PHILO];
    QP::QF::poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

    // initialize publish-subscribe
    static QP::QSubscrList subscrSto[APP::MAX_PUB_SIG];
    QP::QActive::psInit(subscrSto, Q_DIM(subscrSto));

    randomSeed(1234U); // seed the random number generator
}
//............................................................................
void terminate(std::int16_t result) {
    Q_UNUSED_PAR(result);
    QP::QF::stop();
}
//............................................................................
void displayPhilStat(std::uint8_t n, char const *stat) {
    std::cout << "Philosopher "
        << static_cast<unsigned>(n)
        << " is " << stat << std::endl;

    // application-specific record
    QS_BEGIN_ID(PHILO_STAT, APP::AO_Table->getPrio())
        QS_U8(1, n);  // Philosopher number
        QS_STR(stat); // Philosopher status
    QS_END()
}
//............................................................................
void displayPaused(std::uint8_t const paused) {
    std::cout << "Paused is "
        << (paused ? "ON" : "OFF") << std::endl;
}
//............................................................................
void randomSeed(std::uint32_t const seed) {
    l_rnd = seed;
}
//............................................................................
std::uint32_t random() { // a very cheap pseudo-random-number generator
    // "Super-Duper" Linear Congruential Generator (LCG)
    // LCG(2^32, 3*7*11*13*23, 0, seed)
    std::uint32_t const rnd = l_rnd * (3U*7U*11U*13U*23U);
    l_rnd = rnd; // set for the next time

    return rnd >> 8U;
}

} // namespace BSP

//============================================================================
namespace QP {

// QF callbacks...
void QF::onStartup() {
    // start AOs...

    the_Ticker0->start(
        APP::N_PHILO + 4U,    // QP priority
        nullptr, 0U,          // no queue
        nullptr, 0U);         // no stack storage

    static QP::QEvtPtr tableQueueSto[APP::N_PHILO];
    APP::AO_Table->start(
        APP::N_PHILO + 7U,    // QP prio. of the AO
        tableQueueSto,        // event queue storage
        Q_DIM(tableQueueSto), // queue length [events]
        nullptr, 0U);         // no stack storage
    setTickRate(BSP::TICKS_PER_SEC, 50U); // desired tick rate/prio
}
//............................................................................
void QF::onCleanup() {
    std::cout << "Bye! Bye!" << std::endl;
    consoleCleanup();
}
//............................................................................
void QF::onClockTick() {
    //QTimeEvt::TICK_X(0U, &l_clock_tick); // process time events at rate 0
    the_Ticker0->TRIG(&l_clock_tick); // trigger "ticker-0" to process time

    QS_RX_INPUT(); // handle the QS-RX input
    QS_OUTPUT();   // handle the QS output

    switch (consoleGetKey()) {
        case '\33': { // ESC pressed?
            BSP::terminate(0);
            break;
        }
        case 'p': {
            static QEvt const pauseEvt(APP::PAUSE_SIG);
            QActive::PUBLISH(&pauseEvt, &l_clock_tick);
            break;
        }
        case 's': {
            static QEvt const serveEvt(APP::SERVE_SIG);
            QActive::PUBLISH(&serveEvt, &l_clock_tick);
            break;
        }
        default: {
            break;
        }
    }
}

//============================================================================
#ifdef Q_SPY

//............................................................................
void QS::onCommand(std::uint8_t cmdId, std::uint32_t param1,
    std::uint32_t param2, std::uint32_t param3)
{
    Q_UNUSED_PAR(cmdId);
    Q_UNUSED_PAR(param1);
    Q_UNUSED_PAR(param2);
    Q_UNUSED_PAR(param3);
}

#endif // Q_SPY

} // namespace QP

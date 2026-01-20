//============================================================================
// DPP example for QXK
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
#include "test.hpp"

//............................................................................
int main() {
    static QP::QEvtPtr tableQueueSto[N_PHILO];
    static QP::QEvtPtr philoQueueSto[N_PHILO][N_PHILO];
    static QP::QEvtPtr testQueueSto[5];

    static QP::QSubscrList subscrSto[DPP::MAX_PUB_SIG];
    static QF_MPOOL_EL(DPP::TableEvt) smlPoolSto[2*N_PHILO]; // small pool

    // stack for the QXK extended thread
    static uint64_t testStackSto[64];

    QP::QF::init();  // initialize the framework
    DPP::BSP::init(); // initialize the BSP and start the AOs

    // initialize publish-subscribe...
    QP::QActive::psInit(subscrSto, Q_DIM(subscrSto));

    // initialize event pools...
    QP::QF::poolInit(smlPoolSto,
                     sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

    // start the active objects...
    for (uint8_t n = 0U; n < N_PHILO; ++n) {
        DPP::AO_Philo[n]->start(
            static_cast<uint_fast8_t>(n + 1), // QP priority
            philoQueueSto[n],          // event queue storage
            Q_DIM(philoQueueSto[n]),   // queue length [events]
            nullptr,    // no stack storage
            static_cast<uint_fast16_t>(0)); // stack size [bytes]
    }

    DPP::AO_Table->start(
            static_cast<uint_fast8_t>(N_PHILO + 2U), // QP priority
            tableQueueSto,           // event queue storage
            Q_DIM(tableQueueSto),    // queue length [events]
            nullptr,    // no stack storage
            static_cast<uint_fast16_t>(0)); // stack size [bytes]

    // start the extended thread for testing
    DPP::XT_Test->start(
            static_cast<uint_fast8_t>(N_PHILO + 3U), // QP priority
            testQueueSto,            // event queue storage
            Q_DIM(testQueueSto),     // queue length [events]
            testStackSto,            // stack storage
            sizeof(testStackSto));   // stack size [bytes]


    return QP::QF::run(); // run the QF application
}

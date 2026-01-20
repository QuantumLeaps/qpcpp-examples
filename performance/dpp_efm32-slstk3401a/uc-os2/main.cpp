//============================================================================
// DPP example, uC/OS-II kernel
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
#include "test.h"
#include "assert.h"

Q_DEFINE_THIS_FILE

//............................................................................
int main() {
    // stacks for all uC/OS-II threads (grouped together for ease of testing)
    static OS_STK philoStackSto[N_PHILO][128];
    static OS_STK tableStackSto[256];
    static OS_STK testStackSto[128];

    static QP::QEvtPtr tableQueueSto[N_PHILO];
    static QP::QEvtPtr philoQueueSto[N_PHILO][N_PHILO];

    static QP::QSubscrList subscrSto[DPP::MAX_PUB_SIG];
    static QF_MPOOL_EL(DPP::TableEvt) smlPoolSto[2*N_PHILO]; // small pool

    QP::QF::init();  // initialize the framework and the underlying RT kernel
    DPP::BSP::init(); // initialize the BSP and start the AOs

    // initialize publish-subscribe...
    QP::QActive::psInit(subscrSto, Q_DIM(subscrSto));

    // initialize event pools...
    QP::QF::poolInit(smlPoolSto,
                     sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

    // start the active objects...
    for (uint8_t n = 0U; n < N_PHILO; ++n) {
        // NOTE: provide uC/OS-II task attributes for the AO's task
        DPP::AO_Philo[n]->setAttr(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK, 0);
        DPP::AO_Philo[n]->start(
            n + 3U,                    // QP priority
            philoQueueSto[n],          // event queue storage
            Q_DIM(philoQueueSto[n]),   // queue length [events]
            philoStackSto[n],          // stack storage
            sizeof(philoStackSto[n])); // stack size [bytes]
    }

    // NOTE: leave QF priority (N_PHILO + 3U) for the mutex to
    // protect the random number generator shared among Philo AOs

    // NOTE: provide uC/OS-II task attributes for the AO's task
    DPP::AO_Table->setAttr(OS_TASK_OPT_STK_CLR | OS_TASK_OPT_STK_CHK, 0);
    DPP::AO_Table->start(
        N_PHILO + 4U,            // QP priority
        tableQueueSto,           // event queue storage
        Q_DIM(tableQueueSto),    // queue length [events]
        tableStackSto,           // stack storage
        sizeof(tableStackSto));  // stack size [bytes]

    // start a "naked" uC/OS-II task for testing...
    INT8U const err = OSTaskCreateExt(
        &test_thread, // the test thread function
        nullptr,    // the 'pdata' parameter
        &testStackSto[(sizeof(testStackSto)/sizeof(OS_STK)) - 1],
        static_cast<INT8U>(QF_MAX_ACTIVE - (N_PHILO + 5U)), // uC/OS-II prio
        static_cast<INT16U>(N_PHILO + 5U),       // the unique task id
        static_cast<OS_STK *>(&testStackSto[0]), // pbos
        static_cast<INT32U>(sizeof(testStackSto)/sizeof(OS_STK)),
        nullptr,                  // pext
        static_cast<INT16U>(OS_TASK_OPT_STK_CLR)); // task attributes
    Q_ASSERT(err == OS_ERR_NONE);

    return QP::QF::run(); // run the QF application
}

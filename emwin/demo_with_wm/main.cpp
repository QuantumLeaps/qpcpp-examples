//============================================================================
// Product: main task for emWin/uC/GUI, Win32 simulation
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

// GUI includes
Q_DEFINE_THIS_FILE

//............................................................................
extern "C" {

void MainTask(void) {
    QF::init(); // initialize the framework and the underlying RT kernel
    BSP_init(); // initialize the BSP and start the AOs

    // initialize event pools...
    static union SmallEvents {
        void* min_size;
        TableEvt te;
        MouseEvt me;
        // other event types to go into this pool
    } smlPoolSto[2 * N_PHILO]; // storage for the small event pool
    QF::poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

    // initialize publish-subscribe
    static QSubscrList l_subscrSto[MAX_PUB_SIG];
    QActive::psInit(l_subscrSto, Q_DIM(l_subscrSto));

    // start the active objects...
    static QEvtPtr philoQueueSto[N_PHILO][10];
    for (uint8_t n = 0U; n < N_PHILO; ++n) {
        AO_Philo[n]->start(n + 1U,
            philoQueueSto[n], Q_DIM(philoQueueSto[n]),
            nullptr, 1024, nullptr);
    }

    static QEvtPtr tableQueueSto[N_PHILO];
    AO_Table->start(N_PHILO + 1U,
        tableQueueSto, Q_DIM(tableQueueSto),
        nullptr, 1024, nullptr);

    QF::run(); // run the QF application
}

} // extern "C"

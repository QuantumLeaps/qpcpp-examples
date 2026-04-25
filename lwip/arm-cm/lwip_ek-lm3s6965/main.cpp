//============================================================================
// Product: QP-lwIP demonstration
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

//............................................................................
int main(void) {
    QF::init();  // initialize the framework and the underlying RT kernel
    BSP_init(nullptr);  // initialize the BSP and start the AOs

    // initialize event pools...
    static QF_MPOOL_EL(TableEvt) smlPoolSto[20]; // storage for small pool
    QF::poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));
    static QF_MPOOL_EL(TextEvt)  medPoolSto[4];  // storage for med.  pool
    QF::poolInit(medPoolSto, sizeof(medPoolSto), sizeof(medPoolSto[0]));

    // initialize publish-subscribe...
    static QSubscrList subscrSto[MAX_PUB_SIG];
    QActive::psInit(subscrSto, Q_DIM(subscrSto));

    // start the active objects...
    static QEvtPtr lwIPMgrQueueSto[10];
    AO_LwIPMgr->start(1U,
                    lwIPMgrQueueSto, Q_DIM(lwIPMgrQueueSto),
                    nullptr, 0U);

    static QEvtPtr philoQueueSto[N_PHILO][N_PHILO];
    for (uint8_t n = 0U; n < N_PHILO; ++n) {
        AO_Philo[n]->start(n + 2U,
                           philoQueueSto[n], Q_DIM(philoQueueSto[n]),
                           nullptr, 0U);
    }
    static QEvtPtr tableQueueSto[N_PHILO + 5];
    AO_Table->start(N_PHILO + 2U,
                    tableQueueSto, Q_DIM(tableQueueSto),
                    nullptr, 0U);

    return QF::run(); // run the application
}


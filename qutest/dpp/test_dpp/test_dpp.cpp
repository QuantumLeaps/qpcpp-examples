//============================================================================
// Product: QUTEST fixture for the DPP components
// Last updated for version 8.0.0
// Last updated on  2024-07-31
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
#include "dpp.hpp"

//#include "safe_std.h" // portable "safe" <stdio.h>/<string.h> facilities

//============================================================================
int main() {
    QP::QF::init(); // initialize the framework and the underlying RT kernel
    BSP::init();    // initialize the BSP

    // pause execution of the test and wait for the test script to continue
    QS_TEST_PAUSE();

    // initialize event pools...
    static QF_MPOOL_EL(APP::TableEvt) smlPoolSto[2*APP::N_PHILO]; // small pool
    QP::QF::poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

    // initialize publish-subscribe...
    static QP::QSubscrList subscrSto[APP::MAX_PUB_SIG];
    QP::QActive::psInit(subscrSto, Q_DIM(subscrSto));

    // start the active objects...
    static QP::QEvtPtr philoQueueSto[APP::N_PHILO][10];
    for (std::uint8_t n = 0U; n < APP::N_PHILO; ++n) {
        APP::AO_Philo[n]->start(   // AO to start
            n + 1U,                // QF-priority
            philoQueueSto[n],      // event queue storage
            Q_DIM(philoQueueSto[n]), // queue length [events]
            nullptr,               // stack storage (not used)
            0U);                   // size of the stack [bytes]
    }

    static QP::QEvtPtr tableQueueSto[APP::N_PHILO];
    APP::AO_Table->start(          // AO to start
        APP::N_PHILO + 1U,         // QF-priority
        tableQueueSto,             // event queue storage
        Q_DIM(tableQueueSto),      // queue length [events]
        nullptr,                   // stack storage (not used)
        0U);                       // size of the stack [bytes]

    return QP::QF::run(); // run the QF application
}

//============================================================================
namespace QP {

//............................................................................
void QS::onTestSetup(void) {
}
//............................................................................
void QS::onTestTeardown(void) {
}

//............................................................................
// callback function to execute user commands
void QS::onCommand(std::uint8_t cmdId, std::uint32_t param1,
                   std::uint32_t param2, std::uint32_t param3)
{
    Q_UNUSED_PAR(cmdId);
    Q_UNUSED_PAR(param1);
    Q_UNUSED_PAR(param2);
    Q_UNUSED_PAR(param3);

    switch (cmdId) {
       case 0U: {
           QEvt const e_pause(APP::PAUSE_SIG);
           APP::AO_Table->dispatch(&e_pause,
                              static_cast<std::uint_fast8_t>(param1));
           break;
       }
       case 1U: {
           QEvt const e_serve(APP::SERVE_SIG);
           APP::AO_Table->dispatch(&e_serve,
                              static_cast<std::uint_fast8_t>(param1));
           break;
       }
       default:
           break;
    }
}

//============================================================================
// callback function to "massage" the event, if necessary
void QS::onTestEvt(QEvt *e) {
    (void)e;
#ifdef Q_HOST  // is this test compiled for a desktop Host computer?
#else // embedded Target
#endif // embedded Target
}
//............................................................................
// callback function to output the posted QP events (not used here)
void QS::onTestPost(void const *sender, QActive *recipient,
                    QEvt const *e, bool status)
{
    Q_UNUSED_PAR(sender);
    Q_UNUSED_PAR(status);
    switch (e->sig) {
        case APP::EAT_SIG:
        case APP::DONE_SIG:
        case APP::HUNGRY_SIG:
            QS_BEGIN_ID(QUTEST_ON_POST, 0U) // application-specific record
                QS_SIG(e->sig, recipient);
                QS_U8(0, Q_EVT_CAST(APP::TableEvt)->philoId);
            QS_END()
            break;
        default:
            break;
    }
}

} // namespace QP

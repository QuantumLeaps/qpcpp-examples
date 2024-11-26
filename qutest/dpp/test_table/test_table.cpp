//============================================================================
// Purpose: Fixture for QUTEST
//
// Copyright (C) 2005 Quantum Leaps, LLC. All rights reserved.
//
//                    Q u a n t u m  L e a P s
//                    ------------------------
//                    Modern Embedded Software
//
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-QL-commercial
//
// The QP/C software is dual-licensed under the terms of the open-source GNU
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
#include "qpcpp.hpp"
#include "bsp.hpp"
#include "dpp.hpp"

namespace { // unnamed namespace

Q_DEFINE_THIS_FILE

// instantiate dummy collaborator AOs...
static QP::QActiveDummy Philo_dummy[APP::N_PHILO];

} // unnamed namespace

QP::QActive * const APP::AO_Philo[APP::N_PHILO] = {
    &Philo_dummy[0],
    &Philo_dummy[1],
    &Philo_dummy[2],
    &Philo_dummy[3],
    &Philo_dummy[4]
};

//============================================================================
#ifdef Q_HOST
int main(int argc, char *argv[])
#else
int main(void)
#endif
{
    QP::QF::init();  // initialize the framework

    // initialize the QS software tracing
#ifdef Q_HOST
    if (!QS_INIT((argc > 1) ? argv[1] : nullptr)) {
        Q_ERROR();
    }
#else
    if (!QS_INIT(nullptr)) {
        Q_ERROR();
    }
#endif

    BSP::init();    // initialize the BSP

    for (std::uint8_t n = 0U; n < APP::N_PHILO; ++n) {
       QS_OBJ_ARR_DICTIONARY(&Philo_dummy[n], n);
    }

    // pause execution of the test and wait for the test script to continue
    QS_TEST_PAUSE();

    // initialize publish-subscribe...
    static QP::QSubscrList subscrSto[APP::MAX_PUB_SIG];
    QP::QActive::psInit(subscrSto, Q_DIM(subscrSto));

    // initialize event pools...
    static QF_MPOOL_EL(APP::TableEvt) smlPoolSto[2*APP::N_PHILO];
    QP::QF::poolInit(smlPoolSto,
                     sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

    // start and setup dummy AOs...
    // NOTE: You need to start dummy AOs, if you wish to subscribe
    //       them to events.
    for (std::uint8_t n = 0U; n < APP::N_PHILO; ++n) {
        Philo_dummy[n].start(
            n + 1U,        // QP priority
            nullptr, 0U,
            nullptr, 0U);
        Philo_dummy[n].subscribe(APP::EAT_SIG);
    }

    // start the active object under test (AOUT)...
    static QP::QEvtPtr tableQueueSto[APP::N_PHILO];
    APP::AO_Table->start(
        APP::N_PHILO + 1U, // QP priority
        tableQueueSto, Q_DIM(tableQueueSto),
        nullptr, 0U);

    return QP::QF::run(); // run the QF application
}

//============================================================================
namespace QP {

//............................................................................
void QS::onTestSetup() {
}
//............................................................................
void QS::onTestTeardown() {
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
    Q_UNUSED_PAR(e);
#ifdef Q_HOST  // is this test compiled for a desktop Host computer?
#else // this test is compiled for an embedded Target system
#endif
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

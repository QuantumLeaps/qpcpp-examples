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
#include "my_app.hpp" // My Application

Q_DEFINE_THIS_FILE

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

    // object dictionaries...
    QS_OBJ_DICTIONARY(AO_MyAO);

    // pause execution of the test and wait for the test script to continue
    QS_TEST_PAUSE();

    // initialize publish-subscribe..
    static QSubscrList subscrSto[MAX_PUB_SIG];
    QActive::psInit(subscrSto, Q_DIM(subscrSto));

    // initialize event pools...
    static QF_MPOOL_EL(MyEvt3) smlPoolSto[10];
    QF::poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

    static QEvtPtr myAoQueueSto[10];
    AO_MyAO->start(1U,                 // QP priority of the AO
                  myAoQueueSto,         // event queue storage
                  Q_DIM(myAoQueueSto),  // queue length [events]
                  nullptr, 0U);         // stack storage and size (not used)

    return QF::run(); // run the QF application
}

//----------------------------------------------------------------------------
enum AppSpecRecords {
    MY_EVT0 = QS_USER,
    MY_EVT1,
    MY_EVT2,
    MY_EVT3
};

//----------------------------------------------------------------------------
void QS::onTestSetup(void) {
    QS_USR_DICTIONARY(MY_EVT0);
    QS_USR_DICTIONARY(MY_EVT1);
    QS_USR_DICTIONARY(MY_EVT2);
    QS_USR_DICTIONARY(MY_EVT3);
}
//............................................................................
void QS::onTestTeardown(void) {
}

//............................................................................
void QS::onCommand(std::uint8_t cmdId,
                   std::uint32_t param1,
                   std::uint32_t param2,
                   std::uint32_t param3)
{
    (void)param1; // unused parameter
    (void)param2; // unused parameter
    (void)param3; // unused parameter

    switch (cmdId) {
        case 0: {
            break;
        }
        default:
            break;
    }
}
//............................................................................
//! callback function to "massage" the injected QP events (not used here)
void QS::onTestEvt(QEvt *e) {

    switch (e->sig) {
        case MY_EVT0_SIG:
            QS_BEGIN_ID(MY_EVT0, 0U) // application-specific record
            QS_END()
            break;
        case MY_EVT1_SIG:
            QS_BEGIN_ID(MY_EVT1, 0U) // application-specific record
                QS_U32(0, Q_EVT_CAST(MyEvt1)->u32);
            QS_END()
            break;
        case MY_EVT2_SIG:
            QS_BEGIN_ID(MY_EVT2, 0U) // application-specific record
                QS_U32(0, Q_EVT_CAST(MyEvt2)->u32);
                QS_U32(0, Q_EVT_CAST(MyEvt2)->u16);
            QS_END()
            break;
        case MY_EVT3_SIG:
            QS_BEGIN_ID(MY_EVT3, 0U) // application-specific record
                QS_U32(0, Q_EVT_CAST(MyEvt3)->u32);
                QS_U32(0, Q_EVT_CAST(MyEvt3)->u16);
                QS_U32(0, Q_EVT_CAST(MyEvt3)->u8);
            QS_END()
            break;
    }

#ifdef Q_HOST  // is this test compiled for a desktop Host computer?
#else // embedded Target
#endif // embedded Target

}
//............................................................................
// callback function to output the posted QP events (not used here)
void QS::onTestPost(void const *sender, QActive *recipient,
                    QEvt const *e, bool status)
{
    (void)sender;
    (void)recipient;
    (void)e;
    (void)status;
}


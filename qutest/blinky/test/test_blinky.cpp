//============================================================================
// Fixture for QUTEST
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

using namespace APP;

Q_DEFINE_THIS_FILE

//============================================================================
#ifdef Q_HOST
int main(int argc, char *argv[])
#else
int main()
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

    BSP::init(nullptr); // initialize the BSP and start the AOs

    // dictionaries...
    QS_SIG_DICTIONARY(TIMEOUT_SIG, nullptr);

    // pause execution of the test and wait for the test script to continue
    QS_TEST_PAUSE();

    // publish-subscribe not used, no call to QF::psInit()

    // initialize event pools...
    static QF_MPOOL_EL(QP::QEvt) smlPoolSto[10]; // storage for small pool
    QP::QF::poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

    // start the active objects...
    static QP::QEvtPtr blinkyQSto[10]; // event queue storage for Blinky
    AO_Blinky->start(2U, // QF-priority/preemption-threshold
        blinkyQSto, Q_DIM(blinkyQSto),
        nullptr, 0U, nullptr);

    return QP::QF::run();
}

//----------------------------------------------------------------------------
namespace QP {
void QS::onTestSetup(void) {
}
//............................................................................
void QS::onTestTeardown(void) {
}
//............................................................................
void QS::onCommand(std::uint8_t cmdId,
    std::uint32_t param1, std::uint32_t param2, std::uint32_t param3)
{
    Q_UNUSED_PAR(cmdId);
    Q_UNUSED_PAR(param1);
    Q_UNUSED_PAR(param2);
    Q_UNUSED_PAR(param3);
}

//............................................................................
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
    Q_UNUSED_PAR(recipient);
    Q_UNUSED_PAR(e);
    Q_UNUSED_PAR(status);
}

} // namespace QP
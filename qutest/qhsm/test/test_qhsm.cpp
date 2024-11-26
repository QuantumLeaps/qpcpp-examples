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
#include "cut_qhsm.hpp"

Q_DEFINE_THIS_FILE

//============================================================================
namespace APP {

enum {
    BSP_DISPLAY = QP::QS_USER0,
};
enum {
    CMD         = QP::QS_USER1,
};

void BSP_display(char const *msg) {
    QS_BEGIN_ID(BSP_DISPLAY, 0U) // app-specific record
        QS_STR(msg);
    QS_END()
}
//............................................................................
void BSP_terminate(int16_t const result) {
    Q_UNUSED_PAR(result);
}

} // namespace APP

//============================================================================
namespace QP {

//............................................................................
void QS::onTestSetup(void) {
}
//............................................................................
void QS::onTestTeardown(void) {
}
//............................................................................
void QS::onCommand(std::uint8_t cmdId, std::uint32_t param1,
                   std::uint32_t param2, std::uint32_t param3)
{
    Q_UNUSED_PAR(param1);
    Q_UNUSED_PAR(param2);
    Q_UNUSED_PAR(param3);

    //PRINTF_S("<TARGET> Command id=%d param=%d\n", (int)cmdId, (int)param);
    switch (cmdId) {
        case 0U: {
            QP::QStateHandler state = APP::the_sm->getStateHandler();
            QS_BEGIN_ID(APP::CMD, 0U) // app-specific record
                QS_FUN(state);
            QS_END()
            break;
        }
        case 1U: {
            bool ret = APP::TstSM_isIn(param1);
            QS_BEGIN_ID(APP::CMD, 0U) // app-specific record
                QS_U8(0U, ret ? 1 : 0);
                QS_U8(0U, (uint8_t)param1);
            QS_END()
            break;
       }
       default:
           break;
    }
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

//============================================================================
using namespace APP;

//............................................................................
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

    // initialize event pools...
    static QF_MPOOL_EL(QP::QEvt) smlPoolSto[10]; // small pool
    QP::QF::poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

    // dictionaries...
    QS_OBJ_DICTIONARY(the_sm);
    QS_USR_DICTIONARY(BSP_DISPLAY);
    QS_USR_DICTIONARY(CMD);

    return QP::QF::run();
}


//$file${.::reminder2.cpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: reminder2.qm
// File:  ${.::reminder2.cpp}
//
// This code has been generated by QM 7.0.1 <www.state-machine.com/qm>.
// DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
//
// Copyright (c) 2005 Quantum Leaps, LLC. All rights reserved.
//
//                 ____________________________________
//                /                                   /
//               /    GGGGGGG    PPPPPPPP   LL       /
//              /   GG     GG   PP     PP  LL       /
//             /   GG          PP     PP  LL       /
//            /   GG   GGGGG  PPPPPPPP   LL       /
//           /   GG      GG  PP         LL       /
//          /     GGGGGGG   PP         LLLLLLL  /
//         /___________________________________/
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// This generated code is open-source software licensed under the GNU
// General Public License (GPL) as published by the Free Software Foundation
// (see <https://www.gnu.org/licenses>).
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
//
//$endhead${.::reminder2.cpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include "qpcpp.hpp"
#include "bsp.hpp"

#include "safe_std.h"   // portable "safe" <stdio.h>/<string.h> facilities

Q_DEFINE_THIS_FILE

//............................................................................
enum ReminderSignals {
    CRUNCH_SIG = QP::Q_USER_SIG, // the invented reminder signal
    ECHO_SIG,     // check the responsiveness of the system
    TERMINATE_SIG // terminate the application
};

//............................................................................
//$declare${Events::ReminderEvt} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

//${Events::ReminderEvt} .....................................................
class ReminderEvt : public QP::QEvt {
public:
    std::uint32_t iter;

public:
    void init(std::uint32_t i) {
        iter = i;
    }
}; // class ReminderEvt
//$enddecl${Events::ReminderEvt} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

// Active object class -------------------------------------------------------
//$declare${Components::Cruncher} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

//${Components::Cruncher} ....................................................
class Cruncher : public QP::QActive {
private:

    // internal variable
    double m_sum;

public:
    explicit Cruncher() noexcept
      : QActive(Q_STATE_CAST(&initial))
    {}

protected:
    Q_STATE_DECL(initial);
    Q_STATE_DECL(processing);
    Q_STATE_DECL(final);
}; // class Cruncher
//$enddecl${Components::Cruncher} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// Check for the minimum required QP version
#if (QP_VERSION < 730U) || (QP_VERSION != ((QP_RELEASE^4294967295U)%0x2710U))
#error qpcpp version 7.3.0 or higher required
#endif
//$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$define${Components::Cruncher} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

//${Components::Cruncher} ....................................................

//${Components::Cruncher::SM} ................................................
Q_STATE_DEF(Cruncher, initial) {
    //${Components::Cruncher::SM::initial}
    Q_UNUSED_PAR(e);

    QS_FUN_DICTIONARY(&Cruncher::processing);
    QS_FUN_DICTIONARY(&Cruncher::final);

    return tran(&processing);
}

//${Components::Cruncher::SM::processing} ....................................
Q_STATE_DEF(Cruncher, processing) {
    QP::QState status_;
    switch (e->sig) {
        //${Components::Cruncher::SM::processing}
        case Q_ENTRY_SIG: {
            #ifdef QEVT_PAR_INIT
            ReminderEvt const *reminder = QP::QF::q_new<ReminderEvt>(CRUNCH_SIG, 0U);
            #else
            ReminderEvt *reminder = QP::QF::q_new<ReminderEvt>(CRUNCH_SIG);
            reminder->iter = 0U;
            #endif

            POST(reminder, this);
            m_sum = 0.0;
            status_ = Q_RET_HANDLED;
            break;
        }
        //${Components::Cruncher::SM::processing::CRUNCH}
        case CRUNCH_SIG: {
            std::uint32_t i = Q_EVT_CAST(ReminderEvt)->iter;
            std::uint32_t n = i;
            i += 100U;
            for (; n < i; ++n) {
                if ((n & 1) == 0) {
                    m_sum += 1.0/(2U*n + 1U);
                }
                else {
                    m_sum -= 1.0/(2U*n + 1U);
                }
            }
            //${Components::Cruncher::SM::processing::CRUNCH::[i<0x07000000U]}
            if (i < 0x07000000U) {
                #ifdef QEVT_PAR_INIT
                ReminderEvt const *reminder = QP::QF::q_new<ReminderEvt>(CRUNCH_SIG, i);
                #else
                ReminderEvt *reminder = QP::QF::q_new<ReminderEvt>(CRUNCH_SIG);
                reminder->iter = i;
                #endif

                POST(reminder, this);
                status_ = Q_RET_HANDLED;
            }
            //${Components::Cruncher::SM::processing::CRUNCH::[else]}
            else {
                PRINTF_S("pi=%16.14f\n", 4.0*m_sum);
                status_ = tran(&processing);
            }
            break;
        }
        //${Components::Cruncher::SM::processing::ECHO}
        case ECHO_SIG: {
            PRINTF_S("Echo! pi=%16.14f\n", 4.0*m_sum);
            status_ = Q_RET_HANDLED;
            break;
        }
        //${Components::Cruncher::SM::processing::TERMINATE}
        case TERMINATE_SIG: {
            status_ = tran(&final);
            break;
        }
        default: {
            status_ = super(&top);
            break;
        }
    }
    return status_;
}

//${Components::Cruncher::SM::final} .........................................
Q_STATE_DEF(Cruncher, final) {
    QP::QState status_;
    switch (e->sig) {
        //${Components::Cruncher::SM::final}
        case Q_ENTRY_SIG: {
            PRINTF_S("%s\n", "final-ENTRY;");
                     QP::QF::stop(); // terminate the application
            status_ = Q_RET_HANDLED;
            break;
        }
        default: {
            status_ = super(&top);
            break;
        }
    }
    return status_;
}
//$enddef${Components::Cruncher} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

// test harness ==============================================================

// Local-scope objects -------------------------------------------------------
static Cruncher l_cruncher;     // the Cruncher active object

//............................................................................
int main(int argc, char *argv[]) {
    PRINTF_S("Reminder state pattern\nQP version: %s\n"
           "Press 'e' to echo the current value...\n"
           "Press ESC to quit...\n",
           QP_VERSION_STR);

    BSP::init(argc, argv); // initialize the BSP
    QP::QF::init(); // initialize the framework and the underlying RT kernel

    // pools for dynamic (mutable) events
    static QF_MPOOL_EL(ReminderEvt) smlPoolSto[20]; // storage for small pool
    QP::QF::poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

    // publish-subscribe not used, no call to QActive::psInit()

    // instantiate and start the active objects...
    QP::QEvtPtr cruncherQSto[10]; // Event queue storage for Cruncher AO
    l_cruncher.start(1U,
                     cruncherQSto, Q_DIM(cruncherQSto),
                     nullptr, 0U, nullptr);

    return QP::QF::run(); // run the QF application
}
//............................................................................
void BSP::onKeyboardInput(uint8_t key) {
    switch (key) {
        case 'e': { // 'e' pressed -> post the ECHO event
            // NOTE:
            // Posting the ECHO event would be best handled with an
            // immutable event, as it is illustrated in the next case
            // of the TERMINATE event below (case '\033').
            //
            // However, for the sake of demonstrating the q_new_x<>()
            // facility, the ECHO event is allocated dynamically, which
            // is allowed to fail because the ECHO event is not
            // considered critical.
        #ifdef QEVT_PAR_INIT
            QP::QEvt const *echoEvt =
                QP::QF::q_new_x<QP::QEvt>(2U, ECHO_SIG);
        #else
            QP::QEvt *echoEvt = QP::QF::q_new_x<QP::QEvt>(2U, ECHO_SIG);
        #endif
            if (echoEvt != nullptr) { // event allocated successfully?
                l_cruncher.POST_X(echoEvt, 2U, nullptr); // can fail
            }
            break;
        }
        case '\033': { // ESC pressed?
            // NOTE: this immutable event is statically pre-allocated.
            // It can be posted/published as any other event.
            static QP::QEvt const terminateEvt(TERMINATE_SIG);
            l_cruncher.POST(&terminateEvt, nullptr);
            break;
        }
    }
}


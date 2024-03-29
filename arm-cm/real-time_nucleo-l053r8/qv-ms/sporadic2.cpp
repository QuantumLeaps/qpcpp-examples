//$file${.::sporadic2.cpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: real-time-ms.qm
// File:  ${.::sporadic2.cpp}
//
// This code has been generated by QM 5.3.0 <www.state-machine.com/qm>.
// DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// This generated code is open source software: you can redistribute it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation.
//
// This code is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// NOTE:
// Alternatively, this generated code may be distributed under the terms
// of Quantum Leaps commercial licenses, which expressly supersede the GNU
// General Public License and are specifically designed for licensees
// interested in retaining the proprietary status of their code.
//
// Contact information:
// <www.state-machine.com/licensing>
// <info@state-machine.com>
//
//$endhead${.::sporadic2.cpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include "qpcpp.hpp"           // QP/C++ real-time embedded framework
#include "bsp.hpp"             // Board Support Package
#include "app.hpp"             // Application interface

namespace {
Q_DEFINE_THIS_MODULE("sporadic2")
}

//$declare${AOs::Sporadic2} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${AOs::Sporadic2} ..........................................................
class Sporadic2 : public QP::QMActive {
public:
    std::uint16_t m_per_rtc;
    std::uint16_t m_total;
    std::uint16_t m_done;
    static Sporadic2 inst;

public:
    Sporadic2();

protected:
    QM_STATE_DECL( initial);
    QM_STATE_DECL( active);
    QM_STATE_DECL( busy);
    QM_ACTION_DECL(busy_e);
}; // class Sporadic2

} // namespace APP
//$enddecl${AOs::Sporadic2} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// Check for the minimum required QP version
#if (QP_VERSION < 730U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpcpp version 7.3.0 or higher required
#endif
//$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//$define${Shared::AO_Sporadic2} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${Shared::AO_Sporadic2} ....................................................
QP::QActive * const AO_Sporadic2 {&Sporadic2::inst};

} // namespace APP
//$enddef${Shared::AO_Sporadic2} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$define${AOs::Sporadic2} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${AOs::Sporadic2} ..........................................................
Sporadic2 Sporadic2::inst;

//${AOs::Sporadic2::Sporadic2} ...............................................
Sporadic2::Sporadic2()
  : QMActive(Q_STATE_CAST(&Sporadic2::initial))
{}

//${AOs::Sporadic2::SM} ......................................................
QM_STATE_DEF(Sporadic2, initial) {
    //${AOs::Sporadic2::SM::initial}
    Q_UNUSED_PAR(e);

    QS_FUN_DICTIONARY(&Sporadic2::active);
    QS_FUN_DICTIONARY(&Sporadic2::busy);
    static QP::QMTranActTable const tatbl_ = { // tran-action table
        &active_s, // target state
        {
            Q_ACTION_NULL // zero terminator
        }
    };
    return qm_tran_init(&tatbl_);
}

//${AOs::Sporadic2::SM::active} ..............................................
QP::QMState const Sporadic2::active_s = {
    QM_STATE_NULL, // superstate (top)
    &Sporadic2::active,
    Q_ACTION_NULL, // no entry action
    Q_ACTION_NULL, // no exit action
    Q_ACTION_NULL  // no initial tran.
};
//${AOs::Sporadic2::SM::active}
QM_STATE_DEF(Sporadic2, active) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Sporadic2::SM::active::SPORADIC_A}
        case SPORADIC_A_SIG: {
            BSP::d5on();
            AO_Sporadic3->POST(e, this); // Sporadic2 --> Sporadic3
            BSP::d5off();
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Sporadic2::SM::active::SPORADIC_B}
        case SPORADIC_B_SIG: {
            BSP::d5on();
            AO_Periodic1->POST(BSP::getEvtPeriodic1(1U), this);

            m_per_rtc = Q_EVT_CAST(SporadicSpecEvt)->rtc_toggles;
            m_total = Q_EVT_CAST(SporadicSpecEvt)->toggles;
            m_done = 0U;

            #ifdef USE_SCHED_DISABLE
            QP::QV::schedDisable(2U); // <== disable scheduler up to given prio.
            #endif

            BSP::d5off();

            static struct {
                QP::QMState const *target;
                QP::QActionHandler act[2];
            } const tatbl_ = { // tran-action table
                &busy_s, // target state
                {
                    &busy_e, // entry
                    Q_ACTION_NULL // zero terminator
                }
            };
            status_ = qm_tran(&tatbl_);
            break;
        }
        default: {
            status_ = Q_RET_SUPER;
            break;
        }
    }
    return status_;
}

//${AOs::Sporadic2::SM::active::busy} ........................................
QP::QMState const Sporadic2::busy_s = {
    &Sporadic2::active_s, // superstate
    &Sporadic2::busy,
    &Sporadic2::busy_e,
    Q_ACTION_NULL, // no exit action
    Q_ACTION_NULL  // no initial tran.
};
//${AOs::Sporadic2::SM::active::busy}
QM_ACTION_DEF(Sporadic2, busy_e) {
    BSP::d5on();
    static QP::QEvt const reminderEvt(REMINDER_SIG);
    postLIFO(&reminderEvt);
    BSP::d5off();
    return qm_entry(&busy_s);
}
//${AOs::Sporadic2::SM::active::busy}
QM_STATE_DEF(Sporadic2, busy) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Sporadic2::SM::active::busy::REMINDER}
        case REMINDER_SIG: {
            std::uint16_t toggles = m_total - m_done;
            if (toggles > m_per_rtc) {
                toggles = m_per_rtc;
            }
            m_done += toggles;
            for (; toggles > 0U; --toggles) {
                BSP::d5on();
                BSP::d5off();
            }

            #ifdef USE_SCHED_DISABLE
            QP::QV::schedDisable(2U); // <== disable scheduler up to given prio.
            #endif
            //${AOs::Sporadic2::SM::active::busy::REMINDER::[m_done<m_total]}
            if (m_done < m_total) {

                static struct {
                    QP::QMState const *target;
                    QP::QActionHandler act[2];
                } const tatbl_ = { // tran-action table
                    &busy_s, // target state
                    {
                        &busy_e, // entry
                        Q_ACTION_NULL // zero terminator
                    }
                };
                status_ = qm_tran(&tatbl_);
            }
            //${AOs::Sporadic2::SM::active::busy::REMINDER::[else]}
            else {
                static QP::QMTranActTable const tatbl_ = { // tran-action table
                    &active_s, // target state
                    {
                        Q_ACTION_NULL // zero terminator
                    }
                };
                status_ = qm_tran(&tatbl_);
            }
            break;
        }
        default: {
            status_ = Q_RET_SUPER;
            break;
        }
    }
    return status_;
}

} // namespace APP
//$enddef${AOs::Sporadic2} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

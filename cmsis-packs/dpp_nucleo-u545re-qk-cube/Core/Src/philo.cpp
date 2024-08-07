//$file${Src::philo.cpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: dpp.qm
// File:  ${Src::philo.cpp}
//
// This code has been generated by QM 6.1.2 <www.state-machine.com/qm>.
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
//$endhead${Src::philo.cpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include "qpcpp.hpp"             // QP/C++ real-time embedded framework
#include "dpp.hpp"               // DPP Application interface
#include "bsp.hpp"               // Board Support Package

//----------------------------------------------------------------------------
namespace { // unnamed namespace for local definitions with internal linkage

Q_DEFINE_THIS_FILE

// helper function to provide a randomized think time for Philos
static inline QP::QTimeEvtCtr think_time() {
    return static_cast<QP::QTimeEvtCtr>((BSP::random() % BSP::TICKS_PER_SEC)
                                        + (BSP::TICKS_PER_SEC/2U));
}

// helper function to provide a randomized eat time for Philos
static inline QP::QTimeEvtCtr eat_time() {
    return static_cast<QP::QTimeEvtCtr>((BSP::random() % BSP::TICKS_PER_SEC)
                                        + BSP::TICKS_PER_SEC);
}

} // unnamed namespace
//----------------------------------------------------------------------------

//$declare${AOs::Philo} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${AOs::Philo} ..............................................................
class Philo : public QP::QActive {
private:
    QP::QTimeEvt m_timeEvt;
    std::uint8_t m_id;

public:
    static Philo inst[N_PHILO];

public:
    Philo();

protected:
    Q_STATE_DECL(initial);
    Q_STATE_DECL(thinking);
    Q_STATE_DECL(hungry);
    Q_STATE_DECL(eating);
}; // class Philo

} // namespace APP
//$enddecl${AOs::Philo} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// Check for the minimum required QP version
#if (QP_VERSION < 730U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpcpp version 7.3.0 or higher required
#endif
//$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$define${Shared::AO_Philo[N_PHILO]} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${Shared::AO_Philo[N_PHILO]} ...............................................
QP::QActive * const AO_Philo[N_PHILO] = {
    &Philo::inst[0],
    &Philo::inst[1],
    &Philo::inst[2],
    &Philo::inst[3],
    &Philo::inst[4]
};

} // namespace APP
//$enddef${Shared::AO_Philo[N_PHILO]} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$define${AOs::Philo} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${AOs::Philo} ..............................................................
Philo Philo::inst[N_PHILO];

//${AOs::Philo::Philo} .......................................................
Philo::Philo()
  : QActive(Q_STATE_CAST(&initial)),
    m_timeEvt(this, TIMEOUT_SIG, 0U),
    m_id(0xFFU)
{}

//${AOs::Philo::SM} ..........................................................
Q_STATE_DEF(Philo, initial) {
    //${AOs::Philo::SM::initial}
    Q_UNUSED_PAR(e);

    m_id = static_cast<std::uint8_t>(this - &inst[0]);

    QS_OBJ_ARR_DICTIONARY(&Philo::inst[m_id], m_id);
    QS_OBJ_ARR_DICTIONARY(&Philo::inst[m_id].m_timeEvt, m_id);

    subscribe(EAT_SIG);
    subscribe(TEST_SIG);

    QS_FUN_DICTIONARY(&Philo::thinking);
    QS_FUN_DICTIONARY(&Philo::hungry);
    QS_FUN_DICTIONARY(&Philo::eating);

    return tran(&thinking);
}

//${AOs::Philo::SM::thinking} ................................................
Q_STATE_DEF(Philo, thinking) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Philo::SM::thinking}
        case Q_ENTRY_SIG: {
            m_timeEvt.armX(think_time(), 0U);
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Philo::SM::thinking}
        case Q_EXIT_SIG: {
            m_timeEvt.disarm();
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Philo::SM::thinking::TIMEOUT}
        case TIMEOUT_SIG: {
            status_ = tran(&hungry);
            break;
        }
        //${AOs::Philo::SM::thinking::EAT, DONE}
        case EAT_SIG: // intentionally fall through
        case DONE_SIG: {
            // EAT or DONE must be for other Philos than this one
            Q_ASSERT(Q_EVT_CAST(TableEvt)->philoId != m_id);
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Philo::SM::thinking::TEST}
        case TEST_SIG: {
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

//${AOs::Philo::SM::hungry} ..................................................
Q_STATE_DEF(Philo, hungry) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Philo::SM::hungry}
        case Q_ENTRY_SIG: {
            #ifdef QEVT_PAR_INIT
            TableEvt const *pe = Q_NEW(TableEvt, HUNGRY_SIG, m_id);
            #else
            TableEvt *pe = Q_NEW(TableEvt, HUNGRY_SIG);
            pe->philoId = m_id;
            #endif
            AO_Table->POST(pe, this);
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Philo::SM::hungry::EAT}
        case EAT_SIG: {
            //${AOs::Philo::SM::hungry::EAT::[e->philoId==m_id]}
            if (Q_EVT_CAST(TableEvt)->philoId == m_id) {
                status_ = tran(&eating);
            }
            else {
                status_ = Q_RET_UNHANDLED;
            }
            break;
        }
        //${AOs::Philo::SM::hungry::DONE}
        case DONE_SIG: {
            // DONE must be for other Philos than this one
            Q_ASSERT(Q_EVT_CAST(TableEvt)->philoId != m_id);
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

//${AOs::Philo::SM::eating} ..................................................
Q_STATE_DEF(Philo, eating) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Philo::SM::eating}
        case Q_ENTRY_SIG: {
            m_timeEvt.armX(eat_time(), 0U);
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Philo::SM::eating}
        case Q_EXIT_SIG: {
            m_timeEvt.disarm();

            #ifdef QEVT_PAR_INIT
            TableEvt const *pe = Q_NEW(TableEvt, DONE_SIG, m_id);
            #else
            TableEvt *pe = Q_NEW(TableEvt, DONE_SIG);
            pe->philoId = m_id;
            #endif
            QP::QActive::PUBLISH(pe, this);
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Philo::SM::eating::TIMEOUT}
        case TIMEOUT_SIG: {
            status_ = tran(&thinking);
            break;
        }
        //${AOs::Philo::SM::eating::EAT, DONE}
        case EAT_SIG: // intentionally fall through
        case DONE_SIG: {
            // EAT or DONE must be for other Philos than this one
            Q_ASSERT(Q_EVT_CAST(TableEvt)->philoId != m_id);
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

} // namespace APP
//$enddef${AOs::Philo} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

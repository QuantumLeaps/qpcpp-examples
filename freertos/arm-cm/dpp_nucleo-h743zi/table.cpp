//$file${.::table.cpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: dpp.qm
// File:  ${.::table.cpp}
//
// This code has been generated by QM 7.0.0 <www.state-machine.com/qm>.
// DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
//
// Copyright (C) 2005 Quantum Leaps, LLC. All rights reserved.
//
//                    Q u a n t u m  L e a P s
//                    ------------------------
//                    Modern Embedded Software
//
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-QL-commercial
//
// The QP/C++ software is dual-licensed under the terms of the open-source
// GNU General Public License (GPL) or under the terms of one of the closed-
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
//
//$endhead${.::table.cpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include "qpcpp.hpp"             // QP/C++ real-time embedded framework
#include "dpp.hpp"               // DPP Application interface
#include "bsp.hpp"               // Board Support Package

//$declare${AOs::Table} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${AOs::Table} ..............................................................
class Table : public QP::QActive {
private:
    std::uint8_t m_fork[N_PHILO];
    bool m_isHungry[N_PHILO];

public:
    static Table inst;

public:
    Table();

protected:
    Q_STATE_DECL(initial);
    Q_STATE_DECL(active);
    Q_STATE_DECL(serving);
    Q_STATE_DECL(paused);
}; // class Table

} // namespace APP
//$enddecl${AOs::Table} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//----------------------------------------------------------------------------
// unnamed namespace for local definitions with internal linkage
namespace {
Q_DEFINE_THIS_FILE

// helper function to provide the RIGHT neighbour of a Philo[n]
static inline std::uint8_t right(std::uint8_t const n) {
    return static_cast<std::uint8_t>((n + (APP::N_PHILO - 1U)) % APP::N_PHILO);
}

// helper function to provide the LEFT neighbour of a Philo[n]
static inline std::uint8_t left(std::uint8_t const n) {
    return static_cast<std::uint8_t>((n + 1U) % APP::N_PHILO);
}

static constexpr std::uint8_t FREE {0U};
static constexpr std::uint8_t USED {1U};

static constexpr char const * const THINKING {"thinking"};
static constexpr char const * const HUNGRY   {"hungry  "};
static constexpr char const * const EATING   {"eating  "};

} // unnamed namespace
//----------------------------------------------------------------------------
//$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// Check for the minimum required QP version
#if (QP_VERSION < 730U) || (QP_VERSION != ((QP_RELEASE^4294967295U)%0x2710U))
#error qpcpp version 7.3.0 or higher required
#endif
//$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$define${Shared::AO_Table} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${Shared::AO_Table} ........................................................
QP::QActive * const AO_Table = &Table::inst;

} // namespace APP
//$enddef${Shared::AO_Table} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$define${AOs::Table} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${AOs::Table} ..............................................................
Table Table::inst;

//${AOs::Table::Table} .......................................................
Table::Table()
  : QActive(Q_STATE_CAST(&initial))
{
    for (std::uint8_t n = 0U; n < N_PHILO; ++n) {
        m_fork[n] = FREE;
        m_isHungry[n] = false;
    }
}

//${AOs::Table::SM} ..........................................................
Q_STATE_DEF(Table, initial) {
    //${AOs::Table::SM::initial}
    Q_UNUSED_PAR(e);

    QS_OBJ_DICTIONARY(&Table::inst);

    subscribe(DONE_SIG);
    subscribe(PAUSE_SIG);
    subscribe(SERVE_SIG);
    subscribe(TEST_SIG);

    for (std::uint8_t n = 0U; n < N_PHILO; ++n) {
        m_fork[n] = FREE;
        m_isHungry[n] = false;
        BSP::displayPhilStat(n, THINKING);
    }

    QS_FUN_DICTIONARY(&Table::active);
    QS_FUN_DICTIONARY(&Table::serving);
    QS_FUN_DICTIONARY(&Table::paused);

    return tran(&serving);
}

//${AOs::Table::SM::active} ..................................................
Q_STATE_DEF(Table, active) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Table::SM::active::TEST}
        case TEST_SIG: {
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Table::SM::active::EAT}
        case EAT_SIG: {
            Q_ERROR();
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

//${AOs::Table::SM::active::serving} .........................................
Q_STATE_DEF(Table, serving) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Table::SM::active::serving}
        case Q_ENTRY_SIG: {
             // give permissions to eat...
            for (std::uint8_t n = 0U; n < N_PHILO; ++n) {
                if (m_isHungry[n]
                    && (m_fork[left(n)] == FREE)
                    && (m_fork[n] == FREE))
                {
                    m_fork[left(n)] = USED;
                    m_fork[n] = USED;
            #ifdef QEVT_PAR_INIT
                    TableEvt const *te = Q_NEW(TableEvt, EAT_SIG, n);
            #else
                    TableEvt *te = Q_NEW(TableEvt, EAT_SIG);
                    te->philoId = n;
            #endif
                    QP::QActive::PUBLISH(te, this);
                    m_isHungry[n] = false;
                    BSP::displayPhilStat(n, EATING);
                }
            }
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Table::SM::active::serving::HUNGRY}
        case HUNGRY_SIG: {
            std::uint8_t n = Q_EVT_CAST(TableEvt)->philoId;

            // phil ID must be in range and he must be not hungry
            Q_ASSERT((n < N_PHILO) && (!m_isHungry[n]));

            BSP::displayPhilStat(n, HUNGRY);
            std::uint8_t m = left(n);
            //${AOs::Table::SM::active::serving::HUNGRY::[bothfree]}
            if ((m_fork[m] == FREE) && (m_fork[n] == FREE)) {
                m_fork[m] = USED;
                m_fork[n] = USED;
                #ifdef QEVT_PAR_INIT
                TableEvt const *pe = Q_NEW(TableEvt, EAT_SIG, n);
                #else
                TableEvt *pe = Q_NEW(TableEvt, EAT_SIG);
                pe->philoId = n;
                #endif
                QP::QActive::PUBLISH(pe, this);
                BSP::displayPhilStat(n, EATING);
                status_ = Q_RET_HANDLED;
            }
            //${AOs::Table::SM::active::serving::HUNGRY::[else]}
            else {
                m_isHungry[n] = true;
                status_ = Q_RET_HANDLED;
            }
            break;
        }
        //${AOs::Table::SM::active::serving::DONE}
        case DONE_SIG: {
            std::uint8_t n = Q_EVT_CAST(TableEvt)->philoId;

            // phil ID must be in range and he must be not hungry
            Q_ASSERT((n < N_PHILO) && (!m_isHungry[n]));

            BSP::displayPhilStat(n, THINKING);
            std::uint8_t m = left(n);

            // both forks of Phil[n] must be used
            Q_ASSERT((m_fork[n] == USED) && (m_fork[m] == USED));

            m_fork[m] = FREE;
            m_fork[n] = FREE;
            m = right(n); // check the right neighbor

            if (m_isHungry[m] && (m_fork[m] == FREE)) {
                m_fork[n] = USED;
                m_fork[m] = USED;
                m_isHungry[m] = false;
            #ifdef QEVT_PAR_INIT
                TableEvt const *pe = Q_NEW(TableEvt, EAT_SIG, m);
            #else
                TableEvt *pe = Q_NEW(TableEvt, EAT_SIG);
                pe->philoId = m;
            #endif
                QP::QActive::PUBLISH(pe, this);
                BSP::displayPhilStat(m, EATING);
            }
            m = left(n); // check the left neighbor
            n = left(m); // left fork of the left neighbor
            if (m_isHungry[m] && (m_fork[n] == FREE)) {
                m_fork[m] = USED;
                m_fork[n] = USED;
                m_isHungry[m] = false;
            #ifdef QEVT_PAR_INIT
                TableEvt const *pe = Q_NEW(TableEvt, EAT_SIG, m);
            #else
                TableEvt *pe = Q_NEW(TableEvt, EAT_SIG);
                pe->philoId = m;
            #endif
                QP::QActive::PUBLISH(pe, this);
                BSP::displayPhilStat(m, EATING);
            }
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Table::SM::active::serving::EAT}
        case EAT_SIG: {
            Q_ERROR();
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Table::SM::active::serving::PAUSE}
        case PAUSE_SIG: {
            status_ = tran(&paused);
            break;
        }
        default: {
            status_ = super(&active);
            break;
        }
    }
    return status_;
}

//${AOs::Table::SM::active::paused} ..........................................
Q_STATE_DEF(Table, paused) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Table::SM::active::paused}
        case Q_ENTRY_SIG: {
            BSP::displayPaused(1U);
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Table::SM::active::paused}
        case Q_EXIT_SIG: {
            BSP::displayPaused(0U);
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Table::SM::active::paused::SERVE}
        case SERVE_SIG: {
            status_ = tran(&serving);
            break;
        }
        //${AOs::Table::SM::active::paused::HUNGRY}
        case HUNGRY_SIG: {
            std::uint8_t n = Q_EVT_CAST(TableEvt)->philoId;

            // philo ID must be in range and he must be not hungry
            Q_ASSERT((n < N_PHILO) && (!m_isHungry[n]));

            m_isHungry[n] = true;
            BSP::displayPhilStat(n, HUNGRY);
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Table::SM::active::paused::DONE}
        case DONE_SIG: {
            std::uint8_t n = Q_EVT_CAST(TableEvt)->philoId;

            // phil ID must be in range and he must be not hungry
            Q_ASSERT((n < N_PHILO) && (!m_isHungry[n]));

            BSP::displayPhilStat(n, THINKING);
            std::uint8_t m = left(n);

            // both forks of Phil[n] must be used
            Q_ASSERT((m_fork[n] == USED) && (m_fork[m] == USED));

            m_fork[m] = FREE;
            m_fork[n] = FREE;
            status_ = Q_RET_HANDLED;
            break;
        }
        default: {
            status_ = super(&active);
            break;
        }
    }
    return status_;
}

} // namespace APP
//$enddef${AOs::Table} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

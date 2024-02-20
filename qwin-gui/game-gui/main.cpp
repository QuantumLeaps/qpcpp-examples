//============================================================================
// Product: "Fly 'n' Shoot" game example for Windows
// Last updated for version 7.3.0
// Last updated on  2023-09-06
//
//                    Q u a n t u m     L e a P s
//                    ---------------------------
//                    innovating embedded systems
//
// Copyright (C) Quantum Leaps, LLC. All rights reserved.
//
// This program is open source software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Alternatively, this program may be distributed and modified under the
// terms of Quantum Leaps commercial licenses, which expressly supersede
// the GNU General Public License and are specifically designed for
// licensees interested in retaining the proprietary status of their code.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <www.gnu.org/licenses/>.
//
// Contact information:
// <www.state-machine.com/licensing>
// <info@state-machine.com>
//============================================================================
#include "qpcpp.hpp"
#include "bsp.hpp"
#include "game.hpp"

// "fudge factor" for Windows, see NOTE1
enum { WIN_FUDGE_FACTOR = 10 };

static QP::QTicker l_ticker0(0); // ticker for tick rate 0

QP::QTicker* BSP::the_Ticker0 = &l_ticker0;

//............................................................................
int main() {
    QP::QF::init(); // initialize the framework and the underlying RT kernel
    BSP::init();    // initialize the Board Support Package

    // initialize the event pools...
    //static QF_MPOOL_EL(QP::QEvt) smlPoolSto[10*WIN_FUDGE_FACTOR];
    //QP::QF::poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));
    // NOTE:
    // After new rounding up, the "small" memory pool has the same block-size
    // as the next "medium" pool. If such "small" pool is initialized,
    // the next "medium" pool will casuse an assertion (the same block-size
    // pool already exists).

    static QF_MPOOL_EL(GAME::ObjectImageEvt)
        medPoolSto[(2*GAME_MINES_MAX + 10)*WIN_FUDGE_FACTOR];
    QP::QF::poolInit(medPoolSto, sizeof(medPoolSto), sizeof(medPoolSto[0]));

    // init publish-subscribe
    static QP::QSubscrList subscrSto[GAME::MAX_PUB_SIG];
    QP::QActive::psInit(subscrSto, Q_DIM(subscrSto));

    // object dictionaries for AOs...
    QS_OBJ_DICTIONARY(GAME::AO_Missile);
    QS_OBJ_DICTIONARY(GAME::AO_Ship);
    QS_OBJ_DICTIONARY(GAME::AO_Tunnel);

    // signal dictionaries for globally published events...
    QS_SIG_DICTIONARY(GAME::TIME_TICK_SIG,      nullptr);
    QS_SIG_DICTIONARY(GAME::PLAYER_TRIGGER_SIG, nullptr);
    QS_SIG_DICTIONARY(GAME::PLAYER_QUIT_SIG,    nullptr);
    QS_SIG_DICTIONARY(GAME::GAME_OVER_SIG,      nullptr);

    // start the active objects...
    BSP::the_Ticker0->start(1U, // priority
                             0, 0, 0, 0);
    static QP::QEvt const *tunnelQueueSto[(GAME_MINES_MAX + 5)*WIN_FUDGE_FACTOR];
    GAME::AO_Tunnel ->start(2U,                     // priority
                      tunnelQueueSto, Q_DIM(tunnelQueueSto), // evt queue
                      nullptr, 0U);  // no per-thread stack

    static QP::QEvt const *shipQueueSto[3*WIN_FUDGE_FACTOR];
    GAME::AO_Ship   ->start(3U,                     // priority
                      shipQueueSto, Q_DIM(shipQueueSto), // evt queue
                      nullptr, 0U);  // no per-thread stack

    static QP::QEvt const *missileQueueSto[2*WIN_FUDGE_FACTOR];
    GAME::AO_Missile->start(4U,                     // priority
                      missileQueueSto, Q_DIM(missileQueueSto), // evt queue
                      nullptr, 0U);  // no per-thread stack

    return QP::QF::run(); // run the QF application
}


//============================================================================
// NOTE1:
// Windows is not a deterministic real-time system, which means that the
// system can occasionally and unexpectedly "choke and freeze" for a number
// of seconds. The designers of Windows have dealt with these sort of issues
// by massively oversizing the resources available to the applications. For
// example, the default Windows GUI message queues size is 10,000 entries,
// which can dynamically grow to an even larger number. Also the stacks of
// Win32 threads can dynamically grow to several megabytes.
//
// In contrast, the event queues, event pools, and stack size inside the
// real-time embedded (RTE) systems can be (and must be) much smaller,
// because you typically can put an upper bound on the real-time behavior
// and the resulting delays.
//
// To be able to run the unmodified applications designed originally for
// RTE systems on Windows, and to reduce the odds of resource shortages in
// this case, the generous WIN_FUDGE_FACTOR is used to oversize the
// event queues and event pools.
//


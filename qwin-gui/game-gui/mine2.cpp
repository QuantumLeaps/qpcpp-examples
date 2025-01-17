//$file${.::mine2.cpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: game.qm
// File:  ${.::mine2.cpp}
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
//$endhead${.::mine2.cpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include "qpcpp.hpp"
#include "bsp.hpp"
#include "game.hpp"

Q_DEFINE_THIS_FILE

// encapsulated declaration of the Mine2 HSM ---------------------------------
//$declare${AOs::Mine2} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace GAME {

//${AOs::Mine2} ..............................................................
class Mine2 : public QP::QHsm {
private:
    std::uint8_t m_x;
    std::uint8_t m_y;
    std::uint8_t m_exp_ctr;

public:
    static Mine2 inst[GAME_MINES_MAX];

public:
    Mine2()
      : QHsm(&initial)
    {}

protected:
    Q_STATE_DECL(initial);
    Q_STATE_DECL(unused);
    Q_STATE_DECL(used);
    Q_STATE_DECL(exploding);
    Q_STATE_DECL(planted);
}; // class Mine2

} // namespace GAME
//$enddecl${AOs::Mine2} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

namespace GAME {

// helper function to provide the ID of this mine ............................
static inline std::uint8_t MINE_ID(Mine2 const * const me) {
    return static_cast<std::uint8_t>(me - &Mine2::inst[0]);
}

} // namespace GAME

// Mine1 class definition ----------------------------------------------------
//$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// Check for the minimum required QP version
#if (QP_VERSION < 730U) || (QP_VERSION != ((QP_RELEASE^4294967295U)%0x2710U))
#error qpcpp version 7.3.0 or higher required
#endif
//$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$define${Shared::Mine2_getInst} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace GAME {

//${Shared::Mine2_getInst} ...................................................
QP::QHsm * Mine2_getInst(std::uint8_t id) {
    Q_REQUIRE(id < Q_DIM(Mine2::inst));
    return &Mine2::inst[id];
}

} // namespace GAME
//$enddef${Shared::Mine2_getInst} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$define${AOs::Mine2} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace GAME {

//${AOs::Mine2} ..............................................................
Mine2 Mine2::inst[GAME_MINES_MAX];

//${AOs::Mine2::SM} ..........................................................
Q_STATE_DEF(Mine2, initial) {
    //${AOs::Mine2::SM::initial}
    static bool dict_sent = false;
    if (!dict_sent) {
        dict_sent = true;
        // object dictionaries for Mine2 pool...
        QS_OBJ_DICTIONARY(&Mine2::inst[0]);
        QS_OBJ_DICTIONARY(&Mine2::inst[1]);
        QS_OBJ_DICTIONARY(&Mine2::inst[2]);
        QS_OBJ_DICTIONARY(&Mine2::inst[3]);
        QS_OBJ_DICTIONARY(&Mine2::inst[4]);

        // function dictionaries for Mine2 SM...
        QS_FUN_DICTIONARY(&Mine2::initial);
        QS_FUN_DICTIONARY(&Mine2::unused);
        QS_FUN_DICTIONARY(&Mine2::used);
        QS_FUN_DICTIONARY(&Mine2::planted);
        QS_FUN_DICTIONARY(&Mine2::exploding);
    }
    // local signals...
    QS_SIG_DICTIONARY(MINE_PLANT_SIG,    this);
    QS_SIG_DICTIONARY(MINE_DISABLED_SIG, this);
    QS_SIG_DICTIONARY(MINE_RECYCLE_SIG,  this);
    QS_SIG_DICTIONARY(SHIP_IMG_SIG,      this);
    QS_SIG_DICTIONARY(MISSILE_IMG_SIG,   this);

    (void)e; // unused parameter

    QS_FUN_DICTIONARY(&Mine2::unused);
    QS_FUN_DICTIONARY(&Mine2::used);
    QS_FUN_DICTIONARY(&Mine2::exploding);
    QS_FUN_DICTIONARY(&Mine2::planted);

    return tran(&unused);
}

//${AOs::Mine2::SM::unused} ..................................................
Q_STATE_DEF(Mine2, unused) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Mine2::SM::unused::MINE_PLANT}
        case MINE_PLANT_SIG: {
            m_x = Q_EVT_CAST(ObjectPosEvt)->x;
            m_y = Q_EVT_CAST(ObjectPosEvt)->y;
            status_ = tran(&planted);
            break;
        }
        default: {
            status_ = super(&top);
            break;
        }
    }
    return status_;
}

//${AOs::Mine2::SM::used} ....................................................
Q_STATE_DEF(Mine2, used) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Mine2::SM::used}
        case Q_EXIT_SIG: {
            // tell the Tunnel that this mine is becoming disabled
            MineEvt *mev = Q_NEW(MineEvt, MINE_DISABLED_SIG);
            mev->id = MINE_ID(this);
            AO_Tunnel->POST(mev, this);
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Mine2::SM::used::MINE_RECYCLE}
        case MINE_RECYCLE_SIG: {
            status_ = tran(&unused);
            break;
        }
        default: {
            status_ = super(&top);
            break;
        }
    }
    return status_;
}

//${AOs::Mine2::SM::used::exploding} .........................................
Q_STATE_DEF(Mine2, exploding) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Mine2::SM::used::exploding}
        case Q_ENTRY_SIG: {
            m_exp_ctr = 0U;
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Mine2::SM::used::exploding::TIME_TICK}
        case TIME_TICK_SIG: {
            //${AOs::Mine2::SM::used::exploding::TIME_TICK::[stillonscreen?]}
            if ((m_x >= GAME_SPEED_X) && (m_exp_ctr < 15)) {
                ++m_exp_ctr;  // advance the explosion counter
                m_x -= GAME_SPEED_X; // move explosion by 1 step

                // tell the Game to render the current stage of Explosion
                ObjectImageEvt *oie = Q_NEW(ObjectImageEvt, EXPLOSION_SIG);
                oie->x   = m_x + 1U;  // x of explosion
                oie->y   = (int8_t)((int)m_y - 4 + 2); // y of explosion
                oie->bmp = EXPLOSION0_BMP + (m_exp_ctr >> 2);
                AO_Tunnel->POST(oie, this);
                status_ = Q_RET_HANDLED;
            }
            //${AOs::Mine2::SM::used::exploding::TIME_TICK::[else]}
            else {
                status_ = tran(&unused);
            }
            break;
        }
        default: {
            status_ = super(&used);
            break;
        }
    }
    return status_;
}

//${AOs::Mine2::SM::used::planted} ...........................................
Q_STATE_DEF(Mine2, planted) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Mine2::SM::used::planted::TIME_TICK}
        case TIME_TICK_SIG: {
            //${AOs::Mine2::SM::used::planted::TIME_TICK::[m_x>=GAME_SPEED_X]}
            if (m_x >= GAME_SPEED_X) {
                m_x -= GAME_SPEED_X; // move the mine 1 step
                // tell the Tunnel to draw the Mine
                ObjectImageEvt *oie = Q_NEW(ObjectImageEvt, MINE_IMG_SIG);
                oie->x   = m_x;
                oie->y   = m_y;
                oie->bmp = MINE2_BMP;
                AO_Tunnel->POST(oie, this);
                status_ = Q_RET_HANDLED;
            }
            //${AOs::Mine2::SM::used::planted::TIME_TICK::[else]}
            else {
                status_ = tran(&unused);
            }
            break;
        }
        //${AOs::Mine2::SM::used::planted::SHIP_IMG}
        case SHIP_IMG_SIG: {
            uint8_t x   = Q_EVT_CAST(ObjectImageEvt)->x;
            uint8_t y   = Q_EVT_CAST(ObjectImageEvt)->y;
            uint8_t bmp = Q_EVT_CAST(ObjectImageEvt)->bmp;
            //${AOs::Mine2::SM::used::planted::SHIP_IMG::[collisionwithMINE2_BMP?]}
            if (BSP::doBitmapsOverlap(MINE2_BMP, m_x, m_y, bmp, x, y)) {
                static MineEvt const mine2_hit(HIT_MINE_SIG, 2U);
                AO_Ship->POST(&mine2_hit, this);
                // go straight to 'disabled' and let the Ship do
                // the exploding
                status_ = tran(&unused);
            }
            else {
                status_ = Q_RET_UNHANDLED;
            }
            break;
        }
        //${AOs::Mine2::SM::used::planted::MISSILE_IMG}
        case MISSILE_IMG_SIG: {
            uint8_t x   = Q_EVT_CAST(ObjectImageEvt)->x;
            uint8_t y   = Q_EVT_CAST(ObjectImageEvt)->y;
            uint8_t bmp = Q_EVT_CAST(ObjectImageEvt)->bmp;
            //${AOs::Mine2::SM::used::planted::MISSILE_IMG::[collisionwithMINE2_MISSILE_BMP?~}
            if (BSP::doBitmapsOverlap(MINE2_MISSILE_BMP, m_x, m_y, bmp, x, y)) {
                static ScoreEvt const mine2_destroyed(DESTROYED_MINE_SIG, 45U);
                AO_Missile->POST(&mine2_destroyed, this);
                status_ = tran(&exploding);
            }
            else {
                status_ = Q_RET_UNHANDLED;
            }
            break;
        }
        default: {
            status_ = super(&used);
            break;
        }
    }
    return status_;
}

} // namespace GAME
//$enddef${AOs::Mine2} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

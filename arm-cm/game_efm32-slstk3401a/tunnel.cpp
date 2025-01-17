//$file${.::tunnel.cpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: game.qm
// File:  ${.::tunnel.cpp}
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
//$endhead${.::tunnel.cpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include "qpcpp.hpp"
#include "bsp.hpp"
#include "game.hpp"
#include <string.h> // for memmove() and memcpy()

Q_DEFINE_THIS_FILE

// declaration of the Tunnel AO ----------------------------------------------
//$declare${AOs::Tunnel} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace GAME {

//${AOs::Tunnel} .............................................................
class Tunnel : public QP::QActive {
private:
    QP::QTimeEvt m_blinkTimeEvt;
    QP::QTimeEvt m_screenTimeEvt;
    QP::QHsm * m_mines[GAME_MINES_MAX];
    QP::QHsm * m_mine1_pool[GAME_MINES_MAX];
    QP::QHsm * m_mine2_pool[GAME_MINES_MAX];
    std::uint8_t m_blink_ctr;
    std::uint8_t m_last_mine_x;
    std::uint8_t m_last_mine_y;
    std::uint8_t m_wall_thickness_top;
    std::uint8_t m_wall_thickness_bottom;
    std::uint8_t m_wall_gap;

public:
    static Tunnel inst;

public:
    Tunnel();
    void advance();

private:
    void plantMine();
    void dispatchToAllMines(QP::QEvt const * e);

protected:
    Q_STATE_DECL(initial);
    Q_STATE_DECL(active);
    Q_STATE_DECL(show_logo);
    Q_STATE_DECL(demo);
    Q_STATE_DECL(playing);
    Q_STATE_DECL(game_over);
    Q_STATE_DECL(screen_saver);
    Q_STATE_DECL(screen_saver_hide);
    Q_STATE_DECL(screen_saver_show);
    Q_STATE_DECL(final);
}; // class Tunnel

} // namespace GAME
//$enddecl${AOs::Tunnel} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

// Public-scope shared objects -----------------------------------------------
//$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// Check for the minimum required QP version
#if (QP_VERSION < 730U) || (QP_VERSION != ((QP_RELEASE^4294967295U)%0x2710U))
#error qpcpp version 7.3.0 or higher required
#endif
//$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$define${Shared::AO_Tunnel} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace GAME {

//${Shared::AO_Tunnel} .......................................................
QP::QActive * const AO_Tunnel = &Tunnel::inst;

} // namespace GAME
//$enddef${Shared::AO_Tunnel} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

// Active object definition ==================================================
//$define${AOs::Tunnel} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace GAME {

//${AOs::Tunnel} .............................................................
Tunnel Tunnel::inst;

//${AOs::Tunnel::Tunnel} .....................................................
Tunnel::Tunnel()
  : QActive(&initial),
    m_blinkTimeEvt(this, BLINK_TIMEOUT_SIG, 0U),
    m_screenTimeEvt(this, SCREEN_TIMEOUT_SIG, 0U),
    m_last_mine_x(0U), m_last_mine_y(0U)
{
    for (uint8_t n = 0U; n < GAME_MINES_MAX; ++n) {
        m_mine1_pool[n] = Mine1_getInst(n);  // initialize mine1-type pool
        m_mine2_pool[n] = Mine2_getInst(n);  // initialize mine2-type pool
        m_mines[n] = nullptr;                // mine 'n' is unused
    }
}

//${AOs::Tunnel::advance} ....................................................
void Tunnel::advance() {
    uint32_t rnd = (BSP::random() & 0xFFU);

    // reduce the top wall thickness 18.75% of the time
    if ((rnd < 48U) && (m_wall_thickness_top > 0U)) {
        --m_wall_thickness_top;
    }

    // reduce the bottom wall thickness 18.75% of the time
    if ((rnd > 208U) && (m_wall_thickness_bottom > 0U)) {
        --m_wall_thickness_bottom;
    }

    rnd = (BSP::random() & 0xFFU);

    // grow the bottom wall thickness 19.14% of the time
    if ((rnd < 49U)
        && ((GAME_TUNNEL_HEIGHT
             - m_wall_thickness_top
             - m_wall_thickness_bottom) > m_wall_gap))
    {
        ++m_wall_thickness_bottom;
    }

    // grow the top wall thickness 19.14% of the time
    if ((rnd > 207U)
        && ((GAME_TUNNEL_HEIGHT
             - m_wall_thickness_top
             - m_wall_thickness_bottom) > m_wall_gap))
    {
        ++m_wall_thickness_top;
    }

    // advance the Tunnel by 1 game step to the left
    // and copy the Tunnel layer to the main frame buffer
    //
    BSP::advanceWalls(m_wall_thickness_top, m_wall_thickness_bottom);
}

//${AOs::Tunnel::plantMine} ..................................................
void Tunnel::plantMine() {
    uint32_t rnd = (BSP::random() & 0xFFU);

    if (m_last_mine_x > 0U) {
        --m_last_mine_x; // shift the last Mine 1 position to the left
    }
    // last mine far enough?
    if ((m_last_mine_x + GAME_MINES_DIST_MIN < GAME_TUNNEL_WIDTH)
        && (rnd < 8U)) // place the mines only 5% of the time
    {
        uint8_t n;
        for (n = 0U; n < Q_DIM(m_mines); ++n) { // look for disabled mines
            if (m_mines[n] == nullptr) {
                break;
            }
        }
        if (n < Q_DIM(m_mines)) { // a disabled Mine found?
            rnd = (BSP::random() & 0xFFFFU);

            if ((rnd & 1U) == 0U) { // choose the type of the mine
                m_mines[n] = m_mine1_pool[n];
            }
            else {
                m_mines[n] = m_mine2_pool[n];
            }

            // new Mine is planted by the end of the tunnel
            m_last_mine_x = GAME_TUNNEL_WIDTH - 8U;

            // choose a random y-position for the Mine in the Tunnel
            rnd %= (GAME_TUNNEL_HEIGHT
                    - m_wall_thickness_top
                    - m_wall_thickness_bottom - 4U);
            m_last_mine_y = (uint8_t)(m_wall_thickness_top + 2U + rnd);

            // evt to dispatch to the Mine
            ObjectPosEvt const ope(MINE_PLANT_SIG, m_last_mine_x, m_last_mine_y);
            m_mines[n]->dispatch(&ope, getPrio()); // direct dispatch
        }
    }
}

//${AOs::Tunnel::dispatchToAllMines} .........................................
void Tunnel::dispatchToAllMines(QP::QEvt const * e) {
    for (uint8_t n = 0U; n < GAME_MINES_MAX; ++n) {
        if (m_mines[n] != nullptr) { // is the mine used?
            m_mines[n]->dispatch(e, getPrio());
        }
    }
}

//${AOs::Tunnel::SM} .........................................................
Q_STATE_DEF(Tunnel, initial) {
    //${AOs::Tunnel::SM::initial}
    for (uint8_t n = 0; n < GAME_MINES_MAX; ++n) {
       m_mine1_pool[n]->init(getPrio()); // take the initial tran. for Mine1
       m_mine2_pool[n]->init(getPrio()); // take the initial tran. for Mine2
    }

    BSP::randomSeed(1234U); // seed the pseudo-random generator

    subscribe(TIME_TICK_SIG);
    subscribe(PLAYER_TRIGGER_SIG);
    subscribe(PLAYER_QUIT_SIG);

    // object dictionary for Tunnel object...
    QS_OBJ_DICTIONARY(&Tunnel::inst.m_blinkTimeEvt);
    QS_OBJ_DICTIONARY(&Tunnel::inst.m_screenTimeEvt);

    // local signals...
    QS_SIG_DICTIONARY(BLINK_TIMEOUT_SIG,  this);
    QS_SIG_DICTIONARY(SCREEN_TIMEOUT_SIG, this);
    QS_SIG_DICTIONARY(SHIP_IMG_SIG,       this);
    QS_SIG_DICTIONARY(MISSILE_IMG_SIG,    this);
    QS_SIG_DICTIONARY(MINE_IMG_SIG,       this);
    QS_SIG_DICTIONARY(MINE_DISABLED_SIG,  this);
    QS_SIG_DICTIONARY(EXPLOSION_SIG,      this);
    QS_SIG_DICTIONARY(SCORE_SIG,          this);

    (void)e; // unused parameter

    QS_FUN_DICTIONARY(&Tunnel::active);
    QS_FUN_DICTIONARY(&Tunnel::show_logo);
    QS_FUN_DICTIONARY(&Tunnel::demo);
    QS_FUN_DICTIONARY(&Tunnel::playing);
    QS_FUN_DICTIONARY(&Tunnel::game_over);
    QS_FUN_DICTIONARY(&Tunnel::screen_saver);
    QS_FUN_DICTIONARY(&Tunnel::screen_saver_hide);
    QS_FUN_DICTIONARY(&Tunnel::screen_saver_show);
    QS_FUN_DICTIONARY(&Tunnel::final);

    return tran(&show_logo);
}

//${AOs::Tunnel::SM::active} .................................................
Q_STATE_DEF(Tunnel, active) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Tunnel::SM::active::MINE_DISABLED}
        case MINE_DISABLED_SIG: {
            Q_ASSERT((Q_EVT_CAST(MineEvt)->id < GAME_MINES_MAX)
                && (m_mines[Q_EVT_CAST(MineEvt)->id] != nullptr));
            m_mines[Q_EVT_CAST(MineEvt)->id] = nullptr;
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Tunnel::SM::active::PLAYER_QUIT}
        case PLAYER_QUIT_SIG: {
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

//${AOs::Tunnel::SM::active::show_logo} ......................................
Q_STATE_DEF(Tunnel, show_logo) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Tunnel::SM::active::show_logo}
        case Q_ENTRY_SIG: {
            m_blinkTimeEvt.armX(BSP::TICKS_PER_SEC/2U,
                                    BSP::TICKS_PER_SEC/2U); // every 1/2 sec
            m_screenTimeEvt.armX(BSP::TICKS_PER_SEC*5U, 0U); // in 5 sec
            m_blink_ctr = 0U;
            BSP::paintString(24U, (GAME_TUNNEL_HEIGHT / 2U) - 8U, "Quantum LeAps");
            BSP::paintString(16U, (GAME_TUNNEL_HEIGHT / 2U) + 0U, "state-machine.com");

            BSP::paintString(1U, GAME_TUNNEL_HEIGHT - 18U, "Fire missile: BTN0");
            BSP::paintString(1U, GAME_TUNNEL_HEIGHT - 10U, "Fly ship up:  BTN1");

            BSP::updateScreen();
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Tunnel::SM::active::show_logo}
        case Q_EXIT_SIG: {
            m_blinkTimeEvt.disarm();
            m_screenTimeEvt.disarm();
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Tunnel::SM::active::show_logo::SCREEN_TIMEOUT}
        case SCREEN_TIMEOUT_SIG: {
            status_ = tran(&demo);
            break;
        }
        //${AOs::Tunnel::SM::active::show_logo::BLINK_TIMEOUT}
        case BLINK_TIMEOUT_SIG: {
            m_blink_ctr ^= 1U; // toggle the blink counter
            //${AOs::Tunnel::SM::active::show_logo::BLINK_TIMEOUT::[m_blink_ctr==0U]}
            if (m_blink_ctr == 0U) {
                BSP::paintString(24U + 8U*6U, (GAME_TUNNEL_HEIGHT / 2U) - 8U, "LeAps");
                BSP::updateScreen();
                status_ = Q_RET_HANDLED;
            }
            //${AOs::Tunnel::SM::active::show_logo::BLINK_TIMEOUT::[else]}
            else {
                BSP::paintString(24U + 8U*6U, (GAME_TUNNEL_HEIGHT / 2U) - 8U, "LeaPs");
                BSP::updateScreen();
                status_ = Q_RET_HANDLED;
            }
            break;
        }
        default: {
            status_ = super(&active);
            break;
        }
    }
    return status_;
}

//${AOs::Tunnel::SM::active::demo} ...........................................
Q_STATE_DEF(Tunnel, demo) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Tunnel::SM::active::demo}
        case Q_ENTRY_SIG: {
            m_last_mine_x = 0U; // last mine at right edge of the tunnel
            m_last_mine_y = 0U;
            // set the tunnel properties...
            m_wall_thickness_top = 0U;
            m_wall_thickness_bottom = 0U;
            m_wall_gap = GAME_WALLS_GAP_Y;

            BSP::clearWalls(); // erase the tunnel walls

            m_blinkTimeEvt.armX(BSP::TICKS_PER_SEC/2U,
                                    BSP::TICKS_PER_SEC/2U); // every 1/2 sec
            m_screenTimeEvt.armX(BSP::TICKS_PER_SEC*20U, 0U); // in 20 sec

            m_blink_ctr = 0U; // init the blink counter
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Tunnel::SM::active::demo}
        case Q_EXIT_SIG: {
            m_blinkTimeEvt.disarm();
            m_screenTimeEvt.disarm();
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Tunnel::SM::active::demo::BLINK_TIMEOUT}
        case BLINK_TIMEOUT_SIG: {
            m_blink_ctr ^= 1U; /* toggle the blink counter */
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Tunnel::SM::active::demo::SCREEN_TIMEOUT}
        case SCREEN_TIMEOUT_SIG: {
            status_ = tran(&screen_saver);
            break;
        }
        //${AOs::Tunnel::SM::active::demo::TIME_TICK}
        case TIME_TICK_SIG: {
            advance();
            if (m_blink_ctr != 0U) {
                // add the text into the frame buffer
                BSP::paintString((GAME_TUNNEL_WIDTH - 10U*6U)/2U,
                                (GAME_TUNNEL_HEIGHT - 4U)/2U,
                                "Press BTN0");
            }
            BSP::updateScreen();
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Tunnel::SM::active::demo::PLAYER_TRIGGER}
        case PLAYER_TRIGGER_SIG: {
            status_ = tran(&playing);
            break;
        }
        default: {
            status_ = super(&active);
            break;
        }
    }
    return status_;
}

//${AOs::Tunnel::SM::active::playing} ........................................
Q_STATE_DEF(Tunnel, playing) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Tunnel::SM::active::playing}
        case Q_ENTRY_SIG: {
            static QP::QEvt const takeoff(TAKE_OFF_SIG);
            m_wall_gap = GAME_WALLS_GAP_Y;
            AO_Ship->POST(&takeoff, this); // post the TAKEOFF sig
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Tunnel::SM::active::playing}
        case Q_EXIT_SIG: {
            static QP::QEvt const recycle(MINE_RECYCLE_SIG);
            dispatchToAllMines(&recycle); // recycle all Mines
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Tunnel::SM::active::playing::TIME_TICK}
        case TIME_TICK_SIG: {
            // render this frame on the display
            BSP::updateScreen();
            advance();
            plantMine();
            dispatchToAllMines(e);
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Tunnel::SM::active::playing::SHIP_IMG}
        case SHIP_IMG_SIG: {
            uint8_t x   = Q_EVT_CAST(ObjectImageEvt)->x;
            int8_t  y   = Q_EVT_CAST(ObjectImageEvt)->y;
            uint8_t bmp = Q_EVT_CAST(ObjectImageEvt)->bmp;

            // did the Ship/Missile hit the tunnel wall?
            if (BSP::isWallHit(bmp, x, y)) {
                static QP::QEvt const hit(HIT_WALL_SIG);
                AO_Ship->POST(&hit, this);
            }
            BSP::paintBitmap(x, y, bmp);
            dispatchToAllMines(e); // let Mines check for hits
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Tunnel::SM::active::playing::MISSILE_IMG}
        case MISSILE_IMG_SIG: {
            uint8_t x   = Q_EVT_CAST(ObjectImageEvt)->x;
            int8_t  y   = Q_EVT_CAST(ObjectImageEvt)->y;
            uint8_t bmp = Q_EVT_CAST(ObjectImageEvt)->bmp;

            // did the Ship/Missile hit the tunnel wall?
            if (BSP::isWallHit(bmp, x, y)) {
                static QP::QEvt const hit(HIT_WALL_SIG);
                AO_Missile->POST(&hit, this);
            }
            BSP::paintBitmap(x, y, bmp);
            dispatchToAllMines(e); // let Mines check for hits
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Tunnel::SM::active::playing::MINE_IMG}
        case MINE_IMG_SIG: {
            BSP::paintBitmap(Q_EVT_CAST(ObjectImageEvt)->x,
                            Q_EVT_CAST(ObjectImageEvt)->y,
                            Q_EVT_CAST(ObjectImageEvt)->bmp);
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Tunnel::SM::active::playing::EXPLOSION}
        case EXPLOSION_SIG: {
            BSP::paintBitmap(Q_EVT_CAST(ObjectImageEvt)->x,
                            Q_EVT_CAST(ObjectImageEvt)->y,
                            Q_EVT_CAST(ObjectImageEvt)->bmp);
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Tunnel::SM::active::playing::SCORE}
        case SCORE_SIG: {
            BSP::updateScore(Q_EVT_CAST(ScoreEvt)->score);
            // increase difficulty of the game:
            // the tunnel gets narrower as the score goes up
            //
            m_wall_gap = (uint8_t)(GAME_WALLS_GAP_Y
                              - Q_EVT_CAST(ScoreEvt)->score/100U);
            if (m_wall_gap < GAME_WALLS_MIN_GAP_Y) {
                m_wall_gap = GAME_WALLS_MIN_GAP_Y;
            }
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Tunnel::SM::active::playing::GAME_OVER}
        case GAME_OVER_SIG: {
            BSP::clearWalls();
            BSP::updateScore(Q_EVT_CAST(ScoreEvt)->score);
            BSP::updateScreen();
            status_ = tran(&game_over);
            break;
        }
        default: {
            status_ = super(&active);
            break;
        }
    }
    return status_;
}

//${AOs::Tunnel::SM::active::game_over} ......................................
Q_STATE_DEF(Tunnel, game_over) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Tunnel::SM::active::game_over}
        case Q_ENTRY_SIG: {
            m_blinkTimeEvt.armX(BSP::TICKS_PER_SEC/2U,
                                    BSP::TICKS_PER_SEC/2U); // every 1/2 sec
            m_screenTimeEvt.armX(BSP::TICKS_PER_SEC*5U, 0U); // in 5 se
            m_blink_ctr = 0U;
            BSP::paintString((GAME_TUNNEL_WIDTH - 6U * 9U) / 2U,
                            (GAME_TUNNEL_HEIGHT / 2U) - 4U,
                            "Game Over");
            BSP::updateScreen();
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Tunnel::SM::active::game_over}
        case Q_EXIT_SIG: {
            m_blinkTimeEvt.disarm();
            m_screenTimeEvt.disarm();
            BSP::updateScore(0U); // update the score on the display
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Tunnel::SM::active::game_over::BLINK_TIMEOUT}
        case BLINK_TIMEOUT_SIG: {
            m_blink_ctr ^= 1U; // toggle the blink counter
            BSP::paintString((GAME_TUNNEL_WIDTH - 6U*9U) / 2U,
                            (GAME_TUNNEL_HEIGHT / 2U) - 4U,
                            ((m_blink_ctr == 0U)
                            ? "Game Over"
                            : "         "));
            BSP::updateScreen();
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Tunnel::SM::active::game_over::SCREEN_TIMEOUT}
        case SCREEN_TIMEOUT_SIG: {
            status_ = tran(&demo);
            break;
        }
        default: {
            status_ = super(&active);
            break;
        }
    }
    return status_;
}

//${AOs::Tunnel::SM::active::screen_saver} ...................................
Q_STATE_DEF(Tunnel, screen_saver) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Tunnel::SM::active::screen_saver::initial}
        case Q_INIT_SIG: {
            status_ = tran(&screen_saver_hide);
            break;
        }
        //${AOs::Tunnel::SM::active::screen_saver::PLAYER_TRIGGER}
        case PLAYER_TRIGGER_SIG: {
            status_ = tran(&demo);
            break;
        }
        default: {
            status_ = super(&active);
            break;
        }
    }
    return status_;
}

//${AOs::Tunnel::SM::active::screen_saver::screen_saver_hide} ................
Q_STATE_DEF(Tunnel, screen_saver_hide) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Tunnel::SM::active::screen_saver::screen_saver_hide}
        case Q_ENTRY_SIG: {
            BSP::displayOff(); // power down the display
            m_screenTimeEvt.armX(BSP::TICKS_PER_SEC*3U, 0U); // in 3 sec
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Tunnel::SM::active::screen_saver::screen_saver_hide}
        case Q_EXIT_SIG: {
            m_screenTimeEvt.disarm();
            BSP::displayOn(); // power up the display
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Tunnel::SM::active::screen_saver::screen_saver_hid~::SCREEN_TIMEOUT}
        case SCREEN_TIMEOUT_SIG: {
            status_ = tran(&screen_saver_show);
            break;
        }
        default: {
            status_ = super(&screen_saver);
            break;
        }
    }
    return status_;
}

//${AOs::Tunnel::SM::active::screen_saver::screen_saver_show} ................
Q_STATE_DEF(Tunnel, screen_saver_show) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Tunnel::SM::active::screen_saver::screen_saver_show}
        case Q_ENTRY_SIG: {
            BSP::clearFB(); // clear the screen frame buffer
            uint32_t rnd = BSP::random();
            BSP::paintString((uint8_t)(rnd % (GAME_TUNNEL_WIDTH - 10U*6U)),
                            (uint8_t) (rnd % (GAME_TUNNEL_HEIGHT - 8U)),
                            "Press BTN0");
            BSP::updateScreen();
            m_screenTimeEvt.armX(BSP::TICKS_PER_SEC/3U, 0U); // in 1/3 sec
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Tunnel::SM::active::screen_saver::screen_saver_show}
        case Q_EXIT_SIG: {
            m_screenTimeEvt.disarm();
            BSP::clearFB();
            BSP::updateScreen();
            status_ = Q_RET_HANDLED;
            break;
        }
        //${AOs::Tunnel::SM::active::screen_saver::screen_saver_sho~::SCREEN_TIMEOUT}
        case SCREEN_TIMEOUT_SIG: {
            status_ = tran(&screen_saver_hide);
            break;
        }
        default: {
            status_ = super(&screen_saver);
            break;
        }
    }
    return status_;
}

//${AOs::Tunnel::SM::final} ..................................................
Q_STATE_DEF(Tunnel, final) {
    QP::QState status_;
    switch (e->sig) {
        //${AOs::Tunnel::SM::final}
        case Q_ENTRY_SIG: {
            BSP::clearFB();
            BSP::updateScreen();
            QP::QF::stop(); // stop QF and cleanup
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

} // namespace GAME
//$enddef${AOs::Tunnel} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

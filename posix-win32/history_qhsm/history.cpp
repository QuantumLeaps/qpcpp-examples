//$file${.::history.cpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: history.qm
// File:  ${.::history.cpp}
//
// This code has been generated by QM 7.0.0 <www.state-machine.com/qm>.
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
//$endhead${.::history.cpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include "qpcpp.hpp"
#include "history.hpp"

#include "safe_std.h"   // portable "safe" <stdio.h>/<string.h> facilities
#include <stdlib.h>

Q_DEFINE_THIS_FILE

//$declare${SMs::ToastOven} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

//${SMs::ToastOven} ..........................................................
class ToastOven : public QP::QHsm {
public:
    ToastOven();

protected:
    Q_STATE_DECL(initial);
    Q_STATE_DECL(doorClosed);
    Q_STATE_DECL(heating);
    Q_STATE_DECL(toasting);
    Q_STATE_DECL(baking);
    Q_STATE_DECL(off);
    Q_STATE_DECL(doorOpen);
    Q_STATE_DECL(final);

protected:
    QP::QStateHandler hist_doorClosed;
    QP::QStateHandler hist_heating;
}; // class ToastOven
//$enddecl${SMs::ToastOven} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

static ToastOven l_oven; // the only instance of the ToastOven class

// global-scope definitions ------------------------------------
QP::QHsm * const the_oven = &l_oven;       // the opaque pointer

//$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// Check for the minimum required QP version
#if (QP_VERSION < 730U) || (QP_VERSION != ((QP_RELEASE^4294967295U)%0x2710U))
#error qpcpp version 7.3.0 or higher required
#endif
//$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$define${SMs::ToastOven} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

//${SMs::ToastOven} ..........................................................

//${SMs::ToastOven::ToastOven} ...............................................
ToastOven::ToastOven()
 : QHsm(Q_STATE_CAST(&ToastOven::initial))
{}

//${SMs::ToastOven::SM} ......................................................
Q_STATE_DEF(ToastOven, initial) {
    //${SMs::ToastOven::SM::initial}
    Q_UNUSED_PAR(e); // unused parameter
    // state history attributes
    hist_doorClosed = &off;
    hist_heating = &baking;
    return tran(&doorClosed);
}

//${SMs::ToastOven::SM::doorClosed} ..........................................
Q_STATE_DEF(ToastOven, doorClosed) {
    QP::QState status_;
    switch (e->sig) {
        //${SMs::ToastOven::SM::doorClosed}
        case Q_ENTRY_SIG: {
            PRINTF_S("%s;", "door-Closed");
            status_ = Q_RET_HANDLED;
            break;
        }
        //${SMs::ToastOven::SM::doorClosed}
        case Q_EXIT_SIG: {
            // save shallow history
            hist_doorClosed = childState(&doorClosed);
            status_ = Q_RET_HANDLED;
            break;
        }
        //${SMs::ToastOven::SM::doorClosed::initial}
        case Q_INIT_SIG: {
            status_ = tran(&off);
            break;
        }
        //${SMs::ToastOven::SM::doorClosed::TERMINATE}
        case TERMINATE_SIG: {
            status_ = tran(&final);
            break;
        }
        //${SMs::ToastOven::SM::doorClosed::OPEN}
        case OPEN_SIG: {
            status_ = tran(&doorOpen);
            break;
        }
        //${SMs::ToastOven::SM::doorClosed::TOAST}
        case TOAST_SIG: {
            status_ = tran(&toasting);
            break;
        }
        //${SMs::ToastOven::SM::doorClosed::BAKE}
        case BAKE_SIG: {
            status_ = tran(&baking);
            break;
        }
        //${SMs::ToastOven::SM::doorClosed::OFF}
        case OFF_SIG: {
            status_ = tran(&off);
            break;
        }
        default: {
            status_ = super(&top);
            break;
        }
    }
    return status_;
}

//${SMs::ToastOven::SM::doorClosed::heating} .................................
Q_STATE_DEF(ToastOven, heating) {
    QP::QState status_;
    switch (e->sig) {
        //${SMs::ToastOven::SM::doorClosed::heating}
        case Q_ENTRY_SIG: {
            PRINTF_S("%s;", "heater-On");
            status_ = Q_RET_HANDLED;
            break;
        }
        //${SMs::ToastOven::SM::doorClosed::heating}
        case Q_EXIT_SIG: {
            PRINTF_S("%s;", "heater-Off");
            // save deep history
            hist_heating = state();
            status_ = Q_RET_HANDLED;
            break;
        }
        //${SMs::ToastOven::SM::doorClosed::heating::initial}
        case Q_INIT_SIG: {
            PRINTF_S("%s;", "heating-init");
            status_ = tran(&toasting);
            break;
        }
        default: {
            status_ = super(&doorClosed);
            break;
        }
    }
    return status_;
}

//${SMs::ToastOven::SM::doorClosed::heating::toasting} .......................
Q_STATE_DEF(ToastOven, toasting) {
    QP::QState status_;
    switch (e->sig) {
        //${SMs::ToastOven::SM::doorClosed::heating::toasting}
        case Q_ENTRY_SIG: {
            PRINTF_S("%s;", "toasting");
            status_ = Q_RET_HANDLED;
            break;
        }
        default: {
            status_ = super(&heating);
            break;
        }
    }
    return status_;
}

//${SMs::ToastOven::SM::doorClosed::heating::baking} .........................
Q_STATE_DEF(ToastOven, baking) {
    QP::QState status_;
    switch (e->sig) {
        //${SMs::ToastOven::SM::doorClosed::heating::baking}
        case Q_ENTRY_SIG: {
            PRINTF_S("%s;", "baking");
            status_ = Q_RET_HANDLED;
            break;
        }
        default: {
            status_ = super(&heating);
            break;
        }
    }
    return status_;
}

//${SMs::ToastOven::SM::doorClosed::off} .....................................
Q_STATE_DEF(ToastOven, off) {
    QP::QState status_;
    switch (e->sig) {
        //${SMs::ToastOven::SM::doorClosed::off}
        case Q_ENTRY_SIG: {
            PRINTF_S("%s;", "toaster-Off");
            status_ = Q_RET_HANDLED;
            break;
        }
        default: {
            status_ = super(&doorClosed);
            break;
        }
    }
    return status_;
}

//${SMs::ToastOven::SM::doorOpen} ............................................
Q_STATE_DEF(ToastOven, doorOpen) {
    QP::QState status_;
    switch (e->sig) {
        //${SMs::ToastOven::SM::doorOpen}
        case Q_ENTRY_SIG: {
            PRINTF_S("%s;", "door-Open,lamp-On");
            status_ = Q_RET_HANDLED;
            break;
        }
        //${SMs::ToastOven::SM::doorOpen}
        case Q_EXIT_SIG: {
            PRINTF_S("%s;", "lamp-Off");
            status_ = Q_RET_HANDLED;
            break;
        }
        //${SMs::ToastOven::SM::doorOpen::CLOSE}
        case CLOSE_SIG: {
            status_ = tran_hist(hist_doorClosed);
            break;
        }
        //${SMs::ToastOven::SM::doorOpen::TERMINATE}
        case TERMINATE_SIG: {
            status_ = tran(&final);
            break;
        }
        //${SMs::ToastOven::SM::doorOpen::OFF}
        case OFF_SIG: {
            status_ = tran_hist(hist_heating);
            break;
        }
        default: {
            status_ = super(&top);
            break;
        }
    }
    return status_;
}

//${SMs::ToastOven::SM::final} ...............................................
Q_STATE_DEF(ToastOven, final) {
    QP::QState status_;
    switch (e->sig) {
        //${SMs::ToastOven::SM::final}
        case Q_ENTRY_SIG: {
            PRINTF_S("\n%s\n", "Bye! Bye!");
            QP::QF::onCleanup();
            exit(0);
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
//$enddef${SMs::ToastOven} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


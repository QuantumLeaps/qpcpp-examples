//$file${.::ttsched.cpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: real-time-ms-tt.qm
// File:  ${.::ttsched.cpp}
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
//$endhead${.::ttsched.cpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include "qpcpp.hpp"           // QP/C++ real-time event framework
#include "bsp.hpp"             // Board Support Package
#include "app.hpp"             // Application interface

//$declare${Components::TTSched} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${Components::TTSched} .....................................................
class TTSched : public QP::QMsm {
public:
    static TTSched inst;

public:
    TTSched();

protected:
    QM_STATE_DECL( initial);
    QM_STATE_DECL( active);
    QM_STATE_DECL( m0tick0);
    QM_STATE_DECL( m0tick1);
    QM_STATE_DECL( m0tick2);
    QM_STATE_DECL( m0tick3);
    QM_STATE_DECL( m1tick0);
    QM_STATE_DECL( m1tick1);
    QM_STATE_DECL( m1tick2);
    QM_STATE_DECL( m1tick3);
    QM_STATE_DECL( m1tick4);
    QM_STATE_DECL( m1tick5);
}; // class TTSched

} // namespace APP
//$enddecl${Components::TTSched} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Q_DEFINE_THIS_MODULE("ttsched")

static QP::QEvt const runEvt(APP::RUN_SIG);

//$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// Check for the minimum required QP version
#if (QP_VERSION < 730U) || (QP_VERSION != ((QP_RELEASE^4294967295U)%0x2710U))
#error qpcpp version 7.3.0 or higher required
#endif
//$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$define${Shared::TTSched_tick} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${Shared::TTSched_tick} ....................................................
void TTSched_tick() {
    static QP::QEvt const tickEvt(TICK_SIG);
    TTSched::inst.dispatch(&tickEvt, 0U);
}

} // namespace APP
//$enddef${Shared::TTSched_tick} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$define${Shared::TTSched_mode} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${Shared::TTSched_mode} ....................................................
void TTSched_mode(uint8_t mode) {
    static QP::QEvt const modeEvt[2] {
        QP::QEvt(MODE0_SIG),
        QP::QEvt(MODE1_SIG),
    };
    TTSched::inst.dispatch(&modeEvt[mode], 0U);
}

} // namespace APP
//$enddef${Shared::TTSched_mode} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$define${Components::TTSched} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${Components::TTSched} .....................................................
TTSched TTSched::inst;

//${Components::TTSched::TTSched} ............................................
TTSched::TTSched()
  : QMsm(Q_STATE_CAST(&TTSched::initial))
{
    init(0U);
}

//${Components::TTSched::SM} .................................................
QM_STATE_DEF(TTSched, initial) {
    //${Components::TTSched::SM::initial}
    static QP::QMTranActTable const tatbl_ = { // tran-action table
        &m0tick0_s, // target state
        {
            Q_ACTION_NULL // zero terminator
        }
    };
    return qm_tran_init(&tatbl_);
}

//${Components::TTSched::SM::active} .........................................
QP::QMState const TTSched::active_s = {
    QM_STATE_NULL, // superstate (top)
    &TTSched::active,
    Q_ACTION_NULL, // no entry action
    Q_ACTION_NULL, // no exit action
    Q_ACTION_NULL  // no initial tran.
};
//${Components::TTSched::SM::active}
QM_STATE_DEF(TTSched, active) {
    QP::QState status_;
    switch (e->sig) {
        //${Components::TTSched::SM::active::MODE0}
        case MODE0_SIG: {
            static QP::QMTranActTable const tatbl_ = { // tran-action table
                &m0tick0_s, // target state
                {
                    Q_ACTION_NULL // zero terminator
                }
            };
            status_ = qm_tran(&tatbl_);
            break;
        }
        //${Components::TTSched::SM::active::MODE1}
        case MODE1_SIG: {
            static QP::QMTranActTable const tatbl_ = { // tran-action table
                &m1tick0_s, // target state
                {
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

//${Components::TTSched::SM::active::m0tick0} ................................
QP::QMState const TTSched::m0tick0_s = {
    &TTSched::active_s, // superstate
    &TTSched::m0tick0,
    Q_ACTION_NULL, // no entry action
    Q_ACTION_NULL, // no exit action
    Q_ACTION_NULL  // no initial tran.
};
//${Components::TTSched::SM::active::m0tick0}
QM_STATE_DEF(TTSched, m0tick0) {
    QP::QState status_;
    switch (e->sig) {
        //${Components::TTSched::SM::active::m0tick0::TICK}
        case TICK_SIG: {
            AO_Periodic4->POST(&runEvt, 0U);
            static QP::QMTranActTable const tatbl_ = { // tran-action table
                &m0tick1_s, // target state
                {
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

//${Components::TTSched::SM::active::m0tick1} ................................
QP::QMState const TTSched::m0tick1_s = {
    &TTSched::active_s, // superstate
    &TTSched::m0tick1,
    Q_ACTION_NULL, // no entry action
    Q_ACTION_NULL, // no exit action
    Q_ACTION_NULL  // no initial tran.
};
//${Components::TTSched::SM::active::m0tick1}
QM_STATE_DEF(TTSched, m0tick1) {
    QP::QState status_;
    switch (e->sig) {
        //${Components::TTSched::SM::active::m0tick1::TICK}
        case TICK_SIG: {
            static QP::QMTranActTable const tatbl_ = { // tran-action table
                &m0tick2_s, // target state
                {
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

//${Components::TTSched::SM::active::m0tick2} ................................
QP::QMState const TTSched::m0tick2_s = {
    &TTSched::active_s, // superstate
    &TTSched::m0tick2,
    Q_ACTION_NULL, // no entry action
    Q_ACTION_NULL, // no exit action
    Q_ACTION_NULL  // no initial tran.
};
//${Components::TTSched::SM::active::m0tick2}
QM_STATE_DEF(TTSched, m0tick2) {
    QP::QState status_;
    switch (e->sig) {
        //${Components::TTSched::SM::active::m0tick2::TICK}
        case TICK_SIG: {
            AO_Periodic4->POST(&runEvt, 0U);
            static QP::QMTranActTable const tatbl_ = { // tran-action table
                &m0tick3_s, // target state
                {
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

//${Components::TTSched::SM::active::m0tick3} ................................
QP::QMState const TTSched::m0tick3_s = {
    &TTSched::active_s, // superstate
    &TTSched::m0tick3,
    Q_ACTION_NULL, // no entry action
    Q_ACTION_NULL, // no exit action
    Q_ACTION_NULL  // no initial tran.
};
//${Components::TTSched::SM::active::m0tick3}
QM_STATE_DEF(TTSched, m0tick3) {
    QP::QState status_;
    switch (e->sig) {
        //${Components::TTSched::SM::active::m0tick3::TICK}
        case TICK_SIG: {
            AO_Periodic1->POST(&runEvt, 0U);
            static QP::QMTranActTable const tatbl_ = { // tran-action table
                &m0tick0_s, // target state
                {
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

//${Components::TTSched::SM::active::m1tick0} ................................
QP::QMState const TTSched::m1tick0_s = {
    &TTSched::active_s, // superstate
    &TTSched::m1tick0,
    Q_ACTION_NULL, // no entry action
    Q_ACTION_NULL, // no exit action
    Q_ACTION_NULL  // no initial tran.
};
//${Components::TTSched::SM::active::m1tick0}
QM_STATE_DEF(TTSched, m1tick0) {
    QP::QState status_;
    switch (e->sig) {
        //${Components::TTSched::SM::active::m1tick0::TICK}
        case TICK_SIG: {
            AO_Periodic4->POST(&runEvt, 0U);
            AO_Sporadic3->POST(&runEvt, 0U);
            static QP::QMTranActTable const tatbl_ = { // tran-action table
                &m1tick1_s, // target state
                {
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

//${Components::TTSched::SM::active::m1tick1} ................................
QP::QMState const TTSched::m1tick1_s = {
    &TTSched::active_s, // superstate
    &TTSched::m1tick1,
    Q_ACTION_NULL, // no entry action
    Q_ACTION_NULL, // no exit action
    Q_ACTION_NULL  // no initial tran.
};
//${Components::TTSched::SM::active::m1tick1}
QM_STATE_DEF(TTSched, m1tick1) {
    QP::QState status_;
    switch (e->sig) {
        //${Components::TTSched::SM::active::m1tick1::TICK}
        case TICK_SIG: {
            AO_Periodic4->POST(&runEvt, 0U);
            AO_Sporadic3->POST(&runEvt, 0U);
            static QP::QMTranActTable const tatbl_ = { // tran-action table
                &m1tick2_s, // target state
                {
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

//${Components::TTSched::SM::active::m1tick2} ................................
QP::QMState const TTSched::m1tick2_s = {
    &TTSched::active_s, // superstate
    &TTSched::m1tick2,
    Q_ACTION_NULL, // no entry action
    Q_ACTION_NULL, // no exit action
    Q_ACTION_NULL  // no initial tran.
};
//${Components::TTSched::SM::active::m1tick2}
QM_STATE_DEF(TTSched, m1tick2) {
    QP::QState status_;
    switch (e->sig) {
        //${Components::TTSched::SM::active::m1tick2::TICK}
        case TICK_SIG: {
            AO_Periodic4->POST(&runEvt, 0U);
            AO_Sporadic2->POST(&runEvt, 0U);
            static QP::QMTranActTable const tatbl_ = { // tran-action table
                &m1tick3_s, // target state
                {
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

//${Components::TTSched::SM::active::m1tick3} ................................
QP::QMState const TTSched::m1tick3_s = {
    &TTSched::active_s, // superstate
    &TTSched::m1tick3,
    Q_ACTION_NULL, // no entry action
    Q_ACTION_NULL, // no exit action
    Q_ACTION_NULL  // no initial tran.
};
//${Components::TTSched::SM::active::m1tick3}
QM_STATE_DEF(TTSched, m1tick3) {
    QP::QState status_;
    switch (e->sig) {
        //${Components::TTSched::SM::active::m1tick3::TICK}
        case TICK_SIG: {
            AO_Periodic4->POST(&runEvt, 0U);
            AO_Sporadic3->POST(&runEvt, 0U);
            static QP::QMTranActTable const tatbl_ = { // tran-action table
                &m1tick4_s, // target state
                {
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

//${Components::TTSched::SM::active::m1tick4} ................................
QP::QMState const TTSched::m1tick4_s = {
    &TTSched::active_s, // superstate
    &TTSched::m1tick4,
    Q_ACTION_NULL, // no entry action
    Q_ACTION_NULL, // no exit action
    Q_ACTION_NULL  // no initial tran.
};
//${Components::TTSched::SM::active::m1tick4}
QM_STATE_DEF(TTSched, m1tick4) {
    QP::QState status_;
    switch (e->sig) {
        //${Components::TTSched::SM::active::m1tick4::TICK}
        case TICK_SIG: {
            AO_Periodic4->POST(&runEvt, 0U);
            AO_Periodic1->POST(&runEvt, 0U);
            static QP::QMTranActTable const tatbl_ = { // tran-action table
                &m1tick5_s, // target state
                {
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

//${Components::TTSched::SM::active::m1tick5} ................................
QP::QMState const TTSched::m1tick5_s = {
    &TTSched::active_s, // superstate
    &TTSched::m1tick5,
    Q_ACTION_NULL, // no entry action
    Q_ACTION_NULL, // no exit action
    Q_ACTION_NULL  // no initial tran.
};
//${Components::TTSched::SM::active::m1tick5}
QM_STATE_DEF(TTSched, m1tick5) {
    QP::QState status_;
    switch (e->sig) {
        //${Components::TTSched::SM::active::m1tick5::TICK}
        case TICK_SIG: {
            AO_Periodic4->POST(&runEvt, 0U);
            AO_Sporadic3->POST(&runEvt, 0U);
            static QP::QMTranActTable const tatbl_ = { // tran-action table
                &m1tick0_s, // target state
                {
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

} // namespace APP
//$enddef${Components::TTSched} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//============================================================================
// qhsmtst example
//
// Copyright (C) 2005-2026 Quantum Leaps, LLC. All rights reserved.
//
//                    Q u a n t u m  L e a P s
//                    ------------------------
//                    Modern Embedded Software
//
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-QL-commercial
//
// This software is dual-licensed under the terms of the open-source GNU
// General Public License, version 3 or later (GPL-3.0-or-later), or,
// alternatively, under the terms of one of the closed-source Quantum Leaps
// commercial licenses.
//
// Redistributions in source code must retain this top-level comment block.
//
// NOTE:
// The GPL does NOT permit distributing this code as part of proprietary
// (closed-source) programs. For such use, please contact Quantum Leaps for
// a commercial license, which is an alternative to the GPL designed
// explicitly for closed-source distribution.
//
// Quantum Leaps contact information:
// <www.state-machine.com/licensing>
// <info@state-machine.com>
//============================================================================
#include "qpcpp.hpp"
#include "bsp.hpp"
#include "qhsmtst.hpp"

namespace APP {

class QHsmTst : public QP::QActive {
public:
    static QHsmTst inst;

public:
    QHsmTst()
      : QActive(Q_STATE_CAST(&initial))
    {}

protected:
    Q_STATE_DECL(initial);
    Q_STATE_DECL(s1);
    Q_STATE_DECL(s2);
    Q_STATE_DECL(s3);
    Q_STATE_DECL(s4);
    Q_STATE_DECL(s5);
    Q_STATE_DECL(s6);
    Q_STATE_DECL(s7);
    Q_STATE_DECL(s8);
    Q_STATE_DECL(t);
}; // class QHsmTst

//............................................................................
QHsmTst QHsmTst::inst;
QP::QActive * const AO_QHsmTst = &QHsmTst::inst;

//............................................................................
Q_STATE_DEF(QHsmTst, initial) {
    Q_UNUSED_PAR(e);

    QS_OBJ_DICTIONARY(&QHsmTst::inst);

    QS_FUN_DICTIONARY(&QHsmTst::s1);
    QS_FUN_DICTIONARY(&QHsmTst::s2);
    QS_FUN_DICTIONARY(&QHsmTst::s3);
    QS_FUN_DICTIONARY(&QHsmTst::s4);
    QS_FUN_DICTIONARY(&QHsmTst::s5);
    QS_FUN_DICTIONARY(&QHsmTst::s6);
    QS_FUN_DICTIONARY(&QHsmTst::s7);
    QS_FUN_DICTIONARY(&QHsmTst::s8);
    QS_FUN_DICTIONARY(&QHsmTst::t);

    return tran(&t);
}
//............................................................................
Q_STATE_DEF(QHsmTst, s1) {
    QP::QState status_;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            status_ = Q_RET_HANDLED;
            break;
        }
        case Q_EXIT_SIG: {
            status_ = Q_RET_HANDLED;
            break;
        }
        case Q_INIT_SIG: {
            status_ = tran(&s7);
            break;
        }
        default: {
            status_ = super(&top);
            break;
        }
    }
    return status_;
}
//............................................................................
Q_STATE_DEF(QHsmTst, s2) {
    QP::QState status_;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            status_ = Q_RET_HANDLED;
            break;
        }
        case Q_EXIT_SIG: {
            status_ = Q_RET_HANDLED;
            break;
        }
        default: {
            status_ = super(&s1);
            break;
        }
    }
    return status_;
}
//............................................................................
Q_STATE_DEF(QHsmTst, s3) {
    QP::QState status_;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            status_ = Q_RET_HANDLED;
            break;
        }
        case Q_EXIT_SIG: {
            status_ = Q_RET_HANDLED;
            break;
        }
        default: {
            status_ = super(&s2);
            break;
        }
    }
    return status_;
}
//............................................................................
Q_STATE_DEF(QHsmTst, s4) {
    QP::QState status_;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            status_ = Q_RET_HANDLED;
            break;
        }
        case Q_EXIT_SIG: {
            status_ = Q_RET_HANDLED;
            break;
        }
        case Q_INIT_SIG: {
            status_ = tran(&s7);
            break;
        }
        default: {
            status_ = super(&s3);
            break;
        }
    }
    return status_;
}
//............................................................................
Q_STATE_DEF(QHsmTst, s5) {
    QP::QState status_;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            status_ = Q_RET_HANDLED;
            break;
        }
        case Q_EXIT_SIG: {
            status_ = Q_RET_HANDLED;
            break;
        }
        default: {
            status_ = super(&s4);
            break;
        }
    }
    return status_;
}
//............................................................................
Q_STATE_DEF(QHsmTst, s6) {
    QP::QState status_;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            status_ = Q_RET_HANDLED;
            break;
        }
        case Q_EXIT_SIG: {
            status_ = Q_RET_HANDLED;
            break;
        }
        default: {
            status_ = super(&s5);
            break;
        }
    }
    return status_;
}
//............................................................................
Q_STATE_DEF(QHsmTst, s7) {
    QP::QState status_;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            status_ = Q_RET_HANDLED;
            break;
        }
        case Q_EXIT_SIG: {
            status_ = Q_RET_HANDLED;
            break;
        }
        case I_SIG: {
            dispatch(e, 0U);
            status_ = Q_RET_HANDLED;
            break;
        }
        default: {
            status_ = super(&s6);
            break;
        }
    }
    return status_;
}
//............................................................................
Q_STATE_DEF(QHsmTst, s8) {
    QP::QState status_;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            status_ = Q_RET_HANDLED;
            break;
        }
        case Q_EXIT_SIG: {
            status_ = Q_RET_HANDLED;
            break;
        }
        default: {
            status_ = super(&s7);
            break;
        }
    }
    return status_;
}
//............................................................................
Q_STATE_DEF(QHsmTst, t) {
    QP::QState status_;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            status_ = Q_RET_HANDLED;
            break;
        }
        case Q_EXIT_SIG: {
            status_ = Q_RET_HANDLED;
            break;
        }
        case A_SIG: {
            status_ = tran(&s5);
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

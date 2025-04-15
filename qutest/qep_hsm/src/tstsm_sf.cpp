#include "qpcpp.hpp"     // QP/C++ framework
#include "tstsm.hpp"     // TstSM state machine
#include "tstsm_sf.hpp"  // TstSM state machine for Safety Functions

namespace {
Q_DEFINE_THIS_MODULE("tstsm_sf")
}

namespace APP {

//----------------------------------------------------------------------------
class TstSM0 : public QP::QHsm {
public:
    TstSM0() : QP::QHsm(0) {}
    static TstSM0 inst;
};

TstSM0 TstSM0::inst;
QP::QAsm * const the_TstSM0 = &TstSM0::inst;

//----------------------------------------------------------------------------
class TstSM1 : public QP::QHsm {
public:
    TstSM1() : QP::QHsm(Q_STATE_CAST(&TstSM1::initial)) {}
    static TstSM1 inst;

protected:
    Q_STATE_DECL(initial);
};

TstSM1 TstSM1::inst;
QP::QAsm * const the_TstSM1 = &TstSM1::inst;

Q_STATE_DEF(TstSM1, initial) {
    Q_UNUSED_PAR(e);
    return Q_RET_HANDLED; // missing transition
}

//----------------------------------------------------------------------------
class TstSM2 : public QP::QHsm {
public:
    TstSM2() : QP::QHsm(Q_STATE_CAST(&TstSM2::initial)) {}
    static TstSM2 inst;

protected:
    Q_STATE_DECL(initial);
    Q_STATE_DECL(s);
};

TstSM2 TstSM2::inst;
QP::QAsm * const the_TstSM2 = &TstSM2::inst;

Q_STATE_DEF(TstSM2, initial) {
    Q_UNUSED_PAR(e);
    return tran(&s);
}

Q_STATE_DEF(TstSM2, s) {
    QP::QState status_;
    switch (e->sig) {
        default: {
            status_ = super(&top);
            break;
        }
    }
    return status_;
}

//----------------------------------------------------------------------------
class TstSM3 : public QP::QHsm {
public:
    TstSM3() : QP::QHsm(Q_STATE_CAST(&TstSM3::initial)) {}
    static TstSM3 inst;

protected:
    Q_STATE_DECL(initial);
};

TstSM3 TstSM3::inst;
QP::QAsm * const the_TstSM3 = &TstSM3::inst;

Q_STATE_DEF(TstSM3, initial) {
    Q_UNUSED_PAR(e);
    return Q_RET_UNHANDLED; // missing transition
}

//----------------------------------------------------------------------------
class TstSM4 : public QP::QHsm {
public:
    TstSM4() : QP::QHsm(Q_STATE_CAST(&TstSM4::initial)) {}
    static TstSM4 inst;
protected:
    Q_STATE_DECL(initial);
    Q_STATE_DECL(s);
};

TstSM4 TstSM4::inst;
QP::QAsm * const the_TstSM4 = &TstSM4::inst;

Q_STATE_DEF(TstSM4, initial) {
    Q_UNUSED_PAR(e);
    return tran(&s);
}

Q_STATE_DEF(TstSM4, s) {
    QP::QState status_;
    switch (e->sig) {
        default: {
            status_ = super(&s); // circular nesting
            break;
        }
    }
    return status_;
}


//----------------------------------------------------------------------------
class TstSM5 : public QP::QHsm {
public:
    TstSM5() : QP::QHsm(Q_STATE_CAST(&TstSM5::initial)) {}
    static TstSM5 inst;

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

    Q_STATE_DECL(t1);
    Q_STATE_DECL(t2);
    Q_STATE_DECL(t3);
    Q_STATE_DECL(t4);
    Q_STATE_DECL(t5);

    Q_STATE_DECL(u1);
    Q_STATE_DECL(u2);

private:
    friend bool TstSM5_isIn(std::uint32_t const state_num);
    friend QP::QStateHandler TstSM5_child(std::uint32_t const  state_num);
};

TstSM5 TstSM5::inst;
QP::QAsm * const the_TstSM5 = &TstSM5::inst;

//............................................................................
Q_STATE_DEF(TstSM5, initial) {
    Q_UNUSED_PAR(e);

    QS_OBJ_DICTIONARY(&TstSM5::inst);

    QS_FUN_DICTIONARY(&TstSM5::s1);
    QS_FUN_DICTIONARY(&TstSM5::s2);
    QS_FUN_DICTIONARY(&TstSM5::s3);
    QS_FUN_DICTIONARY(&TstSM5::s4);
    QS_FUN_DICTIONARY(&TstSM5::s5);
    QS_FUN_DICTIONARY(&TstSM5::s6);
    QS_FUN_DICTIONARY(&TstSM5::s7);
    QS_FUN_DICTIONARY(&TstSM5::s8);

    QS_FUN_DICTIONARY(&TstSM5::t1);
    QS_FUN_DICTIONARY(&TstSM5::t2);
    QS_FUN_DICTIONARY(&TstSM5::t3);
    QS_FUN_DICTIONARY(&TstSM5::t4);
    QS_FUN_DICTIONARY(&TstSM5::t5);

    QS_FUN_DICTIONARY(&TstSM5::u1);
    QS_FUN_DICTIONARY(&TstSM5::u2);

    return tran(&s6);
}
//............................................................................
Q_STATE_DEF(TstSM5, s1) {
    QP::QState status_;
    switch (e->sig) {
        case Q_INIT_SIG: {
            status_ = tran(&s8);
            break;
        }
        case A_SIG: {
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
Q_STATE_DEF(TstSM5, s2) {
    QP::QState status_;
    switch (e->sig) {
        case Q_INIT_SIG: {
            status_ = tran(&s3);
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
Q_STATE_DEF(TstSM5, s3) {
    QP::QState status_;
    switch (e->sig) {
        case Q_INIT_SIG: {
            status_ = tran(&s4);
            break;
        }
        case C_SIG: {
            status_ = tran(&u2);
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
Q_STATE_DEF(TstSM5, s4) {
    QP::QState status_;
    switch (e->sig) {
        case Q_INIT_SIG: {
            status_ = tran(&s5);
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
Q_STATE_DEF(TstSM5, s5) {
    QP::QState status_;
    switch (e->sig) {
        case Q_INIT_SIG: {
            status_ = tran(&s6);
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
Q_STATE_DEF(TstSM5, s6) {
    QP::QState status_;
    switch (e->sig) {
        case Q_INIT_SIG: {
            status_ = tran(&s7);
            break;
        }
        case B_SIG: {
            status_ = tran(&s8);
            break;
        }
        case D_SIG: {
            dispatch(e, 0U); // RTC violation
            status_ = Q_RET_HANDLED;
            break;
        }
        case E_SIG: {
            status_ = tran(&t1);
            break;
        }
        case H_SIG: {
            status_ = tran(&u1);
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
Q_STATE_DEF(TstSM5, s7) {
    QP::QState status_;
    switch (e->sig) {
        case Q_INIT_SIG: {
            status_ = tran(&s8);
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
Q_STATE_DEF(TstSM5, s8) {
    QP::QState status_;
    switch (e->sig) {
        case F_SIG: {
            status_ = tran(&t5);
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
Q_STATE_DEF(TstSM5, t1) {
    QP::QState status_;
    switch (e->sig) {
        case E_SIG: {
            status_ = tran(&s7);
            break;
        }
        case F_SIG: {
            status_ = tran(&s1);
            break;
        }
        case G_SIG: {
            status_ = tran(&s2);
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
Q_STATE_DEF(TstSM5, t2) {
    QP::QState status_;
    switch (e->sig) {
        default: {
            status_ = super(&t1);
            break;
        }
    }
    return status_;
}
//............................................................................
Q_STATE_DEF(TstSM5, t3) {
    QP::QState status_;
    switch (e->sig) {
        default: {
            status_ = super(&t2);
            break;
        }
    }
    return status_;
}
//............................................................................
Q_STATE_DEF(TstSM5, t4) {
    QP::QState status_;
    switch (e->sig) {
        default: {
            status_ = super(&t3);
            break;
        }
    }
    return status_;
}
//............................................................................
Q_STATE_DEF(TstSM5, t5) {
    QP::QState status_;
    switch (e->sig) {
        case F_SIG: {
            status_ = tran(&s5);
            break;
        }
        default: {
            status_ = super(&t4);
            break;
        }
    }
    return status_;
}
//............................................................................
Q_STATE_DEF(TstSM5, u1) {
    QP::QState status_;
    switch (e->sig) {
        case Q_INIT_SIG: {
            status_ = tran(&u2);
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
Q_STATE_DEF(TstSM5, u2) {
    QP::QState status_;
    switch (e->sig) {
        default: {
            status_ = Q_RET_HANDLED; // should be super()
            break;
        }
    }
    return status_;
}
//............................................................................
bool TstSM5_isIn(std::uint32_t const state_num) {
    bool stat = false;
    switch (state_num) {
        case 1:
            stat = the_TstSM5->isIn(Q_STATE_CAST(&TstSM5::s1));
            break;
        case 2:
            stat = the_TstSM5->isIn(Q_STATE_CAST(&TstSM5::s2));
            break;
        case 3:
            stat = the_TstSM5->isIn(Q_STATE_CAST(&TstSM5::s3));
            break;
        default:
            Q_ERROR();
    }
    return stat;
}
//............................................................................
QP::QStateHandler TstSM5_child(std::uint32_t const  state_num) {
    QP::QStateHandler child;
    switch (state_num) {
        case 1:
            child = TstSM5::inst.childState(Q_STATE_CAST(&TstSM5::s1));
            break;
        case 2:
            child = TstSM5::inst.childState(Q_STATE_CAST(&TstSM5::s2));
            break;
        case 3:
            child = TstSM5::inst.childState(Q_STATE_CAST(&TstSM5::s3));
            break;
        default:
            Q_ERROR();
    }
    return child;
}

} // namespace APP

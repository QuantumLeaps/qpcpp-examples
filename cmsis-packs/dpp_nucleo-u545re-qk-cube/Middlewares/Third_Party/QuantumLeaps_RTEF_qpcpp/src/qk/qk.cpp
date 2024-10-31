//$file${src::qk::qk.cpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: qpcpp.qm
// File:  ${src::qk::qk.cpp}
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
//$endhead${src::qk::qk.cpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#define QP_IMPL             // this is QP implementation
#include "qp_port.hpp"      // QP port
#include "qp_pkg.hpp"       // QP package-scope interface
#include "qsafe.h"          // QP Functional Safety (FuSa) Subsystem
#ifdef Q_SPY                // QS software tracing enabled?
    #include "qs_port.hpp"  // QS port
    #include "qs_pkg.hpp"   // QS facilities for pre-defined trace records
#else
    #include "qs_dummy.hpp" // disable the QS software tracing
#endif // Q_SPY

// protection against including this source file in a wrong project
#ifndef QK_HPP_
    #error "Source file included in a project NOT based on the QK kernel"
#endif // QK_HPP_

// unnamed namespace for local definitions with internal linkage
namespace {
Q_DEFINE_THIS_MODULE("qk")
} // unnamed namespace

//$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// Check for the minimum required QP version
#if (QP_VERSION < 730U) || (QP_VERSION != ((QP_RELEASE^4294967295U)%0x2710U))
#error qpcpp version 7.3.0 or higher required
#endif
//$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$define${QK::QK-base} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace QP {
namespace QK {

//${QK::QK-base::schedLock} ..................................................
QSchedStatus schedLock(std::uint_fast8_t const ceiling) noexcept {
    QF_CRIT_STAT
    QF_CRIT_ENTRY();
    QF_MEM_SYS();

    Q_REQUIRE_INCRIT(100, !QK_ISR_CONTEXT_());
    Q_INVARIANT_INCRIT(102, QK_priv_.lockCeil
        == static_cast<std::uint_fast8_t>(~QK_priv_.lockCeil_dis));

    // first store the previous lock prio
    QSchedStatus stat;
    if (ceiling > QK_priv_.lockCeil) { // raising the lock ceiling?
        QS_BEGIN_PRE(QS_SCHED_LOCK, QK_priv_.actPrio)
            QS_TIME_PRE();   // timestamp
            // the previous lock ceiling & new lock ceiling
            QS_2U8_PRE(static_cast<std::uint8_t>(QK_priv_.lockCeil),
                        static_cast<std::uint8_t>(ceiling));
        QS_END_PRE()

        // previous status of the lock
        stat = static_cast<QSchedStatus>(QK_priv_.lockCeil);

        // new status of the lock
        QK_priv_.lockCeil = ceiling;
    #ifndef Q_UNSAFE
        QK_priv_.lockCeil_dis = static_cast<std::uint_fast8_t>(~ceiling);
    #endif
    }
    else {
        stat = 0xFFU; // scheduler not locked
    }

    QF_MEM_APP();
    QF_CRIT_EXIT();

    return stat; // return the status to be saved in a stack variable
}

//${QK::QK-base::schedUnlock} ................................................
void schedUnlock(QSchedStatus const prevCeil) noexcept {
    // has the scheduler been actually locked by the last QK::schedLock()?
    if (prevCeil != 0xFFU) {
        QF_CRIT_STAT
        QF_CRIT_ENTRY();
        QF_MEM_SYS();

        Q_INVARIANT_INCRIT(202, QK_priv_.lockCeil
            == static_cast<std::uint_fast8_t>(~QK_priv_.lockCeil_dis));
        Q_REQUIRE_INCRIT(210, (!QK_ISR_CONTEXT_())
                              && (QK_priv_.lockCeil > prevCeil));

        QS_BEGIN_PRE(QS_SCHED_UNLOCK, QK_priv_.actPrio)
            QS_TIME_PRE(); // timestamp
            // current lock ceiling (old), previous lock ceiling (new)
            QS_2U8_PRE(static_cast<std::uint8_t>(QK_priv_.lockCeil),
                        static_cast<std::uint8_t>(prevCeil));
        QS_END_PRE()

        // restore the previous lock ceiling
        QK_priv_.lockCeil = prevCeil;
    #ifndef Q_UNSAFE
        QK_priv_.lockCeil_dis = static_cast<std::uint_fast8_t>(~prevCeil);
    #endif

        // find if any AOs should be run after unlocking the scheduler
        if (QK_sched_() != 0U) { // preemption needed?
            QK_activate_(); // activate any unlocked AOs
        }

        QF_MEM_APP();
        QF_CRIT_EXIT();
    }
}

} // namespace QK
} // namespace QP
//$enddef${QK::QK-base} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

extern "C" {
//$define${QK-extern-C} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

//${QK-extern-C::QK_priv_} ...................................................
QK_Attr QK_priv_;

//${QK-extern-C::QK_sched_} ..................................................
std::uint_fast8_t QK_sched_() noexcept {
    // NOTE: this function is entered with interrupts DISABLED

    Q_INVARIANT_INCRIT(402,
        QK_priv_.readySet.verify_(&QK_priv_.readySet_dis));

    std::uint_fast8_t p;
    if (QK_priv_.readySet.isEmpty()) {
        p = 0U; // no activation needed
    }
    else {
        // find the highest-prio AO with non-empty event queue
        p = QK_priv_.readySet.findMax();

        Q_INVARIANT_INCRIT(412, QK_priv_.actThre
            == static_cast<std::uint_fast8_t>(~QK_priv_.actThre_dis));

        // is the AO's prio. below the active preemption-threshold?
        if (p <= QK_priv_.actThre) {
            p = 0U; // no activation needed
        }
        else {
            Q_INVARIANT_INCRIT(422, QK_priv_.lockCeil
                == static_cast<std::uint_fast8_t>(~QK_priv_.lockCeil_dis));

            // is the AO's prio. below the lock-ceiling?
            if (p <= QK_priv_.lockCeil) {
                p = 0U; // no activation needed
            }
            else {
                Q_INVARIANT_INCRIT(432, QK_priv_.nextPrio
                    == static_cast<std::uint_fast8_t>(~QK_priv_.nextPrio_dis));
                QK_priv_.nextPrio = p; // next AO to run
    #ifndef Q_UNSAFE
                QK_priv_.nextPrio_dis
                    = static_cast<std::uint_fast8_t>(~QK_priv_.nextPrio);
    #endif
            }
        }
    }

    return p;
}

//${QK-extern-C::QK_activate_} ...............................................
void QK_activate_() noexcept {
    // NOTE: this function is entered with interrupts DISABLED

    std::uint_fast8_t const prio_in = QK_priv_.actPrio; // save initial prio.
    std::uint_fast8_t p = QK_priv_.nextPrio; // next prio to run

    Q_INVARIANT_INCRIT(502,
       (prio_in == static_cast<std::uint_fast8_t>(~QK_priv_.actPrio_dis))
       && (p == static_cast<std::uint_fast8_t>(~QK_priv_.nextPrio_dis)));
    Q_REQUIRE_INCRIT(510, (prio_in <= QF_MAX_ACTIVE)
       && (0U < p) && (p <= QF_MAX_ACTIVE));

    #if (defined QF_ON_CONTEXT_SW) || (defined Q_SPY)
    std::uint_fast8_t pprev = prio_in;
    #endif // QF_ON_CONTEXT_SW || Q_SPY

    QK_priv_.nextPrio = 0U; // clear for the next time
    #ifndef Q_UNSAFE
    QK_priv_.nextPrio_dis = static_cast<std::uint_fast8_t>(~QK_priv_.nextPrio);
    #endif

    std::uint_fast8_t pthre_in;
    QP::QActive *a;
    if (prio_in == 0U) { // preempting the idle thread?
        pthre_in = 0U;
    }
    else {
        a = QP::QActive::registry_[prio_in];
        Q_ASSERT_INCRIT(510, a != nullptr);

        pthre_in = static_cast<std::uint_fast8_t>(a->getPThre());
        Q_INVARIANT_INCRIT(511, pthre_in == static_cast<std::uint_fast8_t>(
            ~static_cast<std::uint_fast8_t>(a->m_pthre_dis) & 0xFFU));
    }

    // loop until no more ready-to-run AOs of higher pthre than the initial
    do  {
        a = QP::QActive::registry_[p]; // obtain the pointer to the AO
        Q_ASSERT_INCRIT(520, a != nullptr); // the AO must be registered
        std::uint_fast8_t const pthre
            = static_cast<std::uint_fast8_t>(a->getPThre());
        Q_INVARIANT_INCRIT(522, pthre == static_cast<std::uint_fast8_t>(
            ~static_cast<std::uint_fast8_t>(a->m_pthre_dis) & 0xFFU));

        // set new active prio. and preemption-threshold
        QK_priv_.actPrio = p;
        QK_priv_.actThre = pthre;
    #ifndef Q_UNSAFE
        QK_priv_.actPrio_dis = static_cast<std::uint_fast8_t>(~p);
        QK_priv_.actThre_dis = static_cast<std::uint_fast8_t>(~pthre);
    #endif

    #if (defined QF_ON_CONTEXT_SW) || (defined Q_SPY)
        if (p != pprev) { // changing threads?

            QS_BEGIN_PRE(QP::QS_SCHED_NEXT, p)
                QS_TIME_PRE();     // timestamp
                QS_2U8_PRE(p,      // prio. of the scheduled AO
                            pprev); // previous prio.
            QS_END_PRE()

    #ifdef QF_ON_CONTEXT_SW
            QF_onContextSw(QP::QActive::registry_[pprev], a);
    #endif // QF_ON_CONTEXT_SW

            pprev = p; // update previous prio.
        }
    #endif // QF_ON_CONTEXT_SW || Q_SPY

        QF_MEM_APP();
        QF_INT_ENABLE(); // unconditionally enable interrupts

        QP::QEvt const * const e = a->get_();
        // NOTE QActive_get_() performs QF_MEM_APP() before return

        // dispatch event (virtual call)
        a->dispatch(e, a->getPrio());
    #if (QF_MAX_EPOOL > 0U)
        QP::QF::gc(e);
    #endif

        // determine the next highest-prio. AO ready to run...
        QF_INT_DISABLE(); // unconditionally disable interrupts
        QF_MEM_SYS();

        // internal integrity check (duplicate inverse storage)
        Q_INVARIANT_INCRIT(532,
            QK_priv_.readySet.verify_(&QK_priv_.readySet_dis));

        if (a->getEQueue().isEmpty()) { // empty queue?
            QK_priv_.readySet.remove(p);
    #ifndef Q_UNSAFE
            QK_priv_.readySet.update_(&QK_priv_.readySet_dis);
    #endif
        }

        if (QK_priv_.readySet.isEmpty()) {
            p = 0U; // no activation needed
        }
        else {
            // find new highest-prio AO ready to run...
            p = QK_priv_.readySet.findMax();

            // is the new prio. below the initial preemption-threshold?
            if (p <= pthre_in) {
                p = 0U; // no activation needed
            }
            else {
                Q_INVARIANT_INCRIT(542,
                    QK_priv_.lockCeil == ~QK_priv_.lockCeil_dis);

                // is the AO's prio. below the lock preemption-threshold?
                if (p <= QK_priv_.lockCeil) {
                    p = 0U; // no activation needed
                }
                else {
                    Q_ASSERT_INCRIT(550, p <= QF_MAX_ACTIVE);
                }
            }
        }
    } while (p != 0U);

    // restore the active prio. and preemption-threshold
    QK_priv_.actPrio = prio_in;
    QK_priv_.actThre = pthre_in;
    #ifndef Q_UNSAFE
    QK_priv_.actPrio_dis = static_cast<std::uint_fast8_t>(~QK_priv_.actPrio);
    QK_priv_.actThre_dis = static_cast<std::uint_fast8_t>(~QK_priv_.actThre);
    #endif

    #if (defined QF_ON_CONTEXT_SW) || (defined Q_SPY)
    if (prio_in != 0U) { // resuming an active object?
        a = QP::QActive::registry_[prio_in]; // pointer to preempted AO

        QS_BEGIN_PRE(QP::QS_SCHED_NEXT, prio_in)
            QS_TIME_PRE();     // timestamp
            // prio. of the resumed AO, previous prio.
            QS_2U8_PRE(prio_in, pprev);
        QS_END_PRE()
    }
    else {  // resuming prio.==0 --> idle
        a = nullptr; // QK idle loop

        QS_BEGIN_PRE(QP::QS_SCHED_IDLE, pprev)
            QS_TIME_PRE();     // timestamp
            QS_U8_PRE(pprev);  // previous prio.
        QS_END_PRE()
    }

    #ifdef QF_ON_CONTEXT_SW
    QF_onContextSw(QP::QActive::registry_[pprev], a);
    #endif // QF_ON_CONTEXT_SW

    #endif // QF_ON_CONTEXT_SW || Q_SPY
}
//$enddef${QK-extern-C} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
} // extern "C"

//$define${QK::QF-cust} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace QP {
namespace QF {

//${QK::QF-cust::init} .......................................................
void init() {
    bzero_(&QF::priv_,                 sizeof(QF::priv_));
    bzero_(&QK_priv_,                  sizeof(QK_priv_));
    bzero_(&QActive::registry_[0],     sizeof(QActive::registry_));

    // setup the QK scheduler as initially locked and not running
    QK_priv_.lockCeil = (QF_MAX_ACTIVE + 1U); // scheduler locked

    #ifndef Q_UNSAFE
    QK_priv_.readySet.update_(&QK_priv_.readySet_dis);
    QK_priv_.actPrio_dis  = static_cast<std::uint_fast8_t>(~QK_priv_.actPrio);
    QK_priv_.nextPrio_dis = static_cast<std::uint_fast8_t>(~QK_priv_.nextPrio);
    QK_priv_.actThre_dis  = static_cast<std::uint_fast8_t>(~QK_priv_.actThre);
    QK_priv_.lockCeil_dis = static_cast<std::uint_fast8_t>(~QK_priv_.lockCeil);
    #endif

    #ifdef QK_INIT
    QK_INIT(); // port-specific initialization of the QK kernel
    #endif
}

//${QK::QF-cust::stop} .......................................................
void stop() {
    onCleanup();  // cleanup callback
    // nothing else to do for the QK preemptive kernel
}

//${QK::QF-cust::run} ........................................................
int_t run() {
    #ifdef Q_SPY
    // produce the QS_QF_RUN trace record
    QF_INT_DISABLE();
    QF_MEM_SYS();
    QS::beginRec_(QS_REC_NUM_(QS_QF_RUN));
    QS::endRec_();
    QF_MEM_APP();
    QF_INT_ENABLE();
    #endif // Q_SPY

    onStartup(); // application-specific startup callback

    QF_INT_DISABLE();
    QF_MEM_SYS();

    #ifdef QK_START
    QK_START(); // port-specific startup of the QK kernel
    #endif

    QK_priv_.lockCeil = 0U; // unlock the QK scheduler
    #ifndef Q_UNSAFE
    QK_priv_.lockCeil_dis = static_cast<std::uint_fast8_t>(~QK_priv_.lockCeil);
    #endif

    #ifdef QF_ON_CONTEXT_SW
    // officially switch to the idle context
    QF_onContextSw(nullptr, QActive::registry_[QK_priv_.nextPrio]);
    #endif

    // activate AOs to process events posted so far
    if (QK_sched_() != 0U) {
        QK_activate_();
    }

    QF_MEM_APP();
    QF_INT_ENABLE();

    for (;;) { // QK idle loop...
        QK::onIdle(); // application-specific QK on-idle callback
    }

    #ifdef __GNUC__  // GNU compiler?
    return 0;
    #endif
}

} // namespace QF
} // namespace QP
//$enddef${QK::QF-cust} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//$define${QK::QActive} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace QP {

//${QK::QActive} .............................................................

//${QK::QActive::start} ......................................................
void QActive::start(
    QPrioSpec const prioSpec,
    QEvtPtr * const qSto,
    std::uint_fast16_t const qLen,
    void * const stkSto,
    std::uint_fast16_t const stkSize,
    void const * const par)
{
    Q_UNUSED_PAR(stkSto);  // not needed in QK
    Q_UNUSED_PAR(stkSize); // not needed in QK

    QF_CRIT_STAT
    QF_CRIT_ENTRY();
    QF_MEM_SYS();

    Q_REQUIRE_INCRIT(300, (!QK_ISR_CONTEXT_())
                     && (stkSto == nullptr));
    QF_MEM_APP();
    QF_CRIT_EXIT();

    m_prio  = static_cast<std::uint8_t>(prioSpec & 0xFFU); // QF-prio.
    m_pthre = static_cast<std::uint8_t>(prioSpec >> 8U); // preemption-thre.
    register_(); // make QF aware of this AO

    m_eQueue.init(qSto, qLen); // init the built-in queue

    // top-most initial tran. (virtual call)
    this->init(par, m_prio);
    QS_FLUSH(); // flush the trace buffer to the host

    // See if this AO needs to be scheduled if QK is already running
    QF_CRIT_ENTRY();
    QF_MEM_SYS();
    if (QK_sched_() != 0U) { // activation needed?
        QK_activate_();
    }
    QF_MEM_APP();
    QF_CRIT_EXIT();
}

} // namespace QP
//$enddef${QK::QActive} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
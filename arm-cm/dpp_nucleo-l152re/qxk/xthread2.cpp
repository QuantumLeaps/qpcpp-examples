//============================================================================
// QXThread example
//
// Copyright (C) 2005 Quantum Leaps, LLC. All rights reserved.
//
//                    Q u a n t u m  L e a P s
//                    ------------------------
//                    Modern Embedded Software
//
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-QL-commercial
//
// This software is dual-licensed under the terms of the open-source GNU
// General Public License (GPL) or under the terms of one of the closed-
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
//============================================================================
#include "qpcpp.hpp"        // QP/C++ real-time event framework
#include "bsp.hpp"          // Board Support Package
#include "app.hpp"          // Application
//----------------------------------------------------------------------------
// unnamed namespace for local definitions with internal linkage
namespace {
//Q_DEFINE_THIS_FILE
} // unnamed namespace

//----------------------------------------------------------------------------
namespace APP {

//............................................................................
QP::QXSemaphore TH_sema;
QP::QXMutex TH_mutex;

//............................................................................
class XThread2 : public QP::QXThread {
private:
    // NOTE: data needed by this thread should be members of
    // the thread class. That way they are in the memory region
    // accessible from this thread.
    std::uint8_t m_foo;

public:
    static XThread2 inst;
    XThread2();

private:
    static void run(QP::QXThread * const thr);
}; // class XThread2

//............................................................................
QP::QXThread * const TH_XThread2 = &XThread2::inst;

XThread2 XThread2::inst;

XThread2::XThread2()
  : QXThread(&run)
{}

//............................................................................
void XThread2::run(QP::QXThread * const thr) {
    // downcast the generic thr pointer to the specific thread
    //auto me = static_cast<XThread2 *>(thr);

    QS_OBJ_DICTIONARY(TH_XThread2);
    QS_OBJ_DICTIONARY(TH_XThread2->getTimeEvt());
    QS_OBJ_DICTIONARY(&TH_sema);
    QS_OBJ_DICTIONARY(&TH_mutex);

    // initialize the semaphore before using it
    // NOTE: Here the semaphore is initialized in the highest-priority thread
    // that uses it. Alternatively, the semaphore can be initialized
    // before any thread runs.
    TH_sema.init(0U,  // count==0 (signaling semaphore)
                1U); // max_count==1 (binary semaphore)

    // initialize the mutex before using it
    // NOTE: Here the mutex is initialized in the highest-priority thread
    // that uses it. Alternatively, the mutex can be initialized
    // before any thread runs.
    TH_mutex.init(APP::N_PHILO + 6U); // priority-ceiling mutex
    //l_mutex.init(0U); // alternatively: priority-ceiling NOT used

    for (;;) {
        // wait on a semaphore (BLOCK indefinitely)
        TH_sema.wait();

        TH_mutex.lock(QP::QXTHREAD_NO_TIMEOUT); // lock the mutex
        QP::QXThread::delay(5U);  // wait more (BLOCK)
        TH_mutex.unlock();
    }
}

} // namespace APP

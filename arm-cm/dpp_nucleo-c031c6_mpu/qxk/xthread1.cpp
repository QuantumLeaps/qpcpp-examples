//$file${qxk::xthread1.cpp} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
//
// Model: dpp_mpu.qm
// File:  ${qxk::xthread1.cpp}
//
// This code has been generated by QM 5.3.0 <www.state-machine.com/qm>.
// DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
//
// SPDX-License-Identifier: GPL-3.0-or-later
//
// This generated code is open source software: you can redistribute it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation.
//
// This code is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.
//
// NOTE:
// Alternatively, this generated code may be distributed under the terms
// of Quantum Leaps commercial licenses, which expressly supersede the GNU
// General Public License and are specifically designed for licensees
// interested in retaining the proprietary status of their code.
//
// Contact information:
// <www.state-machine.com/licensing>
// <info@state-machine.com>
//
//$endhead${qxk::xthread1.cpp} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#include "qpcpp.hpp"             // QP/C++ real-time embedded framework
#include "dpp.hpp"               // DPP Application interface
#include "bsp.hpp"               // Board Support Package

//----------------------------------------------------------------------------
// unnamed namespace for local definitions with internal linkage
namespace {
Q_DEFINE_THIS_FILE
} // unnamed namespace
//----------------------------------------------------------------------------
//$skip${QP_VERSION} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// Check for the minimum required QP version
#if (QP_VERSION < 730U) || (QP_VERSION != ((QP_RELEASE^4294967295U) % 0x3E8U))
#error qpcpp version 7.3.0 or higher required
#endif
//$endskip${QP_VERSION} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//$define${Shared-TH::TH_sema} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${Shared-TH::TH_sema} ......................................................
QP::QXSemaphore TH_sema;

} // namespace APP
//$enddef${Shared-TH::TH_sema} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$define${Shared-TH::TH_mutex} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${Shared-TH::TH_mutex} .....................................................
QP::QXMutex TH_mutex;

} // namespace APP
//$enddef${Shared-TH::TH_mutex} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

//$declare${XThreads::XThread1} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${XThreads::XThread1} ......................................................
class XThread1 : public QP::QXThread {
private:

    // NOTE: data needed by this thread should be members of
    // the thread class. That way they are in the memory region
    // accessible from this thread.
    std::uint8_t m_foo;

public:
    XThread1();

private:
    static void run(QP::QXThread * const thr);
}; // class XThread1

} // namespace APP
//$enddecl${XThreads::XThread1} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$define${Shared-TH::XThread1_ctor} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${Shared-TH::XThread1_ctor} ................................................
void XThread1_ctor(
    std::uint8_t * const sto,
    std::uint32_t const size,
    void const * const mpu)
{
    Q_REQUIRE(sizeof(XThread1) <= size);

    // run the constructor through placement new()
    auto me = new(sto) XThread1();
    me->setThread(mpu);
}

} // namespace APP
//$enddef${Shared-TH::XThread1_ctor} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$define${Shared-TH::TH_obj_dict} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${Shared-TH::TH_obj_dict} ..................................................
#ifdef Q_SPY
void TH_obj_dict() {
    QS_OBJ_DICTIONARY(TH_XThread1);
    QS_OBJ_DICTIONARY(TH_XThread1->getTimeEvt());

    QS_OBJ_DICTIONARY(TH_XThread2);
    QS_OBJ_DICTIONARY(TH_XThread2->getTimeEvt());
    QS_OBJ_DICTIONARY(&TH_sema);
    QS_OBJ_DICTIONARY(&TH_mutex);
}
#endif // def Q_SPY

} // namespace APP
//$enddef${Shared-TH::TH_obj_dict} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//$define${XThreads::XThread1} vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
namespace APP {

//${XThreads::XThread1} ......................................................

//${XThreads::XThread1::XThread1} ............................................
XThread1::XThread1()
  : QXThread(&run)
{}

//${XThreads::XThread1::run} .................................................
void XThread1::run(QP::QXThread * const thr) {
    // downcast the generic thr pointer to the specific thread
    auto me = static_cast<XThread1 *>(thr);

    // subscribe to the EAT signal (from the application)
    me->subscribe(APP::EAT_SIG);

    for (;;) {
        QP::QEvt const *e = me->queueGet(BSP::TICKS_PER_SEC/4U);
        if (e) {
            TH_sema.signal(); // signal Thread2
            QP::QF::gc(e); // must explicitly recycle the received event!
        }

        TH_mutex.lock(QP::QXTHREAD_NO_TIMEOUT); // lock the mutex
        BSP::ledOn();
        if (TH_mutex.tryLock()) { // exercise the mutex
            // some floating point code to exercise the VFP...
            float volatile x = 1.4142135F;
            x = x * 1.4142135F;
            QP::QXThread::delay(10U);  // BLOCK while holding a mutex
            TH_mutex.unlock();
        }
        TH_mutex.unlock();
        BSP::ledOff();
    }
}

} // namespace APP
//$enddef${XThreads::XThread1} ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

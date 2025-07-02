//============================================================================
// Product: System test fixture for QK on the EFM32 target
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
#include "qpcpp.hpp"
#include "bsp.hpp"

#ifdef Q_HOST
#error "This integration test is not supported on the HOST"
#endif

namespace {

Q_DEFINE_THIS_FILE

//============================================================================
// AO ObjB
enum { NUM_B = 3 };

//............................................................................
// AO ObjB
class ObjB : public QP::QActive {
public:
    static ObjB inst[NUM_B];

public:
    ObjB() : QActive(&initial) {}

protected:
    Q_STATE_DECL(initial);
    Q_STATE_DECL(active);
}; // class ObjB

Q_STATE_DEF(ObjB, initial) {
    static bool registered = false; // starts off with 0, per C-standard
    if (!registered) {
        registered = true;
        QS_FUN_DICTIONARY(&ObjB::initial);
        QS_FUN_DICTIONARY(&ObjB::active);
    }
    subscribe(TEST1_SIG);
    subscribe(TEST2_SIG);
    return tran(&active);
}

Q_STATE_DEF(ObjB, active) {
    QP::QState status_;
    switch (e->sig) {
        case TEST0_SIG: {
            BSP::trace(this, "TEST0 1of2");
            BSP::trigISR();
            BSP::trace(this, "TEST0 2of2");
            status_ = Q_RET_HANDLED;
            break;
        }
        case TEST1_SIG: {
            static QP::QEvt const t2(TEST2_SIG);
            BSP::trace(this, "TEST1 1of2");
            QActive::PUBLISH(&t2, this);
            BSP::trace(this, "TEST1 2of2");
            status_ = Q_RET_HANDLED;
            break;
        }
        case TEST2_SIG: {
            BSP::trace(this, "TEST2 1of1");
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

//============================================================================
enum UserCommands {
    MEM_READ, MEM_WRITE,
    ROM_READ, ROM_WRITE,
    RAM_READ, RAM_WRITE,
};

ObjB ObjB::inst[NUM_B];

} // unnamed namespace

//============================================================================
int main() {

    QP::QF::init();  // initialize the framework and the underlying QXK kernel
    BSP::init(); // initialize the Board Support Package

    // initialize publish-subscribe...
    static QP::QSubscrList subscrSto[MAX_PUB_SIG];
    QP::QActive::psInit(subscrSto, Q_DIM(subscrSto));

    // initialize event pools...
    static QF_MPOOL_EL(QP::QEvt) smlPoolSto[10]; // small pool
    QP::QF::poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

    // dictionaries
    QS_SIG_DICTIONARY(TEST0_SIG,  nullptr);
    QS_SIG_DICTIONARY(TEST1_SIG,  nullptr);
    QS_SIG_DICTIONARY(TEST2_SIG,  nullptr);
    QS_SIG_DICTIONARY(TEST3_SIG,  nullptr);

    QS_ENUM_DICTIONARY(MEM_READ,  QP::QS_CMD);
    QS_ENUM_DICTIONARY(MEM_WRITE, QP::QS_CMD);
    QS_ENUM_DICTIONARY(ROM_READ,  QP::QS_CMD);
    QS_ENUM_DICTIONARY(ROM_WRITE, QP::QS_CMD);
    QS_ENUM_DICTIONARY(RAM_READ,  QP::QS_CMD);
    QS_ENUM_DICTIONARY(RAM_WRITE, QP::QS_CMD);

    for (std::uint8_t n = 0U; n < NUM_B; ++n) {
        QS_OBJ_ARR_DICTIONARY(&ObjB::inst[n], n);
    }

    // priority specifications for ObjBs...
    static QP::QPrioSpec pspecB[NUM_B];
    QS_OBJ_DICTIONARY(pspecB);

    // pause execution of the test and wait for the test script to continue
    // NOTE:
    // this pause gives the test-script a chance to poke pspecB and pspecX
    // variables to start the threads with the desired prio-specifications.
    QS_TEST_PAUSE();

    static QP::QEvt const *aoB_queueSto[NUM_B][10];
    for (std::uint8_t n = 0U; n < NUM_B; ++n) {
        if (pspecB[n] != 0U) {
            ObjB::inst[n].start(pspecB[n],       // QF-prio/p-thre.
                         aoB_queueSto[n],         // event queue storage
                         Q_DIM(aoB_queueSto[n]),  // event length [events]
                         nullptr,                 // no stack storage
                         0U);                     // zero stack size [bytes]
        }
    }

    return QP::QF::run(); // run the QF application
}

//============================================================================
namespace QP {

void QS::onTestSetup(void) {
}
//............................................................................
void QS::onTestTeardown(void) {
}
//............................................................................
//! callback function to execute user commands
void QS::onCommand(uint8_t cmdId,
                  uint32_t param1, uint32_t param2, uint32_t param3)
{
    uint32_t volatile value;

    switch (cmdId) {
        case MEM_READ: { // read MEM (can trip the MPU)
            value = *(uint32_t volatile *)(param1 + param2);
            QS_BEGIN_ID(QS_USER, 0U)
                QS_ENUM(QP::QS_CMD, cmdId);
                QS_U32(0, value);
            QS_END()
            break;
        }
        case MEM_WRITE: { // write MEM (can trip the MPU)
            *(uint32_t volatile *)(param1 + param2) = param3;
            QS_BEGIN_ID(QS_USER, 0U)
                QS_ENUM(QP::QS_CMD, cmdId);
                QS_U32(QS_HEX_FMT , param1);
                QS_U32(QS_HEX_FMT , param2);
                QS_U32(0 , param3);
            QS_END()
            break;
        }
        case ROM_READ: { // read ROM (can trip the MPU)
            value = BSP::romRead((int32_t)param1, param2);
            QS_BEGIN_ID(QS_USER, 0U)
                QS_ENUM(QP::QS_CMD, cmdId);
                QS_U32(0, value);
            QS_END()
            break;
        }
        case ROM_WRITE: { // write ROM (can trip the MPU)
            BSP::romWrite(param1, param2, param3);
            QS_BEGIN_ID(QS_USER, 0U)
                QS_ENUM(QP::QS_CMD, cmdId);
                QS_U32(QS_HEX_FMT , param1);
                QS_U32(QS_HEX_FMT , param2);
                QS_U32(0 , param3);
            QS_END()
            break;
        }
        case RAM_READ: { // read RAM (can trip the MPU)
            value = BSP::ramRead(param1, param2);
            QS_BEGIN_ID(QS_USER, 0U)
                QS_ENUM(QP::QS_CMD, cmdId);
                QS_U32(0, value);
            QS_END()
            break;
        }
        case RAM_WRITE: { // write RAM (can trip the MPU)
            BSP::ramWrite(param1, param2, param3);
            QS_BEGIN_ID(QS_USER, 0U)
                QS_ENUM(QP::QS_CMD, cmdId);
                QS_U32(QS_HEX_FMT , param1);
                QS_U32(QS_HEX_FMT , param2);
                QS_U32(0, param3);
            QS_END()
            break;
        }
        default: {
            break;
        }
    }
}

//============================================================================
//! Host callback function to "massage" the event, if necessary
void QS::onTestEvt(QEvt *e) {
    Q_UNUSED_PAR(e);
}
//............................................................................
//! callback function to output the posted QP events (not used here)
void QS::onTestPost(void const *sender, QActive *recipient,
                   QEvt const *e, bool status)
{
    Q_UNUSED_PAR(sender);
    Q_UNUSED_PAR(status);
}

} // namespace QP

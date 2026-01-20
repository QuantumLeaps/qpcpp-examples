//============================================================================
// Example, Zephyr RTOS kernel
//
// Copyright (C) 2005 Quantum Leaps, LLC. All rights reserved.
//
//                   Q u a n t u m  L e a P s
//                   ------------------------
//                   Modern Embedded Software
//
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-QL-commercial
//
// This software is dual-licensed under the terms of the open source GNU
// General Public License version 3 (or any later version), or alternatively,
// under the terms of one of the closed source Quantum Leaps commercial
// licenses.
//
// The terms of the open source GNU General Public License version 3
// can be found at: <www.gnu.org/licenses/gpl-3.0>
//
// The terms of the closed source Quantum Leaps commercial licenses
// can be found at: <www.state-machine.com/licensing>
//
// Redistributions in source code must retain this top-level comment block.
// Plagiarizing this software to sidestep the license obligations is illegal.
//
// Quantum Leaps contact information:
// <www.state-machine.com/licensing>
// <info@state-machine.com>
//============================================================================
#include "qpcpp.hpp"             // QP/C++ real-time event framework
#include "blinky.hpp"            // Blinky Application interface
#include "bsp.hpp"               // Board Support Package

#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/reboot.h>
// add other drivers if necessary...

// The devicetree node identifier for the "led0" alias.
#define LED0_NODE DT_ALIAS(led0)

#ifdef Q_SPY
    #error Simple Blinky Application does not provide Spy build configuration
#endif

// Local-scope objects -----------------------------------------------------
namespace { // unnamed local namespace

Q_DEFINE_THIS_FILE // define the name of this file for assertions

static struct gpio_dt_spec const l_led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static struct k_timer zephyr_tick_timer;

} // unnamed local namespace

//============================================================================
// Error handler

extern "C" {

Q_NORETURN Q_onError(char const * const module, int_t const id) {
    // NOTE: this implementation of the assertion handler is intended only
    // for debugging and MUST be changed for deployment of the application
    // (assuming that you ship your production code with assertions enabled).
    Q_UNUSED_PAR(module);
    Q_UNUSED_PAR(id);
    QS_ASSERTION(module, id, 10000U); // report assertion to QS
    Q_PRINTK("\nERROR in %s:%d\n", module, id);

#ifndef NDEBUG
    k_panic(); // debug build: halt the system for error search...
#endif

    sys_reboot(SYS_REBOOT_COLD); // release build: reboot the system
    for (;;) { // explicitly "no-return"
    }
}
//............................................................................
// assertion failure handler for the STM32 library, including the startup code
void assert_failed(char const * const module, int_t const id); // prototype
void assert_failed(char const * const module, int_t const id) {
    Q_onError(module, id);
}

//............................................................................
static void zephyr_tick_function(struct k_timer *tid); // prototype
static void zephyr_tick_function(struct k_timer *tid) {
    Q_UNUSED_PAR(tid);

    QP::QTimeEvt::TICK_X(0U, &timerID);
}

} // extern "C"

//============================================================================
namespace BSP {

void init() {
    int ret = gpio_pin_configure_dt(&l_led0, GPIO_OUTPUT_ACTIVE);
    Q_ASSERT(ret >= 0);

    k_timer_init(&zephyr_tick_timer, &zephyr_tick_function, nullptr);
}
//............................................................................
void start() {
    // initialize event pools
    static QF_MPOOL_EL(QP::QEvt) smlPoolSto[10];
    QP::QF::poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

    // initialize publish-subscribe
    static QP::QSubscrList subscrSto[APP::MAX_PUB_SIG];
    QP::QActive::psInit(subscrSto, Q_DIM(subscrSto));

    // start AOs/threads...

    static QP::QEvtPtr blinkyQueueSto[10];
    static K_THREAD_STACK_DEFINE(blinkyStack, 512);
    APP::AO_Blinky->start(
        1U,                       // QP prio. of the AO
        blinkyQueueSto,           // event queue storage
        Q_DIM(blinkyQueueSto),    // queue length [events]
        blinkyStack,              // private stack for embOS
        K_THREAD_STACK_SIZEOF(blinkyStack), // stack size [Zephyr]
        nullptr);                 // no initialization param
}
//............................................................................
void ledOn() {
    gpio_pin_set_dt(&l_led0, true);
    Q_PRINTK("BSP_ledOn\n");
}
//............................................................................
void ledOff() {
    gpio_pin_set_dt(&l_led0, false);
    Q_PRINTK("BSP_ledOff\n");
}

} // namespace BSP

//============================================================================
namespace QP {

// QF callbacks...

//............................................................................
void QF::onStartup() {
    k_timer_start(&zephyr_tick_timer, K_MSEC(1), K_MSEC(1));
    Q_PRINTK("QF::onStartup\n");
}
//............................................................................
void QF::onCleanup() {
    Q_PRINTK("QF::onCleanup\n");
}

} // namespace QP


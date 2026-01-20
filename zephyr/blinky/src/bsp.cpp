//============================================================================
// Example, Zephyr RTOS kernel
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

#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/reboot.h>
// add other drivers if necessary...

// Local-scope defines -------------------------------------------------------
// The devicetree node identifier for the "led0" alias.
#define LED0_NODE DT_ALIAS(led0)

//============================================================================
namespace { // unnamed namespace for local stuff with internal linkage

Q_DEFINE_THIS_FILE  // file name for assertions

static struct gpio_dt_spec const l_led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static struct k_timer zephyr_tick_timer;
#ifdef Q_SPY
    enum AppRecords { // application-specific trace records
        LED_STAT = QP::QS_USER,
    };

    // QSpy source IDs
    static QP::QSpyId const timerID = { QP::QS_ID_AP };
#endif // Q_SPY

} // unnamed namespace

//============================================================================
// Error handler

extern "C" {

Q_NORETURN Q_onError(char const * const module, int_t const id) {
    // NOTE: this implementation of the error handler is intended only
    // for debugging and MUST be changed for deployment of the application.
    Q_UNUSED_PAR(module);
    Q_UNUSED_PAR(id);
    QS_ASSERTION(module, id, 10000U); // report assertion to QS

#ifndef NDEBUG
    Q_PRINTK("\nERROR in %s:%d\n", module, id);
    k_panic(); // debug build: halt the system for error search...
#else
    sys_reboot(SYS_REBOOT_COLD); // release build: reboot the system
#endif
    for (;;) { // explicitly "no-return"
    }
}
//............................................................................
// Zephyr error handler redirecting to the QP error handler
void k_sys_fatal_error_handler(unsigned int reason,
    const struct arch_esf *esf)
{
    Q_UNUSED_PAR(esf);
    Q_onError("zephyr", reason);
}

//............................................................................
static void zephyr_tick_function(struct k_timer *tid); // prototype
static void zephyr_tick_function(struct k_timer *tid) {
    Q_UNUSED_PAR(tid);

    QP::QTimeEvt::TICK_X(0U, &timerID); // process time events at rate 0
}

} // extern "C"

//============================================================================
namespace BSP {

void init(void const * const arg) {
    Q_UNUSED_PAR(arg);

    int ret = gpio_pin_configure_dt(&l_led0, GPIO_OUTPUT_ACTIVE);
    Q_ASSERT(ret >= 0);

    k_timer_init(&zephyr_tick_timer, &zephyr_tick_function, nullptr);

    // initialize QS software tracing...
    if (!QS_INIT(arg)) {
        Q_ERROR();
    }

    // QS dictionaries...
    QS_OBJ_DICTIONARY(&timerID);
    QS_SIG_DICTIONARY(APP::TIMEOUT_SIG, nullptr);
    QS_USR_DICTIONARY(LED_STAT);

    // setup the QS filters...
    QS_GLB_FILTER(QP::QS_GRP_ALL);  // enable all QS trace records
    QS_GLB_FILTER(-QP::QS_QF_TICK); // exclude the tick record

    // mutable events not used -- no need to call QP::QF::poolInit()
    // publish-subscribe not used -- no need to call QP::QActive::psInit()
}
//............................................................................
void ledOn() {
    gpio_pin_set_dt(&l_led0, true);
    Q_PRINTK("BSP_ledOn\n");
    // application-specific trace record
    QS_BEGIN_ID(LED_STAT, APP::AO_Blinky->getPrio())
        QS_STR("ON"); // LED status
    QS_END()
}
//............................................................................
void ledOff() {
    gpio_pin_set_dt(&l_led0, false);
    Q_PRINTK("BSP_ledOff\n");
    // application-specific trace record
    QS_BEGIN_ID(LED_STAT, APP::AO_Blinky->getPrio())
        QS_STR("OFF"); // LED status
    QS_END()
}

} // namespace BSP

//============================================================================
namespace QP {

// QF callbacks...
void QF::onStartup() {
    // start AOs...
    static QEvtPtr blinkyQueueSto[10];
    static K_THREAD_STACK_DEFINE(blinkyStack, 512);
    APP::AO_Blinky->start(
        1U,                       // QP prio. of the AO
        blinkyQueueSto,           // event queue storage
        Q_DIM(blinkyQueueSto),    // queue length [events]
        blinkyStack,              // private stack for embOS
        K_THREAD_STACK_SIZEOF(blinkyStack)); // stack size [Zephyr]

    k_timer_start(&zephyr_tick_timer, K_MSEC(1), K_MSEC(1));
    Q_PRINTK("QF::onStartup\n");
}
//............................................................................
void QF::onCleanup() {
    Q_PRINTK("QF::onCleanup\n");
    QS_EXIT();
}

// QS callbacks ==============================================================
#ifdef Q_SPY

#include <zephyr/drivers/uart.h>

// select the Zephyr shell UART
// NOTE: you can change this to other UART peripheral if desired
static struct device const *uart_dev =
     DEVICE_DT_GET(DT_CHOSEN(zephyr_shell_uart));

//............................................................................
static void uart_cb(const struct device *dev, void *user_data) {
    if (!uart_irq_update(uart_dev)) {
        return;
    }

    if (uart_irq_rx_ready(uart_dev)) {
        std::uint8_t buf[32];
        int n = uart_fifo_read(uart_dev, buf, sizeof(buf));
        for (std::uint8_t const *p = buf; n > 0; --n, ++p) {
            QS::rxPut(*p);
        }
    }
}
//............................................................................
bool QS::onStartup(void const *arg) {
    Q_UNUSED_PAR(arg);

    Q_REQUIRE(uart_dev != nullptr);

    static std::uint8_t qsTxBuf[2*1024]; // buffer for QS-TX channel
    initBuf(qsTxBuf, sizeof(qsTxBuf));

    static std::uint8_t qsRxBuf[128];  // buffer for QS-RX channel
    rxInitBuf(qsRxBuf, sizeof(qsRxBuf));

    // configure interrupt and callback to receive data
    uart_irq_callback_user_data_set(uart_dev, &uart_cb, nullptr);
    uart_irq_rx_enable(uart_dev);

    return true; // return success
}
//............................................................................
void QS::onCleanup(void) {
}
//............................................................................
QSTimeCtr QS::onGetTime(void) {  // NOTE: invoked with interrupts DISABLED
    return k_cycle_get_32();
}
//............................................................................
// NOTE:
// No critical section in QS::onFlush() to avoid nesting of critical sections
// in case QS::onFlush() is called from Q_onError().
void QS::onFlush(void) {
    std::uint16_t len = 0xFFFFU; // to get as many bytes as available
    std::uint8_t const *buf;
    while ((buf = getBlock(&len)) != nullptr) { // QS-TX data available?
        for (; len != 0U; --len, ++buf) {
            uart_poll_out(uart_dev, *buf);
        }
        len = 0xFFFFU; // to get as many bytes as available
    }
}
//............................................................................
void QS::onReset() {
    sys_reboot(SYS_REBOOT_COLD);
}
//............................................................................
void QS::onCommand(std::uint8_t cmdId, std::uint32_t param1,
    std::uint32_t param2, std::uint32_t param3)
{
    Q_UNUSED_PAR(cmdId);
    Q_UNUSED_PAR(param1);
    Q_UNUSED_PAR(param2);
    Q_UNUSED_PAR(param3);
}
//............................................................................
void QF::onIdle() {
    QS::rxParse();  // parse all the received bytes

    std::uint16_t len = 0xFFFFU; // big number to get all available bytes

    QF_CRIT_STAT
    QF_CRIT_ENTRY();
    std::uint8_t const *buf = QS::getBlock(&len);
    QF_CRIT_EXIT();

    // transmit the bytes via the UART...
    for (; len != 0U; --len, ++buf) {
        uart_poll_out(uart_dev, *buf);
    }
}

#endif // Q_SPY

} // namespace QP

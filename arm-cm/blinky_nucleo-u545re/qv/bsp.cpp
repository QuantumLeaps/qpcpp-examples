//============================================================================
// Blinky example, NUCLEO-U545RE-Q board, QV kernel
//
// Copyright (C) 2005 Quantum Leaps, LLC. All rights reserved.
//
//                    Q u a n t u m  L e a P s
//                    ------------------------
//                    Modern Embedded Software
//
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-QL-commercial
//
// The QP/C software is dual-licensed under the terms of the open-source GNU
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
#include "qpcpp.hpp"             // QP/C++ real-time embedded framework
#include "blinky.hpp"            // Blinky Application interface
#include "bsp.hpp"               // Board Support Package

#include "stm32u545xx.h"  // CMSIS-compliant header file for the MCU used
// add other drivers if necessary...

//============================================================================
namespace { // unnamed namespace for local stuff with internal linkage

Q_DEFINE_THIS_FILE

// Local-scope objects -------------------------------------------------------
// LED pins available on the board (just one user LED LD2--Green on PA.5)
constexpr std::uint32_t LD2_PIN     {5U};

// Button pins available on the board (just one user Button B1 on PC.13)
constexpr std::uint32_t B1_PIN      {13U};

// macros from STM32Cube LL:
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)    ((REG) & (BIT))
#define CLEAR_REG(REG)        ((REG) = (0x0))
#define WRITE_REG(REG, VAL)   ((REG) = (VAL))
#define READ_REG(REG)         ((REG))
#define MODIFY_REG(REG, CLEARMASK, SETMASK) \
    WRITE_REG((REG), ((READ_REG(REG) & (~(CLEARMASK))) | (SETMASK)))

} // unnamed namespace

//============================================================================
// Error handler and ISRs...

extern "C" {

Q_NORETURN Q_onError(char const * const module, int_t const id) {
    // NOTE: this implementation of the assertion handler is intended only
    // for debugging and MUST be changed for deployment of the application
    // (assuming that you ship your production code with assertions enabled).
    Q_UNUSED_PAR(module);
    Q_UNUSED_PAR(id);
    QS_ASSERTION(module, id, 10000U);

#ifndef NDEBUG
    // light up the user LED
    GPIOA->BSRR = (1U << LD2_PIN);  // turn LED on
    // for debugging, hang on in an endless loop...
    for (;;) {
    }
#endif

    NVIC_SystemReset();
}
//............................................................................
// assertion failure handler for the STM32 library, including the startup code
void assert_failed(char const * const module, int_t const id); // prototype
void assert_failed(char const * const module, int_t const id) {
    Q_onError(module, id);
}

// ISRs used in the application ============================================

void SysTick_Handler(void); // prototype
void SysTick_Handler(void) {

    QP::QTimeEvt::TICK_X(0U, nullptr); // process time events at rate 0

    QV_ARM_ERRATUM_838869();
}

} // extern "C"


//============================================================================
namespace BSP {

static void STM32U545RE_MPU_setup(void) {
    // Set Attr 0
    ARM_MPU_SetMemAttr(0UL,
        ARM_MPU_ATTR(     // Normal memory
            // Outer Write-Through non-transient
            ARM_MPU_ATTR_MEMORY_(1UL, 0UL, 1UL, 0UL),
            // Inner Write-Through non-transient
            ARM_MPU_ATTR_MEMORY_(1UL, 0UL, 1UL, 0UL))
    );

    MPU->RNR = 0U; // region 0 (for ROM: read-only, can-execute)
    MPU->RBAR = ARM_MPU_RBAR(0x08000000U,
        ARM_MPU_SH_NON,
        ARM_MPU_AP_RO,
        ARM_MPU_AP_PO,
        ARM_MPU_EX);
    MPU->RLAR = ARM_MPU_RLAR(0x0807FFFFU, 0U);

    MPU->RNR = 1U; // region 0 (for RAM1: read-write, execute-never)
    MPU->RBAR = ARM_MPU_RBAR(0x20000000U,
        ARM_MPU_SH_OUTER,
        ARM_MPU_AP_RW,
        ARM_MPU_AP_PO,
        ARM_MPU_XN);
    MPU->RLAR = ARM_MPU_RLAR(0x2003FFFFU, 0U);

    MPU->RNR = 2U; // region 0 (for RAM2: read-write, execute-never)
    MPU->RBAR = ARM_MPU_RBAR(0x28000000U,
        ARM_MPU_SH_OUTER,
        ARM_MPU_AP_RW,
        ARM_MPU_AP_PO,
        ARM_MPU_XN);
    MPU->RLAR = ARM_MPU_RLAR(0x28003FFFU, 0U);

    // Enable MPU with all region definitions
    __DMB();
    MPU->CTRL = MPU_CTRL_PRIVDEFENA_Msk | MPU_CTRL_HFNMIENA_Msk | MPU_CTRL_ENABLE_Msk;

    // Enable MemManage Faults
    SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
    __DSB();
    __ISB();
}
//..........................................................................
void init() {
    // setup the MPU...
    STM32U545RE_MPU_setup();

    // initialize I-CACHE
    MODIFY_REG(ICACHE->CR, ICACHE_CR_WAYSEL, 0U); // 1-way
    SET_BIT(ICACHE->CR, ICACHE_CR_EN); // enable

    // flash prefetch buffer enable
    SET_BIT(FLASH->ACR, FLASH_ACR_PRFTEN);

    // enable PWR clock interface
    SET_BIT(RCC->AHB3ENR, RCC_AHB3ENR_PWREN);

    // NOTE: SystemInit() has been already called from the startup code
    // but SystemCoreClock needs to be updated
    SystemCoreClockUpdate();

    // enable GPIOA clock port for the LED LD4
    RCC->AHB2ENR1 |= RCC_AHB2ENR1_GPIOAEN;

    // set all used GPIOA pins as push-pull output, no pull-up, pull-down
    MODIFY_REG(GPIOA->OSPEEDR,
               GPIO_OSPEEDR_OSPEED0 << (LD2_PIN * GPIO_OSPEEDR_OSPEED1_Pos),
               1U << (LD2_PIN * GPIO_OSPEEDR_OSPEED1_Pos)); // speed==1
    MODIFY_REG(GPIOA->OTYPER,
               1U << LD2_PIN,
               0U << LD2_PIN); // output
    MODIFY_REG(GPIOA->PUPDR,
               GPIO_PUPDR_PUPD0 << (LD2_PIN * GPIO_PUPDR_PUPD1_Pos),
               0U << (LD2_PIN * GPIO_PUPDR_PUPD1_Pos)); // PUSHPULL
    MODIFY_REG(GPIOA->MODER,
               GPIO_MODER_MODE0 << (LD2_PIN * GPIO_MODER_MODE1_Pos),
               1U << (LD2_PIN * GPIO_MODER_MODE1_Pos)); // MODE_1

    // enable GPIOC clock port for the Button B1
    RCC->AHB2ENR1 |=  RCC_AHB2ENR1_GPIOCEN;

    // configure Button B1 pin on GPIOC as input, no pull-up, pull-down
    MODIFY_REG(GPIOC->PUPDR,
               GPIO_PUPDR_PUPD0 << (B1_PIN * GPIO_PUPDR_PUPD1_Pos),
               0U << (B1_PIN * GPIO_PUPDR_PUPD1_Pos)); // NO PULL
    MODIFY_REG(GPIOC->MODER,
               GPIO_MODER_MODE0 << (B1_PIN * GPIO_MODER_MODE1_Pos),
               0U << (B1_PIN * GPIO_MODER_MODE1_Pos)); // MODE_0
}
//............................................................................
void start() {
    // initialize event pools
    static QF_MPOOL_EL(QP::QEvt) smlPoolSto[20];
    QP::QF::poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

    // initialize publish-subscribe
    static QP::QSubscrList subscrSto[APP::MAX_PUB_SIG];
    QP::QActive::psInit(subscrSto, Q_DIM(subscrSto));

    // instantiate and start AOs/threads...

    static QP::QEvtPtr blinkyQueueSto[10];
    APP::AO_Blinky->start(
        1U,                         // QP prio. of the AO
        blinkyQueueSto,              // event queue storage
        Q_DIM(blinkyQueueSto),       // queue length [events]
        nullptr, 0U,                 // no stack storage
        nullptr);                    // no initialization param
}
//............................................................................
void ledOn() {
    GPIOA->BSRR = (1U << LD2_PIN);  // turn LED on
}
//............................................................................
void ledOff() {
    GPIOA->BRR = (1U << LD2_PIN);  // turn LED off
}
//............................................................................
void terminate(int16_t result) {
    Q_UNUSED_PAR(result);
}

} // namespace BSP

//============================================================================
namespace QP {

// QF callbacks...
void QF::onStartup() {
    // set up the SysTick timer to fire at BSP::TICKS_PER_SEC rate
    SysTick_Config(SystemCoreClock / BSP::TICKS_PER_SEC);

    // assign all priority bits for preemption-prio. and none to sub-prio.
    // NOTE: this might have been changed by STM32Cube.
    NVIC_SetPriorityGrouping(0U);

    // set priorities of ALL ISRs used in the system, see NOTE1
    NVIC_SetPriority(SysTick_IRQn,   QF_AWARE_ISR_CMSIS_PRI + 1U);
    // ...

    // enable IRQs...
}
//............................................................................
void QF::onCleanup() {
}
//............................................................................
void QV::onIdle() { // called with interrupts DISABLED, see NOTE01

    // toggle an LED on and then off (not enough LEDs, see NOTE2)
    //GPIOA->BSRR = (1U << LD2_PIN); // turn LED[n] on
    //GPIOA->BRR  = (1U << LD2_PIN); // turn LED[n] off

#ifdef Q_SPY
#elif defined NDEBUG
    // Put the CPU and peripherals to the low-power mode.
    // you might need to customize the clock management for your application,
    // see the datasheet for your particular Cortex-M MCU.
    //
    QV_CPU_SLEEP();  // atomically go to sleep and enable interrupts
#else
    QF_INT_ENABLE(); // just enable interrupts
#endif
}


} // namespace QP

//============================================================================
// NOTE1:
// The QF_AWARE_ISR_CMSIS_PRI constant from the QF port specifies the highest
// ISR priority that is disabled by the QF framework. The value is suitable
// for the NVIC_SetPriority() CMSIS function.
//
// Only ISRs prioritized at or below the QF_AWARE_ISR_CMSIS_PRI level (i.e.,
// with the numerical values of priorities equal or higher than
// QF_AWARE_ISR_CMSIS_PRI) are allowed to call the QV_ISR_ENTRY/
// QV_ISR_ENTRY macros or any other QF/QV services. These ISRs are
// "QF-aware".
//
// Conversely, any ISRs prioritized above the QF_AWARE_ISR_CMSIS_PRI priority
// level (i.e., with the numerical values of priorities less than
// QF_AWARE_ISR_CMSIS_PRI) are never disabled and are not aware of the kernel.
// Such "QF-unaware" ISRs cannot call ANY QF/QV services. In particular they
// can NOT call the macros QV_ISR_ENTRY/QV_ISR_ENTRY. The only mechanism
// by which a "QF-unaware" ISR can communicate with the QF framework is by
// triggering a "QF-aware" ISR, which can post/publish events.
//
// NOTE2:
// The User LED is used to visualize the idle loop activity. The brightness
// of the LED is proportional to the frequency of the idle loop.
// Please note that the LED is toggled with interrupts locked, so no interrupt
// execution time contributes to the brightness of the User LED.
//

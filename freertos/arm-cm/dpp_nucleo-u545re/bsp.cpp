//============================================================================
// Product: DPP example, NUCLEO-U545RE board, FreeRTOS kernel
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
#include "qpcpp.hpp"      // QP/C++ real-time event framework
#include "dpp.hpp"        // DPP Application interface
#include "bsp.hpp"        // Board Support Package

#include "stm32u545xx.h"  // CMSIS-compliant header file for the MCU used
// add other drivers if necessary...

//============================================================================
namespace { // unnamed namespace for local stuff with internal linkage

Q_DEFINE_THIS_FILE

// "RTOS-aware" interrupt priorities for FreeRTOS on ARM Cortex-M, NOTE1
constexpr std::uint32_t RTOS_AWARE_ISR_CMSIS_PRI
    {configMAX_SYSCALL_INTERRUPT_PRIORITY >> (8-__NVIC_PRIO_BITS)};

static std::uint32_t l_rndSeed;

#ifdef Q_SPY

// QSpy source IDs
static QP::QSpyId const l_TickHook = { 0U };
static QP::QSpyId const l_EXTI0_IRQHandler = { 0U };

enum AppRecords { // application-specific trace records
    PHILO_STAT = QP::QS_USER,
    PAUSED_STAT,
};

#endif

} // unnamed namespace

// Local-scope defines -------------------------------------------------------
// LED pins available on the board (just one user LED LD2--Green on PA.5)
#define LD2_PIN  5U

// Button pins available on the board (just one user Button B1 on PC.13)
#define B1_PIN   13U

// macros from STM32Cube LL:
#define SET_BIT(REG, BIT)     ((REG) |= (BIT))
#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))
#define READ_BIT(REG, BIT)    ((REG) & (BIT))
#define CLEAR_REG(REG)        ((REG) = (0x0))
#define WRITE_REG(REG, VAL)   ((REG) = (VAL))
#define READ_REG(REG)         ((REG))
#define MODIFY_REG(REG, CLEARMASK, SETMASK) \
    WRITE_REG((REG), ((READ_REG(REG) & (~(CLEARMASK))) | (SETMASK)))

//============================================================================
// Error handler
extern "C" {

Q_NORETURN Q_onError(char const * const module, int_t const id) {
    // NOTE: this implementation of the error handler is intended only
    // for debugging and MUST be changed for deployment of the application
    // (assuming that you ship your production code with assertions enabled).
    Q_UNUSED_PAR(module);
    Q_UNUSED_PAR(id);
    QS_ASSERTION(module, id, 10000U); // report assertion to QS

#ifndef NDEBUG
    // light up the user LED
    GPIOA->BSRR = (1U << LD2_PIN);  // turn LED on
    // for debugging, hang on in an endless loop...
    for (;;) {
    }
#endif
    NVIC_SystemReset();
    for (;;) { // explicitly "no-return"
    }
}
//............................................................................
void assert_failed(char const * const module, int_t const id); // prototype
void assert_failed(char const * const module, int_t const id) {
    Q_onError(module, id);
}

// ISRs used in the application ==============================================
// NOTE: only the "FromISR" FreeRTOS API variants are allowed in the ISRs!

void EXTI0_IRQHandler(void); // prototype
void EXTI0_IRQHandler(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // for testing...
    APP::AO_Table->POST_FROM_ISR(
        Q_NEW_FROM_ISR(QP::QEvt, APP::MAX_PUB_SIG),
        &xHigherPriorityTaskWoken,
        &l_EXTI0_IRQHandler);

    // the usual end of FreeRTOS ISR...
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}
//............................................................................
#ifdef Q_SPY

// ISR for receiving bytes from the QSPY Back-End
// NOTE: This ISR is "QF-unaware" meaning that it does not interact with
// the FreeRTOS kernel and is not disabled.
void USART1_IRQHandler(void); // prototype
void USART1_IRQHandler(void) { // used in QS-RX (kernel UNAWARE interrupt)
    // while Read Data Register or RX-FIFO Not Empty
    while ((USART1->ISR & USART_ISR_RXNE_Msk ) != 0U) {
        std::uint32_t b = USART1->RDR;
        QP::QS::rxPut(b);
    }
}
#endif // Q_SPY

// Application hooks used in this project ====================================
// NOTE: only the "FromISR" API variants are allowed in vApplicationTickHook

void vApplicationTickHook(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // process time events at rate 0
    QP::QTimeEvt::TICK_X_FROM_ISR(0U, &xHigherPriorityTaskWoken, &l_TickHook);

    // Perform the debouncing of buttons. The algorithm for debouncing
    // adapted from the book "Embedded Systems Dictionary" by Jack Ganssle
    // and Michael Barr, page 71.
    static struct {
        std::uint32_t depressed;
        std::uint32_t previous;
    } buttons = { 0U, 0U };

    std::uint32_t current = GPIOC->IDR; // read Port C with state of Button B1
    std::uint32_t tmp = buttons.depressed; // save the depressed buttons
    buttons.depressed |= (buttons.previous & current); // set depressed
    buttons.depressed &= (buttons.previous | current); // clear released
    buttons.previous   = current; // update the history
    tmp ^= buttons.depressed;     // changed debounced depressed
    current = buttons.depressed;

    if ((tmp & (1U << B1_PIN)) != 0U) { // debounced B1 state changed?
        if ((current & (1U << B1_PIN)) != 0U) { // is B1 depressed?
            static QP::QEvt const pauseEvt(APP::PAUSE_SIG);
            QP::QActive::PUBLISH_FROM_ISR(&pauseEvt,
                                &xHigherPriorityTaskWoken,
                                &l_TickHook);
        }
        else { // the button is released
            static QP::QEvt const serveEvt(APP::SERVE_SIG);
            QP::QActive::PUBLISH_FROM_ISR(&serveEvt,
                                &xHigherPriorityTaskWoken,
                                &l_TickHook);
        }
    }

    // notify FreeRTOS to perform context switch from ISR, if needed
    portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}
//............................................................................
void vApplicationIdleHook(void) {
    // Some floating point code is to exercise the VFP...
    float volatile x = 1.73205F;
    x = x * 1.73205F;
    Q_ASSERT(2.999F < x);

#ifdef Q_SPY
    QP::QS::rxParse();  // parse all the received bytes

    // while Transmit Data Register Empty or TX-FIFO Not Full
    while ((USART1->ISR & USART_ISR_TXE_TXFNF_Msk) != 0U) {
        QF_INT_DISABLE();
        std::uint16_t b = QP::QS::getByte();
        QF_INT_ENABLE();

        if (b != QS_EOD) {   // not End-Of-Data?
            USART1->TDR = b; // put into the DR register
        }
        else {
            break; // done for now
        }
    }
#elif defined NDEBUG
    // Put the CPU and peripherals to the low-power mode.
    // you might need to customize the clock management for your application,
    // see the datasheet for your particular Cortex-M MCU.
    __WFI(); // Wait-For-Interrupt
#endif
}
//............................................................................
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    (void)xTask;
    (void)pcTaskName;
    Q_ERROR();
}
//............................................................................
// configSUPPORT_STATIC_ALLOCATION is set to 1, so the application must
// provide an implementation of vApplicationGetIdleTaskMemory() to provide
// the memory that is used by the Idle task.
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize )
{
    // If the buffers to be provided to the Idle task are declared inside
    // this function then they must be declared static - otherwise they will
    // be allocated on the stack and so not exists after this function exits.
    //
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

    // Pass out a pointer to the StaticTask_t structure in which the
    // Idle task's state will be stored.
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    // Pass out the array that will be used as the Idle task's stack.
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    // Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    // Note that, as the array is necessarily of type StackType_t,
    // configMINIMAL_STACK_SIZE is specified in words, not bytes.
    //
    *pulIdleTaskStackSize = Q_DIM(uxIdleTaskStack);
}

} // extern "C"

// BSP functions =============================================================
namespace BSP {

void init() {
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

    // NOTE: The VFP (hardware Floating Point) unit is configured by QK

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

    BSP::randomSeed(1234U);

    // initialize the QS software tracing...
    if (!QS_INIT(nullptr)) {
        Q_ERROR();
    }

    // dictionaries...
    QS_OBJ_DICTIONARY(&l_TickHook);
    QS_OBJ_DICTIONARY(&l_EXTI0_IRQHandler);
    QS_USR_DICTIONARY(PHILO_STAT);
    QS_USR_DICTIONARY(PAUSED_STAT);

    QS_ONLY(APP::produce_sig_dict());

    // setup the QS filters...
    QS_GLB_FILTER(QP::QS_ALL_RECORDS); // all records
    QS_GLB_FILTER(-QP::QS_QF_TICK);    // exclude the clock tick
}
//............................................................................
void start() {
    // initialize event pools
    static QF_MPOOL_EL(APP::TableEvt) smlPoolSto[2*APP::N_PHILO];
    QP::QF::poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));

    // initialize publish-subscribe
    static QP::QSubscrList subscrSto[APP::MAX_PUB_SIG];
    QP::QActive::psInit(subscrSto, Q_DIM(subscrSto));

    // start AOs/threads...
    static QP::QEvtPtr philoQueueSto[APP::N_PHILO][10];
    static StackType_t philoStack[APP::N_PHILO][configMINIMAL_STACK_SIZE];
    for (std::uint8_t n = 0U; n < APP::N_PHILO; ++n) {
        APP::AO_Philo[n]->start(
            Q_PRIO(n + 3U, 3U),      // QP prio., FreeRTOS prio.
            philoQueueSto[n],        // event queue storage
            Q_DIM(philoQueueSto[n]), // queue length [events]
            philoStack[n],           // stack storage
            sizeof(philoStack[n]));  // stack size [bytes]
    }

    static QP::QEvtPtr tableQueueSto[APP::N_PHILO];
    static StackType_t tableStack[configMINIMAL_STACK_SIZE];
    APP::AO_Table->start(
        Q_PRIO(APP::N_PHILO + 7U, 7U), // QP prio., FreeRTOS prio.
        tableQueueSto,               // event queue storage
        Q_DIM(tableQueueSto),        // queue length [events]
        tableStack,                  // stack storage
        sizeof(tableStack));         // stack size [bytes]
}
//............................................................................
void displayPhilStat(std::uint8_t n, char const *stat) {
    Q_UNUSED_PAR(n);

    if (stat[0] == 'e') {
        GPIOA->BSRR = (1U << LD2_PIN); // turn LED on
    }
    else {
        GPIOA->BRR  = (1U << LD2_PIN); // turn LED off
    }

    // app-specific trace record...
    QS_BEGIN_ID(PHILO_STAT, APP::AO_Table->getPrio())
        QS_U8(1, n);  // Philosopher number
        QS_STR(stat); // Philosopher status
    QS_END()
}
//............................................................................
void displayPaused(std::uint8_t const paused) {
    if (paused != 0U) {
        //GPIOA->BSRR = (1U << LD2_PIN); // turn LED[n] on
    }
    else {
        //GPIOA->BRR  = (1U << LD2_PIN); // turn LED[n] off
    }

    // application-specific trace record
    QS_BEGIN_ID(PAUSED_STAT, APP::AO_Table->getPrio())
        QS_U8(1, paused);  // Paused status
    QS_END()
}
//............................................................................
void randomSeed(std::uint32_t const seed) {
    l_rndSeed = seed;
}
//............................................................................
std::uint32_t random() { // a very cheap pseudo-random-number generator
    // Some floating point code is to exercise the VFP...
    float volatile x = 3.1415926F;
    x = x + 2.7182818F;

    vTaskSuspendAll(); // lock FreeRTOS scheduler
    // "Super-Duper" Linear Congruential Generator (LCG)
    // LCG(2^32, 3*7*11*13*23, 0, seed)
    std::uint32_t rnd = l_rndSeed * (3U*7U*11U*13U*23U);
    l_rndSeed = rnd; // set for the next time
    xTaskResumeAll(); // unlock the FreeRTOS scheduler

    return (rnd >> 8U);
}
//............................................................................
void terminate(std::int16_t result) {
    Q_UNUSED_PAR(result);
}
//............................................................................
void ledOn() {
    GPIOA->BSRR = (1U << LD2_PIN);  // turn LED on
}
//............................................................................
void ledOff() {
    GPIOA->BRR = (1U << LD2_PIN);  // turn LED off
}

} // namespace BSP

//============================================================================
namespace QP {

// QF callbacks...

void QF::onStartup() {
    // set up the SysTick timer to fire at BSP::TICKS_PER_SEC rate
    //SysTick_Config(SystemCoreClock / BSP::TICKS_PER_SEC); // done in FreeRTOS

    // assign all priority bits for preemption-prio. and none to sub-prio.
    NVIC_SetPriorityGrouping(0U);

    // set priorities of ALL ISRs used in the system, see NOTE1
    NVIC_SetPriority(USART1_IRQn,    0U); // kernel unaware interrupt
    NVIC_SetPriority(EXTI0_IRQn,     RTOS_AWARE_ISR_CMSIS_PRI + 0U);
    NVIC_SetPriority(SysTick_IRQn,   RTOS_AWARE_ISR_CMSIS_PRI + 1U);
    // ...

    // enable IRQs...
    NVIC_EnableIRQ(EXTI0_IRQn);

#ifdef Q_SPY
    NVIC_EnableIRQ(USART1_IRQn); // UART interrupt used for QS-RX
#endif
}
//............................................................................
void QF::onCleanup() {
}

//============================================================================
// QS callbacks...
#ifdef Q_SPY

//............................................................................
static std::uint16_t const QS_UARTPrescTable[12] = {
    1U, 2U, 4U, 6U, 8U, 10U, 12U, 16U, 32U, 64U, 128U, 256U
};

#define __LL_USART_DIV_SAMPLING16(__PERIPHCLK__, __PRESCALER__, __BAUDRATE__) \
  ((((__PERIPHCLK__)/(USART_PRESCALER_TAB[(__PRESCALER__)]))\
    + ((__BAUDRATE__)/2U))/(__BAUDRATE__))

#define QS_UART_DIV_SAMPLING16(__PCLK__, __BAUD__, __CLOCKPRESCALER__) \
  ((((__PCLK__)/QS_UARTPrescTable[(__CLOCKPRESCALER__)]) \
  + ((__BAUD__)/2U)) / (__BAUD__))

// USART1 pins PA.9 and PA.10
#define USART1_TX_PIN 9U
#define USART1_RX_PIN 10U

//............................................................................
bool QS::onStartup(void const *arg) {
    Q_UNUSED_PAR(arg);

    static std::uint8_t qsTxBuf[2*1024]; // buffer for QS-TX channel
    initBuf(qsTxBuf, sizeof(qsTxBuf));

    static std::uint8_t qsRxBuf[100];    // buffer for QS-RX channel
    rxInitBuf(qsRxBuf, sizeof(qsRxBuf));

    // enable peripheral clock for USART1 and its pins
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN); // UART1 clock
    SET_BIT(RCC->AHB2ENR1, RCC_AHB2ENR1_GPIOAEN); // GPIOA clock for pins

    // Configure GPIOA to USART1_RX and USART1_TX pins.........................
    MODIFY_REG(GPIOA->OSPEEDR,
        GPIO_OSPEEDR_OSPEED0 << (USART1_TX_PIN * GPIO_OSPEEDR_OSPEED1_Pos),
        0U << (USART1_TX_PIN * GPIO_OSPEEDR_OSPEED1_Pos)); // speed==0
    MODIFY_REG(GPIOA->OSPEEDR,
        GPIO_OSPEEDR_OSPEED0 << (USART1_RX_PIN * GPIO_OSPEEDR_OSPEED1_Pos),
        0U << (USART1_RX_PIN * GPIO_OSPEEDR_OSPEED1_Pos)); // speed==0

    MODIFY_REG(GPIOA->OTYPER,
        (1U << USART1_TX_PIN) | (1U << USART1_RX_PIN),
        ((1U << USART1_TX_PIN) | (1U << USART1_RX_PIN) * 0U)); // output

    MODIFY_REG(GPIOA->PUPDR,
        GPIO_PUPDR_PUPD0 << (USART1_TX_PIN * GPIO_PUPDR_PUPD1_Pos),
        0U << (USART1_TX_PIN * GPIO_PUPDR_PUPD1_Pos)); // PUSHPULL
    MODIFY_REG(GPIOA->PUPDR,
        GPIO_PUPDR_PUPD0 << (USART1_RX_PIN * GPIO_PUPDR_PUPD1_Pos),
        0U << (USART1_RX_PIN * GPIO_PUPDR_PUPD1_Pos)); // PUSHPULL

    MODIFY_REG(GPIOA->AFR[1],
        GPIO_AFRH_AFSEL8 << ((USART1_TX_PIN - 8U) * GPIO_AFRH_AFSEL9_Pos),
        7U << ((USART1_TX_PIN - 8U) * GPIO_AFRH_AFSEL9_Pos)); // AF_7
    MODIFY_REG(GPIOA->AFR[1],
        GPIO_AFRH_AFSEL8 << ((USART1_RX_PIN - 8U) * GPIO_AFRH_AFSEL9_Pos),
        7U << ((USART1_RX_PIN - 8U) * GPIO_AFRH_AFSEL9_Pos)); // AF_7

    MODIFY_REG(GPIOA->MODER,
        GPIO_MODER_MODE0 << (USART1_TX_PIN * GPIO_MODER_MODE1_Pos),
        2U << (USART1_TX_PIN * GPIO_MODER_MODE1_Pos)); // MODE_2
    MODIFY_REG(GPIOA->MODER,
        GPIO_MODER_MODE0 << (USART1_RX_PIN * GPIO_MODER_MODE1_Pos),
        2U << (USART1_RX_PIN * GPIO_MODER_MODE1_Pos)); // MODE_2

    // configure USART1..............................................
    MODIFY_REG(USART1->CR1,
        (USART_CR1_M | USART_CR1_PCE | USART_CR1_PS |
         USART_CR1_TE | USART_CR1_RE | USART_CR1_OVER8),
        0U  | // data==8bits
        0U  | // parity==none
        12U | // direction==TX & RX
        0U);  // oversampling==16
    MODIFY_REG(USART1->CR2,
        USART_CR2_STOP,
        0U);  // stop-bits==1
    MODIFY_REG(USART1->CR3,
        USART_CR3_RTSE | USART_CR3_CTSE,
        0U);  // hardware-flow=NO

    // baud rate
    USART1->BRR = QS_UART_DIV_SAMPLING16(
                       SystemCoreClock, // USART1 clock
                       115200U,         // baud rate
                       0U);             // prescaler

    MODIFY_REG(USART1->PRESC,
        USART_PRESC_PRESCALER,
        0U); // prescaler=DIV1

    // FIFO thresholds
    MODIFY_REG(USART1->CR3,
        USART_CR3_TXFTCFG,
        0U << USART_CR3_TXFTCFG_Pos); // TX-FIFO=threshold-1
    MODIFY_REG(USART1->CR3,
        USART_CR3_RXFTCFG,
        0U << USART_CR3_RXFTCFG_Pos); // RX-FIFO=threshold-1

    SET_BIT(USART1->CR1, USART_CR1_FIFOEN);  // enable FIFO

    // asynch mode
    CLEAR_BIT(USART1->CR2, USART_CR2_LINEN | USART_CR2_CLKEN);
    CLEAR_BIT(USART1->CR3, USART_CR3_SCEN | USART_CR3_IREN | USART_CR3_HDSEL);

    // RXNE and RX FIFO Not Empty Interrupt Enable
    SET_BIT(USART1->CR1, USART_CR1_RXNEIE_Msk);

    SET_BIT(USART1->CR1, USART_CR1_UE); // enable USART

    // configure Timer5 for delivering QS time-stamp...........................
    SET_BIT(RCC->APB1ENR1, RCC_APB1ENR1_TIM5EN);

    uint32_t tmp = TIM5->CR1;
    MODIFY_REG(tmp, TIM_CR1_DIR | TIM_CR1_CMS, 0U); // counter-mode=Up
    MODIFY_REG(tmp, TIM_CR1_CKD, 0U); // clock-division=1
    WRITE_REG(TIM5->CR1, tmp);

    WRITE_REG(TIM5->ARR, 0xFFFFFFFFU); // auto-reload
    WRITE_REG(TIM5->PSC, 0U); // prescaler=1
    SET_BIT(TIM5->EGR, TIM_EGR_UG); // update event to reload prescaler

    CLEAR_BIT(TIM5->CR1, TIM_CR1_ARPE); // disable ARR preload
    MODIFY_REG(TIM5->SMCR, TIM_SMCR_SMS | TIM_SMCR_ECE, 0U); // internal clock
    MODIFY_REG(TIM5->CR2, TIM_CR2_MMS, 0U); // reset timer synchronization
    CLEAR_BIT(TIM5->SMCR, TIM_SMCR_MSM); // disable master-slave

    // enable Timer5 to start time stamp
    SET_BIT(TIM5->CR1, TIM_CR1_CEN);

    return true; // return success
}
//............................................................................
void QS::onCleanup() {
}
//............................................................................
QSTimeCtr QS::onGetTime() { // NOTE: invoked with interrupts DISABLED
    return TIM5->CNT; // 32-bit Timer5 count
}
//............................................................................
// NOTE:
// No critical section in QS::onFlush() to avoid nesting of critical sections
// in case QS::onFlush() is called from Q_onError().
void QS::onFlush() {
    for (;;) {
        std::uint16_t b = getByte();
        if (b != QS_EOD) {
            while ((USART1->ISR & USART_ISR_TXE_Msk) == 0U) { // TXE not empty
            }
            USART1->TDR = b;
        }
        else {
            break;
        }
    }
}
//............................................................................
void QS::onReset() {
    NVIC_SystemReset();
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

#endif // Q_SPY

} // namespace QP

//============================================================================
// NOTE1:
// The configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY constant from the
// FreeRTOS configuration file specifies the highest ISR priority that
// is disabled by the QF framework. The value is suitable for the
// NVIC_SetPriority() CMSIS function.
//
// Only ISRs prioritized at or below the
// configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY level (i.e.,
// with the numerical values of priorities equal or higher than
// configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY) are allowed to call any
// QP/FreeRTOS services. These ISRs are "kernel-aware".
//
// Conversely, any ISRs prioritized above the
// configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY priority level (i.e., with
// the numerical values of priorities less than
// configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY) are never disabled and are
// not aware of the kernel. Such "kernel-unaware" ISRs cannot call any
// QP/FreeRTOS services. The only mechanism by which a "kernel-unaware" ISR
// can communicate with the QF framework is by triggering a "kernel-aware"
// ISR, which can post/publish events.
//
// For more information, see article "Running the RTOS on a ARM Cortex-M Core"
// http://www.freertos.org/RTOS-Cortex-M3-M4.html
//
// NOTE2:
// The User LED is used to visualize the idle loop activity. The brightness
// of the LED is proportional to the frequency of invocations of the idle loop.
// Please note that the LED is toggled with interrupts locked, so no interrupt
// execution time contributes to the brightness of the User LED.

/**
 * \file port.c
 * \brief Porting layer for MSP430 - implementation
 *
 * \copyright Copyright 2021 The ImmortalThreads authors. All rights reserved.
 * \license MIT License
 */

#include "port/portable.h"
#include "../../sched.h"

/* scheduler preemption frequency */
#define TIMER_COUNT 500

/* holds the stack pointer register */
static volatile uint32_t __sp;
/* holds the address of the current thread index */
__fram static volatile uint8_t __th = 0;

static void _setSchedTimer() {
  TA0CCR0 = TIMER_COUNT;
  TA0CTL = TASSEL__ACLK + MC__UP;
  TA0CCTL0 = CCIE;
}

void im_port_init(bool use_timer) {
  /* run a periodic timer for task switching */
  if (use_timer) {
    _setSchedTimer();
  }
}

void im_port_run(immortal_thread_t *volatile threads, uint8_t size) {
  /* enable interrupts at this point */
  _ei();

  /* always update __th since it might be partially updated due to a power
   * failure */
  if (__th == size)
    __th = 0;

  /* MSP430 specific code: save stack pointer */
  __asm__ volatile("MOVA SP, %0"
                   : "=m"(__sp) /* output */
                   :            /* input */
  );

  while (1) {
    /* TODO: Maybe safer to write in assembly. Compiler might use a dirty
     * register so it might cause fatal errors. */
    /* run the thread */
    imt_priv_run_thread(&threads[__th]);

    /* (1) SP = __sp (stack pointer is set to the old value. no effect if we do
     * not return from the ISR) (2) if(++__th == size) __th = 0; (to switch to
     * the next thread) */
    __asm__ volatile(
        "ISR_return:            \n" /* _schedTimerISR will jump here */
        "DINT                   \n" /* disable interrupts */
        "NOP                    \n"
        "MOVA %1, SP            \n" /* __sp = SP  */
        "INC.B %0               \n" /* __th++ */
        "CMP.B %2,%3            \n" /* if (__th == size) */
        "JNZ cont               \n"
        "CLR.B %2               \n" /* __th = 0 */
        "cont:                  \n"
        "NOP                    \n"
        "EINT                   \n" /* enable interrupts */
        "NOP                    \n"
        : "=m"(__th)                      /* output */
        : "m"(__sp), "m"(__th), "m"(size) /* input */
    );
  }
}

/* Timer to generate thread switch */
#if defined(__GNUC__)
void __attribute__((interrupt(TIMER0_A0_VECTOR))) _schedTimerISR(void)
#else
#error Compiler not supported!
#endif
{
  /* TODO: handle 20 bit address */
  /* modify the return address on the stack for RETI -> jump to ISR_return */
  __asm__ volatile("MOV.W #ISR_return, 2(SP)");
}

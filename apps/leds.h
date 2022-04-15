/*
 * MIT License
 *
 * Copyright (c) 2020 Eren Yildiz and Kasim Sinan Yildirim
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef LEDS_H_
#define LEDS_H_

#include <msp430.h>

/* LED configuration for MSP430FR5994 */
#define LED1 BIT1
#define LED1_OUT P1OUT
#define LED1_DIR P1DIR

#define LED2 BIT0
#define LED2_OUT P1OUT
#define LED2_DIR P1DIR

#define __port_init(port, bit)                                                 \
  P##port##OUT &= ~BIT##bit;                                                   \
  P##port##DIR |= BIT##bit;                                                    \
  P##port##OUT &= ~BIT##bit;

#define __port_on(port, bit) P##port##OUT |= BIT##bit;
#define __port_off(port, bit) P##port##OUT &= ~BIT##bit;
#define __port_toggle(port, bit) P##port##OUT ^= BIT##bit;

/* Macros for easy access to the LEDs on the evaulation board. */
#define __led_init(led)                                                        \
  led##_OUT &= ~led;                                                           \
  led##_DIR |= led;                                                            \
  led##_OUT &= ~led;

#define __led_on(led) led##_OUT |= led;
#define __led_off(led) led##_OUT &= ~led;
#define __led_toggle(led) led##_OUT ^= led;

#endif /* LEDS_H_ */

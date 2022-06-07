/**
 * \file linux/port.h
 * \brief Linux port - interface
 *
 * \copyright Copyright 2022 The ImmortalThreads authors. All rights reserved.
 * \license MIT License. See accompanying file LICENSE.txt at
 * https://github.com/tinysystems/ImmortalThreads/blob/main/LICENSE.txt
 */
#ifndef IMMORTALITY_PORT_LINUX_H_
#define IMMORTALITY_PORT_LINUX_H_

#ifdef __cplusplus
extern "C" {
#endif

#define __fram __attribute__((section("persistent")))
#define __ro_fram __attribute__((section(".rodata")))
#define __hifram __attribute__((section(".persistent_hifram ")))
#define __ro_hifram __attribute__((section(".upper.rodata")))

void im_enable_interrupt(void);
void im_disable_interrupt(void);

#define _ei() im_enable_interrupt();
#define _di() im_disable_interrupt();

#ifdef __cplusplus
}
#endif

#endif /* ifndef IMMORTALITY_PORT_LINUX_H_ */

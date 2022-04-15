/**
 * \file port_special.h
 * \brief Platform-specific special interface that is exposed to the
 * application
 *
 * \copyright Copyright 2021 The ImmortalThreads authors. All rights reserved.
 * \license MIT License
 */
#ifndef IMMORTALITY_PORT_LINUX_SPECIAL_H_
#define IMMORTALITY_PORT_LINUX_SPECIAL_H_

#ifdef __cplusplus
extern "C" {
#endif

void im_port_linux_init(int argc, char *argv[]);

#ifdef __cplusplus
}
#endif

#endif /* ifndef IMMORTALITY_PORT_LINUX_SPECIAL_H_ */

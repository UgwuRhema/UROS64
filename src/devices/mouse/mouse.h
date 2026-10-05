#ifndef MOUSE_H
#define MOUSE_H

#include "../../func/func.h"
#include <stdint.h>

extern void mouse_wait(uint8_t);
extern void mouse_write(uint8_t);
extern void mouse_init(void);

#endif

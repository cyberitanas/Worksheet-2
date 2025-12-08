#pragma once
#include "../source/stdint.h"

void pic_init(void);                  // <-- ADD THIS
void pic_acknowledge(unsigned int irq);

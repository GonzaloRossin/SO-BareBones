#ifndef IDT_H
#define IDT_H
#include <stdint.h>

void loadIDT();

void irqDispatcher(int n);

#endif
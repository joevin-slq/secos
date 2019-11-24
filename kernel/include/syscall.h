#ifndef SYSCALL_H
#define SYSCALL_H

#include <intr.h>

void syscall_isr();
void __regparm__(1) syscall_handler(int_ctx_t*);
void syscall();

#endif
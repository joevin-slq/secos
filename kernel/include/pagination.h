#ifndef PAGINATION_H
#define PAGINATION_H

#include <pagemem.h>

void pagination();
void set_pte(pte32_t * adr_pte, int offset);
void set_pde(pde32_t * adr_pde);
void init_pagination();

#endif
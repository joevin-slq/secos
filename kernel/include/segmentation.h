#ifndef SEGMENTATION_H
#define SEGMENTATION_H
#include <segmem.h>

seg_desc_t GDT[6];
tss_t TSS;

#define GDT_C0  1
#define GDT_D0  2
#define GDT_C3  3
#define GDT_D3  4
#define TSS_S0  5

void segmentation();
void cree_seg_gdt();
void cree_seg_tss();
void init_gdt();
void affiche_gdt();
void init_tss();

#endif
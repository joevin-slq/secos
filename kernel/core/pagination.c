#include <pagination.h>
#include <debug.h>
#include <cr.h>
#include <pagemem.h>

pde32_t *pgd  = (pde32_t*)0x600000; // Page Directory -> pde
pte32_t *ptb  = (pte32_t*)0x601000; // Page Table -> pte
pte32_t *ptb2 = (pte32_t*)0x602000; // Page Table -> pte2
pte32_t *ptb3 = (pte32_t*)0x603000; // Page Table -> pte3

void pagination()
{
   debug("PAGINATION : TODO\n");
   init_pagination();
}

// Page Table (PTB)
void set_pte(pte32_t * adr_pte, int offset)
{
    adr_pte->p = 1;
    adr_pte->rw = 1;
    adr_pte->lvl = 0;
    adr_pte->pwt = 0;
    adr_pte->pcd = 0;
    adr_pte->acc = 0;
    adr_pte->d = 0;
    adr_pte->pat = 0;
    adr_pte->g = 0;
    adr_pte->avl = 0;
    adr_pte->addr = offset;
}

// Page Directory (PGD)
void set_pde(pde32_t * adr_pde)
{
    adr_pde->p=1;
    adr_pde->rw=1;
    adr_pde->lvl=0;
    adr_pde->pwt=0;
    adr_pde->pcd=0;
    adr_pde->acc=0;
    adr_pde->mbz=0;
    adr_pde->avl=0;
    adr_pde->addr=0; // adresse de la première PTB
}

void init_pagination()
{
   set_cr3(pgd);
   debug("CR3=%x\n", get_cr(3));

    // Initialisation de la première Page Table Entry
    for(int i=0; i<1024; i++)
        set_pte(&ptb[i], i);
    
    // Page Directory Entry
    set_pde(pgd);
    // Page Directory Entry pointe vers l'adresse de la première Page Table Entry 
    pgd->addr = (uint32_t)&ptb[0]>>PG_4K_SHIFT;


    // Initialisation de la deuxième Page Table Entry
    for(int i=0; i<1024; i++)
        set_pte(&ptb2[i], i+1024);

    // Page Directory Entry n°2
    set_pde(&pgd[1]);
    // Page Directory Entry n°2 pointe vers l'adresse de la deuxième Page Table Entry 
    (&pgd[1])->addr = (uint32_t)&ptb2[0]>>PG_4K_SHIFT;

   // Active la pagination
   set_cr0(get_cr0()|CR0_PG);
   debug("CR0=%x\n", get_cr0());
}

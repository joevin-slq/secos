/* GPLv2 (c) Airbus
 * Initialisation du répertoire de Page Directory
 * Aide : https://wiki.osdev.org/Setting_Up_Paging
 */
#include <debug.h>
#include <info.h>
#include <cr.h>
#include <pagemem.h>

extern info_t *info;
pde32_t *pgd  = (pde32_t*)0x600000; // Page Directory -> pde
pte32_t *ptb  = (pte32_t*)0x601000; // Page Table -> pte
pte32_t *ptb2 = (pte32_t*)0x602000; // Page Table -> pte2
pte32_t *ptb3 = (pte32_t*)0x603000; // Page Table -> pte3

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

void q2() // Q1 et Q2:
{
    set_cr3(pgd);
    debug("CR3=%x\n", get_cr(3));
}

void active_pagination() // Q3:
{
    set_cr0(get_cr0()|CR0_PG); // masque pour éviter d'altérer le CR0 existant
    debug("CR0=%x\n", get_cr0());
}

void id() // Q4 et Q6:
{
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
}

void q5() // Q5:
{   // Page Fault, l'adresse ptb[1] n'est plus atteignable
    // depuis que la pagination est activée car l'adresse n'est pas alouée
    debug("ok : %s\n", ptb[1].raw);
}

void q7() // Q7: L'adresse virtuelle 0xc0000000 permet de modifier la PGD.
{
    uint32_t *target  = (uint32_t*)0xc0000000;
    int ptb_idx = pt32_idx(target); // addr>>4K_SHIFT(12) &0x3ff
    int pgd_idx = pd32_idx(target); // addr>>4M_SHIFT(22) &0x3ff

    // Initialise la table de page n°3
    memset((void*)ptb3, 0, PAGE_SIZE); // &ptb3=0x603000
    pg_set_entry(&ptb3[ptb_idx], PG_KRN|PG_RW, page_nr(pgd));

    // Initialise la page directory
    pg_set_entry(&pgd[pgd_idx], PG_KRN|PG_RW, page_nr(ptb3));

    debug("PGD[0] = %p | target = %p\n", pgd[0].raw, *target);
    for(int i=0; i<10; i++)
    {
        debug("target[%d] = %p\n", i, *(target+i));
    }
}

void q8() // Q8: Les adr virt 0x700000 et 0x7ff000 mappent l'adr phy 0x2000.
{
    char *target1  = (char*)0x700000;
    char *target2  = (char*)0x7ff000;
    int ptb2_idx1 = pt32_idx(target1);
    int ptb2_idx2 = pt32_idx(target2);

    // Définit la table de page n°2
    memset((void*)ptb2, 0, PAGE_SIZE); // &ptb2=0x700000
    pg_set_entry(&ptb2[ptb2_idx1], PG_KRN|PG_RW, 2); // 2->0x2000
    pg_set_entry(&ptb2[ptb2_idx2], PG_KRN|PG_RW, 2);

    debug("target1[%p] = %p\n", target1, *target1);
    debug("target2[%p] = %p\n", target2, *target2);

    // Q9: Effacez la première entrée du PGD.
    *target1 = 'A'; // (c'est censé planter ici)
    invalidate(target1); // flush des tables de cache
}


void tp()
{
    debug("\nHello!\n");
    q2();
    id();
    active_pagination();
    q5();
    q7();
    q8();
}

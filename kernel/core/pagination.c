#include <pagination.h>
#include <debug.h>
#include <cr.h>
#include <pagemem.h>
#include <cartographie.h>

void pagination()
{
    init_noyau();
    init_user1();
    init_user2();
    init_partage();

    // Active la pagination
    set_cr3(PGD_N);
    set_cr0(get_cr0()|CR0_PG);
    
    // Active SMEP : si le CPU est en ring 0,
    // qu'il tente d'exécuter du code prevenant du ring3,
    // il génère une faute de page.
    set_cr4(get_cr4()|CR4_SMEP_BIT);
}

void init_noyau()
{
    pde32_t* pgd_n = (pde32_t*) PGD_N;
    pte32_t* ptb_n = (pte32_t*) PTB_N;

    for(int i = 0; i < 1024; i++) // 0x0000000 à 0x00400000
        pg_set_entry(&ptb_n[i], PG_KRN|PG_RW, i);

    memset((void*)pgd_n, 0, PAGE_SIZE);
    pg_set_entry(&pgd_n[0], PG_KRN|PG_RW, page_nr(PTB_N));
}

void init_user1()
{
    pde32_t* pgd_u1         = (pde32_t*) PGD_U1;
    pte32_t* ptb_u1_pile_n  = (pte32_t*) PTB_U1_PILE_N;
    pte32_t* ptb_u1_pile_u  = (pte32_t*) PTB_U1_PILE_U;
    pte32_t* ptb_u1_partage = (pte32_t*) PTB_U1_PARTAGE;

    for (int i=0; i<1024; i++) // 0x0000000 à 0x00400000
        pg_set_entry(&ptb_u1_pile_n[i], PG_KRN|PG_RW, i); // PG_KRN ici!

    for (int i=0; i<1024; i++) // 0x00400000 à 0x00800000
        pg_set_entry(&ptb_u1_pile_u[i], PG_USR|PG_RW, i+1024);

    for (int i=0; i<1024; i++) // 0x00C00000 à 0x01000000
        pg_set_entry(&ptb_u1_partage[i], PG_USR|PG_RW, i+3072);

    memset((void*)pgd_u1, 0, PAGE_SIZE);
    pg_set_entry(&pgd_u1[0], PG_USR|PG_RW, page_nr(PTB_U1_PILE_N));
    pg_set_entry(&pgd_u1[1], PG_USR|PG_RW, page_nr(PTB_U1_PILE_U));
    pg_set_entry(&pgd_u1[3], PG_USR|PG_RW, page_nr(PTB_U1_PARTAGE));
}

void init_user2()
{
    pde32_t* pgd_u2         = (pde32_t*) PGD_U2;
    pte32_t* ptb_u2_pile_n  = (pte32_t*) PTB_U2_PILE_N;
    pte32_t* ptb_u2_pile_u  = (pte32_t*) PTB_U2_PILE_U; 
    pte32_t* ptb_u2_partage = (pte32_t*) PTB_U2_PARTAGE;

    for(int i=0; i<1024; i++) // 0x0000000 à 0x00400000
        pg_set_entry(&ptb_u2_pile_n[i], PG_KRN|PG_RW, i); // PG_KRN ici!

    for(int i=0; i<1024; i++) // 0x00800000 à 0x00C00000
        pg_set_entry(&ptb_u2_pile_u[i], PG_USR|PG_RW, i+2048);

    for(int i=0; i<1024; i++) // 0x00C00000 à 0x01000000
        pg_set_entry(&ptb_u2_partage[i], PG_USR|PG_RW, i+3072);

    memset((void*)pgd_u2, 0, PAGE_SIZE);
    pg_set_entry(&pgd_u2[0], PG_USR|PG_RW, page_nr(PTB_U2_PILE_N));
    pg_set_entry(&pgd_u2[2], PG_USR|PG_RW, page_nr(PTB_U2_PILE_U));
    pg_set_entry(&pgd_u2[3], PG_USR|PG_RW, page_nr(PTB_U2_PARTAGE));
}

void init_partage()
{
    pte32_t* ptb_u1_pile_u = (pte32_t*) PTB_U1_PILE_U;
    pte32_t* ptb_u2_pile_u = (pte32_t*) PTB_U2_PILE_U;

    int partage_idx_u1 = pt32_idx(U1_CPT);
    int partage_idx_u2 = pt32_idx(U2_CPT);

    pg_set_entry(&ptb_u1_pile_u[partage_idx_u1], PG_USR|PG_RW, page_nr(PARTAGE));
    pg_set_entry(&ptb_u2_pile_u[partage_idx_u2], PG_USR|PG_RW, page_nr(PARTAGE));
}

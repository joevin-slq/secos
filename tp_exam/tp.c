/* GPLv2 (c) Airbus */
#include <debug.h>
#include <segmentation.h>
#include <userland.h>
#include <pagination.h>
#include <taches.h>
#include <asm.h>
#include <cartographie.h>

void init_taches();
extern tache_t* t_courante;

void tp()
{
    segmentation();
    pagination();
    init_taches();
}

/* Démarre les tâches utilisateurs */
void init_taches()
{
    tache_t t_noyau, t_user1, t_user2;

    // Création de la tache noyau
    init_tache_noyau(&t_noyau, (pde32_t*) PGD_N, &t_user1);

    // Création des taches utilisateurs
    init_tache(&t_user1,
                (uint32_t) &user1,
                (uint32_t*) PILE_N1,
                (uint32_t*) PILE_U1,
                (pde32_t*) PGD_U1,
                &t_user2);
    init_tache(&t_user2,
                (uint32_t) &user2,
                (uint32_t*) PILE_N2,
                (uint32_t*) PILE_U2,
                (pde32_t*) PGD_U2,
                &t_user1);

    t_courante = &t_noyau;

    force_interrupts_on();
    while(1);
    debug("Pépin!");
}

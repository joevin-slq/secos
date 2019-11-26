/* GPLv2 (c) Airbus */
#include <debug.h>
#include <segmentation.h>
#include <userland.h>
#include <pagination.h>

void init_tss();

void tp()
{
    segmentation();
    // pagination();
    userland();
}

// Démarre les tâches utilisateurs
void init_taches()
{
    /* Création des taches utilisateurs
    init_tache(&t_user1,
                    (uint32_t) &user1,
                    PILE_NOYEAU1,
                    PILE_USER1,
                    &t_user2);
    init_tache(&t_user2,
                    (uint32_t) &user2,
                    PILE_NOYEAU2,
                    PILE_USER2,
                    &t_user1); */
}
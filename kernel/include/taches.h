#ifndef TACHES_H
#define TACHES_H
#include <pagemem.h>
#include <segmentation.h>
#include <cr.h>
#include <debug.h>
#include <intr.h>
#include <userland.h>

typedef struct tache tache_t;
struct tache
{
    int_ctx_t* pile_haut;
    uint32_t*  pile_noyeau;
    pde32_t*   pgd; // <- contenu du registre CS
    tache_t*   suivante;
};

void init_tache(tache_t *tache,
                uint32_t code,
                uint32_t pile_noyeau,
                uint32_t pile_utilisateur,
                tache_t *tache_suivante);
void ordonnanceur(int_ctx_t *ctx);

#endif
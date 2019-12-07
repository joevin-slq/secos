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
    uint32_t *pile_haut;
    uint32_t *pile_noyau;
    pde32_t  *pgd;
    tache_t  *suivante;
};

void init_tache_noyau(tache_t *tache, pde32_t *pgd, tache_t *tache_suivante);
void init_tache(tache_t  *tache,
                uint32_t  code,
                uint32_t *pile_noyau,
                uint32_t *pile_utilisateur,
                pde32_t  *pgd,
                tache_t  *tache_suivante);
void ordonnanceur(int_ctx_t *ctx);

#endif
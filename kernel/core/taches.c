#include <taches.h>
#include <debug.h>

extern void __regparm__(2) bascule();
extern void resume_from_intr();
tache_t* t_courante;
extern tss_t TSS;

/**
 * "La procédure de changement de contexte va donc avoir pour role de sauver
 * l'état du thread interrompu dans sa pile noyau et charger l'état du thread à
 * lancer pour la première fois ou à relancer, depuis la pile noyau de ce dernier."
 */
void ordonnanceur(int_ctx_t *ctx)
{
    if(ctx->cs.raw == gdt_krn_seg_sel(GDT_C0)) { debug("\n* irq noyau\n"); }
    if(ctx->cs.raw == gdt_usr_seg_sel(GDT_C3)) { debug("\n* irq utilisateur\n"); }
    
    uint32_t EIP= (uint32_t)ctx->eip.raw;
    debug("EIP util:%x\n", EIP);
    uint32_t ESP= (uint32_t)ctx->esp.raw;
    debug("ESP util:%x\n", ESP);
    
    uint32_t *precedent = t_courante->pile_haut;
    t_courante = t_courante->suivante;
    uint32_t *suivant = t_courante->pile_haut;
    set_cr3(t_courante->pgd);
    TSS.s0.esp = (uint32_t)suivant;


    // ((int_ctx_t*)suivant)->eip.raw = EIP;
    // ((int_ctx_t*)suivant)->esp.raw = ESP;
    /* Debug :
    debug("* t_courante->pile_haut\t : %p\n", t_courante->pile_haut);
    debug("* t_courante->pile_noyau : %p\n", t_courante->pile_noyau);
    debug("* precedent \t\t : %p\n", precedent);
    debug("* suivant \t\t : %p\n", suivant); */

    bascule(precedent, suivant);
}

void init_tache_noyau(tache_t *tache, pde32_t *pgd, tache_t *tache_suivante)
{
    memset(tache, 0, sizeof(tache_t));
    tache->pgd      = (pde32_t*)pgd;
    tache->suivante = (tache_t*)tache_suivante;

    debug("\ntache noyau \t: %p\n", tache);
    debug("pgd \t\t: %p\n", pgd);
    debug("tache suivante \t: %p\n", tache_suivante);
}

void init_tache(tache_t  *tache,
                uint32_t  code,
                uint32_t *pile_noyau,
                uint32_t *pile_utilisateur,
                pde32_t  *pgd,
                tache_t  *tache_suivante)
{
    memset(tache, 0, sizeof(tache_t));
    
	tache->pile_noyau = pile_noyau;
	tache->pile_haut  = pile_noyau;

    ((int_ctx_t*)tache->pile_haut)->eip.raw    = code;
    ((int_ctx_t*)tache->pile_haut)->cs.raw     = gdt_usr_seg_sel(GDT_C3);
    ((int_ctx_t*)tache->pile_haut)->eflags.raw = EFLAGS_IF;
    ((int_ctx_t*)tache->pile_haut)->esp.raw    = (uint32_t)pile_utilisateur;
    ((int_ctx_t*)tache->pile_haut)->ss.raw     = gdt_usr_seg_sel(GDT_D3);

    *(--tache->pile_haut) = (offset_t) resume_from_intr;
    *(--tache->pile_haut) = 0xCAFE;

    tache->pgd      = pgd;
    tache->suivante = tache_suivante;

    if((int*)code == (int*)user1)
        debug("\ntache user1\t: %p\n", tache);
    if((int*)code == (int*)user2)
        debug("\ntache user2\t: %p\n", tache);
    debug("eip (code) \t: %p\n", code);
    debug("pile noyau \t: %p\n", tache->pile_noyau);
    debug("pile haut \t: %p\n", tache->pile_haut);
    debug("pile utilisateur: %p\n", pile_utilisateur);
    debug("pgd \t\t: %p\n", pgd);
    debug("tache suivante \t: %p\n", tache_suivante);
}

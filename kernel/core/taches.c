#include <taches.h>

extern void __regparm__(1) bascule();

void ordonnanceur(int_ctx_t *ctx)
{
  debug(".", ctx);

  // uint32_t** sesp = &(current_task->pile_haut);
  // t_courante = t_courante->suivante;
  // set_cr3(t_courante->pgd);
  // TSS.s0.esp = (uint32_t) t_courante->pile_noyeau;
  // bascule(sesp, t_courante->pile_haut);
}

void init_tache(tache_t *tache,
                uint32_t code,
                uint32_t pile_noyeau,
                uint32_t pile_utilisateur,
                tache_t *tache_suivante)
{
  memset(tache, 0, sizeof(tache_t));

  tache->pile_haut->nr.raw     = code;
  tache->pile_haut->cs.raw     = gdt_usr_seg_sel(GDT_C3);
  tache->pile_haut->eflags.raw = get_flags();
  tache->pile_haut->esp.raw    = pile_utilisateur;
  tache->pile_haut->ss.raw     = gdt_usr_seg_sel(GDT_D3);

	tache->pile_noyeau = &pile_noyeau;
	tache->suivante = tache_suivante;	
}

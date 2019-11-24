/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <segmem.h>
#include <string.h>

seg_desc_t GDT[4];

/* Prints the current registered GDT. */
void affiche_gdt()
{
	// GDTR (pointeur vers le registre de segment GDT)
	gdt_reg_t gdtr;

	// Récupère le Global Descriptor-Table Register
	get_gdtr(gdtr);

	// Affiche le contenu de la GDT
	offset_t start = gdtr.addr;
	offset_t end = start + (gdtr.limit - 1);

	debug("\nAdresse de la GDT : 0x%lx\n", gdtr.addr);

	for (offset_t i = start; i < end; i += sizeof(seg_desc_t))
	{
		seg_desc_t *desc = (seg_desc_t *)i;
		uint32_t base = desc->base_3 << 24 | desc->base_2 << 16 | desc->base_1; // concatène base{1,2,3} pour former base
		uint32_t limit = desc->limit_2 << 16 | desc->limit_1;

		char padding = (i == start) ? '\t' : '\0';
		debug("GDT:0x%llx,\t%c", desc->raw, padding);

		debug("B:0x%x", base);			// Base Address (adresse de début du segment)
		debug(", G:%x", desc->g);		// Granularity (0=field limit not scaled, 1=scaled by 4096 bytes)
		debug(", DB:%x", desc->d);		// D/B -> Default Operand Size (1=32-bit, 0=16-bit)
		debug(", AVL:%x", desc->avl);	// AVL -> Available To Software
		debug(", L:0x%x", limit);		// Segment Limit (définit la taille du segment, en octets)
		debug(", P:%x", desc->p);		// Present (1=le segment est chargé en mémoire)
		debug(", DPL:%x", desc->dpl);	// Descriptor Privilege-Level (3, 2 ou 1)
		debug(", S:%x", desc->s);		// S -> Type d'accès : 0=System, 1=User
		debug(", T:0x%x", desc->type);	// Type de segment (LDT, TSS, Gate, Code ou Data)

		debug("\n");
	}
}

//  PARTIE 1 : Créer la GDT (côté OS, en mémoire)
void cree_gdt()
{
	gdt_reg_t gdtr;

	GDT[0].raw = 0ULL; // met tous les 64 bits du premier segment à zéro
	GDT[1].raw = 0ULL; // segment de code
	GDT[2].raw = 0ULL; // segment du données
	GDT[3].raw = 0ULL; // dernier segment

	for (int i = 1; i < 3; i++)
	{
		GDT[i].g = 1;			 // Granularity (multiple d'une page de mémoire physique de 4k)
		GDT[i].d = 1;			 // Default Operand Size (1=32-bit, 0=16-bit)
		GDT[i].avl = 0;			 // Available To Software
		GDT[i].limit_1 = 0xffff; // Limit 1 réglé à 4Go
		GDT[i].limit_2 = 0xf;	 // Limit 2
		GDT[i].p = 1;			 // Present
		GDT[i].dpl = 0;			 // Descriptor Privilege-Level (ring 0)
		GDT[i].s = 1;			 // Type of accès : 0=System, 1=User
		if (i == 1)
		{									// Descriptor Type
			GDT[i].type = SEG_DESC_CODE_XR; // code
		}
		if (i == 2)
		{
			GDT[i].type = SEG_DESC_DATA_RW; // data
		}
	}

	// Paramètre le pointeur de GDT
	gdtr.desc = GDT;
	gdtr.limit = sizeof(GDT) - 1;  // Offset du dernier octet valide dans la GDT
	set_gdtr(gdtr);

	// Définition des registres
	set_cs(gdt_krn_seg_sel(1)); // 	le registre CS comporte 3 champs (cf. diapo 42) : rpl, ti et index.
	set_ss(gdt_krn_seg_sel(2));
	set_ds(gdt_krn_seg_sel(2));
	set_es(gdt_krn_seg_sel(2));
	set_fs(gdt_krn_seg_sel(2));
	set_gs(gdt_krn_seg_sel(2));
}

void copie()
{
	// Modification de la GDT[3]
	uint32_t base = 0x600000; // base 0x600000

	GDT[3].raw = 0;

	GDT[3].base_1 = base;
	GDT[3].base_2 = base >> 16;
	GDT[3].base_3 = base >> 24;

	GDT[3].limit_1 = 32 - 1; // limite 32 octets
	GDT[3].type = SEG_DESC_DATA_RW; // data RW
	GDT[3].d = 1; // 32 bits
	GDT[3].s = 1; // user
	GDT[3].p = 1; // chargé
	GDT[3].dpl = 0; // ring 0

	char src[64];
	char *dst = 0;
	memset(src, 0xff, 64);

	set_es(gdt_krn_seg_sel(3));

	_memcpy8(dst, src, 32);

	//_memcpy8(dst, src, 64);
}

/*
   Question 3 :

	ECX, TAILLE
	ESI, SRC
	EDI, DES
	REP MOVS(W/B/L)

	MOVS utilise 2 registre de segment (DS et ES).
	Il y a des copies de données inter-segment.

	Donc si on essaie de copier plus que 32 octets, on sors de la limite virtuelle du segment de la GDT.
	On crée une faute qui empêche le buffer overflow.
*/

void tp()
{
	debug("\nGDT de GRUB :");
	affiche_gdt();
	cree_gdt();
	debug("\nGDT customisé :");
	affiche_gdt();
	copie();
}
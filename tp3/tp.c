/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <segmem.h>
#include <string.h>

seg_desc_t GDT[6];
tss_t TSS;

extern info_t *info;

#define GDT_C0  1
#define GDT_D0  2
#define GDT_C3  3
#define GDT_D3  4
#define TSS_S0  5

/* Prints the current registered GDT. */
void affiche_gdt()
{
	gdt_reg_t gdtr;
	get_gdtr(gdtr);
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
		debug(", T:0x%x\n", desc->type);// Type de segment (LDT, TSS, Gate, Code ou Data)
	}
}

// Crée les descripteurs de segements
void cree_seg_gdt()
{
	GDT[0].raw = 0ULL; // met tous les 64 bits du premier segment à zéro
	GDT[GDT_C0].raw = 0ULL; // segment de code
	GDT[GDT_D0].raw = 0ULL; // segment des données
	GDT[GDT_C3].raw = 0ULL; // segment de code userland
	GDT[GDT_D3].raw = 0ULL; // segment des données userland

	for (int i = 1; i < 5; i++)
	{
		GDT[i].g = 1;			 // Granularity (multiple d'une page de mémoire physique de 4k)
		GDT[i].d = 1;			 // Default Operand Size (1=32-bit, 0=16-bit)
		GDT[i].avl = 0;			 // Available To Software
		GDT[i].limit_1 = 0xffff; // Limit 1 réglé à 4Go
		GDT[i].limit_2 = 0xf;	 // Limit 2
		GDT[i].p = 1;			 // Present
		GDT[i].dpl = 0;          // Descriptor Privilege-Level (ring 0)
		if (i == GDT_C3 || i == GDT_D3)
		{
			GDT[i].dpl = 3; // ring 3
		}
		GDT[i].s = 1;			 // Type of accès : 0=System, 1=User
		if (i == GDT_C0 || i == GDT_C3)
        {						 // Descriptor Type
			GDT[i].type = SEG_DESC_CODE_XR; // code
		}
		if (i == GDT_D0 || i == GDT_D3)
		{
			GDT[i].type = SEG_DESC_DATA_RW; // data
		}
	}
}

// Crée le du descripteur de tâches
void cree_seg_tss()
{
	GDT[TSS_S0].raw = 0ULL;

	raw32_t addr     = {.raw = (offset_t)&TSS};  
	GDT[TSS_S0].base_1 = addr.wlow;
	GDT[TSS_S0].base_2 = addr._whigh.blow;
	GDT[TSS_S0].base_3 = addr._whigh.bhigh;

	GDT[TSS_S0].limit_1 = sizeof(tss_t) & 0xFFFF;
	GDT[TSS_S0].limit_2 = (sizeof(tss_t) >> 16) & 0xF;

	GDT[TSS_S0].type = SEG_DESC_SYS_TSS_AVL_32;
	GDT[TSS_S0].dpl  = 0; // Granularity
	GDT[TSS_S0].s = 0;    // Type d'accès : 0=System 
	GDT[TSS_S0].p = 1;    // Present
	GDT[TSS_S0].l = 0;    // 32 bits
	GDT[TSS_S0].d = 0;    // Default Operand Size (0=16-bit)
	GDT[TSS_S0].g = 0;    // Granularity
	GDT[TSS_S0].avl = 0;  // Available To Software
}

// Initie la GDT (côté OS, en mémoire)
void init_gdt()
{
	// Paramètre le pointeur de GDT
	gdt_reg_t gdtr;
	gdtr.desc = GDT;
	gdtr.limit = sizeof(GDT) - 1;  // Offset du dernier octet valide dans la GDT
	set_gdtr(gdtr);

	// Définition des registres
	set_cs(gdt_krn_seg_sel(GDT_C0)); // le registre CS comporte 3 champs (cf. diapo 42) : rpl, ti et index.
	set_ss(gdt_krn_seg_sel(GDT_D0));
	set_ds(gdt_krn_seg_sel(GDT_D0));
	set_es(gdt_krn_seg_sel(GDT_D0));
	set_fs(gdt_krn_seg_sel(GDT_D0));
	set_gs(gdt_krn_seg_sel(GDT_D0));
}

void userland()
{
	debug("\nAccès userland !\n");
	// Accès à CR0 intedit en userland
	// asm volatile ("mov %eax, %cr0"); 
}

/** Aide : https://wiki.osdev.org/Getting_to_Ring_3 */
void acces_userland()
{
	// Question 3.1
	// Je peux charger des descripteurs de segments
	// de niveau de privilège inférieurs dans les registres
	// mais ça change pas notre niveau de privilège.
	set_ds(gdt_usr_seg_sel(GDT_D3));
	set_es(gdt_usr_seg_sel(GDT_D3));
	set_fs(gdt_usr_seg_sel(GDT_D3));
	set_gs(gdt_usr_seg_sel(GDT_D3));

	// Question 3.2 (plantage #GP lié à la segmentation)
	// Crash car le niveau de privilège courrant est à la fois lié à cs et à ss
	// set_ss(gdt_usr_seg_sel(GDT_D3));

	// Question 3.3: #GP
	// set_cs(gdt_usr_seg_sel(GDT_C3));
	// fptr32_t fptr;
	// fptr.offset = (uint32_t) userland;	   // chargé dans EIP : adresse de userland
	// fptr.segment = gdt_usr_seg_sel(GDT_C3); // sélecteur : adresse du sélecteur de code
	// farjump(fptr);

	// Création de la Task-State Segment
	// car chaque tâche userland a besoin d'une pile noyeau,
	// si on le fait pas, on a TSS invalide car après avoir déclenché une faute
	// le CPU cherche à gérer la faute dans le noyeau
	TSS.s0.esp = get_ebp(); // ESP0 = pointeur de pile noyeau actuel
	TSS.s0.ss  = gdt_krn_seg_sel(GDT_D0); // SS0 = segment vers la pile noyeau
	set_tr(gdt_krn_seg_sel(TSS_S0));

	// // Question 3.4
	asm volatile(
		"push %0	\n"	// ss de niveau ring 3
		"push %%esp	\n"	// esp (la pile userland qui sera utilisée est dans le noyeau)
		"pushf		\n" // drapeaux du CPU
		"push %1	\n"	// segment de code ring 3
		"push %2	\n"	// nouvel eip = fonction userland
		"iret"
	:: 
		"i"(gdt_usr_seg_sel(GDT_D3)),
		"i"(gdt_usr_seg_sel(GDT_C3)),
		"i"(userland)
	);
}

void tp()
{
	cree_seg_gdt();
	cree_seg_tss();
    init_gdt();
	affiche_gdt();
	acces_userland();
}

#include <userland.h>
#include <segmentation.h>
#include <debug.h>

// Utilisateur n°1
void user1()
{
	debug("\nAccès userland n°1 !\n");

    // Appel de l'interruption "48" de manière logicielle.
	char * chaine = "Bonsoir !";
    asm volatile("int $48"::"a"(0xCAFE), "b"(chaine)); ; // -> GP si IDT non définie
	
    while(1);
}

// Démarre une tâche userland
void userland()
{
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
		"i"(user1)
	);
}

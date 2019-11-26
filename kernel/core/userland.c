#include <userland.h>
#include <segmentation.h>
#include <debug.h>

// Écrit un compteur (uint32_t) dans la zone de mémoire partagée
void __attribute__((section(".user1"))) user1()
{
	debug("\nAccès userland n°1 !\n");

    while(1);
}

// Affiche le compteur depuis la zone de mémoire partagée
void __attribute__((section(".user2"))) user2()
{
	debug("\nAccès userland n°2 !\n");
	
	// Appel système via l'interruption 0x80
	asm volatile("int $0x80"::"S"(20));
    
	while(1);
}

// Démarre une tâche en mode utilisateur
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

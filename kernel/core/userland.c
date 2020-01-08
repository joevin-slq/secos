#include <userland.h>
#include <debug.h>
#include <cartographie.h>

uint32_t* __attribute__((section(".user1_cpt"))) compteur_user1 = (uint32_t*)U1_CPT;
uint32_t* __attribute__((section(".user2_cpt"))) compteur_user2 = (uint32_t*)U2_CPT;

// Écrit un compteur (uint32_t) dans la zone de mémoire partagée
void __attribute__((section(".user1"))) user1()
{
	compteur_user1 = (uint32_t*)0xC00000;
    // *compteur_user1 = 0;

	// debug("-- Compteur user1 : %d\n", *compteur_user1);
	
    while(1)
	{
		(*compteur_user1)++;
	}
}

// Affiche le compteur depuis la zone de mémoire partagée
void __attribute__((section(".user2"))) user2()
{
	compteur_user2 = (uint32_t*)0xC00000;

	// debug("-- Compteur user2 : %d\n", *compteur_user2);

	// Appel système via l'interruption 0x80
	while(1){
			asm volatile("int $0x80"::"S"(compteur_user2));
	}
}

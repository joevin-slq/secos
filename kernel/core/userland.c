#include <userland.h>
#include <debug.h>
#include <cartographie.h>

uint32_t* __attribute__((section(".user1_cpt"))) compteur_user1 = (uint32_t*)U1+0x1000;
uint32_t* __attribute__((section(".user2_cpt"))) compteur_user2 = (uint32_t*)U2+0x1000;

// Écrit un compteur (uint32_t) dans la zone de mémoire partagée
void __attribute__((section(".user1"))) user1()
{
	if( *compteur_user1 > 100) {
		*compteur_user1 = 0;
	} else {
		(*compteur_user1)++;
	}
	
    while(1);
}

// Affiche le compteur depuis la zone de mémoire partagée
void __attribute__((section(".user2"))) user2()
{
	// Appel système via l'interruption 0x80
	asm volatile("int $0x80"::"S"(compteur_user2));

	while(1);
}


#define PGD_N           0x100000
#define PTB_N           0x101000

#define PGD_U1          0x120000
#define PTB_U1_PILE_N   0x121000
#define PTB_U1_PILE_U   0x122000
#define PTB_U1_PARTAGE  0x123000

#define PGD_U2          0x140000
#define PTB_U2_PILE_N   0x141000
#define PTB_U2_PILE_U   0x142000
#define PTB_U2_PARTAGE  0x143000

// Pile noyeau :
#define PILE_N1         0x220000
#define PILE_N2         0x240000

/* Adresses utilisateurs */
#define U1              0x400000 // utilisateur 1 : 0x400000 à 0x800000
#define U1_CPT          0x401000
#define PILE_U1         0x7F0000
#define U2              0x800000 // utilisateur 2 : 0x800000 à 0xC00000
#define U2_CPT          0x801000
#define PILE_U2         0xBF0000
#define PARTAGE         0xD00000 // mémoire partagée

/*
Adresses physiques :
	Utilisateur 1 : 
		P noy	: 0x00000000 à 0x00400000
		P uti 	: 0x00400000 à 0x00800000
		Partage	: 0x00C00000 à 0x01000000
	Utilisateur 2 : 
		P noy	: 0x00000000 à 0x00400000
		P uti	: 0x00800000 à 0x00C00000
		Partage	: 0x00C00000 à 0x01000000
	Partage :
		P1 		: 0xD00000
*/
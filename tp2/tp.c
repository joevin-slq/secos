/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>
#include <segmem.h>
#include <asm.h>
#include <intr.h>
// #pragma GCC diagnostic ignored "-Wunused-but-set-variable"

extern info_t *info;

void question1()
{
    // cf https://c9x.me/x86/html/file_module_x86_id_304.html
    force_interrupts_on(); // sti
    while(true);
}


void bp_handler()
{
    uint32_t adresse;
    // Met l'adresse 4(%%EBP), soit la base de pile -4 instructions dans le registre EAX.
    // adresse contiendra l'adresse de retour de la fonction bp_handler()
    // soit l'adresse juste après "int3" dans bp_trigger().
    asm volatile ("mov 4(%%ebp), %%eax" : "=a" (adresse) :: );
    debug("Valeur de EIP : 0x%x\n", adresse);

    // nettoie la pile
    asm volatile("leave");

    // interruption
    debug("Interruption bp_handler\n");
    
    // retour d'interruption
    asm volatile("iret");
}

void bp_trigger()
{
    asm volatile ("int3");
}

void modif_idt()
{
    idt_reg_t idtr;
    get_idtr(idtr); // récupère la IDTR
    debug("\nAdresse de l'IDT : 0x%x\n", idtr.addr);

    offset_t idt = idtr.addr; // IDT

    idt += sizeof(int_desc_t) * 3; // Accès au descripteur #BP

    int_desc_t *magie = (int_desc_t *)idt; // Changement de type

    // Branchement du gestionnaire d'interruption
    int_desc(magie, gdt_krn_seg_sel(1), (offset_t)bp_handler);
}

void tp()
{
    // question1();
    modif_idt();
    bp_trigger();
}

/*
* Question 3.5 :
* Lors d'un #BP (breakpoint)
* On est en ring0 donc pas de changement de privilège
* Déroutage de l'instruction, appel de l'IDT
* Appel de la fct qui affiche un message
* 0xBreakPoint handler
* Le CPU sauvegarde le contexte dans la pile
* Le CPU prends l'exécution au niveau de bp_handler()
* 
* Au moment où elle se termine y'a un "ret"
* vers l'adresse de retour dans la pile 
* MAIS il vas sauter n'importe où
* (pile potentiellement mal ré-alignée)
* DONC bp_handler ne doit pas faire ret mais plutôt un iret
* mais le iret ne peut pas se faire en C
* le iret se fait uniquement en assembleur inline.
*/
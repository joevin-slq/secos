
#include <debug.h>
#include <syscall.h>

void syscall_isr()
{
   asm volatile (
      "leave ; pusha        \n"
      "mov %esp, %eax       \n"
      "call syscall_handler \n"
      "popa ; iret" // "
      );
}

/* Appel système qui affiche une chaîne de caractères */
void __regparm__(1) syscall_handler(int_ctx_t *ctx)
{
   debug("SYSCALL eax = %p\n", ctx->gpr.eax);
   debug("chaine = %s\n", ctx->gpr.ebx);
}

void syscall() // Appel de l'interruption "48" en ring 3
{
	idt_reg_t idtr;	 // IDTR
	int_desc_t *dsc; // Descripteur d'interruption

	get_idtr(idtr);  // Déjà défini, voir intr.c
	dsc = &idtr.desc[48];
	dsc->dpl = 3;    // Accès ring 3

	// Définit syscall_isr() tel que gestionnaire d'interruption n°48:
	raw32_t syscall_isr_addr = { .raw = (uint32_t) syscall_isr };
	dsc->offset_1 = syscall_isr_addr.wlow;
	dsc->offset_2 = syscall_isr_addr.whigh;
}
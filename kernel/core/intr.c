/* GPLv2 (c) Airbus */
#include <intr.h>
#include <debug.h>
#include <info.h>
#include <asm.h>
#include <taches.h>

extern info_t *info;
extern void idt_trampoline();
static int_desc_t IDT[IDT_NR_DESC];

void intr_init()
{
   idt_reg_t idtr;
   offset_t  isr;
   size_t    i;

   isr = (offset_t)idt_trampoline;

   /* re-use default grub GDT code descriptor */
   for(i=0 ; i<IDT_NR_DESC ; i++, isr += IDT_ISR_ALGN)
      int_desc(&IDT[i], gdt_krn_seg_sel(1), isr);

   idtr.desc  = IDT;
   idtr.limit = sizeof(IDT) - 1;

   idtr.desc[0x80].dpl = 3;

   set_idtr(idtr);

   // Active l'interruption 0x20
   force_interrupts_on();
}

/* Interruption déclenchée à chaque tour d'horloge */
void int20_handler(int_ctx_t *ctx)
{
   ordonnanceur(ctx);
}

/* Appel système pour user1 :
 * -> Affiche le compteur depuis la zone de mémoire partagée. 
 * TODO : Vérifier que l'adresse est valide. */
void int80_handler(int_ctx_t *ctx)
{	
   uint32_t* counter = (uint32_t*) ctx->gpr.esi.raw;
	debug("INT80 : &esi = %p, esi = %d\n", counter, counter);
}

void __regparm__(1) intr_hdlr(int_ctx_t *ctx)
{
   uint8_t vector = ctx->nr.blow;

   switch(vector) {
      case 0x20:
         int20_handler(ctx);
         break;
      case 0x80:
         int80_handler(ctx);
         break;
      default:
         debug("\nIDT event\n"
               " . int    #%d\n"
               " . error  0x%x\n"
               " . cs:eip 0x%x:0x%x\n"
               " . ss:esp 0x%x:0x%x\n"
               " . eflags 0x%x\n"
               "\n- GPR\n"
               "eax     : 0x%x\n"
               "ecx     : 0x%x\n"
               "edx     : 0x%x\n"
               "ebx     : 0x%x\n"
               "esp     : 0x%x\n"
               "ebp     : 0x%x\n"
               "esi     : 0x%x\n"
               "edi     : 0x%x\n"
               ,ctx->nr.raw, ctx->err.raw
               ,ctx->cs.raw, ctx->eip.raw
               ,ctx->ss.raw, ctx->esp.raw
               ,ctx->eflags.raw
               ,ctx->gpr.eax.raw
               ,ctx->gpr.ecx.raw
               ,ctx->gpr.edx.raw
               ,ctx->gpr.ebx.raw
               ,ctx->gpr.esp.raw
               ,ctx->gpr.ebp.raw
               ,ctx->gpr.esi.raw
               ,ctx->gpr.edi.raw);

         if(vector < NR_EXCP)
            excp_hdlr(ctx);
         else
            debug("ignore IRQ %d\n", vector);
         break;
   }
}

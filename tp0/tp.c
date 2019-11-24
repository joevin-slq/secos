/* GPLv2 (c) Airbus */
#include <debug.h>
#include <info.h>

extern info_t   *info;
extern uint32_t __kernel_start__;
extern uint32_t __kernel_end__;

void tp() {
   debug("kernel mem [0x%x - 0x%x]\n", &__kernel_start__, &__kernel_end__);
   debug("MBI flags 0x%x\n", info->mbi->flags);
   
   debug("mmap_addr   -> 0x%x\n", info->mbi->mmap_addr);
   debug("mmap_length -> 0x%x\n", info->mbi->mmap_length);

   multiboot_memory_map_t *start;
   multiboot_memory_map_t *end;

   start = (multiboot_memory_map_t*) info->mbi->mmap_addr;
   end   = (multiboot_memory_map_t*) (info->mbi->mmap_addr + info->mbi->mmap_length);

   while(start < end) {
   	debug("->addr : 0x%x\t", start->addr);
      debug("->size : 0x%x\t", start->size);
      debug("->len  : 0x%x\t", start->len);
      debug("->type : 0x%x\t", start->type);
      debug("\n");
   	start++;
   }

   // On écrit en dehors de la mémoire
   char * p = (char*) 0xFFFFFFFF;
   *p = 'A'; // pas de faute générée
}

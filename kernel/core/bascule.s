.text

.globl bascule
.type  bascule,"function"

/*
** Bascule entre les deux taches
*/
bascule:
	/* push de ebp */
	pushl %ebp
	
	/* changement de pile */
	movl %esp, (%ecx)
	movl %edx, %esp

	/* pop de ebp */
	popl   %ebp

	/* ret */
	ret

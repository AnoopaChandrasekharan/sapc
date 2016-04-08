# like linux SAVE_MOST and RESTORE_MOST macros in irq.h

.globl _syscall, _syscallc
.text

_syscall:
	pushl %edx	# save C scratch regs
  pushl %ecx
	pushl %ebx
	pushl %eax

	call _syscallc  # call C trap handler
	popl %eax
	popl %ebx
	popl %ecx
  popl %edx
	iret

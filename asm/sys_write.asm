global sys_write

section .text

sys_write:
	mov     rax, 1
	syscall

.done:
	ret

global sys_write								; declare global symbol

section .text									; code section

sys_write:
	mov     rax, 1								; set syscall number to 1 (sys_write on Linux x86-64)
	syscall 									; invoke kernel syscall

.done:
	ret     									; return to caller (no explicit handling of return value here)

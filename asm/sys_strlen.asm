global sys_strlen								; declare global symbol

section .text									; code section

sys_strlen:
	xor     rax, rax							; set rax to 0 (this will be used as the length counter)

.loop:
	cmp     byte [rdi + rax], 0					; check if current character at rdi+rax is null terminator
	je      .done								; if zero, we've reached end of string, exit loop

	inc     rax									; increment length counter
	jmp     .loop								; repeat loop

.done:
	ret     									; return length in rax

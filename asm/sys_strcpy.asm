global sys_strcpy

section .text

sys_strcpy:
	mov     rax, rdi

.copy:
	mov     dl, [rsi]
	mov     [rdi], dl

	inc     rsi
	inc     rdi

	cmp     dl, 0
	jne     .copy

.done:
	ret

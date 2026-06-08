global sys_strlen

section .text

sys_strlen:
	xor     rax, rax

.loop:
	cmp     byte[rdi + rax], 0
	je      .done

	inc     rax
	jmp     .loop

.done:
	ret

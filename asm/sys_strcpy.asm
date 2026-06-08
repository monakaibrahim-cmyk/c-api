global sys_strcpy								; declare global symbol

section .text									; code section

sys_strcpy:
	mov     rax, rdi							; copy destination pointer (rdi) into return register (rax)

.copy:
	mov     dl, [rsi]							; load 1 byte from source string (rsi) into dl
	mov     [rdi], dl							; store that byte into destination (rdi)

	inc     rsi									; move source pointer forward by 1 byte
	inc     rdi									; move destination pointer forward by 1 byte

	cmp     dl, 0								; check if the byte just copied was null terminator (0)
	jne     .copy								; if not zero, continue copying loop

.done:
	ret     									; return to caller (rax already holds original destination pointer)

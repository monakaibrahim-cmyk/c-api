global sys_timestamp

section .rodata
default_fmt     db      "%Y-%m-%d %H:%M:%S.%L", 0

month_days      db      31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31

section .text

sys_timestamp:
	push    rbp
	mov     rbp, rsp
	push    rbx
	push    r12
	push    r13
	push    r14
	push    r15

	sub     rsp, 16

	test    rsi, rsi
	jnz     .format_ok
	lea     rsi, [rel default_fmt]

.format_ok:
	mov     r12, rdi
	mov     r13, rsi

	mov     rax, 96
	mov     rdi, rsp
	xor     rsi, rsi
	syscall

	mov     rax, [rsp]
	mov     rcx, [rsp + 8]

	push    rax
	mov     rax, rcx
	mov     rcx, 1000
	xor     rdx, rdx
	div     rcx
	mov     r11, rax
	pop     rax

	mov     rcx, 86400
	xor     rdx, rdx
	div     rcx
	mov     r15, rdx
	mov     r14, rax

	mov     r8, 1970
.year_loop:
	mov     r9, 365

	mov     rax, r8
	and     rax, 3
	jnz     .not_leap
	mov     rax, r8
	xor     rdx, rdx
	mov     rcx, 100
	div     rcx
	test    rdx, rdx
	jnz     .is_leap
	mov     rax, r8
	xor     rdx, rdx
	mov     rcx, 400
	div     rcx
	test    rdx, rdx
	jnz     .not_leap

.is_leap:
	mov     r9, 366
.not_leap:
	cmp     r14, r9
	jl      .year_found
	sub     r14, r9
	inc     r8
	jmp     .year_loop

.year_found:
	mov     r10, 1
	lea     rbx, [rel month_days]

.month_loop:
	movzx   rcx, byte [rbx + r10 - 1]
	cmp     r10, 2
	jne     .check_month
	cmp     r9, 366
	jne     .check_month
	inc     rcx
.check_month:
	cmp     r14, rcx
	jl      .month_found
	sub     r14, rcx
	inc     r10
	jmp     .month_loop

.month_found:
	inc     r14

	mov     rax, r15
	mov     rcx, 3600
	xor     rdx, rdx
	div     rcx
	mov     r15, rax

	mov     rax, rdx
	mov     rcx, 60
	xor     rdx, rdx
	div     rcx

	push    r8
	push    r10
	push    r14
	push    r15
	push    rax
	push    rdx
	push    r11

.parse_loop:
	mov     al, byte [r13]
	test    al, al
	jz      .done

	cmp     al, '%'
	je      .handle_token

	mov     byte [r12], al
	inc     r12
	inc     r13
	jmp     .parse_loop

.handle_token:
	inc     r13
	mov     al, byte [r13]

	cmp     al, 'Y'
	je      .fmt_year
	cmp     al, 'm'
	je      .fmt_month
	cmp     al, 'd'
	je      .fmt_day
	cmp     al, 'H'
	je      .fmt_hour
	cmp     al, 'M'
	je      .fmt_minute
	cmp     al, 'S'
	je      .fmt_second
	cmp     al, 'L'
	je      .fmt_milli

	mov     byte [r12], '%'
	inc     r12
	mov     byte [r12], al
	inc     r12
	inc     r13
	jmp     .parse_loop

.fmt_year:
	mov     rax, [rsp + 48]
	call    write_4digits
	inc     r13
	jmp     .parse_loop
.fmt_month:
	mov     rax, [rsp + 40]
	call    write_2digits
	inc     r13
	jmp     .parse_loop
.fmt_day:
	mov     rax, [rsp + 32]
	call    write_2digits
	inc     r13
	jmp     .parse_loop
.fmt_hour:
	mov     rax, [rsp + 24]
	call    write_2digits
	inc     r13
	jmp     .parse_loop
.fmt_minute:
	mov     rax, [rsp + 16]
	call    write_2digits
	inc     r13
	jmp     .parse_loop
.fmt_second:
	mov     rax, [rsp + 8]
	call    write_2digits
	inc     r13
	jmp     .parse_loop
.fmt_milli:
	mov     rax, [rsp]
	call    write_3digits
	inc     r13
	jmp     .parse_loop

.done:
	mov     byte [r12], 0

	add     rsp, 56
	add     rsp, 16

	pop     r15
	pop     r14
	pop     r13
	pop     r12
	pop     rbx
	pop     rbp
	ret

write_4digits:
	mov     rcx, 1000
	xor     rdx, rdx
	div     rcx
	add     al, '0'
	mov     byte [r12], al
	inc     r12
	mov     rax, rdx
	mov     rcx, 100
	xor     rdx, rdx
	div     rcx
	add     al, '0'
	mov     byte [r12], al
	inc     r12
	mov     rax, rdx
write_2digits:
	mov     rcx, 10
	xor     rdx, rdx
	div     rcx
	add     al, '0'
	add     dl, '0'
	mov     byte [r12], al
	inc     r12
	mov     byte [r12], dl
	inc     r12
	ret

write_3digits:
	mov     rcx, 100
	xor     rdx, rdx
	div     rcx
	add     al, '0'
	mov     byte [r12], al
	inc     r12
	mov     rax, rdx
	jmp     write_2digits

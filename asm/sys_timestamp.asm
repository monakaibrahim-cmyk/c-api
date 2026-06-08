global sys_timestamp							; declare global symbol so linker can see sys_timestamp

section .rodata									; read-only data section
default_fmt     db      "%Y-%m-%d %H:%M:%S.%L", 0	; default timestamp format string

month_days      db      31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31	; days per month

section .text									; code section

sys_timestamp:
	push    rbp									; save base pointer
	mov     rbp, rsp							; set stack frame
	push    rbx									; save callee-saved registers
	push    r12
	push    r13
	push    r14
	push    r15

	sub     rsp, 16								; allocate stack space

	test    rsi, rsi							; check if format string is NULL
	jnz     .format_ok
	lea     rsi, [rel default_fmt]				; if NULL, use default format

.format_ok:
	mov     r12, rdi							; output buffer pointer
	mov     r13, rsi							; format string pointer

	mov     rax, 96								; syscall number (likely gettimeofday or similar)
	mov     rdi, rsp							; pointer to struct timeval buffer
	xor     rsi, rsi
	syscall 									; call kernel

	mov     rax, [rsp]							; seconds part
	mov     rcx, [rsp + 8]						; microseconds part

	push    rax									; save seconds
	mov     rax, rcx
	mov     rcx, 1000
	xor     rdx, rdx
	div     rcx									; convert microseconds to milliseconds
	mov     r11, rax							; store milliseconds
	pop     rax									; restore seconds

	mov     rcx, 86400
	xor     rdx, rdx
	div     rcx									; split into days and seconds
	mov     r15, rdx							; seconds within day
	mov     r14, rax							; total days since epoch

	mov     r8, 1970							; starting year
.year_loop:
	mov     r9, 365								; assume non-leap year

	mov     rax, r8
	and     rax, 3
	jnz     .not_leap							; check leap year condition (fast mod 4)

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
	mov     r9, 366								; leap year has 366 days

.not_leap:
	cmp     r14, r9
	jl      .year_found							; stop when correct year found

	sub     r14, r9								; subtract days in year
	inc     r8									; increment year
	jmp     .year_loop

.year_found:
	mov     r10, 1								; month index starts at 1
	lea     rbx, [rel month_days]

.month_loop:
	movzx   rcx, byte [rbx + r10 - 1]			; get days in current month
	cmp     r10, 2
	jne     .check_month
	cmp     r9, 366
	jne     .check_month
	inc     rcx									; adjust February for leap year

.check_month:
	cmp     r14, rcx
	jl      .month_found						; found correct month
	sub     r14, rcx
	inc     r10
	jmp     .month_loop

.month_found:
	inc     r14									; day of month (1-based)

	mov     rax, r15
	mov     rcx, 3600
	xor     rdx, rdx
	div     rcx									; convert seconds to hours
	mov     r15, rax

	mov     rax, rdx
	mov     rcx, 60
	xor     rdx, rdx
	div     rcx									; convert remainder to minutes

	push    r8									; push year
	push    r10									; push month
	push    r14									; push day
	push    r15									; push hour
	push    rax									; push minutes
	push    rdx									; push seconds
	push    r11									; push milliseconds

.parse_loop:
	mov     al, byte [r13]						; read format char
	test    al, al
	jz      .done								; end of format string

	cmp     al, '%'
	je      .handle_token						; handle format specifier

	mov     byte [r12], al						; copy literal character
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
	call    write_4digits						; write year
	inc     r13
	jmp     .parse_loop

.fmt_month:
	mov     rax, [rsp + 40]
	call    write_2digits						; write month
	inc     r13
	jmp     .parse_loop

.fmt_day:
	mov     rax, [rsp + 32]
	call    write_2digits						; write day
	inc     r13
	jmp     .parse_loop

.fmt_hour:
	mov     rax, [rsp + 24]
	call    write_2digits						; write hour
	inc     r13
	jmp     .parse_loop

.fmt_minute:
	mov     rax, [rsp + 16]
	call    write_2digits						; write minute
	inc     r13
	jmp     .parse_loop

.fmt_second:
	mov     rax, [rsp + 8]
	call    write_2digits						; write second
	inc     r13
	jmp     .parse_loop

.fmt_milli:
	mov     rax, [rsp]
	call    write_3digits						; write milliseconds
	inc     r13
	jmp     .parse_loop

.done:
	mov     byte [r12], 0						; null-terminate output string

	add     rsp, 56								; clean up pushed values
	add     rsp, 16								; free local stack space

	pop     r15									; restore registers
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

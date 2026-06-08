global _start									; declare program entry point for linker

extern main										; declare external C main function

section .text									; code section

_start:
	mov     rdi, [rsp]							; load argc (argument count) from stack into rdi
	lea     rsi, [rsp + 8]						; load pointer to argv array into rsi

	call    main								; call main(argc, argv)

	mov     rdi, rax							; move main return value into rdi (exit code)
	mov     rax, 60								; set syscall number 60 (exit)
	syscall 									; invoke kernel to exit program

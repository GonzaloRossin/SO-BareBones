GLOBAL cpuVendor
GLOBAL _sti
GLOBAL _cli
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL irq0Handler 
EXTERN irqDispatcher

section .text

%macro pushState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

;enable interrupts
_sti:
	sti
	ret
;disable interrupts
_cli:
	cli
	ret

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn
;apply mask to slave pic

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn

;int 20h
irq0Handler:
	mov rdi,0
	call irqDispatcher

	mov al,20h
	out 20h,al

	iretq

;int 80h
;this one is special because it is used as a
;interface between kernel and user space
extern int80Dispatcher
GLOBAL interrupt80
interrupt80:
	sti
	push rbp
	mov rbp, rsp

	call int80Dispatcher

	;save the rax value where the return value will be returned
	push rax
	mov al, 20h
	out 20h, al
	pop rax

	mov rsp, rbp
	pop rbp

	iretq
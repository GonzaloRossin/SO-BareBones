GLOBAL cpuVendor
GLOBAL _sti
GLOBAL _cli
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL irq0Handler 
GLOBAL irq1Handler
GLOBAL get_key
EXTERN int80Dispatcher
GLOBAL int_80
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
%macro irqHandlerMaster 1

	pushState

	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq
%endmacro
picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn

;int 20h
irq0Handler:
	irqHandlerMaster 0
;int 21h
irq1Handler
	irqHandlerMaster 1

;basically checks that keyboard has input and then gets that input
;if it doesnt have then return 0xffff
get_key:
	xor rax,rax
	in al,64h
	and al,0x01
	cmp al,0x01
	jne readCharFromKeyboard

	in ax,60h
	mov ah,0x00
	ret
readCharFromKeyboard:
	mov eax,0xffff
	ret

;int 80h
int_80:
	sti
	push rbp
	mov rbp, rsp

	push rcx
	mov rcx, rax
	call int80Dispatcher

	pop rcx

	mov rsp, rbp
	pop rbp 

	iretq

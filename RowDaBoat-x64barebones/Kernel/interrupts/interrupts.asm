
GLOBAL _sti
GLOBAL _cli
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL irq0Handler 
GLOBAL irq1Handler
GLOBAL get_key
EXTERN int80Dispatcher
GLOBAL int_80
GLOBAL Halt
GLOBAL exception0

EXTERN irqDispatcher
EXTERN printRegister
EXTERN printException
EXTERN reboot



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

;enable interrupts
_sti:
	sti
	ret
;disable interrupts
_cli:
	cli
	ret

;Halt function
Halt:
	hlt
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
irq1Handler:
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
	call int80Dispatcher

	push rax
	mov al, 20h
	out 20h, al
	pop rax

	iretq

%macro exception 1
	;preserve registers
	push rbp
	mov rbp,rsp
	pushState
	;print exception data
	mov rdi,%1
	call printException
	mov rdi,0;rsi
	call printRegister
	mov rdi,1;rax
	mov rsi,rax
	call printRegister
	mov rdi,2;rbx
	mov rsi,rbx
	call printRegister
	mov rdi,3;rcx
	mov rsi,rcx
	call printRegister
	mov rdi,4;rdx
	mov rsi,rdx
	call printRegister
	mov rdi,5;rsp
	mov rsi,[rbp+32]
	call printRegister
	mov rdi,6;rbp
	mov rsi,[rbp];rbp apunta al push de rbp
	call printRegister
	sti
	call reboot
	cli
	hlt
%endmacro

;division by zero exception
GLOBAL exception0
exception0:
	exception 0
	
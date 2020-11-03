GLOBAL cpuVendor
GLOBAL cpuVersion
GLOBAL reboot_asm
GLOBAL saveRegs
GLOBAL getRegs

section .text


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

cpuVersion:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 1
	cpuid


	mov [rdi], eax


	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret
reboot_asm:
	mov al,0xf0
	out 64h,al
	cli
infiniteLoop:
	hlt
	jmp infiniteLoop
	ret
saveRegs:
    push rbp
    mov QWORD [regs + 32], rbp ;ver
    mov rbp, rsp

    mov QWORD [regs], rax 
    mov QWORD [regs + 8], rbx
    mov QWORD [regs + 16], rcx
    mov QWORD [regs + 24], rdx

    mov QWORD [regs + 40], rdi
    mov QWORD [regs + 48], rsi
    mov QWORD [regs + 56], r8
    mov QWORD [regs + 64], r9
    mov QWORD[regs + 72], r10
    mov QWORD[regs + 80], r11
	mov QWORD[regs + 88], r12
	mov QWORD[regs + 96], r13
	mov QWORD[regs + 104], r14
	mov QWORD[regs + 112], r15
    mov QWORD[regs + 120], rbp ; me estoy guardando es rsp antes de llamarlo

    mov rsp, rbp
    pop rbp
    ret

getRegs:
    push rbp
    mov rbp, rsp

    mov rax, [regs];
    mov QWORD [rdi], rax;rax 
    mov rax, [regs + 8];rbx
    mov QWORD [rdi + 8], rax
    mov rax, [regs + 16];rcx
    mov QWORD [rdi + 16], rax
    mov rax, [regs + 24];rdx
    mov QWORD [rdi + 24], rax
	mov rax, [regs + 32];rbp
    mov QWORD [rdi + 32], rax
    mov rax, [regs + 40];rdi
    mov QWORD [rdi + 40], rax
    mov rax, [regs + 48];rsi
    mov QWORD [rdi + 48], rax
    mov rax, [regs + 56];r8
    mov QWORD [rdi + 56], rax
    mov rax, [regs + 64];r9
    mov QWORD [rdi + 64], rax
    mov rax, [regs + 72];r10
    mov QWORD [rdi + 72], rax
    mov rax, [regs + 80];r11
    mov QWORD [rdi + 80], rax
    mov rax, [regs + 88];r12
    mov QWORD [rdi + 88], rax
    mov rax, [regs + 96];r13
    mov QWORD [rdi + 96], rax
    mov rax, [regs + 104];r14
    mov QWORD [rdi + 104], rax
    mov rax, [regs + 112];r15
    mov QWORD [rdi + 112], rax
    mov rax, [regs + 120];rsp
    mov QWORD [rdi + 120], rax

    mov rsp, rbp
    pop rbp
    ret


section .bss
regs resq 16
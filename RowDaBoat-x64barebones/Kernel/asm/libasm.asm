GLOBAL cpuVendor
GLOBAL cpuVersion
GLOBAL reboot_asm

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
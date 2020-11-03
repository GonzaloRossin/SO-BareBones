GLOBAL read
GLOBAL sys_write
GLOBAL get_input
GLOBAL newline
GLOBAL get_CPUvendor
GLOBAL get_InfoReg
GLOBAL get_Memory
GLOBAL putChar

section .text

%macro pushState 0
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
%endmacro


%macro syscall_adjust 0
	mov rcx,rdx
	mov rdx,rsi
	mov rsi,rdi
%endmacro

read:
	pushState
	syscall_adjust
	mov rdi,0 ;sys_call read is call number 0
	int 80h
	popState
	ret

sys_write:
	pushState
	syscall_adjust
	mov rdi,1;sys_call write is call number 1
	int 80h
	popState
	ret
get_Memory:
    pushState
	syscall_adjust
    mov rdi,2
    int 80h
    popState
    ret
newline:
	pushState
	syscall_adjust
	mov rdi,3
	int 80h
	popState
	ret
get_CPUvendor:
	pushState
	syscall_adjust
	mov rdi,4
	int 80h
	popState
	ret
get_InfoReg:
	pushState
	syscall_adjust
	mov rdi,5
	int 80h
	popState
	ret
putActioncall:
	pushState
	syscall_adjust
	mov rdi,7
	int 80h
	popState
	ret

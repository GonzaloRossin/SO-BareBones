GLOBAL read_input
GLOBAL sys_write
GLOBAL get_input
GLOBAL newline
GLOBAL get_CPUvendor
GLOBAL get_InfoReg
GLOBAL get_Memory
GLOBAL put_char
GLOBAL putActioncall
GLOBAL get_RTC
GLOBAL print_num
GLOBAL clearScreen
GLOBAL sys_draw
GLOBAL get_seconds

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

read_input:
	pushState
	syscall_adjust
	mov rdi,0 ;sys_call  0
	int 80h
	popState
	ret

sys_write:
	pushState
	syscall_adjust
	mov rdi,1 ;sys_call 1
	int 80h
	popState
	ret
get_Memory:
    pushState
	syscall_adjust
    mov rdi,2 ;sys_call 2
    int 80h
    popState
    ret
newline:
	pushState
	syscall_adjust
	mov rdi,3 ;sys_call 3
	int 80h
	popState
	ret
get_CPUvendor:
	pushState
	syscall_adjust
	mov rdi,4 ;sys_call 4
	int 80h
	popState
	ret
get_InfoReg:
	pushState
	syscall_adjust
	mov rdi,5 ;sys_call 5
	int 80h
	popState
	ret
put_char:
	pushState
	syscall_adjust
	mov rdi,6 ;sys_call 6
	int 80h
	popState
	ret
putActioncall:
	pushState
	syscall_adjust
	mov rdi,7 ;sys_call 7
	int 80h
	popState
	ret
get_RTC:;syscall to acces time from real time clock
	pushState
	syscall_adjust
	mov rdi,8 ;sys_call 8
	int 80h
	popState
	ret
print_num:
	pushState
	syscall_adjust
	mov rdi,9 ;sys_call 9
	int 80h
	popState
	ret
clearScreen:
	pushState
	syscall_adjust
	mov rdi,10 ;sys_call 10
	int 80h
	popState
	ret
sys_draw:
	pushState
	syscall_adjust
	mov rdi,11 ;sys_call 11
	int 80h
	popState
	ret
get_seconds:
	pushState
	syscall_adjust
	mov rdi,12;sys_call 12
	int 80h
	popState
	ret



GLOBAL cpuVendor
GLOBAL cpuVersion
GLOBAL saveRegs
GLOBAL getRegs
GLOBAL saveMemory
GLOBAL getMemory
GLOBAL readCMOS
GLOBAL _start_process
GLOBAL _halt_and_wait
GLOBAL spin_lock
GLOBAL spin_unlock
EXTERN exit
GLOBAL _int81
GLOBAL _halter



section .text


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
saveMemory:
    push rbp
    mov rbp, rsp

    push rcx
    mov rcx, 0

.ciclo:   
    cmp rcx, 32
    je .end 
    mov al, [rdi + rcx]
    mov [v + rcx], al
    inc rcx
    jmp .ciclo

.end: pop rcx
    
    mov rsp, rbp
    pop rbp
    ret

_start_process:
    push rbx
    mov rbx, rdi       ; main
    mov rdi, rsi       ; argc
    mov rsi, rdx       ; argv

    call rbx           ; Llamada al main

    call exit

    pop rbx
    ret 
_halt_and_wait:

    mov al, 20h
	out 20h, al
    sti

    hlt
    ret

_halter:
_beg:
    sti
    hlt
    jmp _beg
    ret

_int81:
    int 81h
    ret

getMemory:
 push rbp
    mov rbp, rsp

    push rcx
    mov rcx, 0

ciclo2:   
    cmp rcx, 32
    je end2
    mov al, [v + rcx]
    mov [rdi + rcx], al
    inc rcx
    jmp ciclo2

end2: 
    pop rcx
    
    mov rsp, rbp
    pop rbp
    ret
;Function to access real time clock
readCMOS:
	cli
	xor rax,rax
	mov al,dil
	out 70h,al
	in al,71h
	sti
	ret
%macro recover 0
	pop r15
	pop r13
	pop r12
	pop rbx
%endmacro
%macro preserve 0
	push rbx
    push r12
    push r13
    push r15
%endmacro

%macro BuildSF 0
    push rbp
    mov rbp, rsp
%endmacro

%macro breakSF 0
    mov rsp, rbp
    pop rbp
%endmacro
spin_lock:
    BuildSF
    preserve

    mov     al, 1          ; Set the EAX register to 1.

    xchg    al, [rdi]       ; Atomically swap the EAX register with
                            ;  the lock variable.
                            ; This will always store 1 to the lock, leaving
                            ;  the previous value in the EAX register.

    test    al, al          ; Test EAX with itself. Among other things, this will
                            ;  set the processor's Zero Flag if EAX is 0.
                            ; If EAX is 0, then the lock was unlocked and
                            ;  we just locked it.
                            ; Otherwise, EAX is 1 and we didn't acquire the lock.

    jnz     spin_lock       ; Jump back to the MOV instruction if the Zero Flag is
                            ;  not set; the lock was previously locked, and so
                            ; we need to spin until it becomes unlocked.

    recover
    breakSF
    ret                     ; The lock has been acquired, return to the calling
                            ;  function.

spin_unlock:
    BuildSF
    preserve    

    mov     al, 0          ; Set the EAX register to 0.

    xchg    al, [rdi]   ; Atomically swap the EAX register with
                            ;  the lock variable.

    recover
    breakSF
    ret                     ; The lock has been released.
section .bss
regs resq 16
v resb 32

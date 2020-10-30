GLOBAL print

section .text

; void print(char *)
print:
    push rbp
    mov rbp, rsp

    mov rax, 1
    mov rsi, rdi
    int 80h
    
    mov rsp, rbp
    pop rbp
    ret
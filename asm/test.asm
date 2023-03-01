[bits 32]
start:
    mov eax,3
    mov eax,eax
    push ebx
    pushad
    mov eax,4
    popad
    ret
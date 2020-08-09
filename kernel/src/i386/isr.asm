%macro ISR_N 1
global isr_%1
isr_%1:
        pushad

        xor eax, eax
        mov ax, cs
        push eax
        mov ax, ss
        push eax
        mov ax, ds
        push eax
        mov ax, es
        push eax
        mov ax, fs
        push eax
        mov ax, gs
        push eax

        pushfd

        mov eax, cr0
        push eax
        mov eax, cr2
        push eax
        mov eax, cr3
        push eax
        mov eax, cr4
        push eax

        mov eax, dr0
        push eax
        mov eax, dr1
        push eax
        mov eax, dr2
        push eax
        mov eax, dr3
        push eax
        mov eax, dr6
        push eax
        mov eax, dr7
        push eax

        mov eax, esp
        cld
        extern isr_c_%1
        push eax
        call isr_c_%1
        popad
        iret
%endmacro

%macro ISR_E 1
global isr_%1
isr_%1:
        pushad

        xor eax, eax
        mov ax, cs
        push eax
        mov ax, ss
        push eax
        mov ax, ds
        push eax
        mov ax, es
        push eax
        mov ax, fs
        push eax
        mov ax, gs
        push eax

        pushfd

        mov eax, cr0
        push eax
        mov eax, cr2
        push eax
        mov eax, cr3
        push eax
        mov eax, cr4
        push eax

        mov eax, dr0
        push eax
        mov eax, dr1
        push eax
        mov eax, dr2
        push eax
        mov eax, dr3
        push eax
        mov eax, dr6
        push eax
        mov eax, dr7
        push eax

        mov ebx, esp
        cld
        extern isr_c_%1
        mov eax, [esp+8*4]
        push eax
        push ebx
        call isr_c_%1
        popad
        iret
%endmacro

ISR_N dz
ISR_N db
ISR_N nmi
ISR_N br
ISR_N of
ISR_N be
ISR_N io
ISR_N dn
ISR_E df
ISR_N cs
ISR_E ts
ISR_E sn
ISR_E ss
ISR_E gp
ISR_E pf
ISR_N 87
ISR_E ac
ISR_N mc
ISR_N si
ISR_N vi
ISR_E se

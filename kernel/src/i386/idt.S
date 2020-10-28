.section .bss
.align 4
.globl idt_base
idt_base:
.skip 0x2000
idt_end:

.section .data
.align 4
.globl idtr
 idtr:
    .short      idt_end-idt_base
    .long       idt_base

.section .text

.globl idt_install
idt_install:
        push    %ebp
        mov     %esp, %ebp

        mov     $idtr, %eax
        lidt    (%eax)

        mov     %ebp, %esp
        pop     %ebp
        ret

.globl idt_set_interrupt
idt_set_interrupt:
        push    %ebp
        mov     %esp, %ebp

        sub     $8, %esp
        mov     %edi, -4(%ebp)
        mov     %eax, -8(%ebp)

        mov     $idt_base, %edi
        shl     $3, %eax
        add     %eax, %edi
        mov     %cx, (%edi)
        rol     $16, %ecx
        mov     %cx, 6(%edi)
        mov     $0x08, %eax
        mov     %ax, 2(%edi)
        xor     %eax, %eax
        mov     %al, 4(%edi)
        mov     $0x8e, %eax
        mov     %al, 5(%edi)

        mov     -4(%ebp), %edi
        mov     -8(%ebp), %eax
        
        mov     %ebp, %esp
        pop     %ebp
        ret

.globl idt_set_interrupt_c
idt_set_interrupt_c:
        push    %ebp
        mov     %esp, %ebp

        mov     12(%ebp), %ecx
        mov     8(%ebp), %eax
        call    idt_set_interrupt

        mov     %ebp, %esp
        pop     %ebp
        ret

.globl idt_set_interrupt_user
idt_set_interrupt_user:
        push    %ebp
        mov     %esp, %ebp

        sub     $8, %esp
        mov     %edi, -4(%ebp)
        mov     %eax, -8(%ebp)

        mov     $idt_base, %edi
        shl     $3, %eax
        add     %eax, %edi
        mov     %cx, (%edi)
        rol     $16, %ecx
        mov     %cx, 6(%edi)
        mov     $0x08, %eax
        mov     %ax, 2(%edi)
        xor     %eax, %eax
        mov     %al, 4(%edi)
        mov     $0xee, %eax
        mov     %al, 5(%edi)

        mov     -4(%ebp), %edi
        mov     -8(%ebp), %eax

        mov     %ebp, %esp
        pop     %ebp
        ret

.globl idt_set_interrupt_user_c
idt_set_interrupt_user_c:
        push    %ebp
        mov     %esp, %ebp

        mov     12(%ebp), %ecx
        mov     8(%ebp), %eax
        call    idt_set_interrupt_user

        mov     %ebp, %esp
        pop     %ebp
        ret
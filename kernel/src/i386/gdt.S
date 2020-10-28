# GDT
# ---
.section .data
.align 4
.globl gdt_base
gdt_base:
    # NULL
    .long       0
    .long       0

    # KERNEL CODE
    .short      0xffff
    .short      0x0000
    .byte       0x0
    .byte       0x9a
    .byte       0xcf
    .byte       0x0

    # KERNEL DATA
    .short      0xffff
    .short      0x0000
    .byte       0x0
    .byte       0x92
    .byte       0xcf
    .byte       0x0

    # USER CODE
    .short      0xffff
    .short      0x0000
    .byte       0x0
    .byte       0xfa
    .byte       0xcf
    .byte       0x0

    # USER DATA
    .short      0xffff
    .short      0x0000
    .byte       0x0
    .byte       0xf2
    .byte       0xcf
    .byte       0x0

    # TSS
    .short      0x0067
    .short      0x0000
    .byte       0x0
    .byte       0x89
    .byte       0x40
    .byte       0x0
gdt_end:

.globl gdtr
gdtr:
    .short      gdt_end-gdt_base
    .long       gdt_base


.section .text
.globl gdt_install
gdt_install:
        push    %ebp
        mov     %esp, %ebp

        mov     $gdtr, %eax
        lgdt    (%eax)
        mov     $0x10, %ax
        mov     %ax, %ds
        mov     %ax, %es
        mov     %ax, %fs
        mov     %ax, %gs
        mov     %ax, %ss
        ljmp    $0x08, $gdt_install.loadcs
gdt_install.loadcs:

        mov     %ebp, %esp
        pop     %ebp
        ret

.globl gdt_settss
gdt_settss:
        push    %ebp
        mov     %esp, %ebp

        mov     $gdt_base, %edx
        movw     %ax, 42(%edx)
        shr     $16, %eax
        movb     %al, 44(%edx)
        shr     $8, %eax
        movb     %al, 47(%edx)
        mov     $0x2b, %ax
        ltr     %ax

        mov     %ebp, %esp
        pop     %ebp
        ret
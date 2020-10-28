.section .text

# Initialize serial port
# ----------------------
.globl serial_init
serial_init:
        push    %ebp
        mov     %esp, %ebp

        mov     $0x03f8+1, %dx
        mov     $0x0a, %al
        out     %al, %dx
        mov     $0x03f8+3, %dx
        mov     $0x80, %al
        out     %al, %dx
        mov     $0x03f8+0, %dx
        mov     $0x03, %al
        out     %al, %dx
        mov     $0x03f8+2, %dx
        xor     %al, %al
        out     %al, %dx
        mov     $0x03f8+3, %dx
        mov     $0x03, %al
        out     %al, %dx
        mov     $0x03f8+2, %dx
        mov     $0xc7, %al
        out     %al, %dx
        mov     $0x03f8+4, %dx
        mov     $0x0b, %al
        out     %al, %dx

        mov     %ebp, %esp
        pop     %ebp
        ret

# Out character to serial
#   al:         character to send
# -----------------------
.globl serial_out
serial_out:
        push    %ebp
        mov     %esp, %ebp
        sub     $4, %esp
        mov     %al, -4(%ebp)
serial_out.wait:
        mov     $0x03f8+5, %dx
        in      %dx, %al
        test    %al, %al
        jz      serial_out.wait
        mov     $0x03f8+0, %dx
        mov     -4(%ebp), %al
        out     %al, %dx
        mov     %ebp, %esp
        pop     %ebp
        ret

# Out a string to serial
#   eax:        address of NULL terminated string to send
# ----------------------
.globl serial_outs
serial_outs:
        push    %ebp
        mov     %esp, %ebp
        sub     $4, %esp
        mov     %esi, -4(%ebp)
        mov     %eax, %esi
serial_outs.lp:
        mov     (%esi), %al
        test    %al, %al
        jz      serial_outs.end
        call    serial_out
        inc     %esi
        jmp     serial_outs.lp
serial_outs.end:
        mov     -4(%ebp), %esi
        mov     %ebp, %esp
        pop     %ebp
        ret

# Out a hex number to serial
#   eax:        number to print
# --------------------------
.globl serial_outhex
serial_outhex:
        push    %ebp
        mov     %esp, %ebp
        mov     $8, %ecx
        rol     $4, %eax
serial_outhex.lp:
        push    %eax
        push    %ecx
        mov     serial_outhex.chars, %edx
        and     $0xf, %eax
        add     %eax, %edx
        mov     (%edx), %al
        call    serial_out
        pop     %ecx
        pop     %eax
        rol     $4, %eax
        dec     %ecx
        jnz     serial_outhex.lp
        mov     %ebp, %esp
        pop     %ebp
        ret
serial_outhex.chars:
.ascii "0123456789ABCDEF"

# Out a dec number to serial
#   eax:    number to print
# --------------------------
.globl serial_outdec
serial_outdec:
        push    %ebp
        mov     %esp, %ebp
        sub     $40, %esp
        mov     %esi, -4(%ebp)
        mov     %ebx, -8(%ebp)

        movl    $0, -12(%ebp)
        leal    -12(%ebp), %esi
        mov     $10, %ebx
serial_outdec.convert:
        xor     %edx, %edx
        div     %ebx
        add     $48, %edx
        cmp     $57, %edx
        jbe     serial_outdec.store
        add     $7, %edx
serial_outdec.store:
        dec     %esi
        mov     %dl, (%esi)
        and     %eax, %eax
        jnz     serial_outdec.convert
        mov     %esi, %eax
        call    serial_outs

        mov     -4(%ebp), %esi
        mov     -8(%ebp), %ebx
        mov     %ebp, %esp
        pop     %ebp
        ret
.section .text

# ISR_N
.globl isr_dz
isr_dz:
        pusha
        pushf

        xor     %eax, %eax
        mov     %cs, %ax
        push    %eax
        mov     %ss, %ax
        push    %eax
        mov     %ds, %ax
        push    %eax
        mov     %es, %ax
        push    %eax
        mov     %fs, %ax
        push    %eax
        mov     %gs, %ax
        push    %eax

        mov     60(%esp), %eax
        push    %eax

        mov     %cr0, %eax
        push    %eax
        mov     %cr2, %eax
        push    %eax
        mov     %cr3, %eax
        push    %eax
        mov     %cr4, %eax
        push    %eax

        mov     %dr0, %eax
        push    %eax
        mov     %dr1, %eax
        push    %eax
        mov     %dr2, %eax
        push    %eax
        mov     %dr3, %eax
        push    %eax
        mov     %dr6, %eax
        push    %eax
        mov     %dr7, %eax
        push    %eax

        mov     %esp, %eax
        cld
        push    %eax
        call    isr_c_dz
        add     $72, %esp
        popf
        popa
        iret

# ISR_N
.globl isr_db
isr_db:
        pusha
        pushf

        xor     %eax, %eax
        mov     %cs, %ax
        push    %eax
        mov     %ss, %ax
        push    %eax
        mov     %ds, %ax
        push    %eax
        mov     %es, %ax
        push    %eax
        mov     %fs, %ax
        push    %eax
        mov     %gs, %ax
        push    %eax

        mov     60(%esp), %eax
        push    %eax

        mov     %cr0, %eax
        push    %eax
        mov     %cr2, %eax
        push    %eax
        mov     %cr3, %eax
        push    %eax
        mov     %cr4, %eax
        push    %eax

        mov     %dr0, %eax
        push    %eax
        mov     %dr1, %eax
        push    %eax
        mov     %dr2, %eax
        push    %eax
        mov     %dr3, %eax
        push    %eax
        mov     %dr6, %eax
        push    %eax
        mov     %dr7, %eax
        push    %eax

        mov     %esp, %eax
        cld
        push    %eax
        call    isr_c_db
        add     $72, %esp
        popf
        popa
        iret

# ISR_N
.globl isr_nmi
isr_nmi:
        pusha
        pushf

        xor     %eax, %eax
        mov     %cs, %ax
        push    %eax
        mov     %ss, %ax
        push    %eax
        mov     %ds, %ax
        push    %eax
        mov     %es, %ax
        push    %eax
        mov     %fs, %ax
        push    %eax
        mov     %gs, %ax
        push    %eax

        mov     60(%esp), %eax
        push    %eax

        mov     %cr0, %eax
        push    %eax
        mov     %cr2, %eax
        push    %eax
        mov     %cr3, %eax
        push    %eax
        mov     %cr4, %eax
        push    %eax

        mov     %dr0, %eax
        push    %eax
        mov     %dr1, %eax
        push    %eax
        mov     %dr2, %eax
        push    %eax
        mov     %dr3, %eax
        push    %eax
        mov     %dr6, %eax
        push    %eax
        mov     %dr7, %eax
        push    %eax

        mov     %esp, %eax
        cld
        push    %eax
        call    isr_c_nmi
        add     $72, %esp
        popf
        popa
        iret

# ISR_N
.globl isr_br
isr_br:
        pusha
        pushf

        xor     %eax, %eax
        mov     %cs, %ax
        push    %eax
        mov     %ss, %ax
        push    %eax
        mov     %ds, %ax
        push    %eax
        mov     %es, %ax
        push    %eax
        mov     %fs, %ax
        push    %eax
        mov     %gs, %ax
        push    %eax

        mov     60(%esp), %eax
        push    %eax

        mov     %cr0, %eax
        push    %eax
        mov     %cr2, %eax
        push    %eax
        mov     %cr3, %eax
        push    %eax
        mov     %cr4, %eax
        push    %eax

        mov     %dr0, %eax
        push    %eax
        mov     %dr1, %eax
        push    %eax
        mov     %dr2, %eax
        push    %eax
        mov     %dr3, %eax
        push    %eax
        mov     %dr6, %eax
        push    %eax
        mov     %dr7, %eax
        push    %eax

        mov     %esp, %eax
        cld
        push    %eax
        call    isr_c_br
        add     $72, %esp
        popf
        popa
        iret

# ISR_N
.globl isr_of
isr_of:
        pusha
        pushf

        xor     %eax, %eax
        mov     %cs, %ax
        push    %eax
        mov     %ss, %ax
        push    %eax
        mov     %ds, %ax
        push    %eax
        mov     %es, %ax
        push    %eax
        mov     %fs, %ax
        push    %eax
        mov     %gs, %ax
        push    %eax

        mov     60(%esp), %eax
        push    %eax

        mov     %cr0, %eax
        push    %eax
        mov     %cr2, %eax
        push    %eax
        mov     %cr3, %eax
        push    %eax
        mov     %cr4, %eax
        push    %eax

        mov     %dr0, %eax
        push    %eax
        mov     %dr1, %eax
        push    %eax
        mov     %dr2, %eax
        push    %eax
        mov     %dr3, %eax
        push    %eax
        mov     %dr6, %eax
        push    %eax
        mov     %dr7, %eax
        push    %eax

        mov     %esp, %eax
        cld
        push    %eax
        call    isr_c_of
        add     $72, %esp
        popf
        popa
        iret

# ISR_N
.globl isr_be
isr_be:
        pusha
        pushf

        xor     %eax, %eax
        mov     %cs, %ax
        push    %eax
        mov     %ss, %ax
        push    %eax
        mov     %ds, %ax
        push    %eax
        mov     %es, %ax
        push    %eax
        mov     %fs, %ax
        push    %eax
        mov     %gs, %ax
        push    %eax

        mov     60(%esp), %eax
        push    %eax

        mov     %cr0, %eax
        push    %eax
        mov     %cr2, %eax
        push    %eax
        mov     %cr3, %eax
        push    %eax
        mov     %cr4, %eax
        push    %eax

        mov     %dr0, %eax
        push    %eax
        mov     %dr1, %eax
        push    %eax
        mov     %dr2, %eax
        push    %eax
        mov     %dr3, %eax
        push    %eax
        mov     %dr6, %eax
        push    %eax
        mov     %dr7, %eax
        push    %eax

        mov     %esp, %eax
        cld
        push    %eax
        call    isr_c_be
        add     $72, %esp
        popf
        popa
        iret

# ISR_N
.globl isr_io
isr_io:
        pusha
        pushf

        xor     %eax, %eax
        mov     %cs, %ax
        push    %eax
        mov     %ss, %ax
        push    %eax
        mov     %ds, %ax
        push    %eax
        mov     %es, %ax
        push    %eax
        mov     %fs, %ax
        push    %eax
        mov     %gs, %ax
        push    %eax

        mov     60(%esp), %eax
        push    %eax

        mov     %cr0, %eax
        push    %eax
        mov     %cr2, %eax
        push    %eax
        mov     %cr3, %eax
        push    %eax
        mov     %cr4, %eax
        push    %eax

        mov     %dr0, %eax
        push    %eax
        mov     %dr1, %eax
        push    %eax
        mov     %dr2, %eax
        push    %eax
        mov     %dr3, %eax
        push    %eax
        mov     %dr6, %eax
        push    %eax
        mov     %dr7, %eax
        push    %eax

        mov     %esp, %eax
        cld
        push    %eax
        call    isr_c_io
        add     $72, %esp
        popf
        popa
        iret

# ISR_N
.globl isr_dn
isr_dn:
        pusha
        pushf

        xor     %eax, %eax
        mov     %cs, %ax
        push    %eax
        mov     %ss, %ax
        push    %eax
        mov     %ds, %ax
        push    %eax
        mov     %es, %ax
        push    %eax
        mov     %fs, %ax
        push    %eax
        mov     %gs, %ax
        push    %eax

        mov     60(%esp), %eax
        push    %eax

        mov     %cr0, %eax
        push    %eax
        mov     %cr2, %eax
        push    %eax
        mov     %cr3, %eax
        push    %eax
        mov     %cr4, %eax
        push    %eax

        mov     %dr0, %eax
        push    %eax
        mov     %dr1, %eax
        push    %eax
        mov     %dr2, %eax
        push    %eax
        mov     %dr3, %eax
        push    %eax
        mov     %dr6, %eax
        push    %eax
        mov     %dr7, %eax
        push    %eax

        mov     %esp, %eax
        cld
        push    %eax
        call    isr_c_dn
        add     $72, %esp
        popf
        popa
        iret

# ISR_N
.globl isr_cs
isr_cs:
        pusha
        pushf

        xor     %eax, %eax
        mov     %cs, %ax
        push    %eax
        mov     %ss, %ax
        push    %eax
        mov     %ds, %ax
        push    %eax
        mov     %es, %ax
        push    %eax
        mov     %fs, %ax
        push    %eax
        mov     %gs, %ax
        push    %eax

        mov     60(%esp), %eax
        push    %eax

        mov     %cr0, %eax
        push    %eax
        mov     %cr2, %eax
        push    %eax
        mov     %cr3, %eax
        push    %eax
        mov     %cr4, %eax
        push    %eax

        mov     %dr0, %eax
        push    %eax
        mov     %dr1, %eax
        push    %eax
        mov     %dr2, %eax
        push    %eax
        mov     %dr3, %eax
        push    %eax
        mov     %dr6, %eax
        push    %eax
        mov     %dr7, %eax
        push    %eax

        mov     %esp, %eax
        cld
        push    %eax
        call    isr_c_cs
        add     $72, %esp
        popf
        popa
        iret

# ISR_N
.globl isr_87
isr_87:
        pusha
        pushf

        xor     %eax, %eax
        mov     %cs, %ax
        push    %eax
        mov     %ss, %ax
        push    %eax
        mov     %ds, %ax
        push    %eax
        mov     %es, %ax
        push    %eax
        mov     %fs, %ax
        push    %eax
        mov     %gs, %ax
        push    %eax

        mov     60(%esp), %eax
        push    %eax

        mov     %cr0, %eax
        push    %eax
        mov     %cr2, %eax
        push    %eax
        mov     %cr3, %eax
        push    %eax
        mov     %cr4, %eax
        push    %eax

        mov     %dr0, %eax
        push    %eax
        mov     %dr1, %eax
        push    %eax
        mov     %dr2, %eax
        push    %eax
        mov     %dr3, %eax
        push    %eax
        mov     %dr6, %eax
        push    %eax
        mov     %dr7, %eax
        push    %eax

        mov     %esp, %eax
        cld
        push    %eax
        call    isr_c_87
        add     $72, %esp
        popf
        popa
        iret

# ISR_N
.globl isr_mc
isr_mc:
        pusha
        pushf

        xor     %eax, %eax
        mov     %cs, %ax
        push    %eax
        mov     %ss, %ax
        push    %eax
        mov     %ds, %ax
        push    %eax
        mov     %es, %ax
        push    %eax
        mov     %fs, %ax
        push    %eax
        mov     %gs, %ax
        push    %eax

        mov     60(%esp), %eax
        push    %eax

        mov     %cr0, %eax
        push    %eax
        mov     %cr2, %eax
        push    %eax
        mov     %cr3, %eax
        push    %eax
        mov     %cr4, %eax
        push    %eax

        mov     %dr0, %eax
        push    %eax
        mov     %dr1, %eax
        push    %eax
        mov     %dr2, %eax
        push    %eax
        mov     %dr3, %eax
        push    %eax
        mov     %dr6, %eax
        push    %eax
        mov     %dr7, %eax
        push    %eax

        mov     %esp, %eax
        cld
        push    %eax
        call    isr_c_mc
        add     $72, %esp
        popf
        popa
        iret

# ISR_N
.globl isr_si
isr_si:
        pusha
        pushf

        xor     %eax, %eax
        mov     %cs, %ax
        push    %eax
        mov     %ss, %ax
        push    %eax
        mov     %ds, %ax
        push    %eax
        mov     %es, %ax
        push    %eax
        mov     %fs, %ax
        push    %eax
        mov     %gs, %ax
        push    %eax

        mov     60(%esp), %eax
        push    %eax

        mov     %cr0, %eax
        push    %eax
        mov     %cr2, %eax
        push    %eax
        mov     %cr3, %eax
        push    %eax
        mov     %cr4, %eax
        push    %eax

        mov     %dr0, %eax
        push    %eax
        mov     %dr1, %eax
        push    %eax
        mov     %dr2, %eax
        push    %eax
        mov     %dr3, %eax
        push    %eax
        mov     %dr6, %eax
        push    %eax
        mov     %dr7, %eax
        push    %eax

        mov     %esp, %eax
        cld
        push    %eax
        call    isr_c_si
        add     $72, %esp
        popf
        popa
        iret

# ISR_N
.globl isr_vi
isr_vi:
        pusha
        pushf

        xor     %eax, %eax
        mov     %cs, %ax
        push    %eax
        mov     %ss, %ax
        push    %eax
        mov     %ds, %ax
        push    %eax
        mov     %es, %ax
        push    %eax
        mov     %fs, %ax
        push    %eax
        mov     %gs, %ax
        push    %eax

        mov     60(%esp), %eax
        push    %eax

        mov     %cr0, %eax
        push    %eax
        mov     %cr2, %eax
        push    %eax
        mov     %cr3, %eax
        push    %eax
        mov     %cr4, %eax
        push    %eax

        mov     %dr0, %eax
        push    %eax
        mov     %dr1, %eax
        push    %eax
        mov     %dr2, %eax
        push    %eax
        mov     %dr3, %eax
        push    %eax
        mov     %dr6, %eax
        push    %eax
        mov     %dr7, %eax
        push    %eax

        mov     %esp, %eax
        cld
        push    %eax
        call    isr_c_vi
        add     $72, %esp
        popf
        popa
        iret

# ISR_E
.globl isr_pf
isr_pf:
        pusha
        pushf

        xor     %eax, %eax
        mov     %cs, %ax
        push    %eax
        mov     %ss, %ax
        push    %eax
        mov     %ds, %ax
        push    %eax
        mov     %es, %ax
        push    %eax
        mov     %fs, %ax
        push    %eax
        mov     %gs, %ax
        push    %eax

        mov     64(%esp), %eax
        push    %eax

        mov     %cr0, %eax
        push    %eax
        mov     %cr2, %eax
        push    %eax
        mov     %cr3, %eax
        push    %eax
        mov     %cr4, %eax
        push    %eax

        mov     %dr0, %eax
        push    %eax
        mov     %dr1, %eax
        push    %eax
        mov     %dr2, %eax
        push    %eax
        mov     %dr3, %eax
        push    %eax
        mov     %dr6, %eax
        push    %eax
        mov     %dr7, %eax
        push    %eax

        mov     %esp, %ebx
        cld
        mov     104(%esp), %eax
        push    %eax
        push    %ebx
        call    isr_c_pf
        add     $76, %esp
        popf
        popa
        add     $4, %esp
        iret

# ISR_E
.globl isr_df
isr_df:
        pusha
        pushf

        xor     %eax, %eax
        mov     %cs, %ax
        push    %eax
        mov     %ss, %ax
        push    %eax
        mov     %ds, %ax
        push    %eax
        mov     %es, %ax
        push    %eax
        mov     %fs, %ax
        push    %eax
        mov     %gs, %ax
        push    %eax

        mov     64(%esp), %eax
        push    %eax

        mov     %cr0, %eax
        push    %eax
        mov     %cr2, %eax
        push    %eax
        mov     %cr3, %eax
        push    %eax
        mov     %cr4, %eax
        push    %eax

        mov     %dr0, %eax
        push    %eax
        mov     %dr1, %eax
        push    %eax
        mov     %dr2, %eax
        push    %eax
        mov     %dr3, %eax
        push    %eax
        mov     %dr6, %eax
        push    %eax
        mov     %dr7, %eax
        push    %eax

        mov     %esp, %ebx
        cld
        mov     104(%esp), %eax
        push    %eax
        push    %ebx
        call    isr_c_df
        add     $76, %esp
        popf
        popa
        add     $4, %esp
        iret

# ISR_E
.globl isr_ts
isr_ts:
        pusha
        pushf

        xor     %eax, %eax
        mov     %cs, %ax
        push    %eax
        mov     %ss, %ax
        push    %eax
        mov     %ds, %ax
        push    %eax
        mov     %es, %ax
        push    %eax
        mov     %fs, %ax
        push    %eax
        mov     %gs, %ax
        push    %eax

        mov     64(%esp), %eax
        push    %eax

        mov     %cr0, %eax
        push    %eax
        mov     %cr2, %eax
        push    %eax
        mov     %cr3, %eax
        push    %eax
        mov     %cr4, %eax
        push    %eax

        mov     %dr0, %eax
        push    %eax
        mov     %dr1, %eax
        push    %eax
        mov     %dr2, %eax
        push    %eax
        mov     %dr3, %eax
        push    %eax
        mov     %dr6, %eax
        push    %eax
        mov     %dr7, %eax
        push    %eax

        mov     %esp, %ebx
        cld
        mov     104(%esp), %eax
        push    %eax
        push    %ebx
        call    isr_c_ts
        add     $76, %esp
        popf
        popa
        add     $4, %esp
        iret

# ISR_E
.globl isr_sn
isr_sn:
        pusha
        pushf

        xor     %eax, %eax
        mov     %cs, %ax
        push    %eax
        mov     %ss, %ax
        push    %eax
        mov     %ds, %ax
        push    %eax
        mov     %es, %ax
        push    %eax
        mov     %fs, %ax
        push    %eax
        mov     %gs, %ax
        push    %eax

        mov     64(%esp), %eax
        push    %eax

        mov     %cr0, %eax
        push    %eax
        mov     %cr2, %eax
        push    %eax
        mov     %cr3, %eax
        push    %eax
        mov     %cr4, %eax
        push    %eax

        mov     %dr0, %eax
        push    %eax
        mov     %dr1, %eax
        push    %eax
        mov     %dr2, %eax
        push    %eax
        mov     %dr3, %eax
        push    %eax
        mov     %dr6, %eax
        push    %eax
        mov     %dr7, %eax
        push    %eax

        mov     %esp, %ebx
        cld
        mov     104(%esp), %eax
        push    %eax
        push    %ebx
        call    isr_c_sn
        add     $76, %esp
        popf
        popa
        add     $4, %esp
        iret

# ISR_E
.globl isr_ss
isr_ss:
        pusha
        pushf

        xor     %eax, %eax
        mov     %cs, %ax
        push    %eax
        mov     %ss, %ax
        push    %eax
        mov     %ds, %ax
        push    %eax
        mov     %es, %ax
        push    %eax
        mov     %fs, %ax
        push    %eax
        mov     %gs, %ax
        push    %eax

        mov     64(%esp), %eax
        push    %eax

        mov     %cr0, %eax
        push    %eax
        mov     %cr2, %eax
        push    %eax
        mov     %cr3, %eax
        push    %eax
        mov     %cr4, %eax
        push    %eax

        mov     %dr0, %eax
        push    %eax
        mov     %dr1, %eax
        push    %eax
        mov     %dr2, %eax
        push    %eax
        mov     %dr3, %eax
        push    %eax
        mov     %dr6, %eax
        push    %eax
        mov     %dr7, %eax
        push    %eax

        mov     %esp, %ebx
        cld
        mov     104(%esp), %eax
        push    %eax
        push    %ebx
        call    isr_c_ss
        add     $76, %esp
        popf
        popa
        add     $4, %esp
        iret

# ISR_E
.globl isr_gp
isr_gp:
        pusha
        pushf

        xor     %eax, %eax
        mov     %cs, %ax
        push    %eax
        mov     %ss, %ax
        push    %eax
        mov     %ds, %ax
        push    %eax
        mov     %es, %ax
        push    %eax
        mov     %fs, %ax
        push    %eax
        mov     %gs, %ax
        push    %eax

        mov     64(%esp), %eax
        push    %eax

        mov     %cr0, %eax
        push    %eax
        mov     %cr2, %eax
        push    %eax
        mov     %cr3, %eax
        push    %eax
        mov     %cr4, %eax
        push    %eax

        mov     %dr0, %eax
        push    %eax
        mov     %dr1, %eax
        push    %eax
        mov     %dr2, %eax
        push    %eax
        mov     %dr3, %eax
        push    %eax
        mov     %dr6, %eax
        push    %eax
        mov     %dr7, %eax
        push    %eax

        mov     %esp, %ebx
        cld
        mov     104(%esp), %eax
        push    %eax
        push    %ebx
        call    isr_c_gp
        add     $76, %esp
        popf
        popa
        add     $4, %esp
        iret

# ISR_E
.globl isr_ac
isr_ac:
        pusha
        pushf

        xor     %eax, %eax
        mov     %cs, %ax
        push    %eax
        mov     %ss, %ax
        push    %eax
        mov     %ds, %ax
        push    %eax
        mov     %es, %ax
        push    %eax
        mov     %fs, %ax
        push    %eax
        mov     %gs, %ax
        push    %eax

        mov     64(%esp), %eax
        push    %eax

        mov     %cr0, %eax
        push    %eax
        mov     %cr2, %eax
        push    %eax
        mov     %cr3, %eax
        push    %eax
        mov     %cr4, %eax
        push    %eax

        mov     %dr0, %eax
        push    %eax
        mov     %dr1, %eax
        push    %eax
        mov     %dr2, %eax
        push    %eax
        mov     %dr3, %eax
        push    %eax
        mov     %dr6, %eax
        push    %eax
        mov     %dr7, %eax
        push    %eax

        mov     %esp, %ebx
        cld
        mov     104(%esp), %eax
        push    %eax
        push    %ebx
        call    isr_c_ac
        add     $76, %esp
        popf
        popa
        add     $4, %esp
        iret

# ISR_E
.globl isr_se
isr_se:
        pusha
        pushf

        xor     %eax, %eax
        mov     %cs, %ax
        push    %eax
        mov     %ss, %ax
        push    %eax
        mov     %ds, %ax
        push    %eax
        mov     %es, %ax
        push    %eax
        mov     %fs, %ax
        push    %eax
        mov     %gs, %ax
        push    %eax

        mov     64(%esp), %eax
        push    %eax

        mov     %cr0, %eax
        push    %eax
        mov     %cr2, %eax
        push    %eax
        mov     %cr3, %eax
        push    %eax
        mov     %cr4, %eax
        push    %eax

        mov     %dr0, %eax
        push    %eax
        mov     %dr1, %eax
        push    %eax
        mov     %dr2, %eax
        push    %eax
        mov     %dr3, %eax
        push    %eax
        mov     %dr6, %eax
        push    %eax
        mov     %dr7, %eax
        push    %eax

        mov     %esp, %ebx
        cld
        mov     104(%esp), %eax
        push    %eax
        push    %ebx
        call    isr_c_se
        add     $76, %esp
        popf
        popa
        add     $4, %esp
        iret

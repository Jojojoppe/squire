# MBOOT
# -----
.set MBOOT_MAGIC,       0x1badb002
.set MBOOT_FLAGS,       0x7
.set MBOOT_CHECKSUM,    -(MBOOT_MAGIC+MBOOT_FLAGS)

.section .text
.align 4
.long MBOOT_MAGIC
.long MBOOT_FLAGS
.long MBOOT_CHECKSUM
.long 0
.long 0
.long 0
.long 0
.long 0
.long 0
.long 640
.long 480
.long 32
# -----

# DATA
# ----
.section .data
.align 0x1000

# Boot time page directory
boot_PD:
.long 0x00000083
.fill 767, 4, 0x00000000
.long 0x00000083
.fill 255, 4, 0x00000000

fpu_test:
.short 0x55aa

fpu_var0:
.quad 0x4029000000000000
fpu_var1:
.quad 0x4000000000000000
fpu_var2:
.quad 0x0
fpu_var3:
.quad 0x0

# BSS
# ---
.section .bss
.align 0x1000

mboot_header_address:
.skip 4

# Boot time kernel stack
boot_stack_btm:
.skip 0x4000
boot_stack_top:
.globl boot_stack_top_C
boot_stack_top_C:
.skip 4

# TSS space
.globl tss_btm
tss_btm:
.skip 104
tss_top:
.globl TSS
TSS:
.skip 4

# BOOT CODE
#   entry point
# ---------
.section .text
.align 4
.globl _start
_start: 
        cli
        cmp     $0x2badb002, %eax
        jne     hang

        mov     $boot_PD-0xc0000000, %eax
        mov     %eax, %cr3
        mov     %cr4, %eax
        or      $0x00000010, %eax
        mov     %eax, %cr4
        mov     %cr0, %eax
        or      $0x80000010, %eax
        mov     %eax, %cr0
        lea     (start_high), %eax
        jmp     *%eax
start_high:
        movl    $0, (boot_PD)
        mov     $boot_PD-0xc0000000, %eax
        and     $0xfffff000, %eax
        or      $0x00000003, %eax
        mov     %eax, (boot_PD+4092)
        invlpg  (0)

        mov     $boot_stack_top, %esp
        mov     %esp, %ebp
        sub     $4, %esp
        add     $0xc0000000, %ebx
        mov     %ebx, -4(%ebp)
        mov     %ebx, (mboot_header_address)

        call    gdt_install
        call    serial_init
        mov     -4(%ebp), %eax
        push    %eax
        call    pmm_init
        add     $4, %esp
        call    idt_install
        call    interrupts_init
        call    vas_init
        mov     $tss_btm, %eax
        mov     %eax, (TSS)
        call    gdt_settss
        mov     $boot_stack_top_C, %eax
        movl    $boot_stack_top, (%eax)
        sub     $64, %esp

		# Initialize FPU
        mov     %cr0, %eax
        and     $0xfffffff3, %eax
        mov     %eax, %cr0
        finit
        fstsw  (fpu_test)
        mov     (fpu_test), %ax
        cmp     $0, %ax
        jne     hang
		fstcw	(fpu_test)
		mov		(fpu_test), %ax
		cmp		$0x37f, %ax
		jne		hang

        mov     %cr0, %eax
		or		$(1<<1), %eax
        mov     %eax, %cr0

		# TEST FPU
		fldl	(fpu_var0)
		fldl	(fpu_var1)
		fmul
		fstl	(fpu_var2)
		movl	(fpu_var2+4), %eax
		cmp		$0x40390000, %eax
		jne		hang
		movl	(fpu_var2), %eax
		cmp		$0x00000000, %eax
		jne		hang

        mov     $0xc0000000, %eax
        push    %eax
        mov     (mboot_header_address), %eax
        push    %eax
        call    squire_init

hang:
        cli
        hlt
        jmp     hang

bits 64

; ------------
; SECTION TEXT
section .text
align 0x04
; ------------

global g_start:
g_start:
        cli
        hlt
        jmp     g_start
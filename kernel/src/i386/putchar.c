int putchar(char c){
    asm volatile(".intel_syntax noprefix");
    asm volatile("mov eax, [ebp+8]");
    asm volatile("call serial_out");
    asm volatile(".att_syntax noprefix");
    return c;
}
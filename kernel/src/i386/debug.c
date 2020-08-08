#include <i386/config.h>

void debug_print_s(char * s){
    asm volatile(".intel_syntax noprefix");
    asm volatile("mov eax, [ebp+8]");
    asm volatile("call serial_outs");
    asm volatile(".att_syntax noprefix");
}
void debug_print_x(unsigned int num){
    asm volatile(".intel_syntax noprefix");
    asm volatile("mov eax, [ebp+8]");
    asm volatile("call serial_outhex");
    asm volatile(".att_syntax noprefix");
}
void debug_print_d(unsigned int num){
    asm volatile(".intel_syntax noprefix");
    asm volatile("mov eax, [ebp+8]");
    asm volatile("call serial_outdec");
    asm volatile(".att_syntax noprefix");
}

void debug_print_sx(char * name, unsigned int num){
    debug_print_s(name), debug_print_x(num), debug_print_s("\r\n");
}
void debug_print_sd(char * name, unsigned int num){
    debug_print_s(name), debug_print_d(num), debug_print_s("\r\n");
}
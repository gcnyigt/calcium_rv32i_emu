#include"calcium.h"
void printf(char* str) {
    for (int i = 0; *str != '\0'; i++) {
        *((volatile char *)0x90000000) = str[i];
    }
}
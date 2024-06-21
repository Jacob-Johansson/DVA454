#include <stdint.h>
#include "inc/tm4c129encpdt.h"

#ifndef DRIVER_H_
#define DRIVER_H_
#endif

void reset();
void Init(uint32_t ui32Base);
void delay(int x);
char getChar();
void putChar(char user_input);
void getString(char *string);
void putString(char *string);



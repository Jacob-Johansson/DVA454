#include "Driver.h"

int isEndString(const char* str) {
    const char* endString = "end";
    int i;
    for (i = 0; i < 3; ++i) {
        if (str[i] != endString[i]) {
            return 0;
        }
    }
    return 1;
}

int main ()
{
    char Buffer[100];
    reset();
    Init(0);


    while(1)
    {
        putString("\r\n Write a word \r\n");
        getString(Buffer);
        putString("\r\n the word you wrote is : ");
        putString(Buffer);

        if (isEndString(Buffer)) {
            putString("\r\nExiting the loop.\r\n");
            break;
        }
    }
}


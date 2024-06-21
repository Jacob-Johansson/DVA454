#include "Driver.h"
#include "inc/tm4c129encpdt.h"

void reset (){
    SYSCTL_SRUART_R = 0;
    SYSCTL_SRGPIO_R = 0;

}

void Init(uint32_t ui32Base){


        SYSCTL_RCGCUART_R = SYSCTL_RCGCUART_R0;
        SYSCTL_RCGCGPIO_R = SYSCTL_RCGCGPIO_R0;
        GPIO_PORTA_AHB_AFSEL_R = 1 | 2;
        GPIO_PORTA_AHB_PCTL_R = 1|(1<<4);
        GPIO_PORTA_AHB_DEN_R = (1<<0)|(1<<1);
        UART0_CTL_R = 0;

        //For the baud rate, we must calculate the baud rate divisor:
        //BRD = FOSC/(16*BAUD) = 104.1666667, UARTFBRD[DIVFRAC] = (0.1666667 * 64 + 0.5) = 11
        UART0_IBRD_R = 104;
        UART0_FBRD_R = 11;

        UART0_LCRH_R = (0x3<<5)|(1<<4);
        UART0_CC_R = 0x0;


        UART0_CTL_R = ((1<<0)|(1<<8)|(1<<9));

}

void delay(int x) {
    long volatile wait = (x * 25);
    while (wait != 0) {
    wait--;
    }
}


char getChar(){
    char user_input;
    while((UART0_FR_R&(UART_FR_RXFE)) != 0);

    user_input= UART0_DR_R;
    return  user_input;
}


void putChar(char Buffer){
    while((UART0_FR_R&(UART_FR_TXFF)) != 0);

    UART0_DR_R = Buffer;
}


void getString(char *string){
    char *buffer = string;


    char Buffer = getChar();


    while(Buffer != '\r'){
        *string = Buffer;
        putChar(Buffer);


        string++;


        Buffer = getChar();
    }

    *string = '\0';
    string = buffer;
}



void putString(char *string){
    while (*string!='\0'){
        delay(80);

        putChar(*(string++));
    }
}





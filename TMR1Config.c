#include "config.h"
void TMR1Config(void)
{
    INTCONbits.GIE = HIGH;      // global interrupts enabled
    TMR1H = 0x00;
    TMR1L = 0x00;
    T1CONbits.TMR1CS1 = LOW;    // fosc/4 is the clock source = 4MHz
    T1CONbits.TMR1CS0 = HIGH;
    PIE1bits.TMR1IE = HIGH;     //enable TMR1 interrupt
    INTCONbits.PEIE = HIGH;     // TMR1 interrupt enable bit
    T1CONbits.T1CKPS1 = LOW;   // 8 divisions of the clock input (2.5us * 8 = 2us)
    T1CONbits.T1CKPS0 = LOW;   // takes 131 ms to overflow (0x0000 to 0xffff))
    T1CONbits.TMR1CS = LOW;     // internal clock fosc/8 = 2MHz/8 = 0.5us clock period
    
    T1CONbits.TMR1ON = LOW;     // TMR1 disabled
    //T1CONbits.TMR1ON = HIGH;     // TMR1 disabled
    PIR1bits.TMR1IF = LOW;      // clear TMR1 interrupt flag
}

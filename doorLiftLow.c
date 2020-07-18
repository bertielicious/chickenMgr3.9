#include "config.h"
#include "byteToBin.h"
#include "clearRow.h"
#include "printString.h"
#include <stdio.h>
uchar doorLiftLow(uchar statuz)
{
    //static int doorLH, doorHL, units = 0;
    static bool doorPosn;
    uchar setDoor[] = {"Set door lift/ lower"};
    uchar doorStartOpen[]={"     DOOR OPEN  "};
    uchar doorStartClosed[]={"    DOOR CLOSED"};
    uchar doorInPosition[]={"RELEASE + - BUTT"};
    uchar doorDirection[] = {"Is door 100% open or"};
    uchar doorDirection2[] = {"closed? enter UP if "};
    uchar doorDirection3[] = {"OPEN or DN if CLOSED"};        
    uchar press[]={" Press + - together"};
    uchar release[]={"release when door is "};
    uchar release1[]={"fully in position"};
    
    switch (statuz)
    {
            case 1:
                printString(setDoor, 0x80);       //print on LCD 'set door lift/ lower       
                printString(doorDirection, 0xc0); // 'is door 100% open or'               
                printString(doorDirection2,0x94); // 'closed? enter UP if'
                printString(doorDirection3,0xd4); // 'OPEN or DN if CLOSED'
                
                if(UP==LOW && DOWN ==HIGH)
                {
                    statuz = 2;         // transition A
                }
                else if(UP==HIGH && DOWN ==LOW)
                {
                    statuz = 3;         // transition B
                }
            break;
            
            case 2: 
                 clearRow(0x80, 0xd3);       // clear row 1
                 clearRow(0xc0, 0xd3);       // clear row 2
                 clearRow(0x94, 0xa7);       // clear row 3
                 clearRow(0xd4, 0xe7);       // clear row 4  
                 while(UP==LOW);
                 doorPosn = HIGH;       // chicken door is fully open
                 statuz = 4;
                break;
        case 3:
                 clearRow(0x80, 0xd3);       // clear row 1
                 clearRow(0xc0, 0xd3);       // clear row 2
                 clearRow(0x94, 0xa7);       // clear row 3
                 clearRow(0xd4, 0xe7);       // clear row 4  
                 while(DOWN==LOW);
                 doorPosn = LOW;       // chicken door is fully closed
                 statuz = 4;
            break;
        case 4:
            if(doorPosn==HIGH)
            {
                 printString(doorStartOpen, 0x80);
            }
            else if(doorPosn==LOW)
            {
                 printString(doorStartClosed, 0x80);
            }
            statuz = 5;
            break;
        case 5:
            printString(press, 0xc0);   // Press + - together
            printString(release, 0x94); // release when door is
            printString(release1, 0xd4);// fully in position
            
            while(UP==HIGH && DOWN==HIGH||UP==HIGH && DOWN==LOW||UP==LOW && DOWN==HIGH);
            statuz = 6;
            
            break;
        case 6:
            if(doorPosn==HIGH && UP==LOW && DOWN== LOW)//door start position is high
            {  
                while(UP==LOW && DOWN== LOW)
                {
                    T1CONbits.TMR1ON = HIGH;
                    RC2 = LOW;      //door goes from open to closed
                    RA4 = HIGH;
                    doorHL =  doorMove;
                     uchar units = doorHL%10;
                    uchar tens = (doorHL/10)%10;
                    uchar huns = (doorHL/100)%10;
                    uchar thous = doorHL/1000;
                    byteToBin(0, 0x9f);
                    byteToBin(1, units + 0x30);
                    byteToBin(0, 0x9e);
                    byteToBin(1, tens + 0x30);
                    byteToBin(0, 0x9d);
                    byteToBin(1, huns + 0x30);
                    byteToBin(0, 0x9c);
                    byteToBin(1, thous + 0x30);
                    
                }
                T1CONbits.TMR1ON = LOW;
                RA4 = LOW;
                doorPosition = LOW;
                clearRow(0xc0, 0xd3);       // clear row 2
                clearRow(0x94, 0xa7);       // clear row 3
                clearRow(0xd4, 0xe7);       // clear row 4 
                printString(doorStartClosed, 0xd4);
                doorHL =  doorMove;
                doorMove = 0;
                uchar units = doorHL%10;
                uchar tens = (doorHL/10)%10;
                uchar huns = (doorHL/100)%10;
                uchar thous = doorHL/1000;
                byteToBin(0, 0x9f);
                byteToBin(1, units + 0x30);
                byteToBin(0, 0x9e);
                byteToBin(1, tens + 0x30);
                byteToBin(0, 0x9d);
                byteToBin(1, huns + 0x30);
                byteToBin(0, 0x9c);
                byteToBin(1, thous + 0x30);
                
                printf("doorHL %d \n ", doorHL);
                TMR1H = 0x00;
                TMR1L = 0x00;
               // printString(doorInPosition, 0x80);
               // printString(doorHL, 0xc0);
               // clearRow(0x94, 0xa7);       // clear row 3
                //clearRow(0xd4, 0xe7);       // clear row 4  
                
            }
            
            
            else if(doorPosn==LOW && UP==LOW && DOWN==LOW) // door start position is low
            {
                 //TMR1ON = HIGH;
                 while(UP==LOW && DOWN== LOW)
                 {
                    T1CONbits.TMR1ON = HIGH;
                    RC2 = HIGH;      //door goes from closed to open
                    RA4 = LOW;
                    doorLH =  doorMove;
                     uchar units = doorLH%10;
                    uchar tens = (doorLH/10)%10;
                    uchar huns = (doorLH/100)%10;
                    uchar thous = doorLH/1000;
                    byteToBin(0, 0x9f);
                    byteToBin(1, units + 0x30);
                    byteToBin(0, 0x9e);
                    byteToBin(1, tens + 0x30);
                    byteToBin(0, 0x9d);
                    byteToBin(1, huns + 0x30);
                    byteToBin(0, 0x9c);
                    byteToBin(1, thous + 0x30);
                 }
                 T1CONbits.TMR1ON = LOW;
                 RC2 = LOW;
                 doorPosition = HIGH;
                 clearRow(0xc0, 0xd3);       // clear row 2
                 clearRow(0x94, 0xa7);       // clear row 3
                 clearRow(0xd4, 0xe7);       // clear row 4 
                 printString(doorStartOpen, 0xd4);
                 doorLH =  doorMove;
                 doorMove = 0;
                 uchar units = doorLH%10;
                uchar tens = (doorLH/10)%10;
                uchar huns = (doorLH/100)%10;
                uchar thous = doorLH/1000;
                byteToBin(0, 0x9f);
                byteToBin(1, units + 0x30);
                byteToBin(0, 0x9e);
                byteToBin(1, tens + 0x30);
                byteToBin(0, 0x9d);
                byteToBin(1, huns + 0x30);
                byteToBin(0, 0x9c);
                byteToBin(1, thous + 0x30);
                 printf("doorLH %d \n ", doorLH);
                 TMR1H = 0x00;
                 TMR1L = 0x00;
                // printString(doorInPosition, 0x80);
                // clearRow(0xc0, 0xd3);       // clear row 2
                // clearRow(0x94, 0xa7);       // clear row 3
               //  clearRow(0xd4, 0xe7);       // clear row 4  
            }
            
            statuz = 7;
            break;
        case 7:
          //  printf("case 7 doorPosn = %d\n", doorPosn);
         //   printf("case 7 doorHL %d \n ", doorHL);
         //   printf("case 7 doorLH %d \n ", doorLH);
            __delay_ms(5000);
            if((doorLH == 0 || doorHL == 0) && doorPosn == HIGH)
            {   
                doorPosn = LOW;
                statuz = 4;
            }
            else if((doorLH == 0 || doorHL == 0) && doorPosn == LOW)
            {
                doorPosn = HIGH;
                statuz = 4;
            }
            if(doorLH !=0 && doorHL !=0)
            {
                 clearRow(0x80, 0xd3);       // clear row 1
                 clearRow(0xc0, 0xd3);       // clear row 2
                 clearRow(0x94, 0xa7);       // clear row 3
                 clearRow(0xd4, 0xe7);       // clear row 4 
                 statuz = 8;
            }
            break;
            
        case 8:
            break;
    }
    printf("statuz %d\n", statuz);
    return statuz;
}

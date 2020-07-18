#include "config.h"
#include "printString.h"
#include "byteToBin.h"
#include "clearRow.h"
#include<stdio.h>
uchar setDoorTimes(uchar doorState)
{
    uchar sdt[]={"   Set Door Times     "};
    uchar sdct[]={"Set door CLOSE time"};
    uchar sdot[]={"Set door OPEN time "};
    uchar press[]={"UP/ DOWN"};
    uchar hour[]={"hours"};
    uchar minutes[]={"mins"};
    uchar doorOT[]={"door OPEN time"};
    uchar doorCT[]={"door SHUT time"};
    static uchar hourz, LsbH, MsbH, LsbM, MsbM = 0;
    static uchar minz = 0;
    switch(doorState)
    {
        case 1:
            printString(sdt, 0x80);
            printString(sdct, 0xc0);
            doorState = 2;
            break;
            
        case 2:
            printString(press, 0x94);
            printString(hour, 0x9f);
            printString(minutes, 0xe0);
            if(UP==HIGH && DOWN==HIGH)
            {
                doorState = 2;
            }
            else if(UP==LOW && hourz<23)
            {
                doorState = 3;
            }
            break;
            
        case 3:
            if(UP==LOW && hourz<23)
            {
                hourz++;
                __delay_ms(200);
            }
            else if(DOWN==LOW && hourz>0)
            {
                doorState = 4;
            }
            else if(ENT==LOW && UP==HIGH)
            {
                doorState = 5;
            }
            break;
        case 4:
            if(DOWN==LOW && hourz>0)
            {
                hourz--;
                __delay_ms(200);   
            }
            else if(UP==LOW && hourz<23)
            {
                doorState = 3;
            }
            else if(ENT==LOW && UP==HIGH)
            {
                doorState = 5;
            }
            break;
            
        case 5:
            doorCloseHours = hourz;
            while(ENT==LOW);
            doorState = 6;
            break;
            
        case 6:
            if(UP==HIGH && DOWN==HIGH)
            {
                doorState = 6;
            }
            else if(UP==LOW && minz<59)
            {
                doorState = 7;
            }
            break;
            
        case 7:
            if(UP==LOW  && minz<59)
            {
                minz++;
                __delay_ms(200);
            }
            else if(DOWN==LOW && minz>0)
            {
                doorState = 8;
            }
             else if(ENT==LOW && UP==HIGH && DOWN==HIGH)
            {
                doorState = 9;
            }
            break;
            
        case 8:
            
            if(DOWN==LOW && minz>0)
            {
                minz--;
                 __delay_ms(200);
            }
            else if(UP==LOW &&minz<59)
            {
                doorState = 7;
            }
            else if(ENT==LOW && UP==HIGH && DOWN==HIGH)
            {
                doorState = 9;
            }
            break;
        case 9:
            while(ENT==LOW);
            doorCloseMins = minz;
            hourz = 0;
            minz = 0;
            doorState = 10;
            __delay_ms(2000);
            
            break;
            
        case 10:
             printString(sdot, 0xc0);
             doorState = 11;
            if(UP==HIGH && DOWN==HIGH)
            {
                doorState = 10;
            }
            else if(UP==LOW && hourz<23)
            {
                doorState = 11;
            }
            break;
            
            case 11:
            if(UP==LOW && hourz<23)
            {
                hourz++;
                __delay_ms(200);
            }
            else if(DOWN==LOW && hourz>0)
            {
                doorState = 12;
            }
            else if(ENT==LOW && UP==HIGH)
            {
                doorState = 13;
            }
            break;
        case 12:
            if(DOWN==LOW && hourz>0)
            {
                hourz--;
                __delay_ms(200);   
            }
            else if(UP==LOW && hourz<23)
            {
                doorState = 11;
            }
            else if(ENT==LOW && UP==HIGH)
            {
                doorState = 13;
            }
            break;
            
        case 13:
            while(ENT==LOW);
            doorOpenHours = hourz;
            doorState = 14;
            break;
            
        case 14:
            if(UP==HIGH && DOWN==HIGH)
            {
                doorState = 14;
            }
            else if(UP==LOW && minz<59)
            {
                doorState = 15;
            }
            break;
            
        case 15:
            if(UP==LOW  && minz<59)
            {
                minz++;
                __delay_ms(200);
            }
            else if(DOWN==LOW && minz>0)
            {
                doorState = 16;
            }
             else if(ENT==LOW && UP==HIGH && DOWN==HIGH)
            {
                doorState = 17;
            }
            break;
            
        case 16:
            
            if(DOWN==LOW && minz>0)
            {
                minz--;
                 __delay_ms(200);
            }
            else if(UP==LOW &&minz<59)
            {
                doorState = 15;
            }
            else if(ENT==LOW && UP==HIGH && DOWN==HIGH)
            {
                doorState = 17;
            }
            break;
            
        case 17:
            while(ENT==LOW);
            doorOpenMins = minz;
            hourz = 0;
            minz = 0;
             clearRow(0x80, 0x93);       // clear row 1
            clearRow(0xc0, 0xd3);       // clear row 2
            clearRow(0x94, 0xa7);       // clear row 3
            clearRow(0xd4, 0xe7);       // clear row 4
            doorState = 18;
            break;
        case 18:
           
            printString(doorCT, 0x94);           
            printString(doorOT, 0xd4);  
            doorState = 19;
            break;
            
        case 19:
            break;    
    }
   if(doorState <18) 
   {
        LsbH = hourz%10;
        MsbH = hourz/10;
        byteToBin(0, 0xa6);
        byteToBin(1, MsbH + 0x30);
        byteToBin(1, LsbH + 0x30);
    
        LsbM = minz%10;          // display hour/ mins set value on line 3/4
        MsbM = minz/10;
        byteToBin(0, 0xe6);
        byteToBin(1, MsbM + 0x30);
        byteToBin(1, LsbM + 0x30); 
   }
   else if(doorState == 18)
   {
        LsbH = doorCloseHours%10;
        MsbH = doorCloseHours/10;
        byteToBin(0, 0xa3);
        byteToBin(1, MsbH + 0x30);
        byteToBin(1, LsbH + 0x30);
    
        LsbM = doorCloseMins%10;          // display hour/ mins set value on line 3/4
        MsbM = doorCloseMins/10;
        byteToBin(0, 0xa6);
        byteToBin(1, MsbM + 0x30);
        byteToBin(1, LsbM + 0x30); 
        
        LsbH = doorOpenHours%10;
        MsbH = doorOpenHours/10;
        byteToBin(0, 0xe3);
        byteToBin(1, MsbH + 0x30);
        byteToBin(1, LsbH + 0x30);
    
        LsbM = doorOpenMins%10;          // display hour/ mins set value on line 3/4
        MsbM = doorOpenMins/10;
        byteToBin(0, 0xe6);
        byteToBin(1, MsbM + 0x30);
        byteToBin(1, LsbM + 0x30); 
       
   }
   // printf("CHours=%d\t CMins=%d\t OHours=%d\t  OMins=%d\n",doorCloseHours, doorCloseMins, doorOpenHours, doorOpenMins);
    return doorState;
    
}
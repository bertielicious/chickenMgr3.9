
#include"config.h"
#include "decToHex.h"
#include "PCF8583Write.h"
#include "PCF8583Read.h"
#include "byteToBin.h"
#include "clearRow.h"
#include "printString.h"
#include <stdio.h>
uchar setTimeDate(uchar state)
{
    uchar timeDate[] = {"Set time?"};
    uchar respond[] = {"Press UP for yes, or"};
    uchar respond2[] = {"DOWN for no"};
    uchar hourz[] = {"hours"};
    uchar setDate[] = {"Set date?"};
    uchar minz[] = {"minutes"};
    uchar yrz[] = {"year  00  - 99"};
    uchar monthz[] = {"month 1 - 12"};
    uchar dayz[]  = {"day 1 - 31"};
    uchar *ptr;
    uchar LsbH, MsbH, LsbM, MsbM,LsbY, MsbY, LsbMo, MsbMo, LsbD, MsbD = 0;
    uchar temp,  modYr = 0;
   static uchar debug1, debug2, debug3 = 0;
    switch (state)
    {
            case 1:     // ask user if they want to set the time, yes or no
                
                
                printString(timeDate,0xc0);
                printString(respond,0x94);
                
               printString(respond2,0xd4);
                
                
                if(UP == 0)         // set time option selected
                {
                    clearRow(0xc0, 0xd3);       // clear row 2
                    clearRow(0x94, 0xa7);       // clear row 3
                    clearRow(0xd4, 0xe7);       // clear row 4
                    state = 3;      // jump to set time state
                }
                
                else if (DOWN == 0) // set date option selected
                {
                    clearRow(0xc0, 0xd3);       // clear row 2
                    clearRow(0x94, 0xa7);       // clear row 3
                    clearRow(0xd4, 0xe7);       // clear row 4  
                    state = 12;
                }
                else if(UP == 1 && DOWN == 1)    // no option selected, stay in state 1
                {
                    state = 1;
                }
                break;
                
        case 2:
            printString(setDate,0xc0);
                
            if(UP == 1 && DOWN == 1)
            {
                state =2;
            }
            else if(DOWN == 0)
            {
                state = 11;
            }
            break;
            
        case 3:
            printString(hourz,0x94);
            
        
            if(UP == 1)
            {
                state = 3;
            }
            else if(UP==0&& hours<23)
            {
                state = 4;
            }
            break;
            
        case 4:
           
            if(UP == 0 && hours<23)
            {
                hours++;
                __delay_ms(200);
               // state = 4;
            }
            else if (DOWN == 0 && hours > 0)
            {
                state = 5;
            }
            else if(UP == 1 && ENT == 0)
            {
                state = 6;
            }
            break;
            
        case 5:
            if(DOWN == 0 && hours > 0)
            {
                hours--;
                  __delay_ms(200);
            }
            else if(UP==0 && hours <23)
            {
                state = 4;
            }
            else if (DOWN == 1 && ENT == 0)
            {
                state = 6;
            }
            break;
        case 6:

            temp = decToHex(hours);
            PCF8583Write(0xa0, HRS, temp);

            state = 7;              // set mins
            
        break;
           
        case 7:
            printString(minz,0xd4);
            
            if (UP==1)
            {
                state = 7;
            }
            else if(UP == 0 && mins < 59)
            {
                state = 8;
            }
            
            break;
            
            
        case 8:
            if(UP == 0 && mins < 59)
            {
                mins++;
                __delay_ms(200);
            }
            else if( DOWN == 0 && mins > 0)
            {
                state = 9;
            }
            else if(UP == 1 && ENT == 0)
            {
                state = 10;
            }
      
            break;
            
        case 9:
        
            if(DOWN == 0 && mins > 0)
            {
                mins--;
                __delay_ms(200);
            }
            else if(UP==0 && mins <59)
            {
                state = 8;
                 
            }
            else if(DOWN == 1 && ENT == 0)
            {
                state = 10;
            }
              
            break;
            
        case 10:
            

            temp = decToHex(mins);
            PCF8583Write(0xa0, MINS, temp); // set mins

            state = 11;              
            break;
            
        case 11:
            clearRow(0x94, 0xa7);       // clear row 3
            clearRow(0xd4, 0xe7);       // clear row 4
            state = 12;
            break;
            
        case 12: 
            printString(setDate,0xc0);
            printString(respond,0x94);
            printString(respond2,0xd4);

            if(UP==0)               // Yes set date
            {
            clearRow(0xc0, 0xd3);       // clear row 2
            clearRow(0x94, 0xa7);       // clear row 3
            clearRow(0xd4, 0xe7);       // clear row 4    
            state = 13;
            }
            else if(DOWN == 0)      // No skip date setting
            {
                state = 25;         // recover the year value stored in NVR
            }
            else
            {
                state = 12;         // wait for a yes or a no response
            }
            break; 
            
        case 13: 
            printString(yrz,0xc0);
            
            if(UP == 0)
            {
                state = 14;
            }
            break;
            
        case 14:
            if(UP==0 && year < 99 )
            {
                year++;                 // increment years
                __delay_ms(200);
                state = 14;
            }
            else if (DOWN == 0 && year > 0)
            {
                state = 15;
            }
            else if(UP == 1 && ENT == 0)
            {
                state = 16;
            }
            break;
            
        case 15:
            if(DOWN == 0 && year >0)
            {
                year--;
                __delay_ms(200);
            }
            else if (UP == 0 && year <99)
            {
                state = 14;
            }
            else if(DOWN == 1 && ENT == 0)
            {
                state = 16;
            }
            break;
            
        case 16:
           // offsetYr = leapYr();
           // printf("offset %d\n", offsetYr);
            
            //PCF8583Write(0xa0, YRDATE, (offsetYr&0xc0)<<6); // set years
           // PCF8583Write(0xa0, YRDATE, (offsetYr&0xc0)<<6); // set years
            
            tempYr = (year%4)<<6;
            
            state = 17;                
        break;
        
        case 17:
            printString(monthz,0x94);
            
            if(UP == 0)
            {
                state = 18;
            }
            
            break;
            
        case 18:
            if(UP==0 && month < 12 )
            {
                month++;                 // increment months
                __delay_ms(200);
            }
            else if (DOWN == 0 && month > 0)
            {
                state = 19;
            }
            else if(UP == 1 && ENT == 0)
            {
                state = 20;
            }
            break;
            
        case 19:
                if(DOWN == 0 && month > 0)
                {
                    month--;
                     __delay_ms(200);
                }
                else if(UP==0 & month <12)
                {
                    state = 18;
                }
                if(DOWN == 1 && ENT == 0)
                {
                    state = 20;
                }
        break;
        
        case 20:
            temp = decToHex(month);
            PCF8583Write(0xa0, DAYMTHS, temp); // set mins

            state = 21;              
            break;
            
        case 21:
            printString(dayz,0xd4);
            
            if(UP == 0)
            {
                state = 22;
            }
            
            break;
            
        case 22:
            if(UP==0 && day < 31 )
            {
                day++;                 // increment months
                __delay_ms(200);
            }
            else if (DOWN == 0 && day > 0)
            {
                state = 23;
            }
            else if(UP == 1 && ENT == 0)
            {
                state = 24;
            }
            break;
        case 23:
            if(DOWN == 0 && day > 0)
                {
                    day--;
                     __delay_ms(200);
                }
                else if(UP==0 & day <31)
                {
                    state = 22;
                }
                if(DOWN == 1 && ENT == 0)
                {
                    state = 24;
                }
            break;
        case 24:
            temp = decToHex(day);
            PCF8583Write(0xa0, YRDATE, ((temp & 0x3f) | (tempYr))); // set date and year
            PCF8583Write(0xa0, YEARZ, year);      // store year in non volatile RAM
            clearRow(0xc0, 0xd3);       // clear row 2
            clearRow(0x94, 0xa7);       // clear row 3
            clearRow(0xd4, 0xe7);       // clear row 4
            state = 26;              
            break;
        case 25:
            year = PCF8583Read(0xa0, YEARZ);
            state = 26;
            clearRow(0xc0, 0xd3);       // clear row 2
            clearRow(0x94, 0xa7);       // clear row 3
            clearRow(0xd4, 0xe7);       // clear row 4
            break;
        case 26:
            break;
    }
    if(state > 1 && state <=10)
    {
    LsbH = hours%10;
    MsbH = hours/10;
    byteToBin(0, 0xa6);
    byteToBin(1, MsbH + 0x30);
    byteToBin(1, LsbH + 0x30);
    
    LsbM = mins%10;          // display hour/ mins set value on line 3/4
    MsbM = mins/10;
    byteToBin(0, 0xe6);
    byteToBin(1, MsbM + 0x30);
    byteToBin(1, LsbM + 0x30); 
    }   
    if(state >=13 && state< 24)
    {
        LsbY = year%10;
        MsbY = year/10;
        byteToBin(0, 0xd2);
        byteToBin(1, MsbY + 0x30);  // write Year data on second row of LCD
        byteToBin(1, LsbY + 0x30);
    }
    if(state >16 && state< 24)
    {
        LsbMo = month%10;
        MsbMo = month/10;
        byteToBin(0, 0xa6);
        byteToBin(1, MsbMo + 0x30);  // write month data on third row of LCD
        byteToBin(1, LsbMo + 0x30);
    }
    if(state >20 && state< 24)
    {
        LsbD = day%10;
        MsbD = day/10;
        byteToBin(0, 0xe6);
        byteToBin(1, MsbD + 0x30);  // write date data on third row of LCD
        byteToBin(1, LsbD + 0x30);
    }
    
    return state;         
   // printf("state %d\n", state);
}
           
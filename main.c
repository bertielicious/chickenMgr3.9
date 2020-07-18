/*
 * File:   main.c
 * Author: Phil Glazzard
 *
 * Created on 19 November 2019, 18:49
 */
/* Software Construction
 1. Problem definition
 * Control of a motorised door to the chicken coup where the door will be:
 a)  Open if the light level is daylight or the clock timer has been 
 * triggered to open the door, or if the manual open button has been pressed.
 b) Closed if the light level is night level or the clock timer has been 
 * triggered to close the door or the manual close button has been 
 * pressed
 2. Display on LCD the date, time, door open time, door close time, timer ON/OFF
 * light sensor ON/ OFF, manual OPEN/ CLOSE ON/ OFF and battery charge level on 
 * an LCD with three button keypad to control all of above. Confirm key presses
 *  via buzzer.
 3. Via blu-tooth connection, transmit all LCD display info to phone app, and 
 * also allow control of all door functions from the app.
 4. Microcontroller to be in sleep mode except when:
 * a) door needs to move from closed to open or open to closed due to timer or 
 * light sensor inputs or manual close/ open button.
 * b) someone presses a keypad button (time out back to sleep mode after 
 * 2 minutes without button activity)
 * (c) blue tooth transmit and receive
 2. Sub systems
 * a) LCD
 * b) uC sleep
 * c) blue tooth
 * d) door
 * e) timer/ clock
 * f) light sensor
 * g) keypad
 * h) solar psu
 * i) motor
 * 
 3. Structs
 * a) LCD
 *  date(day, month, year)
 *  time (hour, minutes, seconds)
 *  door (open time, close time)
 *  timer(on, off)
 *  set time(door open time, door close time)
 *  light sensor (on, off)
 *  light sensor (adjust up time, adjust down time)
 *  manual door button (open, close)
 *  battery charge level display (o% - 100%)
 *  keypad with three buttons (up, down, enter)
 *  confirm key press with buzzer (key pressed, key not pressed)
 * 
 * b) uC sleep
 * if (blue tooth active or button pressed or door needs to open or close (light sensor or timer))
 *    wake up
 *    run required code
 *    else
 *    sleep
 * 
 * c) blu-tooth
 *    transmit
 *    receive
 *    run necessary code
 * 
 * d) door
 *    open 
 *    close
 *    open or close door
 * 
 * e) clock/ timer
 *      check open
 *      check close
 *      open or close door
 * 
 * f) light sensor
 *      check light
 *      check dark
 *      open or close door
 * 
 * g) keypad
 *    check button press
 *       which button pressed
 *       setup screen or exit to main loop
 *       take action or open / close door
 *  
 * 
 * h) solar psu
 *      charge LiPo battery
 *      display battery charge level
 * 
 * i) motor
 *    motor moves clockwise = open door
 *    motor moves anti-clockwise = close door
 * 
 * 
 *                  16f1459
 *                  ---------
 *   +5 Volts    1 |Vdd      | 20 0 Volts
        LCD D6   2 |RA5   RA0| 19   - PUSH BUTTON
 *    motor ACW  3 |RA4   RA1| 18   + PUSH BUTTON
      IOCA3      4 |RA3      | 17  MOTOR DIRECTION
 *  ENT PBUTTON  5 |RC5   RC0| 16  BOTTOM LIMIT SWITCH
 *    RS         6 |RC4   RC1| 15  motor CW
 *    EN         7 |RC3   RC2| 14  TOP LIMIT SWITCH/ DEBUG LED
 *    LCD D4     8 |RC6   RB4| 13  SDA
 *    LCD D5     9 |RC7   RB5| 12  LCD D7
 *    TX        10 |RB7   RB6| 11  SCL
 *                  ---------
 *
 */

#include "config.h"
#include "configOsc.h"
#include "configPorts.h"
#include "configUsart.h"
#include "putch.h"
#include <stdio.h>
#include "configLCD.h"
#include "pulse.h"
#include "nibToBin.h"
#include "byteToBin.h"
#include "configI2c.h"
#include "i2cStart.h"
#include "i2cWrite.h"
#include "i2cRestart.h"
#include "PCF8583Read.h"
#include "PCF8583Write.h"
#include "setupTime.h"
#include "TMR1Config.h"


#include "clearRow.h"
#include "setupDate.h"
#include "decToHex.h"
#include "hexToDec.h"
#include "configIOC.h"
#include "splashScreen.h"
#include "setTimeDate.h"
#include "doorLiftLow.h"
#include "setDoorTimes.h"
#include "printString.h"
uchar DoorOpen[]={"Door Open @"};   // display Door Open @ on 3rd line of LCD
uchar DoorClose[]={"Door Closed @"};// display Door Open @ on 4th line of LCD

void main()
{
    configOsc();        // configure internal clock to run at 16MHz
    configPorts();      // configure PORTS A/ B/ C
    configUsart();      // allow serial comms to PC for debugging
    configLCD();        // 20 x 4 LCD set up for 4 bit operation
    configI2c();        // I2C setup with PCF8583 RTC chip
    configIOC();
    TMR1Config();
    PORTAbits.RA4 = LOW;
    PORTCbits.RC2 = LOW;
    printf("hello!\n"); // test serial port
    
  
   /****variables defined for displaying set door open/ close times on LCD*******/
     uchar colon = 0x3a;
     uchar dchLsb, dchMsb = 0;
     uchar dcmLsb,dcmMsb = 0;
     uchar dohLsb,dohMsb = 0;
     uchar domLsb, domMsb = 0;
     
 
    /*********FSM state variable definitions***********/
    uchar status = 1;
    uchar statuz = 1;
    uchar doorState = 1;
    uchar doorPosState = 1;
    
    
    
    splashScreen();     // display "Chicken Manager 3" on LCD
    __delay_ms(2000);
    clearRow(0xc0, 0xd3);
    PCF8583Write(0xa0, CTRLSTAT, 0x80);         //turn counting off until counter settings loaded
    
    
    while(statuz< 8)
    {
        statuz = doorLiftLow(statuz);
    }
    while(doorState<18)
    {
        doorState = setDoorTimes(doorState);
        printf("doorState %d\n", doorState);
    }
    status = 1;
    while(status <26)
    {
        status = setTimeDate(status);
        printf("state %d\n", status);
    }
    
    PCF8583Write(0xa0, CTRLSTAT, 0x00);         //turn counting ON*/
    INTCONbits.GIE = 1;                 // enable global interrupts
    /****************doorOpenHours on LCD********************************/
    dohMsb = doorOpenHours/10;          // split doorOpenHours into Msb and Lsb
    dohLsb = doorOpenHours%10;
    printString(DoorOpen,0x94);         // display Door Open on line 3 of LCD
    byteToBin(0,0xa3);                  // set position of hours to be displayed on LCD to 0xA2
    byteToBin(1, dohMsb +0x30);         // write doorOpenHours Msb to LCD
    byteToBin(1, dohLsb +0x30);         // write doorOpenHours Lsb to LCD         
    byteToBin(1, 0x3a);               // colon
    /****************doorOpenMins on LCD********************************/
    domMsb = doorOpenMins/10;          // split doorOpenMins into Msb and Lsb
    domLsb = doorOpenMins%10;
    byteToBin(1, domMsb +0x30);         // write doorOpenMins Msb to LCD
    byteToBin(1, domLsb +0x30);         // write doorOpenMins Lsb to LCD    
    /****************doorCloseHours on LCD********************************/
    dchMsb = doorCloseHours/10;          // split doorCloseHours into Msb and Lsb
    dchLsb = doorCloseHours%10;
    printString(DoorClose,0xd4);         // display Door Open on line 4 of LCD
    byteToBin(0,0xe3);                  // set position of hours to be displayed on LCD to 0xA2
    byteToBin(1, dchMsb +0x30);         // write doorOpenHours Msb to LCD
    byteToBin(1, dchLsb +0x30);         // write doorOpenHours Lsb to LCD         
    byteToBin(1, 0x3a);               // colon
    /****************doorCloseMins on LCD********************************/
    dcmMsb = doorCloseMins/10;          // split doorCloseMins into Msb and Lsb
    dcmLsb = doorCloseMins%10;
    byteToBin(1, dcmMsb +0x30);         // write doorCloseMins Msb to LCD
    byteToBin(1, dcmLsb +0x30);         // write doorCloseMins Lsb to LCD  
    
     while(1)
    {    
        minTime = (rHours*60) + rMins;    // max value at 23:59 of 1439 = integer data type
        dot = (doorOpenHours*60) + doorOpenMins;
        dct = (doorCloseHours*60) + doorCloseMins;
        
        minTime = (rHours*60) + rMins;
        printf("minTime %d\t dot %d\t dct %d\n", minTime, dot, dct);

        switch(doorPosState)
        {
            case 1:
            if((minTime > dct && minTime<1440) || (minTime>=0 && minTime > dot))
            {
                doorPosState = 1;
            }
            else if(minTime>= dot)
            {
                //printf("opening door once only\n");
                while(testDoorLH <=doorLH)      // door opening
            {
                TMR1ON=HIGH;            // start TMR1 counting from 0
                testDoorLH = doorMove;
               // printf("testDoorLH = %d\n",testDoorLH );
                RA4 = LOW;
                RC2 = HIGH;         // open the door   
            //print door open at doorOpenHours:doorOpenMins
            }
            TMR1ON=LOW;         // stop TMR1 from counting
            TMR1H = 0x00;       // clear TMR1 count
            TMR1L = 0x00;
            doorMove = 0;
            RC2 = LOW;          // door should now be open
            doorPosState = 2;
            }
            break;

            case 2:
                if(minTime < dct)
                {
                    doorPosState = 2;
                }
                else if(minTime >= dct)
                {
                    //printf("closing door once only\n");
                    while(testDoorHL <=doorHL)      // door closing
            {
                TMR1ON=HIGH;            // start TMR1 counting from 0
                testDoorHL = doorMove;
               // printf("testDoorHL = %d\n",testDoorHL );
                RC2 = LOW;
                RA4 = HIGH;          // close the door 
            //print door open at doorOpenHours:doorOpenMins
            }
            TMR1ON=LOW;         // stop TMR1 from counting
            TMR1H = 0x00;       // clear TMR1 count
            TMR1L = 0x00;
            doorMove = 0;
            RA4 = LOW;          // door should now be open*/
            doorPosState = 1;
                }
                break;
                }
            }
        
        
     /*   if(dot <= minTime && dct > minTime)
        {
            while(testDoorLH <=doorLH)      // door opening
            {
                TMR1ON=HIGH;            // start TMR1 counting from 0
                testDoorLH = doorMove;
               // printf("testDoorLH = %d\n",testDoorLH );
                RA4 = LOW;
                RC2 = HIGH;         // open the door   
            //print door open at doorOpenHours:doorOpenMins
            }
            TMR1ON=LOW;         // stop TMR1 from counting
            TMR1H = 0x00;       // clear TMR1 count
            TMR1L = 0x00;
            doorMove = 0;
            RC2 = LOW;          // door should now be open
        }
        else if(dct<=minTime)
        {
           
            while(testDoorHL <=doorHL)      // door opening
            {
                TMR1ON=HIGH;            // start TMR1 counting from 0
                testDoorHL = doorMove;
               // printf("testDoorHL = %d\n",testDoorHL );
                RC2 = LOW;
                RA4 = HIGH;          // close the door 
            //print door open at doorOpenHours:doorOpenMins
            }
            TMR1ON=LOW;         // stop TMR1 from counting
            TMR1H = 0x00;       // clear TMR1 count
            TMR1L = 0x00;
            doorMove = 0;
            RA4 = LOW;          // door should now be open*/
          
        
        
        /********************new door logic code 14/7/20**********************/
        /* if((minTime >=0 && minTime <=dot) || (minTime >dct && minTime <1440))
               {
                   while(testDoorLH <=doorLH)      // door opening
            {
                TMR1ON=HIGH;            // start TMR1 counting from 0
                testDoorLH = doorMove;
               // printf("testDoorLH = %d\n",testDoorLH );
                RA4 = LOW;
                RC2 = HIGH;         // open the door   
            //print door open at doorOpenHours:doorOpenMins
            }
            TMR1ON=LOW;         // stop TMR1 from counting
            TMR1H = 0x00;       // clear TMR1 count
            TMR1L = 0x00;
            doorMove = 0;
            RC2 = LOW;          // door should now be open
               }

               else if(minTime > dot) //&& minTime <dct) 
               {
                    while(testDoorHL <=doorHL)      // door closing
            {
                TMR1ON=HIGH;            // start TMR1 counting from 0
                testDoorHL = doorMove;
                printf("testDoorHL = %d\n",testDoorHL );
                RC2 = LOW;
                RA4 = HIGH;          // close the door 
            //print door open at doorOpenHours:doorOpenMins
            }
                TMR1ON=LOW;         // stop TMR1 from counting
                TMR1H = 0x00;       // clear TMR1 count
                TMR1L = 0x00;
                doorMove = 0;
                RA4 = LOW;          // door should now be open
               }*/

              
        
  //      printf("doorPosn = %d\n", doorPosn);
       
        //printf("doorMove %d\t testDoorLH %d\t doorLH %d \n", doorMove,testDoorLH,doorLH );
        
    
}

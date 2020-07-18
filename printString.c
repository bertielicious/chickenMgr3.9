#include "config.h"
#include "byteToBin.h"
void printString(uchar arr[], uchar linePosn)
{
    uchar x = 0;
    byteToBin(LOW, linePosn);       // send command to place cursor at start of lines 1 to 4
    while(arr[x]>0)
    {
        byteToBin(HIGH, arr[x]);      // write the string
        x++;        
    }
}

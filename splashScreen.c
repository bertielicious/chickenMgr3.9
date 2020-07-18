#include "config.h"
#include "byteToBin.h"
#include "printString.h"
void splashScreen(void)
{
    uchar splash[] = {"Chicken Manager 3"};
    printString(splash, 0xc2);
}

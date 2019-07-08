#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "LED.h"

/*Turn LED on*/
void led_on (void)
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2); //led on PF2
}
/*Turn LED off*/
void led_off (void)
{
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0); //led off PF2
}

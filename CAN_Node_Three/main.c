#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "driverlib/can.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/task.h"
#include "FreeRTOS/semphr.h"
#include "MCAL/MCAL.h"
#include "HAL/LED.h"
#include "HAL/CAN_ECUAL.h"

void Init_CAN(void);
void init_Task(void * pvParameters);
void CAN_Receive(void *pvParameters);
void CAN_Send(void *pvParameters);

//TaskHandle_t pxCAN_Receive;
//TaskHandle_t pxCAN_Send;

SemaphoreHandle_t xSemaphore_ready_to_send;
SemaphoreHandle_t xSemaphore_Receive;

void main(void)
{
    xSemaphore_ready_to_send = xSemaphoreCreateBinary();
    xSemaphore_Receive = xSemaphoreCreateBinary();

    xTaskCreate(init_Task, "initialization_Task", 100, NULL, 3, NULL);
    xTaskCreate(CAN_Receive, "Receive", 100, NULL, 1, NULL);
    xTaskCreate(CAN_Send, "Send", 100, NULL, 1, NULL);
    vTaskStartScheduler();
    while (1)
    {
    }
}

void Init_CAN(void)
{

    // Set up CAN0
    SysCtlClockSet(
    SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinConfigure(GPIO_PB4_CAN0RX);
    GPIOPinConfigure(GPIO_PB5_CAN0TX);
    GPIOPinTypeCAN(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_CAN0))
    {
    }
    CANInit(CAN0_BASE);
    CANBitRateSet(CAN0_BASE, SysCtlClockGet(), 500000);
    CANEnable(CAN0_BASE);

}

void init_Task(void * pvParameters)
{
    Init_CAN();
    init();
    //   xTaskNotifyGive(pxCAN_Receive);
    xSemaphoreGive(xSemaphore_Receive);

    vTaskSuspend(NULL);
}

void CAN_Receive(void *pvParameters)
{
    tCANMsgObject sMsgObjectRx;
    uint8_t pui8BufferIn;
    sMsgObjectRx.ui32MsgID = 4;
    sMsgObjectRx.ui32MsgIDMask = 0x1FFFF;
    sMsgObjectRx.ui32Flags = MSG_OBJ_USE_ID_FILTER;
    sMsgObjectRx.pui8MsgData = &pui8BufferIn;
    sMsgObjectRx.ui32MsgLen = 1;
    unsigned long status;
    for (;;)
    {
        //ulTaskNotifyTake(pdTRUE , portMAX_DELAY);
        xSemaphoreTake(xSemaphore_Receive, portMAX_DELAY);
        CANMessageSet(CAN0_BASE, 1, &sMsgObjectRx, MSG_OBJ_TYPE_RX);
        status = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);
        while (!(status & CAN_STATUS_RXOK))
        {
            status = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);

        }
        status = 0;

        CANMessageGet(CAN0_BASE, 1, &sMsgObjectRx, true);
        led_on();/*LED ON*/
        vTaskDelay(1000);
        led_off();/*LED OFF*/

        xSemaphoreGive(xSemaphore_ready_to_send);
        //xTaskNotifyGive(pxCAN_Send);
    }
}

void CAN_Send(void *pvParameters)
{
    tCANMsgObject sMsgObjectTx;
    uint8_t pui8BufferOut = 1;
    sMsgObjectTx.ui32MsgID = 1;
    sMsgObjectTx.ui32Flags = 0;
    sMsgObjectTx.ui32MsgLen = 1;
    sMsgObjectTx.pui8MsgData = &pui8BufferOut;
    unsigned long status;
    for (;;)
    {
        //ulTaskNotifyTake(pdTRUE,portMAX_DELAY);
        xSemaphoreTake(xSemaphore_ready_to_send, portMAX_DELAY);
        CANMessageSet(CAN0_BASE, 1, &sMsgObjectTx, MSG_OBJ_TYPE_TX);
        status = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);
        while (!(status & CAN_STATUS_TXOK))
            status = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);
        status = 0;
        xSemaphoreGive(xSemaphore_Receive);
        //xTaskNotifyGive(pxCAN_Receive);

    }
}

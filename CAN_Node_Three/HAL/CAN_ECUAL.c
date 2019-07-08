/*
 * CAN_ECUAL.c
 *
 *  Created on: Jul 4, 2019
 *      Author: AVE-LAP-049
 */
#include "CAN_ECUAL.h"

volatile bool rxFlag = 0; // msg recieved fl
volatile bool errFlag = 0; // error flag

void CAN0Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinConfigure(GPIO_PB4_CAN0RX);
    GPIOPinConfigure(GPIO_PB5_CAN0TX);
    GPIOPinTypeCAN(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);
    CANInit(CAN0_BASE);
    CANBitRateSet(CAN0_BASE, SysCtlClockGet(), 500000);
    CANIntRegister(CAN0_BASE, CANIntHandler); // use dynamic vector table allocation
    CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);
    IntEnable(INT_CAN0);
    CANEnable(CAN0_BASE);
}

void CANIntHandler(void)
{

    unsigned long status = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE); // read interrupt status

    if (status == CAN_INT_INTID_STATUS)
    { // controller status interrupt
        status = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);
        errFlag = 1;
    }
    else if (status == 1)
    { // msg object 1
        CANIntClear(CAN0_BASE, 1); // clear interrupt
        rxFlag = 1; // set rx flag
        errFlag = 0; // clear any error flags
    }
    else
    { // should never happen
        //UARTprintf("Unexpected CAN bus interrupt\n");
    }
}

void CANMessageInit(tCANMsgObject* msg, uint32_t ID,uint32_t MASK, uint8_t Size,uint32_t flag)
{
    msg->ui32MsgID = ID;
    msg->ui32MsgIDMask = MASK;
    msg->ui32Flags = flag;
    msg->ui32MsgLen = Size;
}
void CANMessageSendBuffer(tCANMsgObject msg, uint8_t *BufferInt,uint8_t mail_box)
{
    msg.pui8MsgData = BufferInt;
    CANMessageSet(CAN0_BASE, mail_box, &msg, MSG_OBJ_TYPE_TX); // send as msg object 1
}


void CANMessegeReceive(tCANMsgObject msg, uint8_t *BufferInt,uint8_t mail_box)
{
    uint8_t msgData[8];
    msg.pui8MsgData = msgData;
    if(rxFlag)
    {
        CANMessageGet(CAN0_BASE,mail_box,&msg,0);
    }
}

/*
 * CAN_ECUAL.h
 *
 *  Created on: Jul 4, 2019
 *      Author: AVE-LAP-049
 */

#ifndef CAN_ECUAL_H_
#define CAN_ECUAL_H_

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



//volatile bool rxFlag = 0; // msg recieved flag
//volatile bool errFlag = 0; // error flag

extern void CAN0Init(void);
extern void CANIntHandler(void);
extern void CANMessageInit(tCANMsgObject* msg, uint32_t ID,uint32_t MASK, uint8_t Size,uint32_t flag);
extern void CANMessageSendBuffer(tCANMsgObject msg, uint8_t *BufferInt,uint8_t mail_box);
extern void CANMessegeReceive(tCANMsgObject msg, uint8_t *BufferInt,uint8_t mail_box);

#endif /* CAN_ECUAL_H_ */

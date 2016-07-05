#ifndef _USR_MAIN_H_
#define _USR_MAIN_H_

#include "common.h"
#include "global.h"

void PIT0_ISR(void);
void PIT1_ISR(void);
void BT_RX_ISR(uint16_t);
void US100_RX_ISR(uint16_t);

#endif // _USER_MAIN_H_

#include "common.h"
#include "global.h"

void PIT0_ISR(void);
void PIT1_ISR(void);
void UART_RX_ISR(uint16_t);

void ov7725_ISR(uint32_t);

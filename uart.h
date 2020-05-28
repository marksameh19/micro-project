#include "stdint.h"
#include "tm4c123gh6pm.h"

void initialize_uart()
{
	SYSCTL_RCGCUART_R |= 0x0002;
	SYSCTL_RCGCGPIO_R |= 0x0002;
	UART1_CTL_R &= ~0x0001;	
	UART1_IBRD_R = 27;						
	UART1_FBRD_R = 8;
	UART1_LCRH_R = 0x0070;
	UART1_CTL_R = 0x0201;
	GPIO_PORTB_AFSEL_R |= 0x03;
	GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFFFFFF00)|(0x00000011);
	GPIO_PORTB_DEN_R |= 0x03;			
	GPIO_PORTB_AMSEL_R &= ~0x03;
}
int uart_get()
{
	while((UART1_FR_R & 0x10) != 0){};
	return (int)UART1_DR_R;
}
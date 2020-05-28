#include "stdint.h"
#include "tm4c123gh6pm.h"
#include "keypad.h"
#include "uart.h"
#define red 0x02
#define blue 0x04
void SystemInit(){}
	
struct room
{
	char password[5];
	int number;
	int state;//0 for empty, 1 for reserved, 2 for cleaning
	int open;
};


void Portf_init()
{
	uint32_t delay;
	SYSCTL_RCGCGPIO_R |= 0x20;
	delay = 1;
	GPIO_PORTF_LOCK_R = 0x4C4F434B;
	GPIO_PORTF_CR_R = 0x1F;
	GPIO_PORTF_AMSEL_R = 0;
	GPIO_PORTF_AFSEL_R = 0;
	GPIO_PORTF_PCTL_R = 0;
	GPIO_PORTF_DIR_R = 0x0E;
	GPIO_PORTF_DEN_R = 0x1F;
	GPIO_PORTF_PUR_R = 0x11;
	
}


char pass[5];
int count = 0;
int y,x;
struct room rooms[10];
int i;
char input;
int get_index(int c)
{
	for(i=0;i<10;i++)
	{
		if(c==rooms[i].number)
			return i;
	}
	return -1;
}
int main()
{
	Portf_init();
	porTA_init();
	porTB_init();
	initialize_uart();	
	for(i=0;i<10;i++)
	{
		struct room r;
		r.password[0]= '7';
		r.password[1]= '7';
		r.password[2]= '7';
		r.password[3]= '7';
		r.open = 0;
		r.state = 0;
		y = uart_get();
		r.number = y;
		rooms[i] = r;
	}
	y = uart_get();
	x = uart_get();
	while(1)
	{
		if(x==0)
		{
			y = uart_get();
			y = get_index(y);
			rooms[y].state = x;
			rooms[y].open = 0;//always closed
			GPIO_PORTF_DATA_R = red;
			x = uart_get();
		}
		if(x==1)
		{
			y = uart_get();
			y = get_index(y);
			count = 0;
			while(count != 4)
			{
				input = getfromkeypad();
				if(input != 'x')
				{
					rooms[y].password[count] = input;
					count++;
				}
			}
			//changepasswordcode
			rooms[y].state = x;
			rooms[y].open = 0;
			GPIO_PORTF_DATA_R = red;
			count = 0;
			while(rooms[y].open == 0)
			{
				if (count == 4)
				{
					count = 0;
				}
				input = getfromkeypad();
				if(input != 'x')
				{
					pass[count] = input;
					count++;
				}
				if(count==4&&pass[0]==rooms[y].password[0] && pass[1]==rooms[y].password[1] && pass[2]==rooms[y].password[2] && pass[3]==rooms[y].password[3])
				{
					rooms[y].open = 1;
					GPIO_PORTF_DATA_R = blue;
				}
			}
			x = uart_get();
		}
		if(x==2)
		{
			y = uart_get();
			y = get_index(y);
			rooms[y].state = x;
			rooms[y].open = 1;//always open
			GPIO_PORTF_DATA_R = blue;
			x = uart_get();
		}
	}
}


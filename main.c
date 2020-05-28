#include "stdint.h"
#include "tm4c123gh6pm.h"
#include "keypad.h"
#include "uart.h"
#define red 0x02  //door closed
#define blue 0x04 //door opened
void SystemInit(){}
	
struct room
{
	char password[5];
	char number[3]; //can be from "00" to "99"
	char state;//0 for empty, 1 for reserved, 2 for cleaning
	int open; //1 if its opened and 0 if its closed
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


char pass[5]; //password entered by keypad or uart
int count = 0; 
char x,y_c[3];
struct room rooms[10];
int i,y;
char input;
int get_index(char c[3]) // we use it to get the room index by its number
{
	for(i=0;i<10;i++)
	{
		if(c[0]==rooms[i].number[0]&&c[1]==rooms[i].number[1])
			return i;
	}
	return 'x';
}
int main()
{
	Portf_init(); //used as indication to know if the door is opened or closed
	porTA_init(); //used for keypad
	porTB_init(); //used for uart and keypad
	initialize_uart();
	y_c[0] = uart_get(); //get number of rooms (max 9)
	y = y_c[0] - '0'; // convert the number from char to int;
	for(i=0;i<y;i++) //initialize rooms
	{
		struct room r;
		r.password[0]= '7'; //default password that can be changed when the room is occupied.
		r.password[1]= '7';
		r.password[2]= '7';
		r.password[3]= '7';
		r.open = 0;        //door is closed by default
		r.state = '0';
		y_c[0] = uart_get();
		y_c[1] = uart_get();
		r.number[0] = y_c[0]; //number can be from "00" to "99"
		r.number[1] = y_c[1];
		rooms[i] = r;
	}
	x = uart_get();  //choose state ('0' is for empty, '1' is for occupied and '2' is for cleaning)
	while(1)
	{
		if(x=='0') //room is empty (closed)
		{
			y_c[0] = uart_get();
			y_c[1] = uart_get();
			y = get_index(y_c); //which room
			rooms[y].state = x;
			rooms[y].open = 0;//always closed
			GPIO_PORTF_DATA_R = red; // red is for closed
			x = uart_get(); //enter next state
		}
		if(x=='1') //room is occupied
		{
			y_c[0] = uart_get();
			y_c[1] = uart_get();
			y = get_index(y_c); //which room
			count = 0;
			while(count != 4) //choose password
			{
				input = uart_get(); 
				rooms[y].password[count] = input;
				count++;
			}
			rooms[y].state = x;
			rooms[y].open = 0;
			GPIO_PORTF_DATA_R = red;// red is for closed (initially its closed)
			count = 0;
			while(rooms[y].open == 0) //now the user should enter the right password
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
					GPIO_PORTF_DATA_R = blue; //blue is for opened
				}
				while(input != 'x') //wait till the user let the button
				{
					input = getfromkeypad();
				}
			}
			x = uart_get(); //next state
		}
		if(x=='2') //room is empty
		{
			y_c[0] = uart_get();
			y_c[1] = uart_get();
			y = get_index(y_c); //which room
			rooms[y].state = x;
			rooms[y].open = 1;//always open
			GPIO_PORTF_DATA_R = blue; //blue is for opened
			x = uart_get(); //next state
		}
	}
}


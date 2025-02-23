
#include "STD_TYPE.h"
#include "BIT_MATH.h"
#include "DIO_Interface.h"
#include "SPI_Interface.h"
#include "MCP2515_Interface.h"
#include <util/delay.h>



void main(void)
{
	MCP2515_VidReceiveInit();
	UART_VidInit();
	DIO_VidSetPinDirection(DIO_PORTD, DIO_PIN2, DIO_INPUT);
	u8 dataR = 0;
	u8 readBuffer[];
	while(1)
	{
		if(DIO_U8GetPinValue(DIO_PORTD, DIO_PIN2) == 0)
		{
			MCP2515_VidReceiveCANmsg(readBuffer);

			for(u8 i=0; i<8; i++)
			{
				dataR = readBuffer[6+i];
				UART_VidSendData(dataR);
			}
			UART_VidSendData('\r');
			UART_VidSendData('\n');
		}

	}
}

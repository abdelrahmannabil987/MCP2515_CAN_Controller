#include "STD_TYPE.h"
#include "SPI_Interface.h"
#include "MCP2515_Interface.h"
#include "MCP2515_Private.h"

/*SPI interface with CAN Controller (MCP2515)*/
u8 MCP2515_U8ReadRegister(u8 address)
{
	u8 readValue;
	SPI_VidChipSelect(1);
	SPI_U8MasterTransmit(INST_READ); //instruction read
	SPI_U8MasterTransmit(address);//address of register you want to read
	readValue = SPI_U8MasterTransmit(0);//send any data to exchange data
	SPI_VidChipSelect(0);

	return readValue;
}

void MCP2515_VidWriteRegister(u8 address, u8 value)
{
	SPI_VidChipSelect(1);
	SPI_U8MasterTransmit(INST_WRITE);//instruction write
	SPI_U8MasterTransmit(address);//address of register you want to write in it
	SPI_U8MasterTransmit(value);//data
	SPI_VidChipSelect(0);
}


void MCP2515_VidReset()
{
	SPI_VidChipSelect(1);
	SPI_U8MasterTransmit(INST_RESET);//reset instruction
	SPI_VidChipSelect(0);
}

void MCP2515_VidChangeBits(u8 RegAddress, u8 Mask, u8 Value)
{
	SPI_VidChipSelect(1);
	SPI_U8MasterTransmit(INST_BIT_MODIFY);//modify instruction
	SPI_U8MasterTransmit(RegAddress);//address you want to modify its data
	SPI_U8MasterTransmit(Mask);//write 1 in front of bit you want to modify
	SPI_U8MasterTransmit(Value);//data
	SPI_VidChipSelect(0);
}


void MCP2515_VidSetBitTiming(u8 cnf1Value, u8 cnf2Value, u8 cnf3Value)
{
	MCP2515_VidWriteRegister(CNF1, cnf1Value);
	MCP2515_VidWriteRegister(CNF2, cnf2Value);
	MCP2515_VidWriteRegister(CNF3, cnf3Value);
}

/*CAN MODES page 59 */
void MCP2515_VidSetMode(u8 mode)
{

	MCP2515_VidChangeBits(CANCTRL, 0xE0, mode << 5);
	while((MCP2515_U8ReadRegister(CANSTAT)>>5) != mode );
}


void MCP2515_VidInit(void)//bit timing and enter normal mode to send or receive
{
	SPI_VidMasterInit();//Master mode in SPI
	MCP2515_VidReset();//better to reset CAN Controller

	// enter configuration mode and enable CLKOUT with no prescaler
	MCP2515_VidWriteRegister(CANCTRL, 0x84);
	while((MCP2515_U8ReadRegister(CANSTAT)>>5) != MCP2515_MODE_CONFG);

	// to run at 250KHz bit rate using 8MHz
	/*Calculations:
	 MCU frequency is 8 MHZ , MCP2515 frequency is 4 MHZ
	                          MCP2515 period time = quanta = 250 nsec
	 if i want 250 kHZ bit rate so each bit take 4MHZ/250KHZ = 16 clk cycle (quanta)
	 synch segment take 1 quanta
	 we will make propagation + phase1 segments = 9 quantas
	              phase segment 2 = 6 quantas
	              so ratio (sample point) is 3:2
	 we will make SJW(synch jump width)=3 to jump well
	              */
//CNF1 for SJW --> put 2 for SJW=3
/*CNF2 for Propagation segment and PS1 lengths
(assume prop segment takes 2 quantas and PS1 takes 7 quantas)*/
//CNF3 for PS2 --> 6 quantas
	MCP2515_VidSetBitTiming((2<<6), (1<<7)|(6<<3)|(1), (5));
//set normal mode for transmit and receive
	MCP2515_VidSetMode(MCP2515_MODE_NORMAL);
}


void MCP2515_VidSendCANmsg(u8 bufIdx, u32 msgID, u8 * data,u8 DLC)
{
	//look at register map in data sheet
	SPI_VidChipSelect(1);
	/* Send header and address */
	SPI_U8MasterTransmit(INST_WRITE);
	/*select transmit buffer 0 for example(you have 3 transmit buffers)*/
	SPI_U8MasterTransmit(TXB0CTRL);
    
	/* Setup message priority */
	SPI_U8MasterTransmit(DLC >> 6);//any value
	/* Setup standard or extended identifier */
		SPI_U8MasterTransmit((u8)(msgID>>3));//in TXB0SIDH register from (bit3 to bit10)
		SPI_U8MasterTransmit((u8)(msgID<<5));//in TXB0SIDL register from (bit0 to bit2)
		SPI_U8MasterTransmit(0);
		SPI_U8MasterTransmit(0);
        
	/* Setup message length and RTR bit */
	SPI_U8MasterTransmit(DLC & 0x0F);//in TXBnDLC for data length code
    
	/* Store the message into the buffer */
	for(u8 i = 0; i < (DLC & 0x0F); i++)
		SPI_U8MasterTransmit(data[i]);
    
	/* Release the bus */
	SPI_VidChipSelect(0);
    
	/* Send request to send */
	SPI_VidChipSelect(1);
	SPI_U8MasterTransmit(INST_RTS_TXB0);//request to send
	SPI_VidChipSelect(0);
	}

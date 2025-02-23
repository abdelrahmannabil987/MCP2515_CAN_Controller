#include "STD_TYPE.h"
#include "SPI_Interface.h"
#include "MCP2515_Interface.h"
#include "MCP2515_Private.h"

/*SPI interface*/
u8 MCP2515_U8ReadRegister(u8 address)
{
	u8 readValue;
	SPI_VidChipSelect(1);
	SPI_U8MasterTransmit(INST_READ);
	SPI_U8MasterTransmit(address);
	readValue = SPI_U8MasterTransmit(0);
	SPI_VidChipSelect(0);

	return readValue;
}

void MCP2515_VidWriteRegister(u8 address, u8 value)
{
	SPI_VidChipSelect(1);
	SPI_U8MasterTransmit(INST_WRITE);
	SPI_U8MasterTransmit(address);
	SPI_U8MasterTransmit(value);
	SPI_VidChipSelect(0);
}


void MCP2515_VidReset()
{
	SPI_VidChipSelect(1);
	SPI_U8MasterTransmit(INST_RESET);
	SPI_VidChipSelect(0);
}

void MCP2515_VidChangeBits(u8 RegAddress, u8 Mask, u8 Value)
{
	SPI_VidChipSelect(1);
	SPI_U8MasterTransmit(INST_BIT_MODIFY);
	SPI_U8MasterTransmit(RegAddress);
	SPI_U8MasterTransmit(Mask);
	SPI_U8MasterTransmit(Value);
	SPI_VidChipSelect(0);
}

void MCP2515_VidSetBitTiming(u8 cnf1Value, u8 cnf2Value, u8 cnf3Value)
{
	MCP2515_VidWriteRegister(CNF1, cnf1Value);
	MCP2515_VidWriteRegister(CNF2, cnf2Value);
	MCP2515_VidWriteRegister(CNF3, cnf3Value);
}

void MCP2515_VidSetMode(u8 mode)
{

	MCP2515_VidChangeBits(CANCTRL, 0xE0, mode << 5);
	while((MCP2515_U8ReadRegister(CANSTAT)>>5) != mode );
}

/* BUKT: Rollover Enable bit
 1 = RXB0 message will roll over and be written to RXB1 if RXB0 is full
 0 = Rollover is disabled */
void MCP2515_VidSetRollover(u8 value)
{
	MCP2515_VidChangeBits(RXB0CTRL, 1<<BUKT, value<<BUKT);
}


/*There are two masks for receive buffer 0 and receive buffer 1 */
void MCP2515_VidSetMask(u8 maskAddress, u32 maskValue, u8 extended)
{
	SPI_VidChipSelect(1);
	SPI_U8MasterTransmit(INST_WRITE);
	SPI_U8MasterTransmit(maskAddress);
//page 37 in data sheet
	if(extended)//1
	{
		// extended address
		SPI_U8MasterTransmit((u8) (maskValue >> 3));// from bit 3 to bit 10
		/*from bit 0 to bit 2
		 put last two bits of the 18 bits extended(bit16,bit17) (28,29 in all ID so shift 27)*/
		SPI_U8MasterTransmit((u8) (maskValue << 5) | (u8) (maskValue >> 27));
		SPI_U8MasterTransmit((u8) (maskValue >> 19));//from bit8 to bit15 in extended
		SPI_U8MasterTransmit((u8) (maskValue >> 11));//first 8 bits in extended
	}
	else//0
	{
		// standard address
		SPI_U8MasterTransmit((u8) (maskValue >> 3));
		SPI_U8MasterTransmit((u8) (maskValue << 5));
		SPI_U8MasterTransmit(0);
		SPI_U8MasterTransmit(0);

	}
	SPI_VidChipSelect(0);
}

/*set filters value
  There are 6 filters*/
void MCP2515_VidSetFilters(u8 filterNUMBER, u32 filterValue)
{
	SPI_VidChipSelect(1);
	SPI_U8MasterTransmit(INST_WRITE);
	SPI_U8MasterTransmit(filterNUMBER);//sending filter address
	SPI_U8MasterTransmit((u8)filterValue >> 3);
	SPI_U8MasterTransmit((u8)filterValue << 5);
	SPI_U8MasterTransmit(0);
	SPI_U8MasterTransmit(0);
	SPI_VidChipSelect(1);
}
void MCP2515_VidReceiveInit(void)
{
	SPI_VidMasterInit();
	MCP2515_VidReset();

//	// enter configuration mode and enable CLKOUT with no prescaler
	MCP2515_VidWriteRegister(CANCTRL, 0x84);
	while((MCP2515_U8ReadRegister(CANSTAT)>>5) != MCP2515_MODE_CONFG);
//	// to run at 250KHz bitrate using 8MHz osc
	MCP2515_VidSetBitTiming((2<<6), (1<<7)|(6<<3)|(1), (5));

	/*accept only the write messages
	 (11 bits are 1 mean compare all received ID bits with filters)*/
	MCP2515_VidSetMask(RXM0SIDH, 0b11111111111 , 0);
	MCP2515_VidSetMask(RXM1SIDH, 0b11111111111 , 0);

	/*set two filters of the 6
	 for two applications
	 received frame ID must be = one of both Filter Values to be accepted*/
	MCP2515_VidSetFilters(Filter0, 0b00000001111);//msg1
	MCP2515_VidSetFilters(Filter1, 0b00011001100);//msg2
	MCP2515_VidSetFilters(Filter2, 0b00011111100);//msg3
	MCP2515_VidSetFilters(Filter3, 0b00001011100);//msg4
	MCP2515_VidSetFilters(Filter4, 0b01011111100);//msg5
	MCP2515_VidSetFilters(Filter5, 0b01111101100);//msg6


	MCP2515_VidSetRollover(1);

	MCP2515_VidWriteRegister(CANINTE, 1<<RX0IE);//interrupt enable

	MCP2515_VidSetMode(MCP2515_MODE_NORMAL);
}


void MCP2515_VidReceiveCANmsg(u8* readBuffer)
{
	SPI_VidChipSelect(1);

	SPI_U8MasterTransmit(INST_READ);
	SPI_U8MasterTransmit(RXB0CTRL);

	for(u8 i=0; i<14; i++)
	{
		readBuffer[i] = SPI_U8MasterTransmit(0);//send any value for exchanging
	}

	SPI_VidChipSelect(0);

	MCP2515_VidWriteRegister(CANINTF, 0);//disable interrupt
}

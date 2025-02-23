#ifndef MCP2515_INTERFACE_H_
#define MCP2515_INTERFACE_H_


u8 MCP2515_U8ReadRegister(u8 address);
void MCP2515_VidWriteRegister(u8 address, u8 value);
void MCP2515_VidSetBitTiming(u8 cnf1Value, u8 cnf2Value, u8 cnf3Value);
void MCP2515_VidReset();
void MCP2515_VidChangeBits(u8 RegAddress, u8 Mask, u8 Value);
void MCP2515_VidSetMode(u8 mode);
void MCP2515_VidReceiveInit(void);
void MCP2515_VidInit(void);
void MCP2515_VidSetFilters(u8 filterNUMBER, u32 filterValue);
void MCP2515_VidSetMask(u8 maskAddress, u32 maskValue, u8 extended);
void MCP2515_VidReceiveCANmsg(u8* readBuffer);

#define MCP2515_MODE_NORMAL			0x0
#define MCP2515_MODE_SLEEP			0x1
#define MCP2515_MODE_LOOPBACK		0x2
#define MCP2515_MODE_LISTEN_ONLY	0x3
#define MCP2515_MODE_CONFG			0x4

//there are 6 filters
#define Filter0    0b00000000
#define Filter1    0b00000100
#define Filter2    0b10000000
#define Filter3    0b00010000
#define Filter4    0b00010100
#define Filter5    0b00011000

#endif /* MCP2515_INTERFACE_H_ */

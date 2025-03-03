#ifndef MCP2515_PRIVATE_H_
#define MCP2515_PRIVATE_H_

/*spi interface instruction set table
  in MCP2515 page 67 */
#define INST_RESET			(0xC0)
#define INST_READ			(0x03)
#define INST_WRITE			(0x02)
#define INST_RTS_TXB0		(0x81)
#define INST_RTS_TXB1		(0x82)
#define INST_RTS_TXB2		(0x84)
#define INST_READ_STATUS	(0xA0)
#define INST_RX_STATUS		(0xB0)
#define INST_BIT_MODIFY		(0x05)


#define CNF1				(0x2A)
#define CNF2				(0x29)
#define CNF3				(0x28)

#define CANCTRL				(0x0F)
#define CANSTAT				(0x0E)

#define TXB0CTRL			(0x30)

#endif /* MCP2515_PRIVATE_H_ */

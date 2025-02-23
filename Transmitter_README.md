# MCP2515 CAN Bus Communication with ATmega32

## Overview
The **MCP2515** is a standalone **CAN controller** that interfaces with a microcontroller through the **SPI** protocol. It allows communication over the **Controller Area Network (CAN)**, which is widely used in automotive and industrial applications.

This repository contains a **C implementation** for transmitting CAN messages using the **MCP2515 with ATmega32**.

## Features
- **SPI Interface:** Communicates with the MCP2515 using SPI.
- **CAN Message Transmission:** Supports sending messages with configurable IDs and data.
- **Bit Timing Configuration:** Allows setting custom bit timing for the CAN bus.
- **Mode Control:** Supports different CAN modes such as Configuration and Normal mode.

## Hardware Requirements
- **ATmega32 Microcontroller**
- **MCP2515 CAN Module**
- **CAN Bus Transceiver (e.g., TJA1050)**
- **SPI-compatible connections**

## Wiring (ATmega32 with MCP2515)
| MCP2515 Pin | ATmega32 Pin |
|------------|------------|
| VCC        | 5V         |
| GND        | GND        |
| CS         | PB4 (SS)   |
| SCK        | PB7 (SCK)  |
| MOSI       | PB5 (MOSI) |
| MISO       | PB6 (MISO) |
| INT        | PD2 (INT0) |

## Code Explanation
### 1. SPI Interface with MCP2515
Functions for **reading**, **writing**, and **modifying registers** through SPI.
```c
#include <avr/io.h>
#include "SPI_Interface.h"
#include "MCP2515_Interface.h"

// Function to read a register from MCP2515
u8 MCP2515_U8ReadRegister(u8 address) {
    u8 readValue;
    SPI_VidChipSelect(1); // Enable MCP2515
    SPI_U8MasterTransmit(INST_READ); // Send READ instruction
    SPI_U8MasterTransmit(address); // Send register address
    readValue = SPI_U8MasterTransmit(0); // Read value
    SPI_VidChipSelect(0); // Disable MCP2515
    return readValue;
}
```

### 2. Initializing MCP2515 with ATmega32
Setting up SPI and configuring MCP2515 for communication.
```c
void MCP2515_VidInit(void) {
    SPI_VidMasterInit(); // Initialize SPI as master
    MCP2515_VidReset(); // Reset MCP2515 to default state
    
    // Enter configuration mode and enable CLKOUT
    MCP2515_VidWriteRegister(CANCTRL, 0x84);
    while((MCP2515_U8ReadRegister(CANSTAT) >> 5) != MCP2515_MODE_CONFG);
    
    // Set bit timing for 250kbps CAN communication
    MCP2515_VidSetBitTiming((2<<6), (1<<7)|(6<<3)|(1), (5));
    
    // Set MCP2515 to normal mode for transmission and reception
    MCP2515_VidSetMode(MCP2515_MODE_NORMAL);
}
```

### 3. Sending a CAN Message with ATmega32
Function to send a CAN message using **Transmit Buffer 0**.
```c
void MCP2515_VidSendCANmsg(u8 bufIdx, u32 msgID, u8 * data, u8 DLC) {
    SPI_VidChipSelect(1); // Enable MCP2515
    SPI_U8MasterTransmit(INST_WRITE); // Send WRITE instruction
    SPI_U8MasterTransmit(TXB0CTRL); // Select TX buffer 0
    
    // Set priority (not crucial for basic use)
    SPI_U8MasterTransmit(DLC >> 6);
    
    // Load message ID (standard 11-bit ID)
    SPI_U8MasterTransmit((u8)(msgID >> 3)); // High 8 bits
    SPI_U8MasterTransmit((u8)(msgID << 5)); // Low 3 bits
    SPI_U8MasterTransmit(0); // Extended ID not used
    SPI_U8MasterTransmit(0);
    
    // Set Data Length Code (DLC) for number of bytes
    SPI_U8MasterTransmit(DLC & 0x0F);
    
    // Load data bytes into transmit buffer
    for(u8 i = 0; i < (DLC & 0x0F); i++) {
        SPI_U8MasterTransmit(data[i]);
    }
    
    SPI_VidChipSelect(0); // Disable MCP2515
    
    // Request to send message from TX buffer 0
    SPI_VidChipSelect(1);
    SPI_U8MasterTransmit(INST_RTS_TXB0);
    SPI_VidChipSelect(0);
}
```

## Example Usage for ATmega32
```c
#include "MCP2515_Interface.h"

int main(void) {
    u8 data[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    MCP2515_VidInit(); // Initialize MCP2515
    while (1) {
        MCP2515_VidSendCANmsg(0, 0x123, data, 8); // Send message every second
        _delay_ms(1000);
    }
    return 0;
}
```

## How to Use
1. **Connect the hardware** as per the wiring diagram.
2. **Compile and upload** the C code to your ATmega32.
3. **Monitor the CAN bus** using an oscilloscope or another CAN node.

## Future Improvements
- **Receive message implementation**
- **Interrupt-based message handling**
- **Error handling and diagnostics**

## License
This project is open-source and available under the **MIT License**.


/* can_rx.c
 *
 * Written for the EK-TM4C123GXL
 *
 * This code sends a message on the CAN0 peripheral and increments
 * the transmitted data after every send.
 * This is meant to be used in conjunction with the can_rx.c code,
 * which receives a message and outputs the 4-bit contents on GPIO E0-E3
 *
 * The CAN0 peripheral is set up for pins E4 (RX) and E5 (TX).
 *
 * Most of this code was taken from the CAN example project for the DK-TM4C123G
 * Other parts taken from example code written by Professor Gene Bogdanov
 * Modified by Ben Titus
 */


#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/can.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"

// Counter for number of transmitted messages
volatile uint32_t g_ui32TXMsgCount = 0;

// Global error flags
volatile uint32_t g_ui32ErrFlag = 0;

// CAN message Object
tCANMsgObject g_sCAN0TxMessage;

// Set TXID to 2
#define CAN0TXID                2

// Set TXOBJECT to channel 2
#define TXOBJECT                2

// Variable to hold transmitted data
uint16_t g_ui8TXMsgData;

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

// CAN 0 Interrupt Handler. It checks for the cause of the interrupt, and
// maintains a count of all messages that have been transmitted / received
void
CAN0IntHandler(void)
{
    uint32_t ui32Status;

    // Read the CAN interrupt status to find the cause of the interrupt
    //
    // CAN_INT_STS_CAUSE register values
    // 0x0000        = No Interrupt Pending
    // 0x0001-0x0020 = Number of message object that caused the interrupt
    // 0x8000        = Status interrupt
    // all other numbers are reserved and have no meaning in this system
    ui32Status = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);

    // If this was a status interrupt acknowledge it by reading the CAN
    // controller status register.
    if(ui32Status == CAN_INT_INTID_STATUS)
    {
        // Read the controller status.  This will return a field of status
        // error bits that can indicate various errors. Refer to the
        // API documentation for details about the error status bits.
        // The act of reading this status will clear the interrupt.
        ui32Status = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);

        // Add ERROR flags to list of current errors. To be handled
        // later, because it would take too much time here in the
        // interrupt.
        g_ui32ErrFlag |= ui32Status;
    }

    // Check if the cause is message object TXOBJECT, which we are using
    // for transmitting messages.
    else if(ui32Status == TXOBJECT)
    {
        // Set message type to RX so that it doesn't continually transmit messages
        // Could get away with using a shutdown or enable pin on transceiver, but
        // TCAN332 doesn't have those features
        CANMessageSet(CAN0_BASE, TXOBJECT, &g_sCAN0TxMessage, MSG_OBJ_TYPE_RX);

        // Clear the message object interrupt.
        CANIntClear(CAN0_BASE, TXOBJECT);

        // Increment a counter to keep track of how many messages have been transmitted.
        g_ui32TXMsgCount++;

        // Since a message was transmitted, clear any error flags.
        g_ui32ErrFlag = 0;
    }

    // Otherwise, something unexpected caused the interrupt.  This should
    // never happen.
    else
    {
        // Spurious interrupt handling can go here.
    }
}

// Setup CAN0 to send messages at 1MHz
// Use PE4 / PE5
// Enable interrupts
void InitCAN0(void) {

    // Enable Port E
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    // Set pins to be used for CAN
    GPIOPinConfigure(GPIO_PE4_CAN0RX);
    GPIOPinConfigure(GPIO_PE5_CAN0TX);

    // Enable CAN functionality for pins E4, E5
    GPIOPinTypeCAN(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    // Enable CAN0 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);

    // Initialize CAN0 controller
    CANInit(CAN0_BASE);

    // Set CAN0 to run at 1Mbps
    CANBitRateSet(CAN0_BASE, SysCtlClockGet(), 1000000);

    // Enable interrupts, error interrupts, and status interrupts
    CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);

    // Enable CAN0 interrupt in interrupt controller
    IntEnable(INT_CAN0);

    // Enable CAN0
    CANEnable(CAN0_BASE);

    // Initialize the transmit message object
    // Set ID to 2
    g_sCAN0TxMessage.ui32MsgID = CAN0TXID;

    // Set mask to 0, doesn't matter for this
    g_sCAN0TxMessage.ui32MsgIDMask = 0;

    // Set TX interrupt flag
    g_sCAN0TxMessage.ui32Flags = MSG_OBJ_TX_INT_ENABLE;

    // Set length to 1 byte
    g_sCAN0TxMessage.ui32MsgLen = sizeof(g_ui8TXMsgData);

    // Set the message data pointer
    g_sCAN0TxMessage.pui8MsgData = (uint8_t *)&g_ui8TXMsgData;
}

// Error handler for CAN errors
// Fill in with what to do
void CANErrorHandler(void) {
    // If the bus is off (Too many errors happened)
    if(g_ui32ErrFlag & CAN_STATUS_BUS_OFF) {
        // Clear the flag
        g_ui32ErrFlag &= ~(CAN_STATUS_BUS_OFF);
    }

    // If there have been many of errors (more than 96)
    if(g_ui32ErrFlag & CAN_STATUS_EWARN) {
        // Clear the flag
        g_ui32ErrFlag &= ~(CAN_STATUS_EWARN);
    }

    // If there have been a lot of errors (more than 127)
    if(g_ui32ErrFlag & CAN_STATUS_EPASS) {
        // Clear the flag
        g_ui32ErrFlag &= ~(CAN_STATUS_EPASS);
    }

    // Received message successfully
    if(g_ui32ErrFlag & CAN_STATUS_RXOK) {
        // Clear the flag
        g_ui32ErrFlag &= ~(CAN_STATUS_RXOK);
    }

    // Transmit message successfully
    if(g_ui32ErrFlag & CAN_STATUS_TXOK) {
        // Clear the flag
        g_ui32ErrFlag &= ~(CAN_STATUS_TXOK);
    }

    // Check the LEC
    if(g_ui32ErrFlag & CAN_STATUS_LEC_MSK) {
        // Clear the flag
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_MSK);
    }

    // Bit stuffing error
    if(g_ui32ErrFlag & CAN_STATUS_LEC_STUFF) {
        // Clear the flag
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_STUFF);
    }

    // Formatting error
    if(g_ui32ErrFlag & CAN_STATUS_LEC_FORM) {
        // Clear the flag
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_FORM);
    }

    // Acknowledge error
    if(g_ui32ErrFlag & CAN_STATUS_LEC_ACK) {
        // Clear the flag
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_ACK);
    }

    // The bus remained a bit level of 1 for longer than is allowed.
    if(g_ui32ErrFlag & CAN_STATUS_LEC_BIT1) {
        // Clear the flag
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_BIT1);
    }

    // The bus remained a bit level of 0 for longer than is allowed.
    if(g_ui32ErrFlag & CAN_STATUS_LEC_BIT0) {
        // Clear the flag
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_BIT0);
    }

    // CRC error
    if(g_ui32ErrFlag & CAN_STATUS_LEC_CRC) {
        // Clear the flag
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_CRC);
    }

    // LEC mask
    if(g_ui32ErrFlag & CAN_STATUS_LEC_MASK) {
        // Clear the flag
        g_ui32ErrFlag &= ~(CAN_STATUS_LEC_MASK);
    }

    // If there are any bits still set in g_ui32ErrFlag then something unhandled has happened.
    if(g_ui32ErrFlag !=0) {
        // Handle this here, shouldn't ever get here
    }
}

// Set up the system, initialize CAN
// Busy wait for ~1 second before transmitting a message on CAN0
// Increment message data after transmitting
int
main(void)
{
    IntMasterDisable();

    // Set the clocking to 50MHz
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    // Initialize CAN0
    InitCAN0();

    // Local variables for timing everything
    unsigned int count = 0;
    uint8_t msg = 0;

    IntMasterEnable();

    // Poll UART for data, transmit across CAN when something is entered
    while(1) {
        count++;

        // When count has reached a big number
        if (count >= 4000000) {
            count = 0;

            // Only transmit if there are no errors
            if (g_ui32ErrFlag == 0) {

                // Set message data pointer
                *g_sCAN0TxMessage.pui8MsgData = msg;

                // increment message data value and mask it to 4 bits
                msg++;
                msg &= 0x000F;

                // Send the CAN message using object number 2
                CANMessageSet(CAN0_BASE, TXOBJECT, &g_sCAN0TxMessage, MSG_OBJ_TYPE_TX);
            }
        }
    }
}

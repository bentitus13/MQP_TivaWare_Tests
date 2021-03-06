/* can_rx.c
 *
 * Written for the EK-TM4C123GXL
 *
 * This code receives a message on the CAN0 peripheral and outputs the lower 4 bits on
 * GPIO pins E0-E3. This is meant to be used in conjunction with the can_tx.c code
 * which transmits a 4 bit message, incrementing the value every transmission.
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

// Number of received messages
volatile uint32_t g_ui32RXMsgCount = 0;

// Global receive flag
volatile bool g_bRXFlag = 0;

// Global error flags
volatile uint32_t g_ui32ErrFlag = 0;

// CAN message object
tCANMsgObject g_sCAN0RxMessage;

// Set RXID to 0 to receive all messages
#define CAN0RXID                0

// Set RXOBJECT to channel 1
#define RXOBJECT                1

// Variable to hold received data
uint8_t g_pui8RXMsgData[8];

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

// Function prototype for writing the LEDs
void writeLEDs(uint8_t leds);


void
CAN0IntHandler(void)
{
    /* This ISR is from example code written by Professor Gene Bogdanov
     * It has been modified slightly to fit with the rest of the example code
     */
    unsigned long ulStatus;

    ulStatus = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);
    switch(ulStatus)
    {
    case RXOBJECT: // message received

        // Set message data pointer
        g_sCAN0RxMessage.pui8MsgData = g_pui8RXMsgData;

        // Get message data
        CANMessageGet(CAN0_BASE, RXOBJECT, &g_sCAN0RxMessage, 1);

        // Write received data to LEDs
        writeLEDs(g_pui8RXMsgData[0]);

        // Increment received message count
        g_ui32RXMsgCount++;
        break;
    default: // status or other interrupt: clear it and set error flags
        g_ui32ErrFlag |= CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);
        CANIntClear(CAN0_BASE, ulStatus);
    }
}

// Write LEDs to the message received on the CAN bus
void writeLEDs(uint8_t leds) {
    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, leds&0x0F);
}

// Enable CAN0 to receive messages at 1MHz
// Use PE4/PE5
// Enable interrupts
void InitCAN0(void) {
    // Enable port E
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    // Enable pins E0, E1, E2, E3
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

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

    // Set up receive message object
    g_sCAN0RxMessage.ui32MsgID = 0x188;     // Accept any ID
    g_sCAN0RxMessage.ui32MsgIDMask = 0; // Accept any ID
    // Interrupt enable and use ID filter
    g_sCAN0RxMessage.ui32Flags = MSG_OBJ_RX_INT_ENABLE | MSG_OBJ_USE_ID_FILTER;
//    g_sCAN0RxMessage.ui32Flags = MSG_OBJ_RX_INT_ENABLE;
    // Message length = 1 byte (g_ui8RXMsgData is 8 bits long)
//    g_sCAN0RxMessage.ui32MsgLen = sizeof(g_ui8RXMsgData);
    g_sCAN0RxMessage.ui32MsgLen = 0x02;
    g_sCAN0RxMessage.pui8MsgData = (unsigned char *)&g_pui8RXMsgData;

    // Load message 1 with g_sCAN0RxMessage settings
    CANMessageSet(CAN0_BASE, RXOBJECT, &g_sCAN0RxMessage, MSG_OBJ_TYPE_RX);
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
// Check flags for errors, if there are any call the error handler
int main(void) {
    // Disable interrupts so nothing bad happens while setting up peripherals
    IntMasterDisable();

    // Set the clocking to run at 50MHz.
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);

    // Initialize CAN0
    InitCAN0();

    // Enable interrupts
    IntMasterEnable();

    while(1) {
        // Check to see if there is an indication that some messages were lost.
        if(g_sCAN0RxMessage.ui32Flags & MSG_OBJ_DATA_LOST) {
            // Handle lost data here
        }

        // Error Handling
        if(g_ui32ErrFlag != 0) {
            CANErrorHandler();
        }
    }
}

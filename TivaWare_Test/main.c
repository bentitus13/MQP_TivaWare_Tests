
//#include <xdc/std.h>
//#include <xdc/runtime/System.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "inc/hw_can.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"

#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/can.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/ssi.h"
#include "driverlib/systick.h"
#include "driverlib/timer.h"

tCANMsgObject sMsgObjectRx; // Receive  CAN message settings
tCANMsgObject sMsgObjectTx; // Transmit CAN message settings
uint8_t ui8CANMsgData;      // CAN message data

uint32_t g_i32Value;        // Value from the ADC
uint32_t g_ui32PWMValue;    // Value to PWM Generator
uint32_t g_ui32SPIData;     // Vale from SPI ADC
bool state = false;         // State of the ADC waveform pin

void setPins(void) {
    // Initialize PE0 as ADC input
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_0);

    // Initialize PB0, PB1, PB5 as GPIO output
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_5);
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_2);

    // Initialize PB6 as PWM output
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinConfigure(GPIO_PB6_M0PWM0);
    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_6);

    // Initialize SSI
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA2_SSI0CLK);          // PA2 -> SCLK
    GPIOPinConfigure(GPIO_PA3_SSI0FSS);          // PA3 -> CS
    GPIOPinConfigure(GPIO_PA4_SSI0RX);           // PA4 -> MISO
    GPIOPinConfigure(GPIO_PA5_SSI0TX);           // PA5 -> MOSI
    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5);

    // Initialize PE4 and PE5 as CAN0Rx and CAN0Tx respectively
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    GPIOPinConfigure(GPIO_PE4_CAN0RX);
    GPIOPinConfigure(GPIO_PE5_CAN0TX);
    GPIOPinTypeCAN(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);
}

void getADC(void) {
    // Clear ADC0SS0 interrupt
    ADCIntClear(ADC0_BASE, 0);
    // Reset P? to measure ADC frequency
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, 0);
    // Retrieve the reading
    ADCSequenceDataGet(ADC0_BASE, 0, &g_i32Value);

    g_i32Value -= 2048;
    if ((int) g_i32Value < 0) {
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, GPIO_PIN_5);
        g_ui32PWMValue = (int) g_i32Value*(-2.5);
        PWMPulseWidthSet(PWM0_BASE, PWM_GEN_0, g_ui32PWMValue);
    } else {
        g_ui32PWMValue = g_i32Value*2.5;
        PWMPulseWidthSet(PWM0_BASE, PWM_GEN_0, g_ui32PWMValue);
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, 0);
    }

    SSIDataPut(SSI0_BASE, 0xD000);
    while(SSIBusy(SSI0_BASE)) ;
    SSIDataGet(SSI0_BASE, &g_ui32SPIData);
    g_ui32SPIData &= 0x0FFF;
}

void setADC(void) {
    // Initialize ADC0 Module
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

//    // Wait for the module to be ready
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0)) {
    }

    ADCSequenceDisable(ADC0_BASE, 0);
//    // ADC_PC_SR_125k, ADC_PC_SR_250k, ADC_PC_SR_500k, ADC_PC_SR_1M
//    ADC0_BASE + ADC_O_PC = (ADC_PC_SR_1M);

    // ADC clock
    // Source - internal PIOSC at 16MHz, clock rate full for now
    // Divider - 1 for now, could change later
    // Maybe use PLL if the frequency isn't high enough
//    ADCClockConfigSet(ADC0_BASE, ADC_CLOCK_SRC_PIOSC | ADC_CLOCK_RATE_FULL, 2);

    // Trigger when the processor tells it to (one shot)
    ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0);

    // Take a sample and interrupt
    ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH3 | ADC_CTL_IE | ADC_CTL_END);

    // Oversample at 16x (could go higher maybe? lower?)
    ADCHardwareOversampleConfigure(ADC0_BASE, 16);

    ADCSequenceEnable(ADC0_BASE, 0);

    ADCIntEnable(ADC0_BASE, 0);
    ADCIntRegister(ADC0_BASE, 0, &getADC);
}

void startADC(void) {
    // Set PB1 to measure ADC frequency
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, GPIO_PIN_1);
    // Start the ADC in one-shot mode
    ADCProcessorTrigger(ADC0_BASE, 0);
    // Move to ISR
//    while(!ADCIntStatus(ADC0_BASE, 0, false));  // wait until the sample sequence has completed
//    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, 0);
//    ADCSequenceDataGet(ADC0_BASE, 0, &g_ui32Value);// retrieve joystick data
//    ADCIntClear(ADC0_BASE, 0);                  // clear ADC sequence interrupt flag
}

uint32_t led;
void sendCAN(void) {
    if (led) {
        *sMsgObjectTx.pui8MsgData = (*sMsgObjectRx.pui8MsgData == 1) ? 0: 1;
        CANMessageSet(CAN0_BASE, 2, &sMsgObjectTx, MSG_OBJ_TYPE_TX);
    }
}
void writeLED(void) {
    led = (*sMsgObjectRx.pui8MsgData == 0) ? 0: 1;
    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, led);
}
int count;
void timerISR(void) {
    TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    startADC();
    count++;
    if (count > 500) {
        count = 0;
        writeLED();
        sendCAN();
    }
}

void CANISR(void) {
    uint32_t ui32Status;

    //
    // Read the CAN interrupt status to find the cause of the interrupt
    //
    ui32Status = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE);

    switch(ui32Status) {
    case CAN_INT_INTID_STATUS:
        // Read error status
        ui32Status = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);
        break;
    case 1: // Message object 1 received message
        // Clear interrupt
        CANIntClear(CAN0_BASE, 1);

        // Read in message (don't normally do this in ISR)
        CANMessageSet(CAN0_BASE, 1, &sMsgObjectRx, MSG_OBJ_TYPE_RX);
        break;
    default:
        break;
    }
}

void setTimer(void) {
    // Enable Timer1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
//    TimerDisable(TIMER1_BASE, TIMER_A);
    // Set clock source
    TimerClockSourceSet(TIMER1_BASE, TIMER_CLOCK_SYSTEM);
    // Set to periodic
    TimerConfigure(TIMER1_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC);
    // Set the prescaler to 1
    TimerPrescaleSet(TIMER1_BASE, TIMER_A, 0);
    // Set load value
    TimerLoadSet(TIMER1_BASE, TIMER_A, 0x0FFFF);
    // Set compare value
//    TimerMatchSet(TIMER1_BASE, TIMER_A, 0x07FFF);
    // Enable timeout interrupt
    TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);
    // Register ISR
    TimerIntRegister(TIMER1_BASE, TIMER_A, &timerISR);
    // Enable Timer1A
    TimerEnable(TIMER1_BASE, TIMER_A);
}

void setPWM(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

    SysCtlPWMClockSet(SYSCTL_PWMDIV_32);
//    PWMClockSet(PWM_BASE, PWM_SYSCLK_DIV_64);

    PWMDeadBandDisable(PWM0_BASE, PWM_GEN_0);

    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, 5120);

    PWMPulseWidthSet(PWM0_BASE, PWM_GEN_0, 2560);

    PWMOutputState(PWM0_BASE, PWM_OUT_0_BIT, true);

    PWMOutputUpdateMode(PWM0_BASE, PWM_OUT_0_BIT, PWM_OUTPUT_MODE_NO_SYNC);

    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
}

void setSSI(void) {
    // Enable peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);

    // Set clock to system clock
    SSIClockSourceSet(SSI0_BASE, SSI_CLOCK_SYSTEM);

    /*  Configure SSI:
     *  Clock speed -> System clock
     *  Mode -> polarity 0, phase 0
     *
     */
    SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 500000, 16);
    SSIEnable(SSI0_BASE);

    // Initialize MCP3202
    SSIDataPut(SSI0_BASE, 0xD000);
    while(SSIBusy(SSI0_BASE)) ;
    SSIDataGet(SSI0_BASE, &g_ui32SPIData);
}

void setCAN(void) {
    // Enable peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);

    // Reset CAN module and objects associated with it
    CANInit(CAN0_BASE);

    // Configure controller for 1Mbit operation
    CANBitRateSet(CAN0_BASE, SysCtlClockGet(), 1000000);

    // Initialize object to accept messages with ID
    sMsgObjectRx.ui32MsgID = (0x400);   // Look for messages with ID     = 1XX_XXXX_XXXX
    sMsgObjectRx.ui32MsgIDMask = 0x7E0; // Filter out messages with mask = 000_0000_0111
    // Filter IDs with mask and enables Rx interrupt
    sMsgObjectRx.ui32Flags = MSG_OBJ_USE_ID_FILTER | MSG_OBJ_RX_INT_ENABLE;

    // set up CAN objects with settings in sMsgObjectRx as receive message objects
    CANMessageSet(CAN0_BASE, 1, &sMsgObjectRx, MSG_OBJ_TYPE_RX); // CAN object 1
//    CANMessageSet(CAN0_BASE, 2, &sMsgObjectRx, MSG_OBJ_TYPE_RX); // CAN object 2
//    CANMessageSet(CAN0_BASE, 3, &sMsgObjectRx, MSG_OBJ_TYPE_RX); // CAN object 3

    // Set message data for 1 byte transmission
    ui8CANMsgData = 0x01;

    // Set up transmit message
    sMsgObjectTx.ui32MsgID = 0x400 | led; // ID = 0x400
    sMsgObjectTx.ui32Flags = 0;     // No flags
    sMsgObjectTx.ui32MsgLen = 1;    // Length 1
    sMsgObjectTx.pui8MsgData = &ui8CANMsgData; // Give some data

    // Set up CAN0 interrupts
    CANIntEnable(CAN0_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);

    // Register the ISR CANISR()
    CANIntRegister(CAN0_BASE, CANISR);

    // Enable CAN0 interrupt
    IntEnable(INT_CAN0);

    //

    // Enable CAN0 device
    CANEnable(CAN0_BASE);
}


/**
 * main.c
 */
int main(void) {
    IntMasterDisable();

    // Set clock speed to 40MHz ?
    SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

    setPins();
    led = (GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_2)) >> 2;
    setADC();
    setPWM();
    setTimer();
    setSSI();
    setCAN();
    IntMasterEnable();
    while(1) {

    }
}

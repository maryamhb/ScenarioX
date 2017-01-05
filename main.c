/*
 * -------------------------------------------
 *    MSP432 DriverLib - v3_10_00_09
 * -------------------------------------------
 *
 * Scenario X: real-time descrambler
 * Maryam H. Group 7
 */

/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#define PTS 7 // # of points in 1 cycle of a sine wave (50kHz / 7 = 7.14 kHz)
#define PI 3.14159

static long temp_ADC = 0;
static volatile float sine[50];
/*-------------------------------------------Filter Implementation---------------------------------------------*/

// 8kHz Bandstop Chebyshev Filter Coefficients
static float bandstop_g = 0.9873701369842;
static float bandstop_num[3] = {0.9873701369842, -1.058453023, 0.9873701369842};
static float bandstop_den[3] = {1, -1.058453022724, 0.9747402739684};

float bandstop_buf[3] = {0,0,0};

void update_buffer(float *w){
    
    w[0] = w[1];              //First buffer value replaced by second value
    w[1] = w[2];              //Second buffer value replaced by third value
    
}

float filter(float x, float *w, float *b, float *a){
    
    w[2] = x - (a[1] * w[1]) - (a[2] * w[0]);                     //Last value of bandstop_buf is changed to (input) - (2nd denominator * 2nd of bandstop_buf value) - (3rd denominator * 1st bandstop_buf value)
    float y = (b[0] * w[2]) + (b[1] * w[1]) + (b[2] * w[0]);      //Output is equal to (first numerator * last bandstop_buf) + (second numerator * second bandstop_buf) + (third numerator * first bandstop_buf)
    
    return y;
    
}

float bandstop(float input){
    
    update_buffer(bandstop_buf); //Update the first and second values of the buffer
    float output = filter(input, bandstop_buf, bandstop_num, bandstop_den);   //Update the third value of the buffer and acquire final output
    
    return output * bandstop_g;  //Return output multiplied by buffer gain
    
}

/*-------------------------------------------------------------------------------------------------*/

int main(void)
{
    /* Halting the Watchdog */
    MAP_WDT_A_holdTimer();
    
    /* Configure P5.6 and P5.7 to their analog functions to output VREF */
    P5SEL0 |= BIT6 | BIT7;
    P5SEL1 |= BIT6 | BIT7;
    
    
    REFCTL0 |= REFON;                     // Turn on reference module
    REFCTL0 |= REFOUT;                    // Output reference voltage to a pin
    
    /* Output VREF = 1.2V */
    REFCTL0 &= ~(REFVSEL_3);              // Clear existing VREF voltage level setting
    REFCTL0 |= REFVSEL_0;                 // Set VREF = 1.2V
    while (REFCTL0 & REFGENBUSY);       // Wait until the reference generation is settled
    
    
    /* Configuring pins for HFXT crystal */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_PJ,
                                                   GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    
    /* Set P6.0 as output */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P6, GPIO_PIN0);
    
    /* Set all 8 pins of P2 as output */
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0 | GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3 | GPIO_PIN4 | GPIO_PIN5 | GPIO_PIN6 | GPIO_PIN7);
    
    
    /* Configuring GPIOs (4.3 MCLK) */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P4, GPIO_PIN3,
                                                    GPIO_PRIMARY_MODULE_FUNCTION);
    
    /* Setting the external clock frequency. This API is optional, but will
     * come in handy if the user ever wants to use the getMCLK/getACLK/etc
     * functions
     */
    CS_setExternalClockSourceFrequency(32000,48000000);
    
    
    /* Starting HFXT in non-bypass mode without a timeout. Before we start
     * we have to change VCORE to 1 to support the 48MHz frequency */
    MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);
    MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
    MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);
    CS_startHFXT(false);
    
    /* Initializing MCLK to HFXT (effectively 48MHz) */
    MAP_CS_initClockSignal(CS_MCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
    
    /* Configuring SysTick to trigger at 50kHz (MCLK is 48MHz so this will
     * make it toggle every 0.5s) */
    MAP_SysTick_enableModule();
    MAP_SysTick_setPeriod(960);
    MAP_Interrupt_enableSleepOnIsrExit();
    MAP_SysTick_enableInterrupt();
    
    /* Initializing ADC (MCLK/1//1) */
    MAP_ADC14_enableModule();
    MAP_ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1,
                         0);
    
    /* Configuring GPIOs (P5.0 (A5) as the ADC input pin) */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN0,
                                                   GPIO_TERTIARY_MODULE_FUNCTION);
    
    /* Configure ADC Resolution */
    ADC14_setResolution(ADC_10BIT);
    
    
    /* Configuring ADC Memory */
    MAP_ADC14_configureSingleSampleMode(ADC_MEM0, true);
    MAP_ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_INTBUF_VREFNEG_VSS,
                                        ADC_INPUT_A5, false);
    
    
    /* Configuring Sample Timer */
    MAP_ADC14_enableSampleTimer(ADC_MANUAL_ITERATION);
    
    /* Enabling MASTER interrupts */
    MAP_Interrupt_enableMaster();
    
     //create the 7kHz sine function at FS = 50kHz
    int s = 0;
    for (s=0; s<50; s++){
        
        float x = (2.0f * PI * (s * 7.0f)) / 50.0f;
        float k = sin(x);
        sine[s] = k;            //50 sine[s] values varies between -1 and 1
        
    }
    
    while (1)
    {
        MAP_PCM_gotoLPM0();
    }
}

int j = 0;

void SysTick_Handler(void)
{
    //AP_GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
    
    P6OUT |= BIT0; // set P6 PIN0 High
    
    static long temp_ADC = 0;
    
    temp_ADC = ADC14->MEM[0];  //Get the conversion result.  Alternatively, you can use temp_ADC = ADC14_getResult(ADC_MEM0)
    float input = temp_ADC / 4;  //We do this because the ADC is set to use 10 bits but P2OUT is only 8 bits.
    
    /* Enabling/Toggling Conversion */
    MAP_ADC14_enableConversion();
    MAP_ADC14_toggleConversionTrigger();
    MAP_ADC14_toggleConversionTrigger();
    
    float bandstopped = bandstop(input); //remove 8kHz frequency
    
    float output = input; * sine[j]; //modulated signal
    
    
    P2OUT = output;
    
    j++;
    if (j > 49){ //7 sine waves => 50 samples
        j = 0;
    }
    
    P6OUT &= ~BIT0; // set P6 PIN0 Low
}


//ADC Interrupt Handler. Called whenever there is a conversion that is finished for ADC_MEM0
void adc_isr(void)
{
    
    uint64_t status = MAP_ADC14_getEnabledInterruptStatus();
    MAP_ADC14_clearInterruptFlag(status);
    
    if(status & ADC_INT0){
        temp_ADC = MAP_ADC14_getResult(ADC_MEM0);
    }
}



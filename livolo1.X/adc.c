#include <xc.h>
#include <stdint.h>
#include "adc.h"


/*
 * Public vars
 */
uint8_t adc_power;

/*
 * Public functions
 */

void
adc_preinit(void)
{
    ADCON0 = 0b00001101;
    // ADFM    0------- Left justified
    // VCFG    -0------ Voltage reference = VDD
    // CHS     --0011-- Channel select = AN3
    // GO      ------0- Do not go just yet
    // ADON    -------1 ADC is enabled

    ADCON1 = 0b00010000;
    // ADCS    -001---- FOSC/8 (101 = FOSC/16)
    // unimpl  x---xxxx
}

uint8_t
adc_read_power(void)
{
    GO = 1;
    while (! GO_nDONE) ;
    adc_power = ADRESH;
    return adc_power;
}

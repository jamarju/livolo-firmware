#ifndef ADC_H
#define	ADC_H

#ifdef	__cplusplus
extern "C" {
#endif

uint8_t adc_power;

void adc_preinit(void);
uint8_t adc_read_power(void);


#ifdef	__cplusplus
}
#endif

#endif	/* ADC_H */


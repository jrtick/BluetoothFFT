#ifndef __ADC_LIB_H_
#define __ADC_LIB_H_

int initADC(void);

float readADC(void);
float readADCavg(const int samples);
#endif  //__ADC_LIB_H_

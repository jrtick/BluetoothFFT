#include <wiringPiSPI.h>
#include <stdio.h>

// ADC defines
#define KHZ 1000
#define SPI_CHANNEL 0
#define SPI_CLOCK (3200*KHZ) //(1200*KHZ)
#define ADC_RESOLUTION 10 // bits
#define V_REF 3.3 // volts
#define LOGIC_HIGH 1
#define SINGLE_MODE 1
#define MSB_FIRST 0

int initADC(){
  // The ADC communicates using SPI
  return wiringPiSPISetup(SPI_CHANNEL, SPI_CLOCK);
}

float readADC_internal() {
  unsigned char buf[2] = {( LOGIC_HIGH<<7) | (SINGLE_MODE<<6) |
	                  (SPI_CHANNEL<<5) | (  MSB_FIRST<<4), 0};
  // this fn "atomically" sends message and puts response into buf
  int count = wiringPiSPIDataRW(SPI_CHANNEL, buf, 2);
  if(count != 2) {
    printf("ERROR: ADC READ FAILED (%d).\n", count);
    return -2*V_REF; // just so it's clearly wrong
  } else {
    const int value = ((buf[0]<<8) | buf[1])>>1;
    // convert raw ADC value to voltage based on Vref
    return (V_REF*1.f*value) / (1<<ADC_RESOLUTION);
  }
}

float readADC() {
  return readADC_internal();
}

float readADCavg(const int samples) {
  float val = 0;
  for(int i=0;i<samples;i++) {
    val += readADC_internal();
  }
  return val/samples;
}

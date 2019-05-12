#include "external/kissfft/tools/kiss_fftr.h"
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <wiringPi.h>
#include "adc_lib.h"
#include <math.h>
#include "bluetooth.h"

#include <pthread.h>
#include <unistd.h>
#include <pigpio.h>

// max sample rate 43khz
#define SAMPLES_PER_FFT 1024
#define LPF 200 // hz

int main(int argc, char* argv[]) {
  // init GPIO
  if(wiringPiSetup()<0) {
    printf("failed to initialize wiring pi\n");
    return -1;
  }
  if(gpioInitialise() < 0) return -1;
  initADC();

  // send pwm to speaker
  int freq=4000, duty=250000;
  if(argc > 1) freq = atoi(argv[1]);
  if(argc > 2) duty = (int)(atof(argv[2])*1000000);
  gpioHardwarePWM(18, freq, duty);

  // init FFT
  kiss_fftr_cfg cfg = kiss_fftr_alloc(SAMPLES_PER_FFT, 0, NULL, NULL);
  kiss_fft_scalar* vals = (kiss_fft_scalar*) malloc(sizeof(kiss_fft_scalar)*SAMPLES_PER_FFT);
  kiss_fft_cpx* freqs = (kiss_fft_cpx*) malloc(sizeof(kiss_fft_cpx)*(SAMPLES_PER_FFT/2+1));
  if(cfg == NULL) {
    printf("Something wrong with KissFFT.\n");
    return -1;
  }
  if(vals == NULL || freqs == NULL) {
    printf("ran out of memory.\n");
    kiss_fftr_free(cfg);
    if(vals) free(vals);
    if(freqs) free(freqs);
    return -1;
  }

  // init bluetooth
  SetupConnection();

  printf("Connected! Streaming data...\n");
  while(true) {
    const unsigned fullstart = micros();

    // get time series
    unsigned start = micros();
    for(int i=0;i<SAMPLES_PER_FFT;i++){ vals[i] = readADCavg(2); } // limit to ~20000hz sampling
    unsigned stop = micros();
    const float samplerate = (SAMPLES_PER_FFT*1e6*1.f)/(stop-start);
    //printf("Sample rate: %.3f\n", samplerate);

    // convert to freq series
    kiss_fftr(cfg, vals, freqs);
    const float freq_inc = samplerate/SAMPLES_PER_FFT;

    // send info to android
    printf("Sending...");fflush(stdout);
    static const int BUF_LEN = 16384;
    char buf[BUF_LEN];
    int len = snprintf(buf, BUF_LEN, "%.4f,", freq_inc);
    int curmax = 0;
    int curmaxidx = -1;
    for(int i=0;i<SAMPLES_PER_FFT/2+1;i++) {
        const float mag = sqrt(pow(freqs[i].r,2)+pow(freqs[i].i,2));

        if(i>ceil(LPF/freq_inc)) {
          if(mag > curmax) {
            curmax = mag;
            curmaxidx = i;
          }
          len += snprintf(buf+len, BUF_LEN-len, "%.2f,", mag);
        } else {
          len += snprintf(buf+len, BUF_LEN-len, "%.2f,", mag);
        }
    }
    //overwrite last comma to be a newline instead
    buf[len-1] = '\n';
    SendMessage(buf, len);
    delay(50); // slow things down a little
    const unsigned fullstop = micros();
    printf("total speed: %.3f (msglen=%d)\t Max freq: %.3f (+/- %.3f)\n", 1e6/(fullstop-fullstart), len,
           (curmaxidx+0.5)*freq_inc, 0.5*freq_inc);
  }

  kiss_fftr_free(cfg);
  CloseConnection();
  return 0;
}

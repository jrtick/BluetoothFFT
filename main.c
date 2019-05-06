#include "external/kissfft/tools/kiss_fftr.h"
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <wiringPi.h>
#include "adc_lib.h" //micros(), delay()
#include <math.h>
#include "bluetooth.h"

// max sample rate 43khz
#define SAMPLES_PER_FFT 4096
#define LPF 200 // hz

int main() {
  if(wiringPiSetup()<0) {
    printf("failed to initialize wiring pi\n");
    return -1;
  }
  initADC();

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

  Connection connection = SetupConnection();

  char user_input[8] = {0};
  while(true) {
    printf("press enter to read value. 'q' to quit.\n");
    while(!fgets(&user_input[0],8, stdin));
    if(user_input[0] == 'q') break;

    //printf("getting vals...\n");
    unsigned start = micros();
    for(int i=0;i<SAMPLES_PER_FFT;i++){ vals[i] = readADC(); }//delayMicroseconds(490); } // limit to ~2000hz sampling
    unsigned stop = micros();
    float samplerate = (SAMPLES_PER_FFT*1e6*1.f)/(stop-start);
    //printf("Sample rate: %.3f\n", samplerate);

    kiss_fftr(cfg, vals, freqs);

    // now find strongest frequency
    float freq_inc = samplerate/SAMPLES_PER_FFT;
    /*float curmax = 0;
    float curmaxIdx = -1;
    for(int i=ceil(LPF/freq_inc);i<SAMPLES_PER_FFT/2+1;i++) {
      float mag = sqrt(pow(freqs[i].r,2)+pow(freqs[i].i,2));

      if(mag > curmax) {
        curmaxIdx = i;
        curmax = mag;
      }

      //printf("%.3f\t%.3f\n", (i+0.5)*freq_inc,mag);
    }
    printf("Detected Freq: %.3f (+/- %.3f)\n", (curmaxIdx+0.5)*freq_inc, freq_inc*0.5);*/

    static const BUF_LEN = 16384;
    char buf[BUF_LEN];
    int len = snprintf(buf, BUF_LEN, "%.3f,", freq_inc);
    for(int i=0;i<SAMPLES_PER_FFT/2+1;i++) {
        const float mag = sqrt(pow(freqs[i].r,2)+pow(freqs[i].i,2));
        len += snprintf(buf+len, BUF_LEN-len, "%.3f,", mag);
    }
    len += snprintf(buf+len, BUF_LEN-len, "\n");
    SendMessage(connection, buf, len);
  }

  kiss_fftr_free(cfg);
  CloseConnection(connection);
  return 0;
}


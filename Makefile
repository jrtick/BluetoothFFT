all: mic

kissfft=external/kissfft/tools/kiss_fftr.c external/kissfft/kiss_fft.c

mic: main.c Makefile adc_lib.* bluetooth.*
	gcc -o mic main.c adc_lib.c $(kissfft) bluetooth.c -lwiringPi -lrt -lpthread -lm -lbluetooth

clean:
	rm mic

all: mic roll

kissfft=external/kissfft/tools/kiss_fftr.c external/kissfft/kiss_fft.c

mic: main.c Makefile adc_lib.* bluetooth.*
	gcc -o mic main.c adc_lib.c $(kissfft) bluetooth.c -lwiringPi -lrt -lpthread -lm -lbluetooth -lpigpio

roll: roll.c Makefile adc_lib.* bluetooth.*
	gcc -o roll roll.c adc_lib.c $(kissfft) bluetooth.c -lwiringPi -lrt -lpthread -lm -lbluetooth -lpigpio

clean:
	rm mic roll

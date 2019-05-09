# BluetoothFFT
Raspberry Pi (Mic->ADC) sends frequency info over bluetooth to android app so phone can process.

I use [KissFFT](https://github.com/mborgerding/kissfft) to get the frequencies.

Apart from pairing RPi with Android phone over bluetooth, you also need to run "sudo sdptool add SP" to enable Serial Port communication over bluetooth each time your pi inits.

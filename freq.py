import RPi.GPIO as GPIO

pin = 18

GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
GPIO.setup(pin,GPIO.OUT)

p = GPIO.PWM(pin,5000)
p.start(50)
input("press return to stop")

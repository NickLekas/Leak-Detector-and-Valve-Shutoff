# Leak Detector and Valve Shutoff
This is an ESP8266 based leak detection system that will close a valve when a leak is detected

This project was developed for the ENGR114 course taught by Peter Kazarinoff at Portland Comunity College in Portland Oregon. Everything in this repo is open for anyone to use at home or develop further.

We developed the code using the Visual Studio Code plugin PlatformIO, a powerful step up from the Arduino IDE. Becasue of this, files for both PlatformIO and the Arduio IDE are provided. If you’re using the Arduino IDE follow the instructions in the ino file. If you’re using PlatformIO, follow the instructions in the ‘Readme.txt’ that is in the ‘src’ folder for each device. PlatofrmIO is setup to download all needed libraies and the Arduino IDE will need a library installed manually which is provided in the Ardruino IDE folder.

Development used Adafruit Feather HUZZAH ESP8266 modules, an Adafruit stepper motor FeatherWing, and a 29Nm stepper motor which was not strong enough to turn the valve you are using. We would not recomend using the FeatherWing motor shield and would suggest using a DRV8825 or stronger driver, Pololu has many options and a great comparison chart

Below is the hardware setup guide. Motor mounting documentation was not created becasue the motor used was significantly underpowered for direct drive mounting. The system we designed is in the repo for you to adapt to your motor if you find it useful.

## Setup guide

### Valve Hub
![](/Images/Valve%20Hub/ValveHub.png)
Step 1: Place your ESP module into a breadboard. In this guide, an Adafruit Feather HUZZAH ESP8266.
![](/Images/Valve%20Hub/1.jpg)
Step 2: Attach your stepper motor driver. In this guide an Adafruit DC Motor + Stepper FeatherWing is used and it connects to the top of the Adafruit Feather HUZZAH ESP8266.
![](/Images/Valve%20Hub/2.jpg)
Step 3: Connect 12V power to the stepper motor driver, on this Adafruit driver, power is connected to the single 2 wire terminal block.
![](/Images/Valve%20Hub/3.jpg)
Step 4: Connect the stepper motor, make sure you know which wires belong to each coil in the motor. If you’re not sure, choose 2 of the 4 wires, hold the exposed ends together and turn the output shaft of the motor. If there is resistance, then you have the wires for 1 coil, if there is no resistance swap 1 wire out for another and test again.
![](/Images/Valve%20Hub/4.jpg)

### Leak Detector
![](/Images/Leak%20Detector/LeakDetector.png)
Step 1: Place your ESP module into a breadboard. In this guide, an Adafruit Feather HUZZAH ESP8266.
![](/Images/Leak%20Detector/1.jpg)
Step 2: The ESP8266 operates at 3.3V, but the ADC can only read from 0-1V. Because of this a voltage divider is needed. Take the 10kΩ and connect one end to the 3.3V out of the ESP to an NC (No Connection) pin on the ESP.
![](/Images/Leak%20Detector/2.jpg)
Step 3: Connect the 3.3kΩ resistor to the ground pin on the ESP and the NC pin that you connected the 10kΩ resistor to.
![](/Images/Leak%20Detector/3.jpg)
Step 4: Connect one of the jumper wires to the NC pin that the 2 resistors are connected to and the other jumper wire to the ADC pin on the ESP. These wires are used to detect water when both are touching the same puddle
![](/Images/Leak%20Detector/4.jpg)

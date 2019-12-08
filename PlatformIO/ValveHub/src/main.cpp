/*
This code is for the valve hub processor
developed by: Mauve Grinders
*/
 
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <Adafruit_MotorShield.h>
 
#include "config.h"
#include "pins.h"
 
void setWifi(); //sets up the wifi network
void handleClients(); //process data from clients and responds
void leakCheck(char* isLeak); //process received data from handleClients and turns the valve if needed
void ready(int flashes); //flashes the LED a number of times when the main code is running using a set number of flashes from the user
 
//sets up the stepper motor
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_StepperMotor *valveMotor = AFMS.getStepper(200, 1);
    
//IP setup
IPAddress APlocal_IP(192, 168, 4, 1);
IPAddress APgateway(192, 168, 4, 1);
IPAddress APsubnet(255, 255, 255, 0);
 
//local port that will be listened to
unsigned int UDPPort = 2390;
 
//enables udp communication
WiFiUDP Udp;
 
//buffer to hold incoming packet
char packetBuffer[255];
char result[10];
 
bool valveClosed = false;
 
void setup() {
  //enables serial monitor output
  Serial.begin(115200);
 
  //sets the LED pin to an output and turns off the LED
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
 
  // Setting Up A Wifi Access Point
  setWifi();
 
  //enables the stepper motor
  AFMS.begin();
  //sets the stepper motor to 20rpm
  valveMotor->setSpeed(20);
 
  //flashes the LED 3 times once the main program is running
  ready(3);
}
 
void loop() {
 
  handleClients();
}
 
//processes the data received from a client and responds with the current uptime
void handleClients(){
  unsigned long tNow;
  
  //stores the size of the received packet
  int packetSize = Udp.parsePacket();
 
  //if a packet is received, process it.
  if (packetSize) {
    //prints the size of the packet to the user
    Serial.print("Received packet of size ");
    Serial.println(packetSize);
    
    //prints which the device sent the data
    Serial.print("From ");
    Serial.print(Udp.remoteIP());
    Serial.print(", port ");
    Serial.println(Udp.remotePort());
 
    //reads the packet into the packet Buffer
    int len = Udp.read(packetBuffer, 255);
    if (len > 0) {
      packetBuffer[len] = 0;
    }
 
    //prints the contents of the packet
    Serial.println("Contents:");
    Serial.println(packetBuffer);
    
    //searches the packet for a '1' indicating there is a leak
    char* isLeak = strchr(packetBuffer, '1');
 
    leakCheck(isLeak);
    
    //gets the current uptime of the controller
    tNow=millis();
    //converts a number into ASCII so it can be sent over UDP
    dtostrf(tNow, 8, 0, result);
    
    //sends the time to the client device
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(result);
    Udp.endPacket();
  }
}
 
void leakCheck(char* isLeak) {
  //if a '1' if found in the packet turn on the LED and turn the motor
    if(isLeak != NULL) {
      //turn on the LED
      digitalWrite(led, 0);
      //turn the motor if it has not yet been moved
      //this is so the motor does not try to turn the vale off again and damage the valve or itself
      if(valveClosed != true) {
        valveMotor->step(50, FORWARD, MICROSTEP);
        valveClosed = true;
      }
    }
    else {
      //if there is no leak detected, turn off the LED
      digitalWrite(led, 1);
    }
}
 
//sets up the wifi network
void setWifi(){
  //stop active previous WIFI, if there is
  //this is mainly a safety check
  WiFi.disconnect();
 
  //sets the WiFi mode
  WiFi.mode(WIFI_AP_STA);
  Serial.println("WIFI Mode : AccessPoint Station");
 
  //starts the AccessPoint
  WiFi.softAPConfig(APlocal_IP, APgateway, APsubnet); //softAPConfig (local_ip, gateway, subnet)
  WiFi.softAP(SSID, PASSWORD, 1 ,0, MAXSC); //WiFi.softAP(ssid, password, channel, hidden, max_connection)                         
  Serial.println("WIFI < " + String(SSID) + " > ... Started");
 
  //waits a bit to make sure things start properly
  delay(50);
 
  //gets the server IP
  IPAddress IP = WiFi.softAPIP();
   
  //prints the server IP Address
  Serial.print("AccessPoint IP : ");
  Serial.println(IP);
 
  //starts the UDP Server
  Udp.begin(UDPPort);
  
  //tells the user the server has started
  Serial.println("Server Started");
}
 
//flashes the LED a number of times when the main code is running using a set number of flashes from the user
void ready(int flashes) {
  int i;
 
  //loops double the times the user enters to create the number of flashes the user has set
  for(i = 0; i < flashes * 2; i++) {
    //if the current cycle is divisible by 2, turn the LED on, if not, turn the LED off
    if(i % 2 == 0) {
      digitalWrite(led, 0);
    }
    else {
      digitalWrite(led, 1);
    }
    //wait 300ms so the LED flashes visibly
    delay(300);
  }
}

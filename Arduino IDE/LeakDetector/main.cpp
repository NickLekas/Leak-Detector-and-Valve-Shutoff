/*
This code is for the leak detector processor
developed by: Mauve Grinders
*/
 
#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
 
#include "config.h"
#include "pins.h"
 
void leakDetect(); //determines if there is a leak based on the voltage to the analog input
void checkWifiAndConnect(); //checks for the specified wifi network and connects to it
void sendDataToServer(); //tells the hub if there is or is not a leak detected
void ready(int flashes); //flashes the LED a number of times when the main code is running using a set number of flashes from the user
 
//wifi Module Role & Port    
IPAddress APlocal_IP(192, 168, 4, 1);
IPAddress APgateway(192, 168, 4, 1);
IPAddress APsubnet(255, 255, 255, 0);
 
//local port to send and listen to  
unsigned int UDPPort = 2390;      
 
//enables UDP
WiFiUDP Udp;
 
//buffer big enough for 7-character float
char result[16]; 
//buffer for incoming data
char packetBuffer[255];       
 
//leak detection cycle count
int count = 0;
//leak detection flag
bool isLeak = false;
 
void setup() {
  //sets up the serial communication
  Serial.begin(115200);
 
  //enables the LED and turns it off
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  
  //establishes a wifi connection with the hub
  checkWifiAndConnect();
 
  //flashes the LED to indicate the device is ready
  ready(3);
}
 
void loop() {
  //checks for a leak and sends data to the hub
  sendDataToServer();
 
  //verifies wifi connection and will attempt to reconnect if lost
  if (WiFi.status() != WL_CONNECTED) {
    //flashes the LED and then holds it on 
    ready(3);
    digitalWrite(led, 0);
 
    //attempts to reconnect to the hub
    checkWifiAndConnect();
 
    //flashes the led to indicate the connection to been reestablished
    ready(3);
  }
}
 
//tells the hub if there is or is not a leak detected
void sendDataToServer() {
  unsigned long tNow;
 
  //checks for a leak
  leakDetect();
 
  //converts a number into ASCII so it can be sent over UDP
  dtostrf(isLeak, 8, 0, result);  
 
  //establishes what port and ip address to send the leak data, sends is, then closes the connection
  Udp.beginPacket(APlocal_IP, UDPPort);              
  Udp.write(result);
  Udp.endPacket(); 
 
 
  while(1) {
    //reads the size of the incoming packet
    int packetSize = Udp.parsePacket();
 
    //if there is a packet size, there is data to be processed
    if (packetSize) {
      //prints out the size of the packet
      Serial.print("Received packet of size ");
      Serial.println(packetSize);
      //prints out which ip sent the packet and on what port
      Serial.print("From ");
      IPAddress remoteIp = Udp.remoteIP();
      Serial.print(remoteIp);
      Serial.print(", port ");
      Serial.println(Udp.remotePort());
 
      //reads the packet into the packet Buffer
      int len = Udp.read(packetBuffer, 255);
      if (len > 0) {
        packetBuffer[len] = 0;
      }
      //prints the contents of the packet
      Serial.print("Contents:");
      Serial.println(packetBuffer);
      break;
    }
    //if there is no received data for 1000ms, a timeout message is printed
    if((millis()-tNow)>1000) { 
      Serial.println("timeout");
      break;                                                   
    }
  }
}
 
//determines if there is a leak based on the voltage to the analog input
void leakDetect() {
  //rads the analog input
  int val = analogRead(leakSense);
 
  //if the analog input is greater than 800 there is a leak
  //a leak condition will not trigger until 100 consecutive loops to filter false readings
  //if a false reading is detected the cycle count will be reset
  if(val >= 800) {
    count++;
    if(count > 50){
      isLeak = true;
      digitalWrite(led, LOW);
    }
  }
  else {
    count = 0;
    isLeak = false;
    digitalWrite(led, HIGH);
  }
}
 
//checks for the specified wifi network and connects to it
void checkWifiAndConnect(){
  //checks for no established wifi connection
  if (WiFi.status() != WL_CONNECTED) {
    //starts searching for the specified wifi connection                                       
    WiFi.begin(SSID, PASSWORD);
    //tells the user that a connection is attempting to be established
    Serial.println();
    Serial.print("Wait for WiFi");
 
    //prints a '.' every 0.5 seconds while waiting for a connection
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    //tells the user a connection has been made and what the ip address is
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: " + WiFi.localIP().toString());
 
    //starts the UDP connection
    Udp.begin(UDPPort);
  }
}
 
//flashes the LED a number of times when the main code is running using a set number of flashes from the user
void ready(int flashes) {
  int i;
 
  //loops double the times the user enters to create the number of flashes the user has set
  for(i = 0; i < (flashes * 2); i++) {
    //if the current cycle is divisible by 2, turn the LED off, if not, turn the LED on
    if(i % 2 == 0) {
      digitalWrite(led, 1);
    }
    else {
      digitalWrite(led, 0);
    }
    delay(300);
  }
}

#define BLYNK_PRINT Serial
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "********";
 char ssid[] = "**********";  // For WiFi
 char pass[] = "***********";  // For WiFi

const int sensorIn = A0;
int mVperAmp = 66; // use 100 for 20A Module and 66 for 30A Module
double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;
double total_price= 0;


BlynkTimer timer;

WidgetTerminal terminal(V1);

void setup() {
  Serial.begin(9600);  // USB Link
  
  Blynk.begin(auth, ssid, pass);
  timer.setInterval(2000L, SensorRead);
}



void loop() {
  Blynk.run();
  timer.run();
}



void SensorRead() {
  Voltage = getVPP();
  VRMS = (Voltage / 2.0) * 0.707;
  AmpsRMS = ((VRMS * 1000 / mVperAmp)-0.1);
  double watt= (AmpsRMS*220*0.9);            
  double price= watt*0.79/3600000;
  delay(1000);
  
  total_price=total_price+price;
  Blynk.virtualWrite(V1,AmpsRMS);
  Blynk.virtualWrite(V0,watt);  // To Gauge Widget @ .9pf and 110vac
  Blynk.virtualWrite(V2,total_price*10000);
  
  
  terminal.print(AmpsRMS);  // To Terminal Widget
  terminal.print(" Akim - ");
  terminal.flush();
}



float getVPP() {
  float result;
  int readValue;  //value read from the sensor
  int maxValue = 0;  // store max value here
  int minValue = 1023;  // store min value here
  uint32_t start_time = millis();
  while ((millis() - start_time) < 1500) {  // sample for 1 Sec
    readValue = analogRead(sensorIn);  // see if you have a new maxValue
    if (readValue > maxValue) {
      maxValue = readValue;  // record the maximum sensor value
    }
    if (readValue < minValue) {
      minValue = readValue;  // record the minimum sensor value
    }
  }
  // Subtract min from max
  result = ((maxValue - minValue) * 5.0) / 1023.0;
  
  return result;
}



void ac_read() {
  int rVal = 0;
  int sampleDuration = 100;  // 100ms
  int sampleCount = 0;
  unsigned long rSquaredSum = 0;
  int rZero = 511;   // For illustrative purposes only - should be measured to calibrate sensor.
  uint32_t startTime = millis();  // take samples for 100ms
  while ((millis() - startTime) < sampleDuration) {
    rVal = analogRead(A0) - rZero;
    rSquaredSum += rVal * rVal;
    sampleCount++;
  }
  double voltRMS = 5.0 * sqrt(rSquaredSum / sampleCount) / 1024.0;
  // x 1000 to convert volts to millivolts
  // divide by the number of millivolts per amp to determine amps measured
  // the 20A module 100 mv/A (so in this case ampsRMS = 10 * voltRMS
  double ampsRMS = voltRMS * 6.6;
}

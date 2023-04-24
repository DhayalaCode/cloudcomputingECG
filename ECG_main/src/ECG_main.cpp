/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "/Users/jansiranibalasubramanian/final_ECG/ECG_main/src/ECG_main.ino"
// This #include statement was automatically added by the Particle IDE.
#include <Grove_LCD_RGB_Backlight.h>

#include <Wire.h>
#include <string>
#include "Grove_LCD_RGB_Backlight/Grove_LCD_RGB_Backlight.h"
void setup();
void loop();
void calculate_bpm();
int reading_input_signal();
void updating_display(int inputSignal, int BPM);
#line 7 "/Users/jansiranibalasubramanian/final_ECG/ECG_main/src/ECG_main.ino"
rgb_lcd lcd;

int xPos = 1;
float height_old = 0;
float height_new = 0;
float inByte = 0;
float inputSignal = 0;//
int BPM = 0;
int beat_old = 0;
float beats[500];
int beatIndex;
float threshold = 620.0;// since the signals were such large numbers 
bool belowThreshold = true;
int sensorPin = A0;
unsigned long lastTime = 0;
unsigned long displayDelay = 128;
int nextTime = millis() + 15000;
bool prevSwitch = FALSE;


void setup() {
  //getting data from the photon and starting it.
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.setRGB(122, 0, 0);
  pinMode(D4, INPUT);
  pinMode(D2, INPUT);
  pinMode(A0, INPUT);
  pinMode(D5, INPUT_PULLDOWN);


  //NP
  //lcd statements for the display
 lcd.setCursor(0,0);
 lcd.print("Powering on...");
 lcd.setRGB(122, 0, 0);
 delay(1000);
 lcd.setRGB(0,122,0);
 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("Heart Rate: ");
 lcd.setCursor(0,1);
 lcd.print("signal: ");

 
}


void loop() {
    
 if (digitalRead(D5)==0 && prevSwitch == TRUE){
  lcd.setRGB(122, 122, 122);
  prevSwitch = FALSE;
 }
 else if (digitalRead(D5) == 1 && prevSwitch == FALSE){
  lcd.setRGB(0, 122, 0);
  prevSwitch = TRUE;
 }

  if ((digitalRead(D4) == 1) || (digitalRead(D2) == 1)) { // if the leads are taking in input but one of them is faulty, data is not being read properly
    Serial.println("Data not being read properly");
    lcd.setCursor(0,1);
    //lcd.print("Data not read");
  }
  else {
    Serial.println(analogRead(A0)); // or else read the data (bpm)
  }

  //lcd.print("Powering on..."); //NP lcd turn on
  delay(125);

  //for graphing
  inByte = analogRead(sensorPin);  //read the A0 pin 
  //inByte = map((int)inByte, 0, 4095, 0, 400); // lets try to graph the data

  //Particle.publish("graph_data", String(height_new), PRIVATE); //graph variable (for future)
  //Particle.publish("BPM_data", String(inByte), PRIVATE); //bpm variable.
  //Particle.publish("BPM",String(BPM), PRIVATE);


//still for graphing
/*
  if (height_old != 0) {
    Particle.publish("line_data", String(xPos - 1) + "," + String(height_old) + "," + String(xPos) + "," + String(height_new), PRIVATE);
  }
  height_old = height_new;
  if (xPos >= 1000) {
    xPos = 0;
    Particle.publish("background_data", "clear", PRIVATE);
  }
  else {
    xPos++;
  }

*/


  if (millis() - lastTime >= displayDelay) { //calculate the bpm calculation at a regular interval
    lastTime = millis();
    calculate_bpm();
  }


 
 
  //dataBPM = "test";
  //Drew code: cloud connectivity right here
 String dataBPM = String(BPM);
 
 
    
    // Sends a reading of your heartrate every 30s
    int currentTime = millis();
    if (nextTime < currentTime) {
        // Email Notification
        Particle.publish("heartRate", dataBPM);
        
        // Update Spreadsheet
        Particle.publish("heartBeat", dataBPM);
        nextTime = currentTime + 15000;
    }





}


void calculate_bpm(){
  inByte = reading_input_signal();

  if (inByte > threshold && belowThreshold == true) { //checking if the signals are within the correct threshold
    int beat_new = millis();
    int diff = beat_new - beat_old;
    float currentBPM = 60000 / diff;
    beats[beatIndex] = currentBPM;
    float total = 0.0;

    for (int i = 0; i < 60; i++) { 
        total += beats[i];
    }

    beatIndex = (beatIndex + 1) % 60; //average beats per minute over 500 seconds.
                                       //beats_average is the function calculating the heart rate over 500 seconds                              
    beat_old = beat_new;
    BPM = total / 60.0;
    belowThreshold = false;

  }

  else if (inByte < threshold){ //if our input signal is under the threshold, it'll wait until we detect a proper heartbeat.
    belowThreshold = true;
  }

  updating_display(inByte, BPM);  //updates on the screen 
  

}

int reading_input_signal(){
  inputSignal = analogRead(sensorPin);  //read the A0 pin 
  return inputSignal;
}

void updating_display(int inputSignal, int BPM){ 
  lcd.setCursor(9,1);//top line
  lcd.print(String(inputSignal));

  lcd.setCursor(11, 0);
  lcd.print(BPM);//bottom line

}
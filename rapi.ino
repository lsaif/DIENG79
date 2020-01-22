#include <Wire.h>
#include <DS1307RTC.h>
#include <LiquidCrystal595.h>

LiquidCrystal595 lcd(2,3,4);
#define BOOL_PIN (2)
#define DC_GAIN (8.5)

#define READ_SAMPLE_INTERVAL (10)
#define READ_SAMPLE_TIMES (20)

#define ZERO_POINT_VOLTAGE (0.324)
#define REACTION_VOLTAGE (0.020)

const int numReadings = 30;
const int pinLed= 13;
const int pinAlarm= 8;
const int pinSensor=5;
int stateAlarm;
int readings[numReadings]; 
int total=0;
int index=0;
int average=0;
int treshold=50;
int jam=0;
int menit=9;
int detik=3;
long time=0;
long previous = 0; 
long interval = 1000; 

void setup(){
  pinMode(pinLed, OUTPUT);  
  pinMode(pinSensor,INPUT);
  pinMode(pinAlarm,OUTPUT);
  pinMode(BOOL_PIN,INPUT);
  digitalWrite(BOOL_PIN,HIGH);
  digitalWrite(pinAlarm,HIGH);
  lcd.begin(16, 2);
  Serial.begin(9600);
  RTC.stop();
  RTC.set(DS1307_SEC,1);        //set the seconds
  RTC.set(DS1307_MIN,23);     //set the minutes
  RTC.set(DS1307_HR,12);       //set the hours
  RTC.set(DS1307_DOW,4);       //set the day of the week
  RTC.set(DS1307_DATE,1);       //set the date
  RTC.set(DS1307_MTH,3);        //set the month
  RTC.set(DS1307_YR,14);         //set the year
  RTC.start();
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
    readings[thisReading] = 0;
}

void loop()
{
  //baca nilai sensor
  /*total= total - readings[index];         
  readings[index] = analogRead(pinSensor); 
  total= total + readings[index];       
  index = index + 1;                    
  if (index >= numReadings)              
    index = 0;                           
  average = total/numReadings;
  */
  float volts;
  volts=MGRead(pinSensor);
  float EMF;
  EMF=-0.0543*(volts*1000-336)+529.831737;
  
  //set alarm
  if(millis() - time > interval) {
      if(EMF>treshold && previous<=treshold && stateAlarm == HIGH){
        stateAlarm = LOW; //dibalik
      } else {
         stateAlarm = HIGH; 
      }
      time = millis();
      Serial.println(EMF); 
      lcd.clear();
      lcd.setCursor(5, 1);
  lcd.print(EMF);
  lcd.setCursor(10, 1);
  lcd.print("ppm");
  lcd.setCursor(0,0);
  lcd.print(jam);
  lcd.setCursor(2,0);
  lcd.print(":");
  lcd.setCursor(3,0);
  lcd.print(menit);
  lcd.setCursor(5,0);
  lcd.print(":");
  lcd.setCursor(6,0);
  lcd.print(detik);
  detik++;
      /*if (Serial.available()) {
        lcd.clear();
        while (Serial.available() > 0) {
          lcd.write(Serial.read());
        }
      }*/
  }
  digitalWrite(pinAlarm, stateAlarm);
  previous == average;
}

float MGRead(int mg_pin){
   int i;
   float v=0;
   
   for (i=0;i<READ_SAMPLE_TIMES;i++){
     v+=analogRead(mg_pin);
     delay(READ_SAMPLE_INTERVAL);  
   }
   v=(v/READ_SAMPLE_TIMES)*5/1024;
   return v;
 }

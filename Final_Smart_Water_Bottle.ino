#include <SPI.h>
#include <Wire.h>
#include <DS3231.h>
/*
 * A2 - pH
 * A3 - Trbdt
 * A4 - OLED-SDA
 * A5 - OLED -SCL
 * A6 - US- BROWN-ECHO
 * A7 - US- Orange-TRIG
 */

DS3231  rtc(SDA, SCL);
Time t;
int storehour, storemin,storesec;
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
/*****FOR OLED*********/
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 //4 // Reset pin # (or -1 if sharing Arduino reset pin)
/*********FOR pH*********/
#define SensorPin A2            //pH meter Analog output to Arduino Analog Input 0
#define Offset -0.1//-0.97 // 0 //          //deviation compensate
#define samplingInterval 1 //1000 //1
#define ArrayLenth  40 //400 //   //times of collection
int pHArray[ArrayLenth];   //Store the average value of the sensor feedback
int pHArrayIndex=0;
#define PH_PRECISION 1

/**** UltraSonic **************/
const unsigned int TRIG_PIN = 4; //A7; //=13;
const unsigned int ECHO_PIN = 3; //A6; //=12;
const unsigned int BAUD_RATE=9600;
void readVolume(void);

int distance;
int nVolume;
/*****Turbidity *********/
#define TURBID_IN A3

#define printInterval 2000
#define ReminderTime 15000
//#define READ_REAL_TURB 1
#define READ_REAL_VOL 1
#define READ_REAL_PH 1

//#define printInterval 2500
#define READ_REAL_TURB 0
//#define READ_REAL_VOL 0
//#define READ_REAL_PH 0

float ntu;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
void setup() {
  Serial.begin(BAUD_RATE);
  rtc.begin();
  //rtc.setTime(12,25,0);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  //Serial.println("Init started");
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  //display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
//  display.clearDisplay();
//  testfilltriangle();
//  delay(2000);
  display.clearDisplay();
 
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 15);
  display.println("Smart     ");
  display.println("  Water   ");
  display.println("    Bottle");
  display.display(); 
  delay(2000);
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println("By: Rishit");
  display.println("    Jishnu");
  display.println("    Aairah");
  display.display(); 
  delay(5000);
 //testdrawchar();
  //delay(5000);
//  display.clearDisplay();
//  display.setTextSize(2);
//  display.setCursor(0, 10);
//  display.print("pH value  : ");
//  display.setCursor(0,30);
//  display.println("Turb(NTU) :");
//  display.setCursor(0, 50);
//  display.println("Volume(ml):");
//  display.setCursor(0, 10);
//  display.print("pH  : ");
//  display.setCursor(0,30);
//  display.print("Turb: ");
//  display.setCursor(0, 50);
//  display.print("Vol.: ");
//  display.display();
//  delay(5000);
//  display.clearDisplay();
//  display.setCursor(0,10);
  //displat.println ;
}

void loop() {
  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
   static unsigned long printTime2 = millis();
  static float pHValue,voltage;
//  if(millis()-samplingTime > samplingInterval)
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.print("pH  : ");
  display.setCursor(0,30);
  display.print("Turb: ");
  display.setCursor(0, 50);
  display.print("Vol.: ");
  //display.display();
  
//  {

      #if READ_REAL_PH 
        pHArray[pHArrayIndex++]=analogRead(SensorPin);
        if(pHArrayIndex==ArrayLenth)pHArrayIndex=0;
        voltage = avergearray(pHArray, ArrayLenth)*5.0/1024;
        pHValue = 3.5*voltage+Offset;
        pHValue = 7.5*voltage - 8;
      /*#else
        pHValue = random(68, 70);
        pHValue /= 10;*/
      #endif
      readTurbidity();
      readVolume();
      samplingTime=millis();
//  }

  if(millis() - printTime > printInterval)   //Every 800 milliseconds, print a numerical
  {
        display.setTextSize(2);
        //display.setTextColor(WHITE);
        display.clearDisplay();
        display.setCursor(0, 10);
        display.print("pH  : ");
        display.setCursor(0,30);
        display.print("Turb: ");
        display.setCursor(0, 50);
        display.print("Vol.: ");
        //display.clearDisplay();
        Serial.print("Voltage:");
        Serial.println(voltage,2);
        //display.display();
        display.fillRect(70, 0, display.width(), display.height(), BLACK);
        display.setTextSize(2);
        display.setCursor(70, 10);
        if(pHValue < 0)
          display.print("O.R.");
        else
          display.print(pHValue,PH_PRECISION);
        display.setCursor(70, 30);
        if(ntu >=3000)
          display.print("O.R.");
        else
        {
          if(ntu < 100)
            display.print(ntu,1);
          else
            display.print((int)ntu);
        }
        display.setCursor(70, 50);

        if(nVolume >= 700)
          nVolume = 700;
        else if (nVolume < 0)
          nVolume = 0;
        display.print(nVolume);
        display.display();
        delay(2000);
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(0,10);
        display.println("Time:");
        display.setCursor(0,40);
        display.println("Temp:");
        t = rtc.getTime();
        display.setCursor(65,10);
        display.print(t.hour,DEC);
        display.print(":");
        
        display.print(round(t.min),DEC);
        //display.print(":");
        
        //display.print(t.sec,DEC);
        display.print("  ");
        display.setCursor(70,40);
        display.println(round(rtc.getTemp()));
        Serial.print(rtc.getTemp());
        // Wait one second before repeating
        
              
        display.display();
        delay (1000);
        display.clearDisplay();
        
        printTime=millis();
  }

if(millis() - printTime2 > ReminderTime)
{
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println("Time");
  display.println("To Drink");
  display.println("Water");
  display.display(); 
  delay(2000);
  printTime2=millis();
}

}

double avergearray(int* arr, int number){
  int i;
  int max,min;
  double avg;
  long amount=0;
  if(number<=0){
    Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if(number<5){   //less than 5, calculated directly statistics
    for(i=0;i<number;i++){
      amount+=arr[i];
    }
    avg = amount/number;
    return avg;
  }else{
    if(arr[0]<arr[1]){
      min = arr[0];max=arr[1];
    }
    else{
      min=arr[1];max=arr[0];
    }
    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;        //arr<min
        min=arr[i];
      }else {
        if(arr[i]>max){
          amount+=max;    //arr>max
          max=arr[i];
        }else{
          amount+=arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount/(number-2);
  }//if
  return avg;
}

void readVolume(void)
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
 #if READ_REAL_VOL
 const unsigned long duration= pulseIn(ECHO_PIN, HIGH);
 distance= duration/29/2;
 if(duration==0){
   Serial.println("Warning: no pulse from sensor");
   } 
  else{
      Serial.print("distance to nearest object:");
      Serial.println(distance);
      Serial.println(" cm");
  }
  //nVolume = (700 - 38.8*distance);
  nVolume = (700 - 31*distance);
  #else
  //nVolume = random(340, 345); 
  #endif
 //delay(100);
}

void readTurbidity(void)
{
  //int sensorValue; // = analogRead(TURBID_IN);// read the input on analog pin 0:
  float flAvg = 0;
  int nCnt;
  #define AVR_NTU 10
  for(nCnt = 0;nCnt< AVR_NTU;nCnt++)
  {
    flAvg += analogRead(TURBID_IN);
  }

  flAvg = flAvg/AVR_NTU;
  //flAvg = analogRead(TURBID_IN);
  float voltage = flAvg * (5.0 / 1024.0); // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):

  Serial.print(voltage); // print out the value you read:
  Serial.print("  "); // print out the value you read:
//  if(voltage < 2.5)
//    ntu = 3000;
//  else
    #if READ_REAL_TURB == 1
      ntu = 10000*voltage*voltage - 33000*voltage + 27000;
    #endif
      //ntu = -370.3704*voltage*voltage +333.3333*voltage + 1333.333;
    //ntu =  392.156*voltage*voltage - 4137*voltage + 10458.82; 
    //ntu = -1120.4*square(voltage) + 5742.3*voltage - 4353.8;
    ntu = -1120.4*voltage*voltage + 5742.3*voltage - 4353.8;
    #if READ_REAL_TURB == 0
    //ntu = random(10, 31);
    ntu = ntu/100;
    #endif
    
 if(ntu <= 0)
   ntu = 0;
 if(ntu >= 3000)
   Serial.println("O.R.");
 else
   Serial.println(ntu);
  //delay(500);
}

void RTC()
{
   t = rtc.getTime();
  Serial.print(t.hour,DEC);
  Serial.print(":");
  
  Serial.print(t.min,DEC);
  Serial.print(":");
  
  Serial.print(t.sec,DEC);
  Serial.print("  ");
  
  Serial.println(rtc.getTemp());
  // Wait one second before repeating
  delay (1000);
  
  
    storehour = t.hour;
    storemin = t.min;
    storesec = t.sec;
    Serial.println("TimeStored: ");
    Serial.print(storehour);
    Serial.print(":");
    Serial.print(storemin);
    Serial.print(":");
    Serial.print(storesec);
    Serial.println();
    delay(100);


   
   //buz=storemin+2;
   //if (t.hour==storehour && t.min== buz && t.sec==storesec)
   
     //tone(buzzer, 1000); // Send 1KHz sound signal...
       // delay(5000);        // ...for 1 sec
        //noTone(buzzer);     // Stop sound...
}

void testfilltriangle(void) {
  display.clearDisplay();

  for(int16_t i=max(display.width(),display.height())/2; i>0; i-=5) {
    // The INVERSE color is used so triangles alternate white/black
    display.fillTriangle(
      display.width()/2  , display.height()/2-i,
      display.width()/2-i, display.height()/2+i,
      display.width()/2+i, display.height()/2+i, INVERSE);
    display.display();
    delay(1);
  }
}




//#include <SPI.h>
//#include <Wire.h>
//#include <DS3231.h>
///*
// * A2 - pH
// * A3 - Trbdt
// * A4 - OLED-SDA
// * A5 - OLED -SCL
// * A6 - US- BROWN-ECHO
// * A7 - US- Orange-TRIG
// */
//
//DS3231  rtc(SDA, SCL);
//Time t;
//int storehour, storemin,storesec;
//#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h>
///*****FOR OLED*********/
//#define SCREEN_WIDTH 128 // OLED display width, in pixels
//#define SCREEN_HEIGHT 64 // OLED display height, in pixels
//
//// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
//#define OLED_RESET     -1 //4 // Reset pin # (or -1 if sharing Arduino reset pin)
///*********FOR pH*********/
//#define SensorPin A2            //pH meter Analog output to Arduino Analog Input 0
//#define Offset -0.1//-0.97 // 0 //          //deviation compensate
//#define samplingInterval 1 //1000 //1
//#define ArrayLenth  40 //400 //   //times of collection
//int pHArray[ArrayLenth];   //Store the average value of the sensor feedback
//int pHArrayIndex=0;
//#define PH_PRECISION 1
//
///**** UltraSonic **************/
//const unsigned int TRIG_PIN = 4; //A7; //=13;
//const unsigned int ECHO_PIN = 3; //A6; //=12;
//const unsigned int BAUD_RATE=9600;
//void readVolume(void);
//
//int distance;
//int nVolume;
///*****Turbidity *********/
//#define TURBID_IN A3
//
//#define printInterval 2000
//#define ReminderTime 15000
////#define READ_REAL_TURB 1
//#define READ_REAL_VOL 1
//#define READ_REAL_PH 1
//
////#define printInterval 2500
//#define READ_REAL_TURB 0
////#define READ_REAL_VOL 0
////#define READ_REAL_PH 0
//
//float ntu;
//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//void setup() {
//  Serial.begin(BAUD_RATE);
//  rtc.begin();
//  pinMode(TRIG_PIN, OUTPUT);
//  pinMode(ECHO_PIN, INPUT);
//  
//  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
//  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
//    Serial.println(F("SSD1306 allocation failed"));
//    for(;;); // Don't proceed, loop forever
//  }
//  //Serial.println("Init started");
//  // Show initial display buffer contents on the screen --
//  // the library initializes this with an Adafruit splash screen.
//  //display.display();
//  delay(2000); // Pause for 2 seconds
//
//  // Clear the buffer
//  display.clearDisplay();
//  testfilltriangle();
//  display.clearDisplay();
//  display.setTextSize(2);
//  display.setTextColor(WHITE);
//  display.setCursor(0, 15);
//  display.println("Smart     ");
//  display.println("  Water   ");
//  display.println("    Bottle");
//  display.display(); 
//  delay(2000);
//  display.clearDisplay();
//  display.setCursor(0, 10);
//  display.println("By: Rishit");
//  display.println("    Jishnu");
//  display.println("    Aairah");
//  display.display(); 
//  delay(5000);
// //testdrawchar();
//  //delay(5000);
//  display.clearDisplay();
//  //display.setTextSize(2);
////  display.setCursor(0, 10);
////  display.print("pH value  : ");
////  display.setCursor(0,30);
////  display.println("Turb(NTU) :");
////  display.setCursor(0, 50);
////  display.println("Volume(ml):");
////  display.setCursor(0, 10);
////  display.print("pH  : ");
////  display.setCursor(0,30);
////  display.print("Turb: ");
////  display.setCursor(0, 50);
////  display.print("Vol.: ");
////  display.display();
////  delay(5000);
////  display.clearDisplay();
////  display.setCursor(0,10)
////  displat.println 
//}
//
//void loop() {
//  static unsigned long samplingTime = millis();
//  static unsigned long printTime = millis();
//  static unsigned long printTime2 = millis();
//  static float pHValue,voltage;
////  if(millis()-samplingTime > samplingInterval)
//  display.setTextSize(2);
//  display.setTextColor(WHITE);
//  display.setCursor(0, 10);
//  display.print("pH  : ");
//  display.setCursor(0,30);
//  display.print("Turb: ");
//  display.setCursor(0, 50);
//  display.print("Vol.: ");
//  //display.display();
//  
////  {
//
//      #if READ_REAL_PH 
//        pHArray[pHArrayIndex++]=analogRead(SensorPin);
//        if(pHArrayIndex==ArrayLenth)pHArrayIndex=0;
//        voltage = avergearray(pHArray, ArrayLenth)*5.0/1024;
//        pHValue = 3.5*voltage+Offset;
//        //pHValue = 7.5*voltage - 8;
//      #else
//        pHValue = random(68, 70);
//        pHValue /= 10;
//      #endif
//      readTurbidity();
//      readVolume();
//      samplingTime=millis();
////  }
//
//  if(millis() - printTime > printInterval)   //Every 800 milliseconds, print a numerical
//  {
//        Serial.print("Voltage:");
//        Serial.println(voltage,2);
//        display.fillRect(70, 0, display.width(), display.height(), BLACK);
//        display.setTextSize(2);
//        display.setCursor(70, 10);
//        if(pHValue < 0)
//          display.print("O.R.");
//        else
//          display.print(pHValue,PH_PRECISION);
//        display.setCursor(70, 30);
//        if(ntu >=3000)
//          display.print("O.R.");
//        else
//        {
//          if(ntu < 100)
//            display.print(ntu,1);
//          else
//            display.print((int)ntu);
//        }
//        display.setCursor(70, 50);
//
//        if(nVolume >= 700)
//          nVolume = 700;
//        else if (nVolume < 0)
//          nVolume = 0;
//        display.print(nVolume);
//        display.display();
//        delay(5000);
//        display.clearDisplay();
//        display.setTextSize(1);
//        display.setCursor(0,10);
//        display.println("Time : ");
//        display.setCursor(0,40);
//        display.println("Temp : ");
//        t = rtc.getTime();
//        display.setCursor(50,10);
//        display.print(t.hour,DEC);
//        display.print(":");
//        
//        display.print(t.min,DEC);
//        display.print(":");
//        
//        display.print(t.sec,DEC);
//        display.print("  ");
//        display.setCursor(50,40);
//        display.println(rtc.getTemp());
//        Serial.print(rtc.getTemp());
//        // Wait one second before repeating
//        
//              
//        display.display();
//        delay (1000);
//        display.clearDisplay();
//        
//        printTime=millis();
//  }
//
//}
//
//double avergearray(int* arr, int number){
//  int i;
//  int max,min;
//  double avg;
//  long amount=0;
//  if(number<=0){
//    Serial.println("Error number for the array to avraging!/n");
//    return 0;
//  }
//  if(number<5){   //less than 5, calculated directly statistics
//    for(i=0;i<number;i++){
//      amount+=arr[i];
//    }
//    avg = amount/number;
//    return avg;
//  }else{
//    if(arr[0]<arr[1]){
//      min = arr[0];max=arr[1];
//    }
//    else{
//      min=arr[1];max=arr[0];
//    }
//    for(i=2;i<number;i++){
//      if(arr[i]<min){
//        amount+=min;        //arr<min
//        min=arr[i];
//      }else {
//        if(arr[i]>max){
//          amount+=max;    //arr>max
//          max=arr[i];
//        }else{
//          amount+=arr[i]; //min<=arr<=max
//        }
//      }//if
//    }//for
//    avg = (double)amount/(number-2);
//  }//if
//  return avg;
//}
//
//void readVolume(void)
//{
//  digitalWrite(TRIG_PIN, LOW);
//  delayMicroseconds(2);
//  digitalWrite(TRIG_PIN, HIGH);
//  delayMicroseconds(10);
//  digitalWrite(TRIG_PIN, LOW);
//  
// #if READ_REAL_VOL
// const unsigned long duration= pulseIn(ECHO_PIN, HIGH);
// distance= duration/29/2;
// if(duration==0){
//   Serial.println("Warning: no pulse from sensor");
//   } 
//  else{
//      Serial.print("distance to nearest object:");
//      Serial.println(distance);
//      Serial.println(" cm");
//  }
//  //nVolume = (700 - 38.8*distance);
//  nVolume = (700 - 31*distance);
//  #else
//  nVolume = random(340, 345); 
//  #endif
// //delay(100);
//}
//
//void readTurbidity(void)
//{
//  //int sensorValue; // = analogRead(TURBID_IN);// read the input on analog pin 0:
//  float flAvg = 0;
//  int nCnt;
//  #define AVR_NTU 10
//  for(nCnt = 0;nCnt< AVR_NTU;nCnt++)
//  {
//    flAvg += analogRead(TURBID_IN);
//  }
//
//  flAvg = flAvg/AVR_NTU;
//  //flAvg = analogRead(TURBID_IN);
//  float voltage = flAvg * (5.0 / 1024.0); // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
//
//  Serial.print(voltage); // print out the value you read:
//  Serial.print("  "); // print out the value you read:
////  if(voltage < 2.5)
////    ntu = 3000;
////  else
//    #if READ_REAL_TURB == 1
//      ntu = 10000*voltage*voltage - 33000*voltage + 27000;
//    #endif
//      //ntu = -370.3704*voltage*voltage +333.3333*voltage + 1333.333;
//    //ntu =  392.156*voltage*voltage - 4137*voltage + 10458.82; 
//    //ntu = -1120.4*square(voltage) + 5742.3*voltage - 4353.8;
//    //ntu = -1120.4*voltage*voltage + 5742.3*voltage - 4353.8;
//    #if READ_REAL_TURB == 0
//    ntu = random(10, 31);
//    ntu = ntu/100;
//    #endif
//    
// if(ntu <= 0)
//   ntu = 0;
// if(ntu >= 3000)
//   Serial.println("O.R.");
// else
//   Serial.println(ntu);
//  //delay(500);
//}
//
//void RTC()
//{
//   t = rtc.getTime();
//  Serial.print(t.hour,DEC);
//  Serial.print(":");
//  
//  Serial.print(t.min,DEC);
//  Serial.print(":");
//  
//  Serial.print(t.sec,DEC);
//  Serial.print("  ");
//  
//  Serial.println(rtc.getTemp());
//  // Wait one second before repeating
//  delay (1000);
//  
//  
//    storehour = t.hour;
//    storemin = t.min;
//    storesec = t.sec;
//    Serial.println("TimeStored: ");
//    Serial.print(storehour);
//    Serial.print(":");
//    Serial.print(storemin);
//    Serial.print(":");
//    Serial.print(storesec);
//    Serial.println();
//    delay(100);
//
//
//   
//   //buz=storemin+2;
//   //if (t.hour==storehour && t.min== buz && t.sec==storesec)
//   
//     //tone(buzzer, 1000); // Send 1KHz sound signal...
//       // delay(5000);        // ...for 1 sec
//        //noTone(buzzer);     // Stop sound...
//}
//void testfilltriangle(void) {
//  display.clearDisplay();
//
//  for(int16_t i=max(display.width(),display.height())/2; i>0; i-=5) {
//    // The INVERSE color is used so triangles alternate white/black
//    display.fillTriangle(
//      display.width()/2  , display.height()/2-i,
//      display.width()/2-i, display.height()/2+i,
//      display.width()/2+i, display.height()/2+i, INVERSE);
//    display.display();
//    delay(1);
//  }
//
//
//

/*
XMas lights program.

Read the light sensor, if dark continue
Read the temperature, record digits
Send to morsecode subroutine that blinks a pin
Connect that pin to your relay to blink the Xmas lights

  For Barometer
 
 *************************************************** 
  This is an example for the BMP085 Barometric Pressure & Temp Sensor

  Designed specifically to work with the Adafruit BMP085 Breakout 
  ----> https://www.adafruit.com/products/391

  These displays use I2C to communicate, 2 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************

Connect VCC of the BMP085 sensor to 3.3V (NOT 5.0V!)
Connect GND to Ground
Connect SCL to i2c clock - on '168\'328 Arduino Uno/Duemilanove/etc thats Analog 5
Connect SDA to i2c data - on '168\'328 Arduino Uno/Duemilanove/etc thats Analog 4
EOC is not used, it signifies an end of conversion
XCLR is a reset pin, also not used here
 
 */

#include <stdlib.h>
// Temp and Barom
#include <Wire.h>                       
#include <Adafruit_BMP085.h>
//
 int k=0;
 int i;
 int j;
 int pinout=12;          // different pin output based on barometer falling (green) or flat (red), use 11 for falling and 12 for flat (or rising)  
 //
 char PTMP[3];        // for assembling the packet of 3 integers from TMP
 // char ppp;
 String TMP[3];
 char BAROM[10];
 short BAROMETER[359];  // if loop takes 10 sec, then 360 loops is one hour, used to compare barom values
 short Deltab;          // if this drops more than 1 over 360 loops (one hour) then rain and green color    
 char LUM[4];
 char PLUM[3];       // for assembling the packet of 3 integers from LUM
 char color;
// 
//
// Items needed for barometer and temperature
//
 Adafruit_BMP085 bmp;   // For Temp and Barom 
 int t;   // temp in F
 float b;   // barometric pressure
 //
 // Items needed for photocell 
 //
 int photocellPin = 1;     // the cell and 10K pulldown are connected to a1
 int photocellReading;     // the analog reading from the sensor divider
 int wpm = 150; // sets speed of morse code 
   
void setup(){
  //
  Serial.begin(9600);                                                         
  // initialize I2C with temp and barom
  bmp.begin();   
  //
  // Morse code stuff:
  //
  // initialize the digital pin as an output.
  // Pin 13 has an LED connected on most Arduino boards:
  pinMode(13, OUTPUT); 
  pinMode(12, OUTPUT); // Connect to relay pin 6 Orange which is outlet 12  This is the usual blinker red lights
  pinMode(11, OUTPUT); // Connect to relay pin 4 Yellow which is outlet 11  This is the barom falling blinker green lights
}

void loop() {
  digitalWrite(13, LOW);
  digitalWrite(3, LOW);
  digitalWrite(11, LOW);
  digitalWrite(12, LOW);
// 
// Do the photocell *******************************************************************************************************************
// Note this functionality is not yet used
//
  photocellReading = analogRead(photocellPin);  // reads an integer off the pin that could be 1, 2 or 3 digits
  if (photocellReading > 99) 
     {
     String LUM = String(photocellReading);
//     lcd.print("Lumino>99 ");
//     lcd.print(LUM);
//     lcd.print(" ");
//     lcd.print(LUM[2]);
     PLUM[0] = LUM[0];
     PLUM[1] = LUM[1];
     PLUM[2] = LUM[2];
     }
    else if (photocellReading < 10)
     {
     String LUM = String(photocellReading);
     PLUM[0] = '0';
     PLUM[1] = '0';
     PLUM[2] = LUM[0];
     }
     else
     {
     String LUM = String(photocellReading);
     PLUM[0] = '0';
     PLUM[1] = LUM[0];
     PLUM[2] = LUM[1];
     }
  //
  delay(1000);
  //
  // Do the temp and barometer reading *******************************************************************************************************************
  //
  t = int(bmp.readTemperature()*9.0/5.0+32.0);
  //  dtostrf(t,3, 0, TMP); // float to string in stdlib is dtostrf(FLOAT,WIDTH,PRECSISION,BUFFER);
    if (t > 99) 
     {
     String TMP = String(t);
     PTMP[0] = TMP[0];
     PTMP[1] = TMP[1];
     PTMP[2] = TMP[2];
     translator(PTMP[0]);
     translator(PTMP[1]);
     translator(PTMP[2]);
     delay(20*wpm);

     }
    else if (t < 10)
     {
     String TMP = String(t);
     PTMP[0] = '/0';
     PTMP[1] = '/0';
     PTMP[2] = TMP[1];
     translator(PTMP[2]);
     delay(20*wpm);
     }
    else 
     {
     String TMP = String(t);
     PTMP[0] = '/0';
     PTMP[1] = TMP[0];
     PTMP[2] = TMP[1];
     translator(PTMP[1]);
     translator(PTMP[2]);
     delay(20*wpm);
     }
  //
  // divide Pa by 100 to bet hPa which is also mb then divide by adjustment factor
  // lcd.print((bmp.readPressure()/100.0)-0);
  // lcd.println(" mb ");
  b = bmp.readPressure()/100.0-0;
  //
  // This Deltab method doesn't appear to work, I saw a > 1 mb pressure change and it didn't change the lights.  So need to test this some more
  //
  if (i > 359) {
    Deltab=BAROMETER[0]-b ;
    i=0;
  }
  BAROMETER[i]=b;
  i++;
  Serial.println(i);
  if (Deltab>1) {
    color='green';
    pinout = 11;
  }
  else
  {
    color='red';
    pinout = 12;   
  } 
  dtostrf(b,4, 1, BAROM); // float to string in stdlib is dtostrf(FLOAT,WIDTH,PRECSISION,BUFFER);
   //  lcd.print(BAROM);
   /// lcd.println(" mb ");
  delay(1000);
  //
  // Assemble Packet  *******************************************************************************************************************
  //
//  Serial.println("f");
//      Serial.print(PTMP[0]);
//      Serial.print(PTMP[1]);
//    Serial.println(PTMP[2]);
//          Serial.print(PLUM[0]);
//      Serial.print(PLUM[1]);
//    Serial.println(PLUM[2]);    
    
// ptmp(wpm);
// barometer(wpm);
// WS(wpm);

}

void translator (char ppp) {
  // Serial.print("p=");    
  // Serial.println(ppp);
  // p = '9';
  // Serial.println(p);
  // ppp = 8;
  //
  if (ppp == '0') 
  {
  Nzero(wpm); 
  } 
  else if (ppp == '1') 
  {
  N1(wpm); 
  } 
  else if (ppp == '2') 
  {
  N2(wpm); 
  } 
  else if (ppp == '3') 
  {
  N3(wpm); 
  } 
  else if (ppp == '4') 
  {
  N4(wpm); 
  } 
  else if (ppp == '5') 
  {
  N5(wpm); 
  } 
  else if (ppp == '6') 
  {
  N6(wpm); 
  } 
  else if (ppp == '7') 
  {
  N7(wpm); 
  } 
  else if (ppp == '8') 
  {
  N8(wpm); 
  } 
  else if (ppp == '9') 
  {
  N9(wpm); 
  } 

// R(wpm);
// R(wpm);
}


void ptmp (int wpm) {
H(wpm);
A(wpm);
P(wpm);
P(wpm);
Y(wpm);
}
void barometer (int wpm) {
N(wpm);
E(wpm);
W(wpm);
}
void WS (int wpm) {
delay(7*wpm);
}
void A (int wpm) {
dit(wpm);
dah(wpm);
delay(3*wpm);
}
void B (int wpm) {
dah(wpm);
dit(wpm);
dit(wpm);
dit(wpm);
delay(3*wpm);
}
void C (int wpm) {
dah(wpm);
dit(wpm);
dah(wpm);
dit(wpm);
delay(3*wpm);
}
void D (int wpm) {
dah(wpm);
dit(wpm);
dit(wpm);
delay(3*wpm);
}
void E (int wpm) {
dit(wpm);
delay(3*wpm);
}
void F1 (int wpm) {
dit(wpm);
dit(wpm);
dah(wpm);
dit(wpm);
delay(3*wpm);
}

void G (int wpm) {
dah(wpm);
dah(wpm);
dit(wpm);
delay(3*wpm);
}
void H (int wpm) {
dit(wpm);
dit(wpm);
dit(wpm);
dit(wpm);
delay(3*wpm);
}
void I (int wpm) {
dit(wpm);
dit(wpm);
delay(3*wpm);
}
void J (int wpm) {
dit(wpm);
dah(wpm);
dah(wpm);
dah(wpm);
delay(3*wpm);
}
void K (int wpm) {
dah(wpm);
dit(wpm);
dah(wpm);
delay(3*wpm);
}
void L (int wpm) {
dit(wpm);
dah(wpm);
dit(wpm);
dit(wpm);
delay(3*wpm);
}
void M (int wpm) {
dah(wpm);
dah(wpm);
delay(3*wpm);
}
void N (int wpm) {
dah(wpm);
dit(wpm);
delay(3*wpm);
}
void O (int wpm) {
dah(wpm);
dah(wpm);
dah(wpm);
delay(3*wpm);
}
void P (int wpm) {
dit(wpm);
dah(wpm);
dah(wpm);
dit(wpm);
delay(3*wpm);
}
void Q (int wpm) {
dah(wpm);
dah(wpm);
dit(wpm);
dah(wpm);
delay(3*wpm);
}
void R (int wpm) {
dit(wpm);
dah(wpm);
dit(wpm);
delay(3*wpm);
}
void S (int wpm) {
dit(wpm);
dit(wpm);
dit(wpm);
delay(3*wpm);
}
void T (int wpm) {
dah(wpm);
delay(3*wpm);
}
void U (int wpm) {
dit(wpm);
dit(wpm);
dah(wpm);
delay(3*wpm);
}
void V (int wpm) {
dit(wpm);
dit(wpm);
dit(wpm);
dah(wpm);
delay(3*wpm);
}
void W (int wpm) {
dit(wpm);
dah(wpm);
dah(wpm);
delay(3*wpm);
}
void X (int wpm) {
dah(wpm);
dit(wpm);
dit(wpm);
dah(wpm);
delay(3*wpm);
}
void Y (int wpm) {
dah(wpm);
dit(wpm);
dah(wpm);
dah(wpm);
delay(3*wpm);
}
void Z (int wpm) {
dah(wpm);
dah(wpm);
dit(wpm);
dit(wpm);
delay(3*wpm);
}
void N1 (int wpm) {
dit(wpm);
dah(wpm);
dah(wpm);
dah(wpm);
dah(wpm);
delay(3*wpm);
}
void N2 (int wpm) {
dit(wpm);
dit(wpm);
dah(wpm);
dah(wpm);
dah(wpm);
delay(3*wpm);
}
void N3 (int wpm) {
dit(wpm);
dit(wpm);
dit(wpm);
dah(wpm);
dah(wpm);
delay(3*wpm);
}
void N4 (int wpm) {
dit(wpm);
dit(wpm);
dit(wpm);
dit(wpm);
dah(wpm);
delay(3*wpm);
}
void N5 (int wpm) {
dit(wpm);
dit(wpm);
dit(wpm);
dit(wpm);
dit(wpm);
delay(3*wpm);
}
void N6 (int wpm) {
dah(wpm);
dit(wpm);
dit(wpm);
dit(wpm);
dit(wpm);
delay(3*wpm);
}
void N7 (int wpm) {
dah(wpm);
dah(wpm);
dit(wpm);
dit(wpm);
dit(wpm);
delay(3*wpm);
}
void N8 (int wpm) {
dah(wpm);
dah(wpm);
dah(wpm);
dit(wpm);
dit(wpm);
delay(3*wpm);
}
void N9 (int wpm) {
dah(wpm);
dah(wpm);
dah(wpm);
dah(wpm);
dit(wpm);
delay(3*wpm);
}
void Nzero (int wpm) {
dah(wpm);
dah(wpm);
dah(wpm);
dah(wpm);
dah(wpm);
delay(3*wpm);
}
//
void dit (int wpm) {
digitalWrite(pinout, HIGH); // set the LED on  
digitalWrite(13, HIGH); // set the LED on
delay(wpm); // wait for a second
digitalWrite(pinout, LOW); // set the LED off
digitalWrite(13, LOW); // set the LED off
delay(wpm); // wait for a second
}
void dah (int wpm) {
digitalWrite(pinout, HIGH); // set the LED on  
digitalWrite(13, HIGH); // set the LED on
delay(3*wpm); // wait for a second
digitalWrite(13, LOW); // set the LED off
digitalWrite(pinout, LOW); // set the LED off
delay(wpm); // wait for a second
}


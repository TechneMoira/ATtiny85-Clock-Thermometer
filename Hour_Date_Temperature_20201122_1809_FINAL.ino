// This program is FREE of use IF NOT USED in COMMERCIAL, MILITARY, or ILLICIT applications or PROFIT GENERATING schemes
// Please SUBSCRIBE to channel TECHNEMOIRA on Youtube
// Technemoira and subsidiaries DOES/DO NOT ASSUME ANY LIABILITY, either explicitly or implicitly for any damages that might 
// be caused by use of this (or other versions of this) program
// The user of this program assumes ALL responsibilities deriving from use of this program or any derived versions of it
// THIS program HAS NOT BEEN OFFICIALLY APPROVED for health-, military- or machine control-applications
// USE THIS PROGRAM AT YOUR OWN RISK AND UNDER YOUR OWN RESPONSIBILITY

// Driving DISPLAY IC MAX7219 READING CLOCK IC DS1302 (written for ATTINY85 Spark)
// Include the AVR interrupt.h library (C++ style include)
#include "C:\Media Files\Projects\Personal\TechneMoira\Resources\Arduino\Install\arduino-1.8.9\arduino-1.8.9\hardware\tools\avr\avr\include\avr\interrupt.h"

void setup() 
{
// Data output
  pinMode(0, OUTPUT);
// Clock
  pinMode(1, OUTPUT);

    sendWord(B00001100, B00000000);//shutdown mode OFF B00000000
    sendWord(B00001011, B00000111);//set scan limit to 8 DIGIT max 
    sendWord(B00001001, B11111111);//B11111111 set decode mode to BCD for DIGIT 0-7
    sendWord(B00001010, B00000011);// TEST set DIGIT to half intensity; needs only to be sent once     
    blankDisplay();
    sendWord(B00001100, B00000001);//shutdown mode ON B00000001

// SETUP INTERRUPT HANDLING HERE
// Arduino style interrupt assignments don't work (yet)
//    attachInterrupt(digitalPinToInterrupt(3), sensePins, LOW);
//    pinMode(3, INPUT);
//    attachInterrupt(3, sensePins, CHANGE); //LOW,CHANGE,RISING,FALLING,HIGH are possible interrupt signals

//  GIMSK = B01100000; //General Interrupt Mask Register = Enable External Interrupts (bit 6) 
                     //Enable Pin change interrupts (bit 5)
//  PCMSK = B00001000;//Enable the desired pin (ATTINY PB3 pin, bit 3: all ports are defined by bits 0 - 5 => PB0 - PB5)
                    //to trigger ISR(PCINT0_vect) or a "higher" interrupt ISR(INT0_vect)
//  sei();//Enable ALL interrupts on the ATTINY85
//END SETUP INTERRUPT
}

//The static volatile variables are global "memory" to hold read pin-levels and share them over all functions/instances at once
  static volatile byte sensePinRegister;// = 0xFF;//Set sensePinRegister to (B11111111)
//static volatile int setCount = 9960;//SetCount is a global variable for the CountDown function

//(MAX7219/MAX7221) characters
// "0","1","2","3","4","5","6","7","8","9","-"," ","H","E","L","P"
//byte displayCharacter [16] = {B00000000,B00000001,B00000010,B00000011,B00000100,B00000101,B00000110,B00000111,B00001000,B00001001,B00001010,B00001111,B00001100,B00001011,B00001101,B00001110}; //Bin character definition
  byte displayCharacter [16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0F,0x0C,0x0B,0x0D,0x0E}; //Hex character definition

//Get data from the clock (DS1302)
  byte clockReadRegister [9] = {0x81,0x83,0x85,0x87,0x89,0x8D,0x8B,0x8F,0x91}; 
//Write data to clock (DS1302)
  byte clockWriteRegister [9] = {0x80,0x82,0x84,0x8A,0x86,0x88,0x8C,0x8E,0x90}; 

//Buffer to Get data from the temperature sensor (DS18B20)
  byte sensorReadBuffer [9] = {0x01,0x00,0x02,0x00,0x03,0x00,0x04,0x00,0x05};

void loop() 
{
int Temperature = 0;

//Initialize clock (call the setWord function CALLS listed hereunder just ONCE 
//if you want to set time and date via program, once time/date is set comment out the calls and upload
//AGAIN to Spark to disable the time/date set Function Calls. Tip: Give yourself 2 minutes headstart)
//setClockWord(2,0,1,clockWriteRegister[0],(byte)0x00);//Seconds (specify the ACTUAL Seconds rather than the HEX value)
//setClockWord(2,0,1,clockWriteRegister[1],(byte)0x22);//Minutes (see Seconds variable)
//setClockWord(2,0,1,clockWriteRegister[2],(byte)0x13);//Hours (see Minutes and Seconds variables)
//setClockWord(2,0,1,clockWriteRegister[3],(byte)0x04);//Day of the Week (Sunday=1,Monday=2,Tuesday=3,Wednesday=4,Thursday=5,Friday=6,Saturday=7)
//setClockWord(2,0,1,clockWriteRegister[4],(byte)0x13);//Days (specify the ACTUAL day of calender rather than the HEX value of it)
//setClockWord(2,0,1,clockWriteRegister[5],(byte)0x11);//Months (see Days variable)
//setClockWord(2,0,1,clockWriteRegister[6],(byte)0x20);//Years (see Months and Days variables)
//END Initialize clock

      for (int countByte = 0; countByte <= 8; countByte++)
       {
        switch (countByte)
        {          
        case 0:
        sendWord((byte)((countByte*2)+1), receiveWord(clockReadRegister[countByte]));//Set Seconds DIGIT
        sendWord((byte)((countByte*2)+2), receiveWord(clockReadRegister[countByte])>>4);//Set Tens of Seconds DIGIT
        break;
        case 1:
        sendWord((byte)((countByte*2)+1), 0x0F);//Set seconds/minute spacer
        sendWord((byte)((countByte*2)+2), receiveWord(clockReadRegister[countByte]));//Set Minutes DIGIT
        sendWord((byte)((countByte*2)+3), receiveWord(clockReadRegister[countByte])>>4);//Set Tens of Minutes DIGIT
        break;
        case 2:
        sendWord((byte)((countByte*2)+2), 0x0F);//Set seconds/minute spacer
        sendWord((byte)((countByte*2)+3), receiveWord(clockReadRegister[countByte]));//Set Hours DIGIT
        sendWord((byte)((countByte*2)+4), receiveWord(clockReadRegister[countByte])>>4);//Set Tens of Hours DIGIT
        delay (2500);
        break;
        case 3:
        sendWord((byte)(countByte-2), receiveWord(clockReadRegister[countByte]));//Set Days DIGIT
        sendWord((byte)(countByte-1), receiveWord(clockReadRegister[countByte])>>4);//Set Tens of Days DIGIT
        break;
        case 4:
        sendWord((byte)(countByte-1), 0x0A);//Set days/months spacer
        sendWord((byte)(countByte), receiveWord(clockReadRegister[countByte]));//Set Months DIGIT
        sendWord((byte)(countByte+1), receiveWord(clockReadRegister[countByte])>>4);//Set Tens of Months DIGIT
        break;
        case 5:
        sendWord((byte)(countByte+1), 0x0A);//Set months/years spacer
        sendWord((byte)(countByte+2), receiveWord(clockReadRegister[countByte]));//Set Years DIGIT
        sendWord((byte)(countByte+3), receiveWord(clockReadRegister[countByte])>>4);//Set Tens of Years DIGIT
        delay (2500);
        break;
        case 6: 
          blankDisplay();
//          Temperature = (readSensor(1,3) & 0x0F) + (((readSensor(1,3) & 0xF0) >> 4) * 16); //use this line for temperature readings in Fahrenheit
//          Temperature = (readSensor(0,3) & 0x0F) + (((readSensor(0,3) & 0xF0) >> 4) * 16); //uncomment this line for temperature readings in Celsius
          Temperature = (readSensor(0,3) & 0x0F) + (((readSensor(0,3) & 0xF0) >> 4) * 16) - 15; //subtract an offset of 15C°(~900mV) to stay above 900mV at lowest temperature (-10C°)
          //Celsius section
          if ((sensorReadBuffer[4] == 0x00) && (Temperature <= 0)) 
            {
                sendWord((byte)((countByte*2)-5), 0x0A); //Send a "minus" symbol to the display if Temperature is in C° and <= 0 degrees 
                sendWord((byte)((countByte*2)-7), round(abs(Temperature) / 10)); //Set temperature unit DIGIT                
              if ( (abs(round(Temperature / 10)) <= 9) && ( ( abs(Temperature) - round( (abs(Temperature) / 10) * 10) ) == 0 ) )
              {
                sendWord((byte)((countByte*2)-7), round(abs(Temperature) / 10)); //Set temperature unit DIGIT                
                sendWord((byte)((countByte*2)-8), 0x00); //Set temperature decade DIGIT to 0
              }
              else if ( (abs(round(Temperature / 10)) <= 9) && ( ( abs(Temperature) - round( (abs(Temperature) / 10) * 10) ) <= -1 ) )
              {
                sendWord((byte)((countByte*2)-7), round(abs(Temperature) / 10)); //Set temperature unit DIGIT                
                sendWord((byte)((countByte*2)-8), abs(Temperature) - (round(abs(Temperature) / 10)*10));//Set temperature decade DIGIT
              }
            }            
          if ((sensorReadBuffer[4] == 0x00) && (Temperature >= 1)) 
            {
              sendWord((byte)((countByte*2)-5), 0x0F); //Set blank spacer to clear any "minus" symbol
              sendWord((byte)((countByte*2)-7), round(Temperature / 10)); //Set temperature unit DIGIT
              sendWord((byte)((countByte*2)-8), Temperature - (round(Temperature / 10)*10));//Set temperature decade DIGIT
            }

          //Fahrenheit section
          if ((sensorReadBuffer[4] == 0x01) && (Temperature <= 32))  
            {
              sendWord((byte)((countByte*2)-5), 0x0A); //Send a "minus" symbol to the display if Temperature is in F° and <= 32 degrees
            }

        delay (2500);             
        break;

/*        case 7:
        break;
        
        default:
        break;
*/
       }//END switch
      }//END countByte

/*  sei();//Enable ALL interrupts on the ATTINY85 */

}
// END void Loop


void blankDisplay(void)
  {
      for (int countByte = 1; countByte <= 8; countByte++)
       {
        byte writeDigit = 0x0F; 
        sendWord((byte)countByte, writeDigit);// Blank display
       }//END countByte    
  }

void sendWord(byte command, byte parameter)
{
  pinMode(0, OUTPUT);//Data
// Chip select _CS
  pinMode(4, OUTPUT);  
    digitalWrite(4, LOW);//CS LOW     
      for (int countBit = 7; countBit >= 0; countBit--)
      {
        digitalWrite(0,bitRead(command, countBit)); //DATA send MSB first
        digitalWrite(1,HIGH); //CLK HIGH
        digitalWrite(1,LOW); //CLK LOW
      };
          
      for (int countBit = 7; countBit >= 0; countBit--)
      {
        digitalWrite(0,bitRead(parameter,countBit)); //DATA send MSB first
        digitalWrite(1,HIGH); //CLK HIGH
        digitalWrite(1,LOW); //CLK LOW
      };
    digitalWrite(4, HIGH);//CS HIGH      
  pinMode(0, INPUT);//Data
}

byte receiveWord(byte command)
{
  byte timeDate;
  timeDate = 0x0A;  
// Chip select _CS
  pinMode(2, OUTPUT);
    digitalWrite(2, HIGH);//CS/CE HIGH     
  pinMode(0, OUTPUT);//Data
      for (int countBit = 0; countBit <= 7; countBit++)
      {
        digitalWrite(0,bitRead(command, countBit)); //DATA sent LSB first
        digitalWrite(1,LOW); //CLK LOW
        digitalWrite(1,HIGH); //CLK HIGH
      };    
  pinMode(0, INPUT);//Data
  digitalRead(0);
      for (int countBit = 0; countBit <= 7; countBit++)
      {
        digitalWrite(1,LOW); //CLK LOW        
        bitWrite(timeDate,countBit,digitalRead(0)); //DATA received LSB first
        digitalWrite(1,HIGH); //CLK HIGH
      };
    digitalWrite(2, LOW);//CS LOW      
  pinMode(0, OUTPUT);//Data
return timeDate;
}

void setClockWord(int pin_CS, int pin_D, int pin_CLK, byte command, byte parameter)
//Mainly used to SET the Clockchip DS1302 registers for now
// pin 2 = Chip Select; pin 0 = Data; pin 1 = Clock
{
// Chip select _CS
//  pinMode(2, OUTPUT); //pin 2 is used for setting the DS1302 Clock 
  pinMode(pin_CS, OUTPUT);
    digitalWrite(pin_CS, HIGH);//CS/CE HIGH //Set Chip Select line to HIGH
  pinMode(pin_D, OUTPUT);//Data
      for (int countBit = 0; countBit <= 7; countBit++) //Send out the COMMAND-byte bits in serial fashion
      {
        digitalWrite(pin_D,bitRead(command, countBit)); //DATA sent LSB first
        digitalWrite(pin_CLK,LOW); //CLK LOW
        digitalWrite(pin_CLK,HIGH); //CLK HIGH
      };    
        digitalWrite(pin_CLK,LOW); //CLK LOW
      for (int countBit = 0; countBit <= 7; countBit++) //Send out the PARAMETER-byte bits in serial fashion
      {
        digitalWrite(pin_D,bitRead(parameter, countBit)); //DATA sent LSB first
        digitalWrite(pin_CLK,HIGH); //CLK HIGH
        digitalWrite(pin_CLK,LOW); //CLK LOW
      };
    digitalWrite(pin_CS, LOW);//CS LOW //Chip Select line LOW
}

byte readSensor(int C_F, int pin_D)
{
  byte temperatureCelsius;
  byte temperatureFahrenheit;

//C_F: conversion factor: 0 = Celsius; 1 = Fahrenheit; 2 = RAW converted value; 3 = DEBUG value 7
//Used to convert the Temperature Sensor chip LM335 voltage
// pin 3 (PB3) => LM335 Data I/O pin (center pin)

// EXPERIMENTAL HERE BELOW PROCEED AT YOUR OWN RISK
// pin 5 (PB5) => Send a positive pulse to portB pin 5 to switch on (or off) something like an LED or a transistor (LOW ~1,3V high ~5V = Vsupply)
// It's IMPERATIVE that the voltage divider output on PB5 remains around +1,3V (too low to switch on a blue LED) or ABOVE! otherwise the Spark will RESET!
// When you digitalWrite a "HIGH" to PB5 you wil switch ON the (blue) LED or any other "device" if set properly
// END EXPERIMENTAL

  pinMode(pin_D, INPUT);// Read Data from Sensor via pin pin_D of the Spark
  sensorReadBuffer[2] = (byte)analogRead(pin_D);//Last two digits of the RAW A/D converted measured value (int value between 0 and 1023)
  sensorReadBuffer[3] = (byte)round(20.0); //DEBUG VALUE (20C° = 68F°)
  sensorReadBuffer[4] = (byte)C_F; //Store the Temperature unit for further use
  sensorReadBuffer[5] = (byte)pin_D; //Store the sensor pin number for further use
  
//CONVERT the measured value from the spark A/D to separate digits for display
//Check the sensor is CALIBRATED properly (18C° ~ 2,911V)
      temperatureCelsius = (byte)round(analogRead(pin_D) / 15); //Convert the integer raw A/D value to int Celsius 
                                                                //(use a conversion factor between 5 and 33 depending on the (internal resistance of the NTC) sensor)      
                                                                //Voltage divider with 10Kohm NTC yields approx. +1.820V at 20C°)
      sensorReadBuffer[0] = temperatureCelsius;//Convert temperature in Celsius to BINARY
      temperatureFahrenheit = (byte)(((int)temperatureCelsius * 1.8) + 32);
      sensorReadBuffer[1] = temperatureFahrenheit;
    switch(C_F)
    {
    case 0:
    return sensorReadBuffer[C_F];
    break;
    case 1:
    return sensorReadBuffer[C_F];
    break;
    case 2:
    return sensorReadBuffer[C_F];
    break;
    case 3:
    return sensorReadBuffer[C_F];
    break;
    case 4:
    return sensorReadBuffer[C_F];
    break;
    case 5:
    return sensorReadBuffer[C_F];
    break;
    }
}

//void sensePins()//Interrupt Service Routine to detect a hardware interrupt (EXPERIMENTAL SECTION NOT USED FOR NOW)
/*
ISR(INT0_vect)//Should be PCINT0 according to Arduino AVR interrupt documentation, maybe because it's not a genuine Spark?
{
  pinMode(3, INPUT);//pin PB3 (Mode select function + set clock function) MUST be EXPLICITLY set to INPUT
  if (digitalRead(3) == 0)//Even though pin PB3 generates an interrupt it's state MUST be read explicitly
    {
      sensePinRegister = 0xFB;//Signal Pin PB3 was pressed by setting volatile byte sensePinRegister bit 2 to LOW = 0
    }
}*/

/*
* For Pro Micro, consider ...\libraries\IRremote\boarddefs.h
* 
* There are sections for:
* 
* 1) ARDUINO_AVR_PROMICRO
* 2) __AVR_ATmega32U4__
*
* Item 2 shows the pin numbers for "Teensy 2.0", not Leonardo or ProMicro
* 
* Item 1 may not exist in the current version. It looked like this:
* 
* // Sparkfun Pro Micro
* #if defined(ARDUINO_AVR_PROMICRO)
*   //#define IR_USE_TIMER1     // tx = pin 9
*     #define IR_USE_TIMER3       // tx = pin 5
*     //#define IR_USE_TIMER4_HS  // tx = pin 5
*     
* Go into the definition for item 1, and uncomment the line with "IR_USE_TIMER3" and comment out the line with "IR_USE_TIMER4_HS"
* 
* Then find the line with "# error "Please add OC3A pin number here\n" and comment that out, and add a line with "#  define SEND_PIN  5"
* 
* Now for ProMicro, connect the IR transmit LED (with series resistor) to pin5 and ground
* 
* Note that you can use either "Leonardo" or "Arduino/Genuino Micro" as the board selection for a "Pro Micro" board
* 
*/

#include <IRremote.h>
IRsend irsend;

//Lirc codes for Dish Network
//power_on 1
//up       26
//select   16
//0        17
//1        4
//2        5
//3        6
//4        8
//5        9
//6        10
//7        12
//8        13
//9        14

//ASCII
//a 0x61
//b 0x62
//p 0x70
//s 0x73
//u 0x75
//0 0x30
// ...
//9 0x39

//0xE0E040BF is Samsung Power "Toggle".
//0xE0E09966 is Samsung Power On, and 0xE0E019E6 is Power Off.
//0xE0E09768 is Samsung "INPUT HDMI"

//0x20DF10EF is LG Power "Toggle".
//0x20DF23DC is LG Power On, and 0x20DFA35C is Power Off.

bool lastState;

void setup()
{
  Serial.begin(9600);
  delay(100);
  Serial.print("Ready");
}

void loop() {
  // if there's any serial available, read it:
  while (Serial.available() > 0) {

    // look for the next valid integer in the incoming serial stream:
    long type = Serial.parseInt();
    // do it again:
    unsigned long data = Serial.parseInt();
    // do it again:
    unsigned long len = Serial.parseInt();

    // user input must be decimal. The two numbers can be separated with spaces and/or commas. 
    // for example, to send NEC code 0x04fb83, send "0,326531,16"

    // look for the newline. That's the end of your sentence:
    if (Serial.read() == '\n') 
    {
      // print the numbers in one string as hexadecimal:
      Serial.print(type, HEX);
      Serial.print(", ");
      Serial.print(data, HEX);
      Serial.print(", ");
      Serial.println(len, HEX);

      switch (type) {
          case 0:
            irsend.sendNEC(data, 32);
            for (int i=0;i<len;i++)
            {
              delay(100);
              irsend.sendNEC(REPEAT, 32);
            }
            break;                        
          case 3:
            for (int i=0;i<3;i++)
            {
              irsend.sendSAMSUNG(data, len);
            }
            break;
          case 4:
            for (int i=0;i<3;i++)
            {
              irsend.sendSony(data, len);
            }
            break;
          case 5:            
            for (int i=0;i<3;i++)
            {
              delay(100);
              irsend.sendRC5(data, len);
            }
            break;
          case 6:
            for (int i=0;i<3;i++)
            {
              delay(100);
              irsend.sendRC6(data, len);
            }
          default:
            // if nothing else matches, do the default
            // default is optional
            send4(data);
          break;
        }
    }
  }
  if (USBDevice.isSuspended()!=lastState)
  {
    if (USBDevice.isSuspended())
    {      
      //delay(5000);   // wait for 5 seconds 
      Serial.println("suspended");
      //0x20DF23DC is LG Power On, and 0x20DFA35C is Power Off.
      //0xE0E09966 is Samsung Power On, and 0xE0E019E6 is Power Off.
      irsend.sendNEC(0x20DFA35C, 32);
      irsend.sendSAMSUNG(0xE0E019E6, 32);           
    }
    else
    {
      //delay(5000);   // wait for 5 seconds 
      Serial.println("NOT suspended");
      //0x20DF23DC is LG Power On, and 0x20DFA35C is Power Off.
      //0xE0E09966 is Samsung Power On, and 0xE0E019E6 is Power Off.
      irsend.sendNEC(0x20DF23DC, 32);
      irsend.sendSAMSUNG(0xE0E09966, 32);       
    }    
  }
  lastState = USBDevice.isSuspended();
  if (USBDevice.isSuspended())
  {
      RXLED1;
      TXLED1;
      delay(500);      
      RXLED0;
      TXLED0;
      delay(500); 
  }
}


// DISH remote always sends the same code 4 times
void send4(long code) {
//  int unit = 0x240; //remote has 'unit code' 10
//  code <<= 10;
//  long data = code | unit; 
  for (int i = 0; i < 4; i++) {
    //irsend.sendDISH(data, 16);
    irsend.sendDISH(code, 16); //sendDISH always leaves the LED on, that's curious, but inconsequential
    //delay(10);
  }
}
  

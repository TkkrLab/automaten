/*
 * Vending machine firmware: front panel
 * 
 * MIT License
 *  
 * Copyright (c) 2019 Renze Nicolai
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 */


//----- CONFIGURATION -----

#define UNIT_ID     3

//#define TESTMODE

#define PIN_BUTTON1 A0
#define PIN_BUTTON2 A1
#define PIN_BUTTON3 A2
#define PIN_BUTTON4 A3
#define PIN_BUTTON5 A4
#define PIN_LED1     6
#define PIN_LED2     5
#define PIN_LED3     4
#define PIN_LED4     3
#define PIN_LED5     2

#define NUM 5

const int buttons[NUM] = {PIN_BUTTON1, PIN_BUTTON2, PIN_BUTTON3, PIN_BUTTON4, PIN_BUTTON5};
const int leds[NUM]    = {PIN_LED1,    PIN_LED2,    PIN_LED3,    PIN_LED4,    PIN_LED5};

//-------------------------

//Do not change anything below this line.

const unsigned long interval = 200; //Interval in milliseconds

uint8_t prevButtonVal = 0;
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < NUM; i++) {
    pinMode(buttons[i], INPUT_PULLUP);
    pinMode(leds[i], OUTPUT);
  }

  #ifdef TESTMODE
  while (1) {
    for (int i = 0; i < NUM; i++) {
      bool val = digitalRead(buttons[i]);
      digitalWrite(leds[i], val);
      Serial.print(val);
    }
    Serial.println();
  }
  #endif
  
  Serial.println("id="+String(UNIT_ID));
  Serial.println("type=frontpanel");
  Serial.println("boot=ok");
}

void loop() {
  while (Serial.available()>0) {
      char c = Serial.read();
      switch(c) {        
        case 'i':
          Serial.println("id="+String(UNIT_ID));
          break;
  
        case 'm':
          Serial.println("motors=0");
          break;
  
        case 't':
          Serial.println("type=frontpanel");
          break;

        case 'l':
        {
            uint8_t led = Serial.parseInt();
            if (led >= NUM) {
              Serial.println("#number too high");
            } else {
              digitalWrite(leds[led], HIGH);
            }
            break;
        }

        case 'L':
        {
            uint8_t led = Serial.parseInt();
            if (led >= NUM) {
              Serial.println("#number too high");
            } else {
              digitalWrite(leds[led], LOW);
            }
            break;
        }

        case 's':
        {
            uint8_t led = Serial.parseInt();
            for (uint8_t i = 0; i < NUM; i++) {
              bool val = (led>>i)&0x01;
              digitalWrite(leds[i], !val);
            }
            break;
        }
    }
  }

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;
    uint8_t buttonVal = 0;
    
    for (int i = 0; i < NUM; i++) {
      if (!digitalRead(buttons[i])) buttonVal += (1<<i);
    }
  
    if (prevButtonVal != buttonVal) {
      prevButtonVal = buttonVal;
      Serial.print("buttons=");
      Serial.println(buttonVal);
    }
  }
}

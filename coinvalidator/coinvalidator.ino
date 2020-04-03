/*
 * Vending machine firmware: coin validator
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

#define UNIT_ID     2

//#define TESTMODE

#define PIN_COIN1   6
#define PIN_COIN2   7
#define PIN_COIN3   8
#define PIN_COIN4   9
#define PIN_COIN5   2
#define PIN_COIN6   3
#define PIN_RETURN  4
#define PIN_INHIBIT 5
#define PIN_LED     10
#define PIN_LED_INT 13

#define NUM_INPUTS 6
const int pins[NUM_INPUTS] = {PIN_COIN1, PIN_COIN2, PIN_COIN3, PIN_COIN4, PIN_COIN5, PIN_COIN6};//, PIN_RETURN};
const char* values[NUM_INPUTS] = {"5", "10", "20", "50", "100", "200"};//, "RETURN"};

//-------------------------

//Do not change anything below this line.

const unsigned long interval = 200; //Interval in milliseconds

bool prev[NUM_INPUTS] = {1};
unsigned long previousMillis[NUM_INPUTS] = {0};

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < NUM_INPUTS; i++) {
    pinMode(pins[i],   INPUT_PULLUP);
  }
  pinMode(PIN_INHIBIT, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_LED_INT, OUTPUT);
  
  digitalWrite(PIN_INHIBIT, HIGH); //Do not accept coins

  #ifdef TESTMODE
  digitalWrite(PIN_INHIBIT, LOW); //Accept coins
  while (1) {
    Serial.print(digitalRead(PIN_COIN1));
    Serial.print(digitalRead(PIN_COIN2));
    Serial.print(digitalRead(PIN_COIN3));
    Serial.print(digitalRead(PIN_COIN4));
    Serial.print(digitalRead(PIN_COIN5));
    Serial.print(digitalRead(PIN_COIN6));
    Serial.print(" ");
    Serial.print(digitalRead(PIN_RETURN));
    Serial.println();
  }
  #endif
  
  Serial.println("id="+String(UNIT_ID));
  Serial.println("type=coin");
  Serial.println("state=disabled");
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

        case 'e':
          digitalWrite(PIN_INHIBIT, LOW);
          Serial.println("state=enabled");
          break;
          
        case 'd':
          digitalWrite(PIN_INHIBIT, HIGH);
          Serial.println("state=disabled");
          break;
  
        case 't':
          Serial.println("type=coin");
          break;

        case 'l':
          digitalWrite(PIN_LED, LOW);
          digitalWrite(PIN_LED_INT, LOW);
          break;
        
        case 'L':
          digitalWrite(PIN_LED, HIGH);
          digitalWrite(PIN_LED_INT, HIGH);
          break;
    }
  }

  unsigned long currentMillis = millis();
  
  for (int i = 0; i < NUM_INPUTS; i++) {
    bool curr = digitalRead(pins[i]);
    if ((!curr) && prev[i]) {
      Serial.print("coin=");
      Serial.println(values[i]);
      prev[i] = false;
      previousMillis[i] = currentMillis;
    }
    if ((curr) && (!prev[i]) && (currentMillis - previousMillis[i] > interval)) prev[i] = true;
  }  
}

#include <OneWire.h>

#define ONEWIRE_LED_RED 10
#define ONEWIRE_LED_GRN 11
#define ONEWIRE_PIN 12

#define UNIT_ID 7

OneWire ds(ONEWIRE_PIN);

unsigned long previousMillis = 0;
const unsigned long interval = 10; //Interval in milliseconds

void halt() {
  Serial.println("#Halting...");
  Serial.println("state=halted");
  while(1) delay(1);
}

void setup() {
  pinMode(ONEWIRE_LED_RED, OUTPUT);
  pinMode(ONEWIRE_LED_GRN, OUTPUT);
  
  Serial.begin(115200);
  Serial.println("id="+String(UNIT_ID));
  Serial.println("type=ibutton");
  Serial.println("state=booting");
  if (!digitalRead(ONEWIRE_PIN)) {
    Serial.println("error=onewire");
    halt();
  }
  Serial.println("boot=ok");
  Serial.println("state=ready");
}

bool newdevice = true;

uint8_t ledPos = 0;
bool ledDir = false;
uint8_t ledColor = false;

bool pollOneWire() {
  bool present = ds.reset();
  ds.skip();
  if (present) {
    uint8_t addr_in[8] = {0};
    uint8_t addr[8] = {0};
    if (ds.search(addr_in)) {
      if (newdevice) {
        if (ds.crc8( addr_in, 7) == addr_in[7]) {
          //Serial.println("state=ibutton");
          String idstring = "";
          for (uint8_t i = 0; i<8; i++) {
            idstring = idstring + ("0123456789abcdef"[addr_in[i]>>4]) + ("0123456789abcdef"[addr_in[i]&0xf]);
          }
          Serial.println("ibutton="+idstring);
        } else {
          Serial.println("nfe=crc");
        }
      }
      newdevice = false;
    } else {
      ds.reset_search();
    }
  } else {
    newdevice = true;
  }
  return !newdevice;
}

bool prevBusy = false;

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
          Serial.println("type=ibutton");
          break;

        case 'l': {
          uint8_t color = Serial.parseInt();
          Serial.println("#LED = "+String(color));
          digitalWrite(ONEWIRE_LED_RED, color&0x01);
          digitalWrite(ONEWIRE_LED_GRN, (color&0x02) >> 1);
          break;
        }
        case 'R': {
          uint8_t color = Serial.parseInt();
          Serial.println("#LED PWM RED = "+String(color));
          analogWrite(ONEWIRE_LED_RED, color);
          break;
        }
        case 'G': {
          uint8_t color = Serial.parseInt();
          Serial.println("#LED PWM GRN ANI = "+String(color));
          analogWrite(ONEWIRE_LED_GRN, color);
          break;
        }
        case 'L': {
            uint8_t color = Serial.parseInt();
            Serial.println("#LED PWM GRN ANI = "+String(color));
            ledPos = 0;
            ledDir = false;
            ledColor = color;
            if (!(color & 0x02)) digitalWrite(ONEWIRE_LED_GRN, LOW);
            if (!(color & 0x01)) digitalWrite(ONEWIRE_LED_RED, LOW);
          break;
        }
        default:
          Serial.print("#Ignored: ");
          Serial.println((uint8_t) c);
      }
    }
  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > interval) {
    previousMillis = currentMillis;

    bool busy = pollOneWire();

    if (prevBusy!=busy) {
      prevBusy = busy;
      if (busy) {
        Serial.println("state=busy");
      } else {
        Serial.println("state=ready");
      }
    }

    if (ledColor > 0) {
      if (!ledDir) {
        ledPos+=1;
        if (ledPos>=126) ledDir = true;
      } else {
        ledPos-=1;
        if (ledPos<=2) ledDir = false;
      }
      if (ledColor&0x01) analogWrite(ONEWIRE_LED_RED, ledPos*2);
      if (ledColor&0x02) analogWrite(ONEWIRE_LED_GRN, ledPos*2);
    }
  }
}

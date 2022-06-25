#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <stdlib.h>

#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// declare an SSD1306 display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// yes i understand there is a 2% - 5% chance of error at high baud
#define PACKET_SIZE 18 // 16 bytes per packet

int packet[PACKET_SIZE]; // serial buffer for incoming data - in bytes
bool lstart = false;      // start of message flag

// the ardunio uno has 2kb operating memory, and the i2c screen uses 1kb 
// leaving me with a migraine and a small buffer
char buffer[128];
int shthold[32] = {0,0,0,0,0,0,0,0};
int shthold2[32] = {0,0,0,0,0,0,0,0};
int container = 0;
int bin = 0;
long bno = 0;
long rem = 0;
long r = 1;
int p = 0;
int o = 0;
char v[4];
char binn[8]; // debug buffer
int y = 0;
bool go = false;

void decode() {
  int l = 0;
  for (int f = 0; f < PACKET_SIZE - 2; f++) {
    container = packet[f];

    //line bounds
    if (packet[f] == 172) {
      go = true;
      l++;
      continue;
    }
    else if (packet[f] == 63) {
      go = false;
      continue;
    }

    shthold[7] = 0;
    shthold[6] = 0;
    shthold[5] = 0;
    shthold[4] = 0;
    shthold[3] = 0;
    shthold[2] = 0;
    shthold[1] = 0;
    shthold[0] = 0;
    shthold2[7] = 0;
    shthold2[6] = 0;
    shthold2[5] = 0;
    shthold2[4] = 0;
    shthold2[3] = 0;
    shthold2[2] = 0;
    shthold2[1] = 0;
    shthold2[0] = 0;
    bin = container;
    while (bin != 0) {
      rem = bin % 2;
      bin = bin / 2;
      shthold2[o] = rem;
      o++;
    }
    y = 8 - o;
    //lazy but it works
    switch(y) {
      case 1:
        shthold[7] = shthold2[6];
        shthold[6] = shthold2[5];
        shthold[5] = shthold2[4];
        shthold[4] = shthold2[3];
        shthold[3] = shthold2[2];
        shthold[2] = shthold2[1];
        shthold[1] = shthold2[0];
        break;
      case 2:
        shthold[7] = shthold2[5];
        shthold[6] = shthold2[4];
        shthold[5] = shthold2[3];
        shthold[4] = shthold2[2];
        shthold[3] = shthold2[1];
        shthold[2] = shthold2[0];
        break;
      case 3:
        shthold[7] = shthold2[4];
        shthold[6] = shthold2[3];
        shthold[5] = shthold2[2];
        shthold[4] = shthold2[1];
        shthold[3] = shthold2[0];
        break;
      case 4:
        shthold[7] = shthold2[3];
        shthold[6] = shthold2[2];
        shthold[5] = shthold2[1];
        shthold[4] = shthold2[0];
        break;
      case 5:
        shthold[7] = shthold2[2];
        shthold[6] = shthold2[1];
        shthold[5] = shthold2[0];
        break;
      case 6:
        shthold[7] = shthold2[1];
        shthold[6] = shthold2[0];
        break;
      case 7:
        shthold[7] = shthold[0];
        break;
      case 8:
        shthold[7] = 0;
        shthold[6] = 0;
        shthold[5] = 0;
        shthold[4] = 0;
        shthold[3] = 0;
        shthold[2] = 0;
        shthold[1] = 0;
        shthold[0] = 0;
        break;
      case 0:
        shthold[7] = shthold2[0];
        shthold[6] = shthold2[1];
        shthold[5] = shthold2[2];
        shthold[4] = shthold2[3];
        shthold[3] = shthold2[4];
        shthold[2] = shthold2[5];
        shthold[1] = shthold2[6];
        shthold[0] = shthold2[7];
        break;
    }
    o = 0;

    for (int t = 0; t < 8; t++) {
      buffer[((f-l)*8)+t] = shthold[t] + 48;
    }
  }
}

void setup() {
  Serial.begin(57600);
  //Serial.begin(9600);

  // initialize OLED display with address 0x3C for 128x64
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }

  delay(2000); // wait for initializing
  oled.setCursor(0, 0);
  Serial.println("Initialized and ready");
}

void loop() {
  if (Serial.available() > 0) {
    delay(4);
    for (int u = 0; u < 18; u++) {
      packet[u] = Serial.read();
      // if decode() is ran here, it becomes so ram heavy it literally boot loops the uno
      //Serial.print(packet[u]);

      if (packet[u] == 'H') { //respond
        lstart = true;
        Serial.println('H');
        oled.clearDisplay();
        oled.setTextSize(2);
        oled.setCursor(0, 10);
        oled.println("ready");
        oled.display();
        u--;
      }

      // frame bounds
      if (packet[u] == 204) {
        oled.clearDisplay();
        p = -1;
        u--;
      }

      else if (packet[u] == 51) {
        oled.display();
        //p--;
        u--;
      }
    }
    
    decode();

    for (int g = 0; g < 128; g++) {
      if (buffer[g] == '1') {
        oled.drawPixel(g, p, WHITE);
      }
      else if (buffer[g] == '0') {
        oled.drawPixel(g, p, BLACK);
      }
    }
    Serial.print("R");
    p++;

    /* if (p >= 63) {
      p = 0;
    } */
  }
}

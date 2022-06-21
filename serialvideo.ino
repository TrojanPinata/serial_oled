#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// declare an SSD1306 display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// yes i understand there is a 2% - 5% chance of error at high baud
#define PACKET_SIZE 16 // 16 bytes per packet

int packet[PACKET_SIZE]; // serial buffer for incoming data - in bytes
bool lstart = false;      // start of message flag

// the ardunio uno has 2kb operating memory, and the i2c screen uses 1kb 
// leaving me with a migraine and a small buffer
char buffer[128];
int shthold[32];
int container = 0;
int bin = 0;
long bno = 0;
long rem = 0;
long r = 1;
int p = 0;
int o = 0;
char v[4];
char binn[8]; // debug buffer

void decode() {
  for (int f = 0; f < PACKET_SIZE; f++) {
    container = packet[f];

    // some cool debug stuff
    //Serial.println(container);
    /* oled.setCursor(0, 0);
    oled.clearDisplay();
    sprintf(binn, "%d", container);
    oled.println(binn);
    oled.display(); */

    bin = container;
    while (bin != 0) {
      rem = bin % 2;
      bin = bin / 2;
      shthold[o] = rem;
      o++;
    }
    o = 0;
    for (int t = 0; t < 8; t++) {
      buffer[(f*8)+t] = shthold[t] + 48;
    }
  }
}

void setup() {
  Serial.begin(9600);

  // initialize OLED display with address 0x3C for 128x64
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }

  delay(2000); // wait for initializing
  oled.setCursor(0, 0);
  Serial.println("SSD1306 initialized\n");
  Serial.println("Ready to accept packets\n");
}

void loop() {
  //Serial.println("Waiting for packet..."); // debug
  //oled.clearDisplay(); // unnecessary due to black pixel overwrites

  if (Serial.available() > 0) {
    for (int u = 0; u < PACKET_SIZE; u++) {
      packet[u] = Serial.read();
      // if decode is ran her it becomes so ram heavy it literally boot loops the uno
    }

    if (packet[0] == 'H' && !lstart) { //Start of message
      lstart = true;
      Serial.write('H');
      oled.clearDisplay();
      oled.setTextSize(2);
      oled.setCursor(16, 10);
      oled.println("ready");
      oled.display();
    }
  
    else {Serial.println("uh oh");}

    decode();
    for (int g = 0; g < 128; g++) {
      if (buffer[g] == '1') {
        oled.drawPixel(g, p, WHITE);
      }
      else if (buffer[g] == '0') {
        oled.drawPixel(g, p, BLACK);
      }
    }
    p++;
    oled.display();
    if (p == 64) {
      p = 0;
      oled.clearDisplay();
    }
    // buffer = ""; // clear the buffer please
  }

  // display
  //oled.display();
  //delay[10];
}

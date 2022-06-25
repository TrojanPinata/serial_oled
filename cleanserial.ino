#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <stdlib.h>

#define SCREEN_WIDTH 128   // OLED display width,  in pixels
#define SCREEN_HEIGHT 64   // OLED display height, in pixels
#define PACKET_SIZE 18     // 16 bytes per packet (+ header and footer)
#define BAUD 57600         // change based on serial buffer speed and processor frequency

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int packet[PACKET_SIZE];   // main packet for receiving data
int p = 0;                 // line print number - needs to be seen by decode() and displayFrame()
int u = 0;                 // main loop variable for iterating through packet - seen by frameCheck and displayFrame()
char buffer[128];          // main buffer for outputting line
int fHeader = 204;
int fFooter = 51;
int lHeader = 172;
int lFooter = 63;
bool go = false;
int DELAY = 4;

void setup() {
   Serial.begin(BAUD);

   // initialize oled @ram address 0x3C - needs at least 1kb of memory 
   // should address be different because of more ram? is this throttling it??
   if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
   }

   delay(2000); // wait for initializing
   oled.setCursor(0, 0);
   Serial.println("Initialized and ready");

   int h = 0;
   int val = 0;
   while (h == 0) {
      if (Serial.available() > 0) {
         val = Serial.read();
         if (val == 'H') {    //respond
            Serial.println('H');
            oled.clearDisplay();
         }
      }
      Serial.print("R");
      p++;
   }
   // lets do it
}

void loop() {
   if (Serial.available() > 0) {
      delay(DELAY);
      for (int u = 0; u < 18; u++) {
         packet[u] = Serial.read();
      }
      // if decode() is ran here, it becomes so ram heavy it literally boot loops the uno
      //Serial.print(packet[u]);
      checkFrame();
      decode();
      displayLine();
      Serial.print("R");
      p++;
   }
}

void checkFrame() {  // checks frame bounds and clears display before next frame
   if (packet[u] == fHeader) {
     oled.clearDisplay();
     p = -1;   // prevents bug in displayLine()
     u--;
   }
   else if (packet[u] == fFooter) {
     oled.display();
     u--;
   }
}

void decode() {
   int l = 0;
   int o = 0;
   int y = 0;
   int rem = 0;
   int bin = 0;
   int shthold[8] = {0,0,0,0,0,0,0,0};
   int shthold2[8] = {0,0,0,0,0,0,0,0};

   for (int f = 0; f < PACKET_SIZE - 2; f++) {
      packet[f];

      //line bounds
      if (packet[f] == lHeader) {
         go = true;
         l++;
         continue;
      }
      else if (packet[f] == lFooter) {
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
      bin = packet[f];
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

void displayLine() {
   for (int g = 0; g < 128; g++) {
      if (buffer[g] == '1') {
         oled.drawPixel(g, p, WHITE);
      }
      else if (buffer[g] == '0') {
         oled.drawPixel(g, p, BLACK);
      }
   }
}

#include <ESP_8_BIT_GFX.h>
#include "ascii_golem.h"

// Create an instance of the graphics library
// 256*240
ESP_8_BIT_GFX videoOut(true /* = NTSC */, 8 /* = RGB332 color */);
int screen;

void setup() {
  // Initial setup of graphics library
  videoOut.begin();
  videoOut.fillScreen(0);
  videoOut.waitForFrame();

  screen = 1;
}

void static_noise() {
  for (int x=0; x<videoOut.width(); x++) {
    for (int y=0; y<videoOut.height(); y++) {
      uint8_t val = esp_random();
      videoOut.drawPixel(x, y, val);
    }
  }
}

void golem_1() {
  videoOut.drawGrayscaleBitmap(0, 0, (const uint8_t *)ascii_golem_bitmap, videoOut.width(), videoOut.height());
}

void loop() {
  // Wait for the next frame to minimize chance of visible tearing
  videoOut.waitForFrame();
  // Clear screen
  videoOut.fillScreen(0);

  switch (screen) {
    case 0:
      static_noise();
      break;
    case 1:
      golem_1();
      break;
  };

  // // Get a random x value from 0 to the width of the screen
  // int x = esp_random() % videoOut.width();
  // // // Get a random y value from 0 to the height of the screen
  // int y = esp_random() % videoOut.height();

  // videoOut.drawGrayscaleBitmap(0, 0, static_noise_array, videoOut.width(), videoOut.height());

  // Draw text in the middle of the screen
  // videoOut.setCursor(25, 80);
  // videoOut.setTextColor(0xFF);
  // videoOut.setTextSize(2);
  // videoOut.setTextWrap(false);
  // videoOut.print("Boldog karacsonyt!");
}

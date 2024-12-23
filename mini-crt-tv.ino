#include <AnimatedGIF.h>
#include <ESP_8_BIT_GFX.h>
#include "ascii_golem.h"
#include "golem_anim.h"

// Create an instance of the graphics library
// 256*240
ESP_8_BIT_GFX videoOut(true /* = NTSC */, 8 /* = RGB332 color */);
AnimatedGIF gif;
int screen;

// Vertical margin to compensate for aspect ratio
const int gif_vertical_margin = 56;

// Draw a line of image to ESP_8_BIT_GFX frame buffer
void GIFDraw(GIFDRAW *pDraw)
{
    uint8_t *s;
    uint16_t *d, *usPalette, usTemp[320];
    int x, y;

    usPalette = pDraw->pPalette;
    y = pDraw->iY + pDraw->y; // current line

    s = pDraw->pPixels;
    if (pDraw->ucDisposalMethod == 2) // restore to background color
    {
      for (x=0; x<pDraw->iWidth; x++)
      {
        if (s[x] == pDraw->ucTransparent)
           s[x] = pDraw->ucBackground;
      }
      pDraw->ucHasTransparency = 0;
    }
    // Apply the new pixels to the main image
    if (pDraw->ucHasTransparency) // if transparency used
    {
      uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
      int x, iCount;
      pEnd = s + pDraw->iWidth;
      x = 0;
      iCount = 0; // count non-transparent pixels
      while(x < pDraw->iWidth)
      {
        c = ucTransparent-1;
        d = usTemp;
        while (c != ucTransparent && s < pEnd)
        {
          c = *s++;
          if (c == ucTransparent) // done, stop
          {
            s--; // back up to treat it like transparent
          }
          else // opaque
          {
             *d++ = usPalette[c];
             iCount++;
          }
        } // while looking for opaque pixels
        if (iCount) // any opaque pixels?
        {
          for(int xOffset = 0; xOffset < iCount; xOffset++ ){
            videoOut.drawPixel(pDraw->iX + x + xOffset, gif_vertical_margin + y, usTemp[xOffset]);
          }
          x += iCount;
          iCount = 0;
        }
        // no, look for a run of transparent pixels
        c = ucTransparent;
        while (c == ucTransparent && s < pEnd)
        {
          c = *s++;
          if (c == ucTransparent)
             iCount++;
          else
             s--;
        }
        if (iCount)
        {
          x += iCount; // skip these
          iCount = 0;
        }
      }
    }
    else
    {
      s = pDraw->pPixels;
      // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
      for (x=0; x<pDraw->iWidth; x++)
      {
        // videoOut.drawPixel(x,vertical_margin + y, usPalette[*s++]);
        videoOut.drawPixel(x,gif_vertical_margin + y, *s++);
      }
    }
} /* GIFDraw() */

void setup() {

  Serial.begin(115200);

  // Initial setup of graphics library
  videoOut.begin();
  videoOut.copyAfterSwap = true; // gif library depends on data from previous buffer
  videoOut.fillScreen(0);
  videoOut.waitForFrame();


  gif.begin(LITTLE_ENDIAN_PIXELS);
  screen = 2;
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

void golem_anim() {
  if (gif.open((uint8_t *)golem_gif, golem_gif_len, GIFDraw))
  {
    while (gif.playFrame(true, NULL))
    {
      videoOut.waitForFrame();
    }
    videoOut.waitForFrame();
    gif.close();
  }
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
    case 2:
      golem_anim();
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

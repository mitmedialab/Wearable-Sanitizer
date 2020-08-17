/*
 * When you double tap, the next neopixel turns on. This should be working in any mode. 
 * 
 */

#include <Adafruit_CircuitPlayground.h>
#define CLICKTHRESHHOLD 120 //for 8G, try 10-20. for 4G try 20-40. for 2G try 40-80


uint8_t tapCounter = 0;       
uint8_t prevTapCounter = -1;

void setup() {
  CircuitPlayground.begin();
  CircuitPlayground.clearPixels();

  CircuitPlayground.setAccelRange(LIS3DH_RANGE_2_G);
  CircuitPlayground.setAccelTap(2, CLICKTHRESHHOLD);
  
  // have a procedure called when a double tap is detected
  attachInterrupt(digitalPinToInterrupt(CPLAY_LIS3DH_INTERRUPT), onDoubleTap, FALLING);
}

void onDoubleTap(){
  tapCounter++;
  if(tapCounter > 4) tapCounter = 0;
}

void loop() {
  if(tapCounter != prevTapCounter){
    CircuitPlayground.clearPixels();
    CircuitPlayground.setBrightness(2);
    for(int i=0; i<=tapCounter; i++)
      CircuitPlayground.setPixelColor(i,101,101,101);
    prevTapCounter = tapCounter;
  }
}

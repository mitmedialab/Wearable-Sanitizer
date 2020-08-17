/*
 * When you press the button, change the mode and indicate this by lighting the corresponding neopixel. 
 * If the mode has not changed between loop iterations, then don't do anything. 
 * 
 *  
 *  ##############################################################
 *  Each  mode value should be a separate state in a state machine.
 *  On the other hand, flowRate should be just a global variable.
 */

#include <Adafruit_CircuitPlayground.h>


enum operatingMode : uint8_t{
  LED0 = 0, LED1, LED2, LED3, LED4
};

operatingMode mode = LED0;
operatingMode prevMode = LED1;

bool buttonState = 0;         // current state of the button
bool prevButtonState = 0;     // previous state of the button

void setup() {
  CircuitPlayground.begin();
  CircuitPlayground.clearPixels();
}

void loop() {
  if(mode != prevMode){ //if there is no mode change, don't execute this block.
    switch(mode){
      case LED0:
        CircuitPlayground.clearPixels();
        CircuitPlayground.setPixelColor(0, 101,101,101);
        break;
      case LED1:
        CircuitPlayground.clearPixels();
        CircuitPlayground.setPixelColor(1, 101,101,101);
        break;
      case LED2:
        CircuitPlayground.clearPixels();
        CircuitPlayground.setPixelColor(2, 101,101,101);
        break;
      case LED3:
        CircuitPlayground.clearPixels();
        CircuitPlayground.setPixelColor(3, 101,101,101);
        break;
      case LED4:
        CircuitPlayground.clearPixels();
        CircuitPlayground.setPixelColor(4, 101,101,101);
        break;
    }
    prevMode = mode;
  }

  buttonState = CircuitPlayground.leftButton();
  if(buttonState != prevButtonState){ //if buttonstate has changed.
    if(buttonState == HIGH)  //and if it is now pressed.
      mode = (operatingMode) ((uint8_t) mode + 1);
      if(mode>LED4) mode=LED0;
      delay(50); //debounce
  }
  prevButtonState = buttonState;
}

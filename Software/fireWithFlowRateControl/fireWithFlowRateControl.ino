/* This combines 'fire' + 'DoubleTap'
 * When you press the fire button, have the LEDs trace out a circle, and then
 * trigger the spraying. While the spraying is happening the LEDs
 * flash in red at high frequency.
 * 
 */
#include <Adafruit_CircuitPlayground.h>

#define pumpPin 3
#define in2 6 //this will be gnd
 
bool fireButtonState = 0;
bool prevFireButtonState = -1;
uint8_t dutyCycles[5] = {50,100,150,200,255}; //These are the duty cycles for the 5 different spray settings.

#define CLICKTHRESHHOLD 120 //for 8G, try 10-20. for 4G try 20-40. for 2G try 40-80


uint8_t tapCounter = 0;       
uint8_t prevTapCounter = -1;

void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();
  CircuitPlayground.clearPixels();
  
  pinMode(pumpPin,OUTPUT);
  pinMode(in2,OUTPUT);
  digitalWrite(in2,LOW);

  CircuitPlayground.setAccelRange(LIS3DH_RANGE_2_G);
  CircuitPlayground.setAccelTap(2, CLICKTHRESHHOLD);

  attachInterrupt(digitalPinToInterrupt(CPLAY_LIS3DH_INTERRUPT), onDoubleTap, FALLING);

}

void onDoubleTap(){
  tapCounter++;
  if(tapCounter > 4) tapCounter = 0;
}

void loop() {
  //If the button is pressed, then start LED circle.
  fireButtonState = CircuitPlayground.rightButton();
  if(fireButtonState != prevFireButtonState){ //if buttonstate has changed.
    if(fireButtonState == HIGH){  //and if it is now pressed.
          makeCircle(1,dutyCycles[tapCounter]); //The second argument is set to this just for debugging purposes, and will be different in the official version.
          spray(dutyCycles[tapCounter], 500);
          delay(50); //debounce
    }
  }
  prevFireButtonState = fireButtonState;

 //If the counter has CHANGED, indicate it on the LEDs
 if(tapCounter != prevTapCounter){
    CircuitPlayground.clearPixels();
    CircuitPlayground.setBrightness(2);
    for(int i=0; i<=tapCounter; i++)
      CircuitPlayground.setPixelColor(i,101,101,101);
    prevTapCounter = tapCounter;
  }
}

//####################################---FUNCTIONS---#############################

void startSpray(uint8_t dutyCycle){
  analogWrite(pumpPin, dutyCycle);
}
void stopSpray(){
  digitalWrite(pumpPin,LOW);
}
void spray(uint8_t dutyCycle, int duration){
  int startTime = millis();
  bool spraying=false;
  while(millis() < startTime + duration){
      if(!spraying){
        analogWrite(pumpPin, dutyCycle);
        spraying=true;
      }     
      setPixelsRed(10);
      delay(15);
      setPixelsOff();
      delay(15);
  }
  digitalWrite(pumpPin,LOW);
}

void setPixelsRed(uint8_t brightness){
  CircuitPlayground.setBrightness(brightness);
  for(int i=0; i<10; i++){
    CircuitPlayground.setPixelColor(i, 255,1,1);
  }
}

void setPixelsOff(){
  CircuitPlayground.clearPixels();
}

void makeCircle(uint8_t brightness, uint8_t delayTime){ //the argument is the delay b/n LEDs.
  CircuitPlayground.setBrightness(brightness);
  for(int i=0; i<10; i++){
    CircuitPlayground.setPixelColor(i, rnd(),rnd(),rnd());
    delay(delayTime);
  }
  delay(2*delayTime);
  CircuitPlayground.clearPixels();
}

int rnd(){
  //returns a random value between 0 and 255
  return rand() % 255;
}

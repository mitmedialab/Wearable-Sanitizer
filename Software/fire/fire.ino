/*
 * When you press the fire button, have the LEDs trace out a circle, and then
 * trigger the spraying. While the spraying is happening the LEDs
 * flash in red at high frequency.
 * 
 */
#include <Adafruit_CircuitPlayground.h>

//You must not use pins 4 and 5 (buttons).
#define pumpPin A1
#define in2 A6 //this will be gnd
 
bool fireButtonState = 0;
bool prevFireButtonState = -1;

void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();
  CircuitPlayground.clearPixels();
  
  pinMode(pumpPin,OUTPUT);
  pinMode(in2,OUTPUT);
  digitalWrite(in2,LOW);

  

  makeCircle(1,70);
}

void loop() {
  //If the button is pressed, then start LED circle.
  fireButtonState = CircuitPlayground.rightButton();
  if(fireButtonState != prevFireButtonState){ //if buttonstate has changed.
    Serial.println("DD");
    if(fireButtonState == HIGH){  //and if it is now pressed.
          makeCircle(1,70);
          Serial.println("HE");
          spray(255, 500);
          delay(50); //debounce
    }
  }
  prevFireButtonState = fireButtonState;  
}

void startSpray(uint8_t dutyCycle){
  analogWrite(pumpPin, dutyCycle);
}
void stopSpray(){
  digitalWrite(pumpPin,LOW);
}
void spray(uint8_t dutyCycle, int duration){
  analogWrite(pumpPin, dutyCycle);
  delay(duration);
  digitalWrite(pumpPin, LOW);
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


void makeCircle(uint8_t brightness, uint8_t d){ //the argument is the delay b/n LEDs.
  CircuitPlayground.setBrightness(brightness);
  for(int i=0; i<10; i++){
    CircuitPlayground.setPixelColor(i, rnd(),rnd(),rnd());
    delay(d);
  }
  delay(2*d);
  CircuitPlayground.clearPixels();
}

int rnd(){
  //returns a random value between 0 and 255
  return rand() % 255;
}

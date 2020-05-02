/*
 * If the device has been steady for (SAMPLES * SAMPLING_DELAY) millisecondssecond, fire.
 */
#define SAMPLES 10 //the number of samples over which we will average.
#define SAMPLING_DELAY 50 //time between the consecutive SAMPLES in ms.
#define AIM_THRESHOLD 0.2
int acquisitionTime = 0; //updates whenever a new sample is obtained.

float xOld[SAMPLES], xNew[SAMPLES]; 
float yOld[SAMPLES], yNew[SAMPLES];
float zOld[SAMPLES], zNew[SAMPLES];
uint8_t iSample=0; //index
float xAvgOld, xAvgNew;
float yAvgOld, yAvgNew;
float zAvgOld, zAvgNew;

#include <Adafruit_CircuitPlayground.h>
#define CLICKTHRESHHOLD 120 //for 8G, try 10-20. for 4G try 20-40. for 2G try 40-80

void setup() {
  CircuitPlayground.begin();
  CircuitPlayground.clearPixels();
  CircuitPlayground.setAccelRange(LIS3DH_RANGE_2_G);
}

void loop() {
  //Algorithm at High-Level:
  //We are going to compare the average of the last 10samples to the average of the previous 10samples, and if the
  //difference between the averages is less than some threshold, we say that the device is in aiming mode. We will acquire
  //one nwe sample every SAMPLING_DELAY milliseconds. We must do this analysis after each new sample; we do NOT want to 
  //have to take in all samples at once.
  
  //Algorithm Implementation:
  //There are two arrays of size SAMPLES, where xNew stores last 10 new samples and xOld sotres the previous 10 samples.
  //Every 100ms, an item at index i is tranferred from the new array to the old array at index i, and then a new sample is placed into the new
  //array at that same index i. The index is then incremented until it reaches SAMPLES, at which point it is reset to 0. 
  
  if(millis()-acquisitionTime >= SAMPLING_DELAY){
    //Get a new Sammple:
    float xSumOld = 0, xSumNew = 0, ySumOld = 0, ySumNew = 0, zSumOld = 0, zSumNew = 0;
    acquisitionTime = millis();
    xOld[iSample]=xNew[iSample];
    xNew[iSample]=CircuitPlayground.motionX();
    yOld[iSample]=yNew[iSample];
    yNew[iSample]=CircuitPlayground.motionY();
    zOld[iSample]=zNew[iSample];
    zNew[iSample]=CircuitPlayground.motionZ();
    iSample++;
    if(iSample==SAMPLES) iSample=0;
  
    //Calculate the averages
    for(int i=0; i<9; i++){
      xSumOld += xOld[i];
      xSumNew += xNew[i];
      ySumOld += yOld[i];
      ySumNew += yNew[i];
      zSumOld += zOld[i];
      zSumNew += zNew[i];
    }
    xAvgOld = xSumOld/10.0f;
    xAvgNew = xSumNew/10.0f;
    yAvgOld = ySumOld/10.0f;
    yAvgNew = ySumNew/10.0f;
    zAvgOld = zSumOld/10.0f;
    zAvgNew = zSumNew/10.0f;
  
    //If the two averages are different by more than AIM_THRESHOLD, then don't do enything. Else, flash LEDs.
    if(abs(xAvgNew - xAvgOld) < AIM_THRESHOLD && abs(yAvgNew - yAvgOld) < AIM_THRESHOLD && abs(zAvgNew - zAvgOld) < AIM_THRESHOLD){
      Serial.println("Aiming");
      Serial.println(abs(xAvgNew - xAvgOld));
      makeCircle(1,30);
    }
  }
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

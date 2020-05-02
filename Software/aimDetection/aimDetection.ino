/*
 * 1. If the device has been steady for 1 second, fire
 * 2. Have a more clever aiming algorithm to distringuish between mo notion vs actual aiming.
 *  If the motion just dies out gradually, it is not because of aiming. But if it suddenly
 *  comes to a stop, then aiming is very likely happening.
 *  
 *  TODO: If it sprays once, it should not be able to spray again unless it has been shaken.
 *  TODO: If the spraying has occurred 
 * 
 */
#define SAMPLES 10 //the number of samples over which we will average.
#define SAMPLING_DELAY 50 //time between the consecutive SAMPLES in ms.
#define AIM_THRESHOLD 0.2
int acquisitionTime = 0; //updates whenver a new sample is obtained.

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

  //SImmple Algorithm:
  //store the last 10 values of x in an array. Take the average accelerations over 1 second, one measurement
  //every 100ms. If the last two average values are the samme, then assume that aiming is happening.
  //But I don't want to have to store 10 values for a whole second before I know the next one. Instead,
  //suppose I have an array of 20 items and I keep adding adding new readings to it while replacing the old ones, 
  //one reading every 100ms. Then I average the last 10 values and the latest 10 values and compare them. 
  //How do I shift the items in the array by 1. 
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
  
    //If the two averages are different by more than XX, then don't do enything. Else, flash LEDs.
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

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
float x[20],y[20],z[20];
float xSumOld,xSumNew, xAvgOld, xAvgNew;
float ySumOld,ySumNew, yAvgOld, yAvgNew;
float zSumOld,zSumNew, zAvgOld, zAvgNew;

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

  xSumOld = 0;
  xSumNew = 0;
  for(int i=19; i>0; i--){
    x[i]=x[i-1]; //shift each element by 1 position
    (i>9) ? xSumOld+=x[i] : xSumNew+=x[i]; //get the sums of the old 10 and new 9 values.
  }
  x[0] = CircuitPlayground.motionX();  
  xSumNew+=x[0]; //Add the 10th value of the new sum.
  xAvgOld = xSumOld/10.0f;
  xAvgNew = xSumNew/10.0f;
/////////////////////////////////////////////
  ySumOld = 0;
  ySumNew = 0;
  for(int i=19; i>0; i--){
    y[i]=y[i-1]; //shift each element by 1 position
    (i>9) ? ySumOld+=y[i] : ySumNew+=y[i]; //get the sums of the old 10 and new 9 values.
  }
  y[0] = CircuitPlayground.motionY();  
  ySumNew+=y[0]; //Add the 10th value of the new sum.
  yAvgOld = ySumOld/10.0f;
  yAvgNew = ySumNew/10.0f;
/////////////////////////////////////////////////////////////
  zSumOld = 0;
  zSumNew = 0;
  for(int i=19; i>0; i--){
    z[i]=z[i-1]; //shift each element by 1 position
    (i>9) ? zSumOld+=z[i] : zSumNew+=z[i]; //get the sums of the old 10 and new 9 values.
  }
  z[0] = CircuitPlayground.motionZ();  
  zSumNew+=z[0]; //Add the 10th value of the new sum.
  zAvgOld = zSumOld/10.0f;
  zAvgNew = zSumNew/10.0f;

  //Calculate the average of the first 10 items, and the average of the last 10 items.
  //If the two averages are different by more than XX, then don't do enything. Else, flash LEDs.

  //oldAverage
  if(abs(xAvgNew - xAvgOld) < 0.3 && abs(yAvgNew - yAvgOld) < 0.3 && abs(zAvgNew - zAvgOld) < 0.3){
    Serial.println("Aiming");
    makeCircle(1,40);
  }
  
  delay(100);
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

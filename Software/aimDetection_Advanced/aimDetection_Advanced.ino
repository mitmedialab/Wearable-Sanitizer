/*
 * 1. If the device has been steady for 1 second, then Fire.
 * 2. If there has seen no motion for 7 seconds, then enter low power mode and stay there until a button press. 
 *  This is to detect when the device is not being worn, and also when the user is simply resting. 
 *      
 *
 * TODO: Have a more clever aiming algorithm to distringuish between no notion vs actual aiming.
 *  If the motion just dies out gradually, that should not be considered aiming. But if it suddenly
 *  comes to a stop, then aiming is very likely happening.  
 *  
 * TODO: Shifting all array elements every iteration of the loop is very inefficient. I can do better 
 *    by having 2 arrays and tranferring one element from the new array to the old array at the original
 *    index value. And then putting the next reading into the new array.
 * 
 */
float x[20],y[20],z[20];
float xSumOld,xSumNew, xAvgOld, xAvgNew;
float ySumOld,ySumNew, yAvgOld, yAvgNew;
float zSumOld,zSumNew, zAvgOld, zAvgNew;

#include <Adafruit_CircuitPlayground.h>
#define CLICKTHRESHHOLD 120 //for 8G, try 10-20. for 4G try 20-40. for 2G try 40-80

#define NOMOTION_THRESHOLD 0.15
#define AIMING_THRESHOLD 0.4
#define LOWPOWERMODE_THRESHOLD 0.3
#define TIMETILLSLEEP 7000
int lowPowerModeTimerStarterAt = 0;

void setup() {
  CircuitPlayground.begin();
  CircuitPlayground.clearPixels();
  CircuitPlayground.setAccelRange(LIS3DH_RANGE_2_G);
}

void loop() {
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
  //If the two averages are different by more than AIMING_THRESHOLD, then there is too much motion and the user
  //is not intending to shoot. If value is less than NOMOTION_THRESHOLD then device is too motionless and thus 
  //probably not being worn. If in the range NOMOTION_THRESHOLD and AIMING_THRESHOLD then user is aiming. 
  //TODO: These thresholds must be adjusted after being tested on the actual device. This must be
  //  tested when no cable is connected to the USB because it exerts forces

  if(abs(xAvgNew - xAvgOld) < NOMOTION_THRESHOLD && abs(yAvgNew - yAvgOld) < NOMOTION_THRESHOLD && abs(zAvgNew - zAvgOld) < NOMOTION_THRESHOLD){
    Serial.println("No Motion");
  }
  else if(abs(xAvgNew - xAvgOld) < AIMING_THRESHOLD && abs(yAvgNew - yAvgOld) < AIMING_THRESHOLD && abs(zAvgNew - zAvgOld) < AIMING_THRESHOLD){
    Serial.println("Aiming");
    makeCircle(1,40);
  }

  //If the the LOWPOWERMODE_THRESHOLD has not been **EXCEEDED** for more than 7 seconds, then assume the device or the 
  //user is at rest, and enter sleep mode. To resume from sleep mode where you are no longer reading the
  //accelerometer. You will only be monitoring the buttons at this poing. To exit sleep mode, the user would
  //have to push the big button.
  if(LOWPOWERMODE_THRESHOLD > (abs(xAvgNew - xAvgOld) + abs(yAvgNew - yAvgOld) + abs(zAvgNew - zAvgOld))/3.0f){
    if(lowPowerModeTimerStarterAt > 0){
      Serial.println(millis() - lowPowerModeTimerStarterAt);
      if(millis() - lowPowerModeTimerStarterAt > TIMETILLSLEEP){
        lowPowerModeTimerStarterAt = 0; //reset the timer. This is critical for when we resume from low power mode.
        enterLowPowerMode();
      }
    }
    else lowPowerModeTimerStarterAt = millis();
  }
  else lowPowerModeTimerStarterAt = 0;
  
//  pastThresholds[iT++] = (abs(xAvgNew - xAvgOld) + abs(yAvgNew - yAvgOld) + abs(zAvgNew - zAvgOld))/3.0f;
//  if(iT==70) iT=0;
  delay(100);
}

void enterLowPowerMode(){
  Serial.println("LOW POWER MODE");
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,HIGH);
  
  while(CircuitPlayground.leftButton()==LOW && CircuitPlayground.rightButton()==LOW){
    delay(10);
  }
  
  digitalWrite(LED_BUILTIN,LOW);
  Serial.println("EXIT LOW POWER MODE");
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

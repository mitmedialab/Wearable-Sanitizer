/*
 Description
  If the device has been steady but not too still for (SAMPLES * SAMPLING_DELAY) millisecondssecond, start firing.
  If there has been no motion for 7 seconds, then enter low power mode and stay there until a button press. 
    (This is to detect when the device is not being worn, and also when the user is simply resting.)
 
 Algorithm at High-Level:
  We are going to compare the average of the last 10 samples to the average of the previous 10 samples. 
  If the difference between the averages is less than NOMOTION_THRESHOLD, the device is not being worn, or the
  wearer is sleeping. Else, if the difference is greater than AIMING_THRESHOLD, then the wearer is just walking.
  Only if the the difference is between those two thresholds, the wearer is aiming, and we start shooting.
  If the device has been staying still for TIMETILLSLEEP milliseconds, we are going to put it to sleep, and the 
  user would have to push a button to wake it up.

  
 Algorithm Implementation:
  We will acquire one new sample every SAMPLING_DELAY milliseconds. We must do this analysis after each new sample; we do NOT want to have to
  take in all samples at once. There are two arrays of size SAMPLES, where xNew stores last 10 new samples and xOld sotres the previous 10 samples.
  Every 100ms, an item at index i is tranferred from the new array to the old array at index i, and then a new sample is placed into the new
  array at that same index i. The index is then incremented until it reaches SAMPLES, at which point it is reset to 0. To enter low-power-mode, we
  are going to monitor if the threshold has been exceeded over a period of 7 seconds. There is no averaging involved in this part. 

 TODO: Have a more clever aiming algorithm to distringuish between no notion vs actual aiming.
   If the motion just dies out gradually, that should not be considered aiming. But if it suddenly
   comes to a stop, then aiming is very likely happening.  
 TODO: These thresholds must be adjusted after being tested on the actual device. This must be
   tested when no cable is connected to the USB because it exerts forces
 */

#include <Adafruit_CircuitPlayground.h>

#define SAMPLES 10 //the number of samples over which we will average.
#define SAMPLING_DELAY 50 //time between the consecutive SAMPLES in ms.
int acquisitionTime = 0; //updates whenver a new sample is obtained.

float xOld[SAMPLES], xNew[SAMPLES]; 
float yOld[SAMPLES], yNew[SAMPLES];
float zOld[SAMPLES], zNew[SAMPLES];
uint8_t iSample=0; //index
float xAvgOld, xAvgNew;
float yAvgOld, yAvgNew;
float zAvgOld, zAvgNew;

#define NOMOTION_THRESHOLD 0.15
#define AIMING_THRESHOLD 0.3
#define LOWPOWERMODE_THRESHOLD 0.3
#define TIMETILLSLEEP 7000
int lowPowerModeTimerStarterAt = 0;

void setup() {
  CircuitPlayground.begin();
  CircuitPlayground.clearPixels();
  CircuitPlayground.setAccelRange(LIS3DH_RANGE_2_G);
}

void loop() {
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

    //Analyze the averages
    if(abs(xAvgNew - xAvgOld) < NOMOTION_THRESHOLD && abs(yAvgNew - yAvgOld) < NOMOTION_THRESHOLD && abs(zAvgNew - zAvgOld) < NOMOTION_THRESHOLD){
      Serial.println("No Motion");
    }
    else if(abs(xAvgNew - xAvgOld) < AIMING_THRESHOLD && abs(yAvgNew - yAvgOld) < AIMING_THRESHOLD && abs(zAvgNew - zAvgOld) < AIMING_THRESHOLD){
      Serial.println("Aiming");
      makeCircle(1,40);
    }

    //If the the LOWPOWERMODE_THRESHOLD has not been **EXCEEDED** for more than 7 seconds, then assume the device or the 
    //user is at rest, and enter sleep mode where only the buttons would be active. To wake up, the user must push the big button.
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
  }
}

//############################################---FUNCTIONS---#####################################

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

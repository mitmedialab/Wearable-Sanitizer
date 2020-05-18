///////////////////////////////////////////////////////////////////////////////
// Circuit Playground Bike Light - Rainbow Chaser
//
// Author: Carter Nelson
// MIT License (https://opensource.org/licenses/MIT)
 
#include <Adafruit_CircuitPlayground.h>
#include <Wire.h>
 
#define ROTATION_RATE   50;    // lower is faster
int in1 = A1; //pump pin
int in2 = A6; //pump pin

//distance sensor

unsigned char ok_flag;
unsigned char fail_flag;

unsigned short lenth_val = 0;
unsigned char i2c_rx_buf[16];
unsigned char dirsend_flag=0;

//button

bool leftButtonPressed;
bool rightButtonPressed;
int mode = 0;

///////////////////////////////////////////////////////////////////////////////
void setup() {
  CircuitPlayground.begin();
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  Wire.begin(); 
  Serial.begin(9600,SERIAL_8N1); 
  CircuitPlayground.setBrightness(200);
}
 
 
///////////////////////////////////////////////////////////////////////////////
void loop() {

   int x=ReadDistance();
   Serial.print(x);
   Serial.println(" mm");
  
  CircuitPlayground.clearPixels();
  button_check();
}

void mode_update()
  {
    if (mode == 3)
       {mode = 0;}
    else{mode++;}

    Serial.println(mode);
  }

void button_check()
{
  leftButtonPressed = CircuitPlayground.leftButton();
  rightButtonPressed = CircuitPlayground.rightButton();

  //Serial.print("Left Button: ");
  if (leftButtonPressed) {
    Serial.println("Left DOWN");
    mode_update();
    delay(600);
    } 
    else {
      //Serial.print("  UP");
      }
  //Serial.print("   Right Button: ");
  if (rightButtonPressed) {
    Serial.println("Right DOWN");
    shoot_1();
    delay(10);
    } 
    else {
      //Serial.print("  UP");
    }
  }


  
//Mode 1
void shoot_1()
{
  for (int i =0; i < 3; i++)    {
    rainbowCycle(0);
      digitalWrite(in2, LOW);
      digitalWrite(in1, HIGH);
      delay(120);
      digitalWrite(in2, LOW);
      digitalWrite(in1, LOW);
      delay(300);
    }
  
  
  }




//Color
void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<10; i++) {
      CircuitPlayground.setPixelColor(i, Wheel(i+j));
    }
    delay(wait);
  }
}

void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*1; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< 10; i++) {
      CircuitPlayground.setPixelColor(i, Wheel(((i * 256 / 10) + j) & 255));
    }
    delay(wait);
  }
}

uint32_t Wheel(byte WheelPos) {
int R = 0;
int G = 0;
int B = 0;

WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    R =  255 - WheelPos * 3;
    G =  0;
    B =  WheelPos * 3;
    long RGB = (R << 16) | (G << 8) | B;
    return RGB;
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
     R =  0;
     G = WheelPos * 3;
     B = 255 - WheelPos * 3;
     
     long RGB = (R << 16) | (G << 8) | B;
     return RGB;
  }
  WheelPos -= 170;
  R = WheelPos * 3;
  G = 255 - WheelPos * 3;
  B = 0;
  
  long RGB = (R << 16) | (G << 8) | B;
  return RGB;
  
}


//distancce sensor
int serial_putc( char c, struct __file * )
{
  Serial.write( c );
  return c;
}



void SensorRead(unsigned char addr,unsigned char* datbuf,unsigned char cnt) 
{
  unsigned short result=0;
  // step 1: instruct sensor to read echoes
  Wire.beginTransmission(82); // transmit to device #82 (0x52)
  // the address specified in the datasheet is 164 (0xa4)
  // but i2c adressing uses the high 7 bits so it's 82
  Wire.write(byte(addr));      // sets distance data address (addr)
  Wire.endTransmission();      // stop transmitting
  // step 2: wait for readings to happen
  delay(1);                   // datasheet suggests at least 30uS
  // step 3: request reading from sensor
  Wire.requestFrom(82, cnt);    // request cnt bytes from slave device #82 (0x52)
  // step 5: receive reading from sensor
  if (cnt <= Wire.available()) { // if two bytes were received
    *datbuf++ = Wire.read();  // receive high byte (overwrites previous reading)
    *datbuf++ = Wire.read(); // receive low byte as lower 8 bits
  }
}

int ReadDistance(){
    SensorRead(0x00,i2c_rx_buf,2);
    lenth_val=i2c_rx_buf[0];
    lenth_val=lenth_val<<8;
    lenth_val|=i2c_rx_buf[1];
    delay(300); 
    return lenth_val;
}

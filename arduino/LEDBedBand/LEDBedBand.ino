#include <LowPower.h>


const int PIN_PIR_RIGHT = 2;
const int PIN_PIR_LEFT = 3;

const int PIN_LED_RED = 6;
const int PIN_LED_GREEN = 5;
const int PIN_LED_BLUE = 9;

const int PIN_VM = 2;


void setup() {
  Serial.begin(9600);
  
  //Activate interrupt in pins
  pinMode(PIN_PIR_RIGHT, INPUT);
  pinMode(PIN_PIR_LEFT, INPUT);
  
  //LED
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);
  pinMode(PIN_LED_BLUE, OUTPUT);
  
  //Activate voltage measurement
  pinMode(PIN_VM, INPUT);
  
  //Check colors
  makeLight(1,0,0);
  delay(500);
  makeLight(0,1,0);
  delay(500);
  makeLight(0,0,1);
  delay(500);
  makeLight(1,1,1);
  delay(500);
  makeLight(0,0,0);  
}

// ISR to detect motion. Called by both PIR via external interrupt
void interrupt() {
  //Nothing to do here. We just want to wake up the Arduino
}

void loop() {

  if(isMotionDetected() && isDark()) 
  {
    turnOnLight(); 
    delay(500);
  }
  else
  {
    turnOffLight();
    goToSleepMode();
  }

}

void blinkLED () {
    digitalWrite(13, HIGH);
    delay(200);
    digitalWrite(13, LOW);  
}

//Power down Arduino, Attach external interrupt
void goToSleepMode() {
    attachInterrupt(0 /*PIN 2*/, interrupt , RISING);  
    attachInterrupt(1 /*PIN 3*/, interrupt , RISING); 
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
    detachInterrupt(0 /*PIN 2*/);
    detachInterrupt(1 /*PIN 3*/);  
}

/* Check if one of the both PIR detected motion */
bool isMotionDetected() {
  return digitalRead(PIN_PIR_RIGHT) == HIGH |  digitalRead(PIN_PIR_LEFT) == HIGH;
}

/* Check if it is dark. Higher values -> darker. Range 0-1023 */
bool isDark() {
  return analogRead(A0) > 980;
}

void turnOnLight() {
  unsigned long color = 0xffffff;
  const float voltage = measureVCC();

  if(voltage > (8*1.30))   //Good condition
    color = 0x00ff00;
  else if(voltage < (8*1.10))  //Should be recharged
    color = 0xff0000;  
    
  makeLight(color, 0.01);
}

void turnOffLight() {
  makeLight(0x000000, 0);
}

void makeLight(const unsigned long color, const float brightness) {
  const unsigned char red = (color >> 16);
  const unsigned char green = (color >> 8) & 0xff;
  const unsigned char blue = color & 0xff;  
  makeLight(red, green, blue, brightness);
}

void makeLight(const unsigned char red, const unsigned char green, const unsigned char blue, const float brightness) {
  makeLight(red * brightness, green * brightness, blue * brightness);
}

void makeLight(const unsigned char red, const unsigned char green, const unsigned char blue) {
  analogWrite(PIN_LED_RED, red);
  analogWrite(PIN_LED_GREEN, green);
  analogWrite(PIN_LED_BLUE, blue); 
}

float measureVCC() {  
  const int voltageRaw = analogRead(PIN_VM);
  //A voltage divider is connected to PIN_VM
  //Vin = 12V, Vout = 4.68V, R1=1M, R2=680K
  return map(voltageRaw, 0, 1023, 0, 1200)/100.0; 
}

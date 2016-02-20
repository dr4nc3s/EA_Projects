#include <Wire.h>
#include <FDRV-04S.h>
#include <MOD1005.h>
#include <TMP275_MOD-1001.h>

#define ONES_WHEEL  1,  7
#define TENS_WHEEL  2,  6
#define BIG1_WHEEL  3,  5
#define BIG10_WHEEL 4,  4

const int delayBtwnTurns = 75;
const int resetDelay = 100;
const int debounceTime = 250;

void checkNine(int driver, int pinz, int& wheel) {
  if (digitalRead(pinz) != LOW) {
    Serial.print("Reel ");
    Serial.print(driver);
    Serial.println(" should be on 9, but it's not. Going to look for 9");
    while (digitalRead(pinz) == HIGH) {
      fdrv.startDriver(driver);
      delay(resetDelay);
    }
    //Serial.println("Found 9. Turning once to go to 0");
    //fdrv.startDriver(driver);
    wheel = 9;
  }
}

void stuckOnNine(int driver, int pinz, int& wheel) {
  int notNine = wheel;
  if (digitalRead(pinz) == LOW) {
    Serial.print("Reel ");
    Serial.print(driver);
    Serial.println(" should be on 0, but it's on 9. Resetting");
    resetWheel(driver, pinz, wheel);
  }
  //turnWheelTo(driver, pinz, wheel, notNine);
  
}

static int onesWheel, tensWheel, bigOnesWheel, bigTensWheel;
static int seconds, minutes, hours, minsPin, hrsPin;
static float celsius;

bool showTemp = false;
int tempDisplayTimer = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Serial.println("Welcome to the Noisy Clock and Temperature Sensor Project!");
  Serial.println("This project uses the FDRV-04S Soloenoid/Motor Driver and the MOD1001 RTC and Temp Sensor");
  Serial.println("Products can be purchased from Embedded Adventures at www.embeddedadventures.com");
  
  configWheels();

  mod1005.init();
  tmp275.init();
  tmp275.setResolution(4);
  tmp275.enableShutdownMode(true);
  hours = mod1005.getHours();
  minutes = mod1005.getMinutes();
  seconds = mod1005.getSeconds();

  pinMode(11, INPUT_PULLUP);      //Temp display
  //Set pin modes for time mod
  //pinMode(8, INPUT_PULLUP);       //
  pinMode(9, INPUT_PULLUP);       //Mins inc  
  pinMode(10, INPUT_PULLUP);      //Seconds inc
}

void loop() {
  if (digitalRead(11) == LOW) { //Display temperature
    tmp275.enableOS();
    showTemp = true;
  }
  else if (digitalRead(10) == LOW) {
    minutes = mod1005.getMinutes();
    delay(debounceTime);
    //if (digitalRead(10) == HIGH) {
      if (minutes == 59)
        mod1005.setMinutes(0);
      else
        mod1005.setMinutes(minutes + 1);
      syncWheelsToRTC();
    //}
  }
  else if (digitalRead(9) == LOW) {
    hours = mod1005.getHours();
    delay(debounceTime);
    //if (digitalRead(9) == HIGH) {
      if (hours == 23)
        mod1005.setHours(0);
      else
        mod1005.setHours(hours + 1);
      syncWheelsToRTC();
    //}
  }
  if (showTemp) 
    syncWheelsToTMP();
  else {
    if (seconds != mod1005.getSeconds()) {
      syncWheelsToRTC();
      //Serial.print("Pin 11 - ");
      //Serial.println(digitalRead(2));
      printTime();
    }
  }
}


void configWheels() {
  fdrv.init(0x1F);
  pinMode(7, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  //Setup the drivers
  fdrv.configPhase(1, 0x03FF, 25);
  fdrv.configPhase(2, 0, 0xFFFF);
  fdrv.configPhase(3, 0, 0);
  fdrv.setupDriver(1, 2);
  fdrv.setupDriver(2, 2);
  fdrv.setupDriver(3, 2);
  fdrv.setupDriver(4, 2);
  //Reset the wheels to 0
  resetWheel(ONES_WHEEL, onesWheel);
  resetWheel(TENS_WHEEL, tensWheel);
  resetWheel(BIG1_WHEEL, bigOnesWheel);
  resetWheel(BIG10_WHEEL, bigTensWheel);
}

/*  Resets the wheel to 0, and resets the variable keeping track of the wheel's digit */
void resetWheel(int driver, int pinz, int& wheel) {
  int timeout = 0;
  while ((digitalRead(pinz) == HIGH) && timeout < 10) {
    fdrv.startDriver(driver);
    timeout++;
    delay(resetDelay);
  }
  delay(resetDelay);
  fdrv.startDriver(driver); 
  wheel = 0;
}


void syncWheelsToRTC() {
  hours = mod1005.getHours();
  minutes = mod1005.getMinutes();
  seconds = mod1005.getSeconds();

  if (bigOnesWheel != (hours / 10))
    turnWheelTo(BIG10_WHEEL, bigTensWheel, (hours / 10));
  if (bigOnesWheel != (hours % 10))
    turnWheelTo(BIG1_WHEEL, bigOnesWheel, (hours % 10));
  if (tensWheel != (minutes / 10));
    turnWheelTo(TENS_WHEEL, tensWheel, (minutes / 10));
  if (onesWheel != (minutes % 10))
    turnWheelTo(ONES_WHEEL, onesWheel, (minutes % 10));
}

/*  Turns correspoing wheel to digit passed.
*  currentDigit is where the reel is right now
*  newDigit is the digit to turn to
*/
void turnWheelTo(int driver, int pinz, int& currentDigit, int newDigit) {
  int turns = ((10 - currentDigit) + newDigit) % 10;
  if (turns < 0)
    turns *= -1;
  for (int i = 0; i < turns; i++) {
    fdrv.startDriver(driver);
    delay(delayBtwnTurns);
  }
  currentDigit = newDigit;

  if (currentDigit == 9)
    checkNine(driver, pinz, currentDigit);
  else 
    stuckOnNine(driver, pinz, currentDigit);
}

void syncWheelsToTMP() {
  celsius = tmp275.getTemperature();
  Serial.print("Temperature: ");
  Serial.println(celsius);
  int cel = (int)(celsius * 100);
  turnWheelTo(BIG10_WHEEL, bigTensWheel, (cel / 1000));
  cel %= 1000;
  
  turnWheelTo(BIG1_WHEEL, bigOnesWheel, (cel / 100));
  cel %= 100;

  turnWheelTo(TENS_WHEEL, tensWheel, (cel / 10));

  turnWheelTo(ONES_WHEEL, onesWheel, (cel % 10));

  delay(1000);
  tempDisplayTimer++;
  if (tempDisplayTimer == 10) {
    showTemp = false;
    tempDisplayTimer = 0;
  }
}

void printTime() {
  Serial.print("RTC Time - ");
  if (hours < 10)
    Serial.print("0");
  Serial.print(hours);
  Serial.print(":");

  if (minutes < 10)
    Serial.print("0");
  Serial.print(minutes);
  Serial.print(":");

  if (seconds < 10)
    Serial.print("0");
  Serial.println(seconds);
}




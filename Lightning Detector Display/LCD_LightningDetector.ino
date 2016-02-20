#include <Wire.h>
#include <MOD1016.h>

#define IRQ_pin 7

volatile bool detected = false;

void setup() {
  Serial.begin(115200);
  //while (!Serial) {}
  Serial.println("Hello");
  Wire.begin();
  mod1016.init(IRQ_pin);
  Serial.println("Init done");
  //Tune Caps, Set AFE, Set Noise Floor
  autoTuneCaps(IRQ_pin);
  //mod1016.setTuneCaps(7);
  mod1016.setOutdoors();
  mod1016.setNoiseFloor(5);
  
  
  Serial.println("TUNE\tIN/OUT\tNOISEFLOOR");
  Serial.print(mod1016.getTuneCaps(), HEX);
  Serial.print("\t");
  Serial.print(mod1016.getAFE(), BIN);
  Serial.print("\t");
  Serial.println(mod1016.getNoiseFloor(), HEX);
  Serial.print("\n");

  pinMode(IRQ_pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(IRQ_pin), alert, RISING);
  //attachInterrupt(IRQ_pin, alert, RISING);
  Serial.println("after interrupt");
}

void loop() {
  if (detected) {
    translateIRQ(mod1016.getIRQ());
    detected = false;
  }
}

void alert() {
  detected = true;
}

void translateIRQ(uns8 irq) {
  switch(irq) {
      case 1:
        Serial.println("NOISE DETECTED");
        break;
      case 4:
        Serial.println("DISTURBER DETECTED");
        break;
      case 8: 
        Serial.println("LIGHTNING DETECTED");
        printDistance();
        break;
      default:
        //Serial.println("IDK");
        break;
    }
}

void printDistance() {
  int distance = mod1016.calculateDistance();
  if (distance == -1)
    Serial.println("Lightning out of range\n");
  else if (distance == 1)
    Serial.println("Distance not in table\n");
  else if (distance == 0)
    Serial.println("Lightning overhead\n");
  else {
    Serial.print("Lightning ~");
    Serial.print(distance);
    Serial.println("km away\n");  
  }
}


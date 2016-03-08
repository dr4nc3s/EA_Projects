#include <ESP8266WiFi.h>
#include <coap-packet.h>
#include <coap-queue.h>

CoapPacket      tx;
String          body;

const char*     ssid     = "whiteBox";
const char*     password = "apartment5g";

void setup() {
  Serial.begin(115200);
  coapQueue.begin();
  Serial.println("Generating coap packets - PING, GET, POST, PUT, DEL");
  connectWiFi();
  tx.begin();
}

void loop() {
  int packetSize = coapQueue.parseUDPPacket();
  if (packetSize) {
    int index = coapQueue.findRxSpace();
    Serial.println("Received something");
    if (index == 4) 
      Serial.println("No space in RX Queue");
    else {
      Serial.print("Bytes read: ");
      Serial.println(coapQueue.receivePacket(index));
      Serial.println(coapQueue.processRxPacket(index));
      coapQueue.printRxPacket(index);
      delay(5);
      autoReply(index);
    }
  }
}

void checkPackets() {
  autoReply(0);
  autoReply(1);
  autoReply(2);
  autoReply(3);
}

void autoReply(int index) {
  int x = coapQueue.needsReply(index);
  if (x == 1) {
    coapQueue.createReply(index);
    coapQueue.addPayload(index, 8, "RECEIVED");
    coapQueue.sendPacket(index);
  }
}

void printRXQueue() {
  int x = 0;
  Serial.println("\nRX QUEUE CONTENTS");
  Serial.println("\nIndex 0");
  x = coapQueue.printRxPacket(0);
  if (x == 0)
    Serial.println("Slot empty");
  Serial.println("\n\nIndex 1");
  x = coapQueue.printRxPacket(1);
  if (x == 0)
    Serial.println("Slot empty");
  Serial.println("\n\nIndex 2");
  x = coapQueue.printRxPacket(2);
  if (x == 0)
    Serial.println("Slot empty");
  Serial.println("\n\nIndex 3");
  x = coapQueue.printRxPacket(3);
  if (x == 0)
    Serial.println("Slot empty");
}

void printTXQueue() {
  int x = 0;
  Serial.println("\nTX QUEUE CONTENTS");
  Serial.println("\nIndex 0");
  x = coapQueue.printTxPacket(0);
  if (x == 0)
    Serial.println("Slot empty");
  Serial.println("\n\nIndex 1");
  x = coapQueue.printTxPacket(1);
  if (x == 0)
    Serial.println("Slot empty");
  Serial.println("\n\nIndex 2");
  x = coapQueue.printTxPacket(2);
  if (x == 0)
    Serial.println("Slot empty");
  Serial.println("\n\nIndex 3");
  x = coapQueue.printTxPacket(3);
  if (x == 0)
    Serial.println("Slot empty");
}

void parseQueue() {
  Serial.println("\nQUEUE CONTENTS");
  for (int i = 0; i < 4; i++) {
    Serial.print("\nRX Queue Index ");
    Serial.print(i);
    if (coapQueue.processRxPacket(i) == 0)
      Serial.println("!!! THIS SLOT IS EMPTY !!!");
    Serial.print("\nTX Queue Index ");
    Serial.print(i);
    if (coapQueue.processTxPacket(i) == 0)
      Serial.println("!!! THIS SLOT IS EMPTY !!!");
  }
}

void queueStatus() {
  int num = coapQueue.numEmptySpaces(0);
  Serial.print("Number of empty space in RXqueue: ");
  Serial.println(num);
  num = coapQueue.numFilledSpaces(0);
  Serial.print("Number of filled space in RXqueue: ");
  Serial.println(num);
  num = coapQueue.numEmptySpaces(1);
  Serial.print("Number of empty space in TXqueue: ");
  Serial.println(num);
  num = coapQueue.numFilledSpaces(1);
  Serial.print("Number of filled space in TXqueue: ");
  Serial.println(num);
}

void connectWiFi() {
  Serial.print("\nConnecting");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IPv4: ");
  Serial.println(WiFi.localIP());
}

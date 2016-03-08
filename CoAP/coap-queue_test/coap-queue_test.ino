#include <ESP8266WiFi.h>
#include <coap-packet.h>
#include <PacketParser.h>
#include <coap-queue.h>

uns8            token[2] = {0x14, 0x15};
String          body;

CoapPacket coapPing, coapGet, coapPost, coapPut, coapDel;

void setup() {
  Serial.begin(115200);
  coapQueue.begin();
  Serial.println("Generating coap packets - PING, GET, POST, PUT, DEL");
  makePackets();
}

void loop() {
  if (Serial.available()) {
    char input = Serial.read();
    switch (input) {
      //case 'a': //makePackets(); break;
      case '1': coapQueue.clearRxQueue(2); coapQueue.clearTxQueue(1);Serial.println("Cleared slot 2 in RX, slot 1 in TX"); queueStatus(); break;
      /*case '2': coapQueue.addToRxBuffer(coapGet.getPacket(), coapGet.getPacketLength()); queueStatus(); break;
      case '3': coapQueue.addToRxBuffer(coapPost.getPacket(), coapPost.getPacketLength()); queueStatus(); break;
      case '4': coapQueue.addToRxBuffer(coapPut.getPacket(), coapPut.getPacketLength()); queueStatus(); break;
      case '5': coapQueue.addToRxBuffer(coapDel.getPacket(), coapDel.getPacketLength()); queueStatus(); break;*/
      case 'a': fillRXQueue(); break;
      case 'b': fillTXQueue(); break;
      case '6': printRXQueue(); printTXQueue(); break;
      case '7': parseQueue(); break;
      case '8': testPackets(2, 0); testPackets(2, 1); testPackets(2, 2); testPackets(2, 3); break; //Test if there's a GET
      case '9': testPackets(5, 0); testPackets(5, 1); testPackets(5, 2); testPackets(5, 3); break;  //Test for DEL
    }
  }
}

void fillRXQueue() {
  Serial.println("Enter 4 digits (1-5) to indicate order of packets in RX Queue");
  while (!Serial.available()){  };
  while (Serial.available()) {
    char slot = Serial.read();
    int x;
    switch (slot) {
      case '1': x = coapQueue.addToRxBuffer(coapPing.getPacket(), coapPing.getPacketLength()); queueStatus(); break;
      case '2': x = coapQueue.addToRxBuffer(coapGet.getPacket(), coapGet.getPacketLength()); queueStatus(); break;
      case '3': x = coapQueue.addToRxBuffer(coapPost.getPacket(), coapPost.getPacketLength()); queueStatus(); break;
      case '4': x = coapQueue.addToRxBuffer(coapPut.getPacket(), coapPut.getPacketLength()); queueStatus(); break;
      case '5': x = coapQueue.addToRxBuffer(coapDel.getPacket(), coapDel.getPacketLength()); queueStatus(); break;
    }
    if (x == 0)
      Serial.println("Packet not added - queue is full");
  }
}

void fillTXQueue() {
  Serial.println("Enter 4 digits (1-5) to indicate order of packets in TX Queue");
  while (!Serial.available()){  };
  while (Serial.available()) {
    char slot = Serial.read();
    switch (slot) {
      case '1': coapQueue.addToTxBuffer(coapPing.getPacket(), coapPing.getPacketLength()); queueStatus(); break;
      case '2': coapQueue.addToTxBuffer(coapGet.getPacket(), coapGet.getPacketLength()); queueStatus(); break;
      case '3': coapQueue.addToTxBuffer(coapPost.getPacket(), coapPost.getPacketLength()); queueStatus(); break;
      case '4': coapQueue.addToTxBuffer(coapPut.getPacket(), coapPut.getPacketLength()); queueStatus(); break;
      case '5': coapQueue.addToTxBuffer(coapDel.getPacket(), coapDel.getPacketLength()); queueStatus(); break;
    }
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
    if (coapQueue.processTxPacket(i) == 0)
      Serial.println("!!! THIS SLOT IS EMPTY !!!");
  }
}

void makePackets() {
  Serial.println("COAP_PING");
  coapPing.begin();
  coapPing.addHeader(TYPE_CON, COAP_PING, 13400);
  coapPing.printPacket();
  Serial.println();
  Serial.println("COAP_GET");
  coapGet.begin();
  coapGet.addHeader(TYPE_CON, COAP_GET, 13402);
  coapGet.addTokens(2, token);
  coapGet.addOption(OPT_URI_PATH, 4, "user");
  coapGet.printPacket();
  Serial.println();
  body = "value:12.34";
  Serial.println("COAP_POST");
  coapPost.begin();
  coapPost.addHeader(TYPE_CON, COAP_POST, 13404);
  coapPost.addOption(OPT_URI_PATH, 6, "stream");
  coapPost.addPayload(body.length(), body.c_str());
  coapPost.printPacket();
  Serial.println();
  body = "PUT";
  Serial.println("COAP_PUT");
  coapPut.begin();
  coapPut.addHeader(TYPE_CON, COAP_PUT, 13408);
  coapPut.addOption(OPT_URI_PATH, 4, "pond");
  coapPut.addPayload(body.length(), body.c_str());
  coapPut.printPacket();
  Serial.println();
  body = "DELETED";
  Serial.println("COAP_DEL");
  coapDel.begin();
  coapDel.addHeader(TYPE_CON, COAP_DEL, 13416);
  coapDel.addPayload(body.length(), body.c_str());
  coapDel.printPacket();
  Serial.println();
}


void queueStatus() {
  int num = coapQueue.numEmptySpaces(0);
  Serial.print("Number of empty space in queue: ");
  Serial.println(num);
  num = coapQueue.numFilledSpaces(0);
  Serial.print("Number of filled space in queue: ");
  Serial.println(num);
}

/* 1 = ping, 2 = get, 3 = post, 4 = put, 5 = del */
void testPackets(int i, int j) {
  int sum = 0;
  uns8* ptr_a;
  int len_a;
  switch (i) {
    case 1: ptr_a = coapPing.getPacket(); len_a = coapPing.getPacketLength(); break;
    case 2: ptr_a = coapGet.getPacket(); len_a = coapGet.getPacketLength(); break;
    case 3: ptr_a = coapPost.getPacket(); len_a = coapPost.getPacketLength(); break;
    case 4: ptr_a = coapPut.getPacket(); len_a = coapPut.getPacketLength(); break;
    case 5: ptr_a = coapDel.getPacket(); len_a = coapDel.getPacketLength(); break;
  }
  uns8* ptr_b = coapQueue.getRX(j);
  int len_b = coapQueue.getRXLength(j);
  int len;
  if (len_a > len_b)
    len = len_b;
  else
    len = len_a;
  Serial.print("\nTesting packet. Length is ");
  Serial.println(len);
  for (int i = 0; i < len; i++) {
    Serial.print("Byte ");
    Serial.print(i);
    if (ptr_a[i] == ptr_b[i]) {
      Serial.println(" - OK");
      sum++;
    }
    else {
      Serial.print(" - NOT OK--> ");
      Serial.print(ptr_a[i], HEX);
      Serial.print(" - ");
      Serial.println(ptr_b[i], HEX);
    }
  }
  if (sum == len) 
    Serial.println("Packet is CORRECT");
  else
    Serial.println("Packet is INCORRECT");
}


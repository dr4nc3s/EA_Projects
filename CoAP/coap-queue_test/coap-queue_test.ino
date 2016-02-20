#include <ESP8266WiFi.h>
#include <coap-packet.h>
#include <PacketParser.h>
#include <coap-queue.h>

uns8            token[2] = {0x14, 0x15};
int             i = 0;
String          body;


CoapPacket tx;

void setup() {
  Serial.begin(115200);
  coapQueue.begin();
}

void loop() {
  if (Serial.available()) {
    char input = Serial.read();
    switch (input) {
      case '0': makeTestPacket(COAP_PING); break;
      case '1': makeTestPacket(COAP_GET); break;
      case '2': makeTestPacket(COAP_POST); break;
      case '3': makeTestPacket(COAP_PUT); break;
      case '4': makeTestPacket(COAP_DEL); break;
      case '5': coapQueue.addToRxBuffer(tx.getPacket(), tx.getPacketLength(), i++); break;
      case '6': printQueue(); break;//coapQueue.printRxPacket(i-1); Serial.print("\nThis is in index "); Serial.println(i); break;
      case '7': parseAndPrintQueue(); break;
      case '8': Serial.println("Nothing");
      case '9': Serial.println("Nothing");
    }
    //i++;
    if (i == 4)
      i = 0;
  }
}

void printQueue() {
  Serial.println("\nQUEUE CONTENTS");
  Serial.println("\nIndex 0");
  coapQueue.printRxPacket(0);
  Serial.println("\n\nIndex 1");
  coapQueue.printRxPacket(1);
  Serial.println("\n\nIndex 2");
  coapQueue.printRxPacket(2);
  Serial.println("\n\nIndex 3");
  coapQueue.printRxPacket(3);
}

void parseAndPrintQueue() {
  Serial.println("\nQUEUE CONTENTS");
  for (int i = 0; i < 4; i++) {
    Serial.print("\nRX Queue Index ");
    Serial.print(i);
    if (coapQueue.processPacket(i) == 0)
      Serial.println("!!! THIS SLOT IS EMPTY !!!");
  }
  
  /*
  coapQueue.processPacket(0);
  //coapQueue.printRxPacket(0);
  Serial.print("\nIndex 1");
  coapQueue.processPacket(1);
  //coapQueue.printRxPacket(1);
  Serial.print("\nIndex 2");
  coapQueue.processPacket(2);
  //coapQueue.printRxPacket(2);
  Serial.print("\nIndex 3");
  coapQueue.processPacket(3);
  //coapQueue.printRxPacket(3);*/
}

void makeTestPacket(int request) {
  Serial.println();
  switch (request) {
    case COAP_PING:
      Serial.println("COAP_PING");
      tx.begin();
      tx.addHeader(TYPE_CON, COAP_PING, 13456);
      //tx.printPacket();
      break;
    case COAP_GET:
      Serial.println("COAP_GET");
      tx.begin();
      tx.addHeader(TYPE_CON, COAP_GET, 13456);
      tx.addTokens(2, token);
      tx.addOption(OPT_URI_PATH, 4, "user");
      //tx.printPacket();
      break;
    case COAP_POST:
      body = "value:12.34";
      Serial.println("COAP_POST");
      tx.begin();
      tx.addHeader(TYPE_CON, COAP_POST, 13456);
      tx.addOption(OPT_URI_PATH, 6, "stream");
      tx.addPayload(body.length(), body.c_str());
      //tx.printPacket();
      break;
    case COAP_PUT:
      body = "PUT";
      Serial.println("COAP_PUT");
      tx.begin();
      tx.addHeader(TYPE_CON, COAP_PUT, 13456);
      tx.addOption(OPT_URI_PATH, 4, "pond");
      tx.addPayload(body.length(), body.c_str());
      //tx.printPacket();
      break;
    case COAP_DEL:
      body = "DELETED";
      Serial.println("COAP_DEL");
      tx.begin();
      tx.addHeader(TYPE_CON, COAP_DEL, 13456);
      tx.addPayload(body.length(), body.c_str());
      break;
  }
  Serial.print("\nPacket Length: ");
  Serial.println(tx.getPacketLength());
  tx.printPacket();
  
}


#include <ESP8266WiFi.h>
#include <coap-packet.h>
#include <coap-queue.h>

CoapPacket coapPing, coapGet, coapPost, coapPut, coapDel;

uns8            token[2] = {0x14, 0x15};
String          body;
const char*     ssid     = "whiteBox";
const char*     password = "apartment5g";
const char*     ip       = "";
int             port     = 200;

void setup() {
  Serial.begin(115200);
  coapQueue.begin();
  Serial.println("Generating coap packets - PING, GET, POST, PUT, DEL");
  makePackets();
  connectWiFi();
}

void loop() {
  if (Serial.available()) {
    char input = Serial.read();
    switch (input) {
      case 'r':
        Serial.println("Receive a packet sim. Which packet?");
        receivePacketSim();
        break;
      case 'a':
        Serial.println("Adding packet to TX");
        addPacketSim();
        break;
      case 'c':
        Serial.println("Clearing slot in rxqueue. Which slot?");
        clearRXSlot();
        break;
      case 'x':
        Serial.println("Clearing slot in txqueue. Which slot?");
        clearTXSlot();
        break;
      case 's':
        Serial.println("Send a packet sim. Which packet?");
        sendPacketSim();
        break;
      case 't':
        Serial.println("Printing TX Queue packets and stats");
        printTXQueue();
        break;
      case 'g':
        Serial.println("Printing RX Queue packets and stats");
        printRXQueue();
        break;
    }
    coapQueue.process_rx_queue();
    printRXPacketStats();
    //coapQueue.process_tx_queue();
    //  printTXPacketStats();
    Serial.println();
  }
}

void addPacketSim() {
  int index;
  while (!Serial.available()) {}
  while (Serial.available())   {
    char slot = Serial.read();
    switch (slot) {
      case '1': 
        index = coapQueue.addToTxBuffer(coapPing.getPacket(), coapPing.getPacketLength());
        Serial.println("Added PING packet");
        break;
      case '2': 
        index = coapQueue.addToTxBuffer(coapGet.getPacket(), coapGet.getPacketLength());
        Serial.println("Added GET packet");
        break;
      case '3': 
        index = coapQueue.addToTxBuffer(coapPost.getPacket(), coapPost.getPacketLength());
        Serial.println("Added POST packet");
        break;
      case '4': 
        index = coapQueue.addToTxBuffer(coapPut.getPacket(), coapPut.getPacketLength());
        Serial.println("Added PUT packet");
        break;
      case '5': 
        index = coapQueue.addToTxBuffer(coapDel.getPacket(), coapDel.getPacketLength());
        Serial.println("Added DEL packet");
        break;
    }
    //Serial.print("Packet sent at time ");
    //Serial.println(coapQueue.timeSent(index));
  }
}

void sendPacketSim() {
  int index;
  while (!Serial.available()) {}
  while (Serial.available())   {
    char slot = Serial.read();
    switch (slot) {
      case '1': 
        coapQueue.sendPacketSim(0);
        Serial.println("Sending packet 0");
        break;
      case '2': 
        coapQueue.sendPacketSim(1);
        Serial.println("Sending packet 1");
        break;
      case '3': 
        coapQueue.sendPacketSim(2);
        Serial.println("Sending packet 2");
        break;
      case '4': 
        coapQueue.sendPacketSim(3);
        Serial.println("Sending packet 3");
        break;
    }
    Serial.print("Packet sent at time ");
    Serial.println(coapQueue.timeSent(index));
  }
}

void clearRXSlot() {
  while (!Serial.available()) {}
  while (Serial.available()) {
    char slot = Serial.read();
    switch (slot) {
      case '1': 
        coapQueue.clearRxQueue(0);
        Serial.println("Cleared slot 0");
        break;
      case '2': 
        coapQueue.clearRxQueue(1);
        Serial.println("Cleared slot 1");
        break;
      case '3': 
        coapQueue.clearRxQueue(2);
        Serial.println("Cleared slot 2");
        break;
      case '4': 
        coapQueue.clearRxQueue(3);
        Serial.println("Cleared slot 3");
        break;
    }
    printRXPacketStats();
  }
}

void clearTXSlot() {
  while (!Serial.available()) {}
  while (Serial.available()) {
    char slot = Serial.read();
    switch (slot) {
      case '1': 
        coapQueue.clearTxQueue(0);
        Serial.println("Cleared slot 0");
        break;
      case '2': 
        coapQueue.clearTxQueue(1);
        Serial.println("Cleared slot 1");
        break;
      case '3': 
        coapQueue.clearTxQueue(2);
        Serial.println("Cleared slot 2");
        break;
      case '4': 
        coapQueue.clearTxQueue(3);
        Serial.println("Cleared slot 3");
        break;
    }
    printTXPacketStats();
  }
}

void printTXPacketStats() {
  Serial.print("Packet 0 status byte - ");
  Serial.println(coapQueue.getTXStatus(0), HEX);
  Serial.print("Packet 1 status byte - ");
  Serial.println(coapQueue.getTXStatus(1), HEX);
  Serial.print("Packet 2 status byte - ");
  Serial.println(coapQueue.getTXStatus(2), HEX);
  Serial.print("Packet 3 status byte - ");
  Serial.println(coapQueue.getTXStatus(3), HEX);
  Serial.print("TX Queue Status - ");
  Serial.println(coapQueue.getBufferStatus(TX), HEX);
}

void printRXPacketStats() {
  Serial.print("Packet 0 status byte - ");
  Serial.println(coapQueue.getRXStatus(0), HEX);
  Serial.print("Packet 1 status byte - ");
  Serial.println(coapQueue.getRXStatus(1), HEX);
  Serial.print("Packet 2 status byte - ");
  Serial.println(coapQueue.getRXStatus(2), HEX);
  Serial.print("Packet 3 status byte - ");
  Serial.println(coapQueue.getRXStatus(3), HEX);
  Serial.print("RX Queue Status - ");
  Serial.println(coapQueue.getBufferStatus(RX), HEX);
}

void receivePacketSim() {
  int index;
  while (!Serial.available()) {}
  while (Serial.available())   {
    char slot = Serial.read();
    switch (slot) {
      case '1': 
        index = coapQueue.addToRxBuffer(coapPing.getPacket(), coapPing.getPacketLength());
        Serial.println("Added PING packet");
        break;
      case '2': 
        index = coapQueue.addToRxBuffer(coapGet.getPacket(), coapGet.getPacketLength());
        Serial.println("Added GET packet");
        break;
      case '3': 
        index = coapQueue.addToRxBuffer(coapPost.getPacket(), coapPost.getPacketLength());
        Serial.println("Added POST packet");
        break;
      case '4': 
        index = coapQueue.addToRxBuffer(coapPut.getPacket(), coapPut.getPacketLength());
        Serial.println("Added PUT packet");
        break;
      case '5': 
        index = coapQueue.addToRxBuffer(coapDel.getPacket(), coapDel.getPacketLength());
        Serial.println("Added DEL packet");
        break;
    }
    Serial.print("Packet received at time ");
    Serial.println(coapQueue.timeReceived(index));
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

void makePackets() {
  Serial.println("COAP_PING");
  coapPing.begin();
  coapPing.addHeader(TYPE_NON, COAP_PING, 13400);
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
  coapDel.addHeader(TYPE_RST, COAP_DEL, 13416);
  coapDel.addPayload(body.length(), body.c_str());
  coapDel.printPacket();
  Serial.println();
}

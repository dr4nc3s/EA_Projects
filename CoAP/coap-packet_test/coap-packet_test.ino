#include <ESP8266WiFi.h>
#include <coap-packet.h>
#include <PacketParser.h>
#include <WiFiUdp.h>

const char*     ssid     = "whiteBox";
const char*     password = "apartment5g";

uns8            token[2] = {0x14, 0x15};
uns8            testCoap[27] = {0x42, 0x01, 0x34, 0x90, 0x14, 0x15, 0xB4, 0x75, 0x73, 0x65, 0x72, 0xFF, 0x74, 0x65, 0x73, 0x74, 0x69, 0x6E, 0x67, 0x20, 0x70, 0x61, 0x79, 0x6C, 0x6F, 0x61, 0x64};

CoapPacket tx;
CoapPacket rx;

void setup() {
  Serial.begin(115200);
  //connectWiFi();
  Serial.println("Ready to test new library");
  Serial.println("1 - make packet");
  Serial.println("2 - make PING packet");
  Serial.println("3 - print packet");
  Serial.println("4 - test packet");
  Serial.println("5 - print human-readable packet");
}

void loop() {
  if (Serial.available()) {
    char input = Serial.read();
    switch (input) {
      case '1': createNewPacket(); break;
      case '2': createPING(); break;
      case '3': {
        Serial.print("\nPacket Length: ");
        Serial.println(tx.getPacketLength());
        tx.printPacket();
        break;
      }
      case '4': testPackets(); break;
      case '5': tx.readPacket(); break;
    }
  }
}

void testPackets() {
  int sum = 0;
  uns8* ptr = tx.getPacket();
  int len = tx.getPacketLength();
  Serial.print("\ntesting tx packet with const packet. Length is ");
  Serial.println(len);
  for (int i = 0; i < len; i++) {
    Serial.print("Byte ");
    Serial.print(i);
    if (ptr[i] == testCoap[i]) {
      Serial.println(" - OK");
      sum++;
    }
    else {
      Serial.print(" - NOT OK--> ");
      Serial.print(ptr[i], HEX);
      Serial.print(" - ");
      Serial.println(testCoap[i], HEX);
    }
  }
  if (sum == len) 
    Serial.println("TX Packet is CORRECT");
  else
    Serial.println("TX Packet is INCORRECT");
}

void createNewPacket() {
  String body = "testing payload";
  tx.begin();
  tx.addHeader(TYPE_CON, COAP_GET, 13456);
  tx.addTokens(2, token);
  tx.addOption(OPT_URI_PATH, 4, "user");
  tx.addOption(OPT_URI_PATH, 5, "login");
  tx.addPayload(body.length(), body.c_str());
  tx.parsePacket();
}

void createPING() {
  Serial.println("Making PING");
  tx.begin();
  tx.addHeader(TYPE_CON, COAP_PING, 13460);
  Serial.println("In ping, before parse");
  tx.parsePacket();
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

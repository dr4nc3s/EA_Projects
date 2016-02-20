#include "Arduino.h"
#include "WiFiUdp.h"
#include "coap-queue.h"

int CoapQueue::begin() {
	Serial.println("In begin");
	//rxBuffer = new CoapPacket[4];
	//txBuffer = new CoapPacket[4];
	for (int i = 0; i < 4; i++) {
		rxStats[i] = SPACE_EMPTY;
		txStats[i] = SPACE_EMPTY;
	}
	//WiFiUDP::begin(thisPort);
	return 1;
}

int CoapQueue::addToRxBuffer(uns8 *packet, int len, int index) {
	Serial.print("\nAdding packet to index number ");
	Serial.println(index);
	errorCheck = rxBuffer[index].begin();
	if (errorCheck == 0) {
		rxBuffer[index].end();
		errorCheck = rxBuffer[index].begin();
	}
	errorCheck += rxBuffer[index].collectPacket(packet, len);
	
	rxStats[index] = 1;		//Not empty, unread, shouldn't be cleared
	return errorCheck;
}

int CoapQueue::addToRxBuffer(int len, int index) {
	errorCheck = rxBuffer[index].begin();
	if (errorCheck == 0)
		rxBuffer[index].end();
	errorCheck = rxBuffer[index].begin();
	errorCheck += rxBuffer[index].collectPacket(inPacket, len);
	
	rxStats[index] = 1;		//Not empty, unread, shouldn't be cleared
	return errorCheck;
}

int CoapQueue::printRxPacket(int i) {
	/*if (rxStats[i] == SPACE_EMPTY)
		return 0;
	uns8 *ptr = rxBuffer[i].getPacket();
	if ((ptr == 0) || (ptr == '\0')) {
		Serial.print("\nPACKET IN SLOT ");
		Serial.print(i);
		Serial.println(" IS EMPTY");
		return 0;
	}
	else {*/
		rxBuffer[i].printPacket();
	//}
	return 1;	
}

int CoapQueue::readRxBuffer(int index) {
	rxBuffer[index].printPacket();
	rxBuffer[index].parsePacket();
	return 1;
}

void CoapQueue::printPayload(int index) {
	uns8 *pay = rxBuffer[index].getPayloadAddr();
	if (pay == 0)
      Serial.println("NO PAYLOAD");
    else {
      while (*pay != '\0') {
        Serial.print((char)*pay);
        pay++;
      }
    }
}

int CoapQueue::processPacket(int i) {
	if (rxStats[i] == SPACE_EMPTY)
		return 0;
	Serial.println("in process packet");
	rxBuffer[i].parsePacket();
	uns8 transaction = rxBuffer[i].getMessageType();
	rxStats[i] = SPACE_CLEAR;
	if (transaction == TYPE_CON) {
		return 1;
	}
	else 
		return 0;
}

uns8* CoapQueue::getTX(int index) {
	return txBuffer[index].getPacket();
}

int CoapQueue::getTXLength(int index) {
	return txBuffer[index].getPacketLength();
}

int CoapQueue::printTX(int index) {
	uns8 *ptr = txBuffer[index].getPacket();
	if ((ptr == 0) || (ptr == '\0')) {
		Serial.print("\nPACKET IN SLOT ");
		Serial.print(index);
		Serial.println(" IS EMPTY");
		return 0;
	}
	else {
		txBuffer[index].printPacket();
	}
	return 1;
}

uns8 CoapQueue::getRxCode(int index) {
	return rxBuffer[index].getResponseCode();
}


int CoapQueue::createReply(int index, uns8 code) {
	int id = rxBuffer[index].getID();
	if (code == 0xFF)
		code = rxBuffer[index].getResponseCode();
	else if (code == TYPE_NON)
		return 0;
	if (!txBuffer[index].begin()) {
		txBuffer[index].end();
		txBuffer[index].begin();
	}
	rxStats[index] = SPACE_DNT_CLEAR;
	txBuffer[index].addHeader(TYPE_ACK, code, id);
	return 1;
}

int CoapQueue::addPayload(int len, const char *payloadValue, int index) {
	return txBuffer[index].addPayload(len, payloadValue);
}

//////////////////////
//	UDP Functions	//
//////////////////////
/*
int CoapQueue::sendPacket(int index) {
		Serial.println("Sending packet");
		WiFiUDP::beginPacket(ipRemote1, portRemote);
		WiFiUDP::write(txBuffer[index].getPacket(), txBuffer[index].getPacketLength());
		WiFiUDP::endPacket();
		
		return 1;
}

int CoapQueue::sendPacket(int index, const char ip, int portOut) {
		Serial.println("Sending packet");
		WiFiUDP::beginPacket(ip, portOut);
		WiFiUDP::write(txBuffer[index].getPacket(), txBuffer[index].getPacketLength());
		WiFiUDP::endPacket();
		
		return 1;
}

int CoapQueue::receivePacket(int index) {
	int packetSize = WiFiUDP::parsePacket();
	if (packetSize) {
		inPacket = new uns8[MAX_SIZE];
		Serial.println("Received something");
		int len = WiFiUDP::read(inPacket, MAX_SIZE);
		portRemote = WiFiUDP::remotePort();
		ipRemote = WiFiUDP::remoteIP();
		addToRxBuffer(len, index);
		delete[] inPacket;
		return 1;
	}
	else
		return 0;
	
}
*/


CoapQueue coapQueue;

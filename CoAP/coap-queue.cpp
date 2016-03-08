#include "Arduino.h"
#include "WiFiUdp.h"
#include "coap-queue.h"

int CoapQueue::begin() {
	for (int i = 0; i < 4; i++) {
		rxPacketStatus[i] = 0;
		txPacketStatus[i] = 0;
		rxTimes[i] = 0;
		txTimes[i] = 0;
	}
	rxQueueStatus = 0x0F;
	txQueueStatus = 0x0F;
	WiFiUDP::begin(thisPort);
	received = false;
	return 1;
}

int CoapQueue::numTimesTransmitted(uns8& stat) {
	return (stat & SREG_MASK_NTIMES);
}

int CoapQueue::oldestPacket(int a, int b, int queue) {
	if (queue) 
		return (rxTimes[a] > rxTimes[b] ? b : a);
	else
		return (txTimes[a] > txTimes[b] ? b : a);
}

int CoapQueue::newestPacket(int a, int b, int queue) {
	if (queue) 
		return (rxTimes[a] > rxTimes[b] ? a : b);
	else
		return (txTimes[a] > txTimes[b] ? a : b);
}

int CoapQueue::numEmptySpaces(int queue) {
	int sum = 0;
	if (queue)
		pStatusPtr = &rxPacketStatus[0];
	else
		pStatusPtr = &txPacketStatus[0];
	for (int i = 0; i < 4; i++) {
		/*if (queue) {
			if ((rxPacketStatus[i] & SREG_MASK_FILLED) == 0x00)
				sum++;
		}
		else {
			if ((txPacketStatus[i] & SREG_MASK_FILLED) == 0x00)
				sum++;
		}*/
		if 	((pStatusPtr[i] & SREG_MASK_FILLED) == 0x00)
			sum++;
	}
	return sum;
}

int CoapQueue::numFilledSpaces(int queue) {
	int sum = 0;
	for (int i = 0; i < 4; i++) {
		if (queue) {
			if (!(rxPacketStatus[i] & SREG_MASK_FILLED) == 0x00)
				sum++;
		}
		else {
			if (!(txPacketStatus[i] & SREG_MASK_FILLED) == 0x00)
				sum++;
		}
			
	}
	return sum;
}

int CoapQueue::addToRxBuffer(uns8 *packet, int len) {
	if (numFilledSpaces(0) == 4)
		return 0;
	int index = 0;
	while (rxPacketStatus[index] != SPACE_EMPTY) {
		index++;
	}
	Serial.print("\nAdding packet to index number ");
	Serial.println(index);
	errorCheck = rxBuffer[index].begin();
	if (errorCheck == 0) {
		errorCheck = rxBuffer[index].begin();
	}
	errorCheck += rxBuffer[index].collectPacket(packet, len);
	rxTimes[index] = millis();
	//Not empty, unread, shouldn't be cleared
	rxPacketStatus[index] |= SREG_MASK_FILLED;
	rxPacketStatus[index]++;
	if (rxBuffer[index].getMessageType() == TYPE_CON)
		rxPacketStatus[index] |= SREG_MASK_SENDACK;
	return index;
}

int CoapQueue::printRxPacket(int i) {
	if (rxPacketStatus[i] == SPACE_EMPTY)
		return 0;
	rxBuffer[i].printPacket();
	Serial.println();
	rxBuffer[i].readPacket();
	return 1;	
}

int CoapQueue::processRxPacket(int i) {
	if (rxPacketStatus[i] == SPACE_EMPTY)
		return 0;
	Serial.println("in processrx");
	return rxBuffer[i].parsePacket();;
}

uns8* CoapQueue::getRX(int index) {
	return rxBuffer[index].getPacket();
}

int CoapQueue::getRXLength(int index) {
	return rxBuffer[index].getPacketLength();
}

int CoapQueue::getRxID(int index) {
	if (rxPacketStatus[index] == SPACE_DNT_CLEAR)
		return rxBuffer[index].getID();
	else
		return 0;
}

uns8 CoapQueue::getRxCode(int index) {
	if (rxPacketStatus[index] == SPACE_DNT_CLEAR)
		return rxBuffer[index].getMessageType();
	else
		return 0;
}

int CoapQueue::addToTxBuffer(uns8 *packet, int len) {
	if (numFilledSpaces(1) == 4)
		return 0;
	int index = 0;
	while (txPacketStatus[index] != SPACE_EMPTY) {
		index++;
	}
	Serial.print("\nAdding packet to index number ");
	Serial.println(index);
	errorCheck = txBuffer[index].begin();
	if (errorCheck == 0) {
		errorCheck = txBuffer[index].begin();
	}
	errorCheck += txBuffer[index].collectPacket(packet, len);
	//txTimes[index] = millis();
	//Not empty, unread, shouldn't be cleared
	txPacketStatus[index] |= SREG_MASK_FILLED;
	//txPacketStatus[index]++;
	if (txBuffer[index].getMessageType() == TYPE_CON)
		txPacketStatus[index] |= SREG_MASK_RCVACK;	
	Serial.print("Status byte: ");
	Serial.println(txPacketStatus[index], HEX);	
	
	return index;
}

void CoapQueue::sendPacketSim(int index) {
	txTimes[index] = millis();
	txPacketStatus[index]++;
}

int CoapQueue::printTxPacket(int i) {
	if (txPacketStatus[i] == SPACE_EMPTY)
		return 0;
	txBuffer[i].readPacket();
	return 1;	
}

int CoapQueue::processTxPacket(int i) {
	if (txPacketStatus[i] == SPACE_EMPTY)
		return 0;
	txBuffer[i].parsePacket();
	return 1;
}

uns8* CoapQueue::getTX(int index) {
	return txBuffer[index].getPacket();
}

int CoapQueue::getTXLength(int index) {
	return txBuffer[index].getPacketLength();
}

int CoapQueue::clearTxQueue(int index) {
	if (index == 4) {
		txPacketStatus[0] = 0;
		txPacketStatus[1] = 0;
		txPacketStatus[2] = 0;
		txPacketStatus[3] = 0;
		
		txTimes[0] = 0;
		txTimes[1] = 0;
		txTimes[2] = 0;
		txTimes[3] = 0;
	}
	else {
		txPacketStatus[index % 4] = 0;
		txTimes[index % 4] = 0;
	}
}

int CoapQueue::clearRxQueue(int index) {
	if (index == 4) {
		rxPacketStatus[0] = 0;
		rxPacketStatus[1] = 0;
		rxPacketStatus[2] = 0;
		rxPacketStatus[3] = 0;
		
		rxTimes[0] = 0;
		rxTimes[1] = 0;
		rxTimes[2] = 0;
		rxTimes[3] = 0;
	}
	else {
		rxPacketStatus[index % 4] = 0;
		rxTimes[index % 4] = 0;
	}
}

int CoapQueue::autoReply(int index) {
	if ((received) && (rxPacketStatus[index] == SPACE_DNT_CLEAR)) {
		int id = getRxID(index);
		uns8 response = rxBuffer[index].getResponseCode();
		uns8 code = getRxCode(index);
		rxBuffer[index].parsePacket();
		if (code == TYPE_CON) {
			Serial.print("Packet with msgID ");
			Serial.print(id, DEC);
			Serial.println(" needs an ACK");
			txBuffer[index].begin();
			txBuffer[index].addHeader(TYPE_ACK, response, id);
			txBuffer[index].addPayload(8, "RECEIVED");
			txPacketStatus[index] = SPACE_DNT_CLEAR;
			Serial.println("Reply packet made");
			sendPacket(index);
		}
		return 1;
	}
	else
		return 0;
}

int CoapQueue::findRxSpace() {
	int x = 0;
	while ((rxPacketStatus[x] == SPACE_DNT_CLEAR) && (x < 4)) {
		x++;
	}
	return x;
}

int CoapQueue::needsReply(int index) {
	if (rxPacketStatus[index] == SPACE_DNT_CLEAR) {
		uns8 code = getRxCode(index);
		if (code == TYPE_CON)
			return 1;
	}
	else
		return 0;
}

int CoapQueue::createReply(int index) {
	int id = getRxID(index);
	uns8 response = rxBuffer[index].getResponseCode();
	txBuffer[index].begin();
	txBuffer[index].addHeader(TYPE_ACK, response, id);
	txPacketStatus[index] = SPACE_DNT_CLEAR;
	Serial.println("Reply packet made");
	rxPacketStatus[index] = SPACE_CLEAR;
	return 1;
}

int CoapQueue::addPayload(int index, int len, const char *pay) {
	return txBuffer[index].addPayload(len, pay);
}

//////////////////////
//	UDP Functions	//
//////////////////////

uns32 CoapQueue::timeSent(int index) {
	return txTimes[index % 4];
}

uns32 CoapQueue::timeReceived(int index) {
	return rxTimes[index % 4];
}

uns8 CoapQueue::getRXStatus(int index) {
	return rxPacketStatus[index];
}

uns8 CoapQueue::getTXStatus(int index) {
	return txPacketStatus[index];
}

int CoapQueue::sendPacket(int index) {
		Serial.println("Sending packet");
		WiFiUDP::beginPacket(ipRemote, portRemote);
		WiFiUDP::write(txBuffer[index].getPacket(), txBuffer[index].getPacketLength());
		WiFiUDP::endPacket();
		txTimes[index] = millis();
		received = false;
		return 1;
}

int CoapQueue::sendPacket(int index, const char* ipAddr, int port) {
		Serial.println("Sending packet");
		WiFiUDP::beginPacket(ipAddr, port);
		WiFiUDP::write(txBuffer[index].getPacket(), txBuffer[index].getPacketLength());
		WiFiUDP::endPacket();
		txTimes[index] = millis();	//Log time
		if (txBuffer[index].getMessageType() == TYPE_CON) {	//check if it needs an ACK
			txPacketStatus[index] |= SREG_MASK_RCVACK;	
		}
		received = false;
		return 1;
}

int CoapQueue::parseUDPPacket() {
	return WiFiUDP::parsePacket();
}

int CoapQueue::receivePacket(int index) {
	Serial.print("\nAdding packet to index number ");
	Serial.println(index);
	rxPacketStatus[index] |= SREG_MASK_FILLED;
	rxPacketStatus[index]++;
	rxBuffer[index].begin();
	int len = WiFiUDP::read(rxBuffer[index].getPacket(), MAX_SIZE);
	rxBuffer[index].setIndex(len);
	portRemote = WiFiUDP::remotePort();
	ipRemote = WiFiUDP::remoteIP();
	rxTimes[index] = millis();
	return len;
}

void CoapQueue::process_rx_queue() {
	findRepeats(RX);
	for (int i = 0; i < 4; i++) {
		if ((rxPacketStatus[i] & SREG_MASK_FILLED) == 0) {	//If empty
			rxQueueStatus |= (1 << i);		//mark index as empty
			continue;
		}
		if ((rxPacketStatus[i] & SREG_MASK_SENDACK) == SREG_MASK_SENDACK) {
			Serial.print("Packet with ID ");
			Serial.print(rxBuffer[i].getID());
			Serial.println(" is waiting for an ACK response");
			rxQueueStatus |= (1 << (i + 4));
		}	//Place ACK bit on the queue status byte
		else
			rxQueueStatus &= (0xFF & ~(1 << (i + 4)));	//Place NO_ACK bit on byte
		rxQueueStatus &= (0xFF & ~(1 << i));	//This packet isn't empty
	}
	
}

void CoapQueue::process_tx_queue() {
	uns32 current;
	bool waitingForAck = false;
	findRepeats(TX);
	Serial.println("After repeats");
	//Go through all packets in the tx queue
	for (int i = 0; i < 4; i++) {
		if ((txPacketStatus[i] & SREG_MASK_FILLED) == 0) {
			txQueueStatus |= 1 << i;
			continue;
		}
		if ((txPacketStatus[i] & SREG_MASK_NTIMES) == 0)	//Packet hasn't been sent yet, send it
			sendPacketSim(i);
		if ((txPacketStatus[i] & SREG_MASK_RCVACK) == SREG_MASK_RCVACK) {	//Packet is CON
			Serial.print("Packet with ID ");
			Serial.print(txBuffer[i].getID());
			Serial.println(" needs to receive an ACK response");
			waitingForAck = true;
			txQueueStatus |= (1 << (i + 4));
			txQueueStatus &= (0xFF & ~(1 << i));
		}
		else {	//It's been sent, it doesn't need an ACK, clear it from txBuff
			txPacketStatus[i] = 0x00;
			txQueueStatus &= (0xFF & ~(1 << (i + 4)));
		}
		current = millis();
		if (((current - txTimes[i]) > TIME_EXPIRE) && (waitingForAck)){
			if ((txPacketStatus[i] & SREG_MASK_NTIMES) == 4) {
				Serial.println("Packet has been sent 4 times already. Deleting...");
				txPacketStatus[i] = 0x00;
			}
			else if ((txPacketStatus[i] & SREG_MASK_NTIMES) > 0){
				Serial.println("This packet waiting for an ACK has expired. Resending...");
				sendPacketSim(i);
			}
		}
		
		//txQueueStatus &= (0xFF & ~(1 << i));
	}
}

void CoapQueue::findRepeats(int queue) {
	unsigned int id[4];
	int old, newP;
	
	if (queue) {
		pStatusPtr = &rxPacketStatus[0];
		qPtr = &rxBuffer[0];
	}
	else {
		pStatusPtr = &txPacketStatus[0];
		qPtr = &txBuffer[0];
	}
	
	for (int i = 0; i < 4; i++) {
		if ((pStatusPtr[i] & SREG_MASK_FILLED) == SREG_MASK_FILLED)
			id[i] = qPtr[i].getID();
		else 
			id[i] = 0;//if it's empty, e'll put 0 on id to skip
	}
	for (int i = 1; i < 4; i++) {//find packet with same id as one in 0
		if ((id[0] == id[i]) && (id[i] != 0)) {
			Serial.print("Packets 0 and");
			Serial.print(i);
			Serial.println(" are identical");
			old = oldestPacket(0, i, queue);
			newP = newestPacket(0, i, queue);
			
			pStatusPtr[old]++;
			pStatusPtr[newP] = pStatusPtr[old];
			pStatusPtr[old] &= 0x00;
		}
	}
	for (int i = 2; i < 4; i++) {//find packet with same id as one in 1
		if ((id[1] == id[i]) && (id[i] != 0)) {
			Serial.print("Packets 1 and");
			Serial.print(i);
			Serial.println(" are identical");
			old = oldestPacket(1, i, queue);
			newP = newestPacket(1, i, queue);
			pStatusPtr[old]++;
			pStatusPtr[newP] = pStatusPtr[old];
			pStatusPtr[old] &= 0x00;
		}
	}
	if ((id[2] == id[3]) && (id[2] != 0)) {//are packets in 2 and 3 equal?
		Serial.println("Packets 2 and 3 are identical");
		old = oldestPacket(2, 3, queue);
		newP = newestPacket(2, 3, queue);
		pStatusPtr[old]++;
		pStatusPtr[newP] = pStatusPtr[old];
		pStatusPtr[old] &= 0x00;
	}
	
	if ((pStatusPtr[newP] & SREG_MASK_NTIMES) > 4) {
		Serial.println("This has been receive 4 times already. Removing from queue");
		pStatusPtr[newP] = 0x00;
	}
}

uns8 CoapQueue::getBufferStatus(int queue) {
	if (queue)
		return rxQueueStatus;
	else
		return txQueueStatus;
}

/*void CoapQueue::findRepeats(int queue, int i) {
	if (queue) {
		qPtr = &rxBuffer[i];
		pStatusPtr = &rxBuffer[i];
	}
	else {
		qPtr = &txBuffer[i];
		pStatusPtr = &txBuffer[i];
	}
	switch(i) {
		case 0:
		case 1:
		case 2:
	}
}*/

CoapQueue coapQueue;

#include "Arduino.h"
#include "coap-packet.h"

CoapPacket::CoapPacket() {
	//parser = new Parser();
	//packetBuffer = NULL;
}

CoapPacket::~CoapPacket() {
	//delete parser;
	//if (packetBuffer != NULL) // this was == but shouldn't it be != ?
		//delete[] packetBuffer;
}

int CoapPacket::begin() {
	index = 0;
	hdrIndex = index;
	tknIndex = 0;
	payloadIndex = 0;
	optionIndex = 0;
	parser = 0;
	
	lastOptionNumber = 0;
	numOptions = 0;
	return 1;
}

int CoapPacket::addHeader(uns8 type, uns8 code, int id) {
	packetBuffer[index++] = (((VERSION & 0x03) << 6) | ((type & 0x03) << 4));	// byte 0
	packetBuffer[index++] = code;	// byte 1
	packetBuffer[index++] = id >> 8;	// byte 2
	packetBuffer[index++] = id & 0xFF;	// byte 3
	return (index > MAX_SIZE) ? 0 : 1;	
}

int CoapPacket::addTokens(int tokenLength, uns8 *tokenValue) {
	if (tokenLength >= MAX_TOKENSIZE)
		return 0;
	tknIndex = index;	// we already know this is going to be byte position 4, but hey
	packetBuffer[hdrIndex] = (packetBuffer[hdrIndex] & 0xF0) | (uns8)tokenLength;
	for (int i = 0; i < tokenLength; i++) {
		packetBuffer[index++] = *tokenValue++;
	}
	return (index > MAX_SIZE) ? 0 : 1;

}

int CoapPacket::addPayload(int len, const char *payloadOut) {
	if (payloadIndex != 0)
		return 0;
	payloadIndex = index;
	packetBuffer[index++] = 0xFF;	// payload indicator
	for (int i = 0; i < len; i++) {
		packetBuffer[index++] = payloadOut[i];
	}
	packetLength = index;
	packetBuffer[index] = '\0';	// just for convenience but not strictly necessary
	return (index > MAX_SIZE) ? 0 : 1;
	
}

int CoapPacket::addOption(uns8 optNum, int len, const char *param) {
	optionIndex = index;	// where THIS option starts
	if (numOptions == 0)
		optionStartIndex = index;
	if (processDelta(optNum))		
		processLength(len);
	else
		return 0;
	for (int i = 0; i < len; i++) {
		packetBuffer[index++] = param[i];
		if (index > MAX_SIZE) {
			return 0;
		}
	}
	numOptions++;
	return (index > MAX_SIZE) ? 0 : 1;
}

bool CoapPacket::processDelta(uns8 optNum) {
	if (lastOptionNumber > optNum)	// must be presented in increasing order
		return false;
	uns16 delta;
	delta = optNum - lastOptionNumber;
	lastOptionNumber = optNum;
	if (delta > 268) { // it's a big one
		delta = delta - 269;
		packetBuffer[index++] = 14 << 4;	// 14 indicating a 16 bit number in optionIndex + 1
		packetBuffer[index++] = delta >> 8;
		packetBuffer[index++] = delta & 0xff;

	} else if (delta > 12) {
		delta = delta - 13;
		packetBuffer[index++] = 13 << 4;	// 13 indicating an 8 bit number in optionIndex + 1
		packetBuffer[index++] = delta;
		// index++;
	} else {  // option delta is 12 or less
		packetBuffer[index++] = delta << 4;
	}
	return true;
}

void CoapPacket::processLength(int len) {
	if (len > 268) { // it's a big one
		len = len - 269;
		packetBuffer[optionIndex] = (packetBuffer[optionIndex] & 0xF0) | 14;
		packetBuffer[index++] = len >> 8;
		packetBuffer[index++] = len & 0xff;

	} else if (len > 12) {
		packetBuffer[optionIndex] = (packetBuffer[optionIndex] & 0xF0) | 13;
		packetBuffer[index++] = len - 13;
	}
	else {
		packetBuffer[optionIndex] = (packetBuffer[optionIndex] & 0xF0) | len;
	}
}

int	CoapPacket::collectPacket(uns8 *rx, int len) {
	if (!packetBuffer) 
		return 0;
	else {
		index = 0;
		for (int i = 0; i < len; i++) {
			packetBuffer[index++] = rx[i];
		}
		packetBuffer[index++] = '\0';
		packetLength = index;
	}
	return 1;	
}

int CoapPacket::parsePacket() {
	int tokenLength = 0, lastDelta = 0;
	parser = 0;
	optionIndex = 0;
	optionStartIndex = 0;
	payloadIndex = 0;
	tknIndex = 0;
	hdrIndex = 0;
	numOptions = 0;
	//Find out if there are tokens (length 1 - 8)
	if (((packetBuffer[0] & 0x0F) > 0) && ((packetBuffer[0] & 0x0F) < 9)) {
		tokenLength = packetBuffer[0] & 0x0F;
		tknIndex = 4;
	}
	parser = 4;
	/*for (int i = 0; i < tokenLength; i++) {
		parser++;
	}*/
	parser += tokenLength;
	//Now, parser should be after tokens
	while (parser < packetLength) {
		if (packetBuffer[parser] != PAYLOAD) {
			if (optionStartIndex == 0)
				optionStartIndex = parser;
			uns8 optionHdr = packetBuffer[parser];
			uns16 delta = (optionHdr >> 4) & 0x000F;
			uns16 len = optionHdr & 0x000F;
			if (delta == 13) {
				parser++;
				delta = packetBuffer[parser] + 13;
			}
			else if (delta == 14) {
				parser++;
				delta = ((packetBuffer[parser++] << 8) | (packetBuffer[parser])) + 269;
			}
			lastDelta += delta;
			delta = lastDelta;
			if (len == 13) {
				parser++;
				len = packetBuffer[parser] + 13;
			}
			else if (len == 14) {
				parser++;
				len = ((packetBuffer[parser++] << 8) | (packetBuffer[parser])) + 269;
			}			
			parser++;
			parser += len;
			numOptions++;
		}
		else {
			payloadIndex = parser;
			parser = packetLength;
		}
	}
	return 1;
}

uns8* CoapPacket::getPacket() {
	if (!packetBuffer)
		return 0;
	return &packetBuffer[0];
}

int CoapPacket::getPacketLength() {
	packetLength = index;
	return packetLength;
}

int CoapPacket::getOptionCount() {
	return numOptions;
}

int CoapPacket::printPacket() {
	if (!packetBuffer) {
		Serial.println("\n\tPACKET DOESN'T EXIST\n");
		return 0;
	}
	Serial.print("\n----PACKET----\n");
	//packetLength = index;
	//Serial.print("Packetlength is");
	//Serial.print(index);
	//Serial.println(packetLength);
	for (int i = 0; i < index; i++) {
		Serial.print(packetBuffer[i], HEX);
		Serial.print(" ");
	}
	return 1;
}

int	CoapPacket::getMessageType() {
	return ((packetBuffer[hdrIndex] >> 4) & 0x03);
}

uns8 CoapPacket::getResponseCode() {
	return packetBuffer[hdrIndex + 1];
}

uns8 CoapPacket::getTokenLength() {
	return (packetBuffer[hdrIndex] & 0x0F);
}

uns8* CoapPacket::getTokens() {
	int len = packetBuffer[hdrIndex] & 0x0F;
	if ((len == 0) || (len > 8))
		return 0;
	/*uns8 tkns[len];
	for (int i = 0; i < len; i++) {
		tkns[i] = packetBuffer[tknIndex + i];
	}
	return &tkns[0];*/
	else
		return &packetBuffer[4];
}

uns16 CoapPacket::getID() {
	return ((uns16)(packetBuffer[2] << 8) | ((uns16)packetBuffer[3] & 0x00FF));
}

uns8* CoapPacket::getPayloadAddr() {
	if (packetBuffer[payloadIndex] != 0xFF)
		return 0;
	return &packetBuffer[payloadIndex];
}

void CoapPacket::printHeader() {
	uns8 vers, msgType, msgCode, tokenLength;
	uns16 msgID = ((packetBuffer[2] << 8) & 0xFF00) | (packetBuffer[3] & 0x00FF);
	vers = (packetBuffer[0] >> 6) & 0x03;
	msgType = (packetBuffer[0] >> 4) & 0x03;
	tokenLength = packetBuffer[0] & 0x0F;
	msgCode = packetBuffer[1];
	
	Serial.print("\n------HEADER------\n");
	
	Serial.print("Version\t");
	Serial.println(vers, HEX);

	Serial.print("Message Type:\t");
	if (msgType == TYPE_CON)
		Serial.println("TYPE_CON");
	else if (msgType == TYPE_NON)
		Serial.println("TYPE_NON");
	else if (msgType == TYPE_ACK)
		Serial.println("TYPE_ACK");
	else if (msgType == TYPE_RST)
		Serial.println("TYPE_RST");
	else if (msgType == TYPE_RST)
		Serial.println("TYPE_RST");
	
	Serial.print("Token Length:\t");
	Serial.println(tokenLength);

	Serial.print("Message Code:\t");
	Serial.println(msgCode);
	
	Serial.print("Message ID:\t");
	Serial.println(msgID);
}

void CoapPacket::printTokens() {
	int tokenLength = packetBuffer[0] & 0x0F;
	if (tokenLength > 0) {
		Serial.print("------TOKENS------\n");
		Serial.print("Token length:\t");
		Serial.println(tokenLength);
		for (int i = 0; i < tokenLength; i++) {
			Serial.print(packetBuffer[i + 4], HEX);
			Serial.print(" ");
		}
		Serial.println();
	}
	else
		Serial.print("\n----NO TOKENS----\n");
}

void CoapPacket::printOptions() {
	int index = optionStartIndex;
	int lastDelta = 0; 
	for (int i = 0; i < numOptions; i++) {
		uns8 optionHdr = packetBuffer[index];
		uns16 delta = (optionHdr >> 4) & 0x000F;
		uns16 len = optionHdr & 0x000F;
		if (delta == 13) {
			index++;
			delta = packetBuffer[index] + 13;
		}
		else if (delta == 14) {
			index++;
			delta = ((packetBuffer[index++] << 8) | (packetBuffer[index])) + 269;
		}
		lastDelta += delta;
		delta = lastDelta;
		
		if (len == 13) {
			index++;
			len = packetBuffer[index] + 13;
		}
		else if (len == 14) {
			index++;
			len = ((packetBuffer[index++] << 8) | (packetBuffer[index])) + 269;
		}
		Serial.print("\n------OPTION------\n");
		Serial.print("Option Number:\t");
		Serial.println(delta, DEC);
		Serial.print("Option Length:\t");
		Serial.println(len, DEC);
		Serial.print("Params:\t");
		index++;
		for (int i = 0; i < len; i++) {
			Serial.print((char)packetBuffer[index++]);
			//index++;
		}
		Serial.println();		
	}
}

void CoapPacket::printPayload() {
	if (payloadIndex > 0) {
		Serial.println("----PAYLOAD----");
		for (int i = payloadIndex; i < packetLength; i++) {
			Serial.print((char)packetBuffer[i]);
		}
		Serial.println();
	}
	else
		Serial.println("----NO PAYLOAD----");
}

int CoapPacket::readPacket() {
	printHeader();
	printTokens();
	if (numOptions > 0)
		printOptions();
	else
		Serial.println("----NO OPTIONS----");
	printPayload();
	return 1;
}

void CoapPacket::setIndex(int i) {
	packetBuffer[i] = '\0';
	this->index = i;
	this->packetLength = index;
}
/*
Serial.println("Testing index counts");
	Serial.print("Header Index:\t");
	Serial.println(hdrIndex);
	Serial.print("Token Index:\t");
	Serial.println(tknIndex);
	Serial.print("First Option Index:\t");
	Serial.println(optionStartIndex);
	Serial.print("Number of Options:\t");
	Serial.println(numOptions);
	Serial.print("Payload Index:\t");
	Serial.println(payloadIndex);
	*/





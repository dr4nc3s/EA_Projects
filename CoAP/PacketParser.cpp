#include "PacketParser.h"
#include "Arduino.h"

Parser::Parser() {
	Serial.println("Making new parser object");
	packet = NULL;
	
}

Parser::~Parser() {
	delete[] packet;
}

int Parser::begin(uns8 *packetPtr, int len) {
	if (!packet) {
		packet = packetPtr;
		packetLength = len;
		index = 0;
		header = 0;
		token = 0;
		payload = 0;
		numberOptions = 0;
		option = 0;
		lastDelta = 0;
		return 1;
	}
	else
		return 0;
}

void Parser::parseHeader() {
	uns8 vers, msgType, msgCode;
	uns16 msgID = ((packet[2] << 8) & 0xFF00) | (packet[3] & 0x00FF);
	vers = (packet[0] >> 6) & 0x03;
	msgType = (packet[0] >> 4) & 0x03;
	tokenLength = packet[0] & 0x0F;
	msgCode = packet[1];
	
	/*
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
	printMsgCode(msgCode);
	
	Serial.print("Message ID:\t");
	Serial.println(msgID);
	*/
}

void Parser::parseTokens() {
	index = 4;
	if (tokenLength > 0) {
		Serial.print("\n------TOKENS------\n");
		Serial.print("Token length:\t");
		Serial.println(tokenLength);
		for (int i = 0; i < tokenLength; i++) {
			Serial.print(packet[index++], HEX);
			Serial.print(" ");
			//index++;
		}
		Serial.println();
		token = 4;
	}
	else {
		Serial.print("\n-----NO TOKENS-----\n");
		token = 0;
	}
}

void Parser::parseOptions() {
	if (option == 0)
		option = index;
	uns8 optionHdr = packet[index];
	//optionIndices[numberOptions] = index;
	uns16 delta = (optionHdr >> 4) & 0x000F;
	uns16 len = optionHdr & 0x000F;
	//Serial.print("Last Delta sum:\t");
	//Serial.println(lastDelta);
	if (delta == 13) {
		index++;
		delta = packet[index] + 13;
	}
	else if (delta == 14) {
		index++;
		delta = ((packet[index++] << 8) | (packet[index])) + 269;
	}
	lastDelta += delta;
	delta = lastDelta;
	
	if (len == 13) {
		index++;
		len = packet[index] + 13;
	}
	else if (len == 14) {
		index++;
		len = ((packet[index++] << 8) | (packet[index])) + 269;
	}
	Serial.print("\n------OPTION------\n");
	Serial.print("Option Number:\t");
	Serial.println(delta, DEC);
	Serial.print("Option Length:\t");
	Serial.println(len, DEC);
	Serial.print("Params:\t");
	index++;
	for (int i = 0; i < len; i++) {
		Serial.print((char)packet[index++]);
		//index++;
	}	
	//Serial.println();
}

void Parser::parsePayload() {
	while (index < packetLength) {
		if ((packet[index] != PAYLOAD) && (packet[index] != '\0')) {
			//Serial.println("This part is an option");
			parseOptions();
			numberOptions++; 
		}
		else {
			payload = index;
			for (int i = index; i < packetLength; i++) {
				//Serial.print((char)packet[index]);
				index++;
			}
		}
	}
	
}

void Parser::printMsgCode(uns8 code) {
	switch(code) {
		case COAP_PING:
			Serial.println("COAP_PING");
			break;
		case COAP_GET:
			Serial.println("COAP_GET");
			break;
		case COAP_POST:
			Serial.println("COAP_POST");
			break;
		case COAP_PUT:
			Serial.println("COAP_PUT");
			break;
		case COAP_DEL:
			Serial.println("COAP_DEL");
			break;
		case CODE_CREATED: 
			Serial.println("CODE_CREATED"); 
			break;
		case CODE_DELETED: 
			Serial.println("CODE_DELETED"); 
			break;
		case CODE_VALID: 
			Serial.println("CODE_VALID"); 
			break;
		case CODE_CHANGED: 
			Serial.println("CODE_CHANGED"); 
			break;
		case CODE_CONTENT: 
			Serial.println("CODE_CONTENT"); 
			break;
		case CODE_BAD_REQUEST: 
			Serial.println("CODE_BAD_REQUEST"); 
			break;
		case CODE_UNAUTHORIZED: 
			Serial.println("CODE_UNAUTHORIZED"); 
			break;
		case CODE_BAD_OPTION: 
			Serial.println("CODE_BAD_OPTION"); 
			break;
		case CODE_FORBIDDEN: 
			Serial.println("CODE_FORBIDDEN"); 
			break;
		case CODE_NOT_FOUND: 
			Serial.println("CODE_NOT_FOUND"); 
			break;
		case CODE_NOT_ALLOWED: 
			Serial.println("CODE_NOT_ALLOWED"); 
			break;
		case CODE_NOT_ACCEPTABLE: 
			Serial.println("CODE_NOT_ACCEPTABLE"); 
			break;
		case CODE_PRECOND_FAIL: 
			Serial.println("CODE_PRECOND_FAIL"); 
			break;
		case CODE_REQ_TOO_LARGE: 
			Serial.println("CODE_REQ_TOO_LARGE"); 
			break;
		case CODE_FORMAT_UNSUPP: 
			Serial.println("CODE_FORMAT_UNSUPP"); 
			break;
		case CODE_INT_SERVER_ERR: 
			Serial.println("CODE_INT_SERVER_ERR"); 
			break;
		case CODE_NOT_IMPLEM: 
			Serial.println("CODE_NOT_IMPLEM"); 
			break;
		case CODE_BAD_GATE: 
			Serial.println("CODE_BAD_GATE"); 
			break;
		case CODE_SVC_UNAVAIL: 
			Serial.println("CODE_SVC_UNAVAIL"); 
			break;
		case CODE_GATE_TIMEOUT: 
			Serial.println("CODE_GATE_TIMEOUT"); 
			break;
		case CODE_PROXY_NSUPPORT: 
			Serial.println("CODE_PROXY_NSUPPORT"); 
			break;
	}
}

int Parser::end() {
	index = 0; 
	header = 0;
	token = 0;
	option = 0;
	payload = 0;
	packetLength = 0;
	numOptions = 0;
	tokenLength = 0;
}

int Parser::getOptionCount() {
	return numberOptions;
}

int Parser::getTokenIndex() {
	return token;
}

int Parser::getOptionIndex() {
	return option;
}

int Parser::getPayloadIndex() {
	return payload;
}

int Parser::getPacketLength() {
	return packetLength;
}
/*
uns8* Parser::getOptionIndeces() {
	if (optionIndices)
		return &optionIndices[0];
	else
		return 0;
}
*/

#ifndef __COAP-PACKET_h
#define __COAP-PACKET_h

#include "PacketParser.h"

#define		uns8		unsigned char
#define		uns16		unsigned int
#define		VERSION		0x01
#define 	MAX_SIZE	1250
#define		MAX_TOKENSIZE	8


class CoapPacket {
private:
	Parser	*parser;
	int		index;
	int		hdrIndex;
	int 	tknIndex;
	int		payloadIndex;
	int 	optionStartIndex;
	int		optionIndex;	
	uns8	packetBuffer[MAX_SIZE];
	int		packetLength;
	
	int		numOptions;
	
	uns8	lastOptionNumber;
	
	bool	processDelta(uns8 optNum);
	void	processLength(int len);
	
	void	printHeader();
	void	printTokens();
	void	printOptions();
	void	printPayload();
	

public:
	CoapPacket();
	~CoapPacket();
	
	//Accessors
	int		getPacketLength();
	uns8*	getPacket();
	
	int		begin();
	int		end();
	// Outgoing packet functions
	int		addHeader(uns8 type, uns8 code, int id);
	int		addTokens(int numTokens, uns8 *tokenValue);
	int		addPayload(int len, const char *payloadValue);
	int		addOption(uns8 optNum, int len, const char *param);
	// Incoming packet functions
	int		collectPacket(uns8 *rx, int len);
	
	uns8*	getTokens();
	uns8	getResponseCode();
	uns8*	getPayloadAddr();
	int		getMessageType();
	uns16	getID();
	int		getOptionCount();
	int		printPacket();
	//Readable coap packet
	int		parsePacket();
	int		readPacket();

};	


#endif
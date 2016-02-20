#ifndef __COAP_QUEUE_h
#define __COAP_QUEUE_h

#include "ESP8266WiFi.h"
#include "WiFiUdp.h"
#include "coap-packet.h"

#define		MAX_QUEUE_SIZE	4

#define		SPACE_EMPTY			0
#define		SPACE_DNT_CLEAR		1
#define		SPACE_CLEAR			2


class CoapQueue : public WiFiUDP {
private:
	
	uns8*			inPacket;
	const int		thisPort = 5683;
	int				portRemote;
	IPAddress		ipRemote;

	CoapPacket		rxBuffer[MAX_QUEUE_SIZE];
	int				rxStats[MAX_QUEUE_SIZE];	
	CoapPacket		txBuffer[MAX_QUEUE_SIZE];
	int				txStats[MAX_QUEUE_SIZE];
	
	int				errorCheck;
	
public:
	int		begin();
	int		addToRxBuffer(uns8 *packet, int len, int index);
	int		addToRxBuffer(int len, int index);
	int		readRxBuffer(int index);
	int		printRxPacket(int i);
	void	printPayload(int index);
	int		processPacket(int i);
	
	//Things that make is easier for caller to handle responses
	uns8*	getTX(int index);
	int		getTXLength(int index);
	int		printTX(int index);
	int		getRxID(int index);
	uns8	getRxCode(int index);
	
	/*Looks at the packet in rxBuffer[index] and determines if it needs a reply*/
	int		createReply(int index, uns8 code = 0xFF);	//Returns 0 if TYPE_NON
	int		addPayload(int len, const char *payloadValue, int index);
	
	int		sendPacket(int index);
	int		sendPacket(int index, const char ip, int portOut);
	int		receivePacket(int index);
	
};

extern CoapQueue coapQueue;
#endif
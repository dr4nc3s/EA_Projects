#ifndef __COAP_QUEUE_h
#define __COAP_QUEUE_h

#include "ESP8266WiFi.h"
#include "WiFiUdp.h"
#include "coap-packet.h"

#define		uns32				unsigned long
#define		MAX_QUEUE_SIZE		4
#define		TIME_EXPIRE			5000

#define		SPACE_EMPTY			0
#define		SPACE_DNT_CLEAR		1
#define		SPACE_CLEAR			2

/*STATUS BYTE
7 - 0 = ok to clear, 1 = do not clear
6 - 0 = packet received isn't CON, 1 = packet received is CON
5 - 0 = packet sent is not CON, 1 = packet sent is CON
4
3
2:0 - # times sent/received
*/
#define		SREG_MASK_FILLED	0x80
#define		SREG_MASK_SENDACK	0x40
#define		SREG_MASK_RCVACK	0x20
#define		SREG_MASK_NTIMES	0x07

#define		RX					1
#define		TX					0


class CoapQueue : public WiFiUDP {
private:
	const int		thisPort = 5683;
	int				portRemote;
	IPAddress		ipRemote;
	bool 			received;

	CoapPacket*		qPtr;
	uns8*			pStatusPtr;
	uns8*			qStatusPtr;
	
	CoapPacket		rxBuffer[MAX_QUEUE_SIZE];
	uns8			rxPacketStatus[MAX_QUEUE_SIZE];	
	uns8			rxQueueStatus;
	
	CoapPacket		txBuffer[MAX_QUEUE_SIZE];
	uns8			txPacketStatus[MAX_QUEUE_SIZE];
	uns8			txQueueStatus;
	
	uns32			rxTimes[MAX_QUEUE_SIZE];
	uns32			txTimes[MAX_QUEUE_SIZE];
	
	int				errorCheck;
	
	//Status checking
	inline int		numTimesTransmitted(uns8& stat);
	int				newestPacket(int a, int b, int rx = 1); //rx = 0, check txQueue.
	int				oldestPacket(int a, int b, int rx = 1); //rx = 0, check txQueue.
	
public:
	void	sendPacketSim(int index);
	int		begin();
	int		numEmptySpaces(int queue);	//1 = rx, 0 = tx
	int		numFilledSpaces(int queue); //1 = rx, 0 = tx
	int		findRxSpace();			//Values 0-4. If 4, no space avail
	void	process_rx_queue();
	void	process_tx_queue();
	void	findRepeats(int queue);
	uns8	getBufferStatus(int queue);
	void	handleAcks(int queue, int i);
	
	
	//RX Packets Queue
	int		addToRxBuffer(uns8 *packet, int len);
	int		printRxPacket(int i);
	int		processRxPacket(int i);
	uns8*	getRX(int index);
	int		getRXLength(int index);
	int		getRxID(int index);
	uns8	getRxCode(int index);
	int		clearRxQueue(int index = 4);
	
	//TX Packets Queue
	int		addToTxBuffer(uns8 *packet, int len);
	int		printTxPacket(int i);
	int		processTxPacket(int i);
	uns8*	getTX(int index);
	int		getTXLength(int index);
	int		clearTxQueue(int index = 4);	//4 = clear all
	
	//UDP Functions
	int		parseUDPPacket();
	int		receivePacket(int index);
	int		sendPacket(int index);
	int		sendPacket(int index, const char* ipAddr, int port);
	uns32	timeSent(int index);
	uns32	timeReceived(int index);
	uns8	getRXStatus(int index);
	uns8	getTXStatus(int index);
	
	
	//Replying Functions
	int		autoReply(int index);
	int		needsReply(int index);
	int		createReply(int index);
	int		addPayload(int index, int len, const char *pay);

};

extern CoapQueue coapQueue;
#endif
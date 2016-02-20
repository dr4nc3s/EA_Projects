#ifndef	__PACKETPARSER_h
#define	__PACKETPARSER_h

#include "inttypes.h"

#define 	uns8 	unsigned char
#define		uns16	unsigned int

#define 	PAYLOAD			0xFF
#define		MAX_OPTIONS		100

//Transaction types
#define TYPE_CON			0x00
#define TYPE_NON			0x01
#define TYPE_ACK			0x02
#define TYPE_RST			0x03

//Method codes
#define COAP_PING			0x00
#define COAP_GET			0x01
#define	COAP_POST			0x02
#define	COAP_PUT			0x03
#define	COAP_DEL			0x04

//Response codes - successes
#define	CODE_CREATED		0x41
#define CODE_DELETED		0x42
#define CODE_VALID			0x43
#define CODE_CHANGED		0x44
#define CODE_CONTENT		0x45
//Response codes - errors
#define CODE_BAD_REQUEST	0x80
#define CODE_UNAUTHORIZED	0x81
#define CODE_BAD_OPTION		0x82
#define CODE_FORBIDDEN		0x83
#define CODE_NOT_FOUND		0x84
#define CODE_NOT_ALLOWED	0x85
#define CODE_NOT_ACCEPTABLE	0x86
#define CODE_PRECOND_FAIL	0x8C
#define CODE_REQ_TOO_LARGE	0x8D
#define CODE_FORMAT_UNSUPP	0x8F
//Response codes - server errors
#define	CODE_INT_SERVER_ERR	0xA0
#define CODE_NOT_IMPLEM		0xA1
#define CODE_BAD_GATE		0xA2
#define CODE_SVC_UNAVAIL	0xA3
#define CODE_GATE_TIMEOUT	0xA4
#define CODE_PROXY_NSUPPORT 0xA5

//Option types
#define	OPTION_REPEAT		0
#define OPT_IF_MATCH		1
#define OPT_URI_HOST		3
#define OPT_ETAG			4
#define OPT_IF_NMATCH		5
#define OPT_URI_PORT		7
#define OPT_LOC_PATH		8
#define OPT_URI_PATH		11
#define OPT_CONTENT_FORMAT	12
#define OPT_MAX_AGE			14
#define OPT_URI_QUERY		15
#define OPT_ACCEPT			17
#define OPT_LOC_QUERY		20
#define OPT_PROXY_URI		35
#define OPT_PROXY_SCH		39
#define OPT_SIZE1			60

class Parser {
private:
	uns8	*packet;
	int		packetLength;
	int		numOptions;
	int		tokenLength;
	int		lastDelta;
	
	//Indices
	int		index;
	int		header;
	int		token;
	int		option;
	int		payload;
	//Other stuff to maintain
	int		numberOptions;
	
	void	printMsgCode(uns8 code);
	
public:
	Parser();
	~Parser();
	
	int		begin(uns8 *packetPtr, int len);
	int		end();
	void	parseHeader();
	void 	parseTokens();
	void	parseOptions();
	void	parsePayload();
	
	int		getOptionCount();
	uns8	getResponseCode();
	int		getTokenIndex();
	int		getOptionIndex();
	int		getPayloadIndex();
	int		getPacketLength();
	
};


#endif
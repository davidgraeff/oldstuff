#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <time.h>
#include <signal.h>
#include <inttypes.h>
#include "SerialPort.h"

void catch_int(int v)
{
	exit(0);
}
enum cmds {
	CMD_INITIALIZE = 0,
	CMD_GETDATA = 1,
	CMD_SETFADESTEP,
	
	CMD_SETLIGHTVALUE = 10,
	CMD_SETLIGHTVALUE_END = 19,
	CMD_FADELIGHTVALUE = 20,
	CMD_FADELIGHTVALUE_END = 29
};
const char *cmdToString[]={
	"CMD_INITIALIZE",
	"CMD_GETDATA","CMD_SETFADESTEP","CMD_SETLIGHTVALUE0","1","2","3","4","5","6","7","8","9"
	"CMD_FADELIGHTVALUE0","1","2","3","4","5","6","7","8","9"
};


void printAnswer(int r, char* input) {
	if (r<=0)
		return;
	printf("Answer: ");
	int start = -1;
	for (int i=0;i<r;++i) {
		if (r>i+1 && (uint8_t)input[i] == 255 && (uint8_t)input[i+1] == 255) {
			start = i;
			break;
		}
	}
	if (start==-1 || start+5>r) {
		printf("[No valid package] ");
		for (int i=0;i<r;++i) {
			unsigned char n = input[i];
			if ((n>='a' && n<='z') || (n>='A' && n<='Z'))
				printf("%c", n);
			else
				printf("(%u)", n);
		}
		return;
	}
	if ((unsigned int)input[3] < sizeof(cmdToString))
		printf("Senderid: %u , CMD: %s, Value: %u",  (uint8_t)input[2], cmdToString[(unsigned int)input[3]], (uint8_t)input[4]);
	printf("\n");
}

inline uint8_t getSenderID(uint8_t address, uint8_t lightid) {
	return address*4 + lightid;
}

#define UART_BROADCAST_ADDRESS 250
int main(int argc, char** argv) {
	signal(SIGINT, catch_int);
	signal(SIGTERM, catch_int);
	if (argc<=1) {
		printf("Erstes Argument muss das Gerät sein!\n");
		return -1;
	}
	
	SerialPort sp(argv[1], 115200, 0, 8, 1, false);
	if (!sp.isOpen()) {
		return -1;
	}
	//     uint8_t v1,v2;
	//     if (argc>=3)
	//         v1 = (uint8_t)atoi(argv[3]);
	//     if (argc>=4)
	//         v2 = (uint8_t)atoi(argv[4]);
	
	printf("Room Leds Test v0.1\n");
	
	char input[100] = {0};
	//char channel;
	bool doend = 1;
	uint8_t value = 2;
	
	while (doend)
	{
		
		int r = sp.readData(input, sizeof(input));
		printAnswer(r, input);
		printf("Kommandos:\n\tBeenden: q\n\tBlinken ein/aus: 3/4\n\tKanal %i aus: 1, Kanel %i an: 2\n\t"
				"Adresse zu %i ändern: b, Adresse ausgeben bei einem Teilnehmer: s, Adressen scannen: a\n", value, value, value);
		printf("\tZwischenspeicherwert {val}: %i (Ändern mit: v)\nEingabe: ", value);
		fgets(input, 3, stdin);
		switch (input[0])
		{
			case 'v':
			{
				printf("Select value (3 digits max): \n");
				fgets(input, 3, stdin);input[3] = 0;
				value = atoi(input);
				break;
			}
			case 'b':
			{
				printf("Set address to %i\n", value);
				const unsigned char t[6] = {0xff, 0xff, UART_BROADCAST_ADDRESS, CMD_SETADDRESS, value, 0x00};
				sp.writeData((char*)t,sizeof(t));
				usleep(1000*100); // 60 ms
				int r = sp.readData(input, sizeof(input));
				//if (t[1] != 0)
				printAnswer(r, input);
				break;
			}
			case 'a':
			{
				printf("Scan lights:");
				unsigned char t[6] = {0xff, 0xff, 0, CMD_INITIALIZE, 0, 0x00};
				for (int i=1;i<16;++i) {
					t[2] = i & 0x0f;
					sp.writeData((char*)t,sizeof(t));
					usleep(1000*500); // 400 ms
					int r = sp.readData(input, sizeof(input));
					//if (t[1] != 0)
					printFoundAnswer(r, input);
				}
				printf("\n");
			}
			break;
			case '1':
			{
				unsigned char t[6] = {0xff, 0xff, value , CMD_SETLIGHTVALUE+2, 0x00, 0x00};
				sp.writeData((char*)t,sizeof(t));
				usleep(1000*5); // 5 ms
			}
			break;
			case '2':
			{
				unsigned char t[6] = {0xff, 0xff, value, CMD_SETLIGHTVALUE+2, 0xff, 0x00};
				sp.writeData((char*)t,sizeof(t));
				usleep(1000*5); // 5 ms
			}
			break;
			case '3':
			{
				unsigned char t[6] = {0xff, 0xff, value , CMD_FADELIGHTVALUE+2, 0x00, 0x00};
				sp.writeData((char*)t,sizeof(t));
				usleep(1000*5); // 5 ms
			}
			break;
			case '4':
			{
				unsigned char t[6] = {0xff, 0xff, value, CMD_FADELIGHTVALUE+2, 0xff, 0x00};
				sp.writeData((char*)t,sizeof(t));
				usleep(1000*5); // 5 ms
			}
			break;
			case '5':
			{
				unsigned char t[6] = {0xff, 0xff, value, CMD_INITIALIZE, 0x01, 0x00};
				printf("Test on\n");
				sp.writeData((char*)t,sizeof(t));
				usleep(1000*5); // 5 ms
				break;
			}
			case '6':
			{
				unsigned char t[6] = {0xff, 0xff, value, CMD_INITIALIZE, 0x00, 0x00};
				printf("Test off\n");
				sp.writeData((char*)t,sizeof(t));
				usleep(1000*5); // 5 ms
				break;
			}
			case 'q':
			{
				doend = 0;
				break;
			}
			default:
				break;
		} // end input switch
		input[0] = 0;
	}
	
	return 0;
}

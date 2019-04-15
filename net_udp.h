#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock.h>
#include <time.h>

extern int PACKET_SIZE; // This should be set from network.cpp

extern SOCKADDR AddressBook[255]; /* remote address of up to 255 clients or server */
extern SOCKADDR_IN saServer; /* our address, used to bind to our socket */
extern SOCKADDR from;
extern SOCKADDR_IN Master;
extern char ReceiveBuffer[65535];
extern int ClientToAddressBook[255];
extern int isAddressBookFree[255];


int net_init (void);
void net_uninit (void);
void net_addClientToAddressBook (int ClientID);
void net_removeClientfromAddressBook (int ClientID);
short net_connect (char *serverName, short port, int ConnectionNumber);
short net_connectToMaster (char *serverName, short port);
short net_startServer (short port);
int net_CheckForDataOnSocket (int p);
int net_CheckIfWriteOk (int p);
void net_sendData (char *data, int sock, int ClientID);
void net_sendReply (char *data, int sock);
void net_sendDataToMaster (char *data, int sock);
void net_sendDataToServer (char *data, int sock);
void net_receiveData (int sock);
void net_disconnect (int sock);

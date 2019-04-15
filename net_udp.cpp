//----------------------------------
//   Low Level Network functions
// for the DerangedRaid Network code
//
//              UDP
//          version 1.0
//
//      written by Werewolf
//----------------------------------

// What does this file do? It contains the most basic network functions for both the server
// and the clients. These functions are not inside network.cpp because alternative versions of
// this low-level code (for example: using TCP instead of UDP) are possible, and desireable.
// The upper layer (network.cpp) only needs to know a few function names to be able to connect
// to servers, or get a network server running. Functions provided here include easy means of
// sending and receiving data.
//
// About the AddressBook functions:
// Using UDP, we don't need any special data for receiving packets. But for sending packets,
// the target's ip address has to be known. Since the address format may differ from protocol to
// protocol, it doesn't fit very well into network.cpp
// That, and the fact that we need a comfortable way to be able to send data to clients via the
// ClientID rather than some abstract network address, brought up the idea of the AddressBook.
// The AddressBook is simply an Array that stores network addresses. Other functions like
// net_SendData() now only need to know the ClientID of the target, and look up the address
// via the AddressBook.
// The AddressBook has to be populated with addresses by the upper layer (network.cpp)
// This is quite easy to understand: Every packet's data string should start with a valid
// "ClientID" data block. If the server receives a packet with a "Join Game" data block, it
// means that a new client has connected and wants to participate in the game.
// The reply from the server should be an "Assign ClientID" block, which tells the client about
// it's new ID on the network, and that the client has been accepted. Since the client is not
// neccessarily being accepted, it isn't practical to immediately put it into the address book.
// Instead, the server can send his reply to the client via the net_SendReply function, which
// automatically sends the packet to the last network address heard - which is usually the client
// which sent the join game request.
//
// This code has a minor flaw: UDP Packets do have an address field themselves, which we don't 
// really care about when receiving packets. Basically, we know where the data came from by
// looking at the "ClientID" data block at the beginning of the received string. For assigning
// an ID to a client, we use a special send function that uses the address of the last sending 
// client. It's not a very beatiful solution, but it should work.





/*
This is how the low level functions are to be used by a client:

    int socket;

    net_init ();
    socket = net_connect ("127.0.0.1", 1369, 0); //IP Address, port, and receiving client ID (which is always 0 if we are a client sending data to the server!!)
    if (socket==-1)
	exit (1);

    // main loop
    if (net_CheckForDataOnSocket (socket)==1)
	    net_receiveData (socket); //The received data is in ReceiveBuffer!!
    net_sendDataToServer ( DataString, socket, 0); //Data to be sent, socket, and connection number

    net_disconnect (socket);
    net_uninit ();
*/




/* This is how the server should use them:

    int socket;

    net_init ();
    socket = net_startServer (1369); //The port number
    if (socket==-1)
	exit (1);

    // main loop
    if (net_CheckForDataOnSocket (socket)==1)
	    net_receiveData (socket); //The received data is in ReceiveBuffer!!

    net_sendData ( DataString, socket, 7); //Data to be sent, socket, and receiving client ID

    net_disconnect (socket);
    net_uninit ();

In addition to that, the upper level network code has to act on incoming connections.
In short: If a new client sends a "game join" packet, the upper level should first assign
the new client a ClientID. Then a simple call to net_AddClientToAddressBook can be used to store
the client's network address. Then, the server can send packets to the client using the
net_sendData function, using the client's ClientID as an argument.

*/



#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock.h>
#include <time.h>

//#include "net_udp.h"

int PACKET_SIZE = 4096; // This should be set from network.cpp

SOCKADDR AddressBook[255]; // remote address of up to 255 clients or server
SOCKADDR_IN saServer;      // our address, used to bind to our socket
SOCKADDR from;
SOCKADDR_IN Master;
char ReceiveBuffer[65535];
int ClientToAddressBook[255];
int isAddressBookFree[255];

FILE *nDataFile = NULL;
char *nfile;

int net_init (void)
{
    WORD version = MAKEWORD(2,2);
    WSADATA data;
    int ret;
    int i;

    for (i=0;i<256;i++)
    {
	isAddressBookFree[i]=1;
	ClientToAddressBook[i]=0;
    }
    ret = WSAStartup (version, &data);

    if (data.wVersion != version)
    {	
        printf ("oh shit bad version\n");
        return(-1);
    }
    return(1);
}

void net_uninit (void)
{
    WSACleanup ();
}

unsigned long LookUpIPAddress(CHAR * inet_address)  
{

	unsigned long ulServerAddr = INADDR_NONE;
	LPHOSTENT hostentry = NULL;

	// Try parsing inet xxx.xxx.xxx.xxx format string
	ulServerAddr = inet_addr(inet_address);
	if (ulServerAddr == INADDR_NONE)  
	{
		// If the IP address parse fails, try				
		// resolving host name using hosts file
		hostentry = gethostbyname(inet_address);
			
		// At this point, the user didn't enter an IP address
		// or valid host name.  Display error message and return
		// exit.  Return FALSE to indicate failure.
		if (hostentry == NULL)
		{
			return INADDR_NONE;
		}
		// The hostname was valid.  Store the IP address
		else
		{
			ulServerAddr = *((unsigned long *)hostentry->h_addr_list[0]);
		}
	}		

	return ulServerAddr;
}


//The higher network functions must call this function after receiving the first packet
//from a newly connected client. This is because the low level network functions
//throw away the source information of incoming packets.
void net_addClientToAddressBook (int ClientID)
{
	int i;
	for (i=0;i<256;i++)
		if (isAddressBookFree[i]==1)
			break;

	ClientToAddressBook[ClientID]=i;
	isAddressBookFree[i]=0;

	AddressBook[i].sa_family = from.sa_family;
	AddressBook[i] = from;
}


void net_removeClientfromAddressBook (int ClientID)
{
	isAddressBookFree[ClientToAddressBook[ClientID]]=1;
	ClientToAddressBook[ClientID]=-1;
}


short net_connect (char *serverName, short port, int ConnectionNumber)
{
	int sock;
	u_long ulServerAddr;
		
	ulServerAddr = LookUpIPAddress(serverName);

	sock = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET)
	{
		printf ("error socket()\n");
		return -1;
	}

    saServer.sin_family = AF_INET;
    saServer.sin_addr.s_addr = ulServerAddr; 
    saServer.sin_port = htons (port);

	return (short)sock;
}


short net_connectToMaster (char *serverName, short port)
{
	int sock;
	u_long ulServerAddr;
		
	ulServerAddr = LookUpIPAddress(serverName);

	sock = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET)
	{
		printf ("error socket()\n");
		return -1;
	}

    Master.sin_family = AF_INET;
    Master.sin_addr.s_addr = ulServerAddr; 
    Master.sin_port = htons (port);

	return (short)sock;
}


short net_startServer (short port)
{
    int sock;
    int ret;
    int i;

    for (i=0;i<256;i++)
    {
	isAddressBookFree[i]=1;
	ClientToAddressBook[i]=0;
    }
    
    sock = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    if (!sock) /* Error socket was not created */
        return FALSE;


    saServer.sin_family = AF_INET;
    saServer.sin_addr.s_addr = INADDR_ANY; 
    saServer.sin_port = htons (port);

    ret = bind (sock, (SOCKADDR *)&saServer, sizeof(SOCKADDR));

    if (ret) /* There was an error binding this socket */
        return -1;

    return (short)sock;
}

int net_CheckForDataOnSocket (int p)
{
	fd_set rread;
	struct timeval to;
	int sr;

	FD_ZERO (&rread);
	FD_SET (p, &rread);	

	memset (&to, 0, sizeof (to));

	to.tv_usec = 1;
	sr = select (1, &rread, NULL, NULL, &to);
 
	if (sr < 0) /* There was an error */
    	return FALSE;

	if (sr > 0)
		if (FD_ISSET (p,&rread))
			return 1;

	return -1;
}

int net_CheckIfWriteOk (int p)
{
	fd_set rwrite;
	struct timeval to;
	int sr;

	FD_ZERO (&rwrite);
	FD_SET (p, &rwrite);	

	memset (&to, 0, sizeof (to));

	to.tv_usec = 1;
	sr = select (1, &rwrite, NULL, NULL, &to);
 
	if (sr < 0) /* There was an error */
    	return FALSE;

	if (sr > 0)
		if (FD_ISSET (p,&rwrite))
			return 1;

	return -1;
}

void net_sendData (char *data, int sock, int ClientID)
{
        sendto (sock, data, strlen (data), 0, (SOCKADDR *)&AddressBook[ClientToAddressBook[ClientID]], sizeof (SOCKADDR));
/*	nfile = "net.log";
	nDataFile = fopen(nfile,"a");
	fprintf(nDataFile,"Sent to %i: %s\n",ClientID,data);
	fclose(nDataFile);*/
}

//This is just like SendData, except that it sends a packet to the last received packet's source
void net_sendReply (char *data, int sock)
{
        sendto (sock, data, strlen (data), 0, (SOCKADDR *)&from, sizeof (SOCKADDR));
/*	nfile = "net.log";
	nDataFile = fopen(nfile,"a");
	fprintf(nDataFile,"Sent as Reply: %s\n",data);
	fclose(nDataFile);*/
}

//And now a similar function to communicate with the master game server
void net_sendDataToMaster (char *data, int sock)
{
        sendto (sock, data, strlen (data), 0, (SOCKADDR *)&Master, sizeof (SOCKADDR));
/*	nfile = "net.log";
	nDataFile = fopen(nfile,"a");
	fprintf(nDataFile,"MASTERDATA: %s\n",data);
	fclose(nDataFile);*/
}
//Again a similar function, but used only by the client
void net_sendDataToServer (char *data, int sock)
{
        sendto (sock, data, strlen (data), 0, (SOCKADDR *)&saServer, sizeof (SOCKADDR));
/*	nfile = "net.log";
	nDataFile = fopen(nfile,"a");
	fprintf(nDataFile,"Sent to Server: %s\n",data);
	fclose(nDataFile);*/
}

//This is a special function because the received data is stored in ReceiveBuffer!!
void net_receiveData (int sock)
{
	int len = sizeof (SOCKADDR);
	memset (ReceiveBuffer, '\0', PACKET_SIZE);
	recvfrom (sock, ReceiveBuffer, PACKET_SIZE, 0, &from, &len);
/*	nfile = "net.log";
	nDataFile = fopen(nfile,"a");
	fprintf(nDataFile,"Received: %s\n",ReceiveBuffer);
	fclose(nDataFile);*/
}

void net_disconnect (int sock)
{
    closesocket (sock);
}


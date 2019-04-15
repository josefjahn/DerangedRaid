//	autoupdate v1.0 for deranged raid 
//	02/2001 Bernhard Uhl 
//  
//	connects to the autoupdate server and updates files if necercary
//  Thank's to the author of GetHTTP (whoever wrote it)

#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <winsock.h>
#include <fstream.h>
#include <iostream.h>
#include <direct.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <process.h>



  
// ******************************** Filename Declaration; must be the same as in CalcCrc.cpp *********
#define VERSIONTAGOLD "version_tag_old.vtg"
#define VERSIONTAGNEW "version_tag_new.vtg"
#define SERVERNAME "www.playspoon.com"
#define FILEONSERVER "/autoupdate/"VERSIONTAGNEW

void fcopy(LPCSTR sourcefile, LPCSTR targetfile);					 
void GetHTTP(LPCSTR lpServerName, LPCSTR lpFileName);
LPCSTR removepath(LPCSTR lpFileName);
long calccrc(LPCSTR file);	
void autoupdate();		

char servername[256], file_on_server[256];	   

// Helper macro for displaying errors															
#define PRINTERROR(s) \
fprintf(stderr,"\n%: %d\n", s, WSAGetLastError()) 

void main()
{
	
	WORD wVersionRequested = MAKEWORD(1,1);
	WSADATA wsaData;
	
	int nRet;
	
	//
	// Initialize WinSock.dll
	//
	nRet = WSAStartup(wVersionRequested, &wsaData);
	if (nRet)
	{
		fprintf(stderr,"\nWSAStartup(): %d\n", nRet);
		WSACleanup();
		return;
	}
	
	//
	// Check WinSock version
	//
	if (wsaData.wVersion != wVersionRequested)
	{
		fprintf(stderr,"\nWinSock version not supported\n");
		WSACleanup();
		return;
	}
	
// **************************** Autoupdate Code in the final Main() ******************	
	strcpy(servername,SERVERNAME);											// initialize the global vars for autoupdate				
	strcpy(file_on_server,FILEONSERVER);
	
	autoupdate();															
	
	
	//
	// Release WinSock
	//
	WSACleanup();
	
	
}





void autoupdate(void)							//  Here starts the autoupdate stuff
{
	
	long crcsum,crc;
	int oldv,newv,updatev,pf;
	char operation[256],new_servername[256],argumentbuffer1[256],argumentbuffer2[256];

	strcpy(servername,SERVERNAME);											// initialize the global vars for autoupdate				
	strcpy(file_on_server,FILEONSERVER);

	printf("Connecting to AutoUpdate Server...\n");

	GetHTTP(servername,file_on_server);										// look for new version file on the autoupdate server

	ifstream OLD_file(VERSIONTAGOLD, ios::in); 								// Open the version files for reading 
	ifstream NEW_file(VERSIONTAGNEW, ios::in); 		

	OLD_file >> argumentbuffer1;											// Check if files are valid	
	NEW_file >> argumentbuffer2;  	

	if ((strcmpi(argumentbuffer1,"OldVersion:") != 0) && (strcmpi(argumentbuffer2,"NewVersion:") != 0)) 
	{
		printf("Can't connect to update server!\n");
		getch();
		WSACleanup();
		exit(0);
	}
	else {

		OLD_file >> oldv;													// Read version numbers
		NEW_file >> newv;  									

		if (oldv == newv)
			printf("No new update available\n");
		
		if (oldv < newv) {													// compare versions, only update if necessary
			printf("Updating from v%i to v%i...\n", oldv, newv);
			
			NEW_file >> new_servername;  									// read url from file
			if (strcmpi(new_servername,"url:") != 0) {
//				printf ("No servername specified, take main servername \n");
				strcpy(new_servername,servername);							// copy servername if not specified in the file 
			}
			else NEW_file >> new_servername;  								// read url from file
			
			do {															// search for the right version
				NEW_file >> argumentbuffer1;								// read version keyword 
				if (strcmpi(argumentbuffer1,"OldVersion:") == 0) {
					NEW_file >> updatev; 
					NEW_file >> crcsum;										// read the Checksum
				}
			} while ((updatev != oldv) && (strcmpi(argumentbuffer1,"end")) != 0);
			
			if (strcmpi(argumentbuffer1,"end") == 0) {						// stop if end of file 
//				printf("version too old for autoupdate\n");
			}
			else {	
				
// *** download files from server to tmp folder ***				
				_mkdir("tmp");												// create a temporary folder for files 
				_chdir("tmp");												// change directory to temporary folder
				NEW_file >> argumentbuffer1;								// read first file to download 
				
				while(strcmpi(argumentbuffer1,"{") != 0) {	
					GetHTTP(new_servername, argumentbuffer1);				// get the files from the net
					NEW_file >> argumentbuffer1;							// read next file to download
				}

// *** do the crc check ***
				NEW_file.close();


				ifstream NEW_file("../"VERSIONTAGNEW, ios::in); 			// open new version file
				updatev = 0;												// reset the updatev 
						NEW_file >> argumentbuffer1;
				
				
				do {														// goto the right version
					NEW_file >> argumentbuffer1;							// read version keyword 
					if (strcmpi(argumentbuffer1,"OldVersion:") == 0) {
						NEW_file >> updatev; 
						NEW_file >> crcsum;									// read the Checksum
					}
				} while ((updatev != oldv) && (strcmpi(argumentbuffer1,"end")) != 0);
				
				crc=0;														// reset crc 
				
				NEW_file >> argumentbuffer1;    
				while(strcmpi(argumentbuffer1,"{") != 0) {				
					crc+=calccrc(removepath(argumentbuffer1));
					NEW_file >> argumentbuffer1;							// read next file name
				}
				


				_chdir("..");												// go back to game directory 
				
				if (crc == crcsum) {


// *** process the operations specified in the version_tag file *** 
					NEW_file >> operation;									// read first operation
					
					while(strcmpi(operation,"}") != 0) {				
						if (strcmpi(operation,"mkdir") == 0) {				// create directory
							NEW_file >> argumentbuffer1;
							_mkdir(argumentbuffer1); 
						}
						else if (strcmpi(operation,"rmdir") == 0) {			// remove directory
							NEW_file >> argumentbuffer1;
							_rmdir(argumentbuffer1); 
						}
						else if (strcmpi(operation,"copy") == 0) {			// copy file
							NEW_file >> argumentbuffer1;
							NEW_file >> argumentbuffer2;
							fcopy(argumentbuffer1,argumentbuffer2);
						}
						else if (strcmpi(operation,"del") == 0) {			// delete file
							NEW_file >> argumentbuffer1;
							pf = remove(argumentbuffer1); 
						}
						else if (strcmpi(operation,"system") == 0) {		// execute file
							NEW_file >> argumentbuffer1;
//							system(argumentbuffer1);  
							_spawnlp(_P_DETACH,argumentbuffer1,argumentbuffer1,NULL);
							NEW_file.close();															// close files
							OLD_file.close();
							fcopy(VERSIONTAGNEW,VERSIONTAGOLD);											// copy new version tag to old
							pf = remove(VERSIONTAGNEW);
							return;
						}
						NEW_file >> operation;								// read next operation
					}																				
				}
				else printf("\nERROR: CRC Invalid!\n");
			}
			
// *** delete the tmp folder *** 
			NEW_file.close();
			ifstream NEW_file(VERSIONTAGNEW, ios::in); 						// reopen new version file
			_chdir("tmp");													// go back to temporary folder
			updatev = 0;													// reset the updatev 
			
			NEW_file >> argumentbuffer1;  
			do {															// goto the right version
				NEW_file >> argumentbuffer1;								// read version keyword 
				if (strcmpi(argumentbuffer1,"OldVersion:") == 0) {
					NEW_file >> updatev; 
					NEW_file >> crcsum;										// read the Checksum
				}	
			} while ((updatev != oldv) && (strcmpi(argumentbuffer1,"end")) != 0);
			
			
			NEW_file >> argumentbuffer1;    
			
			while(strcmpi(argumentbuffer1,"{") != 0) {				
				pf = remove(removepath(argumentbuffer1));					// delete files in tmp folder
				NEW_file >> argumentbuffer1;								// read next file to delete
			}
			
			_chdir("..");													// go back to temporary folder
			_rmdir("tmp");													// delete tmp folder
}
}

NEW_file.close();															// close files
OLD_file.close();	

fcopy(VERSIONTAGNEW,VERSIONTAGOLD);											// copy new version tag to old
pf = remove(VERSIONTAGNEW);

}


void fcopy(LPCSTR sourcefile, LPCSTR targetfile)							// copy a file 
{
	char buffer;
	
	ifstream source(sourcefile, ios::in|ios::nocreate|ios::binary);
	if(!source)
	{
		cerr << "\nKonnte die Datei "<< sourcefile << "  nicht öffnen";
		return;
	}

	ofstream target(targetfile, ios::out|ios::binary);
	if(!target)
	{
		cerr << "\nKonnte die Datei "<< targetfile << "  nicht öffnen";
		return;
	}
	
	while(source.get(buffer))
		target.put(buffer);
	
	source.close();
	target.close();
}

void GetHTTP(LPCSTR lpServerName,LPCSTR lpFileName)							// download a file
{
	//
	// Use inet_addr() to determine if we're dealing with a name
	// or an address
	//
	IN_ADDR iaHost;
	LPHOSTENT lpHostEntry;
	FILE * fp;
	long TotalBytes=0;
	


	fp = fopen(removepath(lpFileName), "w");								// create file without path
	
	if(fp == NULL)
	{
//		printf("File opening failed.\n");
		exit(0);
	}

	
	_setmode(_fileno(fp), _O_BINARY);										// change mode to binary, so it works for all files
	

	iaHost.s_addr = inet_addr(lpServerName);
	if (iaHost.s_addr == INADDR_NONE)
	{
		// Wasn't an IP address string, assume it is a name
		lpHostEntry = gethostbyname(lpServerName);
	}
	else
	{
		// It was a valid IP address string
		lpHostEntry = gethostbyaddr((const char *)&iaHost, 
			sizeof(struct in_addr), AF_INET);
	}
	if (lpHostEntry == NULL)
	{
//		PRINTERROR("gethostbyname()");
		return;
	}
	
	// 
	// Create a TCP/IP stream socket
	//
	SOCKET Socket; 
	
	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Socket == INVALID_SOCKET)
	{
//		PRINTERROR("socket()"); 
		return;
	}
	
	//
	// Find the port number for the HTTP service on TCP
	//
	LPSERVENT lpServEnt;
	SOCKADDR_IN saServer;
	
	lpServEnt = getservbyname("http", "tcp");
	if (lpServEnt == NULL)
		saServer.sin_port = htons(80);
	else
		saServer.sin_port = lpServEnt->s_port;
	
	//
	// Fill in the rest of the server address structure
	//
	saServer.sin_family = AF_INET;
	saServer.sin_addr = *((LPIN_ADDR)*lpHostEntry->h_addr_list);
	
	//
	// Connect the socket
	//
	int nRet;
	
	nRet = connect(Socket, (LPSOCKADDR)&saServer, sizeof(SOCKADDR_IN));
	if (nRet == SOCKET_ERROR)
	{
//		PRINTERROR("connect()");
		closesocket(Socket);
		return;
	}
	
	//
	// Format the HTTP request
	//
	char szBuffer[1024];
	
	sprintf(szBuffer, "GET %s\n", lpFileName);
	nRet = send(Socket, szBuffer, strlen(szBuffer), 0);
	if (nRet == SOCKET_ERROR)
	{
//		PRINTERROR("send()");
		closesocket(Socket); 
		return;
	}

	printf("Downloading %s: %i kB ", lpFileName, (int)(TotalBytes/1024));
	
	//
	// Receive the file contents and print to stdout
	//
	while(1)
	{
		// Wait to receive, nRet = NumberOfBytesReceived
		nRet = recv(Socket, szBuffer, sizeof(szBuffer), 0);
		if (nRet == SOCKET_ERROR)
		{
//			PRINTERROR("recv()");
			break;
		}
		
//		fprintf(stderr,"\nrecv() returned %d bytes", nRet);

		TotalBytes+=(long)nRet;

//		printf("Downloading %s: %i kB\n", lpFileName, (int)(TotalBytes/1024));
		printf(".");
		
		// Did the server close the connection?
		if (nRet == 0)
			break;
		// Write to stdout
		fwrite(szBuffer, nRet, 1, fp);
	}
	printf("\n");
	fclose(fp);
	closesocket(Socket); 
} 



LPCSTR removepath(LPCSTR lpFileName)			// remove the '/' in the filename if possible
{
	LPCSTR lpShortFileName;
	
	lpShortFileName = strrchr(lpFileName,'/');
	
	if (lpShortFileName == 0)
		lpShortFileName = lpFileName;
	else
		lpShortFileName++;
	
	return lpShortFileName;
	
}


long calccrc(LPCSTR file)					    // calc the checksum of a file 
{
	char buffer;
	long crcsum=0;
	
	
	ifstream source(file, ios::in|ios::nocreate|ios::binary);
	if(!source)
	{
		cerr << "\nKonnte die Datei "<< file << "  nicht öffnen";
		return 0;
	}
	
	
	while(source.get(buffer))
		crcsum+=buffer;
	
	source.close();
	return crcsum;
}


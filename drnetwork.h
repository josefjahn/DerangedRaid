#include "net_udp.h"
#include "tlib.h"

extern int gamesocket;
extern int mastersocket;

extern int GamePort;
extern int MasterPort;


typedef struct
{
    char Adress[100];
    char Name[256];
    int MaxClients;
    int CurClients;
    int Version;
	int isActive;
	int isClient;
	int Dedicated;
} ServerData;

typedef struct
{
    char Adress[100];
    char Name[256];
    int Version;
	int isActive;
} cl;

extern ServerData Servers[4000];		//I seriously doubt that we will see more than 4000 gameservers on the internet.

extern int numServers;

extern ServerData myServer;			//This struct stores data about OUR server.
extern char myClientName[256];




// TO ANTHONY: THIS STUFF IS INTERESTING FOR YOU
//------------------------------------------------------------------------------------------

extern char SendBuffer[65535];
extern char ReplyBuffer[65535];
extern int ReceiveBufferPointer;		//I'd use such integer variables to keep track of where we are in the buffer.
extern int SendBufferPointer;
extern int ReplyBufferPointer;

extern void Network_ParseReceiveBuffer(void);
extern int Network_EstablishConnection(char *serveraddress);
extern void Network_Disconnect(void);
extern void Heartbeat(void);
extern void Network_StartServer(void);
extern void GetServerList(void);
extern void Network_StopServer(void);
extern void Network_ClientHandler(void);
extern void Network_ServerHandler(void);

extern void NetRemoveObject(int ObjectID);
extern void NetAddObject(int LocalObjectID, int ObjectType, int ObjectMesh, int TeamNum, int TimeToLive, FxBool IsAIControlled, FxBool IsGuided, FxBool IsVisible, FxBool IsMarkerMissile, FxBool IsLocalPlayer, float xPos, float zPos, float Height, float Yaw, float Pitch, float Roll, int FiringObject);
extern void NetDamageObject(int ObjectID, int Damage, FxBool MarkObject);

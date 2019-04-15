//===============================
// Deranged Raid Network Code
//
// by: Werewolf, Crolyer, Ameise
//===============================


#include "net_udp.h"
#include "derangedraid.h"
#include "drnetwork.h"
#include "tlib.h"

int gamesocket;
int mastersocket;

int GamePort = 1369;
int MasterPort = 1370;

unsigned long HeartbeatDelay;
unsigned long ClientNetDelay;
float netmaxdist = 10000.0f; // Max visible range for networked objects

char DENOMINATOR_ADD_OBJECT[2] = "1";
char DENOMINATOR_DEL_OBJECT[2] = "2";
char DENOMINATOR_UPD_OBJECT[2] = "3";
char DENOMINATOR_ADD_PARTICLEEMITTER[2] = "4";
char DENOMINATOR_DEL_PARTICLEEMITTER[2] = "5";
char DENOMINATOR_ADD_LIGHT[2] = "6";
char DENOMINATOR_DEL_LIGHT[2] = "7";
char DENOMINATOR_DAMAGEINFORMATION[2] = "8";
char DENOMINATOR_MAPINFORMATION[2] = "9";
char DENOMINATOR_DAYTIME[2] = "A";
char DENOMINATOR_CLIENTID[2] = "B";
char DENOMINATOR_PLAYERJOIN[2] = "C";
char DENOMINATOR_PLAYERLEAVE[2] = "D";
char DENOMINATOR_ASSIGN_CLIENTID[2] = "E";
char DENOMINATOR_CHAT[2] = "F";
char DENOMINATOR_TERRAINSETUP[2] = "G";
char DENOMINATOR_GAMESETUP[2] = "H";
char DENOMINATOR_JOINREQUEST[2] = "J";
char DENOMINATOR_LISTENTRY[2] = "W";
char DENOMINATOR_MASTERREPLY[2] = "X";
char DENOMINATOR_LISTREQUEST[2] = "Y";
char DENOMINATOR_HEARTBEAT[2] = "Z";


char ts[256]; //Temporary string variable

//cl Client[256];


void NetLog(char *msg)
{
	FILE *DataFile = NULL;
	DataFile = fopen("network.log","a");
	if (DataFile == NULL)
		return;
	fprintf(DataFile,msg);
	fprintf(DataFile,"\n");
	fclose(DataFile);

}


//Returns true if client ClientID can see object ObjectID
FxBool IsVisibleViaNetwork(int ObjectID, int ClientID)
{
	float vxdist, vydist, vzdist;
	if (Objects[ObjectID].ObjectId != ObjectID)
		return (FXFALSE);

	if (Objects[ObjectID].xPos > Objects[Player[ClientID].ControlledObject].xPos)
		vxdist = (Objects[ObjectID].xPos - Objects[Player[ClientID].ControlledObject].xPos);
	else
		vxdist = (Objects[Player[ClientID].ControlledObject].xPos - Objects[ObjectID].xPos);

	if (Objects[ObjectID].Height > Objects[Player[ClientID].ControlledObject].Height)
		vydist = (Objects[ObjectID].Height - Objects[Player[ClientID].ControlledObject].Height);
	else
		vydist = (Objects[Player[ClientID].ControlledObject].Height - Objects[ObjectID].Height);
           
	if (Objects[ObjectID].zPos > Objects[Player[ClientID].ControlledObject].zPos)
		vzdist = (Objects[ObjectID].zPos - Objects[Player[ClientID].ControlledObject].zPos);
	else
		vzdist = (Objects[Player[ClientID].ControlledObject].zPos - Objects[ObjectID].zPos);

		if (vxdist<0)
			vxdist=(-1.0f)*vxdist;
		if (vydist<0)
			vydist=(-1.0f)*vydist;
		if (vzdist<0)
			vzdist=(-1.0f)*vzdist;

		if ((vxdist<netmaxdist) && (vydist<netmaxdist) && (vzdist<netmaxdist))
			return (FXTRUE);
		else
			return (FXFALSE);
		
}



// TO ANTHONY: THIS STUFF IS INTERESTING FOR YOU
//------------------------------------------------------------------------------------------

char SendBuffer[65535];
char ReplyBuffer[65535];
int ReceiveBufferPointer;		//I'd use such integer variables to keep track of where we are in the buffer.
int SendBufferPointer;
int ReplyBufferPointer;

//This are the string operation functions someone should write someday :)
//-----------------------------------
void ClearSendBuffer(void)
{
sprintf(SendBuffer, "\0");
SendBufferPointer=0;
}
void ClearReplyBuffer(void) //The reply buffer is only used during the client-join phase
{
sprintf(ReplyBuffer, "\0");
ReplyBufferPointer=0;
}

//The "Encode" stuff should just add the things we want into the SendBuffer string
void EncodeFloat(float data)
{
char Buffer[1000];
sprintf(Buffer, "%f ", data);
strcat(SendBuffer, Buffer);
}

void EncodeInt(int data)
{
char Buffer[1000];
sprintf(Buffer, "%i ", data);
strcat(SendBuffer, Buffer);
}

void EncodeBool(FxBool data)
{
char Buffer[1000];
int data1;
if (data)
	data1 = 1;
else
	data1 = 0;
sprintf(Buffer, "%i ", data1);
strcat(SendBuffer, Buffer);
}

void EncodeIntReply(int data) //Same as EncodeInt, but adding the contents to ReplyBuffer!
{
char Buffer[1000];
sprintf(Buffer, "%i ", data);
strcat(ReplyBuffer, Buffer);
}

void EncodeString(char *data)
{
char Buffer[1000];
sprintf(Buffer, "%s ", data);
strcat(SendBuffer, Buffer);
}

void EncodeDenominator(char *data)
{
char Buffer[1000];
//Buffer[0]=data;
//Buffer[1]=' ';
//Buffer[2]=0;
sprintf(Buffer, "%s ", data);
strcat(SendBuffer, Buffer);
}

void EncodeDenominatorReply(char *data) //Same as above, but adding to the ReplyBuffer!
{
char Buffer[1000];
sprintf(Buffer, "%s ", data);
strcat(ReplyBuffer, Buffer);
}





int FindCharInString(char *s, char c)
{
	int m;
	for (m=0; (unsigned int)(m)<strlen(s); m++)
	{
		if (s[m]==c)
			break;
	}
return m;
}


void CutoffFirstWord(void)
{
int i;

//FILE *nDataFile = NULL;
//char *nfile;
int start = FindCharInString(ReceiveBuffer, ' ')+1;


  memset(tempstring, '\0', 256); 
  strncpy(tempstring, ReceiveBuffer, start); 

  for (i=start; i<=(signed int)(strlen(ReceiveBuffer)); i++)
  {
    ReceiveBuffer[i-start] = ReceiveBuffer[i];
  }
  ReceiveBuffer[i-start] = '\0';

//  nfile = "net.log";
//  nDataFile = fopen(nfile,"a");
//  fprintf(nDataFile,"Cutoff: [%s] [%s]\n",tempstring, ReceiveBuffer);
//  fclose(nDataFile);

}


//The "Decode" functions use the BufferPointer variable to know where they are in the
//ReceiveBuffer, and try to decode the next few characters into numbers and strings.

float DecodeFloat(void)
{
float value = 0.0f;
CutoffFirstWord();
sscanf(tempstring, "%f ", &value);
return(value);
}

int DecodeInt(void)
{
int value = 0;
CutoffFirstWord();
sscanf(tempstring, "%i ", &value);
return (value);
}

FxBool DecodeBool(void)
{
int value = 0;
CutoffFirstWord();
sscanf(tempstring, "%i ", &value);
if (value == 1)
	return (FXTRUE);
else
	return (FXFALSE);
}

void DecodeString(void)
{
CutoffFirstWord();
sscanf(tempstring, "%s ", ts);
}

char DecodeDenominator(void)
{
CutoffFirstWord();
return (tempstring[0]);
}

//------------------------------------------------------------------------------------------


//Inform other clients about a joining client
void NotifyOtherClients(int ClientID, int TeamNum, char *ClientName)
{
	int i;
	ClearSendBuffer();
	EncodeDenominator(DENOMINATOR_PLAYERJOIN);
	EncodeInt(ClientID);
	EncodeString(ClientName);
	EncodeInt(TeamNum);
	for (i=0; i<myServer.MaxClients; i++)
	{
		if ((Player[i].isActive) && (Player[i].isNetwork) && (i != localplayer) && (i != ClientID))
			net_sendData( SendBuffer, gamesocket, i);
	}

	//Now tell the new client about our other players
	for (i=0; i<myServer.MaxClients; i++)
	{
		if ((Player[i].isActive) && (i != ClientID))
		{
			ClearSendBuffer();
			EncodeDenominator(DENOMINATOR_PLAYERJOIN);
			EncodeInt(i);
			EncodeString(Player[i].PlayerName);
			EncodeInt(Player[i].Team);
			net_sendData( SendBuffer, gamesocket, ClientID);
		}
	}
}


void NotifyOthersOfDamage(int SendingClient, int IDAtOwningClient, int OwningNetworkClient, int AppliedDamage, FxBool MarkObject)
{
	int i;
	ClearSendBuffer();
	EncodeDenominator(DENOMINATOR_DAMAGEINFORMATION);
	EncodeInt(SendingClient);
	EncodeInt(OwningNetworkClient);
	EncodeInt(IDAtOwningClient);
	EncodeInt(AppliedDamage);
	EncodeBool(MarkObject);
	for (i=0; i<myServer.MaxClients; i++)
	{
		if ((Player[i].isActive) && (Player[i].isNetwork) && (i != localplayer) && (i != OwningNetworkClient))
			net_sendData( SendBuffer, gamesocket, i);
		//TODO: Also don't notify the sender of the damage info, since it could be someone other than OwningNetworkClient!
	}

}


void NotifyOthersOfDelete(int IDAtOwningClient, int OwningNetworkClient)
{
	int i;

	return;

	ClearSendBuffer();
	EncodeDenominator(DENOMINATOR_DEL_OBJECT);
	EncodeInt(OwningNetworkClient);
	EncodeInt(IDAtOwningClient);
	for (i=0; i<myServer.MaxClients; i++)
	{
		if ((Player[i].isActive) && (Player[i].isNetwork) && (i != localplayer) && (i != OwningNetworkClient))
			net_sendData( SendBuffer, gamesocket, i);
		//TODO: Also don't notify the sender of the delete info, since it could be someone other than OwningNetworkClient!
	}

}

void NotifyOthersOfClientLeave(int TempClientID)
{
	int i;

	ClearSendBuffer();
	EncodeDenominator(DENOMINATOR_PLAYERLEAVE);
	EncodeInt(TempClientID);
	for (i=0; i<myServer.MaxClients; i++)
	{
		if ((Player[i].isActive) && (Player[i].isNetwork) && (i != localplayer) && (i != TempClientID))
			net_sendData( SendBuffer, gamesocket, i);
	}


}



//server informed us about a joining client, so add him to our local list
void AddNewClient(int ClientID, char *ClientName, int TeamNum, FxBool isLocalPlayer)
{
	Player[ClientID].isActive=FXTRUE;
	Player[ClientID].isAI=FXFALSE;
	Player[ClientID].isHuman=FXTRUE;
	if (isLocalPlayer)
	{
		localplayer = ClientID;
	}
	Player[ClientID].isNetwork=!isLocalPlayer;
	sprintf(Player[ClientID].PlayerName, "%s", ClientName);
	Player[ClientID].isTank=FXFALSE;
	Player[ClientID].VehicleModel=OBJECTMESH_SHIP1;
	Player[ClientID].Team=TeamNum;
	Team[TeamNum].PlayersInTeam++;
	Player[ClientID].doRespawn=FXTRUE;
	sprintf(tempstring, "%s has joined the game", ClientName);
	AddMessage(tempstring, 255, 255, 0);

	numPlayers++;
}


//Used by the client, adds object updates for one local object to the sendbuffer
void UpdateObject(int ObjectID)
{
	EncodeDenominator(DENOMINATOR_UPD_OBJECT);
	EncodeInt(Objects[ObjectID].OwningNetworkClient);
	EncodeInt(Objects[ObjectID].IDAtOwningClient);
	EncodeFloat(Objects[ObjectID].Speed);
	EncodeFloat(Objects[ObjectID].xPos);
	EncodeFloat(Objects[ObjectID].zPos);
	EncodeFloat(Objects[ObjectID].Height);
	EncodeFloat(Objects[ObjectID].Yaw);
	EncodeFloat(Objects[ObjectID].Pitch);
	EncodeFloat(Objects[ObjectID].Roll);
	EncodeInt(Objects[ObjectID].targetObject);
	EncodeInt(Objects[ObjectID].isMarked);

}


//Used by the server, adds object updates for one client to the sendbuffer
void UpdateObjectOnClient(int ObjectID, int ClientID)
{
	UpdateObject(ObjectID); //Could this work?? i dunno yet...
}


//This function is used by the object owner (a client) to inform the server.
//The server uses his own functions, and sends ADD_OBJECT in the visibility check
void NetAddObject(int LocalObjectID, int ObjectType, int ObjectMesh, int TeamNum, int TimeToLive, FxBool IsAIControlled, FxBool IsGuided, FxBool IsVisible, FxBool IsMarkerMissile, FxBool IsLocalPlayer, float xPos, float zPos, float Height, float Yaw, float Pitch, float Roll, int FiringObject)
{
	if ((myServer.isActive == FXFALSE) && (myServer.isClient)) //But only if we're a client!
	{
			sprintf(tempstring, "SEND: AddObject for Server: local=%i remote=%i owner=%i mesh=%i", LocalObjectID, Objects[LocalObjectID].IDAtOwningClient, Objects[LocalObjectID].OwningNetworkClient, ObjectMesh);
			NetLog(tempstring);

			EncodeDenominator(DENOMINATOR_ADD_OBJECT);
			EncodeInt	(ObjectType);
			EncodeInt	(ObjectMesh);
			EncodeInt	(TeamNum);
			EncodeInt	(TimeToLive);
			EncodeBool	(IsAIControlled);
			EncodeBool	(IsGuided);
			EncodeBool	(IsVisible);
			EncodeBool	(IsMarkerMissile);
			EncodeFloat	(xPos);
			EncodeFloat	(zPos);
			EncodeFloat	(Height);
			EncodeFloat	(Yaw);
			EncodeFloat	(Pitch);
			EncodeFloat	(Roll);
			EncodeInt	(FiringObject);
			EncodeInt	(Objects[LocalObjectID].OwningNetworkClient);	// should be OwningNetworkClient
			EncodeInt	(Objects[LocalObjectID].IDAtOwningClient);		// should be IDAtOwningClient
			
			//I believe this code works. Previous statement is valid for the whole program. I think.

	}
			Objects[LocalObjectID].OwningNetworkClient = localplayer;
			Objects[LocalObjectID].IDAtOwningClient = LocalObjectID;

}


//This function is used by the object owner to inform the server.
//The server also uses this, for his own objects respectively
void NetRemoveObject(int ObjectID)
{
		if ((myServer.isActive) || (Objects[ObjectID].OwningNetworkClient==localplayer))
		{
			sprintf(tempstring, "SEND: Remove Object=%i remote=%i owner=%i mesh=%i", ObjectID, Objects[ObjectID].IDAtOwningClient, Objects[ObjectID].OwningNetworkClient, Objects[ObjectID].ObjectMesh);
			NetLog(tempstring);
			EncodeDenominator(DENOMINATOR_DEL_OBJECT);
			EncodeInt(Objects[ObjectID].OwningNetworkClient);
			EncodeInt(Objects[ObjectID].IDAtOwningClient);
		}
		else
		{
			sprintf(tempstring, "SEND [SUPRESSED]: Remove Object=%i remote=%i owner=%i mesh=%i", ObjectID, Objects[ObjectID].IDAtOwningClient, Objects[ObjectID].OwningNetworkClient, Objects[ObjectID].ObjectMesh);
			NetLog(tempstring);
		}

}


void NetDamageObject(int ObjectID, int Damage, FxBool MarkObject)
{
		sprintf(tempstring, "SEND: Damage Object=%i remote=%i owner=%i mesh=%i damage=%i", ObjectID, Objects[ObjectID].IDAtOwningClient, Objects[ObjectID].OwningNetworkClient, Objects[ObjectID].ObjectMesh, Damage);
		NetLog(tempstring);
		EncodeDenominator(DENOMINATOR_DAMAGEINFORMATION);
		EncodeInt(localplayer);
		EncodeInt(Objects[ObjectID].OwningNetworkClient);
		EncodeInt(Objects[ObjectID].IDAtOwningClient);
		EncodeInt(Damage);
		EncodeBool(MarkObject);
}


void Network_ParseReceiveBuffer(void)
{
char TempDenominator;
int TempClientID;
int i, fu;
int returnID;
int ObjectID;
int myClientID;

ReceiveBufferPointer=0;
TempClientID=-1;
   do
   {
	TempDenominator=DecodeDenominator();

	
	if (TempDenominator == DENOMINATOR_CLIENTID[0])
	{
		TempClientID = DecodeInt();
		sprintf(tempstring, "RECV: DENOMINATOR_CLIENTID id=%i", TempClientID);
		NetLog(tempstring);
	}

	//If it's a chat message and we should hear it, show it on our screen
	else if (TempDenominator == DENOMINATOR_CHAT[0])
	{
		int ListeningTeam = DecodeInt();
		if ((ListeningTeam == Player[localplayer].Team) || (ListeningTeam == -1))
		{
			DecodeString();
			DisplayChatMessage(ts);
		}
		else
			DecodeString(); //else disregard it.

		sprintf(tempstring, "RECV: DENOMINATOR_CHAT team=%i message=%s", ListeningTeam, ts);
		NetLog(tempstring);

	}

	//If we are a client, we can be given a clientID by the server
	else if (TempDenominator == DENOMINATOR_ASSIGN_CLIENTID[0])
	{
		int tempTeam;
		if (myServer.isActive)
		{
			myClientID = DecodeInt();
			tempTeam = DecodeInt();
			AddNewClient(myClientID, myClientName, tempTeam, FXTRUE);
		}
		else
		{
			DecodeInt(); //If we are not the server, disregard it. You'll see a lot of this going on here.
			DecodeInt();
		}

		sprintf(tempstring, "RECV: DENOMINATOR_ASSIGN_CLIENTID id=%i team=%i", myClientID, tempTeam);
		NetLog(tempstring);

	}

	//As a server, we should respond to incoming connections by assigning them a clientID
	else if (TempDenominator == DENOMINATOR_JOINREQUEST[0])
	{
		if (myServer.isActive)
		{
			int remoteVersion = DecodeInt();
			DecodeString();
			char *PlayerName = ts;
			int tempTeam;
			if (remoteVersion == myServer.Version)
			{
				returnID = GetNextFreeClientID();
				tempTeam = GetNextAutobalanceTeam();
				if (returnID != -1)
				{
					net_addClientToAddressBook(returnID);
					TempClientID = returnID;
					AddNewClient( TempClientID, ts, tempTeam, FXFALSE);
					Player[returnID].LastNetUpdate = timeGetTime();
					myServer.CurClients++;
				}
			}
			else
			{
				returnID = -1;
				tempTeam = -1;
			}

			ClearReplyBuffer();
			EncodeDenominatorReply(DENOMINATOR_ASSIGN_CLIENTID);

			EncodeIntReply(returnID);
			EncodeIntReply(tempTeam);

			net_sendReply(ReplyBuffer, gamesocket);

			if (returnID != -1)
			{
				//Send Terrain Seed and Type
				ClearSendBuffer();
				EncodeDenominator(DENOMINATOR_TERRAINSETUP);
				EncodeInt(RandomSeed);
				EncodeInt(TerrainType);
				EncodeInt(0);	//TODO: Waterlevel

				//Send Weather Data
				EncodeDenominator(DENOMINATOR_MAPINFORMATION);
				EncodeInt(current_weather.WeatherType);
				EncodeInt(current_weather.WeatherPrecipitation);
				EncodeFloat(current_weather.PrecipitationStrength);
				EncodeInt(current_weather.ThunderAndLightning);

				//Send Time Of Day
				EncodeDenominator(DENOMINATOR_DAYTIME);
				EncodeInt((int)(daytime));

				//Send CommandCenter positions
				ObjectID = getObject(0, OBJECTTYPE_BUILDING, OBJECTMESH_COMMANDCENTER, 1);
				EncodeDenominator(DENOMINATOR_ADD_OBJECT);
				EncodeInt(Objects[ObjectID].ObjectType);
				EncodeInt(Objects[ObjectID].ObjectMesh);
				EncodeInt(Objects[ObjectID].Team);
				EncodeInt(Objects[ObjectID].TimeToLive);
				EncodeBool(Objects[ObjectID].isAIControlled);
				EncodeBool(Objects[ObjectID].isGuided);
				EncodeBool(Objects[ObjectID].isVisible);
				EncodeBool(Objects[ObjectID].isMarkerMissile);
				EncodeFloat(Objects[ObjectID].xPos);
				EncodeFloat(Objects[ObjectID].zPos);
				EncodeFloat(Objects[ObjectID].Height);
				EncodeFloat(Objects[ObjectID].Yaw);
				EncodeFloat(Objects[ObjectID].Pitch);
				EncodeFloat(Objects[ObjectID].Roll);
				EncodeInt(Objects[ObjectID].FiringObject);
				EncodeInt(Objects[ObjectID].OwningNetworkClient);
				EncodeInt(Objects[ObjectID].IDAtOwningClient);
				Objects[ObjectID].isVisibleToClient[TempClientID]=FXTRUE;

				ObjectID = getObject(1, OBJECTTYPE_BUILDING, OBJECTMESH_COMMANDCENTER, 1);
				EncodeDenominator(DENOMINATOR_ADD_OBJECT);
				EncodeInt(Objects[ObjectID].ObjectType);
				EncodeInt(Objects[ObjectID].ObjectMesh);
				EncodeInt(Objects[ObjectID].Team);
				EncodeInt(Objects[ObjectID].TimeToLive);
				EncodeBool(Objects[ObjectID].isAIControlled);
				EncodeBool(Objects[ObjectID].isGuided);
				EncodeBool(Objects[ObjectID].isVisible);
				EncodeBool(Objects[ObjectID].isMarkerMissile);
				EncodeFloat(Objects[ObjectID].xPos);
				EncodeFloat(Objects[ObjectID].zPos);
				EncodeFloat(Objects[ObjectID].Height);
				EncodeFloat(Objects[ObjectID].Yaw);
				EncodeFloat(Objects[ObjectID].Pitch);
				EncodeFloat(Objects[ObjectID].Roll);
				EncodeInt(Objects[ObjectID].FiringObject);
				EncodeInt(Objects[ObjectID].OwningNetworkClient);
				EncodeInt(Objects[ObjectID].IDAtOwningClient);
				Objects[ObjectID].isVisibleToClient[TempClientID]=FXTRUE;

				net_sendData (SendBuffer, gamesocket, TempClientID);

				NotifyOtherClients(TempClientID, tempTeam, ts);
			}

		}
		else
		{
			DecodeInt();
			DecodeString();
		}

		sprintf(tempstring, "RECV: DENOMINATOR_JOINREQUEST id=%i", returnID);
		NetLog(tempstring);

			
	}
	//If a client wants to disconnect, let them go in peace
	else if (TempDenominator == DENOMINATOR_PLAYERLEAVE[0])
	{
		TempClientID = DecodeInt();
		if ((myServer.isActive == FXTRUE) && (TempClientID != -1))
		{
			net_removeClientfromAddressBook(TempClientID);

			//The next function has to assign each of the left player's objects a
			//new client who they belong to. For every object, he also has to send a
			//"DeleteObject" Denominator to the network, and then an "AddObject"
			//Denominator together with the new owning Client's ID imprinted on the 
			//object. Basically, we hand over ownership of the objects to the next
			//client in the same team as the client who just left the game.

			sprintf(tempstring, "RECV: DENOMINATOR_PLAYERLEAVE id=%i", TempClientID);
			NetLog(tempstring);
			myServer.CurClients--;
			numPlayers--;

			FindNewOwnerForObjects(TempClientID);
			NotifyOthersOfClientLeave(TempClientID);
		}
	}
	//This is the main object updater
	else if (TempDenominator == DENOMINATOR_UPD_OBJECT[0])
	{
		int OwningNetworkClient = DecodeInt();
		int IDAtOwningClient = DecodeInt();
		int foundStuff=0;
		sprintf(tempstring, "RECV: DENOMINATOR_UPD_OBJECT remote=%i owner=%i", IDAtOwningClient, OwningNetworkClient);
		NetLog(tempstring);

		for (i=0; i<=NumObjects; i++)
		{
			//See if it's the same object that we want to update
			if ((Objects[i].OwningNetworkClient == OwningNetworkClient) && (Objects[i].IDAtOwningClient == IDAtOwningClient) && (Objects[i].ObjectId != -1))
			{
				Objects[i].Speed = DecodeFloat();
				Objects[i].xPos = DecodeFloat();
				Objects[i].zPos = DecodeFloat();
				Objects[i].Height = DecodeFloat();
				Objects[i].Yaw = DecodeFloat();
				Objects[i].Pitch = DecodeFloat();
				Objects[i].Roll = DecodeFloat();

				Objects[i].targetObject = DecodeInt();
				Objects[i].isMarked = DecodeBool();

				foundStuff=1;
				sprintf(tempstring, "Found Object at %i !", i);
				NetLog(tempstring);
				break;
			}
		}

		if (foundStuff==0)
		{
			DecodeFloat();
			DecodeFloat();
			DecodeFloat();
			DecodeFloat();
			DecodeFloat();
			DecodeFloat();
			DecodeFloat();
			DecodeInt();
			DecodeBool();
			sprintf(tempstring, "CRITICAL UPDATE ERROR! Could not find Object to be updated in my list!!");
			NetLog(tempstring);
		}


	}
	//Create an object
	else if (TempDenominator == DENOMINATOR_ADD_OBJECT[0])
	{
		int curCl;
		int ObjectType = DecodeInt();
		int ObjectMesh = DecodeInt();
		int TeamNum = DecodeInt();
		int TimeToLive = DecodeInt();
		FxBool IsAIControlled = DecodeBool();
		FxBool IsGuided = DecodeBool();
		FxBool IsVisible = DecodeBool();
		FxBool IsMarkerMissile = DecodeBool();
		float xPos = DecodeFloat();
		float zPos = DecodeFloat();
		float Height = DecodeFloat();
		float Yaw = DecodeFloat();
		float Pitch = DecodeFloat();
		float Roll = DecodeFloat();
		int FiringObject = DecodeInt();
		i = AddObject(ObjectType, ObjectMesh, TeamNum, TimeToLive, IsAIControlled, IsGuided, IsVisible, IsMarkerMissile, FXFALSE, xPos, zPos, Height, Yaw, Pitch, Roll, FiringObject, FXFALSE );
		//We don't take any chances, so we let them send us all the neccessary data...
		Objects[i].OwningNetworkClient = DecodeInt();
//		Objects[i].ControllingPlayer = Objects[i].OwningNetworkClient;
		Objects[i].IDAtOwningClient = DecodeInt();
		if (myServer.isActive)
			for (curCl=0; curCl<myServer.MaxClients; curCl++)
				Objects[i].isVisibleToClient[curCl] = FXFALSE;

		sprintf(tempstring, "RECV: DENOMINATOR_ADD_OBJECT local=%i remote=%i owner=%i", i, Objects[i].IDAtOwningClient, Objects[i].OwningNetworkClient);
		NetLog(tempstring);

	
//Debug output
for (fu=0; fu<NumObjects; fu++)
{
	if (Objects[fu].ObjectId == fu)
	{
		sprintf(tempstring, "Object ID=%i Type=%i Mesh=%i Team=%i OwningNetworkClient=%i IDAtOwningClient=%i TTL=%i", 
				Objects[fu].ObjectId,
				Objects[fu].ObjectType,
				Objects[fu].ObjectMesh,
				Objects[fu].Team,
				Objects[fu].OwningNetworkClient,
				Objects[fu].IDAtOwningClient,
				Objects[fu].TimeToLive);
		NetLog(tempstring);

	}
}

	}
	//Delete an Object
	else if (TempDenominator == DENOMINATOR_DEL_OBJECT[0])
	{
		int curCl;
		int OwningNetworkClient = DecodeInt();
		int IDAtOwningClient = DecodeInt();
		sprintf(tempstring, "RECV: DENOMINATOR_DEL_OBJECT remote=%i owner=%i", IDAtOwningClient, OwningNetworkClient);
		NetLog(tempstring);
		for (i=0; i<=NumObjects; i++)
		{
			//See if it's the same object that we want to delete
			if ((Objects[i].OwningNetworkClient == OwningNetworkClient) && (Objects[i].IDAtOwningClient == IDAtOwningClient) && (Objects[i].ObjectId != -1))
			{

				if (myServer.isActive)
					for (curCl=0; curCl<myServer.MaxClients; curCl++)
						Objects[i].isVisibleToClient[curCl] = FXFALSE;
				sprintf(tempstring, "Found Object at %i, mesh=%i!", i, Objects[i].ObjectMesh);
				NetLog(tempstring);
				RemoveObject(i, FXFALSE);
				break;
			}
		}
		if (myServer.isActive)
			NotifyOthersOfDelete(IDAtOwningClient, OwningNetworkClient);
	}
	//Damage an Object
	else if (TempDenominator == DENOMINATOR_DAMAGEINFORMATION[0])
	{
		int SendingClient= DecodeInt();
		int OwningNetworkClient = DecodeInt();
		int IDAtOwningClient = DecodeInt();
		int AppliedDamage = DecodeInt();
		FxBool MarkObject = DecodeBool();
		sprintf(tempstring, "RECV: DENOMINATOR_DAMAGEINFORMATION remote=%i owner=%i damage=%i", IDAtOwningClient, OwningNetworkClient, AppliedDamage);
		NetLog(tempstring);
		for (i=0; i<=NumObjects; i++)
		{
			//See if it's the same object that we want to damage
			if ((Objects[i].OwningNetworkClient == OwningNetworkClient) && (Objects[i].IDAtOwningClient == IDAtOwningClient) && (Objects[i].ObjectId != -1))
			{
				sprintf(tempstring, "Found Object at %i !", i);
				NetLog(tempstring);
				DamageObject( i, AppliedDamage, MarkObject, FXFALSE);
				break;
			}
		}
		if (myServer.isActive)
			NotifyOthersOfDamage(SendingClient, IDAtOwningClient, OwningNetworkClient, AppliedDamage, MarkObject);
	}
	else if (TempDenominator == DENOMINATOR_PLAYERJOIN[0])
	{
		int tempId = DecodeInt();
		DecodeString();
		int tempTeam = DecodeInt();
		AddNewClient( tempId, ts, tempTeam, FXFALSE);
		sprintf(tempstring, "RECV: DENOMINATOR_PLAYERJOIN id=%i name=%s team=%i", tempId, ts, tempTeam);
		NetLog(tempstring);
	}
	else if (TempDenominator==DENOMINATOR_TERRAINSETUP[0])
	{
		int tempWarterlevel;
		RandomSeed = DecodeInt();
		TerrainType = DecodeInt();
		tempWarterlevel = DecodeInt();
		sprintf(tempstring, "RECV: DENOMINATOR_TERRAINSETUP");
		NetLog(tempstring);
	}	
	else if (TempDenominator==DENOMINATOR_MAPINFORMATION[0])
	{
		current_weather.WeatherType = DecodeInt();
		current_weather.WeatherPrecipitation = DecodeInt();
		current_weather.PrecipitationStrength = DecodeFloat();
		current_weather.ThunderAndLightning = DecodeInt();
		sprintf(tempstring, "RECV: DENOMINATOR_MAPINFORMATION");
		NetLog(tempstring);
	}
	else if (TempDenominator==DENOMINATOR_DAYTIME[0])
	{
		daytime = (float)(DecodeInt());
		sprintf(tempstring, "RECV: DENOMINATOR_DAYTIME daytime=%i", (int)(daytime));
		NetLog(tempstring);
	}


   } while ((signed)(strlen(ReceiveBuffer)) > 1);
}








//--------------------------------------------------------------------------------
// This function is the first network function being called to make a connection
// RETURNS: Client ID if successfull, negative values if not (contains error information)
// -1 = Timeout
// -2 = Server full
// -4 = Wrong password
// -8 = Client's IP is banned
//--------------------------------------------------------------------------------
int Network_EstablishConnection(char *serveraddress)
{
unsigned int timeout = 0;
int TempDenominator;
int ConnectionPhase;
int myClientID;

gamesocket = net_connect (serveraddress, GamePort, 0);
if (gamesocket==-1)
  exit (1);

//sprintf(myClientName, "Werewolf");

ClearSendBuffer();
EncodeDenominator(DENOMINATOR_JOINREQUEST);
EncodeInt(myServer.Version);
EncodeString(myClientName);
net_sendDataToServer(SendBuffer, gamesocket);
ClearSendBuffer();			//Clear the send buffer
ConnectionPhase=0;

do
{

  do {
  timeout++;
  } while ((net_CheckForDataOnSocket (gamesocket)!=1) && (timeout < 60000));
  if (timeout<60000)
  {
	net_receiveData (gamesocket); //The received data is in ReceiveBuffer!!
	do
	{
		TempDenominator=DecodeDenominator();

		if (TempDenominator==DENOMINATOR_ASSIGN_CLIENTID[0])
		{
			myClientID = DecodeInt();
			int tempTeam = DecodeInt();
			if (myClientID != -1)
			{
				AddNewClient(myClientID, myClientName, tempTeam, FXTRUE);
				localplayer = myClientID;
				myServer.isActive = FXFALSE;
				myServer.isClient = FXFALSE;
				ConnectionPhase++;
			}
			else
				return (-1);
		}
		else if (TempDenominator==DENOMINATOR_TERRAINSETUP[0])
		{
			int tempWarterlevel;
			RandomSeed = DecodeInt();
			TerrainType = DecodeInt();
			tempWarterlevel = DecodeInt();
			sprintf(tempstring, "INITIAL RECV: DENOMINATOR_TERRAINSETUP");
			NetLog(tempstring);
			ConnectionPhase++;
		}	
		else if (TempDenominator==DENOMINATOR_MAPINFORMATION[0])
		{
			current_weather.WeatherType = DecodeInt();
			current_weather.WeatherPrecipitation = DecodeInt();
			current_weather.PrecipitationStrength = DecodeFloat();
			current_weather.ThunderAndLightning = DecodeInt();
			sprintf(tempstring, "INITIAL RECV: DENOMINATOR_MAPINFORMATION");
			NetLog(tempstring);
			ConnectionPhase++;
		}
		else if (TempDenominator==DENOMINATOR_DAYTIME[0])
		{
			daytime = (float)(DecodeInt());
			sprintf(tempstring, "INITIAL RECV: DENOMINATOR_DAYTIME daytime=%i", (int)(daytime));
			NetLog(tempstring);
			ConnectionPhase++;
		}
		else if (TempDenominator == DENOMINATOR_ADD_OBJECT[0])
		{
			int ObjectType = DecodeInt();
			int ObjectMesh = DecodeInt();
			int TeamNum = DecodeInt();
			int TimeToLive = DecodeInt();
			FxBool IsAIControlled = DecodeBool();
			FxBool IsGuided = DecodeBool();
			FxBool IsVisible = DecodeBool();
			FxBool IsMarkerMissile = DecodeBool();
			float xPos = DecodeFloat();
			float zPos = DecodeFloat();
			float Height = DecodeFloat();
			float Yaw = DecodeFloat();
			float Pitch = DecodeFloat();
			float Roll = DecodeFloat();
			int FiringObject = DecodeInt();
			int i = AddObject(ObjectType, ObjectMesh, TeamNum, TimeToLive, IsAIControlled, IsGuided, IsVisible, IsMarkerMissile, FXFALSE, xPos, zPos, Height, Yaw, Pitch, Roll, FiringObject, FXFALSE );
			//We don't take any chances, so we let them send us all the neccessary data...
			Objects[i].OwningNetworkClient = DecodeInt();
			Objects[i].IDAtOwningClient = DecodeInt();

			sprintf(tempstring, "INITIAL RECV: DENOMINATOR_ADD_OBJECT local=%i remote=%i owner=%i ObjectType=%i ObjectMesh=%i", i, Objects[i].IDAtOwningClient, Objects[i].OwningNetworkClient, ObjectType, ObjectMesh);
			NetLog(tempstring);
			ConnectionPhase++;
		}
	} while ((signed)(strlen(ReceiveBuffer)) > 1);

  }
  else
  {
	myClientID=-1;
	ClientNetDelay = timeGetTime();
	return(-1);
  }
} while (ConnectionPhase < 6);

myServer.isActive=FXFALSE;
myServer.isClient = FXTRUE;

NewFractal();			//Fractal landscape heightmap
CreateLandscape(TerrainType);		//Convert into landscape mesh and texturize (Lush etc.) as specified
numverts = numLSverts;
numfaces = numLSfaces;

//Give us a new sky
NewFractalSky1();		//sky texture 1
NewFractalSky2();		//sky texture 2
calcSky1();				//convert into OpenGL texture
calcSky2();				//ditto

startFrameTime = TimerGetTime();
ClientNetDelay = timeGetTime();

return myClientID;
}


//--------------------------------------------------------------------------------
// Disconnect from a gameserver.
//--------------------------------------------------------------------------------
void Network_Disconnect(void)
{
    net_disconnect (gamesocket);
	myServer.isClient = FXFALSE;
	myServer.isActive = FXFALSE;

}



//--------------------------------------------------------------------------------
// Keep master informed so that we stay on the gamelist
//--------------------------------------------------------------------------------
void Heartbeat(void)
{
    char TempStuff[1024];
    sprintf(TempStuff, "%s %s %s %i %i %i", "Z", myServer.Adress, 
						 myServer.Name, 
						 myServer.MaxClients, 
						 myServer.CurClients,
						 myServer.Version);

    net_sendDataToMaster (TempStuff, mastersocket);
	HeartbeatDelay=timeGetTime();

}


//--------------------------------------------------------------------------------
// Start a Network Game Server
//--------------------------------------------------------------------------------
void Network_StartServer(void)
{
	LPHOSTENT hostentry = NULL;
	unsigned long ulServerAddr = INADDR_NONE;
	in_addr temp;
	int i;

	for (i=0; i<myServer.MaxClients; i++)
		Player[i].isActive = FXFALSE;

    gethostname ( myServer.Adress, 1024);
	hostentry = gethostbyname(myServer.Adress);

	if (hostentry != NULL)
	{
		ulServerAddr = *((unsigned long *)hostentry->h_addr_list[0]);
	}

	temp.S_un.S_addr=ulServerAddr;

	sprintf(myServer.Adress, "%s", inet_ntoa(temp));
	myServer.isActive=FXTRUE;
	myServer.isClient = FXFALSE;

    //Start our listen server
    gamesocket = net_startServer (GamePort);
    if (gamesocket==-1)
	exit (1);

    //Open a Connection to the master server and tell him about our party here
    mastersocket = net_connectToMaster ("hox.dhs.org", MasterPort);
    if (mastersocket==-1)
	exit (1);

	setupTeam(0);	//setup team 0 and 1
	setupTeam(1);
	if (!myServer.Dedicated)
	{
//		sprintf(myClientName, "Werewolf");
		AddNewClient( 0, myClientName, 0, FXTRUE);
		myServer.CurClients=1;
	}

    //...and immediately send a heartbeat so we get onto the server list
    Heartbeat();
}


//--------------------------------------------------------------------------------
// Retrieve a list of available internet game servers
//--------------------------------------------------------------------------------
void GetServerList(void)
{
    char header[80];
    int index = 0;
	long timeout = 0;

    //Open a Connection to the master server
    mastersocket = net_connectToMaster ("hox.dhs.org", MasterPort);
    if (mastersocket==-1)
	exit (1);

    //Request a list of servers
	sprintf(SendBuffer, "%s", DENOMINATOR_LISTREQUEST);
    net_sendDataToMaster (SendBuffer, mastersocket);


		do {
		timeout++;
		} while ((net_CheckForDataOnSocket (mastersocket)!=1) && (timeout < 6000000));
		if (timeout<6000000)
		{
			do
			{
				net_receiveData (mastersocket); //The received data is in ReceiveBuffer!!
				if (ReceiveBuffer[0] == DENOMINATOR_LISTENTRY[0])
				{
				    sscanf(ReceiveBuffer, "%s %s %s %i %i %i", header, Servers[index].Adress, 
										Servers[index].Name, 
										&Servers[index].MaxClients, 
										&Servers[index].CurClients,
										&Servers[index].Version);
					index++;
				}
			} while (strcmp(ReceiveBuffer, "X Done!") != 0);
		}
		else
			index=-1;

		numServers = index;
		Servers[index+1].Version = -1;
	    net_disconnect (mastersocket);
}




//--------------------------------------------------------------------------------
// Stop hosting a game.
//--------------------------------------------------------------------------------
void Network_StopServer(void)
{
    net_disconnect (mastersocket);
    net_disconnect (gamesocket);
	myServer.CurClients=0;
	myServer.isActive = FXFALSE;
	myServer.isClient = FXFALSE;
}



//--------------------------------------------------------------------------------
// Periodically called with each rendered frame, this function receives and
// decodes incoming packets from the server. NOT USED IF WE ARE THE SERVER!
//--------------------------------------------------------------------------------
void Network_ClientHandler(void)
{
	int i;

	while ((net_CheckForDataOnSocket (gamesocket)==1) && (timeGetTime() - ClientNetDelay < 500))
	{
	    ReceiveBufferPointer=0;
	    net_receiveData (gamesocket);		//The received data is in ReceiveBuffer!!
	    Network_ParseReceiveBuffer();	//and process it into the main Objects[] array
	}

	if (timeGetTime() - ClientNetDelay > 500)
	{
		sprintf(tempstring, "CLIENT: Time for an obj update!");
		NetLog(tempstring);
		for (i=0; i<NumObjects; i++)
		{
			if ((Objects[i].OwningNetworkClient == localplayer) && (Objects[i].ObjectId != -1))
			{
				UpdateObject(i);
				sprintf(tempstring, "SEND: Object Update %i", i);
				NetLog(tempstring);
			}
		}
			
		if (strlen(SendBuffer)>1)
		{
			net_sendDataToServer (SendBuffer, gamesocket);
			ClearSendBuffer();			//Clear the send buffer
		}
		ClientNetDelay = timeGetTime();
	}

}


//--------------------------------------------------------------------------------
// This is the main server code, handling incoming connections and packets,
// and sending data back to the clients. NOT USED IF WE ARE A CLIENT!
//--------------------------------------------------------------------------------
void Network_ServerHandler(void)
{
   int i, ObjectID;


    if (timeGetTime() - HeartbeatDelay > 60000)
	{
		Heartbeat();
		HeartbeatDelay=timeGetTime();
	}


    while (net_CheckForDataOnSocket (gamesocket)==1)
	{
	    ReceiveBufferPointer=0;
	    net_receiveData (gamesocket); 		//The received data is in ReceiveBuffer!!
	    Network_ParseReceiveBuffer();	//and process it into the main Objects[] array
	}



	for (i=0;i<=myServer.MaxClients;i++)
	if ((Player[i].isActive==1) && ((timeGetTime() - Player[i].LastNetUpdate) > 500) && (i != localplayer))
	{
		ClearSendBuffer();			//Clear the send buffer

		EncodeDenominator(DENOMINATOR_DAYTIME);
		EncodeInt((int)(daytime));
		Player[i].LastNetUpdate = timeGetTime();

		for (ObjectID=0; ObjectID<NumObjects; ObjectID++)	//Go through all objects
		//Don't ever mess with a command center, or else the respawn code freaks out!!
		if (Objects[ObjectID].ObjectMesh != OBJECTMESH_COMMANDCENTER)
		if ((Objects[ObjectID].OwningNetworkClient != i))
		{
			if ((IsVisibleViaNetwork(ObjectID, i)) && (Objects[ObjectID].ObjectId != -1))
			{
				//If we are within visible range, but weren't on the last check,
				//tell the client to add the object
				if (Objects[ObjectID].isVisibleToClient[i]==FXFALSE)
				{
					sprintf(tempstring, "SEND: AddObject for Client %i: local=%i remote=%i owner=%i", i, ObjectID, Objects[ObjectID].IDAtOwningClient, Objects[ObjectID].OwningNetworkClient);
					NetLog(tempstring);

					Objects[ObjectID].isVisibleToClient[i]=FXTRUE;
					EncodeDenominator(DENOMINATOR_ADD_OBJECT);
					EncodeInt(Objects[ObjectID].ObjectType);
					EncodeInt(Objects[ObjectID].ObjectMesh);
					EncodeInt(Objects[ObjectID].Team);
					EncodeInt(Objects[ObjectID].TimeToLive);
					EncodeBool(Objects[ObjectID].isAIControlled);
					EncodeBool(Objects[ObjectID].isGuided);
					EncodeBool(Objects[ObjectID].isVisible);
					EncodeBool(Objects[ObjectID].isMarkerMissile);
					EncodeFloat(Objects[ObjectID].xPos);
					EncodeFloat(Objects[ObjectID].zPos);
					EncodeFloat(Objects[ObjectID].Height);
					EncodeFloat(Objects[ObjectID].Yaw);
					EncodeFloat(Objects[ObjectID].Pitch);
					EncodeFloat(Objects[ObjectID].Roll);
					EncodeInt(Objects[ObjectID].FiringObject);
					EncodeInt(Objects[ObjectID].OwningNetworkClient);
					EncodeInt(Objects[ObjectID].IDAtOwningClient);
					Player[i].LastNetUpdate = timeGetTime();
				}
				else	//just do a normal update
				{
					sprintf(tempstring, "SEND: Object Update %i for Client %i", ObjectID, i);
					NetLog(tempstring);
					UpdateObjectOnClient(ObjectID, i);
				}
			}
			else	//Else we just lost eye contact, OR the object doesn't exist anymore!
			{
				//tell the client to remove the object from it's array.
				if (Objects[ObjectID].isVisibleToClient[i]==FXTRUE)
				{
					Objects[ObjectID].isVisibleToClient[i]=FXFALSE;
					NetRemoveObject(ObjectID);
				}
			}
		}
		
		if (strlen(SendBuffer)>1)
		{
			net_sendData (SendBuffer, gamesocket, i);	//...and send it
		}
	}

}



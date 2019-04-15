#include "video_gl.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <conio.h>
#include <iostream.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <winsock.h>
#include <io.h>
#include <fcntl.h>
#define WIN32_LEAN_AND_MEAN
#ifdef USE_GLIDE
#include <glide.h>
#else
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glut.h>
//#include <gl\glext.h>
#endif

#include "tlib.h"
#include "fmod.h"

extern char tempstring[256];

extern int localplayer;		//global playerId of the local human player.

extern int NumObjects;

extern FxBool CompatibilityMode;

extern int frames;

extern int numServers;


extern int SCREEN_COLORBITS;
extern int SCREEN_DEPTHBITS;
extern int SCREEN_MODE;
extern int SCREEN_FULLSCREEN;
extern int SCREEN_TERRAINTEXTURES;
extern int SCREEN_LOD;
extern int SCREEN_WIREFRAME;
extern int SCREEN_FOG;
extern int SCREEN_LORES;
extern int SOUND_ENABLED;
extern int MOUSE_INVERT;
extern int winWidth, winHeight;

extern int TERRAIN_LUSH;
extern int TERRAIN_STONE;
extern int TERRAIN_SNOW;
extern int TERRAIN_DESERT;
extern int TERRAIN_LAVA;

extern int WEATHER_SUNNY;
extern int WEATHER_CLOUDED;
extern int WEATHER_RAIN;
extern int WEATHER_STORM;
extern int WEATHER_SNOW;

extern int OBJECTTYPE_SHIP;
extern int OBJECTTYPE_CAMERA;
extern int OBJECTTYPE_BUILDING;
extern int OBJECTTYPE_WEAPON;
extern int OBJECTTYPE_DOLLY;

extern int OBJECTMESH_SHIP;
extern int OBJECTMESH_LASER;
extern int OBJECTMESH_MISSILE;
extern int OBJECTMESH_COMMANDCENTER;
extern int OBJECTMESH_POWERPLANT;
extern int OBJECTMESH_MINE;
extern int OBJECTMESH_SAM;
extern int OBJECTMESH_AAA;
extern int OBJECTMESH_UPLINK;
extern int OBJECTMESH_LIGHTTANK;
extern int OBJECTMESH_SKYSPHERE1;
extern int OBJECTMESH_SKYSPHERE2;
extern int OBJECTMESH_COMMANDCENTERMENU;
extern int OBJECTMESH_SHIP1;
extern int OBJECTMESH_SHIP2;
extern int OBJECTMESH_SHIP3;
extern int OBJECTMESH_SHIP1_LOD1;
extern int OBJECTMESH_SHIP1_LOD2;
extern int OBJECTMESH_TANK1_LOD1;
extern int OBJECTMESH_TANK1_LOD2;
extern int OBJECTMESH_TANK1_LOD3;
extern int OBJECTMESH_TANK2_LOD1;
extern int OBJECTMESH_TANK2_LOD2;
extern int OBJECTMESH_TANK2_LOD3;


extern int OBJECTMESH_AAA1_LOD1_PART1;
extern int OBJECTMESH_AAA1_LOD2_PART1;
extern int OBJECTMESH_AAA1_LOD3_PART1;
extern int OBJECTMESH_AAA1_LOD1_PART2;
extern int OBJECTMESH_AAA1_LOD2_PART2;
extern int OBJECTMESH_AAA1_LOD3_PART2;
extern int OBJECTMESH_AAA1_LOD1_PART3;
extern int OBJECTMESH_AAA1_LOD2_PART3;
extern int OBJECTMESH_AAA1_LOD3_PART3;

extern int OBJECTMESH_SAT1_LOD1;
extern int OBJECTMESH_SAT1_LOD2;
extern int OBJECTMESH_SAT2_LOD1;
extern int OBJECTMESH_SAT2_LOD2;
extern int OBJECTMESH_UPLINK2;
extern int OBJECTMESH_MEDIUMTANK;


extern int DAMAGE_LASER;
extern int DAMAGE_MISSILE;
extern int DAMAGE_DUMBMISSILE;

extern int MISSILETYPE_GUIDED;
extern int MISSILETYPE_DUMBFIRE;
extern int MISSILETYPE_MARKER;
extern int MISSILETYPE_SWARM;
extern int MISSILETYPE_CRUISE;

extern int SPECIALTYPE_SHIELDS;
extern int SPECIALTYPE_CLOAK;
extern int SPECIALTYPE_ECM;
extern int SPECIALTYPE_RADARJAMMER;
extern int SPECIALTYPE_KAMIKAZE;


extern int RestartProgram;
extern void RestartVideo(void);

extern int LeftMouseButton;
extern int RightMouseButton;
extern int mouse_x, mouse_y;


typedef struct		//Basic Object struct
{
  int ObjectId;		//Every Object has a unique ID, which is also it's position in the Objects[] array

  int ObjectType;	//1=Ship, 2=Camera, 3=Building, 4=Weapon

  int ObjectSaveMesh;
  int ObjectMesh;	//1=ship, 2=laser bolt, 3=missile, 4=command center
					//5=power plant, 6=Mine, 7=SAM-Site, 8=AAA-Site 
					//9=satellite uplink 10=light Tank
					//(Used for adressing the Meshes[] array)
  int laserReloadTime;		//Time between laser shots.
  int missileReloadTime;	//Time between missile shots.

  int Team;			//The team this object belongs to.
  int Health;		//0 to 100
  int Shield;		//0 to 100
  int MaxHealth;	//Maximum amounts
  int MaxShield;	
  int SaveMaxShield;

  float Speed;		//Guess
  float xPos, zPos, Height, Yaw, Pitch, Roll;	//Guess again...
  float oldxPos, oldzPos, oldHeight;		//This is used for the collision detection
  float SunX, SunY, SunZ;
  float SunRIntensity, SunGIntensity, SunBIntensity;
  float sizeX, sizeY, sizeZ;	//The object's dimensions
  int xGrid, zGrid;				//Position on the map grid
  FxBool isVisible;	//Toggles rendering of object on/off
  int TimeToLive;	//Max. livetime of the object (for laser and such)
  FxBool isCollided;//True if Object collided with something
  int FiringObject; //If it's a weapon, this contains the ID of the firing object

  FxBool isAIControlled;	//Is this a computer controlled object (Ship)?
  FxBool isGuided;			//Is this a guided weapon (Missile)?
  int targetObject;			//ObjectID of the current target
  float AITargetPitch;		//Pitch to fly to intercept Target
  float AITargetYaw;		//Yaw to fly to intercept Target
  float AITargetRoll;
  int TimeSinceLastAIPoll;	//It is what it says.

  float xmovement, ymovement, zmovement;	//Calculated from heading and speed

  //The following variables are used for fixed animations (takeoff, land, die)
  FxBool isDocked;		//For ships docked to the command center
  FxBool isStarting;	//Ships taking off of the command center
  FxBool isLanding;		//Duh.
  FxBool isDiving;		//When a ship gets shot down by lasers, it will "dive" into the ground
  FxBool isBusy;		//True if a command center is busy with ship takeoff/landings
  int CommandCenter;	//If we're a ship taking off, this is the command center ID we're at.
  int AnimationPhase;	//These statements need a "counter" to keep track of the time
  float AnimationSteps;	//Depending on the distance to the animation target, we have to know how far to go with each animation step.
  float InitialHeight;	//The origin of the animation

//  int hasSpecial;	//True if the object carries a marker missile
//  int SpecialAmmo;		//Amount of marker missiles loaded on the ship
  FxBool isMarkerMissile;	//Tells the object that it's a marker missile

  FxBool isMarked;		//If a building gets hit by a marker missile, this will be set FXTRUE

  int OwningNetworkClient;
  int IDAtOwningClient;
  int ControllingPlayer;
  FxBool isVisibleToClient[256];

  int MissileType;
  int MissileAmmo;
  int SpecialType;
  int SpecialAmmo;

  int ParentObject; // AAA and SAMs have movable parts. The base is fixed, and the other parts are linked to the base via this variable.

  // These are variables of the base object that point to the moving parts.
  int ChildObject0; //Child 0 inherits the yaw angle
  int ChildObject1; //Child 1 inherits yaw and pitch
  int ChildObject2; //Child 2 inherits pitch (unused for now)

} object3d;



typedef struct		//Basic Team struct
{
  char *TeamName;	//Team description
  int EnergyAvailable;		//Team's energy level
  int ResourcesAvailable;	//Team's available resources
  int EnergyNeeded;			//current Energy demand per time
  int ResourcesNeeded;		//current Resources demand per time

  int PlayersInTeam;	//Number of players (human/AI) in the team
  int BuildingsInTeam;	//Number of intact team buildings

  int FightersKilled;	//Number of killed enemy fighters
  int FightersLost;		//Number of lost own fighters
  int BuildingsKilled;	//Number of destroyed enemy buildings
  int BuildingsLost;	//Number of lost own buildings

  FxBool isActive;	//FALSE if team is empty, TRUE if not.
  FxBool isDefeated;//FALSE on startup, TRUE if team is dead.
  FxBool isPowered;

  int SatelliteTime;	//Time until Satellite is in firing position

} team3d;


typedef struct
{
  char PlayerName[256];	//Duh
  int Team;				//Which team this player belongs to
  int ControlledObject;	//The ObjectID of the ship the player currently sits in
  FxBool isActive;		//Is this Player in the game?
  FxBool isHuman;		//Is it a human? (=local keyboard control)
  FxBool isAI;			//Is it an AI? (=local AI control)
  FxBool isNetwork;		//Is it a network client? (=remote human or remote AI)
  FxBool isTank;		//True if controlling a Tank instead of a ship
  FxBool doRespawn;		//Respawn if dead
  int VehicleModel;		//What kind of Tank/Ship are we piloting?
  unsigned long LastNetUpdate;
} player3d;

typedef struct
{
	int WeatherType;
	int WeatherPrecipitation;	//Rain, snow, etc.
	float PrecipitationStrength;	//Set strength of Rain, etc.
	float CurrentPrecipitationStrength;	//Current strength, for smooth transition.

	int ThunderAndLightning;	//Set 1 for distant rolling thunder and lightning
	int ThunderDelay;
	int TimeAfterLastThunder;
	int LastThunderType;

	int LightningBurstTime[20];
	int NumLightningBursts;
	int NextLightningBurst;

} weather3d;


extern player3d	Player[128];	//Max number of players
extern team3d	Team[10];	//Max number of teams
extern object3d	Objects[2000];	//Max number of Objects
extern weather3d current_weather;

//extern player3d	*Player;	//Max number of players
//extern team3d	*Team;	//Max number of teams
//extern object3d	*Objects;	//Max number of Objects




void UnloadAllTextures(void);
void DRCloseSound(void);

extern void DisplayChatMessage(char *s);
extern int GetNextFreeClientID(void);
extern void FindNewOwnerForObjects(int ClientID);
extern int GetNextAutobalanceTeam(void);

extern void menuLaunch(void);
extern void AddMessage(char *msg, int r, int g, int b);
extern void setupTeam(int TeamNum);

extern int AddObject(int ObjectType, int ObjectMesh, int TeamNum, int TimeToLive, FxBool IsAIControlled, FxBool IsGuided, FxBool IsVisible, FxBool IsMarkerMissile, FxBool IsLocalPlayer, float xPos, float zPos, float Height, float Yaw, float Pitch, float Roll, int FiringObject, int NetSync);
extern void RemoveObject(int ObjectId, FxBool NetSync);
extern void DamageObject(int objectId, int damage, FxBool MarkObject, FxBool NetSync);
extern int numPlayers;


extern unsigned long TimerGetTime();

extern int getObject(int TeamNum, int Type, int Mesh, int count);
extern float daytime;
extern int RandomSeed;
extern int TerrainType;

extern void NewFractal(void);
extern void CreateLandscape(int CurrentTerrain);
extern int numfaces,numverts,numLSverts, numLSfaces;
extern unsigned long startFrameTime, endFrameTime;
extern void NewFractalSky1(void);
extern void NewFractalSky2(void);
extern void calcSky1(void);
extern void calcSky2(void);


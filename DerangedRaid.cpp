/****************************************************************************************/
/*  DerangedRaid.cpp                                                                    */
/*                                                                                      */
/*  Author: Josef Jahn (werewolf@playspoon.com)                                         */
/*  Code version: v4                                                                    */
/*  URL: http://www.playspoon.com                                                       */
/*  Description: Main sourcecode for the game DERANGED RAID                             */
/*                                                                                      */
/*                                                                                      */
/*  Credits & Thanks to:                          Contribution:                         */
/*  ----------------------------------------------------------------------------------  */
/*  Jeff Molofee (http://nehe.gamedev.net)        - GL init code                        */
/*  Nate Miller  (nkmiller@calpoly.edu)           - Text rendering, TGA loader          */
/*                                                  and original menu code              */
/*  Nicholas Anderson (n.anderson@eidosnet.co.uk) - LOD algorithm                       */
/*                                                                                      */
/*  Installation, Requirements, Compilation:                                            */
/*  ----------------------------------------------------------------------------------  */
/*  Required are: VisualC++ 6.0, Glide2x SDK (www.3dfx.com), OpenGL SDK, FSOUND SDK     */
/*                 DR WILL NOT COMPILE WITHOUT ALL THESE LIBRARIES!!!                   */
/*  After having installed all these SDKs, you should be able to compile Deranged Raid! */
/*                                                                                      */
/*  The contents of this file are subject to the terms and conditions of                */
/*  OpenSource software and the GNU Public License.                                     */
/*                                                                                      */
/*  Software distributed under this License is distributed on an "AS IS"                */
/*  basis, WITHOUT WARRANTY OF ANY KIND, neither expressed or implied.  See             */
/*  the License for the specific language governing rights and limitations              */
/*  under the License. You may obtain a copy of the License at http://www.gnu.org       */
/*                                                                                      */
/*                                                                                      */
/*  ==================================================================================  */
/*  FINAL WARNING:                                                                      */
/*  THIS SOURCECODE IS A MESS!!!                                                        */
/*  ==================================================================================  */
/*                                                                                      */
/*                                                                                      */
/*  Copyright (C) 1995-2001 Playspoon, All Rights Reserved                              */
/*                                                                                      */
/****************************************************************************************/


//CHANGE THIS TO "video_glide" TO COMPILE WITH GLIDE-SUPPORT!!

#include "video_gl.h"

//DON'T FORGET TO USE THE _GLIDE PROJECT FILE FOR GLIDE, AND THE _OPENGL
//PROJECT FILE FOR COMPILING WITH OPENGL!!!

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
#include <fstream.h>
#include <direct.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <process.h>


#include "drnetwork.h"
#include "net_udp.h"
#include "derangedraid.h"

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
#include "text.h"
#include "menu.h"


const float Deg = 91.522222f; //was 91.0222
#define DEGREE (.01745328f)

//Autoupdater
#define VERSIONTAGOLD "version_tag_old.vtg"
#define VERSIONTAGNEW "version_tag_new.vtg"
#define SERVERNAME "www.playspoon.com"
#define FILEONSERVER "/autoupdate/"VERSIONTAGNEW

void fcopy(LPCSTR sourcefile, LPCSTR targetfile);					 
void GetHTTP(LPCSTR lpServerName, LPCSTR lpFileName);
LPCSTR removepath(LPCSTR lpFileName);
long calccrc(LPCSTR file);	

char servername[256], file_on_server[256];	   
//------------


	float x1tank,y1tank,z1tank;
	float x2tank,y2tank,z2tank;
	float x3tank,y3tank,z3tank;
	float x4tank,y4tank,z4tank;
	float x5tank,y5tank,z5tank;



float TerrainHeight(float *x, float *y, float *z);

FxBool backbuffer = FXTRUE;
static const char name[]    = "Deranged Raid";

static const int MaxFaces	= 500000;
float FogIntensity	= 0.002f;

float debugfloat = 1.0f;

float FOGCOLOR = 000000;
#ifdef USE_GL

static const double pi = 3.14159265358979323846264338327950288419716939937510f;

static GLfloat light_mat[] = {1.0f, 1.0f, 1.0f, 0.5f};

static GLfloat dark_mat[] = {0.0f, 0.0f, 0.0f, 0.5f};


GLfloat fogColor[] = {0.0f, 0.0f, 0.0f, 1.0f};

GLfloat LightAmbient[]= { 1.0f, 1.0f, 1.0f, 0.5f };

GLfloat LightDiffuse[]= { 10.0f, 10.0f, 10.0f, 0.5f };
#endif

int MouseMoved=0;
int o_mouse_x, o_mouse_y;

int BlendA, BlendB;

extern BOOL keys[256];
extern FxBool useGlide;
float FarClip = 1000000000.0f;
float maxvdist = 2200.0f;
float scalefactor = 100.0f;
static const int MapDimension = 34;


float SoundFactor[60];
float prevSoundFactor[60];
float curSoundFactor[60];
int prevIntSoundFactor[60];
int curIntSoundFactor[60];

float listenerpos[3];

int SAMPLE_ENGINEB = 0;
int SAMPLE_ENGINEA = 1;
int SAMPLE_INTERNAL = 2;
int SAMPLE_FIRE = 3;
int SAMPLE_MISSILE = 4;
int SAMPLE_TAKEOFF = 5;
int SAMPLE_LANDING = 6;
int SAMPLE_GEARDN = 7;
int SAMPLE_GEARUP = 8;
int SAMPLE_MISSILEBLAST = 9;
int SAMPLE_LASERBLAST = 10;
int SAMPLE_SHIPDESTROYED = 11;
int SAMPLE_BUILDINGDESTROYED = 12;
int SAMPLE_SHIPHITL = 13;
int SAMPLE_SHIPHITM = 14;
int SAMPLE_BUILDINGHITL = 15;
int SAMPLE_BUILDINGHITM = 16;
int SAMPLE_LOCK = 17;
int SAMPLE_STALL = 18;
int SAMPLE_DIVE = 19;
int SAMPLE_SHIPHITGROUND = 20;
int SAMPLE_SATELLITE = 21;
int SAMPLE_SATBLAST = 22;
int SAMPLE_GROWLOCK = 23;
int SAMPLE_BASEFIRE = 24;
int SAMPLE_BASEBACKUP = 25;
int SAMPLE_EVENT_THUNDER1 = 26;
int SAMPLE_EVENT_THUNDER2 = 27;
int SAMPLE_EVENT_THUNDER3 = 28;
int SAMPLE_AMBIENT_RAIN = 29;
int SAMPLE_MENU_MON = 30;
int SAMPLE_MENU_SEL = 31;
int SAMPLE_MENU_FLIP = 32;
int SAMPLE_MENU_ENTER = 33;
int SAMPLE_WEAPONCHANGE = 34;
int SAMPLE_NA = 35;
int SAMPLE_RADARLOCK = 36;
int SAMPLE_EVENT_THUNDER4 = 37;
int SAMPLE_EVENT_THUNDER5 = 38;
int SAMPLE_EVENT_THUNDER6 = 39;
int SAMPLE_EVENT_THUNDER7 = 40;
int SAMPLE_EVENT_THUNDER8 = 41;
int SAMPLE_AMBIENT_SUMMER = 42;
int SAMPLE_AMBIENT_WIND = 43;
int SAMPLE_AMBIENT_WINDHIGHALT = 44;
int SAMPLE_AMBIENT_RAINLIGHT = 45;
int SAMPLE_AMBIENT_CRICKETSNIGHT = 46;
int SAMPLE_AMBIENT_CRICKETS = 47;
int SAMPLE_LOOP_AMB1=48;
int SAMPLE_LOOP_AMB2=49;
int SAMPLE_LOOP_AMB3=50;
int SAMPLE_LOOP_AMB4=51;
int SAMPLE_LOOP_SHIP=52;
int SAMPLE_LOOP_TANK=53;
int SAMPLE_LOOP_MISSILE=54;
int SAMPLE_LOOP_TANKROLL=55;

int TERRAIN_LUSH=0;
int TERRAIN_STONE=1;
int TERRAIN_SNOW=2;
int TERRAIN_DESERT=3;
int TERRAIN_LAVA=4;


int OBJECTTYPE_SHIP = 1;
int OBJECTTYPE_CAMERA = 2;
int OBJECTTYPE_BUILDING = 3;
int OBJECTTYPE_WEAPON = 4;
int OBJECTTYPE_DOLLY = 5;


int OBJECTMESH_SHIP = 1;
int OBJECTMESH_LASER = 2;
int OBJECTMESH_MISSILE = 3;
int OBJECTMESH_COMMANDCENTER = 4;
int OBJECTMESH_POWERPLANT = 5;
int OBJECTMESH_MINE = 6;
int OBJECTMESH_SAM = 7;
int OBJECTMESH_AAA = 8;
int OBJECTMESH_UPLINK = 9;
int OBJECTMESH_LIGHTTANK = 10;
int OBJECTMESH_SKYSPHERE1 = 11;
int OBJECTMESH_SKYSPHERE2 = 12;
int OBJECTMESH_COMMANDCENTERMENU = 13;
int OBJECTMESH_SHIP1 = 14;
int OBJECTMESH_SHIP2 = 15;
int OBJECTMESH_SHIP3 = 16;
int OBJECTMESH_SHIP1_LOD1 = 17;
int OBJECTMESH_SHIP1_LOD2 = 18;
int OBJECTMESH_TANK1_LOD1 = 19;
int OBJECTMESH_TANK1_LOD2 = 20;
int OBJECTMESH_TANK1_LOD3 = 21;
int OBJECTMESH_TANK2_LOD1 = 22;
int OBJECTMESH_TANK2_LOD2 = 23;
int OBJECTMESH_TANK2_LOD3 = 24;


int OBJECTMESH_AAA1_LOD1_PART1 = 25;
int OBJECTMESH_AAA1_LOD2_PART1 = 26;
int OBJECTMESH_AAA1_LOD3_PART1 = 27;
int OBJECTMESH_AAA1_LOD1_PART2 = 28;
int OBJECTMESH_AAA1_LOD2_PART2 = 29;
int OBJECTMESH_AAA1_LOD3_PART2 = 30;
int OBJECTMESH_AAA1_LOD1_PART3 = 31;
int OBJECTMESH_AAA1_LOD2_PART3 = 32;
int OBJECTMESH_AAA1_LOD3_PART3 = 33;


int OBJECTMESH_SAT1_LOD1 = 34;
int OBJECTMESH_SAT1_LOD2 = 35;
int OBJECTMESH_SAT2_LOD1 = 36;
int OBJECTMESH_SAT2_LOD2 = 37;
int OBJECTMESH_UPLINK2 = 38;
int OBJECTMESH_MEDIUMTANK = 39;


int DAMAGE_LASER=10;
int DAMAGE_MISSILE=100;
int DAMAGE_DUMBMISSILE=100;


int MISSILETYPE_GUIDED=0;
int MISSILETYPE_DUMBFIRE=1;
int MISSILETYPE_MARKER=2;
int MISSILETYPE_SWARM=3;
int MISSILETYPE_CRUISE=4;


int SPECIALTYPE_SHIELDS=0;
int SPECIALTYPE_CLOAK=1;
int SPECIALTYPE_ECM=2;
int SPECIALTYPE_RADARJAMMER=3;
int SPECIALTYPE_KAMIKAZE=4;


//HUD Icons
int ICON_TARGET = 51;
int ICON_FRIENDLY = 52;
int ICON_ENEMYBASE = 53;
int ICON_FRIENDLYBASE = 54;
//GUI Icons
int ICON_COMMANDCENTER = 55;
int ICON_POWERPLANT = 56;
int ICON_MINE = 57;
int ICON_SAM = 58;
int ICON_AAA = 59;
int ICON_UPLINK = 60;


int TauntMenuHistoryLen;
int TauntMenuDisplaySize;
FxBool TauntMenuActive;
char TauntMenuDisplay[255][255];
int TauntMenuHistory[100];

int NumEmitters=0;
int NumParticles=0;
int MaxParticles=5000;		//Maximum amount of particles allowed in the game
int NumLights=0;
int AbsoluteMaxParticles=10000;//NEVER EVER exceed this many particles!!
int CurrentActiveEmitter;	//When limiting particles is needed, this will tell which emitter is active;

int maxsamples;

int texScale;

#if (defined (random))
#   undef random
#   undef randomize
#endif
#if (defined (min))
#   undef min
#   undef max
#endif

#if (defined (__IS_32BIT__))
#define random(num)     (int) ((long) rand () % (num))
#else
#define random(num)     (int) ((int)  rand () % (num))
#endif

#define SetColor(x) glColor3f( ( x ), ( x ), ( x ) ) // This is just to keep things simpler.

GLvoid glPrint(char *fmt, float x, float y, float r, float g, float b);
GLvoid glFastPrint(char *fmt, float x, float y, float r, float g, float b);


GrHwConfiguration hwconfig;
static char version[256];


HDC DR_HDC;

int f1, f2;
int m1, m2;
float b1;


#define DISTANCE(v1, v2) sqrt((v1[0] - v2[0]) * (v1[0] - v2[0]) + (v1[2] - v2[2]) * (v1[2] - v2[2]))

#define MAP 128
//#define MAP 64
#define QUAD_MAP (MAP + 1)

int quadtree[MAP+1][MAP+1];

#define EDGE_POINT 0
#define NODE_POINT 1
#define UNKNOWN    2

#define NORTH   0
#define WEST    1
#define SOUTH   2
#define EAST    3
#define NW      4
#define NE      5
#define SW      6
#define SE      7
#define NORTH_L 8
#define NORTH_R 9
#define WEST_T  10
#define WEST_B  11
#define SOUTH_L 12
#define SOUTH_R 13
#define EAST_T  14
#define EAST_B  15





GrFog_t  fogtable[GR_FOG_TABLE_SIZE];

#define FOGCOLOR1 LIGHTREDMIST


//int Wireframe;
int StartEditAtFace;
int SunFlare;
int Moon1;
int Moon2;
float dawnR, dawnG, dawnB;
float duskR, duskG, duskB;

float curdawnR, curdawnG, curdawnB;
float curduskR, curduskG, curduskB;

float MenuPositions[10][10];  //8 camera positions for the menu, save 3d coordinates+yaw/pitch/roll.
FxBool MenuActive;			//True if we're inside the 3d menu
FxBool ConsoleActive;		//True if the menu/console text is visible
FxBool LoadoutMenuActive;	//True if the loadout menu is visible
FxBool JoinMenuActive;		//True if the server browser is visible
FxBool HostMenuActive;		//True if the server setup screen is visible
int CurrentMenu;


	int facexy[76][76];
	int facexyB[76][76];
	//[XX] should be > (MapDimension+1)*2 + 2
//	float heightfield[76][76];
//	int sky1[256][256];
//	int sky2[256][256];
//	float *heightfield=NULL;
	float heightfield[78*78];
//	int *sky1=NULL;
//	int *sky2=NULL;
	float sky1[256*256];
	float sky2[256*256];

	unsigned char *skytex1;
	unsigned char *skytex2;



float xpitchTime;

int FramesTillNextMapUpdate;

int ThreatWarningInterval;

float SkyRotator1, SkyRotator2;


struct			 											// Create A Structure For The Timer Information
{
  __int64       frequency;									// Timer Frequency
  float         resolution;									// Timer Resolution
  unsigned long mm_timer_start;								// Multimedia Timer Start Value
  unsigned long mm_timer_elapsed;							// Multimedia Timer Elapsed Time
  bool			performance_timer;							// Using The Performance Timer?
  __int64       performance_timer_start;					// Performance Timer Start Value
  __int64       performance_timer_elapsed;					// Performance Timer Elapsed Time
} timer;													// Structure Is Named timer

unsigned long startFrameTime, endFrameTime;


typedef struct {					//The primary face datatype
  int srcVerts[3];
  int Texture;						//Which texture to use
  int Texture2;						//data for 2d texture array of terrain
  int TextureMode;					//2 triangles form a rect. and this tells us which rect this triangle belongs to.
  int IsVisible;
  GrVertex vtxA, vtxB, vtxC, vtxD;	//Ready-to-draw vertices
  int OwnerObject;					//The object this triangle belongs to
  FxBool Transparent;				//Tells the renderer if the face is transparent
  FxBool isLight;					//For self-lit textures
  FxBool Reflective;				//True for canopy glasses.
} face3d;



typedef struct {					//The primary face datatype
  TlVertex3D v;						//use .v to get the vertice
  TlVertex3D normals;				//Vertex normals for lighting
  int OwnerObject;					//The object this is for
  float Alpha;
} vert3d;



typedef struct		//Not used anymore
{
	FxFloat x, y, z;
	FxFloat r, g, b;
	FxFloat s, t;
}Vert, *Vert_ptr;



typedef struct
{
	face3d	Faces[1000];		//Max number of faces for objects
	vert3d	Vertices[3000];		//Max number of verts for objects
//	face3d	*Faces;		//Max number of faces for objects
//	vert3d	*Vertices;		//Max number of verts for objects
	int numfaces;				//Guess...
	int numverts;
	float sizeX, sizeY, sizeZ;  //will be used for easy object/object collision detection
} mesh3d;


vert3d	Waterplane[4];		//Mesh for the water surface


int MUSIC_MP3 = 0;
int MUSIC_MOD = 1;
int MUSIC_WAV = 2;

typedef struct
{
	char filename[260];
	char displayname[260];
	int type;
} plstruct;

plstruct playlist[500];
int CurrentSong=-1;
int MaxSongs=0;
int ModuleIsFresh=1;




int WEATHERTYPE_SUNNY=0;
int WEATHERTYPE_CLOUDED=1;
int WEATHERTYPE_FOG=2;
int WEATHERTYPE_STORM=3;

int WEATHERPREC_NONE=0;
int WEATHERPREC_RAIN=1;
int WEATHERPREC_SNOW=2;
int WEATHERPREC_METEORITES=3;

weather3d current_weather;


typedef struct
{
  float xPos, zPos, Height;
} raindrop3d;
raindrop3d Raindrops[8000];
int TargetNumRaindrops=0;
int CurNumRaindrops=0;


typedef struct
{
	char Text[256];
	int r,g,b,Age;
} message3d;
message3d conMsg;

typedef struct
{
	int AttachedToObject;
	int isActive;
	int isRunning;
	float MaxAudibleDistance;
	long ChannelID;
	int SampleID;
	float x, y, z;
} noise3d;
noise3d Noise[500];
int NumNoises;



int WeatherType;

int TerrainType;
int RandomSeed;

int Price[20];

int maxTexture;



typedef struct	//Struct for Particle-Slots
{
	vert3d	Vertices[100];
	int numslots;
} ParticleSlot3d;



typedef struct	//Struct for Particle-Emitters
{
  float xPos, zPos, Height;		//Position of the Emitter
  float Yaw, Pitch;				//Emitting angles (we don't need "Roll" for a 2d Sprite)
  float Spreading;				//Tells how much particles will "stray" from the emitting angle
  float SpeedVariation;			//Tells how much the speed of particles may vary from the set speed
  int ParticleType;				//0=smoke (missiles, ships, buildings), 1=bright (explosions)
  float Gravity;				//0=No gravity, for smoke-trails (missiles, ships)
								//+1=falling down(explosions)
								//-1=travelling upwards (smoke at buildings, smoke after explosions)
  float Speed;					//Use high speeds for explosions, low speeds for smoke
  float StartSpeed, EndSpeed;	//Via these you can set deccelerating and accelerating particles
  float StartSize, EndSize;		//Same for the size
  int ParticlesPerFrame;		//How many frames per frame
  int ParticlesEveryNFrame;		//How many frames between new particles
  int FrameCounter;
  float TimeToLive;				//Total lifetime of all particles
  int ParticleEmitterLifeTime;	//Lifetime of the emitter itself. set -1 if forever
  int AttachedToObject;			//ID of the object this emitter is attached to
  int r, g, b;					//Particle Color
  FxBool isActive;				//We can toggle emitters to limit the number of Particles
  FxBool Active;				//Similar to above, but is used to tag DELETED emitters!
  FxBool AlwaysActive;			//Some particle emitters should NEVER be spared from rendering.
  FxBool isAtCenter;			//Force centered emitter position
  int Texture;
  FxBool Sun;					//If true, particle is ALWAYS visible. Used for Sun and Moon
  FxBool ThreeD;				//If true, particle is rendered using a 3d mesh
  int Slot;
} ParticleEmitter3d;

typedef struct	//Struct for the particle itself
{
  float xPos, zPos, Height;		//Particle's current position
  float Yaw, Pitch;				//Particle's current flight direction (Doesn't need "Roll" either)
  int ParticleType;				//0=smoke (missiles, ships, buildings), 1=bright (explosions)
  float Gravity;				//0=No gravity, for smoke-trails (missiles, ships)
								//+1=falling down(explosions)
								//-1=travelling upwards (smoke at buildings, smoke after explosions)
  float Speed;					//Use high speeds for explosions, low speeds for smoke
  float StartSpeed, EndSpeed;	//Via these you can set deccelerating and accelerating particles
  float TotalTimeToLive;		//Total lifetime of this particle
  float TimeToLive;				//Remaining lifetime of this particle
  FxBool isVisible;				//Used by the particle renderer
  float ScreenSize;				//Size on the screen after projection
  float Size;					//The real particle size (100 is a good value)
  float StartSize, EndSize;		//For size transition
  int r, g, b;					//Particle Color
  int Texture;
  FxBool Sun;					//If true, particle is ALWAYS visible. Used for Sun and Moon
  FxBool ThreeD;				//If true, particle is rendered using a 3d mesh
  int OwningObject;				//ID of the owning object
} Particle3d;

typedef struct	//Struct for Trees
{
  float xPos, zPos, Height;		//Particle's current position
  float Yaw, Pitch;				//Particle's current flight direction (Doesn't need "Roll" either)
  FxBool isVisible;				//Used by the particle renderer
  float Size;					//The real particle size (100 is a good value)
  int Texture;
} tree3d;


typedef struct	//Struct for dynamic lights
{
  float xPos, zPos, Height;		//Position of the Light
  int TimeToLive;				//Total lifetime, -1 if forever
  int AttachedToObject;			//ID of the object this light is attached to
#ifdef USE_GL
  int LightEmitter;
  float LightAmbient[4];
  float LightDiffuse[4];
  float LightSpecular[4];
  float ConstantAttenuation;
  float LinearAttenuation;
  float QuadraticAttenuation;
#endif
} light3d;

int EmitterUsed[400];



#ifdef USE_GL
GLubyte map[256][256][3];
GLuint GLTextureHandle[100];
GLuint mapHandle;


GLuint GLTerrainTextureHandle;

typedef struct TEXTURE{
	int w, h;
	unsigned char *data;
} TEXTURE;

//The source for the terrain blending
GLubyte BasicTiles[5][256][256][4];
GLuint GLBasicTileHandle[5];


GLubyte bufferpic[256][256][3];
GLubyte bufferpic1[32][32][3];
#endif

int	TEX_WIDTH = 256;
int	TEX_HEIGHT = 256;

int NumTrees;
int NumObjects;
int MaxNumObjects;
int editObject;
int editSlotMesh, editSlot;


int LocalPlayerLock;
int LockCheckInterval;

int NumTeams, numPlayers;


float     g_dwFrameCount;             // used for fps calc
float     g_dwFramesLast;             // ..
float     g_dwFrameTime;              // ..
float     g_speedfactor;              // ..
float	  g_s1;
float	  g_s2;
float	  g_s3;
float	  g_s4;
float	  g_s5;
float	  g_s6;
float	  g_s7;
float	  g_s8;
float	  g_s9;
float	  g_s10;
float     g_ispeedfactor;              // ..

long startime, endtime, dt, dt2, dt3, frameCount, oldframeCount, GameTime;


	int reloadTime;
	int missileReloadTime;

	int			curorder;			//Various temp. variables
	char tempstring[256];
	int tempint, tempemitter;
	float wWidth, wHeight;
	float TempYaw, TempPitch, TempDist;


	object3d	Objects[2000];  //Max number of Objects
	player3d	Player[128];	//Max number of players
	team3d		Team[10];		//Max number of teams
	ParticleSlot3d	ParticleSlots[20];
//	ParticleEmitter3d	ParticleEmitters[5000];
//	Particle3d			Particles[90000];
	light3d				Lights[100];
	int			v2v[10000];					//Converts VisVertices[] to Vertices[]
	mesh3d		Meshes[40];		//Currently there are 40 different meshes planned.


//	player3d	*Player=NULL;	//Max number of players
//	team3d		*Team=NULL;		//Max number of teams
//	object3d	*Objects=NULL;  //Max number of Objects
//	mesh3d		*Meshes=NULL;		//Currently there are 20 different meshes planned.
//	ParticleSlot3d	*ParticleSlots=NULL;
	ParticleEmitter3d	*ParticleEmitters=NULL;
	Particle3d			*Particles=NULL;
//	light3d				*Lights=NULL;

	vert3d		*TempVertices=NULL;
	face3d		*Faces=NULL;			//Main render-pipeline for Faces
	face3d		*PFaces=NULL;			//Main render-pipeline for Particle Faces
	float		*zOrder=NULL;		//Depth list of the faces
	int			*drawOrder=NULL;	//Depth-Sorted draw order (obsolete)
	vert3d		*Vertices=NULL;			//Main render-pipeline for Vertices
	vert3d		*VisVertices=NULL;		//Visible Vertices
    TlVertex3D	*xfVerts=NULL;		//Rendering stuff
    TlVertex3D	*prjVerts=NULL;
    TlVertex3D	*originalVerts=NULL;
//	tree3d		Trees[3000];	//Max number of Trees
	tree3d		*Trees=NULL;	//Max number of Trees


	
	int numfaces,numverts,numLSverts, numLSfaces;
	//numverts and numfaces are totals
	//NumLS... if the amount of verts/faces of the Landscape

	int curface, curvert;
	int curquad, visface, visverts;
	int tmode;

	float CamXPos, CamZPos, CamHeight, CamYaw, CamPitch, CamRoll, CamAltYaw;
	float SavXPos, SavYPos, SavHeight, SavYaw, SavPitch, SavRoll, SavAltYaw;

	float ControlXPos, ControlYPos, ControlHeight, ControlYaw, ControlPitch, ControlRoll, ControlAltYaw;
	
	float mySpeed, setSpeed;
	float yawTime, pitchTime, rollTime, accTime;
	float tempfloat;
	int xGridPos, zGridPos;
	int localplayer;		//global playerId of the local human player.

	float SunX;
	float SunY;
	float SunZ;
	float daytime;
	float fixtime;
	float daytimeframes;

	float SunRIntensity, SunGIntensity, SunBIntensity;

	int ShieldUpdateTime;

//	unsigned    engineChannel, windChannel, ambientChannel, launchChannel, dockChannel, diveChannel, lockChannel, speechChannel, speech2Channel;
    FMUSIC_MODULE *module=NULL;
	FSOUND_STREAM *stream1=NULL;
	FSOUND_STREAM *stream2=NULL;

//	FSOUND_SAMPLE *samples;
	FSOUND_SAMPLE *samples[60];
	long channels[128];
	int	 channelEmitter[128];


	int ActiveEditingFace;
	int ActiveFace;
	int TextureBlink;


ServerData Servers[4000];		//I seriously doubt that we will see more than 4000 gameservers on the internet.
int numServers, menuchoice, menuoffset;
ServerData myServer;			//This struct stores data about OUR server.
char myClientName[256];

int EngineSound=0;

text_t mytext; 

int frames = -1;

float dDelta = 0.2f;

int CameraMode = 0;		//0=cockpit, 1 to 6 =external
int savCameraMode;
int LookAtObject = 0;
int LastLookAtChange = 0;
int LastModeChange = 0;


FxBool ispm=FXFALSE;			//FXFALSE if am, FXTRUE if pm
static const unsigned max_soundchannels = 127;

int ShieldUpdateInterval = 10; //Increase shields by 1 every 10 frames

int midas_running = 0;
int glide_running = 0;

int use_lighing = 1; //Toggles main engine lighting on/off
int use_midas = 0; //Toggles sound on/off
FxBool GLTextures = FXTRUE;
FxBool GLFog = FXTRUE;
FxBool NightVision = FXFALSE;


FxBool MasterGamePause = FXFALSE;	//Toggle with "P" key
FxBool GameHalted = FXFALSE;		//True if game over
char GameHaltMsg[1024];					//Game-Over message

FxBool NoTexCalcs = FXFALSE;		//Skip the lengthy texture calculations
FxBool WireframeMode = FXFALSE;
FxBool TextureEditor = FXFALSE; //Set FXTRUE for editing Textures
FxBool Cheat = FXTRUE;			//Cheat mode. Start the program with the parameter "x" to cheat
FxBool presetBase = FXFALSE;	//Pre-built base setup
FxBool CompatibilityMode = FXFALSE;	//Render only the most important things
FxBool DemoMode = FXFALSE;			//Automatic Action

int AIPollInterval = 10; //Interval (in frames) between AI "think" polls


#define ReadTil(string) while (strcmp(line, string)) fscanf(fp, "%s",line);

//============================================================================
// End of variable declarations
//============================================================================




























// Follow the white rabbit.
























//============================================================================
// Plant a specific random seed so we can reproduce fractally generated stuff
//============================================================================
void randomize(unsigned num)
{
	if (num=0)
     srand ((unsigned) time (NULL));
	else
	 srand (num);

}


//============================================================================
// Test if number is between border1 and border2
//============================================================================
FxBool Within(float number, float border1, float border2)
{
	if (border2>border1)
	{
		if ((number>border1)&&(number<border2))
			return (FXTRUE);
	}
	else
	{
		if ((number>border2)&&(number<border1))
			return (FXTRUE);
	}
	return (FXFALSE);
}


//============================================================================
// Returns absolute difference between two numbers
//============================================================================
float Difference(float number1, float number2)
{
float diff;

if (number1>number2)
	if (number1-number2 < 180.0f)
		diff = abs(number1-number2);
	else
		diff = abs(number2+360-number1);
else
	if (number2-number2 < 180.0f)
		diff = abs(number2-number1);
	else
		diff = abs(number2+360-number1);
return diff;
}




//-------------- Crash Timer Stuff ----------------------------

void TimerInit(void)										// Initialize Our Timer (Get It Ready)
{
	memset(&timer, 0, sizeof(timer));						// Clear Our Timer Structure

	// Check To See If A Performance Counter Is Available
	// If One Is Available The Timer Frequency Will Be Updated
	if (!QueryPerformanceFrequency((LARGE_INTEGER *) &timer.frequency))
	{
		// No Performace Counter Available
		timer.performance_timer	= FALSE;					// Set Performance Timer To FALSE
		timer.mm_timer_start	= timeGetTime();			// Use timeGetTime() To Get Current Time
		timer.resolution		= 1.0f/10.0f;				// Set Our Timer Resolution To .001f
		timer.frequency			= 10;						// Set Our Timer Frequency To 1000
		timer.mm_timer_elapsed	= timer.mm_timer_start;		// Set The Elapsed Time To The Current Time
	}
	else
	{
		// Performance Counter Is Available, Use It Instead Of The Multimedia Timer
		// Get The Current Time And Store It In performance_timer_start
		QueryPerformanceCounter((LARGE_INTEGER *) &timer.performance_timer_start);
		timer.performance_timer			= TRUE;				// Set Performance Timer To TRUE
		// Calculate The Timer Resolution Using The Timer Frequency
		timer.resolution				= (float) (((double)1.0f)/((double)timer.frequency));
		// Set The Elapsed Time To The Current Time
		timer.performance_timer_elapsed	= timer.performance_timer_start;
	}
}

unsigned long TimerGetTime()										// Get Time In Milliseconds
{
	__int64 time;											// time Will Hold A 64 Bit Integer

	if (timer.performance_timer)							// Are We Using The Performance Timer?
	{
		QueryPerformanceCounter((LARGE_INTEGER *) &time);	// Grab The Current Performance Time
		// Return The Current Time Minus The Start Time Multiplied By The Resolution And 1000 (To Get MS)
		return ( (unsigned long) ( time - timer.performance_timer_start) );
	}
	else
	{
		// Return The Current Time Minus The Start Time Multiplied By The Resolution And 1000 (To Get MS)
		return( (unsigned long) ( timeGetTime() - timer.mm_timer_start) );
	}
}














//----------------- Terrain LOD Stuff ----------------------------

long intdist(int  x1, int z1, int x2, int z2)
{
 long a, b, c;
 if (x1>x2)
	a=(long)(x1)-(long)(x2);
 else
	a=(long)(x2)-(long)(x1);
 if (z1>z2)
	b=(long)(z1)-(long)(z2);
 else
	b=(long)(z2)-(long)(z1);

 c=(a*a) + (b*b);
 return (c);
}


void setVertex(int x, int z) {

	x=x/2;
	z=z/2;

	if (Vertices[Faces[facexy[x][z]].srcVerts[0]].v.b!=0)
	{
	glNormal3f( Vertices[Faces[facexy[x][z]].srcVerts[0]].normals.x, 
				Vertices[Faces[facexy[x][z]].srcVerts[0]].normals.y,
				Vertices[Faces[facexy[x][z]].srcVerts[0]].normals.x);
	}
	else
	{
		glNormal3f( 0.0f, 0.0f, 0.0f); 
	}
	
	glVertex3f(	Vertices[Faces[facexy[x][z]].srcVerts[0]].v.x,
				Vertices[Faces[facexy[x][z]].srcVerts[0]].v.y,
				Vertices[Faces[facexy[x][z]].srcVerts[0]].v.z);

}


void triangle(int x1, int z1, int x2, int z2, int x3, int z3) 
{	

	if( (x1>128) || (x2>128) || (x3>128) ||
		(z1>128) || (z2>128) || (z3>128) ||
		(x1<0) || (x2<0) || (x3<0) ||
		(z1<0) || (z2<0) || (z3<0))
		return;
	
	glBegin(GL_TRIANGLES);
	glTexCoord2f((float)x1/(float)texScale, (float)z1/(float)texScale);
	setVertex(x1, z1);
	
	glTexCoord2f((float)x2/(float)texScale, (float)z2/(float)texScale);
	setVertex(x2, z2);
	
	glTexCoord2f((float)x3/(float)texScale, (float)z3/(float)texScale);
	setVertex(x3, z3);
	glEnd();

}

void draw_point(int x, int z, int width, int direction) {
	
	switch(direction) {
	case NORTH:
		triangle(x, z, x + width, z - width, x - width, z - width);
		return;
	case SOUTH:
		triangle(x, z, x - width, z + width, x + width, z + width);
		return;
	case EAST:
		triangle(x, z, x + width, z + width, x + width, z - width);
		return;
	case WEST:
		triangle(x, z, x - width, z - width, x - width, z + width);
		return; 
	case NORTH_L:
		triangle(x, z, x, z - width, x - width, z - width);
		return;
	case NORTH_R:
		triangle(x, z, x + width, z - width, x, z - width);
		return;
		
	case SOUTH_L:
		triangle(x, z, x - width, z + width, x, z + width);
		return;
	case SOUTH_R:
		triangle(x, z, x, z + width, x + width, z + width);
		return;
		
	case EAST_T:
		triangle(x, z, x + width, z, x + width, z - width);
		return;
	case EAST_B:
		triangle(x, z, x + width, z + width, x + width, z);
		return;
		
	case WEST_T:
		triangle(x, z, x - width, z - width, x - width, z);
		return;
	case WEST_B:
		triangle(x, z, x - width, z, x - width, z + width);
		return;
	default:
		break;
	};
}



int location[3];
int lod_level = 8;

void reset_quad_tree(void) {
   int i, o;
	
   for(i=0; i<QUAD_MAP; i++)
   for(o=0; o<QUAD_MAP; o++)
      quadtree[i][o] = UNKNOWN;
}

void setup_quadtree(int x, int z, int width) {
   int width2;
   int v1[3];

   v1[0] = x;
   v1[2] = z;
	
   width2 = width / 2;


	if (CamXPos>0)
		xGridPos = (int)(CamXPos/scalefactor);
	else
		xGridPos = (int)(CamXPos/scalefactor)-1;

	if (CamZPos>0)
		zGridPos = (int)(CamZPos/scalefactor)+1;
	else
		zGridPos = (int)(CamZPos/scalefactor);

   location[0]=(MapDimension - xGridPos - 1)*2;
   location[2]=(MapDimension - zGridPos)*2;
   if((width > 1) && (intdist(x, z, location[0], location[2]) < (long)(width*lod_level * width*lod_level))) {

	quadtree[x][z] = NODE_POINT;
	quadtree[x - width2][z - width2] = EDGE_POINT;
	quadtree[x + width2][z - width2] = EDGE_POINT;
    quadtree[x - width2][z + width2] = EDGE_POINT;
    quadtree[x + width2][z + width2] = EDGE_POINT;
	setup_quadtree(x - width2, z - width2, width2);
	setup_quadtree(x + width2, z - width2, width2);
	setup_quadtree(x - width2, z + width2, width2);
	setup_quadtree(x + width2, z + width2, width2);
			
   }
   else {
		quadtree[x][z] = EDGE_POINT;
   }
}

void draw(int x, int z, int width, int direction) {
	int width2;
	
   if(width > 1 ) {

	  glBindTexture(GL_TEXTURE_2D, GLTerrainTextureHandle);

      width2 = width / 2;
		
      if(quadtree[x][z] == NODE_POINT) {
			
			// NORTH
			if(quadtree[x - width2][z - width2] == EDGE_POINT &&
				quadtree[x + width2][z - width2] == EDGE_POINT) {
				
				if(quadtree[x][z - width*2] == NODE_POINT || (z-width)%MAP==0) {
					draw_point(x, z, width, NORTH_L);
					draw_point(x, z, width, NORTH_R);
				}
            else {
					draw_point(x, z, width, NORTH);
            }
         }
         else if(quadtree[x - width2][z - width2] == EDGE_POINT) {
				draw_point(x, z, width, NORTH_L);
         }
         else if(quadtree[x + width2][z - width2] == EDGE_POINT) {
				draw_point(x, z, width, NORTH_R);
         }
			
         // SOUTH
			if(quadtree[x - width2][z + width2] == EDGE_POINT &&
				quadtree[x + width2][z + width2] == EDGE_POINT) {
				
				if(quadtree[x][z + width*2] == NODE_POINT || (z+width)%MAP==0) {
					draw_point(x, z, width, SOUTH_L);
					draw_point(x, z, width, SOUTH_R);
				}
            else {
					draw_point(x, z, width, SOUTH);
            }
         }
         else if(quadtree[x - width2][z + width2] == EDGE_POINT) {
				draw_point(x, z, width, SOUTH_L);
         }
         else if(quadtree[x + width2][z + width2] == EDGE_POINT) {
				draw_point(x, z, width, SOUTH_R);
         }
			
         // EAST
			if(quadtree[x + width2][z - width2] == EDGE_POINT &&
				quadtree[x + width2][z + width2] == EDGE_POINT) {
				
				if(quadtree[x + width*2][z] == NODE_POINT || (x+width)%MAP==0) {
					draw_point(x, z, width, EAST_T);
					draw_point(x, z, width, EAST_B);
				}
            else {
					draw_point(x, z, width, EAST);
            }
         }
         else if(quadtree[x + width2][z - width2] == EDGE_POINT) {
				draw_point(x, z, width, EAST_T);
         }
         else if(quadtree[x + width2][z + width2] == EDGE_POINT) {
				draw_point(x, z, width, EAST_B);
         }
			
         // WEST
			if(quadtree[x - width2][z - width2] == EDGE_POINT &&
				quadtree[x - width2][z + width2] == EDGE_POINT) {
				
				if(quadtree[x - width*2][z] == NODE_POINT || (x-width)%MAP==0) {
					draw_point(x, z, width, WEST_T);
					draw_point(x, z, width, WEST_B);
				}
            else {
					draw_point(x, z, width, WEST);
            }
         }
         else if(quadtree[x - width2][z - width2] == EDGE_POINT) {
				draw_point(x, z, width, WEST_T);
         }
         else if(quadtree[x - width2][z + width2] == EDGE_POINT) {
				draw_point(x, z, width, WEST_B);
         }
						
      }
      else {
			return;
      }
		
      draw(x - width2, z - width2, width2, NW);
      draw(x + width2, z - width2, width2, NE);
      draw(x - width2, z + width2, width2, SW);
      draw(x + width2, z + width2, width2, SE);
   }
}

//-------------- End of Terrain LOD Stuff ----------------------











//-------------- Screenshot code -------------------------------
void makeBGR (unsigned char *p, int size)
{
	unsigned char temp;
	int i;

	for (i = 0; i < size * 3; i += 3)
	{
		temp = p[i];
		p[i] = p[i + 2];
		p[i + 2] = temp;
	}
}
void writeTGA (char name[128], unsigned char *buff, int w, int h)
{
	unsigned char header[14] = "\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00";
	unsigned char info[6];
	FILE *s;

	s = fopen (name, "w");

	// write the header to the file
	fwrite (header, sizeof (char), 12, s);
	
	// image dimension information
	info[0] = w;
	info[1] = (w >> 8);
	info[2] = h;
	info[3] = (h >> 8);
	info[4] = 24;
	info[5] = 0;
	
	// write dimension info to file
	fwrite (&info, sizeof (char), 6, s);
	
	// since the frame buffer is RGB we need to convert it to BGR for a targa file
	makeBGR (buff, w * h);

	// dump the image data to the file
	fwrite (buff, sizeof (char), w * h * 3, s);

	fclose (s);
}
int Screenshot(int winW, int winH)
{
	unsigned char *fBuffer;
	FILE *s;
	char name[128];
	int Shotnum=0;
	FxBool FoundFree=FXFALSE;


	do
	{
		sprintf(name, "shot%i.tga", Shotnum);
		s = fopen (name, "r");
		if (s != NULL)
		{
			Shotnum+=1;
			fclose(s);
		}
		else
			FoundFree=FXTRUE;
	} while (!FoundFree);



//	*fBuffer = (unsigned char)(malloc (winW * winH * 3));
	fBuffer = new unsigned char[winW*winH*3];

	// no memory allocated for image data
	if (fBuffer == NULL)
		return 0;

	// read our image data from the frame buffer
	glReadPixels (0, 0, winW, winH, GL_RGB, GL_UNSIGNED_BYTE, fBuffer);

	// write the image data to a .tga file
	writeTGA (name, fBuffer, winW, winH);

//	free (fBuffer);
	delete[] fBuffer;

	return 1;
}

//---------------- End of screenshot code -----------------






#ifdef USE_GL
static void 
drawString(char *string, GLfloat x, GLfloat y, GLfloat color[4])
{
    glColor4fv(color);
    glRasterPos2f(x, y);
    glCallLists(strlen(string), GL_BYTE, (GLbyte *) string);
}

static void
drawStringOutline(char *string, GLfloat x, GLfloat y,
		  GLfloat color[4], GLfloat outline[4])
{
    drawString(string, x-1.0F, y, outline);
    drawString(string, x+1.0F, y, outline);
    drawString(string, x, y-1.0F, outline);
    drawString(string, x, y+1.0F, outline);
    drawString(string, x, y, color);
}

static void
begin2D(int width, int height)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0F, (GLfloat) width, 0.0F, (GLfloat) height, -1.0F, 1.0F);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}

static void
end2D(void)
{
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}
	
#endif	
	
void AddMessage(char *msg, int r, int g, int b)
{

	conMsg.Age=254;
	conMsg.r=r;
	conMsg.g=g;
	conMsg.b=b;
	sprintf(conMsg.Text, "%s", msg);
	
}


void MessageHandler(void)
{
	if (conMsg.Age>0)
	{
		sprintf(tempstring, "\\a(0 0 0 %i)%s", conMsg.Age, conMsg.Text);
		mytext.Size(10);
		mytext.Draw(135, 450, tempstring);
		sprintf(tempstring, "\\a(%i %i %i %i)%s", conMsg.r, conMsg.g, conMsg.b, conMsg.Age, conMsg.Text);
		mytext.Size(10);
		mytext.Draw(136, 451, tempstring);
		conMsg.Age--;
	}
	else
		conMsg.Age=0;
}




void DisplayChatMessage(char *s)
{
	AddMessage(s, 255, 255, 0);
}

int GetNextFreeClientID(void)
{
	int i;
	for (i=0; i<myServer.MaxClients; i++)
	{
		if (!Player[i].isActive)
			return(i);
	}
	return (-1);
}

int GetNextAutobalanceTeam(void)
{
	int team1=0;
	int team2=0;
	int i;

	for (i=0; i<myServer.MaxClients; i++)
	{
		if (!Player[i].isActive)
			if (Player[i].Team == 0)
				team1++;
			else
				team2++;

	}
	if (team2>team1)
		return(0);
	else
		return(1);
}

void FindNewOwnerForObjects(int ClientID)
{
}
	
//============================================================================
// Without this, gamespeed would be equal to r_speed!
//============================================================================
void CalcFrameRate()
{
    float dwFrames;
	float dwTime;
	float targetmaxvdist;
	float TargetFogIntensity;

    dwFrames=0.0f;
	dwTime=0.0f;
	targetmaxvdist=0.0f;
	TargetFogIntensity=0.0f;

    g_dwFrameCount++;

    dwTime = (float)(clock() - g_dwFrameTime);
    if( dwTime > 100.0f )
    {
        dwFrames      = (float)((g_dwFrameCount*1000.0f)/dwTime);
		g_dwFrameTime  = (float)clock();
        g_dwFrameCount = 0.0f;
    }

    if( dwFrames == 0.0f )
        return;

    if (dwFrames != g_dwFramesLast)
        g_dwFramesLast = dwFrames;

	g_s10=g_s9;
	g_s9=g_s8;
	g_s8=g_s7;
	g_s7=g_s6;
	g_s6=g_s5;
	g_s5=g_s4;
	g_s4=g_s3;
	g_s3=g_s2;
	g_s2=g_s1;
	g_s1=(float)(30.0f / g_dwFramesLast);
	g_speedfactor=(g_s1+g_s2+g_s3+g_s4+g_s5+g_s6+g_s7+g_s8+g_s9+g_s10)/10.0f;
	g_ispeedfactor = 1.0f/g_speedfactor;
	MaxParticles = (int)(5000.0f * g_ispeedfactor);
//	AbsoluteMaxParticles = 10000 * (int)g_ispeedfactor;
	AbsoluteMaxParticles = (int)(10000.0f * g_ispeedfactor);


	if (WireframeMode)
		targetmaxvdist = 5000.0f*(g_ispeedfactor);
	else
		targetmaxvdist = 2000.0f*(g_ispeedfactor);

	if (targetmaxvdist<400.0f)
		targetmaxvdist=400.0f;

	if (targetmaxvdist>maxvdist)
		maxvdist+=10.0f;
	else
		maxvdist-=10.0f;

	

	FarClip = maxvdist*100000.0f;

	if ( (winWidth==1280) || (winWidth==1600) )
	{
		TargetFogIntensity	= 0.0030f*(g_speedfactor);
		if (TargetFogIntensity>0.009f)
			TargetFogIntensity=0.009f;
	
		if (TargetFogIntensity>FogIntensity)
			FogIntensity+=0.00001f;
		else
			FogIntensity-=0.00001f;
	}
	if ( (winWidth==800) || (winWidth==1024) )
	{
		TargetFogIntensity	= 0.0010f*(g_speedfactor);
		if (TargetFogIntensity>0.006f)
			TargetFogIntensity=0.006f;

		if (current_weather.WeatherType==WEATHERTYPE_FOG)	
			TargetFogIntensity*=80.0f;

		if (TargetFogIntensity>FogIntensity)
			FogIntensity+=0.00001f;
		else
			FogIntensity-=0.00001f;
	}
	if (winWidth == 640)
	{
		TargetFogIntensity	= 0.0005f*(g_speedfactor);
		if (TargetFogIntensity>0.003f)
			TargetFogIntensity=0.003f;

		if (current_weather.WeatherType==WEATHERTYPE_FOG)	
			TargetFogIntensity*=80.0f;
	
		if (TargetFogIntensity>FogIntensity)
			FogIntensity+=0.00002f;
		else
			FogIntensity-=0.00002f;
	}


#ifdef USE_GLIDE
#else
		glFogf(GL_FOG_DENSITY, (FogIntensity/2.0f));
#endif


}





void SaveSlotPosition(int MeshId)
{
	FILE *DataFile = NULL;
	int curpos;
	char *file;

	if (MeshId==OBJECTMESH_SHIP)
		file="meshes\\NEWSHP1.SLT";
	if (MeshId==OBJECTMESH_COMMANDCENTER)
		file="meshes\\COMMAND.SLT";
	if (MeshId==OBJECTMESH_POWERPLANT)
		file="meshes\\POWER.SLT";
	if (MeshId==OBJECTMESH_MINE)
		file="meshes\\MINE.SLT";
	if (MeshId==OBJECTMESH_MISSILE)
		file="meshes\\MISSILE.SLT";
	if (MeshId==OBJECTMESH_SAM)
		file="meshes\\SAM.SLT";
	if (MeshId==OBJECTMESH_AAA)
		file="meshes\\AAA.SLT";
	if (MeshId==OBJECTMESH_UPLINK)
		file="meshes\\UPLINK.SLT";
	if (MeshId==OBJECTMESH_UPLINK2)
		file="meshes\\UPLINK2.SLT";
	if (MeshId==OBJECTMESH_LIGHTTANK)
		file="meshes\\LTANK.SLT";
	if (MeshId==OBJECTMESH_COMMANDCENTERMENU)
		file="meshes\\COMMAND.SLT";
	if (MeshId==OBJECTMESH_SHIP1)
		file="meshes\\SHIP1.SLT";
	if (MeshId==OBJECTMESH_SHIP2)
		file="meshes\\SHIP2.SLT";
	if (MeshId==OBJECTMESH_SHIP3)
		file="meshes\\SHIP3.SLT";

	DataFile = fopen(file,"w");
	if (DataFile == NULL)
		return;

	fprintf(DataFile,"%i\n",ParticleSlots[MeshId].numslots);
	
	for (curpos=0; curpos<ParticleSlots[MeshId].numslots; curpos++)
	{
		fprintf(DataFile,"%f %f %f\n",ParticleSlots[MeshId].Vertices[curpos].v.x,ParticleSlots[MeshId].Vertices[curpos].v.y,ParticleSlots[MeshId].Vertices[curpos].v.z);
	}
	if (DataFile != NULL)
		fclose(DataFile);
}

void LoadSlotPosition(int MeshId)
{
	FILE *DataFile = NULL;
	int curpos;
	char  line[80];
	char *file;

	if (MeshId==OBJECTMESH_SHIP)
		file="meshes\\NEWSHP1.SLT";
	if (MeshId==OBJECTMESH_COMMANDCENTER)
		file="meshes\\COMMAND.SLT";
	if (MeshId==OBJECTMESH_POWERPLANT)
		file="meshes\\POWER.SLT";
	if (MeshId==OBJECTMESH_MINE)
		file="meshes\\MINE.SLT";
	if (MeshId==OBJECTMESH_MISSILE)
		file="meshes\\MISSILE.SLT";
	if (MeshId==OBJECTMESH_SAM)
		file="meshes\\SAM.SLT";
	if (MeshId==OBJECTMESH_AAA)
		file="meshes\\AAA.SLT";
	if (MeshId==OBJECTMESH_UPLINK)
		file="meshes\\UPLINK.SLT";
	if (MeshId==OBJECTMESH_UPLINK2)
		file="meshes\\UPLINK2.SLT";
	if (MeshId==OBJECTMESH_LIGHTTANK)
		file="meshes\\LTANK.SLT";
	if (MeshId==OBJECTMESH_COMMANDCENTERMENU)
		file="meshes\\COMMAND.SLT";
	if (MeshId==OBJECTMESH_SHIP1)
		file="meshes\\SHIP1.SLT";
	if (MeshId==OBJECTMESH_SHIP2)
		file="meshes\\SHIP2.SLT";
	if (MeshId==OBJECTMESH_SHIP3)
		file="meshes\\SHIP3.SLT";

	DataFile = fopen(file,"r");
	if (DataFile == NULL)
		return;


	fgets(line,80,DataFile);
	sscanf(line,"%i", &ParticleSlots[MeshId].numslots);

	for (curpos=0; curpos<ParticleSlots[MeshId].numslots; curpos++)
	{
		fgets(line,80,DataFile);
		sscanf(line,"%f %f %f", &ParticleSlots[MeshId].Vertices[curpos].v.x,&ParticleSlots[MeshId].Vertices[curpos].v.y,&ParticleSlots[MeshId].Vertices[curpos].v.z);
	}
	if (DataFile != NULL)
		fclose(DataFile);
}













void SaveMenuPositions(void)
{
	FILE *DataFile = NULL;
	int curpos;
	char *file;

	file = "menu.dat";

	DataFile = fopen(file,"w");
	if (DataFile == NULL)
		return;

	for (curpos=0; curpos<9; curpos++)
	{
		fprintf(DataFile,"%f\n%f\n%f\n",MenuPositions[curpos][0],MenuPositions[curpos][1],MenuPositions[curpos][2]);
		fprintf(DataFile,"%f\n%f\n%f\n",MenuPositions[curpos][3],MenuPositions[curpos][4],MenuPositions[curpos][5]);
	}
	if (DataFile != NULL)
		fclose(DataFile);
}

void LoadMenuPositions(void)
{
	FILE *DataFile = NULL;
	char *file;
	int curpos;
	char  line[80];

	file = "menu.dat";

	DataFile = fopen(file,"r");
	if (DataFile == NULL)
		return;

	for (curpos=0; curpos<9; curpos++)
	{
		fgets(line,80,DataFile);
		sscanf(line,"%f", &MenuPositions[curpos][0]);
		fgets(line,80,DataFile);
		sscanf(line,"%f", &MenuPositions[curpos][1]);
		fgets(line,80,DataFile);
		sscanf(line,"%f", &MenuPositions[curpos][2]);
		fgets(line,80,DataFile);
		sscanf(line,"%f", &MenuPositions[curpos][3]);
		fgets(line,80,DataFile);
		sscanf(line,"%f", &MenuPositions[curpos][4]);
		fgets(line,80,DataFile);
		sscanf(line,"%f", &MenuPositions[curpos][5]);

//		for (i=0; i<3; i++)
//			MenuPositions[curpos][i] = MenuPositions[curpos][i] / 0.002f * 0.00025f;

	}
	if (DataFile != NULL)
		fclose(DataFile);
}







void SaveTextureData(void)
{
	FILE *DataFile = NULL;
	int curface;
	char *file;

	if (Objects[editObject].ObjectMesh==OBJECTMESH_SHIP)
		file="meshes\\NEWSHP1.TEX";
	if (Objects[editObject].ObjectMesh==OBJECTMESH_COMMANDCENTER)
		file="meshes\\COMMAND.TEX";
	if (Objects[editObject].ObjectMesh==OBJECTMESH_POWERPLANT)
		file="meshes\\POWER.TEX";
	if (Objects[editObject].ObjectMesh==OBJECTMESH_MINE)
		file="meshes\\MINE.TEX";
	if (Objects[editObject].ObjectMesh==OBJECTMESH_MISSILE)
		file="meshes\\MISSILE.TEX";
	if (Objects[editObject].ObjectMesh==OBJECTMESH_SAM)
		file="meshes\\SAM.TEX";
	if (Objects[editObject].ObjectMesh==OBJECTMESH_AAA)
		file="meshes\\AAA.TEX";
	if (Objects[editObject].ObjectMesh==OBJECTMESH_UPLINK)
		file="meshes\\UPLINK.TEX";
	if (Objects[editObject].ObjectMesh==OBJECTMESH_LIGHTTANK)
		file="meshes\\LTANK.TEX";
	if (Objects[editObject].ObjectMesh==OBJECTMESH_SKYSPHERE1)
		file="meshes\\SKY1.TEX";
	if (Objects[editObject].ObjectMesh==OBJECTMESH_SKYSPHERE2)
		file="meshes\\SKY2.TEX";
	if (Objects[editObject].ObjectMesh==OBJECTMESH_COMMANDCENTERMENU)
		file="meshes\\MENUCMD.TEX";
	if (Objects[editObject].ObjectMesh==OBJECTMESH_SHIP1)
		file="meshes\\SHIP1.TEX";
	if (Objects[editObject].ObjectMesh==OBJECTMESH_SHIP2)
		file="meshes\\SHIP2.TEX";
	if (Objects[editObject].ObjectMesh==OBJECTMESH_SHIP3)
		file="meshes\\SHIP3.TEX";
	if (Objects[editObject].ObjectMesh==OBJECTMESH_TANK1_LOD1)
		file="meshes\\tank1-lod1.TEX";
	if (Objects[editObject].ObjectMesh==OBJECTMESH_TANK1_LOD2)
		file="meshes\\tank1-lod2.TEX";
	if (Objects[editObject].ObjectMesh==OBJECTMESH_TANK1_LOD3)
		file="meshes\\tank1-lod3.TEX";
	if (Objects[editObject].ObjectMesh==OBJECTMESH_TANK2_LOD1)
		file="meshes\\tank2-lod1.TEX";
	if (Objects[editObject].ObjectMesh==OBJECTMESH_TANK2_LOD2)
		file="meshes\\tank2-lod2.TEX";
	if (Objects[editObject].ObjectMesh==OBJECTMESH_TANK2_LOD3)
		file="meshes\\tank2-lod3.TEX";


	DataFile = fopen(file,"w");
	if (DataFile == NULL)
		return;

//	fprintf(DataFile,"Deranged Raid texture data\n");
	for (curface=0; curface<Meshes[Objects[editObject].ObjectMesh].numfaces; curface++)
	{
		fprintf(DataFile,"%i\n%i\n",Meshes[Objects[editObject].ObjectMesh].Faces[curface].Texture, Meshes[Objects[editObject].ObjectMesh].Faces[curface].TextureMode);
		if (Meshes[Objects[editObject].ObjectMesh].Faces[curface].Transparent)
			fprintf(DataFile,"1\n");
		else
			fprintf(DataFile,"0\n");
		if (Meshes[Objects[editObject].ObjectMesh].Faces[curface].isLight)
			fprintf(DataFile,"1\n");
		else
			fprintf(DataFile,"0\n");
			
	}
	if (DataFile != NULL)
		fclose(DataFile);
}

void LoadTextureData(char *file, int meshId)
{
	FILE *DataFile = NULL;
	int curface;
	int transparent;
	int islight;
	char  line[80];

	DataFile = fopen(file,"r");
	if (DataFile == NULL)
		return;

	for (curface=0; curface<Meshes[meshId].numfaces; curface++)
	{
//		fprintf(DataFile,"%i\n%i\n",Meshes[Objects[editObject].ObjectMesh].Faces[curface].Texture, Meshes[Objects[editObject].ObjectMesh].Faces[curface].TextureMode);
		fgets(line,80,DataFile);
		sscanf(line,"%i", &Meshes[meshId].Faces[curface].Texture);
		fgets(line,80,DataFile);
		sscanf(line,"%i", &Meshes[meshId].Faces[curface].TextureMode);
		fgets(line,80,DataFile);
		sscanf(line,"%i", &transparent);
		fgets(line,80,DataFile);
		sscanf(line,"%i", &islight);
		if ((transparent==1) || (CompatibilityMode))
			Meshes[meshId].Faces[curface].Transparent=FXTRUE;
		else
			Meshes[meshId].Faces[curface].Transparent=FXFALSE;
		if (islight==1)
			Meshes[meshId].Faces[curface].isLight=FXTRUE;
		else
			Meshes[meshId].Faces[curface].isLight=FXFALSE;
		if (Meshes[meshId].Faces[curface].TextureMode==0)
			Meshes[meshId].Faces[curface].TextureMode=2;
	}
	if (DataFile != NULL)
		fclose(DataFile);
}




void DumpUsedTextures(void)
{
	FILE *DataFile = NULL;
	int curface=0;
	int curmesh=0;
	FxBool used[100];



	for (curmesh=1; curmesh<=OBJECTMESH_SHIP3 ; curmesh++)
	for (curface=0; curface<Meshes[curmesh].numfaces; curface++)
	{
		used[Meshes[curmesh].Faces[curface].Texture]=FXTRUE;

	}

	DataFile = fopen("Textures.log","w");
	if (DataFile == NULL)
		return;

	for (curface=0; curface<=maxTexture; curface++)
	{
		if (used[curface]==FXTRUE)
				fprintf(DataFile,"Texture %i used: TRUE\n", curface);
		else
				fprintf(DataFile,"Texture %i used: FALSE\n", curface);
	}

	if (DataFile != NULL)
		fclose(DataFile);
}



void WriteErrorLog(char *errmsg)
{
	FILE *DataFile = NULL;
	DataFile = fopen("DerangedRaid.log","a");
	if (DataFile == NULL)
		return;
	fprintf(DataFile,errmsg);
	fprintf(DataFile,"\n");
	fclose(DataFile);

}





//============================================================================
// Get Distance between x and y
//============================================================================
float GetDistance(float x, float y)
{
return ( (float)sqrt((x*x) + (y*y)) );
}


//============================================================================
// Get the distance from x1/y1/z1 to x2/y2/z2
//============================================================================
void GetDistance3d(float x1, float y1, float z1, float x2, float y2, float z2)
{
float c;
float a,b;

a = x2-x1;
b = y2-y1;
c = GetDistance(a, b);

a = z2-z1;
b = c;
c = GetDistance(a, b);
TempDist = c;
}



//============================================================================
// Get the relative angle from x1/y1/z1 to x2/y2/z2
//============================================================================
void GetRelativeAngle(float x1, float y1, float z1, float x2, float y2, float z2)
{
float c;
float a,b;

a = x2-x1;
b = y2-y1;
c = GetDistance(a, b);

if (b>0)
TempYaw = (float)asin((double)(a/c)); //This works for 0-179 degrees only
else
TempYaw = (float)((pi)-asin((double)(a/c))); //for 180-359 degrees, use this.

a = z2-z1;
b = c;
c = GetDistance(a, b);
TempDist = c;
//if (a<0)
TempPitch = (float)asin((double)(a/c));  // This should work all the time.
//else
//TempPitch = (float)((pi)- asin((double)(a/c)));  // This should work all the time.


//sprintf(tempstring, "a=%.3f b=%.3f c=%.3f tp=%.3f", a, b, c, TempPitch);
//AddMessage(tempstring, 0, 255, 0);

}



//============================================================================
// Get the angle to make one Object intercept the other
//============================================================================
void GetObjectInterceptAngle(int obj1, int obj2, float heightOffset)
{

		GetRelativeAngle(	Objects[obj1].xPos, Objects[obj1].zPos, Objects[obj1].Height,
							Objects[obj2].xPos, Objects[obj2].zPos, Objects[obj2].Height-heightOffset
							//Yaw, Pitch );
							);

}



//============================================================================
// Update navigation information of Object pursuerId to follow Object targetId
//============================================================================
void HomeObject(int targetId, int pursuerId, float heightOffset)
{

			GetObjectInterceptAngle(pursuerId, targetId, heightOffset);
			Objects[pursuerId].AITargetYaw = (TempYaw / (2*pi))*360;
			//Don't mess with pitch of Tanks
			if (Objects[pursuerId].ObjectMesh != OBJECTMESH_LIGHTTANK)
				if (!Objects[pursuerId].isDiving)	//Also don't bother dying ships
			Objects[pursuerId].AITargetPitch = (TempPitch / (2*pi))*360;

if (Objects[pursuerId].ObjectType==OBJECTTYPE_SHIP)
{
	if (!Objects[pursuerId].isDiving)
		Objects[pursuerId].AITargetPitch = -Objects[pursuerId].AITargetPitch;
//	sprintf(tempstring, "TY=%.3f TP=%.3f", Objects[pursuerId].AITargetYaw, Objects[pursuerId].AITargetPitch);
//	AddMessage(tempstring, 0, 255, 0);
}

}




//============================================================================
// Gives the direction to move from angle 1 to angle2
//============================================================================
//This is tricky: our angle system goes from 0 to 360, and we need to 
//somehow tell the program that in order to move from 2 downto 358 degrees it should
//go *down*(2,1,0,359,358) instead of up (2,3,4,5,...)
//This function returns 1 for up, and -1 for down
float TransitAngles(float angle1, float angle2)
{
	float newangle;

	if (angle1<angle2)	//use angle1 as new 0°
	{
		newangle=angle2-angle1; //since angle1 is now virtually 0, decrease angle2
		if (newangle>180)
			return(-1);
		else
			return (1);
	}
	else				//use angle2 as new 0°
	{
		newangle=angle1-angle2; //since angle2 is now virtually 0, decrease angle1
		if (newangle>180)
			return(1);
		else
			return (-1);
	}

}


//============================================================================
// Make sure angle doesn't exceed the 0 to 360 degree range
//============================================================================
float FixAngle(float angle)
{
	float x;
	x=angle;
	if (x < -65530.0f)
		x = 0.0f;
	while (x < 0.0f) {
		x += 360.0f;
	}
	while (x >= 360.0f) {
		x -= 360.0f;
	}
return (x);
}





//============================================================================
// Swap two faces (Currently not used, it's for the (obsolete) QuickSort function)
//============================================================================
void swapface (int eins, int zwei)
{
float temp;
int temp1;
temp = zOrder[eins];
zOrder[eins] = zOrder[zwei];
zOrder[zwei] = temp;

temp1 = drawOrder[eins];
drawOrder[eins] = drawOrder[zwei];
drawOrder[zwei] = temp1;
}


//============================================================================
// Alternative Depth sorting (Not used anymore, we use hardware WBUFFER now)
//============================================================================
void QuickSort (int Klein, int Gross)
{
int I, J;
int ZufIndex;
float TeilStck;
	
	if (Klein < Gross) {
      if ((Gross - Klein) == 1) {
		  if (zOrder[Klein] < zOrder[Gross]) {
            swapface(Klein, Gross);
		  }
      } else {
         ZufIndex = random(Gross-Klein) + Klein;
         swapface(Gross, ZufIndex);
         TeilStck = zOrder[Gross];
         do {
            I = Klein; J = Gross;
			while ((I < J) && (zOrder[I] >= TeilStck)) {
               I = I + 1;
            }
            while ((J > I) && (zOrder[J] <= TeilStck)) {
               J = J - 1;
            }

            if (I < J) {
               swapface(I, J);
            }
         } while (I < J);

        swapface(I, Gross);

        if ((I - Klein) < (Gross - I)) {
            QuickSort(Klein, (I - 1));
            QuickSort((I + 1), Gross);
        } else {
            QuickSort((I + 1), Gross);
            QuickSort((Klein), I - 1);
		}
      }
	}
}







#ifdef USE_GL
//============================================================================
// Adds a dynamic lightsource
//============================================================================
void AddLight(int objectId, int TimeToLive, 
			  float AmbientR, float AmbientG, float AmbientB,
			  float DiffuseR, float DiffuseG, float DiffuseB,
			  float SpecularR, float SpecularG, float SpecularB,
			  float ConstantAttenuation, float LinearAttenuation, float QuadraticAttenuation,
			  float xPos, float Height, float zPos)
{
  int i;

  if (NumLights>=98)
	  return;
  if (NumLights>0)
	  return;

  Lights[NumLights].xPos = xPos;
  Lights[NumLights].zPos = zPos;
  Lights[NumLights].Height = Height;
  Lights[NumLights].TimeToLive = TimeToLive;
  Lights[NumLights].AttachedToObject = objectId;
  Lights[NumLights].LightAmbient[0]=AmbientR;
  Lights[NumLights].LightAmbient[1]=AmbientG;
  Lights[NumLights].LightAmbient[2]=AmbientB;
  Lights[NumLights].LightAmbient[3]=0.0f;
  Lights[NumLights].LightDiffuse[0]=DiffuseR;
  Lights[NumLights].LightDiffuse[1]=DiffuseG;
  Lights[NumLights].LightDiffuse[2]=DiffuseB;
  Lights[NumLights].LightDiffuse[3]=1.0f;
  Lights[NumLights].LightSpecular[0]=SpecularR;
  Lights[NumLights].LightSpecular[1]=SpecularG;
  Lights[NumLights].LightSpecular[2]=SpecularB;
  Lights[NumLights].LightSpecular[3]=0.0f;
  Lights[NumLights].ConstantAttenuation=ConstantAttenuation;
  Lights[NumLights].LinearAttenuation=LinearAttenuation;
  Lights[NumLights].QuadraticAttenuation=QuadraticAttenuation;

  for (i=1; i<=NumLights+1; i++)
	if (EmitterUsed[i]!=1)
	{
		Lights[NumLights].LightEmitter = i;
		EmitterUsed[i]=1;
	}

  glEnable((unsigned)(0x4000+Lights[NumLights].LightEmitter));
  NumLights++;

}


//============================================================================
// Removes a dynamic light
//============================================================================
void RemoveLight(int LightId)
{
	int i;
	glDisable((unsigned)(0x4000+Lights[LightId].LightEmitter));
	EmitterUsed[Lights[LightId].LightEmitter]=0;
	if (LightId<=NumLights)
	{
		for(i=LightId;i<(NumLights-1);i++)
		{
			Lights[i]=Lights[i+1];
		}
		NumLights--;
	}
}


//============================================================================
// Removes a lightsource by the attached object's ID
//============================================================================
void RemoveLightByObject(int objectId)
{
	int o;
	if (objectId==-1)
		return;
	for(o=0;o<NumLights;o++)
	{
		if (Lights[o].AttachedToObject==objectId)
		{
			RemoveLight(o);
			o--;
		}
	}
}




//============================================================================
// Dynamic light handler (OpenGL only!)
//============================================================================
void LightHandler(void)
{
int i;
float tempColor1[4];

	if (NumLights>0)
	for (i=0;i<NumLights; i++)
	{
		if (Lights[i].TimeToLive!=-1)
			Lights[i].TimeToLive-=1;
		if (Lights[i].TimeToLive==0)
		{
			RemoveLight(i);
			i--;
		}
		else
		{
			if (Lights[i].AttachedToObject!=-1)
			{
				Lights[i].xPos = Objects[Lights[i].AttachedToObject].xPos;
				Lights[i].zPos = Objects[Lights[i].AttachedToObject].zPos;
				Lights[i].Height = Objects[Lights[i].AttachedToObject].Height;
			}

//			originalVerts[i].x = -Lights[i].xPos;
//			originalVerts[i].y = Lights[i].Height;
//			originalVerts[i].z = -Lights[i].zPos;
		}
	}


	if (NumLights>0)
	for (i=0;i<NumLights; i++)
	{
			glLightfv((unsigned)(0x4000+Lights[i].LightEmitter), GL_AMBIENT, Lights[i].LightAmbient);
			glLightfv((unsigned)(0x4000+Lights[i].LightEmitter), GL_DIFFUSE, Lights[i].LightDiffuse);
			glLightfv((unsigned)(0x4000+Lights[i].LightEmitter), GL_SPECULAR, Lights[i].LightSpecular);
			glLightf((unsigned)(0x4000+Lights[i].LightEmitter), GL_SPOT_EXPONENT, 0.0f);
			glLightf((unsigned)(0x4000+Lights[i].LightEmitter), GL_CONSTANT_ATTENUATION, Lights[i].ConstantAttenuation);
			glLightf((unsigned)(0x4000+Lights[i].LightEmitter), GL_LINEAR_ATTENUATION, Lights[i].LinearAttenuation);
			glLightf((unsigned)(0x4000+Lights[i].LightEmitter), GL_QUADRATIC_ATTENUATION, Lights[i].QuadraticAttenuation);
//			tempColor1[0] = xfVerts[i].x;
//			tempColor1[1] = xfVerts[i].y;
//			tempColor1[2] = xfVerts[i].z;

			tempColor1[0] = -(Lights[i].xPos/2.0f);
			tempColor1[1] = (Lights[i].Height/2.0f);
			tempColor1[2] = -(Lights[i].zPos/2.0f);
			
			tempColor1[3] = 1.0f;
			glLightfv((unsigned)(0x4000+Lights[i].LightEmitter), GL_POSITION, tempColor1);
//			glEnable((unsigned)(0x4000+Lights[i].LightEmitter));

	}


}


#endif










//============================================================================
// Adds a particle emitter
// Textures:
//		6	for smoke
//		30	for flare
//		33	for sun/star
//		34	for moon
//		35	for laser beam
//============================================================================
int AddParticleEmitter(int ParentObject, int ParticleType, int ParticleLifetime, int ParticlesPerFrame, int ParticlesEveryNFrame, float Gravity, float xPos, float zPos, float Height, float Yaw, float Pitch, int EmitterLifetime, float StartSpeed, float EndSpeed, float SpeedVariation, float StartSize, float EndSize, float Spreading, int r, int g, int b, int Texture)
{
	ParticleEmitters[NumEmitters].Active=FXTRUE;
	ParticleEmitters[NumEmitters].AttachedToObject=ParentObject;
	ParticleEmitters[NumEmitters].Gravity=Gravity;
	ParticleEmitters[NumEmitters].Yaw=Yaw;
	ParticleEmitters[NumEmitters].Pitch=Pitch;
	ParticleEmitters[NumEmitters].xPos=xPos;
	ParticleEmitters[NumEmitters].zPos=zPos;
	ParticleEmitters[NumEmitters].Height=Height;
	ParticleEmitters[NumEmitters].ParticleType=ParticleType;
	ParticleEmitters[NumEmitters].TimeToLive=(float)(ParticleLifetime);
	ParticleEmitters[NumEmitters].ParticlesPerFrame=ParticlesPerFrame;
	ParticleEmitters[NumEmitters].ParticlesEveryNFrame=ParticlesEveryNFrame;
	ParticleEmitters[NumEmitters].FrameCounter=0;
	ParticleEmitters[NumEmitters].ParticleEmitterLifeTime=EmitterLifetime;
	ParticleEmitters[NumEmitters].StartSpeed=StartSpeed;
	ParticleEmitters[NumEmitters].EndSpeed=EndSpeed;
	ParticleEmitters[NumEmitters].StartSize=StartSize;
	ParticleEmitters[NumEmitters].EndSize=EndSize;
	ParticleEmitters[NumEmitters].Spreading=Spreading;
	ParticleEmitters[NumEmitters].SpeedVariation=SpeedVariation;
	ParticleEmitters[NumEmitters].AlwaysActive=FXFALSE;
	ParticleEmitters[NumEmitters].r=r;
	ParticleEmitters[NumEmitters].g=g;
	ParticleEmitters[NumEmitters].b=b;
	ParticleEmitters[NumEmitters].Sun=FXFALSE;
	ParticleEmitters[NumEmitters].ThreeD=FXFALSE;
	ParticleEmitters[NumEmitters].Texture=Texture;
	ParticleEmitters[NumEmitters].Slot=-1;
	NumEmitters++;
	ParticleEmitters[NumEmitters].Active=FXFALSE;
	return (NumEmitters-1);
}

//============================================================================
// Adds a single particle
//============================================================================
void AddParticle(int EmitterId)
{
	Particles[NumParticles].Gravity=ParticleEmitters[EmitterId].Gravity;
	Particles[NumParticles].Yaw=ParticleEmitters[EmitterId].Yaw+(float)(random(360)-180);

	if (ParticleEmitters[EmitterId].Spreading>0)
		Particles[NumParticles].Pitch=ParticleEmitters[EmitterId].Pitch+((float)random((int)ParticleEmitters[EmitterId].Spreading)-(ParticleEmitters[EmitterId].Spreading/2));
	else
		Particles[NumParticles].Pitch=ParticleEmitters[EmitterId].Pitch;

	Particles[NumParticles].xPos=ParticleEmitters[EmitterId].xPos;
	Particles[NumParticles].zPos=ParticleEmitters[EmitterId].zPos;
	Particles[NumParticles].Height=-ParticleEmitters[EmitterId].Height;
	Particles[NumParticles].TimeToLive=(float)(ParticleEmitters[EmitterId].TimeToLive);
	Particles[NumParticles].TotalTimeToLive=(float)(ParticleEmitters[EmitterId].TimeToLive);
	Particles[NumParticles].ParticleType=ParticleEmitters[EmitterId].ParticleType;
	
	if (ParticleEmitters[EmitterId].SpeedVariation>1)
		Particles[NumParticles].StartSpeed=ParticleEmitters[EmitterId].StartSpeed+((float)random((int)ParticleEmitters[EmitterId].SpeedVariation)-(ParticleEmitters[EmitterId].SpeedVariation/2));
	else
		Particles[NumParticles].StartSpeed=ParticleEmitters[EmitterId].StartSpeed;

	Particles[NumParticles].EndSpeed=ParticleEmitters[EmitterId].EndSpeed;
	Particles[NumParticles].StartSize=ParticleEmitters[EmitterId].StartSize;
	Particles[NumParticles].EndSize=ParticleEmitters[EmitterId].EndSize;
	Particles[NumParticles].r=ParticleEmitters[EmitterId].r;
	Particles[NumParticles].g=ParticleEmitters[EmitterId].g;
	Particles[NumParticles].b=ParticleEmitters[EmitterId].b;
	Particles[NumParticles].Sun=ParticleEmitters[EmitterId].Sun;
	Particles[NumParticles].ThreeD=ParticleEmitters[EmitterId].ThreeD;
	Particles[NumParticles].Texture=ParticleEmitters[EmitterId].Texture;
	Particles[NumParticles].OwningObject=ParticleEmitters[EmitterId].AttachedToObject;
	
	NumParticles++;
}


//============================================================================
// Removes a particle emitter
//============================================================================
void RemoveParticleEmitter(int EmitterId)
{
	int i;
	if (EmitterId<=NumEmitters)
	{
		ParticleEmitters[EmitterId].Active=FXFALSE;
		for(i=EmitterId;i<(NumEmitters-1);i++)
			{
			ParticleEmitters[i]=ParticleEmitters[i+1];
			}
		ParticleEmitters[NumEmitters].Active=FXFALSE;
		NumEmitters--;
		ParticleEmitters[NumEmitters].Active=FXFALSE;

	}
}


//============================================================================
// Removes a particle emitter by the attached object's ID
//============================================================================
void RemoveParticleEmitterByObject(int objectId)
{
	int o;
	if (objectId==-1)
		return;
	for(o=0;o<NumEmitters;o++)
	{
		if (ParticleEmitters[o].AttachedToObject==objectId)
		{
			RemoveParticleEmitter(o);
			o--;
		}
	}
}


//============================================================================
// Removes a particle
//============================================================================
void RemoveParticle(int ParticleId)
{
int i;

for(i=ParticleId;i<(NumParticles-1);i++)
	{
	Particles[i]=Particles[i+1];
	}
NumParticles--;
}





//============================================================================
// Main Particle Emitter Handler
//============================================================================
void ParticleEmitterHandler(void)
{
	int curobj,i;
	int o;

	// Rotate and Translate the particle slots for every object
	if (NumEmitters>0)
	for (i=0; i<NumObjects; i++)
	{
		tlSetMatrix( tlIdentity() );
		tlMultMatrix( tlXRotation( Objects[i].Pitch ) );
		tlMultMatrix( tlZRotation( Objects[i].Roll ) );
	    tlMultMatrix( tlYRotation( Objects[i].Yaw ) );
	    tlMultMatrix( tlTranslation( -Objects[i].xPos, -Objects[i].Height, -Objects[i].zPos ) );

		//now rotate and translate the object vertices in a temp. array			
		for (o=0; o<ParticleSlots[Objects[i].ObjectMesh].numslots; o++) {
			originalVerts[o] = ParticleSlots[Objects[i].ObjectMesh].Vertices[o].v;
		}
		tlTransformVertices( xfVerts, originalVerts, ParticleSlots[Objects[i].ObjectMesh].numslots );

		//Update Position of attached Particle Emitter(s)
		for (curobj=0; curobj<NumEmitters; curobj++)
			if (ParticleEmitters[curobj].AttachedToObject==i)
				if ((ParticleEmitters[curobj].Slot!=-1) && (ParticleEmitters[curobj].Slot<=ParticleSlots[Objects[i].ObjectMesh].numslots))
				{
					ParticleEmitters[curobj].xPos = xfVerts[ParticleEmitters[curobj].Slot].x;
					ParticleEmitters[curobj].Height = -xfVerts[ParticleEmitters[curobj].Slot].y;
					ParticleEmitters[curobj].zPos = xfVerts[ParticleEmitters[curobj].Slot].z;
				}
	}



	if (NumEmitters>0)
	for (curobj=0; curobj<NumEmitters; curobj++)
	{

	  // Don't add particles on standing tanks
	  if (!( (Objects[ParticleEmitters[curobj].AttachedToObject].ObjectMesh == OBJECTMESH_LIGHTTANK) &&
		  (Objects[ParticleEmitters[curobj].AttachedToObject].Speed <= 0.01f) ))
	  if (!( (Objects[ParticleEmitters[curobj].AttachedToObject].ObjectMesh == OBJECTMESH_MEDIUMTANK) &&
		  (Objects[ParticleEmitters[curobj].AttachedToObject].Speed <= 0.01f) ))
	  {

		if (ParticleEmitters[curobj].ParticleEmitterLifeTime!=-1.0f)
			ParticleEmitters[curobj].ParticleEmitterLifeTime--;
		if (ParticleEmitters[curobj].ParticleEmitterLifeTime==0.0f)
		{
			RemoveParticleEmitter(curobj);
			curobj--;
		}
		else
		{
			if ((ParticleEmitters[curobj].AttachedToObject!=-1) && (ParticleEmitters[curobj].Slot==-1))
			{
				if (Objects[ParticleEmitters[curobj].AttachedToObject].ObjectType!=OBJECTTYPE_BUILDING)
				{
					ParticleEmitters[curobj].xPos = -Objects[ParticleEmitters[curobj].AttachedToObject].xPos;
					ParticleEmitters[curobj].zPos = -Objects[ParticleEmitters[curobj].AttachedToObject].zPos;
					ParticleEmitters[curobj].Height = Objects[ParticleEmitters[curobj].AttachedToObject].Height;
					if (!ParticleEmitters[curobj].isAtCenter)
					{
						ParticleEmitters[curobj].Yaw = -Objects[ParticleEmitters[curobj].AttachedToObject].Yaw;
						ParticleEmitters[curobj].Pitch = -Objects[ParticleEmitters[curobj].AttachedToObject].Pitch;
					}
				}
				else 
				if (!ParticleEmitters[curobj].isAtCenter)
				{
					if (Objects[ParticleEmitters[curobj].AttachedToObject].ObjectMesh==OBJECTMESH_POWERPLANT)
					{
						ParticleEmitters[curobj].xPos = -Objects[ParticleEmitters[curobj].AttachedToObject].xPos-25.0f;
						ParticleEmitters[curobj].zPos = -Objects[ParticleEmitters[curobj].AttachedToObject].zPos-25.0f;
						ParticleEmitters[curobj].Height = Objects[ParticleEmitters[curobj].AttachedToObject].Height-30.0f;
						ParticleEmitters[curobj].Yaw = -Objects[ParticleEmitters[curobj].AttachedToObject].Yaw;
						ParticleEmitters[curobj].Pitch = 90.0f;
					}
					else if (Objects[ParticleEmitters[curobj].AttachedToObject].ObjectMesh==OBJECTMESH_MINE)
					{
						ParticleEmitters[curobj].xPos = -Objects[ParticleEmitters[curobj].AttachedToObject].xPos-28.0f;
						ParticleEmitters[curobj].zPos = -Objects[ParticleEmitters[curobj].AttachedToObject].zPos-25.0f;
						ParticleEmitters[curobj].Height = Objects[ParticleEmitters[curobj].AttachedToObject].Height-25.0f;
						ParticleEmitters[curobj].Yaw = -Objects[ParticleEmitters[curobj].AttachedToObject].Yaw;
						ParticleEmitters[curobj].Pitch = 90.0f;
					}
				}
			}

			if ((ParticleEmitters[curobj].isActive) && (ParticleEmitters[curobj].Active))
			{
				if (ParticleEmitters[curobj].ParticlesEveryNFrame==-1)
				{
					for (i=0; i<ParticleEmitters[curobj].ParticlesPerFrame; i++)
					{
						AddParticle(curobj);
					}
				}
				else
				{
					ParticleEmitters[curobj].FrameCounter++;
					if (ParticleEmitters[curobj].FrameCounter >= ParticleEmitters[curobj].ParticlesEveryNFrame)
					{
						AddParticle(curobj);
 						ParticleEmitters[curobj].FrameCounter=0;
					}
				}
			}
		}
	  }
	}
}

//============================================================================
// Handler for the particles themselves
//============================================================================
void ParticleHandler(void)
{
	float xmovement, ymovement, zmovement;
	int curobj;

	if (NumParticles>0)
	for (curobj=0; curobj<NumParticles; curobj++)
	{
		if (Particles[curobj].TimeToLive!=-1.0f)
			Particles[curobj].TimeToLive-=1.0f;
		if (Particles[curobj].TimeToLive==0.0f)
		{
			RemoveParticle(curobj);
			curobj--;
		}
		else
		{

		//Change Speed over time
		if (Particles[curobj].EndSpeed>Particles[curobj].StartSpeed)
			Particles[curobj].Speed=(float)(Particles[curobj].EndSpeed-(Particles[curobj].TimeToLive/Particles[curobj].TotalTimeToLive)*(Particles[curobj].EndSpeed-Particles[curobj].StartSpeed));
		else
			Particles[curobj].Speed=(float)(Particles[curobj].StartSpeed+(Particles[curobj].TimeToLive/Particles[curobj].TotalTimeToLive)*(Particles[curobj].EndSpeed-Particles[curobj].StartSpeed));

		//Change Size over time
		if (Particles[curobj].EndSize>Particles[curobj].StartSize)
			Particles[curobj].Size=(float)(Particles[curobj].EndSize-(Particles[curobj].TimeToLive/Particles[curobj].TotalTimeToLive)*(Particles[curobj].EndSize-Particles[curobj].StartSize));
		else
			Particles[curobj].Size=(float)(Particles[curobj].StartSize+(Particles[curobj].TimeToLive/Particles[curobj].TotalTimeToLive)*(Particles[curobj].EndSize-Particles[curobj].StartSize));


		Particles[curobj].Yaw = FixAngle(Particles[curobj].Yaw);
		Particles[curobj].Pitch = FixAngle(Particles[curobj].Pitch);

		//Calculate gravity, by simply "pulling" the particles pitch down to -90 degrees
		if (Particles[curobj].Gravity!=0)
		{

			if ((Particles[curobj].Pitch>0.0f) && (Particles[curobj].Pitch<90.0f) ||
				(Particles[curobj].Pitch>270.0f) && (Particles[curobj].Pitch<360.0f))
				Particles[curobj].Pitch-=Particles[curobj].Gravity*2;
			else 
			if ((Particles[curobj].Pitch>90.0f) && (Particles[curobj].Pitch<180.0f) ||
				(Particles[curobj].Pitch>180.0f) && (Particles[curobj].Pitch<270.0f))
				Particles[curobj].Pitch+=Particles[curobj].Gravity*2;

			Particles[curobj].Pitch = FixAngle(Particles[curobj].Pitch);

		}


		//Now move the particle
/*		xmovement=(float)(sin(Particles[curobj].Yaw * DEGREE)*cos(Particles[curobj].Pitch * DEGREE) * (Particles[curobj].Speed/100.0f) * scalefactor * g_speedfactor);
		zmovement=(float)(cos(Particles[curobj].Yaw * DEGREE)*cos(Particles[curobj].Pitch * DEGREE) * (Particles[curobj].Speed/100.0f) * scalefactor * g_speedfactor);
		ymovement=(float)(sin(Particles[curobj].Pitch * DEGREE)* (Particles[curobj].Speed/100.0f) * scalefactor * g_speedfactor);
*/

		xmovement=(float)(Sin[(int)(Particles[curobj].Yaw * Deg)]*Cos[(int)(Particles[curobj].Pitch * Deg)] * (Particles[curobj].Speed/100.0f) * scalefactor * g_speedfactor);
		zmovement=(float)(Cos[(int)(Particles[curobj].Yaw * Deg)]*Cos[(int)(Particles[curobj].Pitch * Deg)] * (Particles[curobj].Speed/100.0f) * scalefactor * g_speedfactor);
		ymovement=(float)(Sin[(int)(Particles[curobj].Pitch * Deg)]* (Particles[curobj].Speed/100.0f) * scalefactor * g_speedfactor);

		Particles[curobj].Height+=ymovement;
		Particles[curobj].xPos-=xmovement;
		Particles[curobj].zPos-=zmovement;
		
		
		}
	}
}




//============================================================================
// Limits output of certain particle emitters to keep up r_speed
//============================================================================
void LimitParticles(void)
{
int i;

	for(i=0;i<NumEmitters;i++)
	{
		if (NumParticles>AbsoluteMaxParticles)
		{
			ParticleEmitters[i].isActive=FXFALSE;
		}
		else
		{
			ParticleEmitters[i].isActive=FXFALSE;
		}

	}

	if (NumParticles>AbsoluteMaxParticles)
		return;

	if (NumParticles<MaxParticles)
	{
		for(i=0;i<NumEmitters;i++)
		{
			ParticleEmitters[i].isActive=FXTRUE;
		}
		return;
	}
	CurrentActiveEmitter++;
	if (CurrentActiveEmitter>=NumEmitters)
		CurrentActiveEmitter=0;

	for(i=0;i<NumEmitters;i++)
	{
		if (!ParticleEmitters[i].AlwaysActive)
		ParticleEmitters[i].isActive=FXFALSE;
	}
	ParticleEmitters[CurrentActiveEmitter].isActive=FXTRUE;
}





//============================================================================
// Easy way to add a particle explosion. It's NOT limited to MaxNumParticles!
//============================================================================

//Missile explosion
void Explosion1(float xPos, float zPos, float Height)
{
/*				AddLight( -1, 10, 
							2.8f, 0.3f, 0.3f,
							2.8f, 0.3f, 0.3f,
							2.8f, 0.3f, 0.3f,
							1.0f, 0.0f, 0.0001f,
							xPos, Height, zPos);
*/
				ParticleEmitters[AddParticleEmitter(-1, 0, 200, 5, -1, 0.0f,
				-xPos, -zPos, Height,
				0.0f, +90.0f,
				2, 0.2f, 0.2f, 0.0f, 500.0f, 1000.0f, 360.0f,
				100, 100, 100, 6)].AlwaysActive=FXTRUE;//Smoke

				ParticleEmitters[AddParticleEmitter(-1, 1, 20, 5, -1, 0.0f,
				-xPos, -zPos, Height, //Position
				0.0f, +90.0f,		//Yaw, Pitch
				2, 1.0f, 0.0f, 1.0f, 500.0f, 1000.0f, 360.0f,
				255, 0, 0, 6)].AlwaysActive=FXTRUE;//Red

				ParticleEmitters[AddParticleEmitter(-1, 1, 70, 15, -1, 1.0f,
				-xPos, -zPos, Height, //Position
				0.0f, +90.0f,		//Yaw, Pitch
				3, 1.0f, 2.0f, 1.0f, 100.0f, 200.0f, 270.0f,
				255, 0, 0, 30)].AlwaysActive=FXTRUE;//Red

				ParticleEmitters[AddParticleEmitter(-1, 1, 70, 30, -1, 1.0f,
				-xPos, -zPos, Height, //Position
				0.0f, +90.0f,		//Yaw, Pitch
				2, 1.0f, 2.0f, 1.0f, 100.0f, 200.0f, 270.0f,
				255, 255, 0, 30)].AlwaysActive=FXTRUE;//Yellow
}


//Laser impact
void Explosion2(float xPos, float zPos, float Height)
{
/*				AddLight( -1, 10, 
							1.8f, 0.3f, 0.3f,
							1.8f, 0.3f, 0.3f,
							1.8f, 0.3f, 0.3f,
							1.0f, 0.0f, 0.0001f,
							xPos, Height, zPos);
*/
				ParticleEmitters[AddParticleEmitter(-1, 1, 5, 3, -1, 0.0f,
				-xPos, -zPos, Height, //Position
				0.0f, +90.0f,		//Yaw, Pitch
				2, 0.2f, 0.2f, 0.0f, 300.0f, 500.0f, 360.0f,
				250, 0, 0, 30)].AlwaysActive=FXTRUE;//Red

}

//Ship explosion
void Explosion3(float xPos, float zPos, float Height)
{
int newemitter;
/*				AddLight( -1, 30, 
							2.8f, 0.3f, 0.3f,
							2.8f, 0.3f, 0.3f,
							2.8f, 0.3f, 0.3f,
							1.0f, 0.0f, 0.00001f,
							xPos, Height, zPos);
*/
				newemitter = AddParticleEmitter(-1, 0, 200, 5, -1, 0.0f,
				-xPos, -zPos, Height, //Position
				0.0f, +90.0f,		//Yaw, Pitch
				2, 0.2f, 0.2f, 0.0f, 500.0f, 1000.0f, 360.0f,
				100, 100, 100, 6);//Smoke

				ParticleEmitters[newemitter].AlwaysActive=FXTRUE;

				newemitter = AddParticleEmitter(-1, 1, 70, 5, -1, 0.0f,
				-xPos, -zPos, Height, //Position
				0.0f, +90.0f,		//Yaw, Pitch
				2, 1.0f, 0.0f, 1.0f, 500.0f, 1000.0f, 360.0f,
				255, 0, 0, 6);//Red

				ParticleEmitters[newemitter].AlwaysActive=FXTRUE;

				newemitter = AddParticleEmitter(-1, 1, 50, 40, -1, 1.0f,
				-xPos, -zPos, Height, //Position
				0.0f, +90.0f,		//Yaw, Pitch
				3, 1.0f, 2.0f, 1.0f, 100.0f, 200.0f, 270.0f,
				255, 0, 0, 30);//Red

				ParticleEmitters[newemitter].AlwaysActive=FXTRUE;

				newemitter = AddParticleEmitter(-1, 1, 50, 60, -1, 1.0f,
				-xPos, -zPos, Height, //Position
				0.0f, +90.0f,		//Yaw, Pitch
				2, 1.0f, 2.0f, 1.0f, 100.0f, 200.0f, 270.0f,
				255, 255, 0, 33);//Yellow

				ParticleEmitters[newemitter].AlwaysActive=FXTRUE;

}

//Building explosion
void Explosion4(float xPos, float zPos, float Height)
{
int newemitter;

/*				AddLight( -1, 30, 
							2.8f, 0.3f, 0.3f,
							2.8f, 0.3f, 0.3f,
							2.8f, 0.3f, 0.3f,
							1.0f, 0.0f, 0.00001f,
							xPos, Height, zPos);
*/
				newemitter = AddParticleEmitter(-1, 0, 200, 5, -1, 0.0f,
				-xPos, -zPos, Height,
				0.0f, +90.0f,
				2, 0.2f, 0.2f, 0.0f, 500.0f, 1000.0f, 360.0f,
				100, 100, 100, 6);//Smoke
				ParticleEmitters[newemitter].AlwaysActive=FXTRUE;

				newemitter = AddParticleEmitter(-1, 1, 30, 5, -1, 0.0f,
				-xPos, -zPos, Height, //Position
				0.0f, +90.0f,		//Yaw, Pitch
				2, 1.0f, 0.0f, 1.0f, 500.0f, 1000.0f, 360.0f,
				255, 0, 0, 6);//Red
				ParticleEmitters[newemitter].AlwaysActive=FXTRUE;

				newemitter = AddParticleEmitter(-1, 1, 70, 50, -1, 1.0f,
				-xPos, -zPos, Height, //Position
				0.0f, +90.0f,		//Yaw, Pitch
				3, 1.0f, 2.0f, 1.0f, 100.0f, 200.0f, 270.0f,
				255, 0, 0, 30);//Red
				ParticleEmitters[newemitter].AlwaysActive=FXTRUE;

				newemitter = AddParticleEmitter(-1, 1, 70, 80, -1, 1.0f,
				-xPos, -zPos, Height, //Position
				0.0f, +90.0f,		//Yaw, Pitch
				2, 1.0f, 2.0f, 1.0f, 100.0f, 200.0f, 270.0f,
				255, 255, 0, 30);//Yellow
				ParticleEmitters[newemitter].AlwaysActive=FXTRUE;
}





//============================================================================
// Particle Renderer
//============================================================================
//The renderer treats every particle as a single vertex for speeding up transformations.
//When it comes to actually drawing stuff on the screen, we'll place a simple
//rectangle with the vertex as centerpoint.
void RenderParticles(FxBool RenderSunAndMoons)
{
int i;
int LastUsedTexture=-1;
float particleIntensity;
float PSunRIntensity, PSunGIntensity, PSunBIntensity;
float vxdist, vydist, vzdist;

if (NumParticles==0)
	return;

#ifdef USE_GLIDE
#else
        tlSetMatrix( tlIdentity() );

        tlMultMatrix( tlTranslation( CamXPos, CamHeight, CamZPos ) );
        
        tlMultMatrix( tlYRotation( CamYaw ) );
		tlMultMatrix( tlXRotation( CamPitch ) );
        tlMultMatrix( tlZRotation( CamRoll ) );
#endif

//============================================================================
// Get rid of vertices that are far away
//============================================================================
	visverts=0;
	for (i=0; i<NumParticles; i++) {


	  if (!RenderSunAndMoons)
	  {
		
		if (Particles[i].xPos > CamXPos)
			vxdist = (Particles[i].xPos + CamXPos);
		else
			vxdist = (CamXPos + Particles[i].xPos);

		if (Particles[i].Height > (+CamHeight))
			vydist = (Particles[i].Height + CamHeight);
		else
			vydist = (CamHeight + Particles[i].Height);
           
		if (Particles[i].zPos > CamZPos)
			vzdist = (Particles[i].zPos + CamZPos);
		else
			vzdist = (CamZPos + Particles[i].zPos);

		if (vxdist<0)
			vxdist=(-1.0f)*vxdist;
		if (vydist<0)
			vydist=(-1.0f)*vydist;
		if (vzdist<0)
			vzdist=(-1.0f)*vzdist;

				v2v[visverts]=-1;
				if( ( (vxdist<maxvdist) && (vydist<maxvdist) && (vzdist<maxvdist) &&
					(!Particles[i].Sun) && (!Particles[i].ThreeD) && !((Particles[i].OwningObject==Player[localplayer].ControlledObject) && (Particles[i].Texture>=ICON_TARGET)) )
					|| (Particles[i].Texture==ICON_ENEMYBASE)
					|| (Particles[i].Texture==ICON_FRIENDLYBASE) )
				{
					VisVertices[visverts].v.x = Particles[i].xPos;
					VisVertices[visverts].v.z = Particles[i].zPos;
					VisVertices[visverts].v.y = Particles[i].Height;
					Particles[i].isVisible=FXTRUE;
					v2v[visverts]=i;
					visverts++;
				} else
					Particles[i].isVisible=FXFALSE;

	  }
	  else		//Do not render normal particles, only suns and moons
	  {
		v2v[visverts]=-1;
		if ((Particles[i].Sun)) //&& (!Particles[i].ThreeD))
		{
			VisVertices[visverts].v.x = Particles[i].xPos;
			VisVertices[visverts].v.z = Particles[i].zPos;
			VisVertices[visverts].v.y = Particles[i].Height;
			Particles[i].isVisible=FXTRUE;
			v2v[visverts]=i;
			visverts++;
		}
		else	//Do not render all the others
			Particles[i].isVisible=FXFALSE;
	  }

	
	}


//============================================================================
// Transform the remaining vertices
//============================================================================

	for (i=0; i<visverts; i++) 
	{
		originalVerts[i] = VisVertices[i].v;
		originalVerts[i].w = 0;
	}
	tlTransformVertices( xfVerts, originalVerts, visverts );

	for (i=0; i<visverts; i++) 
	{
		Particles[v2v[i]].isVisible=FXTRUE;

		if (RenderSunAndMoons)
		{
			if ( (xfVerts[i].z<=0) || (!Particles[v2v[i]].Sun) || (Particles[v2v[i]].ThreeD) )
			  Particles[v2v[i]].isVisible=FXFALSE;
		}
		else
		{
			if ( (xfVerts[i].z<=0) || (xfVerts[i].z>=maxvdist/2) || (Particles[v2v[i]].ThreeD))
			  Particles[v2v[i]].isVisible=FXFALSE;
		}

		
		
		if (Particles[v2v[i]].isVisible)
		{
			if ((!Particles[v2v[i]].Sun) && (Particles[v2v[i]].ParticleType != 2))
				Particles[v2v[i]].ScreenSize=xfVerts[i].z/Particles[v2v[i]].Size;
			else 
				Particles[v2v[i]].ScreenSize=1000.0f/Particles[v2v[i]].Size;
			
		}
	}

//============================================================================
// Projection of the surviving vertices
//============================================================================

	tlProjectVertices( prjVerts, xfVerts, visverts );
	curface=0;
	for (i=0; i<visverts; i++)
	if (Particles[v2v[i]].isVisible)
	{

		//Face 1 of 2
        PFaces[curface].vtxA.x = prjVerts[i].x-0.01/Particles[v2v[i]].ScreenSize;
        PFaces[curface].vtxA.y = prjVerts[i].y-0.01/Particles[v2v[i]].ScreenSize;
        PFaces[curface].vtxA.oow = 1.0f / prjVerts[i].w;


        PFaces[curface].vtxB.x = prjVerts[i].x+0.01/Particles[v2v[i]].ScreenSize;
        PFaces[curface].vtxB.y = prjVerts[i].y-0.01/Particles[v2v[i]].ScreenSize; 
        PFaces[curface].vtxB.oow = 1.0f / prjVerts[i].w;

        
        PFaces[curface].vtxC.x = prjVerts[i].x-0.01/Particles[v2v[i]].ScreenSize;
        PFaces[curface].vtxC.y = prjVerts[i].y+0.01/Particles[v2v[i]].ScreenSize;
        PFaces[curface].vtxC.oow = 1.0f / prjVerts[i].w;
		
        PFaces[curface].vtxA.tmuvtx[0].sow = 0.0f * 255.0f * PFaces[curface].vtxA.oow;
        PFaces[curface].vtxA.tmuvtx[0].tow = 0.0f * 255.0f * PFaces[curface].vtxA.oow;

        PFaces[curface].vtxB.tmuvtx[0].sow = 0.0f * 255.0f * PFaces[curface].vtxB.oow;
        PFaces[curface].vtxB.tmuvtx[0].tow = 1.0f * 255.0f * PFaces[curface].vtxB.oow;

        PFaces[curface].vtxC.tmuvtx[0].sow = 1.0f * 255.0f * PFaces[curface].vtxC.oow;
        PFaces[curface].vtxC.tmuvtx[0].tow = 0.0f * 255.0f * PFaces[curface].vtxC.oow;

		curface++;
        

		//Face 2 of 2        
		PFaces[curface].vtxA.x = prjVerts[i].x+0.01/Particles[v2v[i]].ScreenSize;
        PFaces[curface].vtxA.y = prjVerts[i].y-0.01/Particles[v2v[i]].ScreenSize;
        PFaces[curface].vtxA.oow = 1.0f / prjVerts[i].w;


        PFaces[curface].vtxB.x = prjVerts[i].x+0.01/Particles[v2v[i]].ScreenSize;
        PFaces[curface].vtxB.y = prjVerts[i].y+0.01/Particles[v2v[i]].ScreenSize; 
        PFaces[curface].vtxB.oow = 1.0f / prjVerts[i].w;

        
        PFaces[curface].vtxC.x = prjVerts[i].x-0.01/Particles[v2v[i]].ScreenSize;
        PFaces[curface].vtxC.y = prjVerts[i].y+0.01/Particles[v2v[i]].ScreenSize;
        PFaces[curface].vtxC.oow = 1.0f / prjVerts[i].w;

		PFaces[curface].vtxA.tmuvtx[0].sow = 0.0f * 255.0f * PFaces[curface].vtxA.oow;
        PFaces[curface].vtxA.tmuvtx[0].tow = 1.0f * 255.0f * PFaces[curface].vtxA.oow;

        PFaces[curface].vtxB.tmuvtx[0].sow = 1.0f * 255.0f * PFaces[curface].vtxB.oow;
        PFaces[curface].vtxB.tmuvtx[0].tow = 1.0f * 255.0f * PFaces[curface].vtxB.oow;

        PFaces[curface].vtxC.tmuvtx[0].sow = 1.0f * 255.0f * PFaces[curface].vtxC.oow;
        PFaces[curface].vtxC.tmuvtx[0].tow = 0.0f * 255.0f * PFaces[curface].vtxC.oow;

		curface++;

	}
	visface=curface-1;


	PSunRIntensity=SunRIntensity/256;
	PSunGIntensity=SunGIntensity/256;
	PSunBIntensity=SunBIntensity/256;
	if (PSunRIntensity<0.1f)
		PSunRIntensity=0.1f;
	if (PSunGIntensity<0.1f)
		PSunGIntensity=0.1f;
	if (PSunBIntensity<0.1f)
		PSunBIntensity=0.1f;
	


		glDisable(GL_DEPTH_TEST); 
		glEnable(GL_BLEND); 
		glDisable(GL_LIGHTING); 

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//my constants are calc'ed for 800x600, so we may need a compensation value for other resolutions
//		float ScreenFactor = (((float)winHeight/600.0f) + ((float)winWidth/800.0f));
//		gluOrtho2D( 1.0f-(0.11f*ScreenFactor), 0.11f*ScreenFactor, 0.21f, 0.79f ); // These coordinates are a hack to accomodate to the 60° FOV I set for OpenGL


		gluOrtho2D( 0.89, 0.11, 0.21, 0.79 ); // These coordinates are a hack to accomodate to the 60° FOV I set for OpenGL


		if (GLTextures)
			glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);					// Switch on the darned blending
		if (GLTextures)
			glEnable(GL_TEXTURE_2D);
		glEnable(GL_FOG);

		
	curorder=0;
	for (i=0; i<visverts; i++)
	if (Particles[v2v[i]].isVisible)
	{
		if (GLTextureHandle[Particles[v2v[i]].Texture] != (unsigned)LastUsedTexture)
		{
			glBindTexture(GL_TEXTURE_2D, GLTextureHandle[Particles[v2v[i]].Texture]);
			LastUsedTexture = GLTextureHandle[Particles[v2v[i]].Texture];
		}

		if (Particles[v2v[i]].Texture==6) // Smoke darkens things
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR );
		else
			glBlendFunc(GL_SRC_ALPHA,GL_ONE );

		if (Particles[v2v[i]].Texture != 33)
			if (RenderSunAndMoons)
				particleIntensity=(float)(7200.0f-(float)fixtime)/7200.0f * 3.0f-2.0f;
			else
				particleIntensity=(float)((float)Particles[v2v[i]].TimeToLive/(float)Particles[v2v[i]].TotalTimeToLive*3.0f);
		else
			particleIntensity=(float)((float)Particles[v2v[i]].TimeToLive/(float)Particles[v2v[i]].TotalTimeToLive*3.0f);


		glBegin(GL_TRIANGLES);
			glTexCoord2f(0.0, 0.0);
			if (Particles[v2v[i]].ParticleType==0)
			glColor4f(	(float)(Particles[v2v[i]].r*particleIntensity*PSunRIntensity/256),
						(float)(Particles[v2v[i]].g*particleIntensity*PSunGIntensity/256),
						(float)(Particles[v2v[i]].b*particleIntensity*PSunBIntensity/256),
						0.5f);
			else
			glColor4f(	(float)(Particles[v2v[i]].r*particleIntensity/256),
						(float)(Particles[v2v[i]].g*particleIntensity/256),
						(float)(Particles[v2v[i]].b*particleIntensity/256),
						0.8f);
			glVertex2f(PFaces[curorder].vtxA.x, PFaces[curorder].vtxA.y);

			glTexCoord2f(0.0, 1.0);
			if (Particles[v2v[i]].ParticleType==0)
			glColor4f(	(float)(Particles[v2v[i]].r*particleIntensity*PSunRIntensity/256),
						(float)(Particles[v2v[i]].g*particleIntensity*PSunGIntensity/256),
						(float)(Particles[v2v[i]].b*particleIntensity*PSunBIntensity/256),
						0.5f);
			else
			glColor4f(	(float)(Particles[v2v[i]].r*particleIntensity/256),
						(float)(Particles[v2v[i]].g*particleIntensity/256),
						(float)(Particles[v2v[i]].b*particleIntensity/256),
						0.8f);
			glVertex2f(PFaces[curorder].vtxB.x, PFaces[curorder].vtxB.y);

			glTexCoord2f(1.0, 0.0);
			if (Particles[v2v[i]].ParticleType==0)
			glColor4f(	(float)(Particles[v2v[i]].r*particleIntensity*PSunRIntensity/256),
						(float)(Particles[v2v[i]].g*particleIntensity*PSunGIntensity/256),
						(float)(Particles[v2v[i]].b*particleIntensity*PSunBIntensity/256),
						0.5f);
			else
			glColor4f(	(float)(Particles[v2v[i]].r*particleIntensity/256),
						(float)(Particles[v2v[i]].g*particleIntensity/256),
						(float)(Particles[v2v[i]].b*particleIntensity/256),
						0.8f);
			glVertex2f(PFaces[curorder].vtxC.x, PFaces[curorder].vtxC.y);
		glEnd();

		curorder++;

		glBegin(GL_TRIANGLES);
			glTexCoord2f(0.0, 1.0);
			if (Particles[v2v[i]].ParticleType==0)
			glColor4f(	(float)(Particles[v2v[i]].r*particleIntensity*PSunRIntensity/256),
						(float)(Particles[v2v[i]].g*particleIntensity*PSunGIntensity/256),
						(float)(Particles[v2v[i]].b*particleIntensity*PSunBIntensity/256),
						0.5f);
			else
			glColor4f(	(float)(Particles[v2v[i]].r*particleIntensity/256),
						(float)(Particles[v2v[i]].g*particleIntensity/256),
						(float)(Particles[v2v[i]].b*particleIntensity/256),
						0.8f);
			glVertex2f(PFaces[curorder].vtxA.x, PFaces[curorder].vtxA.y);

			glTexCoord2f(1.0, 1.0);
			if (Particles[v2v[i]].ParticleType==0)
			glColor4f(	(float)(Particles[v2v[i]].r*particleIntensity*PSunRIntensity/256),
						(float)(Particles[v2v[i]].g*particleIntensity*PSunGIntensity/256),
						(float)(Particles[v2v[i]].b*particleIntensity*PSunBIntensity/256),
						0.5f);
			else
			glColor4f(	(float)(Particles[v2v[i]].r*particleIntensity/256),
						(float)(Particles[v2v[i]].g*particleIntensity/256),
						(float)(Particles[v2v[i]].b*particleIntensity/256),
						0.8f);
			glVertex2f(PFaces[curorder].vtxB.x, PFaces[curorder].vtxB.y);

			glTexCoord2f(1.0, 0.0);
			if (Particles[v2v[i]].ParticleType==0)
			glColor4f(	(float)(Particles[v2v[i]].r*particleIntensity*PSunRIntensity/256),
						(float)(Particles[v2v[i]].g*particleIntensity*PSunGIntensity/256),
						(float)(Particles[v2v[i]].b*particleIntensity*PSunBIntensity/256),
						0.5f);
			else
			glColor4f(	(float)(Particles[v2v[i]].r*particleIntensity/256),
						(float)(Particles[v2v[i]].g*particleIntensity/256),
						(float)(Particles[v2v[i]].b*particleIntensity/256),
						0.8f);
			glVertex2f(PFaces[curorder].vtxC.x, PFaces[curorder].vtxC.y);
		glEnd();

		curorder++;

	}


		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		glDisable(GL_BLEND);
		glDisable(GL_FOG);




}




//============================================================================
// Removes Trees in a defined square area. Used to make place for buildings
//============================================================================
void RemoveTrees(float x1, float z1, float x2, float z2)
{
int i;
float tempx, tempz;

if (x1>x2)
{
	tempx=x2;
	x2=x1;
	x1=tempx;
}
if (z1>z2)
{
	tempz=z2;
	z2=z1;
	z1=tempz;
}

  for (i=0; i<NumTrees; i++)
  {
	if (Trees[i].Size>0)
		if ((Trees[i].xPos>x1) && (Trees[i].xPos<x2))
			if ((Trees[i].zPos>z1) && (Trees[i].zPos<z2))
				Trees[i].Size=0;
  }
}

//============================================================================
// Add Trees. Actually uses a sky texture for the fractal pattern..
// This function will have two modes: x) Random and x) Fractal.
// Random is good for having an "alive" map, but trees are too scattered
// Fractal produces realistic "forests" - i.e. Tree populations instead of separate Trees.
// SUGGESTION: Add a few Forests with fractal, then fill with scattered trees via random.
//============================================================================
void AddTrees(int mode)
{
int i;
float minx, maxx;
float minz, maxz;

minx = (float)(-MapDimension+2.0f)*scalefactor;
maxx = (float)(MapDimension-3.0f)*scalefactor;
minz = (float)(-MapDimension+2.0f)*scalefactor;
maxz = (float)(MapDimension-3.0f)*scalefactor;


if (mode==0)
{
  for (i=0; i<500; i++)
  {
	Trees[NumTrees].xPos = (float) (rand()%(int)(maxx-minx))+minx;
	Trees[NumTrees].zPos = (float) (rand()%(int)(maxz-minz))+minz;
	Trees[NumTrees].Size = 200.0f; // + (random(200)-100);
	Trees[NumTrees].Height = (float) (TerrainHeight(&Trees[NumTrees].xPos, &Trees[NumTrees].Height, &Trees[NumTrees].zPos) - 1.0f);
	Trees[NumTrees].Texture = 49; //This should be the texture num of the tree texture
	NumTrees++;
  }
}
else
{
	int NumForests = (int)(rand()%30);

	for (int CurrentForest=0; CurrentForest<NumForests; CurrentForest++)
	{

		float ForestX = (float) ((int)(rand()%(int)(maxx-minx)))+minx;
		float ForestZ = (float) ((int)(rand()%(int)(maxz-minz)))+minz;
		int ForestTrees = (int)((int)(rand()%200));
		float ForestSpread = (float)((int)(rand()%500));
		
		for (int CurrentTree=0; CurrentTree<ForestTrees; CurrentTree++)
		{
			Trees[NumTrees].xPos = ForestX + (float)((int)(rand()%(int)ForestSpread)) - (ForestSpread/2.0f);
			Trees[NumTrees].zPos = ForestZ + (float)((int)(rand()%(int)ForestSpread)) - (ForestSpread/2.0f);
			Trees[NumTrees].Size = 200.0f; // + (random(200)-100);
			Trees[NumTrees].Height = (float) (TerrainHeight(&Trees[NumTrees].xPos, &Trees[NumTrees].Height, &Trees[NumTrees].zPos) + 3.0f);
			Trees[NumTrees].Texture = 49; //This should be the texture num of the tree texture
			NumTrees++;
		}

	}
}





}


//============================================================================
// Render the Trees (just some stupid Quads)
//============================================================================
void RenderTrees(void)
{
int i = 0;
float PSunRIntensity, PSunGIntensity, PSunBIntensity;
float vxdist, vydist, vzdist;
//int ia, ib;

if ((NumTrees==0) || (NumTrees>100000))
	return;


	PSunRIntensity=SunRIntensity/256;
	PSunGIntensity=SunGIntensity/256;
	PSunBIntensity=SunBIntensity/256;
	if (PSunRIntensity<0.1f)
		PSunRIntensity=0.1f;
	if (PSunGIntensity<0.1f)
		PSunGIntensity=0.1f;
	if (PSunBIntensity<0.1f)
		PSunBIntensity=0.1f;

		glEnable(GL_BLEND); 
		glDisable(GL_LIGHTING); 
		if (GLTextures)
			glEnable(GL_TEXTURE_2D);
		glEnable(GL_FOG);

		glDepthFunc(GL_LEQUAL);


		if (!((CamYaw>-10000.0f) && (CamYaw<10000.0f)))
			CamYaw=0;
		tempfloat = -CamYaw+90;
		while (tempfloat < 0.0f) {
			tempfloat += 360.0f;
		}
		while (tempfloat >= 360.0f) {
 			tempfloat -= 360.0f;
		}
		TempYaw=tempfloat;

		if (!((CamPitch>-10000.0f) && (CamPitch<10000.0f)))
			CamPitch=0;
		tempfloat = -CamPitch+90;
		while (tempfloat < 0.0f) {
			tempfloat += 360.0f;
		}
		while (tempfloat >= 360.0f) {
			tempfloat -= 360.0f;
		}
		TempPitch=tempfloat;

		
		glBlendFunc(GL_ONE_MINUS_SRC_ALPHA,GL_SRC_ALPHA);
		glEnable(GL_STENCIL_TEST);

	for (i=0; i<NumTrees; i++)
	{


		if (Trees[i].xPos > -CamXPos)
			vxdist = (Trees[i].xPos - CamXPos);
		else
			vxdist = (-CamXPos + Trees[i].xPos);

		if (Trees[i].Height > -CamHeight)
			vydist = (Trees[i].Height - CamHeight);
		else
			vydist = (-CamHeight + Trees[i].Height);
           
		if (Trees[i].zPos > -CamZPos)
			vzdist = (Trees[i].zPos - CamZPos);
		else
			vzdist = (-CamZPos + Trees[i].zPos);

		if (vxdist<0)
			vxdist=(-1.0f)*vxdist;
		if (vydist<0)
			vydist=(-1.0f)*vydist;
		if (vzdist<0)
			vzdist=(-1.0f)*vzdist;

			
	  if ((vxdist<maxvdist/4.0f) && (vydist<maxvdist/4.0f) && (vzdist<maxvdist/4.0f))
	  if (Trees[i].Size>0)
	  {

		glColor4f(	(float)(PSunRIntensity),(float)(PSunGIntensity),(float)(PSunBIntensity),1.0f);


		glEnable(GL_DEPTH_TEST);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		glStencilOp(GL_ZERO, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xffffffff);



		glBindTexture(GL_TEXTURE_2D, GLTextureHandle[Trees[i].Texture+1]);
		glBegin(GL_QUADS);
			glPolygonOffset(10.0f, 1.0f);
			glTexCoord2f(0.0, 0.0);
			glVertex3f( -Trees[i].xPos-(Trees[i].Size/100),
						Trees[i].Height-(Trees[i].Size/100), 
						-Trees[i].zPos-(Trees[i].Size/100));
		
			glTexCoord2f(0.0, 1.0);
			glVertex3f( -Trees[i].xPos-(Trees[i].Size/100),
						Trees[i].Height-(Trees[i].Size/100), 
						-Trees[i].zPos+(Trees[i].Size/100));
		
			glTexCoord2f(1.0, 1.0);
			glVertex3f( -Trees[i].xPos+(Trees[i].Size/100),
						Trees[i].Height-(Trees[i].Size/100), 
						-Trees[i].zPos+(Trees[i].Size/100));
		
			glTexCoord2f(1.0, 0.0);
			glVertex3f( -Trees[i].xPos+(Trees[i].Size/100),
						Trees[i].Height-(Trees[i].Size/100), 
						-Trees[i].zPos-(Trees[i].Size/100));
		glEnd();

		glBindTexture(GL_TEXTURE_2D, GLTextureHandle[Trees[i].Texture]);
		glBegin(GL_QUADS);
			glPolygonOffset(10.0f, 1.0f);
			glTexCoord2f(1.0, 0.0);
			glVertex3f( -Trees[i].xPos-(Trees[i].Size/100),
						Trees[i].Height, 
						-Trees[i].zPos);
		
			glTexCoord2f(0.0, 0.0);
			glVertex3f( -Trees[i].xPos+(Trees[i].Size/100),
						Trees[i].Height, 
						-Trees[i].zPos);
		
			glTexCoord2f(0.0, 1.0);
			glVertex3f( -Trees[i].xPos+(Trees[i].Size/100),
						Trees[i].Height-(2*Trees[i].Size/100), 
						-Trees[i].zPos);
		
			glTexCoord2f(1.0, 1.0);
			glVertex3f( -Trees[i].xPos-(Trees[i].Size/100),
						Trees[i].Height-(2*Trees[i].Size/100), 
						-Trees[i].zPos);
		glEnd();

		glBegin(GL_QUADS);
			glPolygonOffset(10.0f, 1.0f);
			glTexCoord2f(1.0, 0.0);
			glVertex3f( -Trees[i].xPos,
						Trees[i].Height, 
						-Trees[i].zPos-(Trees[i].Size/100));
		
			glTexCoord2f(0.0, 0.0);
			glVertex3f( -Trees[i].xPos,
						Trees[i].Height, 
						-Trees[i].zPos+(Trees[i].Size/100));
		
			glTexCoord2f(0.0, 1.0);
			glVertex3f( -Trees[i].xPos,
						Trees[i].Height-(2*Trees[i].Size/100), 
						-Trees[i].zPos+(Trees[i].Size/100));
		
			glTexCoord2f(1.0, 1.0);
			glVertex3f( -Trees[i].xPos,
						Trees[i].Height-(2*Trees[i].Size/100), 
						-Trees[i].zPos-(Trees[i].Size/100));
		glEnd();


		
		
		glColor4f(	(float)(PSunRIntensity),(float)(PSunGIntensity),(float)(PSunBIntensity), 1.0f);


		glDisable(GL_DEPTH_TEST); 
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glStencilFunc(GL_EQUAL, 1, 0xffffffff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

		glBindTexture(GL_TEXTURE_2D, GLTextureHandle[Trees[i].Texture+1]);
		glBegin(GL_QUADS);
			glPolygonOffset(10.0f, 1.0f);
			glTexCoord2f(0.0, 0.0);
			glVertex3f( -Trees[i].xPos-(Trees[i].Size/100),
						Trees[i].Height-(Trees[i].Size/100), 
						-Trees[i].zPos-(Trees[i].Size/100));
		
			glTexCoord2f(0.0, 1.0);
			glVertex3f( -Trees[i].xPos-(Trees[i].Size/100),
						Trees[i].Height-(Trees[i].Size/100), 
						-Trees[i].zPos+(Trees[i].Size/100));
		
			glTexCoord2f(1.0, 1.0);
			glVertex3f( -Trees[i].xPos+(Trees[i].Size/100),
						Trees[i].Height-(Trees[i].Size/100), 
						-Trees[i].zPos+(Trees[i].Size/100));
		
			glTexCoord2f(1.0, 0.0);
			glVertex3f( -Trees[i].xPos+(Trees[i].Size/100),
						Trees[i].Height-(Trees[i].Size/100), 
						-Trees[i].zPos-(Trees[i].Size/100));
		glEnd();

		glBindTexture(GL_TEXTURE_2D, GLTextureHandle[Trees[i].Texture]);
		glBegin(GL_QUADS);
			glPolygonOffset(10.0f, 1.0f);
			glTexCoord2f(1.0, 0.0);
			glVertex3f( -Trees[i].xPos-(Trees[i].Size/100),
						Trees[i].Height, 
						-Trees[i].zPos);
		
			glTexCoord2f(0.0, 0.0);
			glVertex3f( -Trees[i].xPos+(Trees[i].Size/100),
						Trees[i].Height, 
						-Trees[i].zPos);
		
			glTexCoord2f(0.0, 1.0);
			glVertex3f( -Trees[i].xPos+(Trees[i].Size/100),
						Trees[i].Height-(2*Trees[i].Size/100), 
						-Trees[i].zPos);
		
			glTexCoord2f(1.0, 1.0);
			glVertex3f( -Trees[i].xPos-(Trees[i].Size/100),
						Trees[i].Height-(2*Trees[i].Size/100), 
						-Trees[i].zPos);
		glEnd();

		glBegin(GL_QUADS);
			glPolygonOffset(10.0f, 1.0f);
			glTexCoord2f(1.0, 0.0);
			glVertex3f( -Trees[i].xPos,
						Trees[i].Height, 
						-Trees[i].zPos-(Trees[i].Size/100));
	
			glTexCoord2f(0.0, 0.0);
			glVertex3f( -Trees[i].xPos,
						Trees[i].Height, 
						-Trees[i].zPos+(Trees[i].Size/100));
		
			glTexCoord2f(0.0, 1.0);
			glVertex3f( -Trees[i].xPos,
						Trees[i].Height-(2*Trees[i].Size/100), 
						-Trees[i].zPos+(Trees[i].Size/100));
		
			glTexCoord2f(1.0, 1.0);
			glVertex3f( -Trees[i].xPos,
						Trees[i].Height-(2*Trees[i].Size/100), 
						-Trees[i].zPos-(Trees[i].Size/100));
		glEnd();



	  }
	}

		glEnable(GL_FOG);
		glDisable(GL_STENCIL_TEST);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glEnable(GL_DEPTH_TEST); 
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		glDisable(GL_BLEND);
		glDisable(GL_FOG);
//		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);				// The Type Of Depth Testing To Do
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);	// Blending Function For Translucency Based On Source Alpha Value
//		glEnable(GL_CULL_FACE);
//		glCullFace(GL_FRONT);



}






//============================================================================
// Render stuff like position lights or other 3d-particles
//============================================================================
void Render3dParticles(void)
{
int i,i2 = 0;
int Visible3dParticles;
float particleIntensity;
float PSunRIntensity, PSunGIntensity, PSunBIntensity;
int LastUsedTexture=-1;

if (NumParticles==0)
	return;

	PSunRIntensity=SunRIntensity/256;
	PSunGIntensity=SunGIntensity/256;
	PSunBIntensity=SunBIntensity/256;
	if (PSunRIntensity<0.1f)
		PSunRIntensity=0.1f;
	if (PSunGIntensity<0.1f)
		PSunGIntensity=0.1f;
	if (PSunBIntensity<0.1f)
		PSunBIntensity=0.1f;


		glEnable(GL_BLEND); 
		glDisable(GL_LIGHTING); 
		if (GLTextures)
			glEnable(GL_TEXTURE_2D);
		glEnable(GL_FOG);

		glDepthFunc(GL_LEQUAL);				// The Type Of Depth Testing To Do
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);	// Blending Function For Translucency Based On Source Alpha Value
		glDisable(GL_FOG);
//		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR );
//		glDisable(GL_CULL_FACE);

		Visible3dParticles=0;
		for (i=0; i<NumParticles; i++)
		{
			if (Particles[i].ThreeD)
			{
//				GetDistance3d(Particles[i].xPos, Particles[i].Height, Particles[i].zPos, -CamXPos, -CamHeight, -CamZPos);
//				zOrder[Visible3dParticles]=TempDist;
				drawOrder[Visible3dParticles]=i;
				Visible3dParticles++;
			}
		}
//		QuickSort( 0, Visible3dParticles);
		
		glEnable(GL_STENCIL_TEST);


	for (i2=0; i2<Visible3dParticles; i2++)
	if (Particles[drawOrder[i2]].ThreeD) // && (Particles[drawOrder[i2]].isVisible)
	{
		i=drawOrder[i2];

		if (!((CamYaw>-10000.0f) && (CamYaw<10000.0f)))
			CamYaw=0;

		tempfloat = -CamYaw+90;
		while (tempfloat < 0.0f) {
			tempfloat += 360.0f;
		}
		while (tempfloat >= 360.0f) {
	 		tempfloat -= 360.0f;
		}
		TempYaw=tempfloat;

		if (!((CamPitch>-10000.0f) && (CamPitch<10000.0f)))
			CamPitch=0;

		tempfloat = -CamPitch+90;
		while (tempfloat < 0.0f) {
			tempfloat += 360.0f;
		}
		while (tempfloat >= 360.0f) {
			tempfloat -= 360.0f;
		}
		TempPitch=tempfloat;


		if ((unsigned)LastUsedTexture != GLTextureHandle[Particles[i].Texture])
		{
			glBindTexture(GL_TEXTURE_2D, GLTextureHandle[Particles[i].Texture]);
			LastUsedTexture = GLTextureHandle[Particles[i].Texture];
		}

		if (Particles[i].Texture==6) // Smoke darkens things
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_COLOR );
		else
			glBlendFunc(GL_SRC_ALPHA,GL_ONE );

		particleIntensity=(float)(Particles[i].TimeToLive/Particles[i].TotalTimeToLive*3.0f);

		if (Particles[i].ParticleType==0)
			glColor4f(	(float)(Particles[i].r*particleIntensity*PSunRIntensity/256),(float)(Particles[i].g*particleIntensity*PSunGIntensity/256),(float)(Particles[i].b*particleIntensity*PSunBIntensity/256),0.1f);
		else
			glColor4f(	(float)(Particles[i].r*particleIntensity/256),(float)(Particles[i].g*particleIntensity/256),(float)(Particles[i].b*particleIntensity/256),0.1f);

		glEnable(GL_DEPTH_TEST);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		/* Draw the ID 1 into the stencil buffer. */
		glStencilOp(GL_ZERO, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xffffffff);



		glBegin(GL_QUADS);
			glPolygonOffset(10.0f, 1.0f);
			glTexCoord2f(0.0, 0.0);
			glVertex3f( Particles[i].xPos-(Particles[i].Size/100),
						Particles[i].Height-(Particles[i].Size/100), 
						Particles[i].zPos);
		
			glTexCoord2f(0.0, 1.0);
			glVertex3f( Particles[i].xPos+(Particles[i].Size/100),
						Particles[i].Height-(Particles[i].Size/100), 
						Particles[i].zPos);
		
			glTexCoord2f(1.0, 1.0);
			glVertex3f( Particles[i].xPos+(Particles[i].Size/100),
						Particles[i].Height+(Particles[i].Size/100), 
						Particles[i].zPos);
		
			glTexCoord2f(1.0, 0.0);
			glVertex3f( Particles[i].xPos-(Particles[i].Size/100),
						Particles[i].Height+(Particles[i].Size/100), 
						Particles[i].zPos);
		glEnd();

		glBegin(GL_QUADS);
			glPolygonOffset(10.0f, 1.0f);
			glTexCoord2f(0.0, 0.0);
			glVertex3f( Particles[i].xPos,
						Particles[i].Height-(Particles[i].Size/100), 
						Particles[i].zPos-(Particles[i].Size/100));
		
			glTexCoord2f(0.0, 1.0);
			glVertex3f( Particles[i].xPos,
						Particles[i].Height-(Particles[i].Size/100), 
						Particles[i].zPos+(Particles[i].Size/100));
		
			glTexCoord2f(1.0, 1.0);
			glVertex3f( Particles[i].xPos,
						Particles[i].Height+(Particles[i].Size/100), 
						Particles[i].zPos+(Particles[i].Size/100));
		
			glTexCoord2f(1.0, 0.0);
			glVertex3f( Particles[i].xPos,
						Particles[i].Height+(Particles[i].Size/100), 
						Particles[i].zPos-(Particles[i].Size/100));
		glEnd();

		glBegin(GL_QUADS);
			glPolygonOffset(10.0f, 1.0f);
			glTexCoord2f(0.0, 0.0);
			glVertex3f( Particles[i].xPos-(Particles[i].Size/100),
						Particles[i].Height, 
						Particles[i].zPos-(Particles[i].Size/100));
		
			glTexCoord2f(0.0, 1.0);
			glVertex3f( Particles[i].xPos-(Particles[i].Size/100),
						Particles[i].Height, 
						Particles[i].zPos+(Particles[i].Size/100));
		
			glTexCoord2f(1.0, 1.0);
			glVertex3f( Particles[i].xPos+(Particles[i].Size/100),
						Particles[i].Height, 
						Particles[i].zPos+(Particles[i].Size/100));
		
			glTexCoord2f(1.0, 0.0);
			glVertex3f( Particles[i].xPos+(Particles[i].Size/100),
						Particles[i].Height, 
						Particles[i].zPos-(Particles[i].Size/100));
		glEnd();


		
		
		if (Particles[i].ParticleType==0)
			glColor4f(	(float)(Particles[i].r*particleIntensity*PSunRIntensity/256),(float)(Particles[i].g*particleIntensity*PSunGIntensity/256),(float)(Particles[i].b*particleIntensity*PSunBIntensity/256),0.8f);
		else
			glColor4f(	(float)(Particles[i].r*particleIntensity/256),(float)(Particles[i].g*particleIntensity/256),(float)(Particles[i].b*particleIntensity/256),0.8f);


		glDisable(GL_DEPTH_TEST); 
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

		/* Now, only render where stencil is set to 1. */
		glStencilFunc(GL_EQUAL, 1, 0xffffffff);  /* draw if stencil == 1 */
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);


		glBegin(GL_QUADS);
			glPolygonOffset(10.0f, 1.0f);
			glTexCoord2f(0.0, 0.0);
			glVertex3f( Particles[i].xPos-(Particles[i].Size/100),
						Particles[i].Height-(Particles[i].Size/100), 
						Particles[i].zPos);
		
			glTexCoord2f(0.0, 1.0);
			glVertex3f( Particles[i].xPos+(Particles[i].Size/100),
						Particles[i].Height-(Particles[i].Size/100), 
						Particles[i].zPos);
		
			glTexCoord2f(1.0, 1.0);
			glVertex3f( Particles[i].xPos+(Particles[i].Size/100),
						Particles[i].Height+(Particles[i].Size/100), 
						Particles[i].zPos);
		
			glTexCoord2f(1.0, 0.0);
			glVertex3f( Particles[i].xPos-(Particles[i].Size/100),
						Particles[i].Height+(Particles[i].Size/100), 
						Particles[i].zPos);
		glEnd();

		glBegin(GL_QUADS);
			glPolygonOffset(10.0f, 1.0f);
			glTexCoord2f(0.0, 0.0);
			glVertex3f( Particles[i].xPos,
						Particles[i].Height-(Particles[i].Size/100), 
						Particles[i].zPos-(Particles[i].Size/100));
		
			glTexCoord2f(0.0, 1.0);
			glVertex3f( Particles[i].xPos,
						Particles[i].Height-(Particles[i].Size/100), 
						Particles[i].zPos+(Particles[i].Size/100));
		
			glTexCoord2f(1.0, 1.0);
			glVertex3f( Particles[i].xPos,
						Particles[i].Height+(Particles[i].Size/100), 
						Particles[i].zPos+(Particles[i].Size/100));
		
			glTexCoord2f(1.0, 0.0);
			glVertex3f( Particles[i].xPos,
						Particles[i].Height+(Particles[i].Size/100), 
						Particles[i].zPos-(Particles[i].Size/100));
		glEnd();

		glBegin(GL_QUADS);
			glPolygonOffset(10.0f, 1.0f);
			glTexCoord2f(0.0, 0.0);
			glVertex3f( Particles[i].xPos-(Particles[i].Size/100),
						Particles[i].Height, 
						Particles[i].zPos-(Particles[i].Size/100));
		
			glTexCoord2f(0.0, 1.0);
			glVertex3f( Particles[i].xPos-(Particles[i].Size/100),
						Particles[i].Height, 
						Particles[i].zPos+(Particles[i].Size/100));
		
			glTexCoord2f(1.0, 1.0);
			glVertex3f( Particles[i].xPos+(Particles[i].Size/100),
						Particles[i].Height, 
						Particles[i].zPos+(Particles[i].Size/100));
		
			glTexCoord2f(1.0, 0.0);
			glVertex3f( Particles[i].xPos+(Particles[i].Size/100),
						Particles[i].Height, 
						Particles[i].zPos-(Particles[i].Size/100));
		glEnd();





	}


		glEnable(GL_FOG);
		glDisable(GL_STENCIL_TEST);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glEnable(GL_DEPTH_TEST); 
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		glDisable(GL_BLEND);
		glDisable(GL_FOG);
//		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);				// The Type Of Depth Testing To Do
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);	// Blending Function For Translucency Based On Source Alpha Value
//		glEnable(GL_CULL_FACE);
//		glCullFace(GL_FRONT);



}





#ifdef USE_GL


//============================================================================
// Calculate Fractal Sky Texture
//============================================================================
void calcSky1(void) {
	GLuint r, g, b;
	int i, j;
	

	for(i=0; i<256; i++)
	  for (j=0;j<256;j++) {

		r = (int)(sky1[(i + (j*256))]*4.0f);
		g = (int)(sky1[(i + (j*256))]*4.0f);
		b = (int)(sky1[(i + (j*256))]*4.0f);

		if (r<0)
			r=0;
		if (g<0)
			g=0;
		if (b<0)
			b=0;
		if (r>255)
			r=255;
		if (g>255)
			g=255;
		if (b>255)
			b=255;

/*		map[i][j][0] = r;
		map[i][j][1] = g;
		map[i][j][2] = b;
		map[i][j][3] = 255;
*/
		skytex1[(j + (i*256))*3 + 0] = (unsigned char)r;
		skytex1[(j + (i*256))*3 + 1] = (unsigned char)g;
		skytex1[(j + (i*256))*3 + 2] = (unsigned char)b;
//		skytex1[(j + (i*256))*4 + 3] = (unsigned char)255;
	  
	}

//	glGenTextures(1, &GLTextureHandle[31]);
 
 	glBindTexture(GL_TEXTURE_2D, GLTextureHandle[31]);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


// These are the most awful settings
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, &map[0][0][0]);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, 256, 256, GL_RGB, GL_UNSIGNED_BYTE, &skytex1[0]); 

//	delete[] skytex1;
}

//============================================================================
// Calc sky texture no 2
//============================================================================
void calcSky2(void) {
	GLuint r, g, b;
	int i, j;
//	unsigned char *skytex2;
//	skytex2 = new unsigned char[256 * 256 * 3]; 

	

	for(i=0; i<256; i++)
	  for (j=0;j<256;j++) {

		r = (int)(sky2[(j + (i*256))]*4.0f);
		g = (int)(sky2[(j + (i*256))]*4.0f);
		b = (int)(sky2[(j + (i*256))]*4.0f);

		if (r<0)
			r=0;
		if (g<0)
			g=0;
		if (b<0)
			b=0;
		if (r>255)
			r=255;
		if (g>255)
			g=255;
		if (b>255)
			b=255;

/*		map[i][j][0] = r;
		map[i][j][1] = g;
		map[i][j][2] = b;
		map[i][j][3] = 255;
*/
		skytex2[(j + (i*256))*3 + 0] = (unsigned char)r;
		skytex2[(j + (i*256))*3 + 1] = (unsigned char)g;
		skytex2[(j + (i*256))*3 + 2] = (unsigned char)b;
//		skytex2[(j + (i*256))*4 + 3] = (unsigned char)255;
	}

//	glGenTextures(1, &GLTextureHandle[32]);

	glBindTexture(GL_TEXTURE_2D, GLTextureHandle[32]);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, &map[0][0][0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, 256, 256, GL_RGB, GL_UNSIGNED_BYTE, &skytex2[0]); 
//	delete[] skytex2;
}










//============================================================================
// Calculate a texture for the map, showing terrain and objects
//============================================================================
void calcMap(void) {
	GLuint r, g, b;
	int i, j, curobj;

	

	for(i=0; i<256; i++)
	  for (j=0;j<256;j++) {
		if((int)heightfield[((i/4) + ((j/4)*64))]*2>(20 + rand()%4)) {
			r = 128;
			g = 128;
			b = 128;
		} 
		else if((int)heightfield[((i/4) + ((j/4)*64))]*2>(0 + rand()%4)) {
			r = 20;
			g = 128;
			b = 20;
		}
		else if((int)heightfield[((i/4) + ((j/4)*64))]*2>-20) {
			r = 128;
			g = 105;
			b = 25;
		}
		else {
			r = 20;
			g = 20;
			b = 90;
		}

		//If there's no object, show the terrain height via color coding

/*		r = 0;
		g = 80+heightfield[((i/4) + ((j/4)*64))]*2;
		b = 0;
*/

		//Objects override terrain on the map		
		for (curobj=0; curobj<NumObjects; curobj++)
		if (Objects[curobj].ObjectId == curobj)
		{
			if (((int)(i/4) == (Objects[curobj].xGrid+31)) && ((int)(j/4) == (Objects[curobj].zGrid+31)))
			{
				if (Objects[curobj].ObjectMesh!=OBJECTMESH_LASER)  //Don't show lasers on map!
				{
					if (Objects[curobj].Team != Player[localplayer].Team)	//Color enemies red
					{
						r = 255;
						g = 0;
						b = 0;
					}
					else
					{
						r = 0;				//..and team stuff blue
						g = 0;
						b = 255;
					}
					
					if (Player[localplayer].ControlledObject == curobj)		//local player=white
					{
						r = 255;
						g = 255;
						b = 255;
					}
	
					if ((Objects[curobj].ObjectType==OBJECTTYPE_WEAPON))  //Show missiles yellow
					{
						r = 255;
						g = 255;
						b = 0;
					}

				}

			}
		} 


		map[255-i][j][0] = r;
		map[255-i][j][1] = g;
		map[255-i][j][2] = b;
//		map[i][j][3] = 255;
	}

//	glGenTextures(1, &mapHandle);
	glBindTexture(GL_TEXTURE_2D, GLTextureHandle[maxTexture+1]);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, &map[0][0][0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//They say that GL_NEAREST is faster than LINEAR_MIPMAP_LINEAR
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 4, 256, 256, GL_RGB, GL_UNSIGNED_BYTE, &map[0][0][0]); 

}









void drawMap(void)
{

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, winWidth*2, winHeight*2, 0); // Set up a 2D screen.
		glTranslatef(0.0, 2.0, 0.0);

//		glMatrixMode(GL_MODELVIEW);
//		glLoadIdentity();

		glDisable(GL_DEPTH_TEST); 
		glEnable(GL_BLEND); 
		glDisable(GL_LIGHTING); 

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, GLTextureHandle[maxTexture+1]); 

		glBegin(GL_QUADS);
			glColor4f(1.0f,1.0f,1.0f,0.9f);
			glTexCoord2f(0.0, 0.0);
			glVertex2f(0.0, 0.0);

			glColor4f(1.0f,1.0f,1.0f,0.9f);
			glTexCoord2f(1.0, 0.0);
			glVertex2f(256.0, 2.0);

			glColor4f(1.0f,1.0f,1.0f,0.9f);
			glTexCoord2f(1.0, 1.0);
			glVertex2f(256.0, 256.0);

			glColor4f(1.0f,1.0f,1.0f,0.9f);
			glTexCoord2f(0.0, 1.0);
			glVertex2f(0.0, 256.0);
		glEnd();

		glEnable(GL_DEPTH_TEST); 
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND); 
		glEnable(GL_LIGHTING); 


}

#endif

//============================================================================
// loads a .RAW texture for OpenGL
//============================================================================
void loadGLTexture (char *file, int id)
{
#ifdef USE_GL
 FILE *FilePic;
 unsigned long i,j;
 unsigned char *AllGLTexture;

 if (SCREEN_LORES==0)
 {
	TEX_WIDTH  = 256;
	TEX_HEIGHT = 256;
 }
 else
 {
	TEX_WIDTH  = 128;
	TEX_HEIGHT = 128;
 }


 AllGLTexture = new unsigned char[TEX_WIDTH * TEX_HEIGHT * 3]; 

 if ((FilePic=fopen(file,"r"))==NULL)
 {
		sprintf(tempstring, "loadGLTexture(): Couldn't load texture %s : File not found!", file);
		WriteErrorLog(tempstring);
		MessageBox(NULL, "Missing OpenGL texture!", "File Not Found", MB_ICONSTOP);
	exit (1);
 }
 fread (bufferpic , 256*256*3, 1 , FilePic);
 fclose (FilePic);

 if (SCREEN_LORES==0)
 {
	for (i=0;i<256;i++)
	{
	    for (j=0;j<256;j++)
	    {
			AllGLTexture[(j + (i*256))*3 + 0] = (unsigned char)bufferpic[i][j][0];
			AllGLTexture[(j + (i*256))*3 + 1] = (unsigned char)bufferpic[i][j][1];
			AllGLTexture[(j + (i*256))*3 + 2] = (unsigned char)bufferpic[i][j][2];
//			AllGLTexture[(j + (i*256))*4 + 3] = (unsigned char)255;
	    }
	}
 }
 
 
//texture downscaling to 128x128
 if (SCREEN_LORES==1)
 {
	for (i=0;i<128;i++)
	{
	    for (j=0;j<128;j++)
	    {
		AllGLTexture[(j + (i*128))*3 + 0] = (unsigned char)((bufferpic[i*2][j*2][0]+
																bufferpic[i*2+1][j*2][0]+
																bufferpic[i*2][j*2+1][0]+
																bufferpic[i*2+1][j*2+1][0]) /4);
		AllGLTexture[(j + (i*128))*3 + 1] = (unsigned char)((bufferpic[i*2][j*2][1]+
																bufferpic[i*2+1][j*2][1]+
																bufferpic[i*2][j*2+1][1]+
																bufferpic[i*2+1][j*2+1][1]) /4);
		AllGLTexture[(j + (i*128))*3 + 2] = (unsigned char)((bufferpic[i*2][j*2][2]+
																bufferpic[i*2+1][j*2][2]+
																bufferpic[i*2][j*2+1][2]+
																bufferpic[i*2+1][j*2+1][2]) /4);
		}
	}
 }

		maxTexture=id;
		glGenTextures(1, &GLTextureHandle[id]);		// Create The Textures

		glBindTexture(GL_TEXTURE_2D, GLTextureHandle[id]);

//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

//		Highest Quality
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


//		Mipmapping
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);

//		Highest rendering speed
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


//		Use this for normal textures
//		glTexImage2D(GL_TEXTURE_2D, 0, 4, TEX_WIDTH, TEX_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, &AllGLTexture[curId][0][0][0]);

//		Use this for mipmapping
		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, TEX_WIDTH, TEX_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, &AllGLTexture[0]); 

delete[] AllGLTexture;





if (glGetError()==GL_OUT_OF_MEMORY)
{
	sprintf(tempstring, "loadGLTexture(): Error loading texture %s: Out of texture memory!", file);
	WriteErrorLog(tempstring);
	sprintf(tempstring, "          NOTE : LoRes Textures have been enabled for subsequent runs to fix this.");
	WriteErrorLog(tempstring);
	SCREEN_LORES=1;

}

#endif
}




//============================================================================
// loads a .RAW texture for OpenGL
//============================================================================
void loadGLAlphaTexture (char *file, char *afile, int id)
{
#ifdef USE_GL
 FILE *FilePic;
 unsigned long i,j;
 unsigned char *AllGLTexture;

 if (SCREEN_LORES==0)
 {
	TEX_WIDTH  = 256;
	TEX_HEIGHT = 256;
 }
 else
 {
	TEX_WIDTH  = 128;
	TEX_HEIGHT = 128;
 }


 AllGLTexture = new unsigned char[TEX_WIDTH * TEX_HEIGHT * 4]; 

 if ((FilePic=fopen(file,"r"))==NULL)
 {
		sprintf(tempstring, "loadGLAlphaTexture(): Couldn't load texture %s : File not found!", file);
		WriteErrorLog(tempstring);
		MessageBox(NULL, "Missing OpenGL texture!", "File Not Found", MB_ICONSTOP);
	exit (1);
 }
 fread (bufferpic , 256*256*3, 1 , FilePic);
 fclose (FilePic);

 if (SCREEN_LORES==0)
 {
	for (i=0;i<256;i++)
	{
	    for (j=0;j<256;j++)
	    {
			AllGLTexture[(j + (i*256))*4 + 0] = (unsigned char)bufferpic[i][j][0];
			AllGLTexture[(j + (i*256))*4 + 1] = (unsigned char)bufferpic[i][j][1];
			AllGLTexture[(j + (i*256))*4 + 2] = (unsigned char)bufferpic[i][j][2];
//			AllGLTexture[(j + (i*256))*4 + 3] = (unsigned char)255;
	    }
	}
 }


//texture downscaling to 128x128
 if (SCREEN_LORES==1)
 {
	for (i=0;i<128;i++)
	{
	    for (j=0;j<128;j++)
	    {
		AllGLTexture[(j + (i*128))*4 + 0] = (unsigned char)((bufferpic[i*2][j*2][0]+
																bufferpic[i*2+1][j*2][0]+
																bufferpic[i*2][j*2+1][0]+
																bufferpic[i*2+1][j*2+1][0]) /4);
		AllGLTexture[(j + (i*128))*4 + 1] = (unsigned char)((bufferpic[i*2][j*2][1]+
																bufferpic[i*2+1][j*2][1]+
																bufferpic[i*2][j*2+1][1]+
																bufferpic[i*2+1][j*2+1][1]) /4);
		AllGLTexture[(j + (i*128))*4 + 2] = (unsigned char)((bufferpic[i*2][j*2][2]+
																bufferpic[i*2+1][j*2][2]+
																bufferpic[i*2][j*2+1][2]+
																bufferpic[i*2+1][j*2+1][2]) /4);
		}
	}
 }


 if ((FilePic=fopen(afile,"r"))==NULL)
 {
		sprintf(tempstring, "loadGLAlphaTexture(): Couldn't load texture %s : File not found!", file);
		WriteErrorLog(tempstring);
		MessageBox(NULL, "Missing OpenGL texture!", "File Not Found", MB_ICONSTOP);
	exit (1);
 }
 fread (bufferpic , 256*256*3, 1 , FilePic);
 fclose (FilePic);

 if (SCREEN_LORES==0)
 {
	for (i=0;i<256;i++)
	{
	    for (j=0;j<256;j++)
	    {
			AllGLTexture[(j + (i*256))*4 + 3] = (unsigned char)bufferpic[i][j][0];
	    }
	}
 }
 
 if (SCREEN_LORES==1)
 {
	for (i=0;i<128;i++)
	{
	    for (j=0;j<128;j++)
	    {
		AllGLTexture[(j + (i*128))*4 + 3] = (unsigned char)((bufferpic[i*2][j*2][0]+
																bufferpic[i*2+1][j*2][0]+
																bufferpic[i*2][j*2+1][0]+
																bufferpic[i*2+1][j*2+1][0]) /4);
		}
	}
 }


		maxTexture=id;
		glGenTextures(1, &GLTextureHandle[id]);		// Create The Textures

		glBindTexture(GL_TEXTURE_2D, GLTextureHandle[id]);

//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

//		Highest Quality
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


//		Mipmapping
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);

//		Highest rendering speed
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


//		Use this for normal textures
//		glTexImage2D(GL_TEXTURE_2D, 0, 4, TEX_WIDTH, TEX_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, &AllGLTexture[curId][0][0][0]);

//		Use this for mipmapping
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, TEX_WIDTH, TEX_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, &AllGLTexture[0]); 

delete[] AllGLTexture;





if (glGetError()==GL_OUT_OF_MEMORY)
{
	sprintf(tempstring, "loadGLAlphaTexture(): Error loading texture %s: Out of texture memory!", file);
	WriteErrorLog(tempstring);
	sprintf(tempstring, "          NOTE : LoRes Textures have been enabled for subsequent runs to fix this.");
	WriteErrorLog(tempstring);
	SCREEN_LORES=1;

}

#endif
}



//============================================================================
// loads a .RAW icon for OpenGL
//============================================================================
void loadGLIcon (char *file, int id)
{
#ifdef USE_GL
 FILE *FilePic;
 unsigned long i,j;
 unsigned char *AllGLTexture;

	TEX_WIDTH  = 32;
	TEX_HEIGHT = 32;


 AllGLTexture = new unsigned char[32*32 * 3]; 

 if ((FilePic=fopen(file,"r"))==NULL)
 {
		sprintf(tempstring, "loadGLIcon(): Couldn't load texture %s : File not found!", file);
		WriteErrorLog(tempstring);
		MessageBox(NULL, "Missing OpenGL texture!", "File Not Found", MB_ICONSTOP);
	exit (1);
 }
 fread (bufferpic1 , 32*32*3, 1 , FilePic);
 fclose (FilePic);

	for (i=0;i<32;i++)
	{
	    for (j=0;j<32;j++)
	    {
 			AllGLTexture[(j + (i*32))*3 + 0] = (unsigned char)bufferpic1[i][j][0];
			AllGLTexture[(j + (i*32))*3 + 1] = (unsigned char)bufferpic1[i][j][1];
			AllGLTexture[(j + (i*32))*3 + 2] = (unsigned char)bufferpic1[i][j][2];
/*			if ((bufferpic[i][j][0]>0) && (bufferpic[i][j][1]>0) && (bufferpic[i][j][2]>0))
				AllGLTexture[(j + (i*32))*4 + 3] = (unsigned char)255;
			else
				AllGLTexture[(j + (i*32))*4 + 3] = (unsigned char)0;
*/	    }
	}
 
 
		maxTexture=id;
		glGenTextures(1, &GLTextureHandle[id]);		// Create The Textures

		glBindTexture(GL_TEXTURE_2D, GLTextureHandle[id]);

//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

//		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

//		Highest Quality
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


//		Mipmapping
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);

//		Highest rendering speed
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


//		Use this for normal textures
//		glTexImage2D(GL_TEXTURE_2D, 0, 4, TEX_WIDTH, TEX_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, &AllGLTexture[0]);

//		Use this for mipmapping
		gluBuild2DMipmaps(GL_TEXTURE_2D, 4, 32, 32, GL_RGB, GL_UNSIGNED_BYTE, &AllGLTexture[0]); 

delete[] AllGLTexture;





if (glGetError()==GL_OUT_OF_MEMORY)
{
	sprintf(tempstring, "loadGLIcon(): Error loading texture %s: Out of texture memory!", file);
	WriteErrorLog(tempstring);
	sprintf(tempstring, "          NOTE : LoRes Textures have been enabled for subsequent runs to fix this.");
	WriteErrorLog(tempstring);
	SCREEN_LORES=1;

}

#endif
}


void DrawPercentageBar1(int pc)
{
	glDisable(GL_DEPTH_TEST); 
	glDisable(GL_BLEND); 
	glDisable(GL_LIGHTING); 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 1, 0, 1); // Set up a 2D screen.
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, GLTextureHandle[25]);
	glBegin(GL_QUADS);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0, 1.0);
	glVertex2f(0.0f, 0.0f);
	glTexCoord2f(0.0, 1.0);
	glVertex2f(1.0f, 0.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex2f(1.0f, 1.0f);
	glTexCoord2f(1.0, 0.0);
	glVertex2f(0.0f, 1.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	
	glBegin(GL_QUADS);
	glColor4f(0.3f, 0.0f, 0.0f, 1.0f);
	glVertex2f(0.2f, 0.53f);
	glVertex2f(0.8f, 0.53f);
	glVertex2f(0.8f, 0.57f);
	glVertex2f(0.2f, 0.57f);
	glEnd();

	glBegin(GL_QUADS);
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex2f(0.2f, 0.53f);
	glVertex2f(0.2f+((float)pc/100.0f*0.4f), 0.53f);
	glVertex2f(0.2f+((float)pc/100.0f*0.4f), 0.57f);
	glVertex2f(0.2f, 0.57f);
	glEnd();
	glEnable(GL_TEXTURE_2D);


}

void DrawPercentageBar2(int pc)
{
	glDisable(GL_DEPTH_TEST); 
	glDisable(GL_BLEND); 
	glDisable(GL_LIGHTING); 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 1, 0, 1); // Set up a 2D screen.
	glEnable(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, GLTextureHandle[25]);
	glBegin(GL_QUADS);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0, 1.0);
	glVertex2f(0.0f, 0.0f);
	glTexCoord2f(0.0, 1.0);
	glVertex2f(1.0f, 0.0f);
	glTexCoord2f(0.0, 0.0);
	glVertex2f(1.0f, 1.0f);
	glTexCoord2f(1.0, 0.0);
	glVertex2f(0.0f, 1.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	
	glBegin(GL_QUADS);
	glColor4f(0.3f, 0.0f, 0.0f, 1.0f);
	glVertex2f(0.2f, 0.53f);
	glVertex2f(0.8f, 0.53f);
	glVertex2f(0.8f, 0.57f);
	glVertex2f(0.2f, 0.57f);
	glEnd();

	glBegin(GL_QUADS);
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex2f(0.2f, 0.53f);
	glVertex2f(0.6f+((float)pc/100.0f*0.2f), 0.53f);
	glVertex2f(0.6f+((float)pc/100.0f*0.2f), 0.57f);
	glVertex2f(0.2f, 0.57f);
	glEnd();
	glEnable(GL_TEXTURE_2D);


}





//Stuff for Terrain texturing
//----------------------------
//Texture data for each terrain tile
//GLubyte TerrainTexture[66][66][256][256][4];
//GLuint GLTerrainTextureHandle[66][66];
//The source for the terrain blending
//GLubyte BasicTiles[5][256][256][4];
//GLuint GLBasicTileHandle[5];



TEXTURE *new_bitmap(int x, int y) {
	TEXTURE *bmp;
	
	bmp = new TEXTURE;

	bmp -> w = x;
	bmp -> h = y;
	bmp -> data = new unsigned char[((bmp -> w)*(bmp -> h))*3];
	
	if(!bmp || !bmp->data) {
			WriteErrorLog("new_bitmap(): memory allocation error");
			MessageBox(NULL, "Memory allocation error!", "new()", MB_ICONSTOP);
		exit (1);
	}
	else {
		return bmp;
	}
}

void destroy_bmp(TEXTURE *bmp)
{
	if (bmp) {
		if (bmp->data) {
			delete[] bmp->data;
		}

		delete[] bmp; 
	}
}


void loadBasicTile (char *file, int id)
{
#ifdef USE_GL
 FILE *FilePic;
 unsigned long i,j;

 if ((FilePic=fopen(file,"r"))==NULL)
  {
  MessageBox(NULL, "Missing OpenGL texture!", "File Not Found", MB_ICONSTOP);
  exit (1);
  }
 fread (bufferpic , 256*256*3, 1 , FilePic);
 fclose (FilePic);

if (SCREEN_LORES==0)
{
 for (i=0;i<256;i++)
   {
    for (j=0;j<256;j++)
    {	BasicTiles[id][i][j][0]=bufferpic[i][j][0];
	 	BasicTiles[id][i][j][1]=bufferpic[i][j][1];
	 	BasicTiles[id][i][j][2]=bufferpic[i][j][2];
		BasicTiles[id][i][j][3]=255;
    }
   }
}
else
//texture downscaling to 128x128
{
	for (i=0;i<128;i++)
	{
	    for (j=0;j<128;j++)
	    {
		BasicTiles[id][i][j][0] =  (bufferpic[i*2][j*2][0]+
									bufferpic[i*2+1][j*2][0]+
									bufferpic[i*2][j*2+1][0]+
									bufferpic[i*2+1][j*2+1][0]) /4;
		BasicTiles[id][i][j][1] =  (bufferpic[i*2][j*2][1]+
									bufferpic[i*2+1][j*2][1]+
									bufferpic[i*2][j*2+1][1]+
									bufferpic[i*2+1][j*2+1][1]) /4;
		BasicTiles[id][i][j][2] =  (bufferpic[i*2][j*2][2]+
									bufferpic[i*2+1][j*2][2]+
									bufferpic[i*2][j*2+1][2]+
									bufferpic[i*2+1][j*2+1][2]) /4;
		BasicTiles[id][i][j][3]=255;
		}
	}
}
#endif
}

/*
void SET_COLOR(int xtile, int ztile, int x, int y, BYTE r, BYTE g, BYTE b) {
	if((x < 2048) && (y < 2048)) {
		TerrainTexture[xtile][ztile]->data[(x + (y*2048))*3 + 0] = r;
		TerrainTexture[xtile][ztile]->data[(x + (y*2048))*3 + 1] = g;
		TerrainTexture[xtile][ztile]->data[(x + (y*2048))*3 + 2] = b;
	}
}
*/
void GET_COLOR(int tex, int x, int y, BYTE *r, BYTE *g, BYTE *b) {

if (SCREEN_LORES==0)
{
	
	x+=256; y+=256;

	do {
		x-=256;
	} while (x>=256);
	
	do {
		y-=256;
	} while (y>=256);

	
	if((x < 256) && (y < 256)) {
		*r = BasicTiles[tex][x][y][0];
		*g = BasicTiles[tex][x][y][1];
		*b = BasicTiles[tex][x][y][2];
	}
}
else
{
	
	x+=128; y+=128;

	do {
		x-=128;
	} while (x>=128);
	
	do {
		y-=128;
	} while (y>=128);

	
	if((x < 128) && (y < 128)) {
		*r = BasicTiles[tex][x][y][0];
		*g = BasicTiles[tex][x][y][1];
		*b = BasicTiles[tex][x][y][2];
	}
}



}


int limit255(int a) {
	if(a < 0) {
		return 0;
	}
	else if(a > 255) {
		return 255;
	}
	else {
		return a;
	}
}


//h1=startheight, h2=endheight, ch=currentheight
float texture_factor(int h1, int h2, int ch) {
	float t;
	float range;

	if (h1>h2)
		range=(h1-h2);
	else
		range=(h2-h1);

	if (h1>ch)
		t = (range - abs(h1 - ch)) / range;
	else
		t = (range - abs(ch - h1)) / range;

	if(t < 0.0f) t = 0.0f;
	else if(t > 1.0f) t = 1.0f;

	return t;
}


float get_height(int xtile, int ztile, float x, float z)
{
float h1, h2, h3, h4;
float x0, x1, midpoint;

/*
h1= (float)(heightfield[xtile+1][ztile]);
h2= (float)(heightfield[xtile][ztile]);
h3= (float)(heightfield[xtile+1][ztile+1]);
h4= (float)(heightfield[xtile][ztile+1]);
*/

h1= (float)(heightfield[(xtile+1 + (ztile*64))]);
h2= (float)(heightfield[(xtile + (ztile*64))]);
h3= (float)(heightfield[(xtile+1 + ((ztile+1)*64))]);
h4= (float)(heightfield[(xtile + ((ztile+1)*64))]);

// h1 - h2
// |    |
// h3 - h4


if (SCREEN_LORES==0)
{
	x0 = (float)(h1 + (h3 - h1)*(float)(z/32.0f));
	x1 = (float)(h2 + (h4 - h2)*(float)(z/32.0f));
	midpoint = (float)(x1  + (x0 - x1)*(float)(x/32.0f));
}
else
{
	x0 = (float)(h1 + (h3 - h1)*(float)(z/16.0f));
	x1 = (float)(h2 + (h4 - h2)*(float)(z/16.0f));
	midpoint = (float)(x1  + (x0 - x1)*(float)(x/16.0f));
}
	midpoint *= 5.0f;
return (midpoint);
}


void GenerateTerrainTextures(void)
{
	int x, z;
	int h1;
	float f0;
	float f1;
	float f2;
	float f3;
	float f4;
	unsigned char *rgb;
	int op = 0;
	int lsdim;

	BYTE r[5], g[5], b[5];
	BYTE new_r, new_g, new_b;


	if (SCREEN_LORES==0)
		lsdim=2048;
	else
		lsdim=1024;

	rgb = new unsigned char[lsdim*lsdim * 3]; 


		for(x=0; x<lsdim; x++) {

		if ( (int)(100.0f*((float)(x)/(float)(lsdim))) > op)
		{
			op = (int)(100.0f*((float)x/(float)lsdim));
//			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			glClearColor(0.0, 0.0, 0.0, 1.0);
			DrawPercentageBar2(op);
			mytext.Size(10);
			sprintf(tempstring, "\\c(255 255 255)Texturing Terrain:\\c(128 128 128) %i%s", op, " percent done." );
			mytext.Draw(20, 20, tempstring);
			glFlush();
			SwapBuffers( DR_HDC ); 
		}
			
		  for(z=0; z<lsdim; z++) {

			if (SCREEN_LORES==0)
				h1 = (get_height((x/32), (z/32), x%32, z%32));
			else
				h1 = (get_height((x/16), (z/16), x%16, z%16));

			f4 = texture_factor(160, 110, h1);
			f3 = texture_factor(110, 60, h1);
			f2 = texture_factor(60, 0, h1);
			f1 = texture_factor(0, -120, h1);
			f0 = texture_factor(-90, -170, h1);
			
			GET_COLOR(0, x, z, &r[0], &g[0], &b[0]);
			GET_COLOR(1, x, z, &r[1], &g[1], &b[1]);
			GET_COLOR(2, x, z, &r[2], &g[2], &b[2]);
			GET_COLOR(3, x, z, &r[3], &g[3], &b[3]);

			new_r = limit255((r[0] * f0) + (r[1] * f1) + (r[2] * f2) + (r[3] * f3) + (r[3] * f4)); // + (r[1] * f3) + (r[1] * f4));
			new_g = limit255((g[0] * f0) + (g[1] * f1) + (g[2] * f2) + (g[3] * f3) + (g[3] * f4)); // + (g[1] * f3) + (g[1] * f4));
			new_b = limit255((b[0] * f0) + (b[1] * f1) + (b[2] * f2) + (b[3] * f3) + (b[3] * f4)); // + (b[1] * f3) + (b[1] * f4));
			
			if((x < lsdim) && (z < lsdim)) {
				rgb[(x + (z*lsdim))*3 + 0] = new_r;
				rgb[(x + (z*lsdim))*3 + 1] = new_g;
				rgb[(x + (z*lsdim))*3 + 2] = new_b;
			}

		  }
		}


		glBindTexture(GL_TEXTURE_2D, GLTerrainTextureHandle);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB5, lsdim, lsdim, GL_RGB, GL_UNSIGNED_BYTE, rgb); 


if (glGetError()==GL_OUT_OF_MEMORY)
{
	sprintf(tempstring, "GenerateTerrainTextures(): Error storing landscape texture: Out of texture memory!");
	WriteErrorLog(tempstring);
	if (SCREEN_LORES==0)
	{
		sprintf(tempstring, "                    NOTE : LoRes Textures have been enabled for subsequent runs to fix this");
		WriteErrorLog(tempstring);
		SCREEN_LORES=1;
	}
	else
	{
		sprintf(tempstring, "                    NOTE : LoRes Textures already enabled. Your 3d card has less than 8 Megs of RAM!! I cannot work around this situation!");
		WriteErrorLog(tempstring);
	}

}

delete [] rgb;

}


//-----------------------------









void LoadTerrainTileset(int CurrentTerrain)
{
	if (CurrentTerrain==TERRAIN_LUSH)
	{
	loadBasicTile("textures\\lush\\tex0.raw", 0);
	loadBasicTile("textures\\lush\\tex1.raw", 1);
	loadBasicTile("textures\\lush\\tex2.raw", 2);
	loadBasicTile("textures\\lush\\tex3.raw", 3);
	}
	if (CurrentTerrain==TERRAIN_STONE)
	{
	loadBasicTile("textures\\stone\\tex0.raw", 0);
	loadBasicTile("textures\\stone\\tex1.raw", 1);
	loadBasicTile("textures\\stone\\tex2.raw", 2);
	loadBasicTile("textures\\stone\\tex3.raw", 3);
	}
	if (CurrentTerrain==TERRAIN_SNOW)
	{
	loadBasicTile("textures\\snow\\tex0.raw", 0);
	loadBasicTile("textures\\snow\\tex1.raw", 1);
	loadBasicTile("textures\\snow\\tex2.raw", 2);
	loadBasicTile("textures\\snow\\tex3.raw", 3);
	}
	if (CurrentTerrain==TERRAIN_DESERT)
	{
	loadBasicTile("textures\\desert\\tex0.raw", 0);
	loadBasicTile("textures\\desert\\tex1.raw", 1);
	loadBasicTile("textures\\desert\\tex2.raw", 2);
	loadBasicTile("textures\\desert\\tex3.raw", 3);
	}
	if (CurrentTerrain==TERRAIN_LAVA)
	{
	loadBasicTile("textures\\lava\\tex0.raw", 0);
	loadBasicTile("textures\\lava\\tex1.raw", 1);
	loadBasicTile("textures\\lava\\tex2.raw", 2);
	loadBasicTile("textures\\lava\\tex3.raw", 3);
	}
}


void LoadAllTextures(void)
{

glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
glClearColor(0.0, 0.0, 0.0, 1.0);
mytext.Size(10);
sprintf(tempstring, "\\c(255 255 255)Loading Textures");
mytext.Draw(20, 20, tempstring);
glFlush();
SwapBuffers( DR_HDC ); 

	loadGLTexture("textures\\dirt1.raw", 0);
	loadGLTexture("textures\\rock14.raw", 1);
	loadGLTexture("textures\\cube3.raw", 2);
	loadGLTexture("textures\\laser.raw", 3);
	loadGLTexture("textures\\gray.raw", 4);
	loadGLTexture("textures\\minel.raw", 5);
	loadGLTexture("textures\\smoke.raw", 6);
	loadGLTexture("textures\\wall1.raw", 7);
	loadGLTexture("textures\\wall2.raw", 8);
	loadGLTexture("textures\\wall3.raw", 9);
	loadGLTexture("textures\\wall4.raw", 10);
	loadGLTexture("textures\\wall5.raw", 11);
	loadGLTexture("textures\\wall6.raw", 12);
	loadGLTexture("textures\\wall7.raw", 13);
	loadGLTexture("textures\\beam.raw", 14);
	loadGLTexture("textures\\blite.raw", 15);
	loadGLTexture("textures\\blue.raw", 16);
	loadGLTexture("textures\\bolts.raw", 17);
	loadGLTexture("textures\\catwalk.raw", 18);
	loadGLTexture("textures\\concrete.raw", 19);
	loadGLTexture("textures\\corrugated.raw", 20);
	loadGLTexture("textures\\dam.raw", 21);
	loadGLTexture("textures\\dirt1.raw", 22);
	loadGLTexture("textures\\door128.raw", 23);
	loadGLTexture("textures\\door2.raw", 24);
	loadGLTexture("textures\\gdoor.raw", 25);
	loadGLTexture("textures\\gravel1.raw", 26);
	loadGLTexture("textures\\tex10.raw", 27);
	loadGLTexture("textures\\wallmark.raw", 28);
	loadGLTexture("textures\\tanktire.raw", 29);
	loadGLTexture("textures\\flare.raw", 30);
	//31 and 32 are left our for the fractal sky texture
	loadGLTexture("textures\\star.raw", 33);
	loadGLTexture("textures\\moon.raw", 34);
	loadGLTexture("textures\\laserbeam.raw", 35);
	loadGLTexture("textures\\exit.raw", 36);
	loadGLTexture("textures\\console1.raw", 37);	//3 console screens
	loadGLTexture("textures\\console2.raw", 38);
	loadGLTexture("textures\\console3.raw", 39);
	loadGLTexture("textures\\console11.raw", 40);	//same as above, without text display
	loadGLTexture("textures\\console21.raw", 41);
	loadGLTexture("textures\\console31.raw", 42);
	loadGLTexture("textures\\waterG.raw", 43);		//For the Waterplane
	loadGLTexture("textures\\s1body.raw", 44);
	loadGLTexture("textures\\s1wing.raw", 45);
	loadGLTexture("textures\\s1side.raw", 46);
	loadGLTexture("textures\\s2body.raw", 47);
	loadGLTexture("textures\\s2wing.raw", 48);

//	loadGLTexture("textures\\tree1_1.raw", 49);
//	loadGLTexture("textures\\tree1_2.raw", 50);

	loadGLAlphaTexture("textures\\tree1_1.raw", "textures\\tree1_1a.raw", 49);
	loadGLAlphaTexture("textures\\tree1_2.raw", "textures\\tree1_2a.raw", 50);

	loadGLIcon("textures\\icon_target.raw", ICON_TARGET);
	loadGLIcon("textures\\icon_friendly.raw", ICON_FRIENDLY);
	loadGLIcon("textures\\icon_enemybase.raw", ICON_ENEMYBASE);
	loadGLIcon("textures\\icon_friendlybase.raw", ICON_FRIENDLYBASE);
	
	
	//maxTexture+1 is left out for the map texture
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &GLTextureHandle[maxTexture+1]); //One spare texture for the map rendering
	glGenTextures(1, &GLTerrainTextureHandle);

}


void UnloadAllTextures(void)
{
int curId;

if (GLTextures)
{
glDeleteTextures(1, &GLTerrainTextureHandle );
for (curId=0; curId<=maxTexture+1; curId++)
	glDeleteTextures(1, &GLTextureHandle[curId] );
}

	delete[] TempVertices;
	delete[] Faces;
	delete[] PFaces;
	delete[] zOrder;
	delete[] drawOrder;
	delete[] Vertices;
	delete[] VisVertices;
	delete[] xfVerts;
	delete[] prjVerts;
	delete[] originalVerts;
	delete[] Trees;
//	delete[] Noise;
//	delete[] samples;

//	delete[] heightfield;
//	delete[] sky1;
//	delete[] sky2;

//	delete[] Player;
//	delete[] Team;
//	delete[] Objects;
//	for (tempint=0; tempint<30; tempint++)
//	{
//		delete[] Meshes[tempint].Faces;
//		delete[] Meshes[tempint].Vertices;
//	}
//	delete[] Meshes;
//	delete[] ParticleSlots;
	delete[] ParticleEmitters;
	delete[] Particles;
//	delete[] Lights;

	delete[] skytex1;
	delete[] skytex2;


}


void init_gl(void) 
{


#ifdef USE_GL

	sprintf(tempstring, "Vendor: %s", glGetString(GL_VENDOR)); WriteErrorLog(tempstring);
	sprintf(tempstring, "Renderer: %s", glGetString(GL_RENDERER)); WriteErrorLog(tempstring);
	sprintf(tempstring, "Version: %s", glGetString(GL_VERSION)); WriteErrorLog(tempstring);
	sprintf(tempstring, "Extensions: %s", glGetString(GL_EXTENSIONS)); WriteErrorLog(tempstring);

	glClearColor(0.0, 0.0, 0.0, 0.0);
		
	glShadeModel(GL_SMOOTH);

//	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
	
	mytext.Load("fonts\\font.tga");     // load the font image
	menuInit();
		

	glClearDepth(1.0f);					// Depth Buffer Setup	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);				// The Type Of Depth Testing To Do

	glBlendFunc(GL_SRC_ALPHA,GL_ONE);	// Blending Function For Translucency Based On Source Alpha Value

	glCullFace(GL_BACK);

	glViewport(0, 0, winWidth, winHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat) winWidth/(GLfloat) winHeight, 1.0, 3000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glFogi(GL_FOG_MODE, GL_EXP2);
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, 0.001f);
	glFogf(GL_FOG_START, 19000.0f);
	glFogf(GL_FOG_END, 20000.0f);
	glHint(GL_FOG_HINT, GL_NICEST);

    glDisable(GL_LINE_SMOOTH);
    glDisable(GL_POINT_SMOOTH);

	if (SCREEN_LORES==0)
		texScale=128;
	else
		texScale=128;



#endif
}






//============================================================================
// Chooses the renderer to use
//============================================================================
void initglide(void)
{ 
		init_gl();
}



//============================================================================
// Error handler for MIDAS"errors
//============================================================================
void MIDASerror(void)
{

   FSOUND_Close();
    exit(EXIT_FAILURE);
}



void ExitWithError(char *errormsg)
{
	WriteErrorLog(errormsg);
	MessageBox( NULL, errormsg, "Critical Error", MB_ICONSTOP);
	exit (1);
}


//============================================================================
// Initialize sound, open channel, load global samples
//============================================================================
void InitSound(HWND hWnd)
{
	int i;
	if (use_midas!=1)
		return;

	for (i=0; i<55; i++)
	{
		curSoundFactor[i]=0.0f;
		prevSoundFactor[i]=100.0f;
		SoundFactor[i]=0.0f;
		curIntSoundFactor[i]=0;
		prevIntSoundFactor[i]=100;
	}

	for (i=0; i<400; i++)
	{
		Noise[i].isRunning=0;
	}


    /* Initialize MIDAS: */

FSOUND_SetOutput(FSOUND_OUTPUT_DSOUND);
FSOUND_SetDriver(0);
FSOUND_SetMixer(FSOUND_MIXER_AUTODETECT | FSOUND_MIXER_QUALITY_AUTODETECT);
FSOUND_SetHWND(hWnd);

	if (!FSOUND_Init(44100, 128, 0))
	{
		WriteErrorLog("No sound available!");
		use_midas=0;
		SOUND_ENABLED=0;
		return;
	}


	samples[0] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\EngineB.wav", FSOUND_LOOP_NORMAL | FSOUND_16BITS | FSOUND_STEREO, 0);
	if (!samples[0])
		ExitWithError("Couldn't load EngineB.wav");
	samples[1] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\EngineA.wav", FSOUND_LOOP_NORMAL | FSOUND_16BITS | FSOUND_STEREO, 0);
	if (!samples[1])
		ExitWithError("Couldn't load Enginea.wav");
	samples[2] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\Internal.wav", FSOUND_LOOP_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[2])
		exit(1);
	samples[3] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\fire.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[3])
		ExitWithError("Couldn't load fire.wav");
	samples[4] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\missile.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[4])
		ExitWithError("Couldn't load missile.wav");
	samples[5] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\takeoff.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[5])
		ExitWithError("Couldn't load takeoff.wav");
	samples[6] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\landing.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[6])
		ExitWithError("Couldn't load landing.wav");
	samples[7] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\geardn.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[7])
		ExitWithError("Couldn't load geardn.wav");
	samples[8] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\gearup.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[8])
		ExitWithError("Couldn't load gearup.wav");
	samples[9] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\missileblast.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[9])
		ExitWithError("Couldn't load missileblast.wav");
	samples[10] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\laserblast.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[10])
		ExitWithError("Couldn't load laserblast.wav");
	samples[11] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\shipdestroyed.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[11])
		ExitWithError("Couldn't load shipdestroyed.wav");
	samples[12] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\buildingdestroyed.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_STEREO, 0);
	if (!samples[12])
		ExitWithError("Couldn't load buildingdestroyed.wav");
	samples[13] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\shiphitl.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[13])
		ExitWithError("Couldn't load shiphitl.wav");
	samples[14] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\shiphitm.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[14])
		ExitWithError("Couldn't load shiphitm.wav");
	samples[15] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\buildinghitl.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[15])
		ExitWithError("Couldn't load buildinghitl.wav");
	samples[16] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\buildinghitm.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_STEREO, 0);
	if (!samples[16])
		ExitWithError("Couldn't load buildinghitm.wav");
	samples[17] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\lock.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[17])
		ExitWithError("Couldn't load lock.wav");
	samples[18] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\stall.wav", FSOUND_LOOP_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[18])
		ExitWithError("Couldn't load stall.wav");
	samples[19] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\dive.wav", FSOUND_LOOP_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[19])
		ExitWithError("Couldn't load dive.wav");
	samples[20] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\shiphitground.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[20])
		ExitWithError("Couldn't load shiphitground.wav");
	samples[21] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\satellite.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_STEREO, 0);
	if (!samples[21])
		ExitWithError("Couldn't load satellite.wav");
	samples[22] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\satblast.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[22])
		ExitWithError("Couldn't load satblast.wav");
	samples[23] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\growlock.wav", FSOUND_LOOP_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[23])
		ExitWithError("Couldn't load growlock.wav");
	samples[24] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\basefire.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_STEREO, 0);
	if (!samples[24])
		ExitWithError("Couldn't load basefire.wav");
	samples[25] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\basebackup.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_STEREO, 0);
	if (!samples[25])
		ExitWithError("Couldn't load basebackup.wav");
	samples[26] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\event_thunder1.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[26])
		ExitWithError("Couldn't load event_thunder1.wav");
	samples[27] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\event_thunder2.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[27])
		ExitWithError("Couldn't load event_thunder2.wav");
	samples[28] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\event_thunder3.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[28])
		ExitWithError("Couldn't load event_thunder3.wav");
	samples[29] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\ambient_rain.wav", FSOUND_LOOP_NORMAL | FSOUND_16BITS | FSOUND_MONO| FSOUND_2D, 0);
	if (!samples[29])
		ExitWithError("Couldn't load ambient_rain.wav");
	samples[30] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\menu_mon.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[30])
		ExitWithError("Couldn't load menu_mon.wav");
	samples[31] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\menu_sel.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[31])
		ExitWithError("Couldn't load menu_sel.wav");
	samples[32] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\menu_flip.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[32])
		ExitWithError("Couldn't load menu_flip.wav");
	samples[33] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\menu_enter.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[33])
		ExitWithError("Couldn't load menu_enter.wav");
	samples[34] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\weaponchange.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[34])
		ExitWithError("Couldn't load weaponchange.wav");
	samples[35] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\na.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[35])
		ExitWithError("Couldn't load na.wav");
	samples[36] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\radarlock.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[36])
		ExitWithError("Couldn't load radarlock.wav");
	samples[37] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\event_thunder4.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[37])
		ExitWithError("Couldn't load event_thunder4.wav");
	samples[38] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\event_thunder5.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[38])
		ExitWithError("Couldn't load event_thunder5.wav");
	samples[39] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\event_thunder6.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[39])
		ExitWithError("Couldn't load event_thunder6.wav");
	samples[40] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\event_thunder7.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[40])
		ExitWithError("Couldn't load event_thunder7.wav");
	samples[41] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\event_thunder8.wav", FSOUND_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[41])
		ExitWithError("Couldn't load event_thunder8.wav");
	samples[42] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\ambient_summer.wav", FSOUND_LOOP_NORMAL | FSOUND_16BITS | FSOUND_MONO| FSOUND_2D, 0);
	if (!samples[42])
		ExitWithError("Couldn't load ambient_summer.wav");
	samples[43] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\ambient_wind.wav", FSOUND_LOOP_NORMAL | FSOUND_16BITS | FSOUND_MONO| FSOUND_2D, 0);
	if (!samples[43])
		ExitWithError("Couldn't load ambient_wind.wav");
	samples[44] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\ambient_windhighalt.wav", FSOUND_LOOP_NORMAL | FSOUND_16BITS | FSOUND_MONO| FSOUND_2D, 0);
	if (!samples[44])
		ExitWithError("Couldn't load ambient_windhighalt.wav");
	samples[45] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\ambient_rainlight.wav", FSOUND_LOOP_NORMAL | FSOUND_16BITS | FSOUND_MONO| FSOUND_2D, 0);
	if (!samples[45])
		ExitWithError("Couldn't load ambient_rainlight.wav");
	samples[46] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\ambient_cricketsnight.wav", FSOUND_LOOP_NORMAL | FSOUND_16BITS | FSOUND_MONO| FSOUND_2D, 0);
	if (!samples[46])
		ExitWithError("Couldn't load ambient_cricketsnight.wav");
	samples[47] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\ambient_crickets.wav", FSOUND_LOOP_NORMAL | FSOUND_16BITS | FSOUND_MONO| FSOUND_2D, 0);
	if (!samples[47])
		ExitWithError("Couldn't load ambient_crickets.wav");
	samples[48] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\loop_amb1.wav", FSOUND_LOOP_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[48])
		ExitWithError("Couldn't load loop_amb1.wav");
	samples[49] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\loop_amb2.wav", FSOUND_LOOP_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[49])
		ExitWithError("Couldn't load loop_amb2.wav");
	samples[50] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\loop_amb3.wav", FSOUND_LOOP_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[50])
		ExitWithError("Couldn't load loop_amb3.wav");
	samples[51] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\loop_amb4.wav", FSOUND_LOOP_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[51])
		ExitWithError("Couldn't load loop_amb4.wav");
	samples[52] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\loop_ship.wav", FSOUND_LOOP_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[52])
		ExitWithError("Couldn't load loop_ship.wav");
	samples[53] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\loop_tank.wav", FSOUND_LOOP_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[53])
		ExitWithError("Couldn't load loop_tank.wav");
	samples[54] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\loop_missile.wav", FSOUND_LOOP_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[54])
		ExitWithError("Couldn't load loop_missile.wav");
	samples[55] = FSOUND_Sample_Load(FSOUND_FREE, "sounds\\loop_tankroll.wav", FSOUND_LOOP_NORMAL | FSOUND_16BITS | FSOUND_MONO, 0);
	if (!samples[55])
		ExitWithError("Couldn't load loop_tankroll.wav");


	maxsamples=55;
}



//============================================================================
// Close Sound system, free channels and samples
//============================================================================
void ShutdownSound(void)
{
	if (use_midas!=1)
		return;
//	for (tempint=0;tempint<=maxsamples;tempint++)
//		FSOUND_Sample_Free(samples[tempint]);

		FSOUND_Close();

}



//============================================================================
// Start playing a module
//============================================================================
void StartModuleMusic(void)
{
	if (use_midas!=1)
		return;
	FMUSIC_PlaySong(module);

}


//============================================================================
// Load (and play) a module
//============================================================================
void LoadModuleMusic(char *filename)
{
	if (use_midas!=1)
		return;

	module = FMUSIC_LoadSong(filename);

	if (!module)
	{
		sprintf(tempstring, "LoadModuleMusic(): Error loading file %s", filename);
		WriteErrorLog(tempstring);
		exit(1);
	}
	StartModuleMusic();
	FMUSIC_SetMasterVolume(module, 128);
}


//============================================================================
// Free module memory
//============================================================================
void UnloadModuleMusic(void)
{
	if (use_midas!=1)
		return;
	FMUSIC_FreeSong(module);
}


//============================================================================
// Start playing streamed WAV data
//============================================================================
void LoadStreamMusic(char *filename)
{
	if (use_midas!=1)
		return;

//	stream1 = FSOUND_Stream_OpenWav(filename, FSOUND_LOOP_NORMAL | FSOUND_NORMAL);
	stream1 = FSOUND_Stream_OpenFile(filename, FSOUND_LOOP_OFF | FSOUND_NORMAL,0);
	if (!stream1)
	{
		sprintf(tempstring, "LoadStreamMusic(): Error loading file %s", filename);
		WriteErrorLog(tempstring);
		exit(1);
	}
	channels[127] = FSOUND_Stream_Play(FSOUND_FREE, stream1);


}



//============================================================================
// Global command, stops ALL kinds of music currently playing (module & stream)
//============================================================================
void StopModuleMusic(void)
{
	if (use_midas!=1)
		return;
	if (FMUSIC_IsPlaying(module))
		FMUSIC_StopSong(module);
	UnloadModuleMusic();
}

void StopStreamMusic(void)
{
	if (use_midas!=1)
		return;
	FSOUND_Stream_Stop(stream1);
	FSOUND_Stream_Close(stream1);
}



int AddNoiseEmitter(int AttachedToObject, int SampleID, float xPos, float zPos, float Height, float maxSoundDist)
{
  int i=-1;

	if (use_midas!=1)
		return -1;

	do {
		i++;
		if (i>490)
			return -1;
	} while (Noise[i].isActive==1);


	Noise[i].isActive = 1;
	Noise[i].isRunning = 0;
	Noise[i].MaxAudibleDistance = maxSoundDist;
	Noise[i].SampleID = SampleID;
	Noise[i].x = xPos;
	Noise[i].y = Height;
	Noise[i].z = zPos;
	Noise[i].AttachedToObject = AttachedToObject;
	Noise[i].ChannelID=0;
	if (i==NumNoises)
	{
		NumNoises++;
		Noise[NumNoises].isActive = 0;
	}

	//StartSound
	Noise[i].ChannelID = FSOUND_PlaySound(FSOUND_FREE, samples[Noise[i].SampleID]);

	return (i);
}

void RemoveNoiseEmitter(int EmitterID)
{

if (use_midas!=1)
	return;
	
	Noise[EmitterID].isActive = 0;
	if (FSOUND_IsPlaying(Noise[EmitterID].ChannelID))
		FSOUND_StopSound(Noise[EmitterID].ChannelID);


	do {
		if (NumNoises>0)
		if ((Noise[NumNoises-1].isActive != 1))
		{
			Noise[NumNoises-1].isActive = 0;
			NumNoises--;
		}
		else
			break;
	} while (Noise[NumNoises-1].isActive != 1);


}

void MuteObject(int ObjectID)
{
int CurEmitter;

  for (CurEmitter=0; CurEmitter<NumNoises; CurEmitter++)
  {
	if (Noise[CurEmitter].isActive)
		if (Noise[CurEmitter].AttachedToObject == ObjectID)
			RemoveNoiseEmitter(CurEmitter);
  }
}



//============================================================================
// Handler for all the looping 3d sounds in the game
//============================================================================
void NoiseEmitterHandler(void)
{
int CurEmitter;
float tempPos[3];

if (use_midas!=1)
	return;


  for (CurEmitter=0; CurEmitter<NumNoises; CurEmitter++)
	if (Noise[CurEmitter].isActive)
	{

		//Move with attached Object
		if (Noise[CurEmitter].AttachedToObject >= 0)
		{
			Noise[CurEmitter].x = Objects[Noise[CurEmitter].AttachedToObject].xPos + (float)(sin((Objects[-Noise[CurEmitter].AttachedToObject].Yaw+180.0f) * DEGREE)*cos(Objects[Noise[CurEmitter].AttachedToObject].Pitch * DEGREE));
			Noise[CurEmitter].y = Objects[Noise[CurEmitter].AttachedToObject].Height - (float)(sin(Objects[Noise[CurEmitter].AttachedToObject].Pitch * DEGREE));
			Noise[CurEmitter].z = Objects[Noise[CurEmitter].AttachedToObject].zPos - (float)(cos((Objects[-Noise[CurEmitter].AttachedToObject].Yaw+180.0f) * DEGREE)*cos(Objects[Noise[CurEmitter].AttachedToObject].Pitch * DEGREE));
		}
		else if (Noise[CurEmitter].AttachedToObject == -1)
		{
			Noise[CurEmitter].x = CamXPos + (float)(sin(CamYaw * DEGREE)*cos(CamPitch * DEGREE));
			Noise[CurEmitter].y = CamHeight - (float)(sin(CamPitch * DEGREE));
			Noise[CurEmitter].z = CamZPos - (float)(cos(CamYaw * DEGREE)*cos(CamPitch * DEGREE));
		}
		
		//StartSound
//		if (!FSOUND_IsPlaying(Noise[CurEmitter].ChannelID))
//		    Noise[CurEmitter].ChannelID = FSOUND_PlaySound(FSOUND_FREE, samples[Noise[CurEmitter].SampleID]);


		if (FSOUND_IsPlaying(Noise[CurEmitter].ChannelID))
		{

		  tempPos[0] = Noise[CurEmitter].x;
		  tempPos[1] = -Noise[CurEmitter].y;
		  tempPos[2] = Noise[CurEmitter].z;
		  FSOUND_3D_SetAttributes(Noise[CurEmitter].ChannelID, tempPos, NULL);


		  //Engine whine changes pitch according to speed
		  if (Noise[CurEmitter].AttachedToObject != -1)
		  if ((Objects[Noise[CurEmitter].AttachedToObject].ObjectMesh == OBJECTMESH_SHIP) ||
			(Objects[Noise[CurEmitter].AttachedToObject].ObjectMesh == OBJECTMESH_LIGHTTANK) ||
			(Objects[Noise[CurEmitter].AttachedToObject].ObjectMesh == OBJECTMESH_MEDIUMTANK) ||
			(Objects[Noise[CurEmitter].AttachedToObject].ObjectMesh == OBJECTMESH_SHIP1) ||
			(Objects[Noise[CurEmitter].AttachedToObject].ObjectMesh == OBJECTMESH_SHIP2) ||
			(Objects[Noise[CurEmitter].AttachedToObject].ObjectMesh == OBJECTMESH_SHIP3))
		  {
			if (Player[localplayer].ControlledObject == Noise[CurEmitter].AttachedToObject)
			{
				if (mySpeed>0.0f)
				{
					FSOUND_SetFrequency(Noise[CurEmitter].ChannelID, (int)(2.0f*(16050.0f+ (4000.0f*(float)(mySpeed)))));
					FSOUND_SetVolume(Noise[CurEmitter].ChannelID, 255);
				}
				else
					FSOUND_SetVolume(Noise[CurEmitter].ChannelID, 0);
			}
			else
			{
				if (Objects[Noise[CurEmitter].AttachedToObject].Speed>0.0f)
				{
					FSOUND_SetFrequency(Noise[CurEmitter].ChannelID, (int)(2.0f*(16050.0f+ (4000.0f*(float)(Objects[Noise[CurEmitter].AttachedToObject].Speed)))));
					FSOUND_SetVolume(Noise[CurEmitter].ChannelID, 255);
				}
				else
					FSOUND_SetVolume(Noise[CurEmitter].ChannelID, 0);
			}
		  }
		}
		else
		  RemoveNoiseEmitter(CurEmitter);




	}
}





//============================================================================
// Play a sound with correct volume/panning according to sound source position
// Similar to the SoundEmitterHandler stuff, but for short, fixed sounds (like thunder)
//============================================================================
void PlayPositionalSound(int sampleNum, int handle, float xPos, float zPos, float Height, float maxSoundDist)
{
//float maxSoundDist = 1400;
	if (use_midas!=1)
		return;

AddNoiseEmitter(-1, sampleNum, xPos, zPos, Height, maxSoundDist);

}

//============================================================================
// Stops a running sound
//============================================================================
void StopSoundFX(int handle)
{
	if (use_midas!=1)
		return;

	sprintf(tempstring, "StopSoundFX(%i)", handle);
	WriteErrorLog(tempstring);

	if (handle>=0)
		if (FSOUND_IsPlaying(Noise[channelEmitter[handle]].ChannelID))
			FSOUND_StopSound(Noise[channelEmitter[handle]].ChannelID);
//		if (FSOUND_IsPlaying(channels[handle]))
			RemoveNoiseEmitter(channelEmitter[handle]);
	channels[handle]=-1;

}


//============================================================================
// Play a sound using automatic channels
//============================================================================
void PlaySoundFX(int sampleNum, int handle)
{

//	float nullarray[] = {0.0f, 0.0f, 0.0f};

	if (use_midas!=1)
		return;
	sprintf(tempstring, "PlaySoundFX(%i, %i)", sampleNum, handle);
	WriteErrorLog(tempstring);

//    channels[handle] = FSOUND_PlaySound(FSOUND_FREE, samples[sampleNum]);

	channelEmitter[handle] = AddNoiseEmitter(-1, sampleNum, CamXPos, CamZPos, CamHeight, 200.0f);
	channels[handle] = Noise[channelEmitter[handle]].ChannelID;
//	FSOUND_3D_SetAttributes(channels[handle],nullarray,NULL);

//	if ((sampleNum==0) || (sampleNum==1) || (sampleNum==12) || (sampleNum==16))
//		FSOUND_SetPan(channels[handle], FSOUND_STEREOPAN);

}




/*
//============================================================================
// Stops a running sound
//============================================================================
void StopSoundFX(int handle)
{
	if (use_midas!=1)
		return;
	if (channels[handle]!=-1)
		if (FSOUND_IsPlaying(channels[handle]))
			FSOUND_StopSound(channels[handle]);
	channels[handle]=-1;

}


//============================================================================
// Play a sound using automatic channels
//============================================================================
void PlaySoundFX(int sampleNum, int handle)
{
if (use_midas!=1)
	return;

    channels[handle] = FSOUND_PlaySound(FSOUND_FREE, samples[sampleNum]);
	if ((sampleNum==0) || (sampleNum==1) || (sampleNum==12) || (sampleNum==16))
		FSOUND_SetPan(channels[handle], FSOUND_STEREOPAN);

}



int AddNoiseEmitter(int AttachedToObject, int SampleID, float xPos, float zPos, float Height, float maxSoundDist)
{
  int i=-1;

	if (use_midas!=1)
		return -1;

	do {
		i++;
		if (i>490)
			return -1;
	} while (Noise[i].isActive==1);

	sprintf(tempstring, "- AddNoiseEmitter %i", i);
	WriteErrorLog(tempstring);

	Noise[i].isActive = 1;
	Noise[i].isRunning = 0;
	Noise[i].MaxAudibleDistance = maxSoundDist;
	Noise[i].SampleID = SampleID;
	Noise[i].x = xPos;
	Noise[i].y = Height;
	Noise[i].z = zPos;
	Noise[i].AttachedToObject = AttachedToObject;
	Noise[i].ChannelID=0;
	if (i==NumNoises)
	{
		NumNoises++;
		Noise[NumNoises].isActive = 0;
	}
	return (i);
//	WriteErrorLog("...done");
}

void RemoveNoiseEmitter(int EmitterID)
{

if (use_midas!=1)
	return;
	
	sprintf(tempstring, "- RemoveNoiseEmitter %i", EmitterID);
	WriteErrorLog(tempstring);


	Noise[EmitterID].isActive = 0;
	if (Noise[EmitterID].isRunning==1)
		if (FSOUND_IsPlaying(Noise[EmitterID].ChannelID))
			FSOUND_StopSound(Noise[EmitterID].ChannelID);


	do {
		if (NumNoises>0)
		if ((Noise[NumNoises-1].isActive != 1))
		{
			Noise[NumNoises-1].isActive = 0;
			NumNoises--;
		}
		else
			break;
	} while (Noise[NumNoises-1].isActive != 1);

//	WriteErrorLog("...done");

}

void MuteObject(int ObjectID)
{
int CurEmitter;
	sprintf(tempstring, "- MuteObject %i", ObjectID);
	WriteErrorLog(tempstring);

  for (CurEmitter=0; CurEmitter<NumNoises; CurEmitter++)
  {
	if (Noise[CurEmitter].isActive)
		if (Noise[CurEmitter].AttachedToObject == ObjectID)
			RemoveNoiseEmitter(CurEmitter);
  }
}



//============================================================================
// Handler for all the looping 3d sounds in the game
//============================================================================
void NoiseEmitterHandler(void)
{
int CurEmitter;

if (use_midas!=1)
	return;

  WriteErrorLog("- NoiseEmitterHandler:");

  sprintf(tempstring, "NumNoises=%i", NumNoises);
  WriteErrorLog(tempstring);

  for (CurEmitter=0; CurEmitter<NumNoises; CurEmitter++)
	if (Noise[CurEmitter].isActive)
	{

		sprintf(tempstring, "Noise[%i].AttachedToObject=%i", CurEmitter, Noise[CurEmitter].AttachedToObject);
		WriteErrorLog(tempstring);
		sprintf(tempstring, "Noise[%i].ChannelID=%i", CurEmitter, Noise[CurEmitter].ChannelID);
		WriteErrorLog(tempstring);
		sprintf(tempstring, "Noise[%i].isActive=%i", CurEmitter, Noise[CurEmitter].isActive);
		WriteErrorLog(tempstring);
		sprintf(tempstring, "Noise[%i].isRunning=%i", CurEmitter, Noise[CurEmitter].isRunning);
		WriteErrorLog(tempstring);
		sprintf(tempstring, "Noise[%i].MaxAudibleDistance=%f", CurEmitter, Noise[CurEmitter].MaxAudibleDistance);
		WriteErrorLog(tempstring);
		sprintf(tempstring, "Noise[%i].SampleID=%i", CurEmitter, Noise[CurEmitter].SampleID);
		WriteErrorLog(tempstring);
		sprintf(tempstring, "Noise[%i].x=%d", CurEmitter, Noise[CurEmitter].x);
		WriteErrorLog(tempstring);
		sprintf(tempstring, "Noise[%i].y=%d", CurEmitter, Noise[CurEmitter].y);
		WriteErrorLog(tempstring);
		sprintf(tempstring, "Noise[%i].z=%d", CurEmitter, Noise[CurEmitter].z);
		WriteErrorLog(tempstring);


		WriteErrorLog("Move Noise...");
		//Move with attached Object
		if (Noise[CurEmitter].AttachedToObject != -1)
		{
			Noise[CurEmitter].x = (float)Objects[Noise[CurEmitter].AttachedToObject].xPos;
			Noise[CurEmitter].y = (float)Objects[Noise[CurEmitter].AttachedToObject].Height;
			Noise[CurEmitter].z = (float)Objects[Noise[CurEmitter].AttachedToObject].zPos;
		}
		
		GetRelativeAngle( Noise[CurEmitter].x, Noise[CurEmitter].z, Noise[CurEmitter].y,
						  CamXPos, CamZPos, CamHeight );
		if (TempDist<Noise[CurEmitter].MaxAudibleDistance)
		{
		  //Restart sound if it came in range
		  if (Noise[CurEmitter].isRunning != 1)
		  {
			WriteErrorLog("Restart Sound");
			Noise[CurEmitter].isRunning = 1;
		    Noise[CurEmitter].ChannelID = FSOUND_PlaySound(FSOUND_FREE, samples[Noise[CurEmitter].SampleID]);
		  }

		  WriteErrorLog("Calc Panning");

		  //Calculate panning
		  TempYaw=FixAngle(TempYaw/(2.0f*pi)*360.0f);
		  TempYaw=CamYaw-TempYaw;
		  TempYaw=FixAngle(TempYaw);
		  TempYaw=256.0f-TempYaw/360.0f*256.0f;
		  if (TempYaw>128.0f)
			TempYaw=256.0f-TempYaw;
		  TempYaw = TempYaw * 2.0f;
		  FSOUND_SetPan(Noise[CurEmitter].ChannelID, (int)(TempYaw));
		  FSOUND_SetVolume(Noise[CurEmitter].ChannelID, (int)(255.0f*(float)((Noise[CurEmitter].MaxAudibleDistance-TempDist)/Noise[CurEmitter].MaxAudibleDistance)));

		  WriteErrorLog("Set Pitch");

		  //Engine whine changes pitch according to speed
		  if (Noise[CurEmitter].AttachedToObject != -1)
		  if ((Objects[Noise[CurEmitter].AttachedToObject].ObjectMesh == OBJECTMESH_SHIP) ||
			(Objects[Noise[CurEmitter].AttachedToObject].ObjectMesh == OBJECTMESH_LIGHTTANK) ||
			(Objects[Noise[CurEmitter].AttachedToObject].ObjectMesh == OBJECTMESH_MEDIUMTANK) ||
			(Objects[Noise[CurEmitter].AttachedToObject].ObjectMesh == OBJECTMESH_SHIP1) ||
			(Objects[Noise[CurEmitter].AttachedToObject].ObjectMesh == OBJECTMESH_SHIP2) ||
			(Objects[Noise[CurEmitter].AttachedToObject].ObjectMesh == OBJECTMESH_SHIP3))
		  {
			if (Player[localplayer].ControlledObject == Noise[CurEmitter].AttachedToObject)
			    FSOUND_SetFrequency(Noise[CurEmitter].ChannelID, (int)(2.0f*(16050.0f+ (4000.0f*(float)(mySpeed)))));
			else
				FSOUND_SetFrequency(Noise[CurEmitter].ChannelID, (int)(2.0f*(16050.0f+ (4000.0f*(float)(Objects[Noise[CurEmitter].AttachedToObject].Speed)))));
		  }

		}

		//Stop sound once we get out of audible range
		if ((TempDist>=Noise[CurEmitter].MaxAudibleDistance) && (Noise[CurEmitter].isRunning == 1))
		{
			WriteErrorLog("Stop Noise");
			Noise[CurEmitter].isRunning=0;
			FSOUND_StopSound(Noise[CurEmitter].ChannelID);
		}



	}
}





//============================================================================
// Play a sound with correct volume/panning according to sound source position
// Similar to the SoundEmitterHandler stuff, but for short, fixed sounds (like thunder)
//============================================================================
void PlayPositionalSound(int sampleNum, int handle, float xPos, float zPos, float Height, float maxSoundDist)
{
//float maxSoundDist = 1400;
	if (use_midas!=1)
		return;
	GetRelativeAngle( xPos, zPos, Height,
					  CamXPos, CamZPos, CamHeight );
	if (TempDist<maxSoundDist)
	{
		TempYaw=FixAngle(TempYaw/(2.0f*pi)*360.0f);
		TempYaw=CamYaw-TempYaw;
		TempYaw=FixAngle(TempYaw);

		TempYaw=256.0f-TempYaw/360.0f*256.0f;
		if (TempYaw>128.0f)
			TempYaw=256.0f-TempYaw;
		TempYaw;

	    channels[handle] = FSOUND_PlaySound(FSOUND_FREE, samples[sampleNum]);
		FSOUND_SetPan(channels[handle], (int)(TempYaw));
		FSOUND_SetVolume(channels[handle], (int)(255.0f*(float)((maxSoundDist-TempDist)/maxSoundDist)));
	}
}
*/


//============================================================================
// Shutdown Sound system, free samples... This is the MAIN sound shutdown code
//============================================================================
void DRCloseSound(void)
{
	if ((use_midas!=0) && (midas_running==1))
	{
		//Stop our jukebox
		if (playlist[CurrentSong].type==MUSIC_MOD)
			StopModuleMusic();
		if (playlist[CurrentSong].type==MUSIC_MP3)
			StopStreamMusic();

//		for (i=0; i<127; i++)
//			if (FSOUND_IsPlaying(channels[i]))
//				StopSoundFX(channels[i]);

//		for (i=0; i<NumNoises; i++)
//		if (Noise[i].isActive)
//			RemoveNoiseEmitter(i);

		ShutdownSound();
		midas_running=0;
	}
}



//=============================================================================
// Startup Sound system, load samples, etc. This is the MAIN sound startup code
//=============================================================================
void DRStartSound(HWND hWnd)
{
	if ((use_midas!=0) && (midas_running==0))
	{
		InitSound(hWnd);

		Noise[0].isActive=0;
		Noise[0].isRunning=0;

		FSOUND_3D_Listener_SetDistanceFactor(100.0f);
		FSOUND_3D_Listener_SetRolloffFactor(0.06f);


//		PlaySoundFX(0,0);	//Start engine sounds
//		PlaySoundFX(1,1);
//		PlaySoundFX(2,2);

//		FSOUND_SetVolume(channels[0],0);
//		FSOUND_SetVolume(channels[1],0);
//		FSOUND_SetVolume(channels[2],0);

// Start playin' music
		if (MaxSongs>0)
		{
			ModuleIsFresh=1;
			CurrentSong = random(MaxSongs);
			if (CurrentSong>=MaxSongs)
				CurrentSong=0;
			if (playlist[CurrentSong].type==MUSIC_MOD)
				LoadModuleMusic(playlist[CurrentSong].filename);
			if (playlist[CurrentSong].type==MUSIC_MP3)
				LoadStreamMusic(playlist[CurrentSong].filename);
//		MusicHandler(FXTRUE);
		}


		midas_running=1;
	}
}



//============================================================================
// Music Handler responsible for keeping the jukebox playin'
//============================================================================
void MusicHandler(FxBool ForceRestart)
{
	int RestartPlaying=0;
	int NextSong;

	if (ForceRestart)
		RestartPlaying=1;

	if ((use_midas!=1) || (MaxSongs<=0))
		return;
	if (!ForceRestart)
	//Check if the current song is finished
	if (playlist[CurrentSong].type==MUSIC_MOD)
	{
		if ( (FMUSIC_GetOrder(module)>1) && (ModuleIsFresh==1) && (FMUSIC_IsPlaying(module)) )
			ModuleIsFresh=0;
		if ( ((FMUSIC_GetOrder(module)==1)&&(ModuleIsFresh==0)) || (!FMUSIC_IsPlaying(module)) )
		{
			StopModuleMusic();
			ModuleIsFresh=1;
			RestartPlaying=1;
		}
	}
	else if (playlist[CurrentSong].type==MUSIC_MP3)
	{
		if ( (FSOUND_Stream_GetPosition(stream1)==0) || (FSOUND_Stream_GetPosition(stream1)==FSOUND_Stream_GetLength(stream1)) )
		{
			StopStreamMusic();
			RestartPlaying=1;
		}
	}

	//The current song has finished, so start the new one!
	if (RestartPlaying == 1)
	{
		do 
		{ 
			NextSong = random(MaxSongs);
		} while (NextSong==CurrentSong);
		CurrentSong = NextSong;
		if (CurrentSong>=MaxSongs)
			CurrentSong=0;
		if (playlist[CurrentSong].type==MUSIC_MOD)
		{
			LoadModuleMusic(playlist[CurrentSong].filename);
			ModuleIsFresh=1;
		}
		if (playlist[CurrentSong].type==MUSIC_MP3)
			LoadStreamMusic(playlist[CurrentSong].filename);

		sprintf(tempstring, "Music: %s", playlist[CurrentSong].displayname);
		AddMessage(tempstring, 255,255,255);
		startFrameTime = TimerGetTime();
	}
}




//============================================================================
// Look into our jukebox's cabin too see what music we got...
//============================================================================
void InitPlaylist(void)
{
	WIN32_FIND_DATA		fd;
	HANDLE			hd;
	char			FileLoc[256];


	
	
	


	if (use_midas!=1)
		return;

	//First, take all MP3 files
	sprintf(FileLoc, "music\\*.mp3");
	hd = FindFirstFile(FileLoc, &fd);
	if (hd != INVALID_HANDLE_VALUE)
	{
		sprintf(playlist[MaxSongs].filename, "music\\%s\0", fd.cFileName);
		sprintf(playlist[MaxSongs].displayname, "%s\0", fd.cFileName);
		playlist[MaxSongs].type=MUSIC_MP3;
		MaxSongs++;

		while (FindNextFile(hd, &fd))
		{
			sprintf(playlist[MaxSongs].filename, "music\\%s\0", fd.cFileName);
			sprintf(playlist[MaxSongs].displayname, "%s\0", fd.cFileName);
			playlist[MaxSongs].type=MUSIC_MP3;
			MaxSongs++;
		}
		FindClose(hd);
	}

	//Second, take all IT files
	sprintf(FileLoc, "music\\*.it");
	hd = FindFirstFile(FileLoc, &fd);
	if (hd != INVALID_HANDLE_VALUE)
	{
		sprintf(playlist[MaxSongs].filename, "music\\%s\0", fd.cFileName);
		sprintf(playlist[MaxSongs].displayname, "%s\0", fd.cFileName);
		playlist[MaxSongs].type=MUSIC_MOD;
		MaxSongs++;

		while (FindNextFile(hd, &fd))
		{
			sprintf(playlist[MaxSongs].filename, "music\\%s\0", fd.cFileName);
			sprintf(playlist[MaxSongs].displayname, "%s\0", fd.cFileName);
			playlist[MaxSongs].type=MUSIC_MOD;
			MaxSongs++;
		}
		FindClose(hd);
	}

	//Third, take all XM files
	sprintf(FileLoc, "music\\*.xm");
	hd = FindFirstFile(FileLoc, &fd);
	if (hd != INVALID_HANDLE_VALUE)
	{
		sprintf(playlist[MaxSongs].filename, "music\\%s\0", fd.cFileName);
		sprintf(playlist[MaxSongs].displayname, "%s\0", fd.cFileName);
		playlist[MaxSongs].type=MUSIC_MOD;
		MaxSongs++;

		while (FindNextFile(hd, &fd))
		{
			sprintf(playlist[MaxSongs].filename, "music\\%s\0", fd.cFileName);
			sprintf(playlist[MaxSongs].displayname, "%s\0", fd.cFileName);
			playlist[MaxSongs].type=MUSIC_MOD;
			MaxSongs++;
		}
		FindClose(hd);
	}

	//Ok ok, S3M's are cool too
	sprintf(FileLoc, "music\\*.s3m");
	hd = FindFirstFile(FileLoc, &fd);
	if (hd != INVALID_HANDLE_VALUE)
	{
		sprintf(playlist[MaxSongs].filename, "music\\%s\0", fd.cFileName);
		sprintf(playlist[MaxSongs].displayname, "%s\0", fd.cFileName);
		playlist[MaxSongs].type=MUSIC_MOD;
		MaxSongs++;

		while (FindNextFile(hd, &fd))
		{
			sprintf(playlist[MaxSongs].filename, "music\\%s\0", fd.cFileName);
			sprintf(playlist[MaxSongs].displayname, "%s\0", fd.cFileName);
			playlist[MaxSongs].type=MUSIC_MOD;
			MaxSongs++;
		}
		FindClose(hd);
	}

	//Yeah, MODs too.
	sprintf(FileLoc, "music\\*.mod");
	hd = FindFirstFile(FileLoc, &fd);
	if (hd != INVALID_HANDLE_VALUE)
	{
		sprintf(playlist[MaxSongs].filename, "music\\%s\0", fd.cFileName);
		sprintf(playlist[MaxSongs].displayname, "%s\0", fd.cFileName);
		playlist[MaxSongs].type=MUSIC_MOD;
		MaxSongs++;

		while (FindNextFile(hd, &fd))
		{
			sprintf(playlist[MaxSongs].filename, "music\\%s\0", fd.cFileName);
			sprintf(playlist[MaxSongs].displayname, "%s\0", fd.cFileName);
			playlist[MaxSongs].type=MUSIC_MOD;
			MaxSongs++;
		}
		FindClose(hd);
	}
	
}






//Satellite Plasma Cannon
void SatelliteBlast(float xPos, float zPos, float Height)
{
float curHeight;
float maxHeight = 1500; //Max. height of plasma beam
int newemitter;

	for (curHeight=-Height; curHeight<maxHeight; curHeight+=20)
	{
		//Yellow beam
		newemitter = AddParticleEmitter(-1, 1, 50, 1, -1, 0.0f,
		-xPos, -zPos, -curHeight, //Position
		0.0f, +90.0f,		//Yaw, Pitch
		2, 0.0f, 0.0f, 0.0f, 500.0f, 2000.0f, 0.0f,
		100, 255, 0, 33);//Yellow-Green plasma laser
		ParticleEmitters[newemitter].AlwaysActive=FXTRUE;
	}


/*	AddLight( -1, 40, 
			  3.8f, 3.8f, 0.3f,
			  3.8f, 3.8f, 0.3f,
			  3.8f, 3.8f, 0.3f,
			  1.0f, 0.0f, 0.00001f,
			  xPos, Height, zPos);
*/
	//Blue impact particles
	newemitter = AddParticleEmitter(-1, 1, 50, 3, -1, 0.0f,
	-xPos, -zPos, Height, //Position
	0.0f, +90.0f,		//Yaw/Pitch
	20, 1.0f, 1.0f, 1.0f, 500.0f, 500.0f, 270.0f,
	100, 100, 255, 30);
	ParticleEmitters[newemitter].AlwaysActive=FXTRUE;

	//Make some scary noise
	PlayPositionalSound(22, 22, xPos, zPos, Height, 3000.0f);

}






















//============================================================================
// Initialize vertex normals of the landscape
//============================================================================
void PrecomputeLandscapeVertexNormals(void)
{	// precomputes all vertexnormals for the Landscape
	TlVertex3D a,b;
	int    deler;
	int count, count2;
	float  x,y,z,length;
	int op = 0;
	

	if (use_lighing==0)
		return;

	// Take a vertex
	for (count=0;count<numverts; count++)
	{ 

	if ((int)(100.0f*(float)(count)/(float)(numverts)) > op) 
	{
		op=(int)(100.0f*(float)(count)/(float)(numverts));
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		DrawPercentageBar1(op);
		sprintf(tempstring, "\\c(255 255 255)Lighting Terrain: \\c(128 128 128)%i%s", op, " percent done.");
		mytext.Size(10);
		mytext.Draw(20, 20, tempstring);
		glFlush();
		SwapBuffers( DR_HDC );
	}


		x=y=z=0;		
		if ( (count == 1000) || (count==2000) || (count==3000) || (count==4000))
		{
		}

		deler = 0; // deler is used as to divide x, y and z by the number of faces which share the vertex
		// Go though all faces to see which faces have this vertex in common
		for (count2=0;count2<numfaces; count2++)
		{	// Now if if the face shares the vertex -- compute face normal and add its x component to x, its y to y and its z to z
			// also increase deler.
			if(	Faces[count2].srcVerts[0]==count || Faces[count2].srcVerts[1]==count || Faces[count2].srcVerts[2]==count   )
			{	deler++;
				a.x = Vertices[Faces[count2].srcVerts[2]].v.x - Vertices[Faces[count2].srcVerts[0]].v.x;
				a.y = Vertices[Faces[count2].srcVerts[2]].v.y - Vertices[Faces[count2].srcVerts[0]].v.y;
				a.z = Vertices[Faces[count2].srcVerts[2]].v.z - Vertices[Faces[count2].srcVerts[0]].v.z;
				b.x = Vertices[Faces[count2].srcVerts[1]].v.x - Vertices[Faces[count2].srcVerts[0]].v.x;
				b.y = Vertices[Faces[count2].srcVerts[1]].v.y - Vertices[Faces[count2].srcVerts[0]].v.y;
				b.z = Vertices[Faces[count2].srcVerts[1]].v.z - Vertices[Faces[count2].srcVerts[0]].v.z;
				
				x = x + ((b.y*a.z)-(b.z*a.y));	
				y = y + ((b.z*a.x)-(b.x*a.z));
				z = z + ((b.x*a.y)-(b.y*a.x));
			}	
		}
		if (deler != 0)
		{	Vertices[count].normals.x = x / deler;
			Vertices[count].normals.y = y / deler;
			Vertices[count].normals.z = z / deler;
		} 
		// Normalize the length to 1 to save dividing time in our (realtime) routine as dividing by 1 is not needed !
		length = (float)sqrt((Vertices[count].normals.x*Vertices[count].normals.x) +
					  (Vertices[count].normals.y*Vertices[count].normals.y) +
					  (Vertices[count].normals.z*Vertices[count].normals.z) );


		Vertices[count].normals.x=(Vertices[count].normals.x / length);
		Vertices[count].normals.y=(Vertices[count].normals.y / length);
		Vertices[count].normals.z=(Vertices[count].normals.z / length);
		
		x=0;y=0;z=0;
	}
}



//============================================================================
// Initialize vertex normals of an object
//============================================================================
void PrecomputeVertexNormals(int MeshId)
{	// This function is used by the LoadAscFile function. It precomputes all vertexnormals
	TlVertex3D a,b;
	int    deler;
	int count, count2;
	float  x,y,z,length;	
	

	if (use_lighing==0)
		return;

	// Take a vertex
	for (count=0;count<Meshes[MeshId].numverts; count++)
	{	deler = 0; // deler is used as to divide x, y and z by the number of faces which share the vertex
		// Go though all faces to see which faces have this vertex in common
		for (count2=0;count2<Meshes[MeshId].numfaces; count2++)
		{	// Now if if the face shares the vertex -- compute face normal and add its x component to x, its y to y and its z to z
			// also increase deler.
			if(	Meshes[MeshId].Faces[count2].srcVerts[0]==count || Meshes[MeshId].Faces[count2].srcVerts[1]==count || Meshes[MeshId].Faces[count2].srcVerts[2]==count   )
			{	deler++;
				a.x = Meshes[MeshId].Vertices[Meshes[MeshId].Faces[count2].srcVerts[2]].v.x - Meshes[MeshId].Vertices[Meshes[MeshId].Faces[count2].srcVerts[0]].v.x;
				a.y = Meshes[MeshId].Vertices[Meshes[MeshId].Faces[count2].srcVerts[2]].v.y - Meshes[MeshId].Vertices[Meshes[MeshId].Faces[count2].srcVerts[0]].v.y;
				a.z = Meshes[MeshId].Vertices[Meshes[MeshId].Faces[count2].srcVerts[2]].v.z - Meshes[MeshId].Vertices[Meshes[MeshId].Faces[count2].srcVerts[0]].v.z;
				b.x = Meshes[MeshId].Vertices[Meshes[MeshId].Faces[count2].srcVerts[1]].v.x - Meshes[MeshId].Vertices[Meshes[MeshId].Faces[count2].srcVerts[0]].v.x;
				b.y = Meshes[MeshId].Vertices[Meshes[MeshId].Faces[count2].srcVerts[1]].v.y - Meshes[MeshId].Vertices[Meshes[MeshId].Faces[count2].srcVerts[0]].v.y;
				b.z = Meshes[MeshId].Vertices[Meshes[MeshId].Faces[count2].srcVerts[1]].v.z - Meshes[MeshId].Vertices[Meshes[MeshId].Faces[count2].srcVerts[0]].v.z;
				
				x = x + ((b.y*a.z)-(b.z*a.y));	
				y = y + ((b.z*a.x)-(b.x*a.z));
				z = z + ((b.x*a.y)-(b.y*a.x));
			}	
		}
		if (deler != 0)
		{	Meshes[MeshId].Vertices[count].normals.x = x / deler;
			Meshes[MeshId].Vertices[count].normals.y = y / deler;
			Meshes[MeshId].Vertices[count].normals.z = z / deler;
		} 
		// Normalize the length to 1 to save dividing time in our (realtime) routine as dividing by 1 is not needed !
		length = (float)sqrt((Meshes[MeshId].Vertices[count].normals.x*Meshes[MeshId].Vertices[count].normals.x) +
					  (Meshes[MeshId].Vertices[count].normals.y*Meshes[MeshId].Vertices[count].normals.y) +
					  (Meshes[MeshId].Vertices[count].normals.z*Meshes[MeshId].Vertices[count].normals.z) );
		
		Meshes[MeshId].Vertices[count].normals.x=Meshes[MeshId].Vertices[count].normals.x / length / 1000.0f;
		Meshes[MeshId].Vertices[count].normals.y=Meshes[MeshId].Vertices[count].normals.y / length / 1000.0f;
		Meshes[MeshId].Vertices[count].normals.z=Meshes[MeshId].Vertices[count].normals.z / length / 1000.0f;
		
		x=0;y=0;z=0;
	}
}


/*
//============================================================================
// Rotate vertex normals of an object (for lighting)
//============================================================================
void RotateVertexNormals(int ObjectId, float angleX, float angleY, float angleZ)
{	float temp_x,temp_y,temp_z;
	float sx, sy, sz;
	int i;

	if (use_lighing==0)
		return;

	
	sx=SunX;
	sy=SunY;
	sz=SunZ;
	// Instead of rotating all the vertex normals I only rotate the lightsource in 
	// the opposite direction, relative to the object...
	// It isn't really a clean way of doing it but it sure saves some processing time !

	temp_x	= Cos[(int)(angleX*Deg)] * sx - Sin[(int)(angleX*Deg)] * sy;
	temp_y	= Sin[(int)(angleX*Deg)] * sx + Cos[(int)(angleX*Deg)] * sy;
		
	sx = Cos[(int)(angleY*Deg)] * temp_x - Sin[(int)(angleY*Deg)] * sz;
	temp_z	= Sin[(int)(angleY*Deg)] * temp_x + Cos[(int)(angleY*Deg)] * sz;

 	sy = Cos[(int)(angleZ*Deg)] * temp_y - Sin[(int)(angleZ*Deg)] * temp_z;
	sz = Cos[(int)(angleZ*Deg)] * temp_z + Sin[(int)(angleZ*Deg)] * temp_y;

	for (i=0; i<Meshes[Objects[ObjectId].ObjectMesh].numverts; i++)
	{
		Objects[ObjectId].Vertices[i].normals.x=Meshes[Objects[ObjectId].ObjectMesh].Vertices[i].normals.x *sx;
		Objects[ObjectId].Vertices[i].normals.y=Meshes[Objects[ObjectId].ObjectMesh].Vertices[i].normals.y *sy;
		Objects[ObjectId].Vertices[i].normals.z=Meshes[Objects[ObjectId].ObjectMesh].Vertices[i].normals.z *sz;
	}

}
*/

//============================================================================
// Center an object (needed for proper rotations)
//============================================================================
void CenterMesh(int MeshId, float xOffset, float zOffset, float yOffset)
{
float xmax, ymax, zmax, xmin, ymin, zmin;
  xmin = 65535;
  ymin = 65535;
  zmin = 65535;
  xmax = -65535;
  ymax = -65535;
  zmax = -65535;


	//First, find the meshes' edges
	for (curvert=0;curvert<Meshes[MeshId].numverts;curvert++)
	{
		if (Meshes[MeshId].Vertices[curvert].v.x > xmax)
			xmax = Meshes[MeshId].Vertices[curvert].v.x;
		if (Meshes[MeshId].Vertices[curvert].v.x < xmin)
			xmin = Meshes[MeshId].Vertices[curvert].v.x;
		
		if (Meshes[MeshId].Vertices[curvert].v.y > ymax)
			ymax = Meshes[MeshId].Vertices[curvert].v.y;
		if (Meshes[MeshId].Vertices[curvert].v.y < ymin)
			ymin = Meshes[MeshId].Vertices[curvert].v.y;
		
		if (Meshes[MeshId].Vertices[curvert].v.z > zmax)
			zmax = Meshes[MeshId].Vertices[curvert].v.z;
		if (Meshes[MeshId].Vertices[curvert].v.z < zmin)
			zmin = Meshes[MeshId].Vertices[curvert].v.z;
	}
	//Then center it
	for (curvert=0;curvert<Meshes[MeshId].numverts;curvert++)
	{
		Meshes[MeshId].Vertices[curvert].v.x -= (xmin+xmax)/2 + xOffset;
		Meshes[MeshId].Vertices[curvert].v.y -= (ymin+ymax)/2 + yOffset;
		Meshes[MeshId].Vertices[curvert].v.z -= (zmin+zmax)/2 + zOffset;
	}

	//Calc new edges
	for (curvert=0;curvert<Meshes[MeshId].numverts;curvert++)
	{
		if (Meshes[MeshId].Vertices[curvert].v.x > xmax)
			xmax = Meshes[MeshId].Vertices[curvert].v.x;
		if (Meshes[MeshId].Vertices[curvert].v.x < xmin)
			xmin = Meshes[MeshId].Vertices[curvert].v.x;
		
		if (Meshes[MeshId].Vertices[curvert].v.y > ymax)
			ymax = Meshes[MeshId].Vertices[curvert].v.y;
		if (Meshes[MeshId].Vertices[curvert].v.y < ymin)
			ymin = Meshes[MeshId].Vertices[curvert].v.y;
		
		if (Meshes[MeshId].Vertices[curvert].v.z > zmax)
			zmax = Meshes[MeshId].Vertices[curvert].v.z;
		if (Meshes[MeshId].Vertices[curvert].v.z < zmin)
			zmin = Meshes[MeshId].Vertices[curvert].v.z;
	}
	
	//Now compute the meshes' true size
	xmin -= (xmin+xmax)/2;	xmax -= (xmin+xmax)/2;
	ymin -= (ymin+ymax)/2;	ymax -= (ymin+ymax)/2;
	zmin -= (zmin+zmax)/2;	zmax -= (zmin+zmax)/2;
	if (xmax>xmin)
		Meshes[MeshId].sizeX = (xmax - xmin)/2;
	else
		Meshes[MeshId].sizeX = (xmin - xmax)/2;

	if (ymax>ymin)
		Meshes[MeshId].sizeY = (ymax - ymin)/2;
	else
		Meshes[MeshId].sizeY = (ymin - ymax)/2;

	if (zmax>zmin)
		Meshes[MeshId].sizeZ = (zmax - zmin)/2;
	else
		Meshes[MeshId].sizeZ = (zmin - zmax)/2;

}


//============================================================================
// Load a 3DStudio exported ASCII-Mesh
//============================================================================
int LoadAscFile(char *filename, int MeshId, float scale, float Yaw, float Pitch, float Roll)
{	FILE  *fp;
	char  line[80];
	char  trash[20];
	int count;
	float x,y,z;
	int a,b,c,ab,bc,ca,o;

//grGlideShutdown();

	// Open the file
	if ((fp=fopen(filename,"r" ))==NULL)					
	{	printf("Error opening file: Can't find file\n");
		return(0);
	}
	
	// Skip 3 lines and get the fourth line
//	fgets(line,80,fp);fgets(line,80,fp);
//	fgets(line,80,fp);fgets(line,80,fp);

	while (Meshes[MeshId].numverts==0)
	{
		fgets(line,80,fp);
		sscanf(line,"Tri-mesh, Vertices: %i Faces: %i", &Meshes[MeshId].numverts, &Meshes[MeshId].numfaces);
	}

//	Meshes[MeshId].numfaces++;
	
	// read number of vertices and number of faces
//	sscanf(line,"Tri-mesh, Vertices: %f Faces: %f", Meshes[MeshId].numverts, Meshes[MeshId].numfaces);
	// And reserve memory for this object
//	D3VertexList = (D3Vertex_ptr) calloc(Object->NumVertices, sizeof(D3Vertex) );
//	Faces		 = (Face_ptr)     calloc(Object->NumFaces, sizeof(Face));

	// Skip another line. Now read all the value's of the vertices into our Object
	// This function also scales the vertices by a factor 'SCALE' 
	fgets(line,80,fp);								
	for (count=0; count<Meshes[MeshId].numverts; count++)
	{	ReadTil("Vertex");
		fgets(line,80,fp);
		
		sscanf(line,"%s X: %f Y: %f Z: %f", &trash, &x, &y, &z);
		Meshes[MeshId].Vertices[count].v.x=x * scale;
		Meshes[MeshId].Vertices[count].v.y=z * scale;
		Meshes[MeshId].Vertices[count].v.z=y * scale;
		Meshes[MeshId].Vertices[count].v.r = 200;
		Meshes[MeshId].Vertices[count].v.g = 200;
		Meshes[MeshId].Vertices[count].v.b = 200;

	}	

	// Read all faces, etc...
	fgets(line,80,fp);								
	for (count=0; count<Meshes[MeshId].numfaces; count++)
	{	ReadTil("Face");
		fgets(line,80,fp);
		sscanf(line,"%s A:%d B:%d C:%d AB:%d BC:%d CA:%d",&trash, &a, &b, &c, &ab, &bc, &ca);

		Meshes[MeshId].Faces[count].srcVerts[0]=b;
		Meshes[MeshId].Faces[count].srcVerts[1]=c;
		Meshes[MeshId].Faces[count].srcVerts[2]=a;
		if (MeshId == 2) //We already know texture data for the laser bolt...
		{
			Meshes[MeshId].Faces[count].Texture=3;
			Meshes[MeshId].Faces[count].Transparent=TRUE;
			Meshes[MeshId].Faces[count].isLight=TRUE;
		}

	}

	CenterMesh(MeshId, 0.0f, 0.0f, 0.0f);

	tlSetMatrix( tlIdentity() );
	tlMultMatrix( tlXRotation( Pitch ) );
	tlMultMatrix( tlZRotation( Roll ) );
	tlMultMatrix( tlYRotation( Yaw ) );
//	tlMultMatrix( tlTranslation( Objects[i].xPos, Objects[i].Height, Objects[i].zPos ) );

	//now rotate and translate the object vertices in a temp. array			
	for (o=0; o<Meshes[MeshId].numverts; o++)
	{
		originalVerts[o] = Meshes[MeshId].Vertices[o].v;
	}
	tlTransformVertices( xfVerts, originalVerts, Meshes[MeshId].numverts );
	for (o=0; o<Meshes[MeshId].numverts; o++)
	{
		Meshes[MeshId].Vertices[o].v = xfVerts[o];
	}


	PrecomputeVertexNormals(MeshId); 

	fclose(fp);
	return(1);
}







int AddInternalObject (int ObjectType, int ObjectMesh, int TeamNum, int TimeToLive, FxBool IsAIControlled, FxBool IsGuided, FxBool IsVisible, FxBool IsMarkerMissile, FxBool IsLocalPlayer, float xPos, float zPos, float Height, float Yaw, float Pitch, float Roll, int FiringObject, FxBool NetSync)
{
int fo, fu;
if (NumObjects==0)
Objects[NumObjects].ObjectId = -1;

for (fo=0; fo<=NumObjects; fo++)
{
	 //Search for an empty slot in the objects array
//	if ((Objects[fo].ObjectId == -1) ||
//		(Objects[fo].ObjectId == 0))
	if (Objects[fo].ObjectId != fo)
		break;
}

Objects[fo].ObjectId = fo;
Objects[fo].ObjectMesh = ObjectMesh;
Objects[fo].ObjectType = ObjectType;

Objects[fo].Health = Objects[fo].MaxHealth;
Objects[fo].Shield = Objects[fo].MaxShield;
Objects[fo].xPos = xPos;
Objects[fo].zPos = zPos;
Objects[fo].Height = Height;
Objects[fo].Yaw = Yaw;
Objects[fo].Pitch = Pitch;
Objects[fo].Roll = Roll;
Objects[fo].isMarkerMissile = IsMarkerMissile;
Objects[fo].SpecialType = 0;
Objects[fo].SpecialAmmo = 0;
Objects[fo].MissileType = 0;
Objects[fo].MissileAmmo = 10;
Objects[fo].isMarked = FXFALSE;
Objects[fo].sizeX = Meshes[ObjectMesh].sizeX;
Objects[fo].sizeY = Meshes[ObjectMesh].sizeY;
Objects[fo].sizeZ = Meshes[ObjectMesh].sizeZ;
Objects[fo].TimeToLive = TimeToLive;
Objects[fo].isCollided = FALSE;
Objects[fo].targetObject = -1;
Objects[fo].Team = TeamNum;
Objects[fo].isAIControlled = IsAIControlled;
Objects[fo].isGuided = IsGuided;
Objects[fo].isDiving = FXFALSE;
Objects[fo].isVisible = IsVisible;
Objects[fo].TimeSinceLastAIPoll = 10;
Objects[fo].isDiving=FXFALSE;
Objects[fo].isDocked=FXFALSE;
Objects[fo].isLanding=FXFALSE;
Objects[fo].isStarting=FXFALSE;
Objects[fo].AnimationPhase=0;
Objects[fo].FiringObject=FiringObject;
Objects[fo].AITargetYaw=0;
Objects[fo].AITargetPitch=0;
Objects[fo].AITargetRoll=0;

for (fu=0; fu<myServer.MaxClients; fu++)
	Objects[fo].isVisibleToClient[fu]=FXFALSE;


if (fo >= NumObjects)	//If no empty slot was found, increase NumObjects
{
	NumObjects++;
	MaxNumObjects=NumObjects;
}


//Send all that shit to the netcode
if (NetSync)
	if (!DemoMode)
	NetAddObject( fo, ObjectType, ObjectMesh, TeamNum, TimeToLive, IsAIControlled, IsGuided, IsVisible, IsMarkerMissile, IsLocalPlayer, xPos, zPos, Height, Yaw, Pitch, Roll, FiringObject);

return (fo);

}






//============================================================================
// Add an Object to the scene.
//============================================================================
// note that the actual vertex&face copying takes place in the main render loop!
int AddObject(int ObjectType, int ObjectMesh, int TeamNum, int TimeToLive, FxBool IsAIControlled, FxBool IsGuided, FxBool IsVisible, FxBool IsMarkerMissile, FxBool IsLocalPlayer, float xPos, float zPos, float Height, float Yaw, float Pitch, float Roll, int FiringObject, FxBool NetSync)
{
int fo, fu;
if (NumObjects==0)
Objects[NumObjects].ObjectId = -1;

for (fo=0; fo<=NumObjects; fo++)
{
	if (Objects[fo].ObjectId != fo)
		break;
}
Objects[fo].ObjectId = fo;
Objects[fo].ObjectMesh = ObjectMesh;
Objects[fo].ObjectType = ObjectType;
Objects[fo].ObjectSaveMesh=ObjectMesh;


if (ObjectMesh==OBJECTMESH_MISSILE)
{
	AddNoiseEmitter(fo, SAMPLE_LOOP_MISSILE, 0.0f, 0.0f, 0.0f, 30.0f);
	tempint=AddParticleEmitter(	fo,
							1, 2, 1, -1, 0.0f, 0.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 55.0f, 55.0f, 0.0f,
							255, 255, 255, 30); //Missile flare
	ParticleEmitters[tempint].Slot=0;
	ParticleEmitters[tempint].ThreeD=FXTRUE;
}
if (ObjectMesh==OBJECTMESH_LIGHTTANK)
{
	AddNoiseEmitter(fo, SAMPLE_LOOP_TANKROLL, 0.0f, 0.0f, 0.0f, 100.0f);
	Objects[fo].MaxHealth = 100;
	Objects[fo].MaxShield = 50;
	Objects[fo].SaveMaxShield = 50;
}
if (ObjectMesh==OBJECTMESH_SHIP)
{
	AddNoiseEmitter(fo, SAMPLE_LOOP_SHIP, 0.0f, 0.0f, 0.0f, 100.0f);
	Objects[fo].MaxHealth = 50;
	Objects[fo].MaxShield = 100;
	Objects[fo].SaveMaxShield = 100;
}
if (ObjectMesh==OBJECTMESH_SHIP1)
{
	AddNoiseEmitter(fo, SAMPLE_LOOP_SHIP, 0.0f, 0.0f, 0.0f, 100.0f);
	Objects[fo].MaxHealth = 50;
	Objects[fo].MaxShield = 50;
	Objects[fo].SaveMaxShield = 50;
	//Add exhaust flares
	tempint=AddParticleEmitter(	fo,
							1, 2, 1, -1, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 55.0f, 55.0f, 0.0f,
							100, 100, 255, 30);
	ParticleEmitters[tempint].Slot=0;
	ParticleEmitters[tempint].ThreeD=FXTRUE;
	tempint=AddParticleEmitter(	fo,
							1, 2, 1, -1, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 55.0f, 55.0f, 0.0f,
							100, 100, 255, 30);
	ParticleEmitters[tempint].Slot=1;
	ParticleEmitters[tempint].ThreeD=FXTRUE;
	tempint=AddParticleEmitter(	fo,
							1, 2, 1, -1, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 55.0f, 55.0f, 0.0f,
							100, 100, 255, 30);
	ParticleEmitters[tempint].Slot=2;
	ParticleEmitters[tempint].ThreeD=FXTRUE;
// Position lights
tempint=AddParticleEmitter(	fo,
							1, 2, -1, 100, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 37.0f, 37.0f, 0.0f,
							255, 255, 255, 30);
ParticleEmitters[tempint].Slot=3;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 2, -1, 105, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 37.0f, 37.0f, 0.0f,
							255, 20, 20, 30);
ParticleEmitters[tempint].Slot=4;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 2, -1, 110, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 37.0f, 37.0f, 0.0f,
							20, 255, 20, 30);
ParticleEmitters[tempint].Slot=5;
ParticleEmitters[tempint].ThreeD=FXTRUE;

}
if (ObjectMesh==OBJECTMESH_SHIP2)
{
	AddNoiseEmitter(fo, SAMPLE_LOOP_SHIP, 0.0f, 0.0f, 0.0f, 100.0f);
	Objects[fo].MaxHealth = 100;
	Objects[fo].MaxShield = 100;
	Objects[fo].SaveMaxShield = 100;
	//Add exhaust flares
	tempint=AddParticleEmitter(	fo,
							1, 2, 1, -1, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 65.0f, 65.0f, 0.0f,
							100, 100, 255, 30);
	ParticleEmitters[tempint].Slot=0;
	ParticleEmitters[tempint].ThreeD=FXTRUE;
	//Second exhaust flare
	tempint=AddParticleEmitter(	fo,
							1, 2, 1, -1, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 65.0f, 65.0f, 0.0f,
							100, 100, 255, 30);
	ParticleEmitters[tempint].Slot=4;
	ParticleEmitters[tempint].ThreeD=FXTRUE;
//Position lights
	tempint=AddParticleEmitter(	fo,
							1, 2, -1, 105, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 37.0f, 37.0f, 0.0f,
							255, 20, 20, 30);
	ParticleEmitters[tempint].Slot=1;
	ParticleEmitters[tempint].ThreeD=FXTRUE;
	tempint=AddParticleEmitter(	fo,
							1, 2, -1, 110, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 37.0f, 37.0f, 0.0f,
							20, 255, 20, 30);
	ParticleEmitters[tempint].Slot=2;
	ParticleEmitters[tempint].ThreeD=FXTRUE;
	tempint=AddParticleEmitter(	fo,
							1, 2, -1, 100, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 37.0f, 37.0f, 0.0f,
							255, 255, 255, 30);
	ParticleEmitters[tempint].Slot=3;
	ParticleEmitters[tempint].ThreeD=FXTRUE;
	
}
if (ObjectMesh==OBJECTMESH_SHIP3)
{
	AddNoiseEmitter(fo, SAMPLE_LOOP_SHIP, 0.0f, 0.0f, 0.0f, 100.0f);
	Objects[fo].MaxHealth = 100;
	Objects[fo].MaxShield = 50;
	Objects[fo].SaveMaxShield = 50;
	//Exhaust flares
	tempint=AddParticleEmitter(	fo,
							1, 2, 1, -1, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 75.0f, 75.0f, 0.0f,
							100, 100, 255, 30);
	ParticleEmitters[tempint].Slot=0;
	ParticleEmitters[tempint].ThreeD=FXTRUE;
	tempint=AddParticleEmitter(	fo,
							1, 2, 1, -1, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 75.0f, 75.0f, 0.0f,
							100, 100, 255, 30);
	ParticleEmitters[tempint].Slot=1;
	ParticleEmitters[tempint].ThreeD=FXTRUE;
	tempint=AddParticleEmitter(	fo,
							1, 2, 1, -1, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 75.0f, 75.0f, 0.0f,
							100, 100, 255, 30);
	ParticleEmitters[tempint].Slot=2;
	ParticleEmitters[tempint].ThreeD=FXTRUE;
	tempint=AddParticleEmitter(	fo,
							1, 2, 1, -1, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 75.0f, 75.0f, 0.0f,
							100, 100, 255, 30);
	ParticleEmitters[tempint].Slot=3;
	ParticleEmitters[tempint].ThreeD=FXTRUE;
// Position lights
tempint=AddParticleEmitter( fo,
							1, 2, -1, 100, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 37.0f, 37.0f, 0.0f,
							255, 255, 255, 30);
ParticleEmitters[tempint].Slot=4;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 2, -1, 105, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 37.0f, 37.0f, 0.0f,
							255, 20, 20, 30);
ParticleEmitters[tempint].Slot=5;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 2, -1, 105, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 37.0f, 37.0f, 0.0f,
							255, 20, 20, 30);
ParticleEmitters[tempint].Slot=6;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 2, -1, 110, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 37.0f, 37.0f, 0.0f,
							20, 255, 20, 30);
ParticleEmitters[tempint].Slot=7;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 2, -1, 110, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 37.0f, 37.0f, 0.0f,
							20, 255, 20, 30);
ParticleEmitters[tempint].Slot=8;
ParticleEmitters[tempint].ThreeD=FXTRUE;
}
if (ObjectMesh==OBJECTMESH_COMMANDCENTER)
{
	AddNoiseEmitter(fo, SAMPLE_LOOP_AMB1, 0.0f, 0.0f, 0.0f, 100.0f);
	Objects[fo].isBusy=FXFALSE;
	Objects[fo].MaxHealth = 500;
	Objects[fo].MaxShield = 1000;
tempint=AddParticleEmitter(	fo,
							1, 2, 1, -1, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 37.0f, 37.0f, 0.0f,
							0, 255, 0, 30);
ParticleEmitters[tempint].Slot=0;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 2, 1, -1, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 37.0f, 37.0f, 0.0f,
							0, 255, 0, 30);
ParticleEmitters[tempint].Slot=1;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 2, 1, -1, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 37.0f, 37.0f, 0.0f,
							0, 255, 0, 30);
ParticleEmitters[tempint].Slot=2;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 2, 1, -1, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 37.0f, 37.0f, 0.0f,
							0, 255, 0, 30);
ParticleEmitters[tempint].Slot=3;
ParticleEmitters[tempint].ThreeD=FXTRUE;

tempint=AddParticleEmitter(	fo,
							1, 10, -1, 40, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 87.0f, 87.0f, 0.0f,
							255, 0, 0, 30);
ParticleEmitters[tempint].Slot=4;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 10, -1, 40, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 87.0f, 87.0f, 0.0f,
							255, 0, 0, 30);
ParticleEmitters[tempint].Slot=5;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 10, -1, 40, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 87.0f, 87.0f, 0.0f,
							255, 0, 0, 30);
ParticleEmitters[tempint].Slot=6;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 10, -1, 40, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 87.0f, 87.0f, 0.0f,
							255, 0, 0, 30);
ParticleEmitters[tempint].Slot=7;
ParticleEmitters[tempint].ThreeD=FXTRUE;

}
if (ObjectMesh==OBJECTMESH_POWERPLANT)
{
	AddNoiseEmitter(fo, SAMPLE_LOOP_AMB2, 0.0f, 0.0f, 0.0f, 100.0f);
	Objects[fo].MaxHealth = 200;
	Objects[fo].MaxShield = 200;
	//Smoke
	tempint=AddParticleEmitter(	fo,
								0, 150, -1, 6, 0.0f, 0.0f, 0.0f, -500.0f, 0.0f, 90.0f, -1, 0.02f, 0.15f, 0.0f, 120.0f, 200.0f, 50.0f,
								128,128,128, 6);
	ParticleEmitters[tempint].Slot=0;
	ParticleEmitters[tempint].ThreeD=FXFALSE;
//	ParticleEmitters[tempint].ThreeD=FXTRUE;
	ParticleEmitters[tempint].Pitch = 90;
//Red Lights
tempint=AddParticleEmitter(	fo,
							1, 50, -1, 70, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 87.0f, 87.0f, 0.0f,
							255, 50, 50, 30);
ParticleEmitters[tempint].Slot=1;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 50, -1, 70, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 87.0f, 87.0f, 0.0f,
							255, 50, 50, 30);
ParticleEmitters[tempint].Slot=2;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 50, -1, 70, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 87.0f, 87.0f, 0.0f,
							255, 50, 50, 30);
ParticleEmitters[tempint].Slot=3;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 50, -1, 70, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 87.0f, 87.0f, 0.0f,
							255, 50, 50, 30);
ParticleEmitters[tempint].Slot=4;
ParticleEmitters[tempint].ThreeD=FXTRUE;

}
if (ObjectMesh==OBJECTMESH_MINE)
{
	AddNoiseEmitter(fo, SAMPLE_LOOP_AMB3, 0.0f, 0.0f, 0.0f, 100.0f);
	Objects[fo].MaxHealth = 200;
	Objects[fo].MaxShield = 200;
//White lights
tempint=AddParticleEmitter(	fo,
							1, 2, 1, -1, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 37.0f, 37.0f, 0.0f,
							255, 255, 160, 30);
ParticleEmitters[tempint].Slot=0;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 2, 1, -1, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 37.0f, 37.0f, 0.0f,
							255, 255, 160, 30);
ParticleEmitters[tempint].Slot=1;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 2, 1, -1, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 37.0f, 37.0f, 0.0f,
							255, 255, 160, 30);
ParticleEmitters[tempint].Slot=2;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 2, 1, -1, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 37.0f, 37.0f, 0.0f,
							255, 255, 160, 30);
ParticleEmitters[tempint].Slot=3;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 2, 1, -1, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 37.0f, 37.0f, 0.0f,
							255, 255, 160, 30);
ParticleEmitters[tempint].Slot=4;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 2, 1, -1, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 37.0f, 37.0f, 0.0f,
							255, 255, 160, 30);
ParticleEmitters[tempint].Slot=5;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 2, 1, -1, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 37.0f, 37.0f, 0.0f,
							255, 255, 160, 30);
ParticleEmitters[tempint].Slot=6;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 2, 1, -1, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 37.0f, 37.0f, 0.0f,
							255, 255, 160, 30);
ParticleEmitters[tempint].Slot=7;
ParticleEmitters[tempint].ThreeD=FXTRUE;
//Chimney blinkers
tempint=AddParticleEmitter(	fo,
							1, 60, -1, 90, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 50.0f, 50.0f, 0.0f,
							255, 50, 50, 30);
ParticleEmitters[tempint].Slot=8;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 60, -1, 90, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 50.0f, 50.0f, 0.0f,
							255, 50, 50, 30);
ParticleEmitters[tempint].Slot=9;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 60, -1, 90, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 50.0f, 50.0f, 0.0f,
							255, 50, 50, 30);
ParticleEmitters[tempint].Slot=10;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 60, -1, 90, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 50.0f, 50.0f, 0.0f,
							255, 50, 50, 30);
ParticleEmitters[tempint].Slot=11;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 60, -1, 90, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 50.0f, 50.0f, 0.0f,
							255, 50, 50, 30);
ParticleEmitters[tempint].Slot=12;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 60, -1, 90, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 50.0f, 50.0f, 0.0f,
							255, 50, 50, 30);
ParticleEmitters[tempint].Slot=13;
ParticleEmitters[tempint].ThreeD=FXTRUE;
//Chimney Smoke
tempint=AddParticleEmitter( fo,
							0, 150, -1, 5, 0.0f, 0.0f, 0.0f, -500.0f, 90.0f, 0.0f, -1, 0.01f, 0.15f, 0.0f, 25.0f, 75.0f, 20.0f,
							255,255,255, 6);
ParticleEmitters[tempint].Slot=14;
ParticleEmitters[tempint].ThreeD=FXFALSE;
ParticleEmitters[tempint].Pitch = 90;
tempint=AddParticleEmitter(	fo,
							0, 150, -1, 5, 0.0f, 0.0f, 0.0f, -500.0f, 90.0f, 0.0f, -1, 0.01f, 0.15f, 0.0f, 25.0f, 75.0f, 20.0f,
							255,255,255, 6);
ParticleEmitters[tempint].Slot=15;
ParticleEmitters[tempint].ThreeD=FXFALSE;
ParticleEmitters[tempint].Pitch = 90;
}
if (ObjectMesh==OBJECTMESH_SAM)
{
	Objects[fo].MaxHealth = 200;
	Objects[fo].MaxShield = 100;
} 
if (ObjectMesh==OBJECTMESH_AAA)
{
	Objects[fo].MaxHealth = 200;
	Objects[fo].MaxShield = 200;
} 

if (ObjectMesh==OBJECTMESH_UPLINK)
{
	AddNoiseEmitter(fo, SAMPLE_LOOP_AMB4, 0.0f, 0.0f, 0.0f, 100.0f);
	Objects[fo].MaxHealth = 100;
	Objects[fo].MaxShield = 300;
//Red blinking lights
tempint=AddParticleEmitter(	fo,
							1, 60, -1, 100, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 75.0f, 75.0f, 0.0f,
							255, 50, 50, 30);
ParticleEmitters[tempint].Slot=1;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 60, -1, 100, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 75.0f, 75.0f, 0.0f,
							255, 50, 50, 30);
ParticleEmitters[tempint].Slot=2;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 60, -1, 100, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 75.0f, 75.0f, 0.0f,
							255, 50, 50, 30);
ParticleEmitters[tempint].Slot=3;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 60, -1, 100, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 75.0f, 75.0f, 0.0f,
							255, 50, 50, 30);
ParticleEmitters[tempint].Slot=4;
ParticleEmitters[tempint].ThreeD=FXTRUE;
//Yellow antenna top
tempint=AddParticleEmitter(	fo,
							1, 2, 1, -1, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 25.0f, 25.0f, 0.0f,
							255, 255, 0, 30);
ParticleEmitters[tempint].Slot=0;
ParticleEmitters[tempint].ThreeD=FXTRUE;

} 


if (ObjectMesh==OBJECTMESH_UPLINK2)
{
	AddNoiseEmitter(fo, SAMPLE_LOOP_AMB4, 0.0f, 0.0f, 0.0f, 100.0f);
	Objects[fo].MaxHealth = 300;
	Objects[fo].MaxShield = 200;
//Red blinking lights
tempint=AddParticleEmitter(	fo,
							1, 60, -1, 100, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 75.0f, 75.0f, 0.0f,
							255, 50, 50, 30);
ParticleEmitters[tempint].Slot=1;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 60, -1, 100, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 75.0f, 75.0f, 0.0f,
							255, 50, 50, 30);
ParticleEmitters[tempint].Slot=2;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 60, -1, 100, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 75.0f, 75.0f, 0.0f,
							255, 50, 50, 30);
ParticleEmitters[tempint].Slot=3;
ParticleEmitters[tempint].ThreeD=FXTRUE;
tempint=AddParticleEmitter(	fo,
							1, 60, -1, 100, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 75.0f, 75.0f, 0.0f,
							255, 50, 50, 30);
ParticleEmitters[tempint].Slot=4;
ParticleEmitters[tempint].ThreeD=FXTRUE;
//Yellow antenna top
tempint=AddParticleEmitter(	fo,
							1, 2, 1, -1, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 25.0f, 25.0f, 0.0f,
							255, 255, 0, 30);
ParticleEmitters[tempint].Slot=0;
ParticleEmitters[tempint].ThreeD=FXTRUE;

} 


//The tanks stir up dust as they travel. Type and color depending on terrain.
if (ObjectMesh==OBJECTMESH_LIGHTTANK)
{

	if (TerrainType == TERRAIN_LUSH)
	{
		tempemitter = AddParticleEmitter( fo,
							0, (int)(100.0f*g_ispeedfactor), -1, 2, 0.0f, 0.0f, 0.0f, -500.0f, 90.0f, 0.0f, -1, 0.15f, 0.01f, 0.0f, 10.0f, 175.0f, 20.0f,
							100, 100, 50, 6);
		ParticleEmitters[tempemitter].Slot=0;
		tempemitter = AddParticleEmitter( fo,
							0, (int)(100.0f*g_ispeedfactor), -1, 2, 0.0f, 0.0f, 0.0f, -500.0f, 90.0f, 0.0f, -1, 0.15f, 0.01f, 0.0f, 10.0f, 175.0f, 20.0f,
							100, 100, 50, 6);
		ParticleEmitters[tempemitter].Slot=1;
	}
	else if (TerrainType == TERRAIN_DESERT)
	{
		tempemitter = AddParticleEmitter( fo,
							0, (int)(150.0f*g_ispeedfactor), -1, 1, 0.0f, 0.0f, 0.0f, -500.0f, 90.0f, 0.0f, -1, 0.15f, 0.01f, 0.0f, 25.0f, 200.0f, 20.0f,
							100, 100, 50, 6);
		ParticleEmitters[tempemitter].Slot=0;
		tempemitter = AddParticleEmitter( fo,
							0, (int)(150.0f*g_ispeedfactor), -1, 1, 0.0f, 0.0f, 0.0f, -500.0f, 90.0f, 0.0f, -1, 0.15f, 0.01f, 0.0f, 25.0f, 200.0f, 20.0f,
							100, 100, 50, 6);
		ParticleEmitters[tempemitter].Slot=1;
	}
	else if (TerrainType == TERRAIN_LAVA)
	{
		tempemitter = AddParticleEmitter( fo,
							0, (int)(150.0f*g_ispeedfactor), -1, 2, 0.0f, 0.0f, 0.0f, -500.0f, 90.0f, 0.0f, -1, 0.15f, 0.01f, 0.0f, 15.0f, 180.0f, 20.0f,
							50, 50, 50, 6);
		ParticleEmitters[tempemitter].Slot=0;
		tempemitter = AddParticleEmitter( fo,
							0, (int)(150.0f*g_ispeedfactor), -1, 2, 0.0f, 0.0f, 0.0f, -500.0f, 90.0f, 0.0f, -1, 0.15f, 0.01f, 0.0f, 15.0f, 180.0f, 20.0f,
							50, 50, 50, 6);
		ParticleEmitters[tempemitter].Slot=1;
	}
	else if (TerrainType == TERRAIN_SNOW)
	{
		tempemitter = AddParticleEmitter( fo,
							0, (int)(60.0f*g_ispeedfactor), -1, 2, 0.0f, 0.0f, 0.0f, -500.0f, 90.0f, 0.0f, -1, 0.15f, 0.01f, 0.0f, 25.0f, 100.0f, 20.0f,
							200, 200, 200, 6);
		ParticleEmitters[tempemitter].Slot=0;
		tempemitter = AddParticleEmitter( fo,
							0, (int)(60.0f*g_ispeedfactor), -1, 2, 0.0f, 0.0f, 0.0f, -500.0f, 90.0f, 0.0f, -1, 0.15f, 0.01f, 0.0f, 25.0f, 100.0f, 20.0f,
							200, 200, 200, 6);
		ParticleEmitters[tempemitter].Slot=1;
	}
	else if (TerrainType == TERRAIN_STONE)
	{
		tempemitter = AddParticleEmitter( fo,
							0, (int)(50.0f*g_ispeedfactor), -1, 3, 0.0f, 0.0f, 0.0f, -500.0f, 90.0f, 0.0f, -1, 0.15f, 0.01f, 0.0f, 15.0f, 100.0f, 20.0f,
							80, 80, 50, 6);
		ParticleEmitters[tempemitter].Slot=0;
		tempemitter = AddParticleEmitter( fo,
							0, (int)(50.0f*g_ispeedfactor), -1, 3, 0.0f, 0.0f, 0.0f, -500.0f, 90.0f, 0.0f, -1, 0.15f, 0.01f, 0.0f, 15.0f, 100.0f, 20.0f,
							80, 80, 50, 6);
		ParticleEmitters[tempemitter].Slot=1;
	}
}


	if (ObjectType == OBJECTTYPE_SHIP)
	{
		if (TeamNum == Player[localplayer].Team)
			AddParticleEmitter(fo,2,2,1,-1,0.0f,xPos,zPos,Height, 0.0f,0.0f,-1,0.0f,0.0f,0.0f,1500.0f,1500.0f,0.0f,255,255,255,ICON_FRIENDLY);
		else
			AddParticleEmitter(fo,2,2,1,-1,0.0f,xPos,zPos,Height, 0.0f,0.0f,-1,0.0f,0.0f,0.0f,1500.0f,1500.0f,0.0f,255,255,255,ICON_TARGET);
	}

	if (ObjectMesh == OBJECTMESH_COMMANDCENTER)
	{
		if (TeamNum == Player[localplayer].Team)
			AddParticleEmitter(fo,2,2,1,-1,0.0f,-xPos,-zPos,-400.0f, 0.0f,0.0f,-1,0.0f,0.0f,0.0f,500.0f,500.0f,0.0f,255,255,255,ICON_FRIENDLYBASE);
		else
			AddParticleEmitter(fo,2,2,1,-1,0.0f,-xPos,-zPos,-400.0f, 0.0f,0.0f,-1,0.0f,0.0f,0.0f,500.0f,500.0f,0.0f,255,255,255,ICON_ENEMYBASE);
	}

Objects[fo].Health = Objects[fo].MaxHealth;
Objects[fo].Shield = Objects[fo].MaxShield;
Objects[fo].xPos = xPos;
Objects[fo].zPos = zPos;
Objects[fo].Height = Height;
Objects[fo].Yaw = Yaw;
Objects[fo].Pitch = Pitch;
Objects[fo].Roll = Roll;
Objects[fo].isMarkerMissile = IsMarkerMissile;
Objects[fo].SpecialType = 0;
Objects[fo].SpecialAmmo = 0;
Objects[fo].MissileType = 0;
Objects[fo].MissileAmmo = 10;
Objects[fo].isMarked = FXFALSE;
Objects[fo].sizeX = Meshes[ObjectMesh].sizeX;
Objects[fo].sizeY = Meshes[ObjectMesh].sizeY;
Objects[fo].sizeZ = Meshes[ObjectMesh].sizeZ;
Objects[fo].TimeToLive = TimeToLive;
Objects[fo].isCollided = FALSE;
Objects[fo].targetObject = -1;
Objects[fo].Team = TeamNum;
Objects[fo].isAIControlled = IsAIControlled;
Objects[fo].isGuided = IsGuided;
Objects[fo].isDiving = FXFALSE;
Objects[fo].isVisible = IsVisible;
Objects[fo].TimeSinceLastAIPoll = 10;
Objects[fo].isDiving=FXFALSE;
Objects[fo].isDocked=FXFALSE;
Objects[fo].isLanding=FXFALSE;
Objects[fo].isStarting=FXFALSE;
Objects[fo].AnimationPhase=0;
Objects[fo].FiringObject=FiringObject;
Objects[fo].AITargetYaw=0;
Objects[fo].AITargetPitch=0;
Objects[fo].AITargetRoll=0;


for (fu=0; fu<myServer.MaxClients; fu++)
	Objects[fo].isVisibleToClient[fu]=FXFALSE;

if (fo >= NumObjects)	//If no empty slot was found, increase NumObjects
{
	NumObjects++;
	MaxNumObjects=NumObjects;
}
//Objects[NumObjects].ObjectId = -1;


//Send all that shit to the netcode
if (NetSync)
	if (!DemoMode)
	{
		Objects[fo].OwningNetworkClient = localplayer;
		Objects[fo].IDAtOwningClient = fo;
		NetAddObject( fo, ObjectType, ObjectMesh, TeamNum, TimeToLive, IsAIControlled, IsGuided, IsVisible, IsMarkerMissile, IsLocalPlayer, xPos, zPos, Height, Yaw, Pitch, Roll, FiringObject);
	}


/*
sprintf(tempstring, "\n------------FREAKIN' SITUATION AFTER ADDOBJECT:------------");
WriteErrorLog(tempstring);
sprintf(tempstring, "MaxNum=%i: Num=%i", MaxNumObjects, NumObjects);
WriteErrorLog(tempstring);
for (fu=0; fu<MaxNumObjects; fu++)
{
	sprintf(tempstring, "No. %i: ID=%i Mesh=%i", fu, Objects[fu].ObjectId, Objects[fu].ObjectMesh);
	WriteErrorLog(tempstring);
}
sprintf(tempstring, "...and I added object %i to the mess\n", fo);
WriteErrorLog(tempstring);
*/
	Objects[fo].ParentObject=-1;
	Objects[fo].ChildObject0=-1;
	Objects[fo].ChildObject1=-1;
	Objects[fo].ChildObject2=-1;

if (Objects[fo].ObjectMesh==OBJECTMESH_AAA)
{
	Objects[fo].ObjectMesh=OBJECTMESH_AAA1_LOD1_PART1;

	Objects[fo].sizeX = Meshes[Objects[fo].ObjectMesh].sizeX;
	Objects[fo].sizeY = Meshes[Objects[fo].ObjectMesh].sizeY;
	Objects[fo].sizeZ = Meshes[Objects[fo].ObjectMesh].sizeZ;

	int Child0 = AddInternalObject (ObjectType, OBJECTMESH_AAA1_LOD1_PART2, TeamNum, TimeToLive, IsAIControlled, IsGuided, IsVisible, IsMarkerMissile, IsLocalPlayer, xPos, zPos, (Height-(Meshes[Objects[fo].ObjectMesh].sizeY*2.2f)), Yaw, Pitch, Roll, fo, FXFALSE);
	int Child1 = AddInternalObject (ObjectType, OBJECTMESH_AAA1_LOD1_PART3, TeamNum, TimeToLive, IsAIControlled, IsGuided, IsVisible, IsMarkerMissile, IsLocalPlayer, xPos, zPos, (Height-Meshes[Objects[fo].ObjectMesh].sizeY-(Meshes[Objects[Child0].ObjectMesh].sizeY/2.3f)), Yaw, Pitch, Roll, fo, FXFALSE);
	Objects[fo].ChildObject0 = Child0;
	Objects[fo].ChildObject1 = Child1;
	Objects[Child0].ParentObject = fo;
	Objects[Child1].ParentObject = fo;

	Objects[fo].ObjectMesh=OBJECTMESH_AAA;

}

/*
if (Objects[fo].ObjectMesh==OBJECTMESH_SAM)
{
	Objects[fo].ObjectMesh=OBJECTMESH_SAM1_LOD1_PART1;

	Objects[fo].sizeX = Meshes[Objects[fo].ObjectMesh].sizeX;
	Objects[fo].sizeY = Meshes[Objects[fo].ObjectMesh].sizeY;
	Objects[fo].sizeZ = Meshes[Objects[fo].ObjectMesh].sizeZ;

	int Child0 = AddInternalObject (ObjectType, OBJECTMESH_SAM1_LOD1_PART2, TeamNum, TimeToLive, IsAIControlled, IsGuided, IsVisible, IsMarkerMissile, IsLocalPlayer, xPos, zPos, (Height-Meshes[Objects[fo].ObjectMesh].sizeY), Yaw, Pitch, Roll, fo);
	int Child1 = AddInternalObject (ObjectType, OBJECTMESH_SAM1_LOD1_PART3, TeamNum, TimeToLive, IsAIControlled, IsGuided, IsVisible, IsMarkerMissile, IsLocalPlayer, xPos, zPos, (Height-Meshes[Objects[fo].ObjectMesh].sizeY-Meshes[Objects[Child0].ObjectMesh].sizeY), Yaw, Pitch, Roll, fo);
	Objects[fo].ChildObject0 = Child0;
	Objects[fo].ChildObject1 = Child1;
	Objects[Child0].ParentObject = fo;
	Objects[Child1].ParentObject = fo;

	Objects[fo].ObjectMesh=OBJECTMESH_AAA;

}
*/


sprintf(tempstring, "Added Object %i", fo);
WriteErrorLog(tempstring);
/*//Debug output
for (fu=0; fu<NumObjects; fu++)
{
	if (Objects[fu].ObjectId == fu)
	{
		sprintf(tempstring, "Object ID=%i Type=%i Mesh=%i Team=%i ControllingPlayer=%i OwningNetworkClient=%i IDAtOwningClient=%i TTL=%i", 
				Objects[fu].ObjectId,
				Objects[fu].ObjectType,
				Objects[fu].ObjectMesh,
				Objects[fu].Team,
				Objects[fu].ControllingPlayer,
				Objects[fu].OwningNetworkClient,
				Objects[fu].IDAtOwningClient,
				Objects[fu].TimeToLive);
		WriteErrorLog(tempstring);

	}
}
*/




return (fo);
}


//============================================================================
// Check if building at xg/zg is possible
//============================================================================
int CheckBuildingSiteClear(int xg, int zg)
{
int curobj;

for (curobj=0; curobj<NumObjects; curobj++)
{
	if ((Objects[curobj].ObjectType==OBJECTTYPE_BUILDING) && (Objects[curobj].ObjectId != -1))
	{
		// Is Position already occupied?
//		if ((Objects[curobj].xGrid == xg) && (Objects[curobj].zGrid == zg))
//			return 0;

		// Buildings also cannot be built diagonally, or
		// else the terrain evening procedure halfway "buries" the other buildings...
		if (((Objects[curobj].xGrid-1) == xg) && (Objects[curobj].zGrid == zg))
			return 0;
		if ((Objects[curobj].xGrid == xg) && ((Objects[curobj].zGrid-1) == zg))
			return 0;
		if (((Objects[curobj].xGrid-1) == xg) && ((Objects[curobj].zGrid-1) == zg))
			return 0;
		if (((Objects[curobj].xGrid+1) == xg) && (Objects[curobj].zGrid == zg))
			return 0;
		if ((Objects[curobj].xGrid == xg) && ((Objects[curobj].zGrid+1) == zg))
			return 0;
		if (((Objects[curobj].xGrid+1) == xg) && ((Objects[curobj].zGrid+1) == zg))
			return 0;
		if (((Objects[curobj].xGrid-1) == xg) && ((Objects[curobj].zGrid+1) == zg))
			return 0;
		if (((Objects[curobj].xGrid+1) == xg) && ((Objects[curobj].zGrid-1) == zg))
			return 0;
		}
	}
	return 1;
}



//============================================================================
// Add a building to the scene
//============================================================================
//Same as AddObject, but it is meant for buildings. 
//It will automatically "even" the landscape's surface, remove the trees and 
//place the mesh of the building directly atop.
//Buildings:	4=command center 5=power plant, 6=Mine, 7=SAM-Site, 8=AAA-Site
int AddBuilding(int Building, int TeamNum, float xb, float zb, float Yaw)
{
float h1, h2, h3, h4, maxh, minh, placeX, placeY, placeZ;
int fo, xg, zg;
int tempObject;

if (xb>0.0f)
	xg = (int)(xb/scalefactor)+1;
else
	xg = (int)(xb/scalefactor);
if (zb>0.0f)
	zg = (int)(zb/scalefactor)+1;
else
	zg = (int)(zb/scalefactor);

//Should we be even building there?
if (CheckBuildingSiteClear(xg, zg)==0)
	return -1;

//Check if something blocks the building site...
for (tempObject=0; tempObject<MaxNumObjects; tempObject++)
if (Objects[tempObject].ObjectId == tempObject)
{
	if( (Within(xb+Meshes[Building].sizeX, Objects[tempObject].xPos+Objects[tempObject].sizeX*2, Objects[tempObject].xPos-Objects[tempObject].sizeX*2)) &&
		(Within(zb+Meshes[Building].sizeZ , Objects[tempObject].zPos+Objects[tempObject].sizeZ*2, Objects[tempObject].zPos-Objects[tempObject].sizeZ*2)) )
		return -1;
	if( (Within(xb+Meshes[Building].sizeX, Objects[tempObject].xPos+Objects[tempObject].sizeX*2, Objects[tempObject].xPos-Objects[tempObject].sizeX*2)) &&
		(Within(zb-Meshes[Building].sizeZ , Objects[tempObject].zPos+Objects[tempObject].sizeZ*2, Objects[tempObject].zPos-Objects[tempObject].sizeZ*2)) )
		return -1;
	if( (Within(xb-Meshes[Building].sizeX, Objects[tempObject].xPos+Objects[tempObject].sizeX*2, Objects[tempObject].xPos-Objects[tempObject].sizeX*2)) &&
		(Within(zb-Meshes[Building].sizeZ , Objects[tempObject].zPos+Objects[tempObject].sizeZ*2, Objects[tempObject].zPos-Objects[tempObject].sizeZ*2)) )
		return -1;
	if( (Within(xb-Meshes[Building].sizeX, Objects[tempObject].xPos+Objects[tempObject].sizeX*2, Objects[tempObject].xPos-Objects[tempObject].sizeX*2)) &&
		(Within(zb+Meshes[Building].sizeZ , Objects[tempObject].zPos+Objects[tempObject].sizeZ*2, Objects[tempObject].zPos-Objects[tempObject].sizeZ*2)) )
		return -1;
	if (TeamNum == Player[localplayer].Team)
	{
		sprintf(tempstring, "Building site blocked by ID %i Mesh %i!", tempObject, Objects[tempObject].ObjectMesh);
		AddMessage( tempstring, 255,100,100);
	}
}

//Even terrain prior to building
h1= Vertices[Faces[facexy[MapDimension-xg-1][MapDimension-zg]].srcVerts[0]].v.y;
h2= Vertices[Faces[facexy[MapDimension-xg-1][MapDimension-zg]].srcVerts[1]].v.y;
h3= Vertices[Faces[facexy[MapDimension-xg-1][MapDimension-zg]].srcVerts[2]].v.y;
h4= Vertices[Faces[facexyB[MapDimension-xg-1][MapDimension-zg]].srcVerts[1]].v.y;
maxh=h1;
minh=h1;
if (h2>maxh)
	maxh=h2;
if (h2<minh)
	minh=h2;
if (h3>maxh)
	maxh=h3;
if (h3<minh)
	minh=h3;
if (h4>maxh)
	maxh=h4;
if (h4<minh)
	minh=h4;
maxh=minh+((maxh-minh)/2);

for (fo=0; fo<numLSverts; fo++)
{
	if ((Difference(Vertices[fo].v.x, Vertices[Faces[facexy[MapDimension-xg-1][MapDimension-zg]].srcVerts[0]].v.x) < 10.0f) &&
		(Difference(Vertices[fo].v.z, Vertices[Faces[facexy[MapDimension-xg-1][MapDimension-zg]].srcVerts[0]].v.z) < 10.0f))
		Vertices[fo].v.y = maxh;
	if ((Difference(Vertices[fo].v.x, Vertices[Faces[facexy[MapDimension-xg-1][MapDimension-zg]].srcVerts[1]].v.x) < 10.0f) &&
		(Difference(Vertices[fo].v.z, Vertices[Faces[facexy[MapDimension-xg-1][MapDimension-zg]].srcVerts[1]].v.z) < 10.0f))
		Vertices[fo].v.y = maxh;
	if ((Difference(Vertices[fo].v.x, Vertices[Faces[facexy[MapDimension-xg-1][MapDimension-zg]].srcVerts[2]].v.x) < 10.0f) &&
		(Difference(Vertices[fo].v.z, Vertices[Faces[facexy[MapDimension-xg-1][MapDimension-zg]].srcVerts[2]].v.z) < 10.0f))
		Vertices[fo].v.y = maxh;
	if ((Difference(Vertices[fo].v.x, Vertices[Faces[facexyB[MapDimension-xg-1][MapDimension-zg]].srcVerts[1]].v.x) < 10.0f) &&
		(Difference(Vertices[fo].v.z, Vertices[Faces[facexyB[MapDimension-xg-1][MapDimension-zg]].srcVerts[1]].v.z) < 10.0f))
		Vertices[fo].v.y = maxh;
}

placeY = -(maxh + (Meshes[Building].sizeY/2.0f))-0.5f;
placeX = xb;
placeZ = zb;


// Timbeeeeeer!!!! ;)
RemoveTrees(placeX-Meshes[Building].sizeX,
			placeZ-Meshes[Building].sizeZ,
			placeX+Meshes[Building].sizeX,
			placeZ+Meshes[Building].sizeZ);

fo = AddObject( OBJECTTYPE_BUILDING, Building, TeamNum, -1, FXFALSE, FXFALSE, FXTRUE, FXFALSE, FXFALSE, placeX, placeZ, placeY, Yaw, 0.0f, 0.0f, -1, FXTRUE);
Objects[fo].xGrid=xg;
Objects[fo].zGrid=zg;

Team[TeamNum].BuildingsInTeam++;
return (fo);
}



//============================================================================
// Returns the objectID of an object that matches the given type, team and mesh
//============================================================================
//Usage: Tell this function which objecttype, mesh and team you're looking for.
//If you want to have the first object this function can find, set the variable 
//"count" to 1. If you want the second matching object, set it to 2, etc.
//When getObject() can't find any more objects, it will return -1
int getObject(int TeamNum, int Type, int Mesh, int count)
{
	int i, found;
	found=0;
	for (i=0; i<NumObjects; i++)
	{
		if ((Objects[i].Team==TeamNum) && (Objects[i].ObjectId != -1))
			if (Objects[i].ObjectType==Type)
				if (Objects[i].ObjectMesh==Mesh)
				{
					found++;
					if (found==count)
						return(i);
				}
	}
	return (-1);
}



//============================================================================
// Sets variables for a human player, an AI or a remote client, and returns the playerId
//============================================================================
//Because a player can respawn several times, I've separated the player setup and spawn routines.
int setupPlayer(FxBool isAIPlayer, FxBool isHumanPlayer, FxBool isNetworkPlayer, FxBool isTankPlayer, int TeamNum, int VehicleModel)
{
	Player[numPlayers].isActive=FXTRUE;
	Player[numPlayers].isAI=isAIPlayer;
	Player[numPlayers].isHuman=isHumanPlayer;
	if (isHumanPlayer)
	{
		localplayer = numPlayers;
	}
	Player[numPlayers].isNetwork=isNetworkPlayer;
//	Player[numPlayers].PlayerName=*PlayerName;
	Player[numPlayers].isTank=isTankPlayer;
	Player[numPlayers].VehicleModel=VehicleModel;
	Player[numPlayers].Team=TeamNum;
	Player[numPlayers].doRespawn=FXTRUE;
	numPlayers++;
	Team[TeamNum].PlayersInTeam++;
	return (numPlayers-1);
}



//============================================================================
// Spawns a human player or an AI
//============================================================================
void spawnPlayer(int playerId)
{
	float xPos;
	float zPos;
	float Height;
	int HomeCenter;


	if (Player[playerId].isActive!=FXTRUE)
		return;
	if (Player[playerId].isNetwork)
		return;


	HomeCenter = getObject(Player[playerId].Team, OBJECTTYPE_BUILDING, OBJECTMESH_COMMANDCENTER, 1);

	if (Objects[HomeCenter].isBusy)
		return;

	if (playerId!=localplayer)
		Objects[HomeCenter].isBusy=FXTRUE;

	Player[playerId].doRespawn=FXFALSE;

	xPos = Objects[HomeCenter].xPos;
	zPos = Objects[HomeCenter].zPos;
	Height = Objects[HomeCenter].Height-0.5f; //start above the command center
	
	
//	if (Player[playerId].isTank)	//Spawn either as Tank or as Ship
//		Player[playerId].ControlledObject = AddObject( OBJECTTYPE_SHIP, OBJECTMESH_LIGHTTANK, Player[playerId].Team, -1, Player[playerId].isAI, FXFALSE, FXTRUE, FXFALSE, xPos, zPos, Height, 0.0f, 0.0f, 0.0f, -1, FXTRUE);
//	else
		Player[playerId].ControlledObject = AddObject( OBJECTTYPE_SHIP, Player[playerId].VehicleModel, Player[playerId].Team, -1, Player[playerId].isAI, FXFALSE, FXTRUE, FXFALSE, (playerId==localplayer), xPos, zPos, Height, 0.0f, 0.0f, 0.0f, -1, FXTRUE);

	Objects[Player[playerId].ControlledObject].FiringObject=Player[playerId].ControlledObject;
	Objects[Player[playerId].ControlledObject].ControllingPlayer = playerId;
	Objects[Player[playerId].ControlledObject].CommandCenter=HomeCenter;

//	if ((!TextureEditor) && (!Cheat))

	Objects[Player[playerId].ControlledObject].isDocked=FXTRUE;
	Objects[Player[playerId].ControlledObject].isDiving=FXFALSE;
	Objects[Player[playerId].ControlledObject].isLanding=FXFALSE;
	Objects[Player[playerId].ControlledObject].isStarting=FXFALSE;
	Objects[Player[playerId].ControlledObject].AnimationPhase=0;


	Objects[Player[playerId].ControlledObject].Health=Objects[Player[playerId].ControlledObject].MaxHealth;
	Objects[Player[playerId].ControlledObject].Shield=Objects[Player[playerId].ControlledObject].MaxShield;
	Objects[Player[playerId].ControlledObject].Yaw=0;
	Objects[Player[playerId].ControlledObject].TimeToLive=-1;
	Objects[Player[playerId].ControlledObject].Pitch=0;
	Objects[Player[playerId].ControlledObject].Roll=0;
	Objects[Player[playerId].ControlledObject].ControllingPlayer = playerId;

	if (playerId==localplayer)
	{
//		RemoveParticleEmitterByObject(Player[localplayer].ControlledObject);
//		PlaySoundFX(7, 7);
		CamXPos=xPos;
		CamZPos=zPos;
		CamHeight=Height;
		mySpeed = 0.0f;
		accTime = 0.0f;
		setSpeed = mySpeed;
		xpitchTime=0;
		yawTime=0;
		rollTime=0;
		ControlYaw=0;
		ControlPitch=0;
		ControlRoll=0;
		SavXPos=CamXPos; //Save Camera Setup
		SavYPos=CamZPos;
		SavHeight=CamHeight;
		SavYaw=ControlYaw;
		SavPitch=ControlPitch;
		SavRoll=ControlRoll;
		SavAltYaw=ControlAltYaw;

	}


	sprintf(tempstring, "RESPAWN: Player %i controls object %i", playerId, Player[playerId].ControlledObject);
	WriteErrorLog(tempstring);
	sprintf(tempstring, "RESPAWN: Mesh %i, Type %i, CC %i", Objects[Player[playerId].ControlledObject].ObjectMesh, Objects[Player[playerId].ControlledObject].ObjectType, Objects[Player[playerId].ControlledObject].CommandCenter);
	WriteErrorLog(tempstring);

	//If it's an AI in the same team as the local player, spawn a little higher
/*	if ((Player[playerId].isAI) &&
		(Objects[Player[playerId].ControlledObject].Team==Player[localplayer].Team))
	{
		Objects[Player[playerId].ControlledObject].Height-=100;
		Objects[Player[playerId].ControlledObject].xPos-=100;
	}
*/

}





//============================================================================
// Tries to respawn all dead players.
//============================================================================
void respawnAllPlayers(void)
{
	int curplayer;
	for (curplayer=0; curplayer<numPlayers; curplayer++)
		if ((Player[curplayer].isActive) && (Player[curplayer].doRespawn))
			spawnPlayer(curplayer);
}



//============================================================================
// Creates initial conditions for a new team (place command center, etc.)
//============================================================================
void setupTeam(int TeamNum)
{
	int r, tries;
	int xPos, zPos;
	float xp, zp;
	int borderspace = 9;

	//If there's already a team then stop.
	if (Team[TeamNum].isActive)
		return;

	Team[TeamNum].BuildingsInTeam=0;
	Team[TeamNum].BuildingsKilled=0;
	Team[TeamNum].BuildingsLost=0;
	Team[TeamNum].EnergyAvailable=0;
	Team[TeamNum].EnergyNeeded=0;
	Team[TeamNum].FightersKilled=0;
	Team[TeamNum].FightersLost=0;
	Team[TeamNum].isActive=TRUE;
	Team[TeamNum].isDefeated=FALSE;
	Team[TeamNum].PlayersInTeam=0;
	Team[TeamNum].ResourcesAvailable=3000;
	Team[TeamNum].ResourcesNeeded=0;
//	Team[TeamNum].TeamName="";

	tries = 0;
	do{
		tries++;
		r=random(4);
		if ((r==0) || (r==4))
		{
			xPos=-MapDimension + borderspace;
			zPos=-MapDimension + borderspace;
		}
		if (r==1)
		{
			xPos=MapDimension - borderspace;
			zPos=MapDimension - borderspace;
		}
		if (r==2)
		{
			xPos=MapDimension - borderspace;
			zPos=-MapDimension + borderspace;
		}
		if (r==3)
		{
			xPos=-MapDimension + borderspace;
			zPos=MapDimension - borderspace;
		}

		xp = (float)Vertices[Faces[facexy[MapDimension-xPos-1][MapDimension-zPos]].srcVerts[0]].v.x + (float)Meshes[OBJECTMESH_COMMANDCENTER].sizeX;
		zp = (float)Vertices[Faces[facexy[MapDimension-xPos-1][MapDimension-zPos]].srcVerts[0]].v.z + (float)Meshes[OBJECTMESH_COMMANDCENTER].sizeZ;

	} while ((AddBuilding(OBJECTMESH_COMMANDCENTER, TeamNum, xp, zp, 0) == -1) && (tries<30));
	NumTeams++;
}




//============================================================================
// Opdate object pointers prior to object removal
//============================================================================
//This has caused a lot of bugs, and it took me a while to figure out that I
//have to update all the stored object-ID stuff when modifying the Objects[]
//array directly
void UpdateEmitter(int oldObj, int newObj)
{
int o;
	for(o=0;o<NumEmitters;o++)
	{
		if (ParticleEmitters[o].AttachedToObject==oldObj)
			ParticleEmitters[o].AttachedToObject=newObj;
	}
}

void UpdateLights(int oldObj, int newObj)
{
int o;
	for(o=0;o<NumLights;o++)
	{
		if (Lights[o].AttachedToObject==oldObj)
			Lights[o].AttachedToObject=newObj;
	}
}

void UpdatePlayers(int oldObj, int newObj)
{
int o;
	for(o=0;o<numPlayers;o++)
	{
		if (Player[o].ControlledObject==oldObj)
			Player[o].ControlledObject=newObj;
	}
}

void UpdateObjectTargets(int oldObj, int newObj)
{
int o;
	for(o=0;o<NumObjects;o++)
	{
		if (Objects[o].targetObject==oldObj)
			Objects[o].targetObject=newObj;
		if (Objects[o].FiringObject==oldObj)
			Objects[o].FiringObject=newObj;
	}
}

//============================================================================
// Removes an object
//============================================================================
void RemoveObject(int ObjectId, FxBool NetSync)
{

sprintf(tempstring, "Removed Object %i", ObjectId);
WriteErrorLog(tempstring);


//if we're a parent Object, remove all childs too!
if (Objects[ObjectId].ChildObject0!=-1)
	Objects[Objects[ObjectId].ChildObject0].TimeToLive = 1;
if (Objects[ObjectId].ChildObject1!=-1)
	Objects[Objects[ObjectId].ChildObject1].TimeToLive = 1;
if (Objects[ObjectId].ChildObject2!=-1)
	Objects[Objects[ObjectId].ChildObject2].TimeToLive = 1;

if (NetSync)
	if (!DemoMode)
		if (!myServer.isActive)
			NetRemoveObject(ObjectId);

RemoveParticleEmitterByObject(ObjectId);
MuteObject(ObjectId);
#ifdef USE_GL
RemoveLightByObject(ObjectId);
#endif
//Objects[NumObjects].ObjectId = -1;

//for(i=ObjectId;i<(NumObjects);i++)
//	{
//	UpdateEmitter( (i+1), i );	//Tell the particle emitters about the change
//	UpdatePlayers( (i+1), i );	//Update Player[].ControlledObject too
//	UpdateObjectTargets( (i+1), i );	//... and guided missiles should be aware of the change too
//	UpdateLights( (i+1), i );	//Almost forgot the lights ;-)
//	Objects[i]=Objects[i+1];
//	}
//NumObjects--;

Objects[ObjectId].ObjectId = -1;
if (Player[localplayer].ControlledObject==ObjectId)
	Player[localplayer].ControlledObject=-1;

//if (ObjectId >= (NumObjects-1))
//	NumObjects--;

do {
	if ((Objects[NumObjects-1].ObjectId != NumObjects-1) && (NumObjects>0))
	{
		Objects[NumObjects-1].ObjectId=-1;
		NumObjects--;
	}
	else
		break;
} while (Objects[NumObjects-1].ObjectId != NumObjects-1);	//was ==-1


/*
sprintf(tempstring, "\n------------FREAKIN' SITUATION AFTER REMOVEOBJECT:------------");
WriteErrorLog(tempstring);
sprintf(tempstring, "MaxNum=%i: Num=%i", MaxNumObjects, NumObjects);
WriteErrorLog(tempstring);
for (fu=0; fu<MaxNumObjects; fu++)
{
	sprintf(tempstring, "No. %i: ID=%i Mesh=%i", fu, Objects[fu].ObjectId, Objects[fu].ObjectMesh);
	WriteErrorLog(tempstring);
}
sprintf(tempstring, "...and I removed object %i from the mess\n", ObjectId);
WriteErrorLog(tempstring);
*/


}



//============================================================================
// Removes ALL objects
//============================================================================
void RemoveAllObjects(void)
{
int i;

for(i=0;i<(NumLights);i++)
RemoveLight(i);

//for(i=0;i<(NumParticles);i++)
//RemoveParticle(i);
NumParticles = 0;

for(i=0;i<(NumEmitters);i++)
ParticleEmitters[i].Active=FXFALSE;
//RemoveParticleEmitter(i);

NumEmitters = 0;

for(i=0;i<MaxNumObjects;i++)
{
	if (Objects[i].ObjectId==i)
	{
//		RemoveParticleEmitterByObject(i);
//		RemoveLightByObject(i);
		RemoveObject(i, FXTRUE);
//		Objects[i].ObjectId = -1;
//		Objects[i].TimeToLive = 0;
	}
}

for(i=0;i<(numPlayers);i++)
{
	Player[i].isActive=FXFALSE;
	Player[i].ControlledObject=-1;
	Player[i].isHuman=FXFALSE;;
	Player[i].isNetwork=FXFALSE;;
	Player[i].isAI=FXFALSE;;
	Player[i].isTank=FXFALSE;;
}

NumObjects=0;
MaxNumObjects=0;
Objects[0].ObjectId=-1;
NumLights=0;
NumParticles=0;
NumEmitters=0;
NumTeams=0;
localplayer=0;
numPlayers=0;
Player[localplayer].ControlledObject=-1;

Team[0].isActive=FXFALSE;
Team[1].isActive=FXFALSE;
Team[2].isActive=FXFALSE;
Team[3].isActive=FXFALSE;
Team[4].isActive=FXFALSE;


}



//============================================================================
// Check for collision between an object and the landscape
//============================================================================
int DetectCollision(float *x, float *y, float *z, float offset)
{
float h1,h2,h3,h4, x0, x1, midpoint;
int xg, zg;
float xa, za;
float xpos, zpos;
int CrossedTheLine;
float TheMiddle;
int TileType;

	if (*x>0.0f)
		xg = (int)(*x/scalefactor)+1;
	else
		xg = (int)(*x/scalefactor);
	if (*z>0.0f)
		zg = (int)(*z/scalefactor)+1;
	else
		zg = (int)(*z/scalefactor);

		xa = (float)(xg*scalefactor);
		za = (float)(zg*scalefactor);

h1= Vertices[Faces[facexy[MapDimension-xg-1][MapDimension-zg]].srcVerts[0]].v.y;
h2= Vertices[Faces[facexy[MapDimension-xg-1][MapDimension-zg]].srcVerts[1]].v.y;
h3= Vertices[Faces[facexy[MapDimension-xg-1][MapDimension-zg]].srcVerts[2]].v.y;
h4= Vertices[Faces[facexyB[MapDimension-xg-1][MapDimension-zg]].srcVerts[1]].v.y;


//     h4---+---h3
//     |    |   |
//zpos +====X===+
//     |    |   |
//     h2---+---h1
//        xpos


xpos = (float)(xa-*x);
zpos = (float)(za-*z);

if (xpos<0.0f)
	xpos*=-1.0f;
if (zpos<0.0f)
	zpos*=-1.0f;


//See !work2.jpg on why and how we do this

if ((xg+zg)%2 == 0)
	TileType=1;
else
	TileType=0;

if (TileType==0)
{
	TheMiddle = h1 + (h4-h1) * 0.5f;
	if (xpos>=zpos)
		CrossedTheLine=1;
	else
		CrossedTheLine=0;

	if (CrossedTheLine==1)
		h3 = TheMiddle + (TheMiddle-h2);
	else
		h2 = TheMiddle + (TheMiddle-h3);
}
else
{
	TheMiddle = h2 + (h3-h2) * 0.5f;
	if ((scalefactor-xpos) >=zpos)
		CrossedTheLine=1;
	else
		CrossedTheLine=0;

	if (CrossedTheLine==1)
		h4 = TheMiddle + (TheMiddle-h1);
	else
		h1 = TheMiddle + (TheMiddle-h4);
}


	x0 = (float)(h1 + (h3 - h1)*(float)(zpos/scalefactor));
	x1 = (float)(h2 + (h4 - h2)*(float)(zpos/scalefactor));
	midpoint = (float)(x0 + (x1 - x0)*(float)(xpos/scalefactor) + 0.1f);

//	sprintf(tempstring, "x=%f\n z=%f\n xpos=%f\n zpos=%f\n h1=%f\n h2=%f\n h3=%f\n h4=%f\n x0=%f\n x1=%f\n mid=%f\n TileType=%i\n CrossedLine=%i", *x, *z, xpos/scalefactor, zpos/scalefactor, h1, h2, h3, h4, x0, x1, midpoint, TileType, CrossedTheLine);
//	AddMessage(tempstring, 255, 255, 255);

	if (-*y<midpoint-offset)
	{
		*y=-(midpoint-offset);
		return (1);
	}
	else
	{
		return (0);
	}

}


//============================================================================
// Same as DetectCollision, but returns terrain height
//============================================================================
float TerrainHeight(float *x, float *y, float *z)
{
float h1,h2,h3,h4, x0, x1, midpoint;
int xg, zg;
float xa, za;
float xpos, zpos;
int CrossedTheLine;
float TheMiddle;
int TileType;

	if (*x>0.0f)
		xg = (int)(*x/scalefactor)+1;
	else
		xg = (int)(*x/scalefactor);
	if (*z>0.0f)
		zg = (int)(*z/scalefactor)+1;
	else
		zg = (int)(*z/scalefactor);

		xa = (float)(xg*scalefactor);
		za = (float)(zg*scalefactor);


h1= Vertices[Faces[facexy[MapDimension-xg-1][MapDimension-zg]].srcVerts[0]].v.y;
h2= Vertices[Faces[facexy[MapDimension-xg-1][MapDimension-zg]].srcVerts[1]].v.y;
h3= Vertices[Faces[facexy[MapDimension-xg-1][MapDimension-zg]].srcVerts[2]].v.y;
h4= Vertices[Faces[facexyB[MapDimension-xg-1][MapDimension-zg]].srcVerts[1]].v.y;




//     h4---+---h3
//     |    |   |
//zpos +====X===+
//     |    |   |
//     h2---+---h1
//        xpos


xpos = (float)(xa-*x);
zpos = (float)(za-*z);

if (xpos<0.0f)
	xpos*=-1.0f;
if (zpos<0.0f)
	zpos*=-1.0f;


//See !work2.jpg on why and how we do this

if ((xg+zg)%2 == 0)
	TileType=1;
else
	TileType=0;

if (TileType==0)
{
	TheMiddle = h1 + (h4-h1) * 0.5f;
	if (xpos>=zpos)
		CrossedTheLine=1;
	else
		CrossedTheLine=0;

	if (CrossedTheLine==1)
		h3 = TheMiddle + (TheMiddle-h2);
	else
		h2 = TheMiddle + (TheMiddle-h3);
}
else
{
	TheMiddle = h2 + (h3-h2) * 0.5f;
	if ((scalefactor-xpos) >=zpos)
		CrossedTheLine=1;
	else
		CrossedTheLine=0;

	if (CrossedTheLine==1)
		h4 = TheMiddle + (TheMiddle-h1);
	else
		h1 = TheMiddle + (TheMiddle-h4);
}


	x0 = (float)(h1 + (h3 - h1)*(float)(zpos/scalefactor));
	x1 = (float)(h2 + (h4 - h2)*(float)(zpos/scalefactor));
	midpoint = (float)(x0 + (x1 - x0)*(float)(xpos/scalefactor)); //was +2.0f

	return (midpoint);

}






//============================================================================
// Check for collision between two objects
//============================================================================
FxBool CheckObjectCollision(int wobject, int hobject)
{
	float i, xmovement, ymovement, zmovement;
	float tempHeight, txPos, tzPos;
	int ObjectId;

	if (Objects[wobject].Team == Objects[hobject].Team)
	{
		if( (Objects[wobject].ObjectMesh == OBJECTMESH_COMMANDCENTER)
		 || (Objects[hobject].ObjectMesh == OBJECTMESH_COMMANDCENTER) )
		 return (FXFALSE);
	}

	if ((Objects[wobject].ObjectType==OBJECTTYPE_DOLLY) || (Objects[hobject].ObjectType==OBJECTTYPE_DOLLY))
		return (FXFALSE);

	if	((wobject==hobject) || (Objects[wobject].FiringObject==hobject))
		return (FXFALSE);
	if	( (Within(Objects[wobject].xPos, Objects[hobject].xPos+Objects[hobject].sizeX, Objects[hobject].xPos-Objects[hobject].sizeX)) &&
		(Within(Objects[wobject].zPos, Objects[hobject].zPos+Objects[hobject].sizeZ, Objects[hobject].zPos-Objects[hobject].sizeZ)) &&
		(Within(Objects[wobject].Height, Objects[hobject].Height+Objects[hobject].sizeY, Objects[hobject].Height-Objects[hobject].sizeY)) )
		{
			return (FXTRUE);
		}
	else	//The object may have hit us between two frames, so check that now
	{
	  ObjectId = wobject;
	  if (Objects[ObjectId].Speed != 0.0f)
	  {
		tempHeight =Objects[ObjectId].Height;
		txPos =Objects[ObjectId].xPos;
		tzPos =Objects[ObjectId].zPos;
		xmovement=(float)(Sin[(int)(Objects[ObjectId].Yaw * Deg)]*Cos[(int)(Objects[ObjectId].Pitch * Deg)] * (Objects[ObjectId].Speed/500.0f) * scalefactor * g_speedfactor);
		zmovement=(float)(Cos[(int)(Objects[ObjectId].Yaw * Deg)]*Cos[(int)(Objects[ObjectId].Pitch * Deg)] * (Objects[ObjectId].Speed/500.0f) * scalefactor * g_speedfactor);
		ymovement=(float)(Sin[(int)(Objects[ObjectId].Pitch * Deg)]* (Objects[ObjectId].Speed/500.0f) * scalefactor * g_speedfactor);
		for (i=0.0f; i<5.0f; i++)	//check 5 steps
		{
			tempHeight -= ymovement;
			txPos += xmovement;
			tzPos += zmovement;
			if	( (Within(txPos, Objects[hobject].xPos+Objects[hobject].sizeX, Objects[hobject].xPos-Objects[hobject].sizeX)) &&
				(Within(tzPos, Objects[hobject].zPos+Objects[hobject].sizeZ, Objects[hobject].zPos-Objects[hobject].sizeZ)) &&
				(Within(tempHeight, Objects[hobject].Height+Objects[hobject].sizeY, Objects[hobject].Height-Objects[hobject].sizeY)) )
			{
				return (FXTRUE);
			}

		}
	  }

	  ObjectId = hobject;
	  if (Objects[ObjectId].Speed != 0.0f)
	  {
		tempHeight =Objects[ObjectId].Height;
		txPos =Objects[ObjectId].xPos;
		tzPos =Objects[ObjectId].zPos;
		xmovement=(float)(Sin[(int)(Objects[ObjectId].Yaw * Deg)]*Cos[(int)(Objects[ObjectId].Pitch * Deg)] * (Objects[ObjectId].Speed/500.0f) * scalefactor * g_speedfactor);
		zmovement=(float)(Cos[(int)(Objects[ObjectId].Yaw * Deg)]*Cos[(int)(Objects[ObjectId].Pitch * Deg)] * (Objects[ObjectId].Speed/500.0f) * scalefactor * g_speedfactor);
		ymovement=(float)(Sin[(int)(Objects[ObjectId].Pitch * Deg)]* (Objects[ObjectId].Speed/500.0f) * scalefactor * g_speedfactor);
		for (i=0.0f; i<5.0f; i++)	//check 5 steps
		{
			tempHeight -= ymovement;
			txPos += xmovement;
			tzPos += zmovement;
			if	( (Within(txPos, Objects[wobject].xPos+Objects[wobject].sizeX, Objects[wobject].xPos-Objects[wobject].sizeX)) &&
				(Within(tzPos, Objects[wobject].zPos+Objects[wobject].sizeZ, Objects[wobject].zPos-Objects[wobject].sizeZ)) &&
				(Within(tempHeight, Objects[wobject].Height+Objects[wobject].sizeY, Objects[wobject].Height-Objects[wobject].sizeY)) )
			{
				return (FXTRUE);
			}

		}
	  }
		
		return (FXFALSE);
	}
}

//============================================================================
// Check if weapon weaponId has hit an object, return ID of hit object (-1 if false)
//============================================================================
int CheckWeaponCollision(int weaponId)
{
	int i;
	// If we don't own the ship or the missile, don't check for collision,
	// only the ship-/missile-owner should do that!
	if (Objects[weaponId].OwningNetworkClient == localplayer)
	for (i=0; i<NumObjects; i++)
	{
		//Player can't shoot down weapons *except missiles*
		if ((((Objects[i].ObjectType!=OBJECTTYPE_WEAPON)&&(Objects[i].ObjectType!=OBJECTTYPE_DOLLY)) || (Objects[i].ObjectMesh==OBJECTMESH_MISSILE)) && (Objects[i].ObjectId != -1))
			if ((Objects[weaponId].FiringObject != i) && (!Objects[i].isDocked))	//Missile doesn't harm to it's launching object or to docked ships
				if (Objects[i].ParentObject != Objects[weaponId].FiringObject)		//A parent's weapon doesn't harm it's childs
					if (CheckObjectCollision(weaponId, i))
						return (i);
	}
return (-1);
}


//============================================================================
// Check if ship shipId has hit an another ship or a building
//============================================================================
int CheckShipCollision(int shipId)
{
	int i;
	// If we don't own the ship or the missile, don't check for collision,
	// only the ship-/missile-owner should do that!
	if (Objects[shipId].OwningNetworkClient == localplayer)
	if ((!Objects[shipId].isDocked) && (!Objects[shipId].isStarting) && (!Objects[shipId].isLanding))
	for (i=0; i<NumObjects; i++)
	{
		if (((Objects[i].ObjectType==OBJECTTYPE_BUILDING) || (Objects[i].ObjectType==OBJECTTYPE_SHIP)) && (Objects[i].ObjectId != -1))
			if (CheckObjectCollision(shipId, i))
			{
				sprintf(tempstring, "Ship %i has collided with Object %i, Mesh %i", shipId, i, Objects[i].ObjectMesh);
				WriteErrorLog(tempstring);
				return (i);
			}
	}
return (-1);
}




//============================================================================
// Make a landscape grid, assign height info from NewFractal() and texture it
//============================================================================
void CreateLandscape(int CurrentTerrain)
{
	int o,u;

	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	sprintf(tempstring, "\\c(255 255 255)Building Terrain Grid");
	mytext.Size(10);
	mytext.Draw(20, 20, tempstring);
	glFlush();
	SwapBuffers( DR_HDC ); 
	
	
	
	
curface = 0;
curvert = 0;
tmode=0;

Waterplane[0].v.x = (float)(1.0f-MapDimension)*scalefactor;
Waterplane[0].v.z = (float)((-1)*MapDimension)*scalefactor;

Waterplane[1].v.x = (float)(MapDimension+1.0f)*scalefactor;
Waterplane[1].v.z = (float)((-1)*MapDimension)*scalefactor;

Waterplane[2].v.x = (float)(1.0f-MapDimension)*scalefactor;
Waterplane[2].v.z = (float)(MapDimension-2.0f)*scalefactor;

Waterplane[3].v.x = (float)(MapDimension+1.0f)*scalefactor;
Waterplane[3].v.z = (float)(MapDimension-2.0f)*scalefactor;


for (o=((-1)*MapDimension);o<=MapDimension+1;o++)
{
  for (u=((-1)*MapDimension);u<=(MapDimension-1);u++)
  {

 	Vertices[curvert].OwnerObject = -1; //ID -1 is the terrain

	Vertices[curvert].v.x = (float)(1.0f+o)*scalefactor;		//A

//	Vertices[curvert].v.y = (heightfield[o+MapDimension][u+MapDimension] / 10.0f)*scalefactor;
	Vertices[curvert].v.y = (heightfield[(o+MapDimension + ((u+MapDimension)*64))] / 10.0f)*scalefactor;
//	Vertices[curvert].v.y = (float)random(500)/500.0f*1.0f;
//	Vertices[curvert].v.y = 0.0f;

	Vertices[curvert].v.z = (float)(0.0f+u)*scalefactor;
 	Vertices[curvert].v.w = 1.0f;
  tmode++;
  if (tmode==5)
	  tmode=1;

//	Vertices[curvert].Alpha = (blendfield[o+MapDimension][u+MapDimension]);

	Vertices[curvert].v.r = 255;
	Vertices[curvert].v.g = 255;
	Vertices[curvert].v.b = 255;
if (tmode==1)	// this sets the texture orientation.
{				// -pretty much obsolete since this is overridden later.
    Vertices[curvert].v.s = 0.0f, Vertices[curvert].v.t = 0.0f;
} else if (tmode==2)
{
    Vertices[curvert].v.s = 1.0f, Vertices[curvert].v.t = 0.0f;
} else if (tmode==3)
{
    Vertices[curvert].v.s = 0.0f, Vertices[curvert].v.t = 1.0f;
} else if (tmode==4)
{
    Vertices[curvert].v.s = 1.0f, Vertices[curvert].v.t = 1.0f;
}


	curvert++;

  }
}

numverts=curvert;



curface = 0;
curquad=0;
for (o=0;o<=(2*MapDimension);o++)
{
  for (u=0;u<=((2*MapDimension)-3);u++)
  {
	Faces[curface].Transparent=FALSE;
	Faces[curface].Texture = o;
	Faces[curface].Texture2 = u;

	Faces[curface].OwnerObject=-1;	//ID -1 is the terrain
	Faces[curface].srcVerts[0]=(2*MapDimension)*o + u;		//A
	Faces[curface].srcVerts[1]=(2*MapDimension)*(o+1) + u;	//B
	Faces[curface].srcVerts[2]=(2*MapDimension)*o + (u+1);//D

	facexy[o][u]=curface;
	Faces[curface].TextureMode=1;
	curface++;


	Faces[curface].Transparent=FALSE;
	Faces[curface].Texture = o;
	Faces[curface].Texture2 = u;

	Faces[curface].OwnerObject=-1;
	Faces[curface].srcVerts[2]=(2*MapDimension)*o + (u+1);	//D
	Faces[curface].srcVerts[1]=(2*MapDimension)*(o+1) + (u+1);//C
	Faces[curface].srcVerts[0]=(2*MapDimension)*(o+1) + u;	//B

	facexyB[o][u]=curface;
	Faces[curface].TextureMode=2;
	curface++;


//ADC
  }
}
numfaces=curface;
numLSverts = numverts;
numLSfaces = numfaces;

if (use_lighing)
PrecomputeLandscapeVertexNormals();	//Important for Lighting and stuff

if (!NoTexCalcs)
	LoadTerrainTileset(CurrentTerrain);	//Load the 4 textures for the landscape
TerrainType=CurrentTerrain;

if (!NoTexCalcs)
GenerateTerrainTextures();			//mix the 4 basic tiles together to create one hugeass texture

NumTrees=0;
AddTrees(1);


}


//============================================================================
// Create fractal landscape height information
//============================================================================
void NewFractal(void)
{	
	long x, z;
	long bsize, csize;
	long i, r;
	long MAPSIZE = (MapDimension+1)*2;
	int startvalue=0;

	for(i=0; i<64; i++) 
	{
		for(r=0; r<64; r++) 
		{
			heightfield[(i + (r*64))] = 0.0f;
		}
	}

	randomize(RandomSeed);

//	r = 64; // Roughness
	r=(long)(30+random(10));

	bsize = MAPSIZE;

	if (bsize>0)
	for(i=0; i<9; i++) 
	{

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		sprintf(tempstring, "\\c(255 255 255)Generating Fractal Terrain: \\c(128 128 128)%i%s", (int)(100.0f*(float)(i)/8), " percent done.");
		mytext.Size(10);
		mytext.Draw(20, 20, tempstring);
		glFlush();
		SwapBuffers( DR_HDC ); 


	if (bsize>0)
		for(x=0; x<MAPSIZE; x+=bsize) {
			for(z=0; z<MAPSIZE; z+=bsize) {			
				heightfield[(x + (z*64))] += rand()%(r+1) - r/2;

			}
		}

		if(i>2) {
			r = r/2;
		}

		csize = bsize/2;

		if(csize > 0) {
			for(x=0; x<MAPSIZE; x+=bsize) {
				for(z=0; z<MAPSIZE; z+=bsize) {


					heightfield[(x+csize + (z*64))] = (heightfield[(x + (z*64))] + heightfield[(x+bsize + (z*64))])/2;
					heightfield[(x + ((z+csize)*64))] = (heightfield[(x + (z*64))] + heightfield[(x + ((z+bsize)*64))])/2;
					heightfield[(x+csize + ((z+csize)*64))] = (heightfield[(x + (z*64))] + 
												   heightfield[(x+bsize + (z*64))] +
												   heightfield[(x+bsize + ((z+bsize)*64))] +
												   heightfield[(x + ((z+bsize)*64))])/4.0f;


				}
			}
		}

		bsize /= 2;
	}

//	if (bsize>0)
//		startvalue = 0;
//	else 
//		startvalue = 3;
	if (bsize>0)
	for(x=0+startvalue; x<(MAPSIZE-startvalue); x++) {
		for(z=0+startvalue; z<(MAPSIZE-startvalue); z++) {

			heightfield[(x + (z*64))] = (heightfield[(x-1 + ((z-1)*64))] + 
								  heightfield[(x-1 + (z*64))] + 
								  heightfield[(x-1 + ((z+1)*64))] + 
								  heightfield[(x + ((z-1)*64))] + 
								  heightfield[(x + (z*64))] + 
								  heightfield[(x + ((z+1)*64))] + 
								  heightfield[(x+1 + ((z-1)*64))] + 
								  heightfield[(x+1 + (z*64))] + 
								  heightfield[(x+1 + ((z+1)*64))])/9.0f;
		}
	}



/*
  //Optional: "Cutting" the terrain at a certain level, for water, etc
	for(i=0; i<MAPSIZE; i++) {
		for(r=0; r<MAPSIZE; r++) {
			if(heightfield[i][r] < 0.0f)
				heightfield[i][r] = 0.0f;
		}
	}
*/


}




//============================================================================
// Directly create fractal sky texture
//============================================================================
void NewFractalSky1(void)
{	
	long x, z;
	long bsize, csize;
	long i, r;
	int startvalue=0;

	for(i=0; i<256; i++) 
	{
		for(r=0; r<256; r++) 
		{
			sky1[(i + (r*256))] = 0.0f;
		}
	}


	r = 70; // Roughness
//	r=(long)(40+random(40));

	bsize = 256;

	if (bsize>0)
	for(i=0; i<9; i++) 
	{
	if (bsize>0)
		for(x=0; x<256; x+=bsize) {
			for(z=0; z<256; z+=bsize) {			
				sky1[(x + (z*256))] += (float)(rand()%(r+1) - r/2);
				if ((x==0) || (x==255) || (z==0) || (z==255))
					sky1[(x + (z*256))] = 0.0f;
			}
		}

		if(i>2) {
			r = r/2;
		}

		csize = bsize/2;

		if(csize > 0) {
			for(x=0; x<256; x+=bsize) {
				for(z=0; z<256; z+=bsize) {			
					sky1[(x+csize + (z*256))] = (float)((sky1[(x + (z*256))] + sky1[(x+bsize + (z*256))])/2.0f);
					sky1[(x + ((z+csize)*256))] = (float)((sky1[(x + (z*256))] + sky1[(x + ((z+bsize)*256))])/2.0f);
					sky1[(x+csize + ((z+csize)*256))] = (float)((sky1[(x + (z*256))] + 
												   sky1[(x+bsize + (z*256))] +
												   sky1[(x+bsize + ((z+bsize)*256))] +
												   sky1[(x + ((z+bsize)*256))])/4.0f);
				}
			}
		}

		bsize /= 2;
	}


	if (bsize>0)
		startvalue = 0;
	else 
		startvalue = 4;

	if (bsize>0)
	for(x=0+startvalue; x<256-startvalue; x++) {
		for(z=0+startvalue; z<256-startvalue; z++) {
			sky1[(x + (z*256))] = (float)((sky1[(x-1 + ((z-1)*256))] + 
								  sky1[(x-1 + ((z)*256))] + 
								  sky1[(x-1 + ((z+1)*256))] + 
								  sky1[(x + ((z-1)*256))] + 
								  sky1[(x + ((z)*256))] + 
								  sky1[(x + ((z+1)*256))] + 
								  sky1[(x+1 + ((z-1)*256))] + 
								  sky1[(x+1 + ((z)*256))] + 
								  sky1[(x+1 + ((z+1)*256))])/9.0f);
		}
	}



  //Optional: "Cutting" the fractal data at 0 level
	for(i=0; i<256; i++) {
		for(r=0; r<256; r++) {
			if(sky1[(i + (r*256))] < 1.0f)
				sky1[(i + (r*256))] = 1.0f;
			if(sky1[(i + (r*256))] > 255.0f)
				sky1[(i + (r*256))] = 255.0f;
		}
	}


	//Darken things
	if ((current_weather.WeatherType==WEATHERTYPE_STORM) ||
		(current_weather.WeatherType==WEATHERTYPE_CLOUDED))
	for(i=0; i<256; i++) 
	{
		for(r=0; r<256; r++) 
		{
			if (current_weather.WeatherType==WEATHERTYPE_STORM)
			sky1[(i + (r*256))] += 100.0f;
			else
			sky1[(i + (r*256))] += 50.0f;
		}
	}


}

void NewFractalSky2(void)
{	
	long x, z;
	long bsize, csize;
	long i, r;
	int startvalue=0;

	for(i=0; i<256; i++) 
	{
		for(r=0; r<256; r++) 
		{
			sky2[(i + (r*256))] = 0.0f;
		}
	}


	r = 50; // Roughness
//	r=(long)(40+random(40));

	bsize = 256;

	if (bsize>0)
	for(i=0; i<9; i++) 
	{
	if (bsize>0)
		for(x=0; x<256; x+=bsize) {
			for(z=0; z<256; z+=bsize) {			
				sky2[(x + (z*256))] += (float)(rand()%(r+1) - r/2);
				if ((x==0) || (x==255) || (z==0) || (z==255))
					sky2[(x + (z*256))] = 0.0f;
			}
		}

		if(i>2) {
			r = r/2;
		}

		csize = bsize/2;

		if(csize > 0) {
			for(x=0; x<256; x+=bsize) {
				for(z=0; z<256; z+=bsize) {			
					sky2[(x+csize + (z*256))] = (float)((sky2[(x + (z*256))] + sky2[(x+bsize + (z*256))])/2.0f);
					sky2[(x + ((z+csize)*256))] = (float)((sky2[(x + (z*256))] + sky2[(x + ((z+bsize)*256))])/2.0f);
					sky2[(x+csize + ((z+csize)*256))] = (float)((sky2[(x + (z*256))] + 
												   sky2[(x+bsize + (z*256))] +
												   sky2[(x+bsize + ((z+bsize)*256))] +
												   sky2[(x + ((z+bsize)*256))])/4.0f);
				}
			}
		}

		bsize /= 2;
//		if (bsize==0)
//			exit;
	}

	if (bsize>0)
		startvalue = 0;
	else 
		startvalue = 4;

	if (bsize>0)
	for(x=0+startvalue; x<256-startvalue; x++) {
		for(z=0+startvalue; z<256-startvalue; z++) {
			sky2[(x + (z*256))] = (float)((sky2[(x-1 + ((z-1)*256))] + 
								  sky2[(x-1 + ((z)*256))] + 
								  sky2[(x-1 + ((z+1)*256))] + 
								  sky2[(x + ((z-1)*256))] + 
								  sky2[(x + ((z)*256))] + 
								  sky2[(x + ((z+1)*256))] + 
								  sky2[(x+1 + ((z-1)*256))] + 
								  sky2[(x+1 + ((z)*256))] + 
								  sky2[(x+1 + ((z+1)*256))])/9.0f);
		}
	}



  //"Cutting" the fractal data at 0 level
	for(i=0; i<256; i++) {
		for(r=0; r<256; r++) {
			if(sky2[(i + (r*256))] < 1.0f)
				sky2[(i + (r*256))] = 1.0f;
			if(sky2[(i + (r*256))] > 255.0f)
				sky2[(i + (r*256))] = 255.0f;
		}
	}

	//Darken things
	if ((current_weather.WeatherType==WEATHERTYPE_STORM) ||
		(current_weather.WeatherType==WEATHERTYPE_CLOUDED))
	for(i=0; i<256; i++) 
	{
		for(r=0; r<256; r++) 
		{
			if (current_weather.WeatherType==WEATHERTYPE_STORM)
			sky2[(i + (r*256))] += 50.0f;
			else
			sky2[(i + (r*256))] += 20.0f;
		}
	}

}







//============================================================================
// Kills a ship, and respawns it at the home command center
//============================================================================
void KillShip(int curobj)
{
	int playerId;

					if (!GameHalted)
						Team[Objects[curobj].Team].FightersLost++;

					RemoveParticleEmitterByObject(curobj);
					playerId = Objects[curobj].ControllingPlayer;
					StopSoundFX(18);	//Stop stall alarm
					StopSoundFX(19);	//Stop "Dive" sound
					Player[playerId].doRespawn=FXTRUE;

					RemoveObject(curobj, FXTRUE);			//remove dead ship

}



//============================================================================
// Apply damage to a hit object, and handle object destroying / effects
//============================================================================
void DamageObject(int objectId, int damage, FxBool MarkObject, FxBool NetSync)
{
int TempEmitter, TempObj;
	
	if ( (Cheat) && (Player[localplayer].ControlledObject==objectId) )
		return;


	//if we're just a child object, damage only the parent!
	if (Objects[objectId].ParentObject!=-1)
		objectId = Objects[objectId].ParentObject;

	if (NetSync)
		if (!DemoMode)
			NetDamageObject(objectId, damage, MarkObject);

	if (MarkObject)	//Marker missiles only mark the target, and do not damage it
	{
		Objects[objectId].isMarked = FXTRUE;
		return;
	}


	//If the base is getting under attack, notify the player
	if ( (Objects[objectId].ObjectMesh==OBJECTMESH_COMMANDCENTER) && 
		(Objects[objectId].Shield>0) && 
		(Objects[objectId].Shield<Objects[objectId].MaxShield-50) &&
		(Objects[objectId].Shield+damage>Objects[objectId].MaxShield-50) &&
		(Objects[objectId].Team==Player[localplayer].Team) )
	{
		PlaySoundFX(24, 24);
	}

	if ( (NetSync) && (Objects[objectId].OwningNetworkClient!=localplayer) )
		return;

	if (Objects[objectId].Health!=0)
	Objects[objectId].Shield-=damage;	//Shields damaged

	if (Objects[objectId].Shield<0)		//Shields failed?
	{
		Objects[objectId].Health+=Objects[objectId].Shield;	//Substract excess shield damage
															//and add it to the hull damage
		Objects[objectId].Shield=0;							//Set shields = 0 when done

		// Shields are down, object is taking structural damage, so add some smoke
		if ( (Objects[objectId].Health < Objects[objectId].MaxHealth) &&
			((Objects[objectId].Health+damage) >= Objects[objectId].MaxHealth) &&
			(Player[localplayer].ControlledObject!=objectId) )
		{
			TempEmitter= AddParticleEmitter( objectId,
				1, 100, -1, 4, 0.0f, -Objects[objectId].xPos, -Objects[objectId].zPos, Objects[objectId].Height, 0.0f, 90.0f, -1, 0.1f, 0.3f, 0.3f, 80.0f, 150.0f, 40.0f,
				80, 30, 30, 6);//color
			ParticleEmitters[TempEmitter].isAtCenter = FXTRUE;
			//If the Command center is burning, tell the player to get the hell over here!!
			if ( (Objects[objectId].ObjectMesh==OBJECTMESH_COMMANDCENTER) &&
				(Objects[objectId].Team==Player[localplayer].Team) )
			{
				PlaySoundFX(25, 25);
			}
		}

		if (Objects[objectId].Health<0)		//If it's dead...
		{
			Objects[objectId].Health=0;		//... set health to 0...
		}
	}
	if ((Objects[objectId].Health<=0) && (Objects[objectId].OwningNetworkClient==localplayer))
	{
			RemoveParticleEmitterByObject(objectId);
										//... do some explosion effects
			if (Objects[objectId].ObjectType==OBJECTTYPE_SHIP)
			{
				if (Player[localplayer].ControlledObject!=objectId)
					Explosion3(Objects[objectId].xPos, Objects[objectId].zPos, Objects[objectId].Height);
				if (use_midas!=0)
					PlayPositionalSound(11, 11, Objects[objectId].xPos, Objects[objectId].zPos, Objects[objectId].Height,1100.0f);
			}
			if (Objects[objectId].ObjectType==OBJECTTYPE_BUILDING)
			{
				Explosion4(Objects[objectId].xPos, Objects[objectId].zPos, Objects[objectId].Height);
				if (use_midas!=0)
					PlayPositionalSound(12, 12, Objects[objectId].xPos, Objects[objectId].zPos, Objects[objectId].Height,1100.0f);
			}
											//... and remove it after a few frames
			if (Objects[objectId].ObjectType==OBJECTTYPE_SHIP)
			{
				if (damage>=DAMAGE_MISSILE)
//					Objects[objectId].TimeToLive=3;
					KillShip(objectId);
				else
				{
					Objects[objectId].isDiving=FXTRUE;
					if (Player[localplayer].ControlledObject!=objectId)
						AddParticleEmitter(	objectId,
						1, 100, 1, -1, 0.0f, 0.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 60.0f, 50.0f, 100.0f,
						100, 50, 50, 6);//color
				}
			}
			else
			{
				if ((Objects[objectId].ObjectMesh==OBJECTMESH_COMMANDCENTER) ||
					(Objects[objectId].ObjectMesh==OBJECTMESH_COMMANDCENTERMENU))
				{
					GameHalted=FXTRUE;
					Objects[objectId].Health=Objects[objectId].MaxHealth;
					Objects[objectId].Shield=Objects[objectId].MaxShield;
					//Place all ships to their respective homebases...
					for (TempObj=0;TempObj<NumObjects; TempObj++)
						if (Objects[TempObj].ObjectType==OBJECTTYPE_SHIP)
						{
							if (Player[localplayer].ControlledObject==TempObj)
							{
								if (Objects[objectId].Team==Player[localplayer].Team)
									sprintf(GameHaltMsg, "\\a(255 0 0 255)Your Team has been defeated!");
								else
									sprintf(GameHaltMsg, "\\a(0 255 0 255)Your Team was victorious!");
							}
							KillShip(TempObj);
						}
				}
				else
					Objects[objectId].TimeToLive=3;
			}

	}
}



/*
void GetRelativeAngle(float x1, float y1, float z1, float x2, float y2, float z2)
{
float c;
float a,b;

a = x2-x1;
b = y2-y1;
c = GetDistance(a, b);

if (b>0)
TempYaw = (float)asin((double)(a/c)); //This works for 0-179 degrees only
else
TempYaw = (float)((pi)-asin((double)(a/c))); //for 180-359 degrees, use this.

a = z2-z1;
b = c;
c = GetDistance(a, b);
TempDist = c;
TempPitch = (float)asin((double)(a/c));  // This should work all the time.
}
*/


//============================================================================
// Calc tank's pitch and roll according to landscape angle
//============================================================================
void GroundVehicleOrientation(int ObjectId)
{
	float x1, x2, x3, x4, x5, y1, y2, y3, y4, y5, z1, z2, z3, z4, z5;
	float a, b, TempRoll;
	float NegYaw;
	

// positon of the vehicle
	x1 = Objects[ObjectId].xPos;
	y1 = Objects[ObjectId].Height;
	z1 = Objects[ObjectId].zPos;

	NegYaw = FixAngle(-Objects[ObjectId].Yaw);


// four dots around the vehicle
/*	x2 = x1 + 70.0f*(float)(Sin[(int)(Objects[ObjectId].Yaw * Deg)]);
	z2 = z1 + 70.0f*(float)(Cos[(int)(Objects[ObjectId].Yaw * Deg)]);

    x3 = x1 - 70.0f*(float)(Sin[(int)(Objects[ObjectId].Yaw * Deg)]);
	z3 = z1 - 70.0f*(float)(Cos[(int)(Objects[ObjectId].Yaw * Deg)]);

	x4 = x1 + 70.0f*(float)(Cos[(int)(NegYaw * Deg)]);
	z4 = z1 + 70.0f*(float)(Sin[(int)(NegYaw * Deg)]);
	
	x5 = x1 - 70.0f*(float)(Cos[(int)(NegYaw * Deg)]);
	z5 = z1 - 70.0f*(float)(Sin[(int)(NegYaw * Deg)]);
*/
	x2 = x1 + 5.0f*(float)(sin((double)(Objects[ObjectId].Yaw * 0.01745328f)));
	z2 = z1 + 5.0f*(float)(cos((double)(Objects[ObjectId].Yaw * 0.01745328f)));

    x3 = x1 - 5.0f*(float)(sin((double)(Objects[ObjectId].Yaw * 0.01745328f)));
	z3 = z1 - 5.0f*(float)(cos((double)(Objects[ObjectId].Yaw * 0.01745328f)));

	x4 = x1 + 5.0f*(float)(cos((double)(-Objects[ObjectId].Yaw * 0.01745328f)));
	z4 = z1 + 5.0f*(float)(sin((double)(-Objects[ObjectId].Yaw * 0.01745328f)));
	
	x5 = x1 - 5.0f*(float)(cos((double)(-Objects[ObjectId].Yaw * 0.01745328f)));
	z5 = z1 - 5.0f*(float)(sin((double)(-Objects[ObjectId].Yaw * 0.01745328f)));


// get the height of the dots on the surface
	y2 = -TerrainHeight(&x2, &y2, &z2);
	y3 = -TerrainHeight(&x3, &y3, &z3);
	y4 = -TerrainHeight(&x4, &y4, &z4);
	y5 = -TerrainHeight(&x5, &y5, &z5);


// dots for debug
	x1tank=x1;
	y1tank=-y1;
	z1tank=z1;

	x2tank=x2;
	y2tank=-y2;
	z2tank=z2;

	x3tank=x3;
	y3tank=-y3;
	z3tank=z3;

	x4tank=x4;
	y4tank=-y4;
	z4tank=z4;

	x5tank=x5;
	y5tank=-y5;
	z5tank=z5;


// get the diameter 
	GetDistance3d(x4,y4,z4,x5,y5,z5);
	a=TempDist;
	GetDistance3d(x2,y2,z2,x3,y3,z3);
	b=TempDist;

// calc the angles
	TempRoll = (float)asin((double)(y4-y5)/a); 
	TempPitch = (float)asin((double)(y3-y2)/b); 

	Objects[ObjectId].AITargetPitch = TempPitch / (2.0f*pi)*360.0f;
	Objects[ObjectId].Pitch = Objects[ObjectId].AITargetPitch;

	Objects[ObjectId].AITargetRoll = TempRoll / (2.0f*pi)*360.0f;
	Objects[ObjectId].Roll = Objects[ObjectId].AITargetRoll;


//	if (Difference(Objects[ObjectId].Pitch, Objects[ObjectId].AITargetPitch) > dDelta*19*g_speedfactor)
//		Objects[ObjectId].Pitch+=dDelta*19*g_speedfactor*TransitAngles(Objects[ObjectId].Pitch, Objects[ObjectId].AITargetPitch);
//	else 
//		Objects[ObjectId].Pitch=Objects[ObjectId].AITargetPitch;

/*	if (Difference(Objects[ObjectId].Roll, Objects[ObjectId].AITargetRoll) > dDelta*19*g_speedfactor)
		Objects[ObjectId].Roll+=dDelta*19*g_speedfactor*TransitAngles(Objects[ObjectId].Roll, Objects[ObjectId].AITargetRoll);
	else 
		Objects[ObjectId].Roll=Objects[ObjectId].AITargetRoll;
*/

}




//============================================================================
// Move an object depending on it's yaw, pitch, and speed.
//============================================================================
void MoveObject(int ObjectId)
{
	int i;
	float xmovement, ymovement, zmovement;

	if (Objects[ObjectId].isDocked)
		return;

		Objects[ObjectId].Yaw = FixAngle(Objects[ObjectId].Yaw);
		Objects[ObjectId].Pitch = FixAngle(Objects[ObjectId].Pitch);
		Objects[ObjectId].Roll = FixAngle(Objects[ObjectId].Roll);

//		Objects[ObjectId].xmovement=(float)(sin(Objects[ObjectId].Yaw * DEGREE)*cos(Objects[ObjectId].Pitch * DEGREE) * (Objects[ObjectId].Speed/100.0f) * scalefactor * g_speedfactor);
//		Objects[ObjectId].zmovement=(float)(cos(Objects[ObjectId].Yaw * DEGREE)*cos(Objects[ObjectId].Pitch * DEGREE) * (Objects[ObjectId].Speed/100.0f) * scalefactor * g_speedfactor);
		Objects[ObjectId].xmovement=(float)(Sin[(int)(Objects[ObjectId].Yaw * Deg)]*Cos[(int)(Objects[ObjectId].Pitch * Deg)] * (Objects[ObjectId].Speed/100.0f) * scalefactor * g_speedfactor);
		Objects[ObjectId].zmovement=(float)(Cos[(int)(Objects[ObjectId].Yaw * Deg)]*Cos[(int)(Objects[ObjectId].Pitch * Deg)] * (Objects[ObjectId].Speed/100.0f) * scalefactor * g_speedfactor);
		if ((Objects[ObjectId].ObjectMesh!=OBJECTMESH_LIGHTTANK) && (Objects[ObjectId].ObjectMesh!=OBJECTMESH_MEDIUMTANK))
		{
			//Ships
//			Objects[ObjectId].ymovement=(float)(sin(Objects[ObjectId].Pitch * DEGREE)* (Objects[ObjectId].Speed/100.0f) * scalefactor * g_speedfactor);
			Objects[ObjectId].ymovement=(float)(Sin[(int)(Objects[ObjectId].Pitch * Deg)]* (Objects[ObjectId].Speed/100.0f) * scalefactor * g_speedfactor);
		}

		
		
		if ((Objects[ObjectId].ObjectMesh==OBJECTMESH_LIGHTTANK) || (Objects[ObjectId].ObjectMesh==OBJECTMESH_MEDIUMTANK))
		{
			//Tanks
			Objects[ObjectId].ymovement=0;
			Objects[ObjectId].Height = -TerrainHeight(&Objects[ObjectId].xPos, &Objects[ObjectId].Height, &Objects[ObjectId].zPos) - (Objects[ObjectId].sizeY/2.0f) - (Objects[ObjectId].sizeY)-0.1f;
			GroundVehicleOrientation(ObjectId);
		}

		xmovement = Objects[ObjectId].xmovement;
		ymovement = Objects[ObjectId].ymovement;
		zmovement = Objects[ObjectId].zmovement;

		Objects[ObjectId].oldHeight = Objects[ObjectId].Height;
		Objects[ObjectId].oldxPos = Objects[ObjectId].xPos;
		Objects[ObjectId].oldzPos = Objects[ObjectId].zPos;

		Objects[ObjectId].Height+=ymovement;
		Objects[ObjectId].xPos-=xmovement;
		Objects[ObjectId].zPos-=zmovement;

xGridPos = (int)(Objects[ObjectId].xPos/scalefactor);
zGridPos = (int)(Objects[ObjectId].zPos/scalefactor);


Objects[ObjectId].xGrid = xGridPos;
Objects[ObjectId].zGrid = zGridPos;

// Same as with Camera code: If the object collided with the Landscape
// or tried to leave the arena, undo the latest movement.


  if (Objects[ObjectId].OwningNetworkClient != localplayer)
  {

	if ((xGridPos < -31) || (xGridPos > 28))
	{
		Objects[ObjectId].xPos+=xmovement;
		Objects[ObjectId].isCollided=TRUE;
	}
	if ((zGridPos < -27) || (zGridPos > 29))
	{
		Objects[ObjectId].zPos+=zmovement;
		Objects[ObjectId].isCollided=TRUE;
	}

	i = DetectCollision(&Objects[ObjectId].xPos, &Objects[ObjectId].Height, &Objects[ObjectId].zPos, Objects[ObjectId].sizeY);
	if (i==1)
	{
		sprintf(tempstring, "Ship %i crashed into the ground", ObjectId);
		WriteErrorLog(tempstring);

		Objects[ObjectId].isCollided=TRUE;
		if (Objects[ObjectId].isDiving)
		{
			if (Player[localplayer].ControlledObject == ObjectId)
			{
				StopSoundFX(18);
				StopSoundFX(19);
			}
			Objects[ObjectId].TimeToLive=2;
			Objects[ObjectId].isDiving=FXFALSE;
			if (Player[localplayer].ControlledObject!=ObjectId)
				Explosion3(Objects[ObjectId].xPos, Objects[ObjectId].zPos, Objects[ObjectId].Height);
			PlayPositionalSound(20, 20, Objects[ObjectId].xPos, Objects[ObjectId].zPos, Objects[ObjectId].Height,1100.0f);
			KillShip(ObjectId);
		}
		else if (Player[localplayer].ControlledObject == ObjectId)
		{
			// Else we're just too dumb to not fly into the ground for no reason...
			DamageObject(Player[localplayer].ControlledObject, DAMAGE_MISSILE, FXFALSE, FXTRUE);
		}

	
	}

  }

}




//============================================================================
// Handler for laser movement, TTL (TimeToLive) and collisions
//============================================================================
void LaserHandler(void)
{
int curobj, hitobj;

for (curobj=0;curobj<NumObjects;curobj++)
	{
		if ((Objects[curobj].ObjectMesh==2) && (Objects[curobj].ObjectId != -1))
		if (Objects[curobj].OwningNetworkClient == localplayer)
		{
			Objects[curobj].Speed=15.0f;
			Objects[curobj].TimeToLive--;
			hitobj=CheckWeaponCollision(curobj);
			if ((Objects[curobj].TimeToLive<0)  || (Objects[curobj].isCollided) || (hitobj!=-1))	//Remove old laser bolts
			{
				if ((Objects[curobj].isCollided) || (hitobj!=-1))
				{
				//Play proper impact sound
				if (use_midas!=0)
				{
					if (hitobj==-1)
						PlayPositionalSound(10, 10, Objects[curobj].xPos, Objects[curobj].zPos, Objects[curobj].Height,1200.0f);
					else if (hitobj==Player[localplayer].ControlledObject)
						PlaySoundFX(13, 13);
					else if (Objects[hitobj].ObjectType==OBJECTTYPE_BUILDING)
						PlayPositionalSound(15, 15, Objects[curobj].xPos, Objects[curobj].zPos, Objects[curobj].Height,1200.0f);
					else if (Objects[hitobj].ObjectType==OBJECTTYPE_SHIP)
						PlayPositionalSound(13, 13, Objects[curobj].xPos, Objects[curobj].zPos, Objects[curobj].Height,1200.0f);
				}
					if (Player[localplayer].ControlledObject!=curobj)
						Explosion2(Objects[curobj].xPos, Objects[curobj].zPos, Objects[curobj].Height);
					if (hitobj!=-1)
						DamageObject(hitobj, DAMAGE_LASER, FXFALSE, FXTRUE);
				}
				RemoveObject(curobj, FXTRUE);
				curobj--;
			}
		}

	}

}


//============================================================================
// Starts the satellite countdown
//============================================================================
void StartSatellite(int TeamNum)
{

	Team[TeamNum].SatelliteTime=500;
	if (TeamNum==Player[localplayer].Team)
		PlaySoundFX(21, 21); //Play "satellite active" sound

}

//============================================================================
// Handles Satellite countdown and fire
//============================================================================
void SatelliteHandler(void)
{
int curteam, curobj;
for (curteam=0; curteam<NumTeams; curteam++)
{
	if (Team[curteam].SatelliteTime<=0)	//Don't do anything if it's inactive
		return;

	if ((getObject(curteam, OBJECTTYPE_BUILDING, OBJECTMESH_UPLINK, 1)==-1) || (!Team[curteam].isPowered))
	{
//		Team[curteam].SatelliteTime = -1;
		if (!Cheat)
			return;								//Abort if satellite uplink building doesn't exist
	}											//or team has not enough energy. (Or we're not cheating)

	

	Team[curteam].SatelliteTime--;
	if (Team[curteam].SatelliteTime==0)	//Time's up, FIRE!!
	{
		for (curobj=0; curobj<NumObjects; curobj++)
		{
			if ((Objects[curobj].isMarked) && (Objects[curobj].ObjectId != -1)) //We want the marker missile work on friendly stuff too
			{	//Blow up the darned thing!!! YEAH!!!!
				SatelliteBlast(Objects[curobj].xPos, Objects[curobj].zPos, Objects[curobj].Height);
				DamageObject(curobj, 1000, FXFALSE, FXTRUE);
			}
		}
	}
}
}



//============================================================================
// Handler for ALL missiles
//============================================================================
void MissileHandler(void)
{
int curobj, hitobj;

for (curobj=0;curobj<NumObjects;curobj++)
	{
		if ((Objects[curobj].ObjectMesh==OBJECTMESH_MISSILE) && (Objects[curobj].ObjectId != -1))
		if (Objects[curobj].OwningNetworkClient == localplayer)
		{
			if (Objects[curobj].isGuided)
				Objects[curobj].Speed=-4.0f;
			else
				Objects[curobj].Speed=-6.0f;

			Objects[curobj].TimeToLive--;
			hitobj=CheckWeaponCollision(curobj);
			if ( (hitobj!=Objects[curobj].FiringObject) && ((Objects[curobj].TimeToLive<0) || (Objects[curobj].isCollided) || (hitobj!=-1)) )	//Remove old missiles
			{
				//Play proper impact sound
				if (use_midas!=0)
				{
					if (hitobj==-1)
						PlayPositionalSound(9, 9, Objects[curobj].xPos, Objects[curobj].zPos, Objects[curobj].Height,1300.0f);
					else if (hitobj==Player[localplayer].ControlledObject)
						PlaySoundFX(14, 14);
					else if (Objects[hitobj].ObjectType==OBJECTTYPE_BUILDING)
						PlayPositionalSound(16, 16, Objects[curobj].xPos, Objects[curobj].zPos, Objects[curobj].Height,1300.0f);
					else if (Objects[hitobj].ObjectType==OBJECTTYPE_SHIP)
						PlayPositionalSound(14, 14, Objects[curobj].xPos, Objects[curobj].zPos, Objects[curobj].Height,1300.0f);
				}




				if ((hitobj!=-1) && (hitobj!=Objects[curobj].FiringObject))
				{
					if (Objects[curobj].isMarkerMissile) //Whoa! The object was hit by a marker missile!
					{
						if (Team[Objects[curobj].Team].SatelliteTime<=0)
							StartSatellite(Objects[curobj].Team);
						DamageObject(hitobj, DAMAGE_MISSILE, FXTRUE, FXTRUE);
						if (Player[localplayer].ControlledObject!=curobj)
							Explosion2(Objects[curobj].xPos, Objects[curobj].zPos, Objects[curobj].Height);
					}
					else
						DamageObject(hitobj, DAMAGE_MISSILE, FXFALSE, FXTRUE);
						if (Player[localplayer].ControlledObject!=curobj)
							Explosion1(Objects[curobj].xPos, Objects[curobj].zPos, Objects[curobj].Height);
				}
				else
				{
					if (Player[localplayer].ControlledObject!=curobj)
						Explosion2(Objects[curobj].xPos, Objects[curobj].zPos, Objects[curobj].Height);
				}
				RemoveObject(curobj, FXTRUE);
				curobj--;
			}
		}

	}

}


//============================================================================
// Handler for updating the sun's position and the ambient fog color.
//============================================================================
void SunHandler(FxBool InitialConditions)
{
int i;

daytimeframes+=1.0f;

#ifdef USE_GL
  if (InitialConditions)
  {
	AddLight( -1, -1, 0.0f, 0.0f, 0.0f,
					  0.0f, 0.0f, 0.0f,
					  0.0f, 0.0f, 0.0f,
					  1.0f, 0.0f, 0.0f,
					  0.0f, 0.0f, 0.0f);
	SunFlare = AddParticleEmitter( -1,
						1, 2, 1, -1, 0.0f, 0.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 20000.0f, 20000.0f, 0.0f,
						255, 255, 230, 33); //Sun flare
	ParticleEmitters[SunFlare].Sun=FXTRUE;

	//Insert stars here!
	for (i=0; i<300; i++)
	{
		float StarYaw = (float)(rand()%3600) / 10.0f;
		float StarPitch = ((float)(rand()%1800) / 10.0f) + 90.0f;
		float StarX = sin(StarYaw*DEGREE) * cos(StarPitch * DEGREE)*20000.0f;
		float StarZ = cos(StarYaw*DEGREE) * cos(StarPitch * DEGREE)*20000.0f;
		float StarY = sin(StarPitch*DEGREE)*20000.0f;
		
		int tempstar = AddParticleEmitter( -1,
						1, 2, 1, -1, 0.0f, StarX, StarZ, StarY, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 200.0f, 200.0f, 0.0f,
						255, 255, 255, 30); //just an itsy bitsy tiny star
		ParticleEmitters[tempstar].Sun=FXTRUE;
	}


	dawnR=(float)random(05)/10.0f;
	dawnG=(float)random(10)/10.0f;
	dawnB=(float)random(35)/10.0f;
	if ((dawnG-15)>dawnB)	//Don't allow "all green" dawns
	{
		dawnB=dawnG;
		dawnR=dawnG;
	}

	duskR=(float)random(25)/10.0f;
	duskG=(float)random(15)/10.0f;
	duskB=(float)random(05)/10.0f;
	if ((duskG-10)>duskR)	//Don't allow "all green" dusks
	{
		duskB=duskG;
		duskR=duskG;
	}

  }
#endif
  if ( (daytimeframes>=4.0f) || (InitialConditions) )
  {
	daytimeframes=0.0f;
	daytime+=1.0f;
	if (daytime>14400.0f)
	{

		daytime=0.0f;						// Start a new day
		dawnR=(float)random(05)/10.0f;
		dawnG=(float)random(10)/10.0f;
		dawnB=(float)random(35)/10.0f;
		duskR=(float)random(35)/10.0f;
		duskG=(float)random(25)/10.0f;
		duskB=(float)random(05)/10.0f;


		current_weather.ThunderAndLightning = 0;
		current_weather.PrecipitationStrength = 0.0f;
		current_weather.WeatherPrecipitation = WEATHERPREC_NONE;
		//Set new weather type
		i = rand()%6;
		if (i < 3)
			current_weather.WeatherType = WEATHERTYPE_SUNNY;
		else if (i < 4)
			current_weather.WeatherType = WEATHERTYPE_CLOUDED;
		else if (i < 5)
			current_weather.WeatherType = WEATHERTYPE_STORM;
		else
			current_weather.WeatherType = WEATHERTYPE_FOG;

		if (current_weather.WeatherType == WEATHERTYPE_SUNNY)
			current_weather.WeatherPrecipitation = WEATHERPREC_NONE;

		if ((current_weather.WeatherType == WEATHERTYPE_CLOUDED) || (current_weather.WeatherType == WEATHERTYPE_STORM))
		{
			i=(int)(rand()%6);
			if ((i<4) && (TerrainType!=TERRAIN_DESERT))
			{
				if (TerrainType == TERRAIN_SNOW)
					current_weather.WeatherPrecipitation = WEATHERPREC_SNOW;
				else
					current_weather.WeatherPrecipitation = WEATHERPREC_RAIN;

				current_weather.PrecipitationStrength = ((float)((int)(rand()%1000)))/1000.0f;
			}

			i=(int)(rand()%4);
			if (i<2)
				if (TerrainType != TERRAIN_SNOW)
					current_weather.ThunderAndLightning = 1;


		}

		//Give us a new sky
		NewFractalSky1();		//sky texture 1
		NewFractalSky2();		//sky texture 2
		calcSky1();				//convert into OpenGL texture
		calcSky2();				//ditto

	}

	fixtime=daytime;
	ispm=FXFALSE;
	if (fixtime>7200.0f)
	{
		fixtime=14400.0f-fixtime;
		ispm=FXTRUE;
	}
					//fixtime goes from 0 to 7200 (60minutes * 12hours)
					//and then back from 7200 to 0 (12h - 24h)

	//Sun position according to daytime
	SunY = -800.0f*(fixtime/7200.0f)+300.0f;
	SunX = -1000.0f*(daytime/14400.0f)+600.0f;
	SunZ = -1000.0f*(daytime/14400.0f)+300.0f;

	//dusk/dawn transition
	curdawnR=1.0f*((float)fixtime/7200.0f) + dawnR*((float)(7200-fixtime)/7200.0f);
	curdawnG=1.0f*((float)fixtime/7200.0f) + dawnG*((float)(7200-fixtime)/7200.0f);
	curdawnB=1.0f*((float)fixtime/7200.0f) + dawnB*((float)(7200-fixtime)/7200.0f);

	curduskR=1.0f*((float)fixtime/7200.0f) + duskR*((float)(7200-fixtime)/7200.0f);
	curduskG=1.0f*((float)fixtime/7200.0f) + duskG*((float)(7200-fixtime)/7200.0f);
	curduskB=1.0f*((float)fixtime/7200.0f) + duskB*((float)(7200-fixtime)/7200.0f);



	if (!ispm)
	{
		SunRIntensity = 215.0f*(((float)fixtime*curdawnR)/7200.0f);		//During AM times, we have a light blue sky
		SunGIntensity = 225.0f*(((float)fixtime*curdawnG)/7200.0f);
		SunBIntensity = 225.0f*(((float)fixtime*curdawnB)/7200.0f);
//		SunBIntensity = 225.0f*(((float)fixtime*3.2)/7200.0f);
	}
	else
	{
//		SunRIntensity = 225.0f*(((float)fixtime*2.3f)/7200.0f); //... it turns red at the evening (PM)
		SunRIntensity = 225.0f*(((float)fixtime*curduskR)/7200.0f); //... it turns red at the evening (PM)
		SunGIntensity = 225.0f*(((float)fixtime*curduskG)/7200.0f);
		SunBIntensity = 215.0f*(((float)fixtime*curduskB)/7200.0f);
	}


	if (SunBIntensity<20.0f)	//Allow no pitch dark night, leave a little blue
		SunBIntensity=17.0f;


	if (SunRIntensity>205.0f)
		SunRIntensity=205.0f;
	if (SunGIntensity>205.0f)
		SunGIntensity=205.0f;
	if (SunBIntensity>205.0f)
		SunBIntensity=205.0f;
	if (SunRIntensity<0.0f)
		SunRIntensity=0.0f;
	if (SunGIntensity<0.0f)
		SunGIntensity=0.0f;
	if (SunBIntensity<0.0f)
		SunBIntensity=0.0f;

	if ((current_weather.WeatherType==WEATHERTYPE_CLOUDED) || (current_weather.WeatherType==WEATHERTYPE_FOG))
	{
		SunRIntensity=(SunRIntensity+SunGIntensity+SunBIntensity)/4.0f;
		SunGIntensity=(SunRIntensity+SunGIntensity+SunBIntensity)/4.0f;
		SunBIntensity=(SunRIntensity+SunGIntensity+SunBIntensity)/4.0f;
	}
	else if (current_weather.WeatherType==WEATHERTYPE_STORM)
	{
		SunRIntensity=(SunRIntensity+SunGIntensity+SunBIntensity)/7.0f;
		SunGIntensity=(SunRIntensity+SunGIntensity+SunBIntensity)/7.0f;
		SunBIntensity=(SunRIntensity+SunGIntensity+SunBIntensity)/7.0f;
	}


		
	if (NightVision)
	{
		SunRIntensity=0.0f;
		SunGIntensity=255.0f;
		SunBIntensity=0.0f;
		SunY = -500.0f;
		SunX = 0.0f;
		SunZ = 0.0f;
	}


		fogColor[0] = (float)((float)SunRIntensity/456.0f);
		fogColor[1] = (float)((float)SunGIntensity/456.0f);
		fogColor[2] = (float)((float)SunBIntensity/456.0f);
		fogColor[3] = 1.0f;
		if (current_weather.WeatherType==WEATHERTYPE_FOG)	
		{
			fogColor[0] *=2.0f;
			fogColor[1] *=2.0f;
			fogColor[2] *=2.0f;
		}
		glFogfv(GL_FOG_COLOR, fogColor);

		if (current_weather.WeatherType==WEATHERTYPE_STORM)
		{
			Lights[0].LightAmbient[0]=(float)SunRIntensity/256.0f/2.0f;
			Lights[0].LightAmbient[1]=(float)SunGIntensity/256.0f/2.0f;
			Lights[0].LightAmbient[2]=(float)SunBIntensity/256.0f/2.0f;
			Lights[0].LightAmbient[3]=0.0f;
			Lights[0].LightDiffuse[0]=(float)SunRIntensity/256.0f;
			Lights[0].LightDiffuse[1]=(float)SunGIntensity/256.0f;
			Lights[0].LightDiffuse[2]=(float)SunBIntensity/256.0f;
			Lights[0].LightDiffuse[3]=0.0f;
		}
		else
		{
			Lights[0].LightAmbient[0]=(float)SunRIntensity/256.0f*2.0f;
			Lights[0].LightAmbient[1]=(float)SunGIntensity/256.0f*2.0f;
			Lights[0].LightAmbient[2]=(float)SunBIntensity/256.0f*2.0f;
			Lights[0].LightAmbient[3]=0.0f;
			Lights[0].LightDiffuse[0]=(float)SunRIntensity/256.0f*10.0f;
			Lights[0].LightDiffuse[1]=(float)SunGIntensity/256.0f*10.0f;
			Lights[0].LightDiffuse[2]=(float)SunBIntensity/256.0f*10.0f;
			Lights[0].LightDiffuse[3]=0.0f;
		}


		if ((current_weather.WeatherType==WEATHERTYPE_STORM) ||
			(current_weather.WeatherType==WEATHERTYPE_CLOUDED))
		{
			Lights[0].xPos = SunX*100.0f;
			Lights[0].zPos = SunZ*200.0f;
			Lights[0].Height = SunY*200.0f;
			ParticleEmitters[SunFlare].Height = 5000.0f;
		}
		else
		{
			Lights[0].xPos = SunX*100.0f;
			Lights[0].zPos = SunZ*200.0f;
			Lights[0].Height = SunY*100.0f;

			if (!NightVision)
			{
				ParticleEmitters[SunFlare].xPos = SunX*100.0f;
				ParticleEmitters[SunFlare].zPos = SunZ*200.0f;
				ParticleEmitters[SunFlare].Height = SunY*100.0f;
			}
			else
				ParticleEmitters[SunFlare].Height = 5000.0f;
		}



  }
}




//============================================================================
// Find a nearby enemy
//============================================================================
int FindTarget(int objectId, int TeamNum, float Range)
{
int o;
int Target;
float currange;

	o=0;Target=-1;
	currange=9999999.9f;
	if (NumObjects>0)
	for (o=0;o<NumObjects;o++)
	{
	  if  (Objects[o].ObjectId != -1)
	  {
		//This is the targeting logic:
		//Only target enemies!
		//Don't target enemy laser shots, but DO target enemy missiles!
		//if we're a tank, only target enemy buildings!
		if ((Objects[o].Team != TeamNum)&&(((Objects[o].ObjectMesh != OBJECTMESH_LASER) && (Objects[objectId].ObjectMesh!=OBJECTMESH_LIGHTTANK)) || ((Objects[o].ObjectType==OBJECTTYPE_BUILDING) && (Objects[objectId].ObjectMesh==OBJECTMESH_LIGHTTANK))))
		{
			HomeObject(o, objectId,0);	//hack to get distance information
			if (((TempDist<Range) || (Range == -1)) && (TempDist<currange))
			{
				currange=TempDist;
				Target=o;
			}
		}
	  }
	}
return(Target);
}





//============================================================================
// Find an enemy which is not closer than minRange
//============================================================================
int FindFarTarget(int objectId, int TeamNum, float minRange)
{
int o;
int Target;
float currange;

	o=0;Target=-1;
	currange=9999999.9f;
	if (NumObjects>0)
	for (o=0;o<NumObjects;o++)
	{
	  if  (Objects[o].ObjectId != -1)
	  {
		if ((Objects[o].Team != TeamNum)&&(Objects[o].ObjectMesh != OBJECTMESH_LASER)) //Don't target enemy laser shots
		{
			HomeObject(o, objectId,0);	//hack to get distance information
			if ((TempDist>minRange) || (minRange == -1))
			{
				currange=TempDist;
				Target=o;
				return(Target);
			}
		}
	  }
	}
return(Target);
}






//============================================================================
// Find an enemy within range AND angle
//============================================================================
int FindTargetByAngle(int objectId, int TeamNum, float Angle, float Range)
{
int o;
int Target, TempLock;
float YawDiff, PitchDiff, MinDiff;
float vxdist, vydist, vzdist;

	MinDiff=99999.0f;
	TempLock=-1;
	o=0;Target=-1;
	if (NumObjects>0)
	for (o=0;o<NumObjects;o++)
	{
	  if  (Objects[o].ObjectId != -1)
	  {
		if ((Objects[o].Team != TeamNum)&&(Objects[o].ObjectType != OBJECTTYPE_WEAPON)) //Don't target enemy missiles and laser shots
		{

			if (Objects[o].xPos > Objects[objectId].xPos)
				vxdist = (Objects[o].xPos - Objects[objectId].xPos);
			else
				vxdist = (Objects[objectId].xPos - Objects[o].xPos);
	
			if (Objects[o].Height > Objects[objectId].Height)
				vydist = (Objects[o].Height - Objects[objectId].Height);
			else
				vydist = (Objects[objectId].Height - Objects[o].Height);
	           
			if (Objects[o].zPos > Objects[objectId].zPos)
				vzdist = (Objects[o].zPos - Objects[objectId].zPos);
			else
				vzdist = (Objects[objectId].zPos - Objects[o].zPos);
	
			if (vxdist<0)
				vxdist=(-1.0f)*vxdist;
			if (vydist<0)
				vydist=(-1.0f)*vydist;
			if (vzdist<0)
				vzdist=(-1.0f)*vzdist;
		
		  if ((vxdist<Range) && (vydist<Range) && (vzdist<Range))
		  {
			HomeObject(o, objectId,0);	//hack to get distance & angle information

			if ((TempDist<Range) || (Range == -1))
			{
				if (Objects[objectId].Yaw>Objects[objectId].AITargetYaw)
					YawDiff=Objects[objectId].Yaw-Objects[objectId].AITargetYaw;
				else
					YawDiff=Objects[objectId].AITargetYaw-Objects[objectId].Yaw;
				if (Objects[objectId].Pitch>Objects[objectId].AITargetPitch)
					PitchDiff=Objects[objectId].Pitch-Objects[objectId].AITargetPitch;
				else
					PitchDiff=Objects[objectId].AITargetYaw-Objects[objectId].Yaw;
				if (YawDiff>180)
					YawDiff=360-YawDiff;
				if (PitchDiff>180)
					PitchDiff=360-PitchDiff;

				if ((YawDiff < Angle) && (PitchDiff < Angle) && (YawDiff+PitchDiff<MinDiff))
				{
					MinDiff = (YawDiff+PitchDiff);
					TempLock = o;
				}
			}
		  }
		}
	  }
	}
return(TempLock);
}

//============================================================================
// Checks if a ship has a lock on any object
//============================================================================
int CheckLockPlayer(int objectId, int TeamNum, float Angle, float Range)
{
int o;
int Target, TempLock;
float YawDiff, PitchDiff;

	TempLock=-1;
	o=0;Target=-1;
	if (NumObjects>0)
	for (o=0;o<NumObjects;o++)
	{
	  if (Objects[o].ObjectId != -1)
	  {
		if ((Objects[o].Team != TeamNum)&&(Objects[o].ObjectType != OBJECTTYPE_WEAPON)) //Don't target enemy missiles and laser shots
		{
			HomeObject(o, objectId,0);	//hack to get distance & angle information

			if ((TempDist<Range) || (Range == -1))
			{
				if (Objects[objectId].Yaw>Objects[objectId].AITargetYaw)
					YawDiff=Objects[objectId].Yaw-Objects[objectId].AITargetYaw;
				else
					YawDiff=Objects[objectId].AITargetYaw-Objects[objectId].Yaw;
				if (Objects[objectId].Pitch>Objects[objectId].AITargetPitch)
					PitchDiff=Objects[objectId].Pitch-Objects[objectId].AITargetPitch;
				else
					PitchDiff=Objects[objectId].AITargetYaw-Objects[objectId].Yaw;
				if (YawDiff>180)
					YawDiff=360-YawDiff;
				if (PitchDiff>180)
					PitchDiff=360-PitchDiff;

				if ((YawDiff < Angle) && (PitchDiff < Angle))
				{
					TempLock = o;
					return (TempLock);
				}

			}
		}
	  }
	}
return(TempLock);
}

//============================================================================
// Optimised version of CheckLockPlayer() for the AI only
//============================================================================
int CheckLockTarget(int objectId, int TeamNum, float Angle, float Range, int AITarget)
{
int o;
int Target, TempLock;
float YawDiff, PitchDiff;

	TempLock=-1;
	o=AITarget;Target=-1;
	if ((Objects[o].Team != TeamNum)&&(Objects[o].ObjectType != OBJECTTYPE_WEAPON)) //Don't target enemy missiles and laser shots
	{
		HomeObject(o, objectId,0);	//hack to get distance & angle information
		if ((TempDist<Range) || (Range == -1))
		{
			if (Objects[objectId].Yaw>Objects[objectId].AITargetYaw)
				YawDiff=Objects[objectId].Yaw-Objects[objectId].AITargetYaw;
			else
				YawDiff=Objects[objectId].AITargetYaw-Objects[objectId].Yaw;
			if (Objects[objectId].Pitch>Objects[objectId].AITargetPitch)
				PitchDiff=Objects[objectId].Pitch-Objects[objectId].AITargetPitch;
			else
				PitchDiff=Objects[objectId].AITargetYaw-Objects[objectId].Yaw;
			if (YawDiff>180)
				YawDiff=360-YawDiff;
			if (PitchDiff>180)
				PitchDiff=360-PitchDiff;
			if ((YawDiff < Angle) && (PitchDiff < Angle))
			{
				TempLock = o;
				return (TempLock);
			}
		}
	}
return(TempLock);
}

//============================================================================
// Polled AI callback
//============================================================================
void AIPollID(int objectId)
{
 if (Objects[objectId].isAIControlled)
 {
	Objects[objectId].TimeSinceLastAIPoll++;
	if (Objects[objectId].TimeSinceLastAIPoll > AIPollInterval)
	{
		Objects[objectId].TimeSinceLastAIPoll=0;

		if (Objects[objectId].isDocked) //If AI is docked, takeoff!
		{
			Objects[objectId].isDocked=FXFALSE;
			Objects[objectId].isLanding=FXFALSE;
			Objects[objectId].isStarting=FXTRUE;
			Objects[objectId].AnimationPhase=0;
			Objects[objectId].AnimationSteps=0.2f;
			Objects[objectId].InitialHeight = Objects[objectId].Height;
			Objects[objectId].Speed = 0.0f;
			if (use_midas!=0)
				PlayPositionalSound(5, 5, Objects[objectId].xPos, Objects[objectId].zPos, Objects[objectId].Height,1200.0f);
		}


		if (Objects[objectId].targetObject == -1) //What? No target?
			Objects[objectId].targetObject = FindTarget(objectId, Objects[objectId].Team, -1);//...then find something to shoot
		else
		{
			Objects[objectId].targetObject = FindTarget(objectId, Objects[objectId].Team, -1);//...then find something to shoot
			HomeObject(Objects[objectId].targetObject, objectId,0);
			if (TempDist<100)
			{
				//If we're too close to a building, look for a new target...
				if (Objects[Objects[objectId].targetObject].ObjectType==OBJECTTYPE_BUILDING)
				{
					Objects[objectId].targetObject = FindFarTarget(objectId, Objects[objectId].Team, 200);//...then find something to shoot
					Objects[objectId].AITargetPitch-=60;
				}
				else	//We're too close to a ship or missile, so let's just evade horizontally
					Objects[objectId].AITargetYaw+=60;
			}
		}
	}
 }
}


//============================================================================
// AI handler
//============================================================================
void AIPoll(void)
{
int i, objectId, newmissile;

	for (i=0;i<NumObjects;i++)
	{
		if ((Objects[i].isAIControlled) && (Objects[i].ObjectId != -1))
		{	objectId = i;
			AIPollID(i);

//			Objects[i].AITargetYaw = FixAngle(Objects[i].AITargetYaw);
//			Objects[i].AITargetPitch = FixAngle(Objects[i].AITargetPitch);
//			Objects[i].AITargetRoll = FixAngle(Objects[i].AITargetRoll);

			if (Difference(Objects[i].Yaw, Objects[i].AITargetYaw) > dDelta*15*g_speedfactor)
				Objects[i].Yaw+=dDelta*15*g_speedfactor*TransitAngles(Objects[i].Yaw, Objects[i].AITargetYaw);
			else
				Objects[i].Yaw=Objects[i].AITargetYaw;

			


			if (Objects[i].ObjectMesh!=OBJECTMESH_LIGHTTANK)
			{
				if (Difference(Objects[i].Yaw, Objects[i].AITargetYaw) > dDelta*15*g_speedfactor)
					if (TransitAngles(Objects[i].Yaw, Objects[i].AITargetYaw) > 0)
						Objects[i].AITargetRoll=dDelta*180.0f;
					else
						Objects[i].AITargetRoll=-dDelta*180.0f;
				else
					Objects[i].AITargetRoll=0.0f;

				if (Difference(Objects[i].Pitch, Objects[i].AITargetPitch) > dDelta*15*g_speedfactor)
					Objects[i].Pitch+=dDelta*15*g_speedfactor*TransitAngles(Objects[i].Pitch, Objects[i].AITargetPitch);
				else
					Objects[i].Pitch=Objects[i].AITargetPitch;
				if (Difference(Objects[i].Roll, Objects[i].AITargetRoll) > dDelta*15*g_speedfactor)
					Objects[i].Roll+=dDelta*15*g_speedfactor*TransitAngles(Objects[i].Roll, Objects[i].AITargetRoll);
				else
					Objects[i].Roll=Objects[i].AITargetRoll;

			}

			if (Objects[objectId].laserReloadTime>0)
				Objects[objectId].laserReloadTime--;
			if (Objects[objectId].missileReloadTime>0)
				Objects[objectId].missileReloadTime--;


//			HomeObject(o, objectId,0);	//hack to get distance & angle information

			if (Objects[objectId].targetObject != -1)
			{
//				if (CheckLockTarget(objectId, Objects[objectId].Team, 10, 1000, Objects[objectId].targetObject)!=-1);
				GetDistance3d( Objects[objectId].xPos, Objects[objectId].Height, Objects[objectId].zPos, Objects[Objects[objectId].targetObject].xPos, Objects[Objects[objectId].targetObject].Height, Objects[Objects[objectId].targetObject].zPos);
				if ((TempDist<1500) && (Objects[objectId].missileReloadTime<=0)) //Fire missile
				{
					if (use_midas!=0)
						PlayPositionalSound(4, 4, Objects[objectId].xPos, Objects[objectId].zPos, Objects[objectId].Height,900.0f);
					newmissile = AddObject(	OBJECTTYPE_WEAPON, //weapon
										OBJECTMESH_MISSILE, //missile mesh
										Objects[objectId].Team, //team
										600, //time to live
										FXFALSE, //not AI controlled
										FXTRUE,  //Homing weapon
										FXTRUE,	 //visible
										FXFALSE, //not a marker missile
										FXFALSE,
										Objects[objectId].xPos, Objects[objectId].zPos, Objects[objectId].Height, 
										Objects[objectId].AITargetYaw, Objects[objectId].AITargetPitch, 0.0f, objectId, FXTRUE);

					Objects[newmissile].FiringObject=objectId;

					//Hack: If the AI attacks a building, automatically make this missile unguided
					if ((Objects[Objects[objectId].targetObject].ObjectType==OBJECTTYPE_BUILDING) && (Objects[objectId].ObjectMesh==OBJECTMESH_LIGHTTANK))
					{
					    Objects[objectId].missileReloadTime=(int)(50.0f*g_ispeedfactor);
						tempint=AddParticleEmitter(	newmissile,
										0, (int)(50.0f*g_ispeedfactor), 1, -1, 0.0f, 0.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.3f, 0.3f, 0.0f, 100.0f, 50.0f, 160.0f,
										100, 100, 0, 6);//color yellow
						ParticleEmitters[tempint].Slot=1;
						Objects[newmissile].isGuided=FXFALSE;
						Objects[newmissile].targetObject = Objects[objectId].targetObject;
					}
					else //else use guided missiles
					{
						Objects[objectId].missileReloadTime=(int)(500.0f*g_ispeedfactor);
						tempint=AddParticleEmitter(	newmissile,
										0, (int)(100.0f*g_ispeedfactor), 1, -1, 0.0f, 0.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.1f, 0.0f, 0.0f, 50.0f, 50.0f, 160.0f,
										255, 255, 255, 6);//color
						ParticleEmitters[tempint].Slot=1;
						Objects[newmissile].isGuided=FXTRUE;
						Objects[newmissile].targetObject = Objects[objectId].targetObject;
					}
				}
				
	
	
	
	
//				if (CheckLockTarget(objectId, Objects[objectId].Team, 10, 500, Objects[objectId].targetObject)!=-1);
				if ((TempDist<500) && (Objects[objectId].laserReloadTime<=0) && (Objects[objectId].ObjectMesh!=OBJECTMESH_LIGHTTANK)) //Fire Lasers
				{
					Objects[objectId].laserReloadTime=(int)(10.0f * g_ispeedfactor);
							if (use_midas!=0)
								PlayPositionalSound(3, 3, Objects[objectId].xPos, Objects[objectId].zPos, Objects[objectId].Height,900.0f);
	
					tempint = AddObject(	OBJECTTYPE_WEAPON, //weapon
								OBJECTMESH_LASER, //laser bolt mesh
								Objects[objectId].Team, //team
								150, //time to live
								FXFALSE, //not AI controlled
								FXFALSE, //not homing
								FXTRUE,  //visible
								FXFALSE, //not a marker missile
								FXFALSE,
								Objects[objectId].xPos, Objects[objectId].zPos, Objects[objectId].Height, 
								180.0f+Objects[objectId].Yaw, -Objects[objectId].Pitch, 0.0f, objectId, FXTRUE);
#ifdef USE_GL
					ParticleEmitters[AddParticleEmitter( tempint,
							1, 2, 1, -1, 0.0f, 0.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 200.0f, 200.0f, 0.0f,
							30, 30, 0, 30)].ThreeD=FXTRUE; //Laser flare
					
/*					AddLight( tempint, -1, 
							0.8f, 0.8f, 0.0f,
							0.8f, 0.8f, 0.0f,
							0.8f, 0.8f, 0.0f,
							1.0f, 0.0f, 0.0001f,
							CamXPos, CamZPos, CamHeight+1.0f);*/
#endif
				}
			}			
		}
	}
}


//============================================================================
// Handler for AAA-Sites
//============================================================================
//This is much like the ship AI stuff, except that the targeting information is used for
//the lasers, not for movement.
void AAAHandler(void)
{
int curobj;
for (curobj=0;curobj<NumObjects;curobj++)
	{
		if (Objects[curobj].ObjectId != -1)
		if (((Objects[curobj].ObjectMesh==OBJECTMESH_AAA)
			     && (Team[Objects[curobj].Team].isPowered)) ||
			 (Objects[curobj].ObjectMesh==OBJECTMESH_COMMANDCENTER) )
		{
			Objects[curobj].TimeSinceLastAIPoll++;
			if (Objects[curobj].TimeSinceLastAIPoll > AIPollInterval)
			{
				Objects[curobj].TimeSinceLastAIPoll=0;
				if (Objects[curobj].targetObject == -1) //What? No target?
					Objects[curobj].targetObject = FindTarget(curobj, Objects[curobj].Team, 600);//...then find something to shoot
				else
				{	//Aim laser at target
					Objects[curobj].targetObject = FindTarget(curobj, Objects[curobj].Team, 600);
					HomeObject(Objects[curobj].targetObject, curobj, -3);	//Laser emitter is 3 above ground
																			//so we have to compensate this
				}															


			}

			if ((Objects[curobj].targetObject != -1) && (Objects[curobj].ObjectMesh != OBJECTMESH_COMMANDCENTER))
			{
				if (Difference(Objects[curobj].AITargetYaw, Objects[curobj].Yaw) > dDelta*15*g_speedfactor)
					Objects[curobj].Yaw+=dDelta*15*g_speedfactor*TransitAngles((Objects[curobj].AITargetYaw+180.0f), Objects[curobj].Yaw);
				else
					Objects[curobj].Yaw=Objects[curobj].AITargetYaw;
				if (Difference(Objects[curobj].Pitch, Objects[curobj].AITargetPitch) > dDelta*15*g_speedfactor)
					Objects[curobj].Pitch+=dDelta*15*g_speedfactor*TransitAngles(Objects[curobj].Pitch, Objects[curobj].AITargetPitch);
				else
					Objects[curobj].Pitch=Objects[curobj].AITargetPitch;
			}
			
			//We have a target and it's within range, so let's shoot at it!
			Objects[curobj].laserReloadTime++;
			if ( (Objects[curobj].targetObject != -1) && (Objects[curobj].laserReloadTime>(15*(g_ispeedfactor))) )
			{
				Objects[curobj].laserReloadTime=0;
						if (use_midas!=0)
							PlayPositionalSound(3, 3, Objects[curobj].xPos, Objects[curobj].zPos, Objects[curobj].Height,900.0f);

				tempint = AddObject(	OBJECTTYPE_WEAPON, //weapon
							OBJECTMESH_LASER, //laser bolt mesh
							Objects[curobj].Team, //team
							200, //time to live
							FXFALSE, //not AI controlled
							FXFALSE, //not homing
							FXTRUE,  //visible
							FXFALSE, //not a marker missile
							FXFALSE,
							Objects[curobj].xPos, Objects[curobj].zPos, Objects[curobj].Height-3, 
							180.0f+Objects[curobj].Yaw, Objects[curobj].Pitch, 0.0f, curobj, FXTRUE);
#ifdef USE_GL
				ParticleEmitters[AddParticleEmitter(	tempint,
							1, 2, 1, -1, 0.0f, 0.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 200.0f, 200.0f, 0.0f,
							30, 30, 0, 30)].ThreeD=FXTRUE; //Laser flare
/*				AddLight( tempint, -1, 
							0.8f, 0.8f, 0.0f,
							0.8f, 0.8f, 0.0f,
							0.8f, 0.8f, 0.0f,
							1.0f, 0.0f, 0.0001f,
							CamXPos, CamZPos, CamHeight+1.0f);*/
#endif
			}
		}
	}
}


//============================================================================
// Guided Missile seeker warhead
//============================================================================
void MissilePollID(int objectId)
{
 if (Objects[objectId].isGuided)
 {
	Objects[objectId].TimeSinceLastAIPoll++;
	if (Objects[objectId].TimeSinceLastAIPoll > AIPollInterval)
	{
		Objects[objectId].TimeSinceLastAIPoll=0;
//		if (Objects[objectId].targetObject == -1) //What? No target?
//			Objects[objectId].targetObject = FindTargetByAngle(objectId, Objects[objectId].Team, 10, 1000);
//			Objects[objectId].targetObject = FindTarget(objectId, Objects[objectId].Team, 1000);
							//...then find something to shoot
							//10 degrees lock angle
							//100 units max distance
//		else
		if (Objects[objectId].targetObject != -1)
		{
			//Steer towards target
			HomeObject(Objects[objectId].targetObject, objectId,0);
			if (TempDist>2500)
				Objects[objectId].targetObject=-1;
		}
	}
 }
}


//============================================================================
// Guided missile handler
//============================================================================
void GuidedMissileHandler(void)
{
int i;
	for (i=0;i<NumObjects;i++)
	{
		if (Objects[i].ObjectId != -1)
		if (Objects[i].isGuided)
		{
			MissilePollID(i);
			if (Objects[i].targetObject != -1)
			{
				if (Difference(Objects[i].Yaw, Objects[i].AITargetYaw) > dDelta*19*g_speedfactor)
					Objects[i].Yaw+=dDelta*19*g_speedfactor*TransitAngles(Objects[i].Yaw, Objects[i].AITargetYaw);
				else
					Objects[i].Yaw=Objects[i].AITargetYaw;
				if (Difference(Objects[i].Pitch, -Objects[i].AITargetPitch) > dDelta*19*g_speedfactor)
					Objects[i].Pitch+=dDelta*19*g_speedfactor*TransitAngles(Objects[i].Pitch, -Objects[i].AITargetPitch);
				else 
					Objects[i].Pitch=-Objects[i].AITargetPitch;
			}
		}
	}
}



//============================================================================
// Handler for SAM-Sites
//============================================================================
//Same stuff as the AAA-Handler, only that it fires guided missiles.
void SAMHandler(void)
{
int curobj, newmissile;
for (curobj=0;curobj<NumObjects;curobj++)
	{
		if (Objects[curobj].ObjectId != -1)
		if ( ((Objects[curobj].ObjectMesh==OBJECTMESH_SAM) && (Team[Objects[curobj].Team].isPowered)) ||
			 (Objects[curobj].ObjectMesh==OBJECTMESH_COMMANDCENTER) )
		{
			Objects[curobj].TimeSinceLastAIPoll++;
			if (Objects[curobj].TimeSinceLastAIPoll > AIPollInterval)
			{
				Objects[curobj].TimeSinceLastAIPoll=0;
				if (Objects[curobj].targetObject == -1) //What? No target?
					Objects[curobj].targetObject = FindTarget(curobj, Objects[curobj].Team, 1500);//...then find something to shoot
				else
				{	//Aim laser at target
					Objects[curobj].targetObject = FindTarget(curobj, Objects[curobj].Team, 1500);
					HomeObject(Objects[curobj].targetObject, curobj, 0);
				}
			}
			//We have a target and it's within range, so let's shoot at it!
			Objects[curobj].missileReloadTime++;
			if ((Objects[curobj].targetObject != -1) && (Objects[curobj].missileReloadTime>(500*(g_ispeedfactor))) ) //SAM's have a long reload time
			{
				Objects[curobj].missileReloadTime=0;
				if (use_midas!=0)
					PlayPositionalSound(4, 4, Objects[curobj].xPos, Objects[curobj].zPos, Objects[curobj].Height,900.0f);
				newmissile = AddObject(	OBJECTTYPE_WEAPON, //weapon
									OBJECTMESH_MISSILE, //missile mesh
									Objects[curobj].Team, //team
									600, //time to live
									FXFALSE, //not AI controlled
									FXTRUE,  //Homing weapon
									FXTRUE,	 //visible
									FXFALSE, //not a marker missile
									FXFALSE,
									Objects[curobj].xPos, Objects[curobj].zPos, Objects[curobj].Height-3.0f, 
									Objects[curobj].AITargetYaw, 90.0f, 0.0f, curobj, FXTRUE);
				tempint=AddParticleEmitter(	newmissile,
									0, (int)(100.0f*g_ispeedfactor), 1, -1, 0.0f, 0.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.1f, 0.1f, 0.0f, 100.0f, 50.0f, 160.0f,
									255, 255, 255, 6);//color
				ParticleEmitters[tempint].Slot=1;
				Objects[newmissile].targetObject = Objects[curobj].targetObject;
			}
		}
	}
}




void DisplayTauntMenu(void)
{
	int i;
	if (!TauntMenuActive)
		return;

	for (i=1; i<=TauntMenuDisplaySize; i++)
	{
		sprintf(tempstring, "\\a(255 255 0 188)%s\n",TauntMenuDisplay[i]);
		mytext.Size(8);
		mytext.Draw(250, 200-(i*9), tempstring);
	}



}


void DisplayTeamStats(void)
{

	sprintf(tempstring, "\\a(0 0 0 188)Energy Available: %i\n",Team[Player[localplayer].Team].EnergyAvailable);
	mytext.Size(10);
    mytext.Draw(30, 20, tempstring);
	sprintf(tempstring, "\\a(128 128 128 188)Energy Available: %i\n",Team[Player[localplayer].Team].EnergyAvailable);
	mytext.Size(10);
    mytext.Draw(29, 21, tempstring);

	sprintf(tempstring, "\\a(0 0 0 188)Energy Drain: %i\n",Team[Player[localplayer].Team].EnergyNeeded);
	mytext.Size(10);
    mytext.Draw(30, 35, tempstring);
	sprintf(tempstring, "\\a(128 128 128 188)Energy Drain: %i\n",Team[Player[localplayer].Team].EnergyNeeded);
	mytext.Size(10);
    mytext.Draw(29, 36, tempstring);

	sprintf(tempstring, "\\a(0 0 0 188)Resources Available: %i\n",Team[Player[localplayer].Team].ResourcesAvailable);
	mytext.Size(10);
    mytext.Draw(30, 50, tempstring);
	sprintf(tempstring, "\\a(128 128 128 188)Resources Available: %i\n",Team[Player[localplayer].Team].ResourcesAvailable);
	mytext.Size(10);
    mytext.Draw(29, 51, tempstring);
	sprintf(tempstring, " ");

	if (Player[localplayer].ControlledObject!=-1)
	{
		if (Objects[Player[localplayer].ControlledObject].MissileType==MISSILETYPE_GUIDED)
		{
			sprintf(tempstring, "\\a(0 0 0 188)Guided Missiles: %i\n",Objects[Player[localplayer].ControlledObject].MissileAmmo);
			mytext.Size(10);
			mytext.Draw(30, 65, tempstring);
			sprintf(tempstring, "\\a(128 128 0 188)Guided Missiles: %i\n",Objects[Player[localplayer].ControlledObject].MissileAmmo);
			mytext.Size(10);
			mytext.Draw(29, 66, tempstring);
			sprintf(tempstring, " ");
			}
		if (Objects[Player[localplayer].ControlledObject].MissileType==MISSILETYPE_MARKER)
		{
			sprintf(tempstring, "\\a(0 0 0 188)Marker Missiles: %i\n",Objects[Player[localplayer].ControlledObject].MissileAmmo);
			mytext.Size(10);
			mytext.Draw(30, 65, tempstring);
			sprintf(tempstring, "\\a(128 128 0 188)Marker Missiles: %i\n",Objects[Player[localplayer].ControlledObject].MissileAmmo);
			mytext.Size(10);
			mytext.Draw(29, 66, tempstring);
			sprintf(tempstring, " ");
		}
		if (Objects[Player[localplayer].ControlledObject].MissileType==MISSILETYPE_DUMBFIRE)
		{
			sprintf(tempstring, "\\a(0 0 0 188)Dumb Missiles: %i\n",Objects[Player[localplayer].ControlledObject].MissileAmmo);
			mytext.Size(10);
			mytext.Draw(30, 65, tempstring);
			sprintf(tempstring, "\\a(128 128 0 188)Dumb Missiles: %i\n",Objects[Player[localplayer].ControlledObject].MissileAmmo);
			mytext.Size(10);
			mytext.Draw(29, 66, tempstring);
			sprintf(tempstring, " ");
		}
	}

	if (Team[Player[localplayer].Team].SatelliteTime>0)
	{
		sprintf(tempstring, "\\c(255 40 40)Satellite firing in: %i\n",Team[Player[localplayer].Team].SatelliteTime);
		mytext.Size(10);
		mytext.Draw(30, 80, tempstring);
//		glFastPrint(tempstring, -0.95f, -0.7f, 1.0f, 1.0f, 1.0f);
		sprintf(tempstring, " ");
	}


//	sprintf(tempstring, "\\c(255 40 40)P/Y: %f/%f\n",xpitchTime, yawTime);
//	mytext.Draw(20, 100, tempstring);
//	sprintf(tempstring, " ");


/*	for (tempint=0; tempint<NumObjects; tempint++)
	{
		if (Objects[tempint].ObjectMesh==OBJECTMESH_LIGHTTANK)
		{
			sprintf(tempstring, "Tank Pitch: %f\n",Objects[tempint].AITargetPitch);
			glFastPrint(tempstring, -0.95f, -0.4f, 1.0f, 1.0f, 0.0f);
			sprintf(tempstring, "Tank Roll: %f\n",Objects[tempint].AITargetRoll);
			glFastPrint(tempstring, -0.95f, -0.5f, 1.0f, 1.0f, 0.0f);
			sprintf(tempstring, "Tank Yaw: %f\n",Objects[tempint].Yaw);
			glFastPrint(tempstring, -0.95f, -0.6f, 1.0f, 1.0f, 0.0f);
		}
	}
*/
}




void UpdateTeamStats(void)
{
	int i;
	for (i=0; i<NumTeams; i++)
	{
		Team[i].EnergyNeeded=0;
		Team[i].EnergyAvailable=0;
// This code was just quitting if one team lost. Now we have a better handling for this
//		if ((getObject(i, OBJECTTYPE_BUILDING, OBJECTMESH_COMMANDCENTER, 1)==-1) &&
//		    (getObject(i, OBJECTTYPE_BUILDING, OBJECTMESH_COMMANDCENTERMENU, 1)==-1))
//			frames=0;
	}
	for (i=0; i<NumObjects; i++)
	{
		if (Objects[i].ObjectId != -1)
		{
		if (Objects[i].ObjectMesh==OBJECTMESH_POWERPLANT)
			Team[Objects[i].Team].EnergyAvailable+=1500;
		if (Objects[i].ObjectMesh==OBJECTMESH_AAA)
			Team[Objects[i].Team].EnergyNeeded+=300;
		if (Objects[i].ObjectMesh==OBJECTMESH_SAM)
			Team[Objects[i].Team].EnergyNeeded+=500;
		if (Objects[i].ObjectMesh==OBJECTMESH_UPLINK)
			Team[Objects[i].Team].EnergyNeeded+=2000;
		if (Objects[i].ObjectMesh==OBJECTMESH_MINE)
			Team[Objects[i].Team].EnergyNeeded+=400;
		}
	}
	for (i=0; i<NumTeams; i++)
	{
		if (Team[i].EnergyAvailable-Team[i].EnergyNeeded<0)
			Team[i].isPowered=FXFALSE;
		else
			Team[i].isPowered=FXTRUE;

	}
}



void ObjectAnimationHandler(void)
{
int curobj;
float temppitch;

for (curobj=0;curobj<NumObjects;curobj++)
	{
	if (Objects[curobj].ObjectId != -1)
	{
		if (Objects[curobj].isStarting || Objects[curobj].isLanding || Objects[curobj].isDiving)
		{
			Objects[curobj].AnimationPhase++;
			Objects[curobj].isDocked=FXFALSE;
		}
		
		if (Objects[curobj].isDiving)
			if (Objects[curobj].AnimationPhase==1)
			{
				if (Player[localplayer].ControlledObject==curobj)
				{
					PlaySoundFX(18, 18);	//Sound stall alarm
					PlaySoundFX(19, 19);	//Play "Dive" sound
				}
				else
				{
					PlayPositionalSound( 19, 19, Objects[curobj].xPos, Objects[curobj].zPos, Objects[curobj].Height,1100.0f);	//Play "Dive" sound
				}
			}
			else
			{
				Objects[curobj].AITargetPitch=-60.0f;
				Objects[curobj].Speed=-2.0f-(float)(Objects[curobj].AnimationPhase)/100.0f;
				Objects[curobj].AITargetPitch = FixAngle(Objects[curobj].AITargetPitch);
				Objects[curobj].Pitch = FixAngle(Objects[curobj].Pitch);
				if (Player[localplayer].ControlledObject==curobj)
				{
					if (Difference(Objects[curobj].Pitch, Objects[curobj].AITargetPitch) > dDelta*5*g_speedfactor)
						Objects[curobj].Pitch+=dDelta*5*g_speedfactor*TransitAngles(Objects[curobj].Pitch, Objects[curobj].AITargetPitch);
					else
						Objects[curobj].Pitch=Objects[curobj].AITargetPitch;

					ControlPitch=Objects[curobj].Pitch;
					mySpeed=-Objects[curobj].Speed;
//					ControlPitch=-60;
				}


			}

		//Stop takeoff animation after 400 timeframes
		if ((Objects[curobj].isStarting) && (Objects[curobj].AnimationPhase>400))
		{
			Objects[curobj].AnimationPhase=0;
			Objects[curobj].isStarting=FXFALSE;
			Objects[Objects[curobj].CommandCenter].isBusy=FXFALSE;
		}

		//takeoff animation
		if (Objects[curobj].isStarting)
		{
			Objects[Objects[curobj].CommandCenter].isBusy=FXTRUE;
			if (Objects[curobj].AnimationPhase==1)
			{

			if (Player[localplayer].ControlledObject==curobj)
			{
				ControlHeight= Objects[curobj].Height;
				mySpeed = -Objects[curobj].Speed;
				accTime = mySpeed;
				setSpeed = mySpeed;
			}

			if (Player[localplayer].ControlledObject==curobj)
				PlaySoundFX(8, 8);
			else
				PlayPositionalSound( 8, 8, Objects[curobj].xPos, Objects[curobj].zPos, Objects[curobj].Height,1100.0f );
			}



			if (Objects[curobj].AnimationPhase>100)
			{
				Objects[curobj].Height= Objects[curobj].InitialHeight - ((Objects[curobj].AnimationPhase-100) * Objects[curobj].AnimationSteps);
				if (Objects[curobj].AnimationPhase>200)
				{
					Objects[curobj].Speed=-2.0f*(float)(Objects[curobj].AnimationPhase-200)/200;
				}
				if (Player[localplayer].ControlledObject==curobj)
				{
					ControlHeight= Objects[curobj].Height;
					mySpeed = -Objects[curobj].Speed;
					accTime = mySpeed;
					setSpeed = mySpeed;
				}
			}
		}

		//Stop landing animation after 210 timeframes
		if ((Objects[curobj].isLanding) && (Objects[curobj].AnimationPhase>200))
		{
			Objects[Objects[curobj].CommandCenter].isBusy=FXFALSE;
			if (Player[localplayer].ControlledObject==curobj)
				PlaySoundFX(7, 7);
			else
				PlayPositionalSound( 7, 7, Objects[curobj].xPos, Objects[curobj].zPos, Objects[curobj].Height,1100.0f );

			Objects[curobj].AnimationPhase=0;
			Objects[curobj].isLanding=FXFALSE;
			Objects[curobj].isDocked=FXTRUE;	//It's only logical that we're docked after landing ;-)
			Objects[curobj].Speed=0;
			if (Player[localplayer].ControlledObject==curobj)
			{
				mySpeed = 0.0f;
				accTime = 0.0f;
				setSpeed = 0.0f;
				xpitchTime=0.0f;
				yawTime=0.0f;
				rollTime=0.0f;
			}
		}

		//landing animation
		if (Objects[curobj].isLanding)
		{
			Objects[Objects[curobj].CommandCenter].isBusy=FXTRUE;
			Objects[curobj].Height= Objects[curobj].InitialHeight - (float)(Objects[curobj].AnimationPhase * Objects[curobj].AnimationSteps);

			if (Objects[curobj].Speed > 0) //Slow down
				Objects[curobj].Speed-=0.1f;
			if (Objects[curobj].Speed < 0)
				Objects[curobj].Speed+=0.1f;

			
			temppitch = Objects[curobj].Pitch;
			if (temppitch < 180.0f)
			{
				if (temppitch > 0.0f)
					temppitch-=1.0f;
			}
			else if (temppitch >= 180.0f)
			{
				if (temppitch<360.0f)
					temppitch+=1.0f;
			}
			if ((temppitch>358.0f) || (temppitch<2.0f))
				temppitch = 0.0f;

			Objects[curobj].Pitch = temppitch;
			if (curobj == Player[localplayer].ControlledObject)
				ControlPitch = temppitch;

			if (Player[localplayer].ControlledObject==curobj)
			{
				mySpeed = -Objects[curobj].Speed;
				accTime = mySpeed;
				setSpeed = mySpeed;
				ControlHeight= Objects[curobj].Height;
			}
		}
	}
	}
}








//============================================================================
// Handler for Buildings (removal, management, etc)
//============================================================================
void BuildingHandler(void)
{
int curobj;
for (curobj=0;curobj<NumObjects;curobj++)
	{
		if (Objects[curobj].ObjectId != -1)
		if (Objects[curobj].ObjectType==OBJECTTYPE_BUILDING)
		{
			if (Objects[curobj].TimeToLive!=-1)
			{
				Objects[curobj].TimeToLive--;
				if (Objects[curobj].TimeToLive<0)	//The building died, so let's remove it
				{
					Team[Objects[curobj].Team].BuildingsLost++;
					RemoveObject(curobj, FXTRUE);
					curobj--;
				}
			}
			else	//The building is alive, so let it produce something
			{
				if ((Objects[curobj].ObjectMesh==OBJECTMESH_MINE) && (Team[Objects[curobj].Team].isPowered))
					Team[Objects[curobj].Team].ResourcesAvailable++;
				if (Objects[curobj].ObjectMesh==OBJECTMESH_POWERPLANT)
					Team[Objects[curobj].Team].EnergyAvailable++;
			}
		}
	}
}


//============================================================================
// Handler for Ships (collision, killing, removal, etc)
//============================================================================
void ShipHandler(void)
{
int curobj, hitobj;

for (curobj=0;curobj<NumObjects;curobj++)
	{
	  if (Objects[curobj].ObjectId != -1)
		if (Objects[curobj].ObjectType==OBJECTTYPE_SHIP)
		{

			if ((!Objects[curobj].isDocked) && (!Objects[curobj].isLanding) && (!Objects[curobj].isStarting))
			{
				hitobj = CheckShipCollision(curobj);
				if ((hitobj!=-1) && (Objects[curobj].TimeToLive==-1))		//Ooops, we crashed!
				{
					if (Player[localplayer].ControlledObject!=curobj)
					Explosion1(Objects[curobj].xPos, Objects[curobj].zPos, Objects[curobj].Height);
					DamageObject(hitobj, 3*DAMAGE_MISSILE, FXFALSE, FXTRUE);
					DamageObject(curobj, 2*DAMAGE_MISSILE, FXFALSE, FXTRUE);

					if (Objects[hitobj].ObjectType==OBJECTTYPE_BUILDING)	//Ship vs Building -> Ship dead
					{
						KillShip(curobj);
					}
					else if (Objects[hitobj].ObjectType==OBJECTTYPE_SHIP)	//Ship vs Ship -> both are killed
					{
						Objects[hitobj].isDiving=FXTRUE;
						Objects[curobj].isDiving=FXTRUE;
						AddParticleEmitter(	curobj,
						1, 100, 1, -1, 0.0f, 0.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.1f, 0.1f, 0.0f, 100.0f, 200.0f, 100.0f,
						100, 50, 50, 6);
						AddParticleEmitter(	hitobj,
						1, 100, 1, -1, 0.0f, 0.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.1f, 0.1f, 0.0f, 100.0f, 200.0f, 100.0f,
						100, 50, 50, 6);

					}
				}
			}
			else
			if (Objects[curobj].isDocked)
			{
				if (Objects[curobj].Health<Objects[curobj].MaxHealth)
				Objects[curobj].Health++;
				if (Objects[curobj].Shield<Objects[curobj].MaxShield)
				Objects[curobj].Shield++;

				//Reload missiles
				if (Objects[curobj].MissileType==MISSILETYPE_GUIDED)
					Objects[curobj].MissileAmmo=10;
				if (Objects[curobj].MissileType==MISSILETYPE_MARKER)
					Objects[curobj].MissileAmmo=2;
				if (Objects[curobj].MissileType==MISSILETYPE_DUMBFIRE)
					Objects[curobj].MissileAmmo=20;

			} 
			else
			if ((Objects[curobj].TimeToLive!=-1))// && (!Objects[curobj].isDiving))
			{
				Objects[curobj].TimeToLive--;
				if (Objects[curobj].TimeToLive<0)	//Ship died, so remove it and respawn
				{
					KillShip(curobj);
				}
			}
		}
	}
}


//============================================================================
// This handler sounds a warning klaxon in case a missile locks on to the ship
//============================================================================
void MissileWarningHandler(void)
{
int curobj;
ThreatWarningInterval--;
if (ThreatWarningInterval<0)
	ThreatWarningInterval=0;

for (curobj=0;curobj<NumObjects;curobj++)
	{
		if (Objects[curobj].ObjectId != -1)
		if	( (Objects[curobj].ObjectType==OBJECTTYPE_WEAPON) && 
			(Objects[curobj].isGuided) &&
			(Objects[curobj].targetObject==Player[localplayer].ControlledObject))
		{
			if (ThreatWarningInterval==0)
			{
				GetDistance3d(	Objects[curobj].xPos, Objects[curobj].zPos, Objects[curobj].Height,
									CamXPos, CamZPos, CamHeight);
				ThreatWarningInterval=(int)(TempDist/70.0f)+5;
				PlaySoundFX(17, 17);
				AddMessage("WARNING: MISSILE LOCK!", 255,60,10);
			}
		}
		else if ( (Objects[curobj].targetObject==Player[localplayer].ControlledObject) &&
			   (Objects[curobj].ObjectType==OBJECTTYPE_SHIP) && 
			   (Objects[curobj].isAIControlled) )	//Sound radar warning klaxon
		{
			GetDistance3d(	Objects[curobj].xPos, Objects[curobj].zPos, Objects[curobj].Height,
							CamXPos, CamZPos, CamHeight);
			if	(TempDist < 1000.0f)
	
			{
				if (ThreatWarningInterval==0)
				{
					ThreatWarningInterval=5;
					PlaySoundFX(36, 36);
					AddMessage("Radar Warning", 255,60,10);
				}
			}
		}

	}
}


//============================================================================
// Slowly recharges all shields
//============================================================================
void ShieldHandler(void)
{
	int curobj;
	ShieldUpdateTime++;
	if (ShieldUpdateTime>ShieldUpdateInterval)
	{
		ShieldUpdateTime=0;
		for (curobj=0;curobj<NumObjects;curobj++)
		{
			if (Objects[curobj].ObjectId != -1)
			if ((Objects[curobj].TimeToLive==-1) && (Objects[curobj].Shield<Objects[curobj].MaxShield))
			{
				Objects[curobj].Shield++;
			}
		}
	}
}


//============================================================================
// Does the local player's missile lock
//============================================================================
void LockHandler(void)
{
	int TempLock;
	LockCheckInterval++;
	if (LockCheckInterval==30)
	{
		LockCheckInterval=0;
//		TempLock = CheckLockPlayer(Player[localplayer].ControlledObject, Player[localplayer].Team, 10, 1000);
		TempLock = FindTargetByAngle(Player[localplayer].ControlledObject, Player[localplayer].Team, 10, 1000);

		if (TempLock != LocalPlayerLock)
		{
			LocalPlayerLock=TempLock;
			if ((!Objects[Player[localplayer].ControlledObject].isDiving) && (TempLock!=-1))
			{
				StopSoundFX(23);
				PlaySoundFX(23, 23);
				Objects[Player[localplayer].ControlledObject].targetObject=TempLock;
			}
			if (TempLock==-1)
				StopSoundFX(23);
		}
	}
}


void DrawLine(float x1, float y1, float x2, float y2, float r1, float g1, float b1, float r2, float g2, float b2)
{
	glBegin(GL_LINES);
	glColor4f((r1/256.0f),(g1/256.0f),(b1/256.0f),0.9f);
	glVertex2f(x1, y1);
	glColor4f((r2/256.0f),(g2/256.0f),(b2/256.0f),0.9f);
	glVertex2f(x2, y2);
	glEnd();

}





//============================================================================
// Displays a targeting retice. Far from finished...
//============================================================================
void DrawHud(void)
{
  GrVertex vtxA, vtxB;	//Ready-to-draw vertices
  float InnerDistance;
  float OuterDistance;

  int HomeBase;

  float Health;
  float Shields;

  float OffsetY;

		glDisable(GL_DEPTH_TEST); 
		glEnable(GL_BLEND); 
		glDisable(GL_LIGHTING); 

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0, 1, 0, 1); // Set up a 2D screen.

		glDisable(GL_TEXTURE_2D);


	if ((!LoadoutMenuActive) &&
		(!GameHalted))
	{
	//Draw targeting retice

    vtxA.oow = 1.0f;
    vtxB.oow = 1.0f;

    vtxA.a = 255.0f;
    vtxB.a = 255.0f;

    vtxA.r = 0;
    vtxA.g = 255.0f;
    vtxA.b = 0;

	if (LocalPlayerLock==-1)
	{
		vtxB.r = 0;
		vtxB.g = 0;
		vtxB.b = 0;
	} else
	{
		vtxB.r = 255.0f;
		vtxB.g = 0;
		vtxB.b = 0;
	}


	InnerDistance = 0.02f;
	OuterDistance = 0.06f;

DrawLine(0.5f, 0.5f-OuterDistance, 0.5f, 0.5f-InnerDistance, 
		 vtxA.r, vtxA.g, vtxA.b, 
		 vtxB.r, vtxB.g, vtxB.b);

DrawLine(0.5f, 0.5f+OuterDistance, 0.5f, 0.5f+InnerDistance, 
		 vtxA.r, vtxA.g, vtxA.b, 
		 vtxB.r, vtxB.g, vtxB.b);

	InnerDistance = 0.017f;
	OuterDistance = 0.049f;

DrawLine(0.5f-OuterDistance, 0.5f, 0.5f-InnerDistance, 0.5f, 
		 vtxA.r, vtxA.g, vtxA.b, 
		 vtxB.r, vtxB.g, vtxB.b);

DrawLine(0.5f+OuterDistance, 0.5f, 0.5f+InnerDistance, 0.5f,
		 vtxA.r, vtxA.g, vtxA.b, 
		 vtxB.r, vtxB.g, vtxB.b);

	}
	//End of Retice drawing

	//Draw Ship health display

    vtxA.r = 0;
    vtxA.g = 50;
    vtxA.b = 0;
    vtxB.r = 0;
    vtxB.g = 50;
    vtxB.b = 0;

DrawLine(0.010f, 0.1f, 0.010f, 0.3f, 
		 vtxA.r, vtxA.g, vtxA.b, 
		 vtxB.r, vtxB.g, vtxB.b);

DrawLine(0.011f, 0.1f, 0.011f, 0.3f, 
		 vtxA.r, vtxA.g, vtxA.b, 
		 vtxB.r, vtxB.g, vtxB.b);

DrawLine(0.012f, 0.1f, 0.012f, 0.3f, 
		 vtxA.r, vtxA.g, vtxA.b, 
		 vtxB.r, vtxB.g, vtxB.b);

    vtxA.r = 0;
    vtxA.g = 0;
    vtxA.b = 50;
    vtxB.r = 0;
    vtxB.g = 0;
    vtxB.b = 50;

DrawLine(0.020f, 0.1f, 0.020f, 0.3f, 
		 vtxA.r, vtxA.g, vtxA.b, 
		 vtxB.r, vtxB.g, vtxB.b);

DrawLine(0.021f, 0.1f, 0.021f, 0.3f, 
		 vtxA.r, vtxA.g, vtxA.b, 
		 vtxB.r, vtxB.g, vtxB.b);

DrawLine(0.022f, 0.1f, 0.022f, 0.3f, 
		 vtxA.r, vtxA.g, vtxA.b, 
		 vtxB.r, vtxB.g, vtxB.b);



	if (Player[localplayer].ControlledObject!=-1)
	{
		Health = (float)((float)Objects[Player[localplayer].ControlledObject].Health / (float)Objects[Player[localplayer].ControlledObject].MaxHealth);
		Shields = (float)((float)Objects[Player[localplayer].ControlledObject].Shield / (float)Objects[Player[localplayer].ControlledObject].MaxShield);
	}
	else
	{
		Health = 0.0f;
		Shields = 0.0f;
	}

	Health = 0.1f+ 0.2f * Health;
	Shields = 0.1f+ 0.2f * Shields;

    vtxA.r = 0;
    vtxA.g = 255;
    vtxA.b = 0;
    vtxB.r = 0;
    vtxB.g = 255;
    vtxB.b = 0;


DrawLine(0.010f, 0.1f, 0.010f, Health, 
		 vtxA.r, vtxA.g, vtxA.b, 
		 vtxB.r, vtxB.g, vtxB.b);

DrawLine(0.011f, 0.1f, 0.011f, Health, 
		 vtxA.r, vtxA.g, vtxA.b, 
		 vtxB.r, vtxB.g, vtxB.b);

DrawLine(0.012f, 0.1f, 0.012f, Health, 
		 vtxA.r, vtxA.g, vtxA.b, 
		 vtxB.r, vtxB.g, vtxB.b);

	if (Player[localplayer].ControlledObject!=-1)
	if (Objects[Player[localplayer].ControlledObject].SpecialType==SPECIALTYPE_SHIELDS)
	{
    vtxA.r = 0;
    vtxA.g = 0;
    vtxA.b = 255;
    vtxB.r = 0;
    vtxB.g = 0;
    vtxB.b = 255;

DrawLine(0.020f, 0.1f, 0.020f, Shields, 
		 vtxA.r, vtxA.g, vtxA.b, 
		 vtxB.r, vtxB.g, vtxB.b);

DrawLine(0.021f, 0.1f, 0.021f, Shields, 
		 vtxA.r, vtxA.g, vtxA.b, 
		 vtxB.r, vtxB.g, vtxB.b);

DrawLine(0.022f, 0.1f, 0.022f, Shields, 
		 vtxA.r, vtxA.g, vtxA.b, 
		 vtxB.r, vtxB.g, vtxB.b);
	}


	//Draw Base health display


    vtxA.r = 0;
    vtxA.g = 50;
    vtxA.b = 0;
    vtxB.r = 0;
    vtxB.g = 50;
    vtxB.b = 0;
DrawLine(0.990f, 0.1f, 0.990f, 0.2f, 
		 vtxA.r, vtxA.g, vtxA.b, 
		 vtxB.r, vtxB.g, vtxB.b);

DrawLine(0.989f, 0.1f, 0.989f, 0.2f, 
		 vtxA.r, vtxA.g, vtxA.b, 
		 vtxB.r, vtxB.g, vtxB.b);

DrawLine(0.988f, 0.1f, 0.988f, 0.2f, 
		 vtxA.r, vtxA.g, vtxA.b, 
		 vtxB.r, vtxB.g, vtxB.b);
    
    vtxA.r = 0;
    vtxA.g = 0;
    vtxA.b = 50;
    vtxB.r = 0;
    vtxB.g = 0;
    vtxB.b = 50;
DrawLine(0.980f, 0.1f, 0.980f, 0.2f, 
		 vtxA.r, vtxA.g, vtxA.b, 
		 vtxB.r, vtxB.g, vtxB.b);

DrawLine(0.979f, 0.1f, 0.979f, 0.2f, 
		 vtxA.r, vtxA.g, vtxA.b, 
		 vtxB.r, vtxB.g, vtxB.b);

DrawLine(0.978f, 0.1f, 0.978f, 0.2f, 
		 vtxA.r, vtxA.g, vtxA.b, 
		 vtxB.r, vtxB.g, vtxB.b);


	HomeBase = getObject(Player[localplayer].Team, OBJECTTYPE_BUILDING, OBJECTMESH_COMMANDCENTER, 1);

	Health = (float)((float)Objects[HomeBase].Health / (float)Objects[HomeBase].MaxHealth);
	Shields = (float)((float)Objects[HomeBase].Shield / (float)Objects[HomeBase].MaxShield);
	Health = 0.1f+ 0.1f * Health;
	Shields = 0.1f+ 0.1f * Shields;

    vtxA.r = 0;
    vtxA.g = 255;
    vtxA.b = 0;
    vtxB.r = 0;
    vtxB.g = 255;
    vtxB.b = 0;
DrawLine(0.990f, 0.1f, 0.990f, Health, 
		 vtxA.r, vtxA.g, vtxA.b, 
		 vtxB.r, vtxB.g, vtxB.b);

DrawLine(0.989f, 0.1f, 0.989f, Health, 
		 vtxA.r, vtxA.g, vtxA.b, 
		 vtxB.r, vtxB.g, vtxB.b);

DrawLine(0.988f, 0.1f, 0.988f, Health, 
		 vtxA.r, vtxA.g, vtxA.b, 
		 vtxB.r, vtxB.g, vtxB.b);

    
    vtxA.r = 0;
    vtxA.g = 0;
    vtxA.b = 255;
    vtxB.r = 0;
    vtxB.g = 0;
    vtxB.b = 255;
DrawLine(0.980f, 0.1f, 0.980f, Shields, 
		 vtxA.r, vtxA.g, vtxA.b, 
		 vtxB.r, vtxB.g, vtxB.b);

DrawLine(0.979f, 0.1f, 0.979f, Shields, 
		 vtxA.r, vtxA.g, vtxA.b, 
		 vtxB.r, vtxB.g, vtxB.b);

DrawLine(0.978f, 0.1f, 0.978f, Shields, 
		 vtxA.r, vtxA.g, vtxA.b, 
		 vtxB.r, vtxB.g, vtxB.b);



// Draw artificial horizon


float RotationX = sin((ControlRoll+90.0f)*DEGREE);
float RotationY = cos((ControlRoll+90.0f)*DEGREE);

for (float tempCount=-60.0f; tempCount<=60.0f; tempCount+=30.0f)
{
	if ( (ControlPitch>270.0f) || (ControlPitch<90.0f) )
		OffsetY	= sin((ControlPitch+tempCount)*DEGREE);
	else
		OffsetY	= sin((180.0f+ControlPitch+tempCount)*DEGREE);

	DrawLine(0.5f-(RotationX*0.1f), 0.5f-OffsetY-(RotationY*0.1f), 0.5f+(RotationX*0.1f), 0.5f-OffsetY+(RotationY*0.1f), 
		 0, 200, 0, 
		 0, 200, 0);
}







	if (Player[localplayer].ControlledObject!=-1)
	if (Objects[Player[localplayer].ControlledObject].isStarting)
	{
		sprintf(tempstring, "\\a(0 0 255 128)Takeoff..\n");
//		glPrint(tempstring, -0.09f, 0.0f, 0.0f, 0.0f, 1.0f);
		mytext.Size(10);
		mytext.Draw(310, 200, tempstring);
	}
	
	if (Player[localplayer].ControlledObject!=-1)
	if (Objects[Player[localplayer].ControlledObject].isLanding)
	{
		sprintf(tempstring, "\\a(0 0 255 128)Landing..\n");
//		glPrint(tempstring, -0.09f, 0.0f, 0.0f, 0.0f, 1.0f);
		mytext.Size(10);
		mytext.Draw(310, 200, tempstring);
	}

	if (Player[localplayer].ControlledObject!=-1)
	if (Objects[Player[localplayer].ControlledObject].isDiving)
	{
		sprintf(tempstring, "\\a(255 0 0 128)EJECT!\n");
//		glPrint(tempstring, -0.05f, 0.0f, 1.0f, 0.0f, 0.0f);
		mytext.Size(10);
		mytext.Draw(310, 200, tempstring);
	}
		glEnable(GL_DEPTH_TEST); 
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND); 
		glEnable(GL_LIGHTING); 




}





void BuyBuilding(int building, int TeamNum, float xg, float zg)
{

	int tempbuilding;

	if ((Team[TeamNum].ResourcesAvailable-Price[building]<0) && (!Cheat))
		return;


	tempbuilding = AddBuilding( building, TeamNum, xg, zg, 0);

	if ((tempbuilding!=-1) && (!Cheat))
		Team[TeamNum].ResourcesAvailable-=Price[building];

	
}


void setupPresetBase(int TeamNum)
{
	int HomeCenter;
	FxBool SaveCheat;
	SaveCheat = Cheat;
	Cheat = FXTRUE;

	HomeCenter = getObject(TeamNum, OBJECTTYPE_BUILDING, OBJECTMESH_COMMANDCENTER, 1);

	return;
	BuyBuilding(OBJECTMESH_POWERPLANT, TeamNum, Objects[HomeCenter].xGrid-2, Objects[HomeCenter].zGrid-2);
	BuyBuilding(OBJECTMESH_POWERPLANT, TeamNum, Objects[HomeCenter].xGrid, Objects[HomeCenter].zGrid-2);
	BuyBuilding(OBJECTMESH_SAM, TeamNum, Objects[HomeCenter].xGrid+2, Objects[HomeCenter].zGrid-2);
	BuyBuilding(OBJECTMESH_AAA, TeamNum, Objects[HomeCenter].xGrid-4, Objects[HomeCenter].zGrid-2);
	BuyBuilding(OBJECTMESH_AAA, TeamNum, Objects[HomeCenter].xGrid+2, Objects[HomeCenter].zGrid);
	BuyBuilding(OBJECTMESH_MINE, TeamNum, Objects[HomeCenter].xGrid+4, Objects[HomeCenter].zGrid);
	BuyBuilding(OBJECTMESH_SAM, TeamNum, Objects[HomeCenter].xGrid-2, Objects[HomeCenter].zGrid);
	BuyBuilding(OBJECTMESH_UPLINK, TeamNum, Objects[HomeCenter].xGrid, Objects[HomeCenter].zGrid+3);
	BuyBuilding(OBJECTMESH_AAA, TeamNum, Objects[HomeCenter].xGrid-2, Objects[HomeCenter].zGrid+4);
	BuyBuilding(OBJECTMESH_AAA, TeamNum, Objects[HomeCenter].xGrid+2, Objects[HomeCenter].zGrid+4);
	BuyBuilding(OBJECTMESH_POWERPLANT, TeamNum, Objects[HomeCenter].xGrid-4, Objects[HomeCenter].zGrid+2);
	BuyBuilding(OBJECTMESH_POWERPLANT, TeamNum, Objects[HomeCenter].xGrid-2, Objects[HomeCenter].zGrid+2);

	Cheat = SaveCheat;
}


#ifdef USE_GL

void GLParticlesSetup(void)
{
int i;
float particleIntensity;
float PSunRIntensity, PSunGIntensity, PSunBIntensity;

	PSunRIntensity=SunRIntensity/256;
	PSunGIntensity=SunGIntensity/256;
	PSunBIntensity=SunBIntensity/256;
	if (PSunRIntensity<0.1f)
		PSunRIntensity=0.1f;
	if (PSunGIntensity<0.1f)
		PSunGIntensity=0.1f;
	if (PSunBIntensity<0.1f)
		PSunBIntensity=0.1f;

		if (GLTextures)
			glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);					// Switch on the darned blending
		glBindTexture(GL_TEXTURE_2D, GLTextureHandle[6]);

	for (i=0; i<NumParticles; i++)
	if (Particles[i].isVisible)
	{
		particleIntensity=(float)(Particles[i].TimeToLive/Particles[i].TotalTimeToLive);

		glBegin(GL_QUADS);
			glTexCoord2f(0.0, 0.0);
			if (Particles[i].ParticleType==0)
			glColor4f(	(float)(Particles[i].r*particleIntensity*PSunRIntensity/256),
						(float)(Particles[i].g*particleIntensity*PSunGIntensity/256),
						(float)(Particles[i].b*particleIntensity*PSunBIntensity/256),
						0.5f);
			else
			glColor4f(	(float)(Particles[i].r*particleIntensity/256),
						(float)(Particles[i].g*particleIntensity/256),
						(float)(Particles[i].b*particleIntensity/256),
						0.5f);
			glVertex3f(Particles[i].xPos-100.0f, Particles[i].Height-100.0, Particles[i].zPos);

			glTexCoord2f(0.0, 1.0);
			if (Particles[i].ParticleType==0)
			glColor4f(	(float)(Particles[i].r*particleIntensity*PSunRIntensity/256),
						(float)(Particles[i].g*particleIntensity*PSunGIntensity/256),
						(float)(Particles[i].b*particleIntensity*PSunBIntensity/256),
						0.5f);
			else
			glColor4f(	(float)(Particles[i].r*particleIntensity/256),
						(float)(Particles[i].g*particleIntensity/256),
						(float)(Particles[i].b*particleIntensity/256),
						0.5f);
			glVertex3f(Particles[i].xPos-100.0, Particles[i].Height+100.0, Particles[i].zPos);

			glTexCoord2f(1.0, 1.0);
			if (Particles[i].ParticleType==0)
			glColor4f(	(float)(Particles[i].r*particleIntensity*PSunRIntensity/256),
						(float)(Particles[i].g*particleIntensity*PSunGIntensity/256),
						(float)(Particles[i].b*particleIntensity*PSunBIntensity/256),
						0.5f);
			else
			glColor4f(	(float)(Particles[i].r*particleIntensity/256),
						(float)(Particles[i].g*particleIntensity/256),
						(float)(Particles[i].b*particleIntensity/256),
						0.5f);
			glVertex3f(Particles[i].xPos+100.0, Particles[i].Height+100.0, Particles[i].zPos);

			
			glTexCoord2f(1.0, 0.0);
			if (Particles[i].ParticleType==0)
			glColor4f(	(float)(Particles[i].r*particleIntensity*PSunRIntensity/256),
						(float)(Particles[i].g*particleIntensity*PSunGIntensity/256),
						(float)(Particles[i].b*particleIntensity*PSunBIntensity/256),
						0.5f);
			else
			glColor4f(	(float)(Particles[i].r*particleIntensity/256),
						(float)(Particles[i].g*particleIntensity/256),
						(float)(Particles[i].b*particleIntensity/256),
						0.5f);
			glVertex3f(Particles[i].xPos+100.0, Particles[i].Height-100.0, Particles[i].zPos);


		glEnd();
		

	}


		glDisable(GL_BLEND);					// Switch on the darned blending

}

#endif



void RenderWater(void)
{
	if ((TerrainType==TERRAIN_LUSH) || (TerrainType==TERRAIN_STONE) || (TerrainType==TERRAIN_SNOW))
	{
		//Now render water surface
		if (!CompatibilityMode)
		{
//			glEnable(GL_TEXTURE_GEN_S);
//			glEnable(GL_TEXTURE_GEN_T);
//			glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);
//			glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);
//			glBindTexture(GL_TEXTURE_2D, GLTextureHandle[43]);

			glBindTexture(GL_TEXTURE_2D, GLTextureHandle[43]);
//			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);


			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		}
		glBegin(GL_TRIANGLES);
		glTexCoord2f(0.0f, 0.0f);
		glColor4f(0.4f,0.4f,0.4f,0.6f);
	    glNormal3f( 0.3f, -0.3f, 0.3f);
		glVertex3f( Waterplane[0].v.x, -200.0f, Waterplane[0].v.z);
		glTexCoord2f(0.0f, 64.0f);
		glColor4f(0.4f,0.4f,0.4f,0.6f);
	    glNormal3f( 0.3f, -0.3f, 0.3f);
		glVertex3f( Waterplane[1].v.x, -200.0f, Waterplane[1].v.z);
		glTexCoord2f(64.0f, 0.0f);
		glColor4f(0.4f,0.4f,0.4f,0.6f);
	    glNormal3f( 0.3f, -0.3f, 0.3f);
		glVertex3f( Waterplane[2].v.x, -200.0f, Waterplane[2].v.z);
		glEnd();		
		glBegin(GL_TRIANGLES);
		glTexCoord2f(64.0f, 64.0f);
		glColor4f(0.4f,0.4f,0.4f,0.6f);
	    glNormal3f( 0.3f, -0.3f, 0.3f);
		glVertex3f( Waterplane[3].v.x, -200.0f, Waterplane[3].v.z);
		glTexCoord2f(0.0f, 64.0f);
		glColor4f(0.4f,0.4f,0.4f,0.6f);
	    glNormal3f( 0.3f, -0.3f, 0.3f);
		glVertex3f( Waterplane[1].v.x, -200.0f, Waterplane[1].v.z);
		glTexCoord2f(64.0f, 0.0f);
		glColor4f(0.4f,0.4f,0.4f,0.6f);
	    glNormal3f( 0.3f, -0.3f, 0.3f);
		glVertex3f( Waterplane[2].v.x, -200.0f, Waterplane[2].v.z);
		glEnd();

		if (!CompatibilityMode)
		{
			glDisable(GL_BLEND);
//			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T); 
		}
	}
}

void RenderFloor(void)
{
		glBindTexture(GL_TEXTURE_2D, GLTerrainTextureHandle);
		glDisable(GL_BLEND);
		glBegin(GL_TRIANGLES);
		glTexCoord2f(0.0f, 0.0f);
		glColor4f(0.4f,0.4f,0.4f,0.6f);
	    glNormal3f( 0.3f, 0.3f, 0.3f);
		glVertex3d( Waterplane[0].v.x*100.0f, -1000.0f, Waterplane[0].v.z*100.0f);
		glTexCoord2f(0.0f, 16.0f);
		glColor4f(0.4f,0.4f,0.4f,0.6f);
	    glNormal3f( 0.3f, 0.3f, 0.3f);
		glVertex3d( Waterplane[1].v.x*100.0f, -1000.0f, Waterplane[1].v.z*100.0f);
		glTexCoord2f(16.0f, 0.0f);
		glColor4f(0.4f,0.4f,0.4f,0.6f);
	    glNormal3f( 0.3f, 0.3f, 0.3f);
		glVertex3d( Waterplane[2].v.x*100.0f, -1000.0f, Waterplane[2].v.z*100.0f);
		glEnd();		
		glBegin(GL_TRIANGLES);
		glTexCoord2f(16.0f, 16.0f);
		glColor4f(0.4f,0.4f,0.4f,0.6f);
	    glNormal3f( 0.3f, 0.3f, 0.3f);
		glVertex3d( Waterplane[3].v.x*100.0f, -1000.0f, Waterplane[3].v.z*100.0f);
		glTexCoord2f(0.0f, 16.0f);
		glColor4f(0.4f,0.4f,0.4f,0.6f);
	    glNormal3f( 0.3f, 0.3f, 0.3f);
		glVertex3d( Waterplane[1].v.x*100.0f, -1000.0f, Waterplane[1].v.z*100.0f);
		glTexCoord2f(16.0f, 0.0f);
		glColor4f(0.4f,0.4f,0.4f,0.6f);
	    glNormal3f( 0.3f, 0.3f, 0.3f);
		glVertex3d( Waterplane[2].v.x*100.0f, -1000.0f, Waterplane[2].v.z*100.0f);
		glEnd();		
}


void RenderSky(void)
{
GLfloat skyFogColor[] = {0.0f, 0.0f, 0.0f, 1.0f};

		glEnable(GL_BLEND);
		//Darken things
		if ((current_weather.WeatherType==WEATHERTYPE_STORM) ||
			(current_weather.WeatherType==WEATHERTYPE_CLOUDED))
				glBlendFunc(GL_SRC_ALPHA,GL_ZERO);
			else
				glBlendFunc(GL_SRC_ALPHA,GL_ONE);

//		glDisable(GL_LIGHTING);

		if (current_weather.WeatherType!=WEATHERTYPE_FOG)
		{
			// Set a whiteish fog
			skyFogColor[0] = fogColor[0]*1.5f;
			skyFogColor[1] = fogColor[1]*1.5f;
			skyFogColor[2] = fogColor[2]*1.5f;
			skyFogColor[3] = fogColor[3]*1.5f;
			glFogfv(GL_FOG_COLOR, skyFogColor);
			glFogf(GL_FOG_DENSITY, (FogIntensity/20.0f));
			glEnable(GL_FOG);
		}
		else
		{
			glFogfv(GL_FOG_COLOR, fogColor);
			glFogf(GL_FOG_DENSITY, (FogIntensity/2.0f));
			glEnable(GL_FOG);
		}

		glBindTexture(GL_TEXTURE_2D, GLTextureHandle[32]);
		glBegin(GL_TRIANGLES);
		glTexCoord2f(0.0f+SkyRotator2, 0.0f+SkyRotator2);
		glColor4f(1.6f,1.6f,1.6f,1.8f);
	    glNormal3f( 6.0f, 6.0f, 6.0f);
		glVertex3d( Waterplane[0].v.x*50.0f, 2000.0f, Waterplane[0].v.z*50.0f);
		glTexCoord2f(0.0f+SkyRotator2, 20.0f+SkyRotator2);
		glColor4f(1.6f,1.6f,1.6f,1.8f);
	    glNormal3f( 6.0f, 6.0f, 6.0f);
		glVertex3d( Waterplane[1].v.x*50.0f, 2000.0f, Waterplane[1].v.z*50.0f);
		glTexCoord2f(20.0f+SkyRotator2, 0.0f+SkyRotator2);
		glColor4f(1.6f,1.6f,1.6f,1.8f);
	    glNormal3f( 6.0f, 6.0f, 6.0f);
		glVertex3d( Waterplane[2].v.x*50.0f, 2000.0f, Waterplane[2].v.z*50.0f);
		glEnd();		
		glBegin(GL_TRIANGLES);
		glTexCoord2f(20.0f+SkyRotator2, 20.0f+SkyRotator2);
		glColor4f(1.6f,1.6f,1.6f,0.8f);
	    glNormal3f( 6.0f, 6.0f, 6.0f);
		glVertex3d( Waterplane[3].v.x*50.0f, 2000.0f, Waterplane[3].v.z*50.0f);
		glTexCoord2f(0.0f+SkyRotator2, 20.0f+SkyRotator2);
		glColor4f(1.6f,1.6f,1.6f,1.8f);
	    glNormal3f( 6.0f, 6.0f, 6.0f);
		glVertex3d( Waterplane[1].v.x*50.0f, 2000.0f, Waterplane[1].v.z*50.0f);
		glTexCoord2f(20.0f+SkyRotator2, 0.0f+SkyRotator2);
		glColor4f(1.6f,1.6f,1.6f,1.8f);
	    glNormal3f( 6.0f, 6.0f, 6.0f);
		glVertex3d( Waterplane[2].v.x*50.0f, 2000.0f, Waterplane[2].v.z*50.0f);
		glEnd();		

		
		glBindTexture(GL_TEXTURE_2D, GLTextureHandle[31]);
		glBegin(GL_TRIANGLES);
		glTexCoord2f(0.0f+SkyRotator1, 0.0f+SkyRotator1);
		glColor4f(0.6f,0.6f,0.6f,1.8f);
	    glNormal3f( 6.0f, 6.0f, 6.0f);
		glVertex3d( Waterplane[0].v.x*50.0f, 3000.0f, Waterplane[0].v.z*50.0f);
		glTexCoord2f(0.0f+SkyRotator1, 20.0f+SkyRotator1);
		glColor4f(0.6f,0.6f,0.6f,1.8f);
	    glNormal3f( 6.0f, 6.0f, 6.0f);
		glVertex3d( Waterplane[1].v.x*50.0f, 3000.0f, Waterplane[1].v.z*50.0f);
		glTexCoord2f(20.0f+SkyRotator1, 0.0f+SkyRotator1);
		glColor4f(0.6f,0.6f,0.6f,1.8f);
	    glNormal3f( 6.0f, 6.0f, 6.0f);
		glVertex3d( Waterplane[2].v.x*50.0f, 3000.0f, Waterplane[2].v.z*50.0f);
		glEnd();		
		glBegin(GL_TRIANGLES);
		glTexCoord2f(20.0f+SkyRotator1, 20.0f+SkyRotator1);
		glColor4f(0.6f,0.6f,0.6f,1.8f);
	    glNormal3f( 6.0f, 6.0f, 6.0f);
		glVertex3d( Waterplane[3].v.x*50.0f, 3000.0f, Waterplane[3].v.z*50.0f);
		glTexCoord2f(0.0f+SkyRotator1, 20.0f+SkyRotator1);
		glColor4f(0.6f,0.6f,0.6f,1.8f);
	    glNormal3f( 6.0f, 6.0f, 6.0f);
		glVertex3d( Waterplane[1].v.x*50.0f, 3000.0f, Waterplane[1].v.z*50.0f);
		glTexCoord2f(20.0f+SkyRotator1, 0.0f+SkyRotator1);
		glColor4f(0.6f,0.6f,0.6f,1.8f);
	    glNormal3f( 6.0f, 6.0f, 6.0f);
		glVertex3d( Waterplane[2].v.x*50.0f, 3000.0f, Waterplane[2].v.z*50.0f);
		glEnd();		
		
		glDisable(GL_BLEND);
//		glEnable(GL_LIGHTING);

		//Restore fog settings
		glFogfv(GL_FOG_COLOR, fogColor);
		glFogf(GL_FOG_DENSITY, (FogIntensity/2.0f));
		if (GLFog)
			glEnable(GL_FOG);
		else
			glDisable(GL_FOG);

}





void menuCreateTerrain(void)
{

//RemoveAllObjects();		//Clear everything

NewFractalSky1();		//sky texture 1
NewFractalSky2();		//sky texture 2

calcSky1();				//convert into OpenGL texture
calcSky2();				//ditto

RandomSeed = (int)(rand()%65534);

NewFractal();			//Fractal landscape heightmap

CreateLandscape(menu[2].val);		//Convert into landscape mesh and texturize (Lush etc.) as specified

numverts = numLSverts;
numfaces = numLSfaces;

startFrameTime = TimerGetTime();
}


//Same as menuCreateTerrain, but sends terrain information (Random seed, etc.) to all connected clients
void menuCreateTerrainMulti(void)
{

	startFrameTime = TimerGetTime();
}

void menuStartSingle(void)
{
int curplayer;
int ep, fp;

fp=menu[0].val;
ep=menu[1].val;

GameHalted=FXFALSE;

RemoveAllObjects();		//Clear everything

if (TerrainType!=menu[2].val)
	menuCreateTerrain();

	setupTeam(0);			//setup team 0 and 1
	setupTeam(1);
	setupPlayer(FXFALSE, FXTRUE, FXFALSE, FXFALSE, 0, OBJECTMESH_SHIP1); //The human player

	daytime = 2000.0f;		//Start somewhere in the morning
	SunHandler(FXTRUE);		//Initial Sun Stuff
	calcMap();



for (curplayer=0; curplayer<fp; curplayer++)		//Spawn Friendly AIs
	setupPlayer(FXTRUE, FXFALSE, FXFALSE, FXFALSE, 0, OBJECTMESH_SHIP1);

for (curplayer=0; curplayer<ep; curplayer++)		//Spawn Enemy AIs
	setupPlayer(FXTRUE, FXFALSE, FXFALSE, FXFALSE, 1, OBJECTMESH_SHIP2);


if ((!TextureEditor) && (presetBase))
	setupPresetBase(0);

if (!TextureEditor)
	setupPresetBase(1);


MenuActive=FXFALSE;
ConsoleActive=FXFALSE;
CurrentMenu=0;
startFrameTime = TimerGetTime();

}



void menuGameHost(void)
{

if (use_midas!=0)
	PlaySoundFX(SAMPLE_MENU_ENTER, SAMPLE_MENU_ENTER);

	if (myServer.isClient)
		Network_Disconnect();
	if (myServer.isActive)
		Network_StopServer();

	WriteErrorLog("X) Starting Network");
	myServer.MaxClients = menu[0].val;
//	sprintf(myServer.Name, "Testserver");

	sprintf(tempstring, "Particles: %i", NumParticles);
	WriteErrorLog(tempstring);
	RemoveAllObjects();
	WriteErrorLog("-  All objects cleared");
	Network_StartServer();
	WriteErrorLog("X) Network startup complete");

startFrameTime = TimerGetTime();
}


void menuGameJoin(void)
{
int i;

if (use_midas!=0)
	PlaySoundFX(SAMPLE_MENU_ENTER, SAMPLE_MENU_ENTER);

	if (myServer.isClient)
		Network_Disconnect();
	if (myServer.isActive)
		Network_StopServer();


//GetServerList();

numServers = 30;
for (i=0; i<numServers; i++)
{
	sprintf(Servers[i].Adress, "127.0.0.%i", i);
	sprintf(Servers[i].Name, "Testserver %i", i);
	Servers[i].CurClients=0;
	Servers[i].MaxClients=16;
	Servers[i].Version=myServer.Version;
}


JoinMenuActive = FXTRUE;

//GameHalted=FXFALSE;
startFrameTime = TimerGetTime();
}


void menuApplyGFX(void)
{

	if ((SCREEN_TERRAINTEXTURES==0) && (menu[2].state==1))
		GenerateTerrainTextures();

	//Restart game if we changed something drastic (like fullscreen and stuff
/*	if( (SCREEN_MODE!=(int)menu[0].val) ||
		(SCREEN_COLORBITS!=(int)menu[1].val) ||
		(SCREEN_FULLSCREEN!=(int)menu[4].val) ||
		(SOUND_ENABLED!=(int)menu[6].val) )
		RestartVideo();

	if( (SOUND_ENABLED!=(int)menu[6].val) )
		RestartProgram = 1;
*/
//Restart code doesn't work yet


	SCREEN_MODE = (int)(menu[0].val);
	SCREEN_COLORBITS = (int)(menu[1].val);
	SCREEN_LORES = menu[2].state;
	SCREEN_TERRAINTEXTURES = menu[3].state;
	if ((int)(menu[4].val)==0)
		SCREEN_LOD = 4;
	if ((int)(menu[4].val)==1)
		SCREEN_LOD = 8;
	if ((int)(menu[4].val)==2)
		SCREEN_LOD = 16;
	SCREEN_FULLSCREEN = menu[5].state;
//	SCREEN_FOG = menu[6].state;
	SOUND_ENABLED = menu[6].state;
	MOUSE_INVERT = menu[7].state;

//	use_midas = SOUND_ENABLED;
// Don't apply sound changes right now, that would cause problems!

	lod_level = SCREEN_LOD;

	if (SCREEN_FOG==0)
		GLFog = FXFALSE;
	else
		GLFog = FXTRUE;

	if (SCREEN_TERRAINTEXTURES==0)
		NoTexCalcs = FXTRUE;		//Skip the lengthy texture calculations
	else
		NoTexCalcs = FXFALSE;


if (CompatibilityMode)
{
	GLFog=FXFALSE;
	GLTextures=FXFALSE;
	use_lighing=FXFALSE;
	NoTexCalcs=FXTRUE;
	WireframeMode=FXTRUE;
}


}


//This is actually a copy of the old keyboard handler for the 'L' key, responsible for takeoff and landing
void menuLaunch(void)
{
	//We're in the air, so let's land
	if	((!Objects[Player[localplayer].ControlledObject].isDocked) &&
		(!Objects[Player[localplayer].ControlledObject].isDiving) &&
		(!Objects[Player[localplayer].ControlledObject].isLanding) &&
		(!Objects[Player[localplayer].ControlledObject].isStarting))
	{
		//...but only if we're over the command center
		if ((xGridPos==Objects[getObject(Player[localplayer].Team, OBJECTTYPE_BUILDING, OBJECTMESH_COMMANDCENTER, 1)].xGrid) &&
			(zGridPos==Objects[getObject(Player[localplayer].Team, OBJECTTYPE_BUILDING, OBJECTMESH_COMMANDCENTER, 1)].zGrid) &&
			(!Objects[getObject(Player[localplayer].Team, OBJECTTYPE_BUILDING, OBJECTMESH_COMMANDCENTER, 1)].isBusy) )
		{
			Objects[Player[localplayer].ControlledObject].isLanding=FXTRUE;
			Objects[Player[localplayer].ControlledObject].AnimationPhase=0;
			Objects[Player[localplayer].ControlledObject].AnimationSteps= (Objects[Player[localplayer].ControlledObject].Height - (Objects[getObject(Player[localplayer].Team, OBJECTTYPE_BUILDING, OBJECTMESH_COMMANDCENTER, 1)].Height+5.0f)) / 200;
			Objects[Player[localplayer].ControlledObject].InitialHeight= Objects[Player[localplayer].ControlledObject].Height;
			if (use_midas!=0)
				PlaySoundFX(6, 6);
			}
	}
	else	//If we're already docked, let's start
	if( (Objects[Player[localplayer].ControlledObject].isDocked) &&
		(!Objects[getObject(Player[localplayer].Team, OBJECTTYPE_BUILDING, OBJECTMESH_COMMANDCENTER, 1)].isBusy) )
	{
		if (menu[0].val==0)
			Objects[Player[localplayer].ControlledObject].ObjectMesh=OBJECTMESH_SHIP1;
		if (menu[0].val==1)
			Objects[Player[localplayer].ControlledObject].ObjectMesh=OBJECTMESH_SHIP2;
		if (menu[0].val==2)
			Objects[Player[localplayer].ControlledObject].ObjectMesh=OBJECTMESH_SHIP3;
		if (menu[0].val==3)
			Objects[Player[localplayer].ControlledObject].ObjectMesh=OBJECTMESH_LIGHTTANK;
		if (menu[0].val==4)
			Objects[Player[localplayer].ControlledObject].ObjectMesh=OBJECTMESH_MEDIUMTANK;

		int OldShip = Player[localplayer].ControlledObject;
		Player[localplayer].ControlledObject = AddObject( OBJECTTYPE_SHIP, Objects[Player[localplayer].ControlledObject].ObjectMesh,
					Objects[Player[localplayer].ControlledObject].Team,
					Objects[Player[localplayer].ControlledObject].TimeToLive,
					Objects[Player[localplayer].ControlledObject].isAIControlled,
					FXFALSE,
					FXTRUE,
					FXFALSE,
					FXTRUE,
					Objects[Player[localplayer].ControlledObject].xPos,
					Objects[Player[localplayer].ControlledObject].zPos,
					Objects[Player[localplayer].ControlledObject].Height,
					Objects[Player[localplayer].ControlledObject].Yaw,
					Objects[Player[localplayer].ControlledObject].Pitch,
					Objects[Player[localplayer].ControlledObject].Roll,
					Objects[Player[localplayer].ControlledObject].FiringObject,
					FXTRUE);
		RemoveObject(OldShip, FXTRUE);

		Objects[Player[localplayer].ControlledObject].MissileType = menu[1].val;
		Objects[Player[localplayer].ControlledObject].SpecialType = menu[2].val;

		//Reload missiles
		if (Objects[Player[localplayer].ControlledObject].MissileType==MISSILETYPE_GUIDED)
			Objects[Player[localplayer].ControlledObject].MissileAmmo=10;
		if (Objects[Player[localplayer].ControlledObject].MissileType==MISSILETYPE_MARKER)
			Objects[Player[localplayer].ControlledObject].MissileAmmo=2;
		if (Objects[Player[localplayer].ControlledObject].MissileType==MISSILETYPE_DUMBFIRE)
			Objects[Player[localplayer].ControlledObject].MissileAmmo=20;

		//Don't give him shields
		if (Objects[Player[localplayer].ControlledObject].SpecialType != SPECIALTYPE_SHIELDS)
		{
			Objects[Player[localplayer].ControlledObject].Shield=1;
			Objects[Player[localplayer].ControlledObject].MaxShield=1;
		}
		else
		{
			Objects[Player[localplayer].ControlledObject].Shield=Objects[Player[localplayer].ControlledObject].SaveMaxShield;
			Objects[Player[localplayer].ControlledObject].MaxShield=Objects[Player[localplayer].ControlledObject].SaveMaxShield;
		}

		Objects[Player[localplayer].ControlledObject].isDocked=FXFALSE;
		Objects[Player[localplayer].ControlledObject].isLanding=FXFALSE;
		Objects[Player[localplayer].ControlledObject].isStarting=FXTRUE;
		Objects[Player[localplayer].ControlledObject].Speed=0.0f;
		Objects[Player[localplayer].ControlledObject].AnimationPhase=0;
		Objects[Player[localplayer].ControlledObject].AnimationSteps=0.2f;
		Objects[Player[localplayer].ControlledObject].InitialHeight = Objects[Player[localplayer].ControlledObject].Height;
		LoadoutMenuActive=FXFALSE;
		if (use_midas!=0)
			PlaySoundFX(5, 5);
	}
}




void getVersion(void)
{
	
	char argumentbuffer1[256];

	strcpy(servername,SERVERNAME);											// initialize the global vars for autoupdate				
	strcpy(file_on_server,FILEONSERVER);

	ifstream OLD_file(VERSIONTAGOLD, ios::in); 								// Open the version files for reading 

	OLD_file >> argumentbuffer1;											// Check if files are valid	

	if (strcmpi(argumentbuffer1,"OldVersion:") != 0) 
	{
		OLD_file.close();	
		return;
	}
	else {
		OLD_file >> myServer.Version;													// Read version numbers
		OLD_file.close();	
		return;
	}
}


void menuAutoUpdate(void)							//  Here starts the autoupdate stuff
{
	
	long crcsum,crc;
	int oldv,newv,updatev,pf;
	char operation[256],new_servername[256],argumentbuffer1[256],argumentbuffer2[256];

	strcpy(servername,SERVERNAME);											// initialize the global vars for autoupdate				
	strcpy(file_on_server,FILEONSERVER);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	sprintf(tempstring, "\\c(255 255 255)Connecting to AutoUpdate Server...");
	mytext.Size(10);
	mytext.Draw(20, 20, tempstring);
	glFlush();
	SwapBuffers( DR_HDC ); 
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	sprintf(tempstring, "\\c(255 255 255)Connecting to AutoUpdate Server...");
	mytext.Size(10);
	mytext.Draw(20, 20, tempstring);
	glFlush();
	SwapBuffers( DR_HDC ); 



	WriteErrorLog("AUTOUPDATE: Getting Version File");

	GetHTTP(servername,file_on_server);										// look for new version file on the autoupdate server

	WriteErrorLog("AUTOUPDATE: Open Version files");
	
	ifstream OLD_file(VERSIONTAGOLD, ios::in); 								// Open the version files for reading 
	ifstream NEW_file(VERSIONTAGNEW, ios::in); 		

	WriteErrorLog("AUTOUPDATE: Validity Check");
	
	OLD_file >> argumentbuffer1;											// Check if files are valid	
	NEW_file >> argumentbuffer2;  	

	if ((strcmpi(argumentbuffer1,"OldVersion:") != 0) && (strcmpi(argumentbuffer2,"NewVersion:") != 0)) 
	{
		WriteErrorLog("Can't connect to update server!");
		AddMessage("Can't connect to update server!", 255, 255, 0);
		NEW_file.close();															// close files
		OLD_file.close();	
		pf = remove(VERSIONTAGNEW);
		startFrameTime = TimerGetTime();											// Keep our watchdog friendly
		return;
	}
	else {

		WriteErrorLog("AUTOUPDATE: Reading Version File");
		
		OLD_file >> oldv;													// Read version numbers
		NEW_file >> newv;  									

		if (oldv == newv)
		{
			AddMessage("No new update available", 255, 255, 0);
			WriteErrorLog("No new update available");
		}
		
		if (oldv < newv) {													// compare versions, only update if necessary
			AddMessage("Update successfull.\nPlease restart program!", 255, 255, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(0.0, 0.0, 0.0, 1.0);
			sprintf(tempstring, "\\c(255 255 255)Updating from v%i to v%i...", oldv, newv);
			mytext.Size(10);
			mytext.Draw(20, 20, tempstring);
			glFlush();
			SwapBuffers( DR_HDC ); 
			
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

					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					glClearColor(0.0, 0.0, 0.0, 1.0);
					sprintf(tempstring, "\\c(255 255 255)Processing update...");
					mytext.Size(10);
					mytext.Draw(20, 20, tempstring);
					glFlush();
					SwapBuffers( DR_HDC ); 

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
							frames=1;										// and quit
						}
						NEW_file >> operation;								// read next operation
					}																				
				}
//				else printf("\ncrc wrong\n");
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

startFrameTime = TimerGetTime();											// Keep our watchdog friendly

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

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		sprintf(tempstring, "\\c(255 255 255)Downloading %s: %i kB", lpFileName, (int)(TotalBytes/1024));
		mytext.Size(10);
		mytext.Draw(20, 20, tempstring);
		glFlush();
		SwapBuffers( DR_HDC ); 
		
		// Did the server close the connection?
		if (nRet == 0)
			break;
		// Write to stdout
		fwrite(szBuffer, nRet, 1, fp);
	}
	
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




//=================================================================================
// This function sets the camera's LookAt target so that we get the best action
//=================================================================================
void SearchForCoolLookAt(int type)
{

	int m2, t = 0;
	int found = 0;

//	n2 = random(10);

	LastLookAtChange=timeGetTime();

	do
	{
		m2 = random(NumObjects);
		t++;
		
		if (Objects[m2].ObjectId == m2)
		{
			if (type == OBJECTTYPE_BUILDING)
			{
				if (Objects[m2].ObjectType==OBJECTTYPE_BUILDING)
				{
					LookAtObject=m2;
					return;
				}
			} 
			else if ((type == OBJECTTYPE_SHIP) || (type == -1))
			{
				if (Objects[m2].ObjectType==OBJECTTYPE_SHIP)
				{
					LookAtObject=m2;
					return;
				}
			}
			else if (type == OBJECTTYPE_WEAPON)
			{
				if (Objects[m2].ObjectMesh==OBJECTMESH_MISSILE)
				{
					LookAtObject=m2;
					return;
				}
			}
		}
	} while	(t < 5*NumObjects);

}


//=================================================================================
// Responsible for the thunder/lightning gfx and sound
//=================================================================================
void ThunderHandler()
{
	int CurLightning=0;
	int tempLightningDelay=0;

	if (current_weather.ThunderAndLightning == 0)
	return;


	//Lightning flashes before the thunder "roll"
    if( ( ((int)(clock()) - current_weather.TimeAfterLastThunder ) > current_weather.LightningBurstTime[current_weather.NextLightningBurst]) && (current_weather.LightningBurstTime[current_weather.NextLightningBurst]>0))
	{
		if (current_weather.NextLightningBurst < current_weather.NumLightningBursts)
		current_weather.NextLightningBurst++;
		AddLight ( -1, 2, 
					235.0f,235.0f,255.0f,
					235.0f,235.0f,255.0f,
					235.0f,235.0f,255.0f,
					1.0f,0.0f,0.0f,
					0.0f, -500.0f, 0.0f);
	}


	// Thunder Roll and next thunder setup
    if( ((int)(clock()) - current_weather.TimeAfterLastThunder ) > current_weather.ThunderDelay)
    {
		do
		{
			tempint = (int)(rand()%8);
			if (tempint>=3)
				tempint=37-26;
		} while (current_weather.LastThunderType == tempint);

		//Don't sound thunder all the time
		if (((int)(rand()%5)) > 1)
		{
			current_weather.LastThunderType = tempint;
			PlayPositionalSound ( 26+tempint,26+tempint,0,0,-300,10000.0f );
//			FSOUND_SetFrequency ( channels[26], (20100+ ((int)rand()%13000)) );
		}

		//setup next thunder
		current_weather.TimeAfterLastThunder  = (int)(clock());
		current_weather.ThunderDelay = 5000+rand()%25000; // add a 0 to both values

		//Setup next lightning
		current_weather.NextLightningBurst = 0;
		current_weather.NumLightningBursts = rand()%10 - 3;
		if (current_weather.NumLightningBursts<1)
			current_weather.NumLightningBursts=1;

		if (current_weather.NumLightningBursts>4)
			tempLightningDelay=1000;
		else
			tempLightningDelay=600;

		for (CurLightning=0; CurLightning<current_weather.NumLightningBursts; CurLightning++)
		{
			current_weather.LightningBurstTime[CurLightning]=current_weather.ThunderDelay-(int)(rand()%tempLightningDelay);
			tempLightningDelay = tempLightningDelay-(int)(rand()%tempLightningDelay);
		}
		current_weather.LightningBurstTime[current_weather.NumLightningBursts]=0;
		current_weather.LightningBurstTime[current_weather.NumLightningBursts+1]=0;
		current_weather.LightningBurstTime[current_weather.NumLightningBursts+2]=0;

    }
}



//=================================================================================
// Responsible for ambient sound fx
//=================================================================================
void AmbientSoundHandler()
{
int i;

	SoundFactor[SAMPLE_AMBIENT_CRICKETS]=0.0f;
	SoundFactor[SAMPLE_AMBIENT_CRICKETSNIGHT]=0.0f;
	SoundFactor[SAMPLE_AMBIENT_RAIN]=0.0f;
	SoundFactor[SAMPLE_AMBIENT_RAINLIGHT]=0.0f;
	SoundFactor[SAMPLE_AMBIENT_SUMMER]=0.0f;
	SoundFactor[SAMPLE_AMBIENT_WIND]=0.0f;
	SoundFactor[SAMPLE_AMBIENT_WINDHIGHALT]=0.0f;

/*	if (!FSOUND_IsPlaying(Noise[channels[SAMPLE_AMBIENT_WINDHIGHALT]].ChannelID))
			PlaySoundFX(SAMPLE_AMBIENT_WINDHIGHALT, SAMPLE_AMBIENT_WINDHIGHALT);
	if (!FSOUND_IsPlaying(Noise[channels[SAMPLE_AMBIENT_WIND]].ChannelID))
			PlaySoundFX(SAMPLE_AMBIENT_WIND, SAMPLE_AMBIENT_WIND);
	if (!FSOUND_IsPlaying(Noise[channels[SAMPLE_AMBIENT_SUMMER]].ChannelID))
			PlaySoundFX(SAMPLE_AMBIENT_SUMMER, SAMPLE_AMBIENT_SUMMER);
	if (!FSOUND_IsPlaying(Noise[channels[SAMPLE_AMBIENT_RAINLIGHT]].ChannelID))
			PlaySoundFX(SAMPLE_AMBIENT_RAINLIGHT, SAMPLE_AMBIENT_RAINLIGHT);
	if (!FSOUND_IsPlaying(Noise[channels[SAMPLE_AMBIENT_RAIN]].ChannelID))
			PlaySoundFX(SAMPLE_AMBIENT_RAIN, SAMPLE_AMBIENT_RAIN);
	if (!FSOUND_IsPlaying(Noise[channels[SAMPLE_AMBIENT_CRICKETSNIGHT]].ChannelID))
			PlaySoundFX(SAMPLE_AMBIENT_CRICKETSNIGHT, SAMPLE_AMBIENT_CRICKETSNIGHT);
	if (!FSOUND_IsPlaying(Noise[channels[SAMPLE_AMBIENT_CRICKETS]].ChannelID))
			PlaySoundFX(SAMPLE_AMBIENT_CRICKETS, SAMPLE_AMBIENT_CRICKETS);
*/

/*
	if (!FSOUND_IsPlaying(channels[SAMPLE_AMBIENT_WINDHIGHALT]))
	{
			PlaySoundFX(SAMPLE_AMBIENT_WINDHIGHALT, SAMPLE_AMBIENT_WINDHIGHALT);
			FSOUND_SetVolume(channels[SAMPLE_AMBIENT_WINDHIGHALT], 0);
	}
	if (!FSOUND_IsPlaying(channels[SAMPLE_AMBIENT_WIND]))
	{
			PlaySoundFX(SAMPLE_AMBIENT_WIND, SAMPLE_AMBIENT_WIND);
			FSOUND_SetVolume(channels[SAMPLE_AMBIENT_WIND], 0);
	}
	if (!FSOUND_IsPlaying(channels[SAMPLE_AMBIENT_SUMMER]))
	{
			PlaySoundFX(SAMPLE_AMBIENT_SUMMER, SAMPLE_AMBIENT_SUMMER);
			FSOUND_SetVolume(channels[SAMPLE_AMBIENT_SUMMER], 0);
	}
	if (!FSOUND_IsPlaying(channels[SAMPLE_AMBIENT_RAINLIGHT]))
	{
			PlaySoundFX(SAMPLE_AMBIENT_RAINLIGHT, SAMPLE_AMBIENT_RAINLIGHT);
			FSOUND_SetVolume(channels[SAMPLE_AMBIENT_RAINLIGHT], 0);
	}
	if (!FSOUND_IsPlaying(channels[SAMPLE_AMBIENT_RAIN]))
	{
			PlaySoundFX(SAMPLE_AMBIENT_RAIN, SAMPLE_AMBIENT_RAIN);
			FSOUND_SetVolume(channels[SAMPLE_AMBIENT_RAIN], 0);
	}
	if (!FSOUND_IsPlaying(channels[SAMPLE_AMBIENT_CRICKETSNIGHT]))
	{
			PlaySoundFX(SAMPLE_AMBIENT_CRICKETSNIGHT, SAMPLE_AMBIENT_CRICKETSNIGHT);
			FSOUND_SetVolume(channels[SAMPLE_AMBIENT_CRICKETSNIGHT], 0);
	}
	if (!FSOUND_IsPlaying(channels[SAMPLE_AMBIENT_CRICKETS]))
	{
			PlaySoundFX(SAMPLE_AMBIENT_CRICKETS, SAMPLE_AMBIENT_CRICKETS);
			FSOUND_SetVolume(channels[SAMPLE_AMBIENT_CRICKETS], 0);
	}
*/

	//Rain Stuff
	if (current_weather.WeatherPrecipitation==WEATHERPREC_RAIN)
	{
		SoundFactor[SAMPLE_AMBIENT_RAINLIGHT]=current_weather.PrecipitationStrength*4.0f;
		if (SoundFactor[SAMPLE_AMBIENT_RAINLIGHT]>1.0f)
			SoundFactor[SAMPLE_AMBIENT_RAINLIGHT]=1.0f;
		SoundFactor[SAMPLE_AMBIENT_RAIN]=current_weather.PrecipitationStrength;
			SoundFactor[SAMPLE_AMBIENT_RAINLIGHT]/=2.0f;
			SoundFactor[SAMPLE_AMBIENT_RAINLIGHT]/=2.0f;
	}

	if ((current_weather.WeatherPrecipitation!=WEATHERPREC_SNOW) && 
		(current_weather.WeatherPrecipitation!=WEATHERPREC_METEORITES) && 
		(current_weather.WeatherType==WEATHERTYPE_SUNNY))
	{

		SoundFactor[SAMPLE_AMBIENT_CRICKETS]=( fixtime/7200.0f*3.0f ) - 2.0f ;
		if (SoundFactor[SAMPLE_AMBIENT_CRICKETS]>1.0f)
			SoundFactor[SAMPLE_AMBIENT_CRICKETS]=1.0f;
		if (SoundFactor[SAMPLE_AMBIENT_CRICKETS]<0.0f)
			SoundFactor[SAMPLE_AMBIENT_CRICKETS]=0.0f;

		SoundFactor[SAMPLE_AMBIENT_CRICKETSNIGHT]=( (1.0f-(fixtime/7200.0f))*3.0f ) - 2.0f;
		if (SoundFactor[SAMPLE_AMBIENT_CRICKETSNIGHT]>1.0f)
			SoundFactor[SAMPLE_AMBIENT_CRICKETSNIGHT]=1.0f;
		if (SoundFactor[SAMPLE_AMBIENT_CRICKETSNIGHT]<0.0f)
			SoundFactor[SAMPLE_AMBIENT_CRICKETSNIGHT]=0.0f;

		SoundFactor[SAMPLE_AMBIENT_SUMMER]=fixtime/7200.0f ;
		SoundFactor[SAMPLE_AMBIENT_WIND]=(1.0f-(fixtime/7200.0f)) / 4.0f ;


	}
	
	if (current_weather.WeatherType==WEATHERTYPE_STORM)
	{
		SoundFactor[SAMPLE_AMBIENT_CRICKETS]=0.0f;
		SoundFactor[SAMPLE_AMBIENT_CRICKETSNIGHT]=0.0f;
		SoundFactor[SAMPLE_AMBIENT_SUMMER]=0.0f;
		SoundFactor[SAMPLE_AMBIENT_WIND]=0.4f;
	}

	if (current_weather.WeatherType==WEATHERTYPE_FOG)
	{
		SoundFactor[SAMPLE_AMBIENT_WIND]=(7200.0f-fixtime)/7200.0f / 2.0f ;
	}

	if (current_weather.WeatherType==WEATHERTYPE_CLOUDED)
	{
		SoundFactor[SAMPLE_AMBIENT_WIND]=(7200.0f-fixtime)/7200.0f / 2.0f + 0.3f;
	}


	//The Ground-Level sounds are less intense the higher the camera is

	SoundFactor[SAMPLE_AMBIENT_WINDHIGHALT] = (-CamHeight/200.0f);
	if (SoundFactor[SAMPLE_AMBIENT_WINDHIGHALT]>1.0f)
		SoundFactor[SAMPLE_AMBIENT_WINDHIGHALT]=1.0f;
	if (SoundFactor[SAMPLE_AMBIENT_WINDHIGHALT]<0.0f)
		SoundFactor[SAMPLE_AMBIENT_WINDHIGHALT]=0.0f;
	SoundFactor[SAMPLE_AMBIENT_SUMMER] *= (1.0f - SoundFactor[SAMPLE_AMBIENT_WINDHIGHALT]);

	//Crickets are only audible very close to the ground.
	SoundFactor[SAMPLE_AMBIENT_CRICKETS] *= (1.0f - (-CamHeight/20.0f));
	if (SoundFactor[SAMPLE_AMBIENT_CRICKETS]>1.0f)
		SoundFactor[SAMPLE_AMBIENT_CRICKETS]=1.0f;
	if (SoundFactor[SAMPLE_AMBIENT_CRICKETS]<0.0f)
		SoundFactor[SAMPLE_AMBIENT_CRICKETS]=0.0f;
	SoundFactor[SAMPLE_AMBIENT_CRICKETSNIGHT] *= (1.0f - (-CamHeight/20.0f));
	if (SoundFactor[SAMPLE_AMBIENT_CRICKETSNIGHT]>1.0f)
		SoundFactor[SAMPLE_AMBIENT_CRICKETSNIGHT]=1.0f;
	if (SoundFactor[SAMPLE_AMBIENT_CRICKETSNIGHT]<0.0f)
		SoundFactor[SAMPLE_AMBIENT_CRICKETSNIGHT]=0.0f;



	for (i=0; i<=maxsamples; i++)
	{

		if (curSoundFactor[i] <= SoundFactor[i])
			curSoundFactor[i] = curSoundFactor[i] + 0.01f*(float)g_ispeedfactor;
		if (curSoundFactor[i] > SoundFactor[i])
			curSoundFactor[i] = curSoundFactor[i] - 0.01f*(float)g_ispeedfactor;

		if (curSoundFactor[i] < 0.0f)
			curSoundFactor[i] = 0.0f;
		if (curSoundFactor[i] > 1.0f)
			curSoundFactor[i] = 1.0f;
		if (SoundFactor[i] < 0.0f)
			SoundFactor[i] = 0.0f;
		if (SoundFactor[i] > 1.0f)
			SoundFactor[i] = 1.0f;


		curIntSoundFactor[i] = (int)(curSoundFactor[i]*256.0f);
		prevIntSoundFactor[i] = (int)(prevSoundFactor[i]*256.0f);


//	sprintf(tempstring, "curIntSoundFactor[%i]=%i", i, curIntSoundFactor[i]);
//	WriteErrorLog(tempstring);
//	sprintf(tempstring, "prevIntSoundFactor[%i]=%i", i, prevIntSoundFactor[i]);
//	WriteErrorLog(tempstring);


		if ( (curIntSoundFactor[i] != 0.0f) && (prevIntSoundFactor[i] == 0.0f) )
			PlaySoundFX(i, i);
		else if ( (curIntSoundFactor[i] == 0.0f) && (prevIntSoundFactor[i] != 0.0f) )
			StopSoundFX(i);


		if (curIntSoundFactor[i] != prevIntSoundFactor[i])
			FSOUND_SetVolume(Noise[channelEmitter[i]].ChannelID, (int)(curSoundFactor[i]*256.0f));	//Just a volume change, so update it.
		prevSoundFactor[i]=curSoundFactor[i];
	}
}




//=================================================================================
// Draws a single raindrop
//=================================================================================
void RenderRaindrop(int i)
{

	glBegin(GL_LINES);
	if (((fogColor[0]*1.1f) + (fogColor[1]*1.1f) + (fogColor[2]*1.1f)) / 3.0f < 0.7f)
		glColor4f(fogColor[0]*1.1f,fogColor[1]*1.1f,fogColor[2]*1.1f,0.8f);
	else
		glColor4f(0.7f, 0.7f, 0.7f, 0.8f);
	glVertex3f(Raindrops[i].xPos, Raindrops[i].Height+1.0f, Raindrops[i].zPos);
	glVertex3f(Raindrops[i].xPos, Raindrops[i].Height-1.0f, Raindrops[i].zPos);
	glEnd();

}



//=================================================================================
// Manages all raindrops
//=================================================================================
void RenderRain(void)
{
	int i;
	int NewDropletsPerFrame = 5;
	float vxdist, vydist, vzdist;

	TargetNumRaindrops = (int)((float)(current_weather.PrecipitationStrength)*4000.0f);

	// Add Raindrops
	if (CurNumRaindrops<TargetNumRaindrops)
	{
		for (i=CurNumRaindrops; i<(CurNumRaindrops+NewDropletsPerFrame); i++)
		{
			Raindrops[i].Height = -CamHeight + ((float)(rand()%2000)/10.0f) - 100.0f;
			Raindrops[i].xPos = -CamXPos + (float)(rand()%200) - 100.0f;
			Raindrops[i].zPos = -CamZPos + (float)(rand()%200) - 100.0f;
		}
		CurNumRaindrops+=NewDropletsPerFrame;
	}

	// Remove Raindrops
	if (CurNumRaindrops>TargetNumRaindrops)
		CurNumRaindrops-=NewDropletsPerFrame;

	if (CurNumRaindrops>3979)
		CurNumRaindrops=3979;
	if (TargetNumRaindrops>3979)
		TargetNumRaindrops=3979;

	// Exit if we don't need rain
	if (CurNumRaindrops == 0)
		return;


	glDisable(GL_LIGHTING); 

	// Move downwards and check if we need to reset a raindrop
	for (i=0; i<CurNumRaindrops; i++)
	{
		Raindrops[i].Height -= 4.0f*g_ispeedfactor;

		if (Raindrops[i].xPos > CamXPos)
			vxdist = (Raindrops[i].xPos + CamXPos);
		else
			vxdist = (CamXPos + Raindrops[i].xPos);

		if (Raindrops[i].Height > CamHeight)
			vydist = (Raindrops[i].Height + CamHeight);
		else
			vydist = (CamHeight + Raindrops[i].Height);
           
		if (Raindrops[i].zPos > CamZPos)
			vzdist = (Raindrops[i].zPos + CamZPos);
		else
			vzdist = (CamZPos + Raindrops[i].zPos);

		if (vxdist<0)
			vxdist=(-1.0f)*vxdist;
		if (vydist<0)
			vydist=(-1.0f)*vydist;
		if (vzdist<0)
			vzdist=(-1.0f)*vzdist;

		if (vydist > 100.0f)
			Raindrops[i].Height = -CamHeight + ((float)(rand()%2000)/10.0f) - 100.0f;
		if (vxdist > 100.0f)
			Raindrops[i].xPos = -CamXPos + ((float)(rand()%2000)/10.0f) - 100.0f;
		if (vzdist > 100.0f)
			Raindrops[i].zPos = -CamZPos + ((float)(rand()%2000)/10.0f) - 100.0f;


		RenderRaindrop(i);

	}

	glEnable(GL_LIGHTING); 

}


int FindChrInString(char *s, char c, int num)
{
	int m;
	int fnum=0;
	for (m=0; (unsigned int)(m)<strlen(s); m++)
	{
		if (s[m]==c)
		{
			fnum++;
			if (fnum>=num)
				break;
		}
	}
return m;
}


void FindWord(char *line, int num)
{
int i;
int start, end;

  if (num==0)
  {
	start = 0;
	end = FindChrInString( line, ' ', 1 ) - 1;
  }
  else
  {
	start = FindChrInString( line, ' ', num ) + 1;
	end = FindChrInString( line, ' ', (num+1) ) - 1;
  }


  for (i=start; i<=end; i++)
  {
    tempstring[i-start] = line[i];
  }
  tempstring[i-start] = '\0';


}

void FindRemainingLineAfterWord(char *line, int num)
{
int i;
int start, end;

  if (num==0)
  {
	start = 0;
	end = strlen(line);
  }
  else
  {
	start = FindChrInString( line, ' ', num ) + 1;
	end = strlen(line);
  }


  for (i=start; i<=end; i++)
  {
    tempstring[i-start] = line[i];
  }
  tempstring[i-start] = '\0';


}

void SendTeamMessage(char *msg)
{

}

void SendGlobalMessage(char *msg)
{

}

void PlayTaunt(char *file)
{
	char RealFile[256];

	if (use_midas!=1)
		return;

	sprintf(RealFile, "taunts\\%s", file);

	stream2 = FSOUND_Stream_OpenFile(RealFile, FSOUND_NORMAL | FSOUND_2D, 0);
	if (!stream2)
	{
		sprintf(tempstring, "WARNING! PlayTaunt(): Error loading file %s", RealFile);
		WriteErrorLog(tempstring);
	}
	else
		channels[126] = FSOUND_Stream_Play(FSOUND_FREE, stream2);
}


void LoadTauntMenu(int TauntChoice)
{
	FILE *DataFile = NULL;
	char *file;
	int curpos=0;
	int i;
	int maxpos=0;
	char  line[80];
	char cursubmenu[80];
	int curTauntHistoryPos;

	file = "taunts\\taunts.cfg";

//	WriteErrorLog("Executing LoadTauntMenu");
//	sprintf(tempstring, "%i", TauntChoice);
//	WriteErrorLog(tempstring);
//	WriteErrorLog("History:");
//	for (curTauntHistoryPos=0; curTauntHistoryPos < TauntMenuHistoryLen; curTauntHistoryPos++)
//	{
//		sprintf(tempstring, "%i", TauntMenuHistory[curTauntHistoryPos]);
//		WriteErrorLog(tempstring);
//	}


	DataFile = fopen(file,"r");
	if (DataFile == NULL)
		return;

	if ((TauntMenuHistoryLen == 0) && (TauntChoice == 0))
	{
		//Load the main menu
		curpos=0; maxpos=0;
		do 
		{
			do 
			{
				fgets(line,80,DataFile);
			} while ((line[0] == '/') && (line[1] == '/'));
			tempstring[0]=line[0];
			tempstring[1]='\0';
			curpos = atoi(tempstring);
			if (curpos>0)
			{
				FindRemainingLineAfterWord(line, 2);
				sprintf(TauntMenuDisplay[curpos], "%i. %s", curpos, tempstring);
				if (curpos>maxpos)
					maxpos=curpos;
			}
	
		} while (line[0] != '*');
		if (DataFile != NULL)
			fclose(DataFile);
		TauntMenuDisplaySize = maxpos;
		return;
	}
	else
	{
		if (TauntChoice > 0)
		{
			for (i=0; i<TauntChoice; i++)
			{
				TauntMenuHistory[TauntMenuHistoryLen] = TauntChoice;
				TauntMenuHistoryLen++;
				if (TauntMenuHistoryLen >= 99)
				{
					TauntMenuHistoryLen=0;
					TauntMenuActive=FXFALSE;
					return;
				}
			}
		}

		//Browse through the taunt menu history.
		for (curTauntHistoryPos=0; curTauntHistoryPos < TauntMenuHistoryLen; curTauntHistoryPos++)
		{
//				sprintf(tempstring, "We're at history %i of %i", curTauntHistoryPos, TauntMenuHistoryLen);
//				WriteErrorLog(tempstring);

				do 
				{
					fgets(line,80,DataFile);
				} while ((line[0] == '/') && (line[1] == '/'));
				tempstring[0]=line[0];
				tempstring[1]='\0';
				curpos = atoi(tempstring);
				// Found our menu entry
				if (curpos == TauntMenuHistory[curTauntHistoryPos])
				{
//					WriteErrorLog("Found menuentry");
					FindWord(line, 1);
//					WriteErrorLog(tempstring);

					//Check if it really points to another submenu
					if (tempstring[0] == '*')
					{
//						WriteErrorLog("It's a submenu!");

						//We found a submenu entry
						FindRemainingLineAfterWord(line, 2);
						sprintf(cursubmenu, "%s", tempstring);
//						WriteErrorLog("Looking for..");
//						WriteErrorLog(tempstring);

						//browse though the rest of the file for the submenu definition
						do 
						{
							do 
							{
								fgets(line,80,DataFile);
							} while ((line[0] == '/') && (line[1] == '/'));
							FindRemainingLineAfterWord(line, 1);
//							WriteErrorLog("..comparing with:");
//							WriteErrorLog(tempstring);
						} while ((line[0]!='*') || (strcmp(tempstring,cursubmenu) != 0));
						
						if ((TauntMenuHistoryLen-1) == curTauntHistoryPos)
						{
//							WriteErrorLog("We're at the last point of our history!");

							//Load the submenu
							curpos=0; maxpos=0;
							do 
							{
								do 
								{
									fgets(line,80,DataFile);
								} while ((line[0] == '/') && (line[1] == '/'));
//								WriteErrorLog("Loading final menu:");
//								WriteErrorLog(line);

								tempstring[0]=line[0];
								tempstring[1]='\0';
								curpos = atoi(tempstring);
								if (curpos>0)
								{
									FindRemainingLineAfterWord(line, 2);
									sprintf(TauntMenuDisplay[curpos], "%i. %s", curpos, tempstring);
									if (curpos>maxpos)
										maxpos=curpos;
								}
							} while (line[0] != '*');
							if (DataFile != NULL)
								fclose(DataFile);
							TauntMenuDisplaySize = maxpos;
							return;
						}
					}
					else // We found a wave file, so play it!
					{
//						WriteErrorLog("Playing Wave...");
//						WriteErrorLog(tempstring);

						PlayTaunt(tempstring);
						if (TauntMenuHistory[0]<=2)
						{
							FindRemainingLineAfterWord(line, 2);
							SendTeamMessage(tempstring);
						}
						else
						{
							FindRemainingLineAfterWord(line, 2);
							SendGlobalMessage(tempstring);
						}
						TauntMenuActive=FXFALSE;
						TauntMenuDisplaySize=0;
						TauntMenuHistory[0]=0;
						TauntMenuHistoryLen=0;
						if (DataFile != NULL)
							fclose(DataFile);
						return;
					}
				}
		}

	}


	if (DataFile != NULL)
		fclose(DataFile);
}











/*

  "Why do my eyes hurt?"

  - "You've never used them before"

*/






//============================================================================
//****************************************************************************
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//****************************************************************************
//============================================================================
//
// Start of void main()
//
//============================================================================
//****************************************************************************
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//****************************************************************************
//============================================================================




void main( int argc, char **argv, HDC hDC, HWND hWnd) {
    char match; 
    char **remArgs;
    int  rv;
	int i, o;
	float st, fi;
	int xxx;
	float fps;
	MSG msg;
	int LastUsedTexture=-1;

	float vxdist, vydist, vzdist;
	float xrot, yrot, zrot;
    float      xdist, ydist, height;
	char lead1[2];
	char lead2[2];
			
	WORD wVersionRequested = MAKEWORD(1,1);
	WSADATA wsaData;
	int nRet;


	DR_HDC = hDC;

	xxx=0;
	st=0.0f;
	fi=0.0f;
	fps=0.0f;

	f1++;

	getVersion();
	TauntMenuDisplaySize=0;
	TauntMenuHistory[0]=0;
	TauntMenuHistoryLen=0;
	TauntMenuActive=FXFALSE;

	g_s1 = 1.0f;
	g_s2 = 1.0f;
	g_s3 = 1.0f;
	g_s4 = 1.0f;
	g_s5 = 1.0f;
	g_s6 = 1.0f;
	g_s7 = 1.0f;
	g_s8 = 1.0f;
	g_s9 = 1.0f;
	g_s10 = 1.0f;

	NumEmitters = 0;
	NumParticles = 0;
	MaxParticles = 5000;		//Maximum amount of particles allowed in the game
	NumLights=0;
	AbsoluteMaxParticles = 10000;//NEVER EVER exceed this many particles!!
	CurrentActiveEmitter = 0;	//When limiting particles is needed, this will tell which emitter is active;
	maxsamples=0;


	Price[OBJECTMESH_COMMANDCENTER]=15000;
	Price[OBJECTMESH_POWERPLANT]=1000;
	Price[OBJECTMESH_MINE]=1500;
	Price[OBJECTMESH_AAA]=500;
	Price[OBJECTMESH_SAM]=800;
	Price[OBJECTMESH_UPLINK]=7000;


	myServer.isClient=FXFALSE;
	myServer.isActive=FXFALSE;

	use_midas = SOUND_ENABLED;
	lod_level = SCREEN_LOD;

	if (SCREEN_FOG==0)
		GLFog = FXFALSE;
	else
		GLFog = FXTRUE;

	if (SCREEN_TERRAINTEXTURES==0)
		NoTexCalcs = FXTRUE;		//Skip the lengthy texture calculations
	else
		NoTexCalcs = FXFALSE;

	if (SCREEN_WIREFRAME==0)
		WireframeMode = FXFALSE;
	else
		WireframeMode = FXTRUE;





    /* Process Command Line Arguments */
    while( rv = tlGetOpt( argc, argv, "nr", &match, &remArgs ) ) {
        if ( rv == -1 ) {
            printf( "Unrecognized command line argument\n" );
            printf( "%s \n", name );
            return;
        }
        switch( match ) {
        case 'n':
            frames = atoi( remArgs[0] );
            break;
        case 'x':
            Cheat = FXTRUE;
            break;

		case 'b':
			presetBase=FXTRUE;
			break;

        }
    }

	WriteErrorLog("X) Initializing Winsock");

	//
	// Initialize WinSock.dll
	//
	nRet = WSAStartup(wVersionRequested, &wsaData);
	if (nRet)
	{
		WriteErrorLog("Error Initializing Winsock!");
		MessageBox(NULL, "Winsock could not be initialized!", "Winsock initialisation failed", MB_ICONSTOP);
		WSACleanup();
		exit (1);
	}
	//
	// Check WinSock version
	//
	if (wsaData.wVersion != wVersionRequested)
	{
		WriteErrorLog("Invalid Winsock Version!");
		MessageBox(NULL, "Invalid Winsock Version!", "Winsock initialisation failed", MB_ICONSTOP);
		WSACleanup();
		exit (1);
	}


	WriteErrorLog("X) Allocating Memory for Render Pipeline");

//	heightfield = new float[78*78]; if (heightfield == NULL) { WriteErrorLog("Memory allocation error!"); exit(1); }
//	heightfield = (float *)malloc(78*78); memset(heightfield,0,(78*78));

//	sky1 = new int[65536]; if (sky1 == NULL) { WriteErrorLog("Memory allocation error!"); exit(1); }
//	sky2 = new int[65536]; if (sky2 == NULL) { WriteErrorLog("Memory allocation error!"); exit(1); }

	skytex1 = new unsigned char[256 * 256 * 3]; 
	skytex2 = new unsigned char[256 * 256 * 3]; 


//	Meshes = new mesh3d[30]; if (Faces == NULL) { WriteErrorLog("Memory allocation error!"); exit(1); }
//	for (tempint=0; tempint<30; tempint++)
//	{
//		Meshes[tempint].Faces = new face3d[1000]; if (Meshes[tempint].Faces == NULL) { WriteErrorLog("Memory allocation error!"); exit(1); }
//		Meshes[tempint].Vertices = new vert3d[3000]; if (Meshes[tempint].Vertices == NULL) { WriteErrorLog("Memory allocation error!"); exit(1); }
//	}

//	Player = new player3d[128]; if (Player == NULL) { WriteErrorLog("Memory allocation error!"); exit(1); }
//	Team = new team3d[10]; if (Team == NULL) { WriteErrorLog("Memory allocation error!"); exit(1); }
//	Objects = new object3d[2000]; if (Objects == NULL) { WriteErrorLog("Memory allocation error!"); exit(1); }

//	ParticleSlots = new ParticleSlot3d[20]; if (ParticleSlots == NULL) { WriteErrorLog("Memory allocation error!"); exit(1); }
	ParticleEmitters = new ParticleEmitter3d[5000]; if (ParticleEmitters == NULL) { WriteErrorLog("Memory allocation error!"); exit(1); }
	Particles = new Particle3d[50000]; if (Particles == NULL) { WriteErrorLog("Memory allocation error!"); exit(1); }
//	Lights = new light3d[1000]; if (Lights == NULL) { WriteErrorLog("Memory allocation error!"); exit(1); }


	TempVertices = new vert3d[50000]; if (TempVertices == NULL) { WriteErrorLog("Memory allocation error!"); exit(1); }
	PFaces = new face3d[15000]; if (PFaces == NULL) { WriteErrorLog("Memory allocation error!"); exit(1); }
	zOrder = new float[50000]; if (zOrder == NULL) { WriteErrorLog("Memory allocation error!"); exit(1); }
	drawOrder = new int[15000]; if (drawOrder == NULL) { WriteErrorLog("Memory allocation error!"); exit(1); }
	Vertices = new vert3d[50000]; if (Vertices == NULL) { WriteErrorLog("Memory allocation error!"); exit(1); }
	VisVertices = new vert3d[50000]; if (VisVertices == NULL) { WriteErrorLog("Memory allocation error!"); exit(1); }
    xfVerts = new TlVertex3D[50000]; if (xfVerts == NULL) { WriteErrorLog("Memory allocation error!"); exit(1); }
    prjVerts = new TlVertex3D[50000]; if (prjVerts == NULL) { WriteErrorLog("Memory allocation error!"); exit(1); }
    originalVerts = new TlVertex3D[50000]; if (originalVerts == NULL) { WriteErrorLog("Memory allocation error!"); exit(1); }
	Faces = new face3d[15000]; if (Faces == NULL) { WriteErrorLog("Memory allocation error!"); exit(1); }
	Trees = new tree3d[10000]; if (Trees == NULL) { WriteErrorLog("Memory allocation error!"); exit(1); }
//	Noise = new noise3d[100]; if (Noise == NULL) { WriteErrorLog("Memory allocation error!"); exit(1); }
//	samples = new FSOUND_SAMPLE[60]; if (samples == NULL) { WriteErrorLog("Memory allocation error!"); exit(1); }


	randomize(2);


if (CompatibilityMode)
{
	GLFog=FXFALSE;
	GLTextures=FXFALSE;
	use_lighing=FXFALSE;
	NoTexCalcs=FXTRUE;
	WireframeMode=FXTRUE;
}



//	SetIntelPrecision();

	initTrigs();		//Fill the sin[] and cos[] arrays

	WriteErrorLog("X) Starting OpenGL");

	initglide();		//Start gfx

	TimerInit();

	WriteErrorLog("X) Loading Textures");

	if (GLTextures)
	LoadAllTextures();	//Load the textures into OpenGL

	CurrentMenu=0;

#ifdef USE_GLIDE
    wWidth = (float)grSstScreenWidth();
    wHeight = (float)grSstScreenHeight();
#else
	wWidth = winWidth;
	wHeight = winHeight;

#endif
    
	
   	if (xxx++ == 10)
	{
		fi = (float)clock();
		fps =  (float)(fi - st);
		st = fi;
		xxx=0;
	}




#define MAX_DIST 5.0f
#define MIN_DIST -5.0f
ControlYaw=0.0f;
ControlPitch=0.0f;
ControlRoll=0.0f;




//============================================================================
// load Meshes
//============================================================================

WriteErrorLog("X) Loading Models");

glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
glClearColor(0.0, 0.0, 0.0, 1.0);
sprintf(tempstring, "\\c(255 255 255)Loading Models");
mytext.Size(10);
mytext.Draw(20, 20, tempstring);
glFlush();
SwapBuffers( DR_HDC ); 


NumObjects = 0;
numPlayers = 0;
localplayer = 0;

if (LoadAscFile("meshes\\NEWSHP1.ASC", OBJECTMESH_SHIP, 0.000025f*scalefactor, 270, 0, 0) == 0) //SHIP
	frames = 0;
if (LoadAscFile("meshes\\LASER.ASC", OBJECTMESH_LASER, 0.000012f*scalefactor, 270, 0, 0) == 0) //LASER BOLT
	frames = 0;
if (LoadAscFile("meshes\\MISSILE.ASC", OBJECTMESH_MISSILE, 0.000015f*scalefactor, 90, 0, 0) == 0) //MISSILE
	frames = 0;
if (LoadAscFile("meshes\\COMMAND.ASC", OBJECTMESH_COMMANDCENTER, 0.00025f*scalefactor, 0, 0, 0) == 0) //COMMAND CENTER
	frames = 0;
if (LoadAscFile("meshes\\POWER.ASC", OBJECTMESH_POWERPLANT, 0.0001f*scalefactor, 0, 0, 0) == 0) //POWER PLANT
	frames = 0;
if (LoadAscFile("meshes\\MINE.ASC", OBJECTMESH_MINE, 0.00022f*scalefactor, 0, 0, 0) == 0) //MINE
	frames = 0;
if (LoadAscFile("meshes\\SAM.ASC", OBJECTMESH_SAM, 0.00013f*scalefactor, 0, 0, 0) == 0) //SAM-SITE
	frames = 0;
if (LoadAscFile("meshes\\AAA.ASC", OBJECTMESH_AAA, 0.00013f*scalefactor, 0, 0, 0) == 0) //AAA-SITE
	frames = 0;
if (LoadAscFile("meshes\\UPLINK.ASC", OBJECTMESH_UPLINK, 0.00045f*scalefactor, 0, 0, 0) == 0) //AAA-SITE
	frames = 0;
if (LoadAscFile("meshes\\LTANK.ASC", OBJECTMESH_LIGHTTANK, 0.0001f*scalefactor, 180, 0, 0) == 0) //LIGHT MISSILE TANK
	frames = 0;
if (LoadAscFile("meshes\\MENUCMD.ASC", OBJECTMESH_COMMANDCENTERMENU, 0.00025f*scalefactor, 0, 0, 0) == 0) //COMMAND CENTER USED FOR MENU
	frames = 0;
if (LoadAscFile("meshes\\Ship1.ASC", OBJECTMESH_SHIP1, 0.00015f*scalefactor, 0, 0, 0) == 0) //Small Ship
	frames = 0;
if (LoadAscFile("meshes\\Ship2.ASC", OBJECTMESH_SHIP2, 0.00015f*scalefactor, 0, 0, 0) == 0) //Medium Ship
	frames = 0;
if (LoadAscFile("meshes\\Ship3.ASC", OBJECTMESH_SHIP3, 0.00015f*scalefactor, 0, 0, 0) == 0) //Heavy Ship
	frames = 0;
if (LoadAscFile("meshes\\tank1-lod1.ASC", OBJECTMESH_TANK1_LOD1, 0.00025f*scalefactor, 0, 0, 0) == 0)
	frames = 0;
if (LoadAscFile("meshes\\tank1-lod2.ASC", OBJECTMESH_TANK1_LOD2, 0.00025f*scalefactor, 0, 0, 0) == 0)
	frames = 0;
if (LoadAscFile("meshes\\tank1-lod3.ASC", OBJECTMESH_TANK1_LOD3, 0.00025f*scalefactor, 0, 0, 0) == 0)
	frames = 0;
if (LoadAscFile("meshes\\tank2-lod1.ASC", OBJECTMESH_TANK2_LOD1, 0.00025f*scalefactor, 0, 0, 0) == 0)
	frames = 0;
if (LoadAscFile("meshes\\tank2-lod2.ASC", OBJECTMESH_TANK2_LOD2, 0.00025f*scalefactor, 0, 0, 0) == 0)
	frames = 0;
if (LoadAscFile("meshes\\tank2-lod3.ASC", OBJECTMESH_TANK2_LOD3, 0.00025f*scalefactor, 0, 0, 0) == 0)
	frames = 0;

if (LoadAscFile("meshes\\aaa1-lod1-part1.ASC", OBJECTMESH_AAA1_LOD1_PART1, 0.0010f*scalefactor, 0, 0, 0) == 0)
	frames = 0;
if (LoadAscFile("meshes\\aaa1-lod2-part1.ASC", OBJECTMESH_AAA1_LOD2_PART1, 0.0010f*scalefactor, 0, 0, 0) == 0)
	frames = 0;
if (LoadAscFile("meshes\\aaa1-lod3-part1.ASC", OBJECTMESH_AAA1_LOD3_PART1, 0.0010f*scalefactor, 0, 0, 0) == 0)
	frames = 0;

if (LoadAscFile("meshes\\aaa1-lod1-part2.ASC", OBJECTMESH_AAA1_LOD1_PART2, 0.0010f*scalefactor, 0, 0, 0) == 0)
	frames = 0;
if (LoadAscFile("meshes\\aaa1-lod2-part2.ASC", OBJECTMESH_AAA1_LOD2_PART2, 0.0010f*scalefactor, 0, 0, 0) == 0)
	frames = 0;
if (LoadAscFile("meshes\\aaa1-lod3-part2.ASC", OBJECTMESH_AAA1_LOD3_PART2, 0.0010f*scalefactor, 0, 0, 0) == 0)
	frames = 0;

if (LoadAscFile("meshes\\aaa1-lod1-part3.ASC", OBJECTMESH_AAA1_LOD1_PART3, 0.0010f*scalefactor, 0, 0, 0) == 0)
	frames = 0;
if (LoadAscFile("meshes\\aaa1-lod2-part3.ASC", OBJECTMESH_AAA1_LOD2_PART3, 0.0010f*scalefactor, 0, 0, 0) == 0)
	frames = 0;
if (LoadAscFile("meshes\\aaa1-lod3-part3.ASC", OBJECTMESH_AAA1_LOD3_PART3, 0.0010f*scalefactor, 0, 0, 0) == 0)
	frames = 0;

if (LoadAscFile("meshes\\sat1-lod1.ASC", OBJECTMESH_SAT1_LOD1, 0.0025f*scalefactor, 0, 0, 0) == 0)
	frames = 0;
if (LoadAscFile("meshes\\sat1-lod2.ASC", OBJECTMESH_SAT1_LOD2, 0.0025f*scalefactor, 0, 0, 0) == 0)
	frames = 0;
if (LoadAscFile("meshes\\sat2-lod1.ASC", OBJECTMESH_SAT2_LOD1, 0.0025f*scalefactor, 0, 0, 0) == 0)
	frames = 0;
if (LoadAscFile("meshes\\sat2-lod2.ASC", OBJECTMESH_SAT2_LOD2, 0.0025f*scalefactor, 0, 0, 0) == 0)
	frames = 0;


	//Nudge the AAA-Site's laser turrets to their correct position
	CenterMesh(OBJECTMESH_AAA1_LOD1_PART3, 0.0f, 1.2f, 0.0f);
	CenterMesh(OBJECTMESH_AAA1_LOD2_PART3, 0.0f, 1.2f, 0.0f);
	CenterMesh(OBJECTMESH_AAA1_LOD3_PART3, 0.0f, 1.2f, 0.0f);

	Meshes[OBJECTMESH_AAA].sizeX = Meshes[OBJECTMESH_AAA1_LOD1_PART1].sizeX;
	Meshes[OBJECTMESH_AAA].sizeY = Meshes[OBJECTMESH_AAA1_LOD1_PART1].sizeY;
	Meshes[OBJECTMESH_AAA].sizeZ = Meshes[OBJECTMESH_AAA1_LOD1_PART1].sizeZ;
	Meshes[OBJECTMESH_UPLINK].sizeX = Meshes[OBJECTMESH_SAT1_LOD1].sizeX;
	Meshes[OBJECTMESH_UPLINK].sizeY = Meshes[OBJECTMESH_SAT1_LOD1].sizeY;
	Meshes[OBJECTMESH_UPLINK].sizeZ = Meshes[OBJECTMESH_SAT1_LOD1].sizeZ;
	Meshes[OBJECTMESH_UPLINK2].sizeX = Meshes[OBJECTMESH_SAT2_LOD1].sizeX;
	Meshes[OBJECTMESH_UPLINK2].sizeY = Meshes[OBJECTMESH_SAT2_LOD1].sizeY;
	Meshes[OBJECTMESH_UPLINK2].sizeZ = Meshes[OBJECTMESH_SAT2_LOD1].sizeZ;

	Meshes[OBJECTMESH_LIGHTTANK].sizeX = Meshes[OBJECTMESH_TANK1_LOD1].sizeX;
	Meshes[OBJECTMESH_LIGHTTANK].sizeY = Meshes[OBJECTMESH_TANK1_LOD1].sizeY;
	Meshes[OBJECTMESH_LIGHTTANK].sizeZ = Meshes[OBJECTMESH_TANK1_LOD1].sizeZ;

	Meshes[OBJECTMESH_MEDIUMTANK].sizeX = Meshes[OBJECTMESH_TANK2_LOD1].sizeX;
	Meshes[OBJECTMESH_MEDIUMTANK].sizeY = Meshes[OBJECTMESH_TANK2_LOD1].sizeY;
	Meshes[OBJECTMESH_MEDIUMTANK].sizeZ = Meshes[OBJECTMESH_TANK2_LOD1].sizeZ;


WriteErrorLog("X) Loading Model Texture Data");

LoadTextureData("meshes\\NEWSHP1.TEX", OBJECTMESH_SHIP);
//LoadTextureData("meshes\\LASER.TEX", OBJECTMESH_LASER);	//Laser textures are hardcoded
LoadTextureData("meshes\\MISSILE.TEX", OBJECTMESH_MISSILE);
LoadTextureData("meshes\\COMMAND.TEX", OBJECTMESH_COMMANDCENTER);
LoadTextureData("meshes\\POWER.TEX", OBJECTMESH_POWERPLANT);
LoadTextureData("meshes\\MINE.TEX", OBJECTMESH_MINE);
LoadTextureData("meshes\\SAM.TEX", OBJECTMESH_SAM);
LoadTextureData("meshes\\AAA.TEX", OBJECTMESH_AAA);
LoadTextureData("meshes\\UPLINK.TEX", OBJECTMESH_UPLINK);
LoadTextureData("meshes\\LTANK.TEX", OBJECTMESH_LIGHTTANK);
LoadTextureData("meshes\\MENUCMD.TEX", OBJECTMESH_COMMANDCENTERMENU);
LoadTextureData("meshes\\SHIP1.TEX", OBJECTMESH_SHIP1);
LoadTextureData("meshes\\SHIP2.TEX", OBJECTMESH_SHIP2);
LoadTextureData("meshes\\SHIP3.TEX", OBJECTMESH_SHIP3);

LoadTextureData("meshes\\tank1-lod1.TEX", OBJECTMESH_TANK1_LOD1);
LoadTextureData("meshes\\tank1-lod2.TEX", OBJECTMESH_TANK1_LOD2);
LoadTextureData("meshes\\tank1-lod3.TEX", OBJECTMESH_TANK1_LOD3);
LoadTextureData("meshes\\tank2-lod1.TEX", OBJECTMESH_TANK2_LOD1);
LoadTextureData("meshes\\tank2-lod2.TEX", OBJECTMESH_TANK2_LOD2);
LoadTextureData("meshes\\tank2-lod3.TEX", OBJECTMESH_TANK2_LOD3);
LoadTextureData("meshes\\aaa1-lod1-part1.TEX", OBJECTMESH_AAA1_LOD1_PART1);
LoadTextureData("meshes\\aaa1-lod2-part1.TEX", OBJECTMESH_AAA1_LOD2_PART1);
LoadTextureData("meshes\\aaa1-lod3-part1.TEX", OBJECTMESH_AAA1_LOD3_PART1);
LoadTextureData("meshes\\aaa1-lod1-part2.TEX", OBJECTMESH_AAA1_LOD1_PART2);
LoadTextureData("meshes\\aaa1-lod2-part2.TEX", OBJECTMESH_AAA1_LOD2_PART2);
LoadTextureData("meshes\\aaa1-lod3-part2.TEX", OBJECTMESH_AAA1_LOD3_PART2);
LoadTextureData("meshes\\aaa1-lod1-part3.TEX", OBJECTMESH_AAA1_LOD1_PART3);
LoadTextureData("meshes\\aaa1-lod2-part3.TEX", OBJECTMESH_AAA1_LOD2_PART3);
LoadTextureData("meshes\\aaa1-lod3-part3.TEX", OBJECTMESH_AAA1_LOD3_PART3);

LoadTextureData("meshes\\sat1-lod1.TEX", OBJECTMESH_SAT1_LOD1);
LoadTextureData("meshes\\sat1-lod2.TEX", OBJECTMESH_SAT1_LOD2);
LoadTextureData("meshes\\sat2-lod1.TEX", OBJECTMESH_SAT2_LOD1);
LoadTextureData("meshes\\sat2-lod2.TEX", OBJECTMESH_SAT2_LOD2);



WriteErrorLog("X) Loading Particle Slots");

LoadSlotPosition(OBJECTMESH_SHIP1);
LoadSlotPosition(OBJECTMESH_SHIP2);
LoadSlotPosition(OBJECTMESH_SHIP3);
LoadSlotPosition(OBJECTMESH_COMMANDCENTER);
LoadSlotPosition(OBJECTMESH_POWERPLANT);
LoadSlotPosition(OBJECTMESH_MISSILE);
LoadSlotPosition(OBJECTMESH_MINE);
LoadSlotPosition(OBJECTMESH_UPLINK);
LoadSlotPosition(OBJECTMESH_UPLINK2);
LoadSlotPosition(OBJECTMESH_LIGHTTANK);


WriteErrorLog("X) Loading Menu Position Data");

LoadMenuPositions();

//DumpUsedTextures();  //Reports unused textures in the file textures.log


current_weather.WeatherType=WEATHERTYPE_SUNNY;
current_weather.WeatherPrecipitation = WEATHERPREC_NONE;
current_weather.PrecipitationStrength = 0.0f;
current_weather.ThunderAndLightning = 0;

//============================================================================
// Landscape generation
//============================================================================

WriteErrorLog("X) Generating Sky");

glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
glClearColor(0.0, 0.0, 0.0, 1.0);
sprintf(tempstring, "\\c(255 255 255)Generating Sky");
mytext.Size(10);
mytext.Draw(20, 20, tempstring);
glFlush();
SwapBuffers( DR_HDC ); 

NewFractalSky1();		//sky texture 1
NewFractalSky2();		//sky texture 2

calcSky1();				//convert into OpenGL texture
calcSky2();				//ditto

WriteErrorLog("X) Creating Fractal Landscape");

RandomSeed = (int)(rand()%65534);

NewFractal();			//Fractal landscape heightmap
TerrainType = TERRAIN_LUSH;

WriteErrorLog("X) Building Landscape Mesh");

CreateLandscape(TerrainType);		//Convert into landscape mesh

numverts = numLSverts;
numfaces = numLSfaces;




//============================================================================
// Sound startup
//============================================================================
	WriteErrorLog("X) Starting Sound");
	InitPlaylist();	//Fill the playlist
	DRStartSound(hWnd);

	WriteErrorLog("X) Sound Startup complete");

	RemoveAllObjects();



editObject = 0;
//Texture Editor
//	if (TextureEditor)
//		AddObject( OBJECTTYPE_SHIP, OBJECTMESH_TANK1_LOD1, 0, -1, FXFALSE, FXFALSE, FXTRUE, FXFALSE, FXFALSE, 0.0f, 0.0f, -500.0f, 0.0f, 0.0f, 0.0f, 0, FXFALSE);


//Enable this for the Slot editor,
/*	editSlotMesh=OBJECTMESH_LIGHTTANK;
	AddObject( OBJECTTYPE_BUILDING, editSlotMesh, 0, -1, FXFALSE, FXFALSE, FXTRUE, FXFALSE, FXFALSE, 0.0f, 0.0f, -500.0f, 0.0f, 0.0f, 0.0f, 0, FXFALSE);
	editSlot=0;
*/
//Enable this for the Slot editor, if editing a new object
//	ParticleSlots[editSlotMesh].numslots=1;





if (!DemoMode)
{
	setupTeam(0);
	setupTeam(1);
	setupPlayer(FXFALSE, FXTRUE, FXFALSE, FXFALSE, 0, OBJECTMESH_SHIP1); //The human player
}
else
{
	setupTeam(0);	//setup team 0 and 1 for demo mode
	setupTeam(1);
}




if (DemoMode)
{
	setupPresetBase(0);
	setupPresetBase(1);

	setupPlayer(FXTRUE, FXFALSE, FXFALSE, FXFALSE, 0, OBJECTMESH_SHIP1);
	setupPlayer(FXTRUE, FXFALSE, FXFALSE, FXFALSE, 0, OBJECTMESH_SHIP2);

	setupPlayer(FXTRUE, FXFALSE, FXFALSE, FXFALSE, 1, OBJECTMESH_SHIP1);
	setupPlayer(FXTRUE, FXFALSE, FXFALSE, FXFALSE, 1, OBJECTMESH_SHIP2);

}



daytime = 4000.0f;		//Start somewhere in the morning
SunHandler(FXTRUE);		//Initial Sun Stuff

MenuActive = FXTRUE;

#ifdef USE_GL
	calcMap();
#endif


//Enable this for the Slot editor, if editing a new object
/*tempint=AddParticleEmitter(	0,
							1, 2, 1, -1, 0.0f, 10.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 100.0f, 100.0f, 0.0f,
							255, 255, 255, 30);
ParticleEmitters[tempint].Slot=0;
ParticleEmitters[tempint].ThreeD=FXTRUE;
*/
	
//============================================================================
// Begin MAIN GAME LOOP
//============================================================================

WriteErrorLog("X) LOADING COMPLETE. Entering game loop");


while( (frames-- && tlOkToRender()) && (RestartProgram==0)) {

		if (frames<-10)
			frames=-10;

	startFrameTime = TimerGetTime();

	respawnAllPlayers();

	numverts = numLSverts;
	numfaces = numLSfaces;


	CalcFrameRate();


	if (Player[localplayer].ControlledObject!=-1)
	{
		Objects[Player[localplayer].ControlledObject].Yaw = -ControlYaw+180;
		Objects[Player[localplayer].ControlledObject].Pitch = ControlPitch;
		Objects[Player[localplayer].ControlledObject].Roll = ControlRoll;
		Objects[Player[localplayer].ControlledObject].xPos = ControlXPos;
		Objects[Player[localplayer].ControlledObject].zPos = ControlYPos;
		Objects[Player[localplayer].ControlledObject].Height = ControlHeight;
		Objects[Player[localplayer].ControlledObject].Speed = mySpeed;

		if( (Objects[Player[localplayer].ControlledObject].ObjectMesh == OBJECTMESH_LIGHTTANK) ||
			(Objects[Player[localplayer].ControlledObject].ObjectMesh == OBJECTMESH_MEDIUMTANK) )
		{
			Objects[Player[localplayer].ControlledObject].ymovement=0;
			Objects[Player[localplayer].ControlledObject].Height = -TerrainHeight(&Objects[Player[localplayer].ControlledObject].xPos, &Objects[Player[localplayer].ControlledObject].Height, &Objects[Player[localplayer].ControlledObject].zPos) - (Objects[Player[localplayer].ControlledObject].sizeY)-0.1f;
			GroundVehicleOrientation(Player[localplayer].ControlledObject);
			ControlPitch = Objects[Player[localplayer].ControlledObject].Pitch;
			ControlRoll = Objects[Player[localplayer].ControlledObject].Roll;
			ControlHeight = Objects[Player[localplayer].ControlledObject].Height;
		}

	}


//============================================================================
// various handlers
//============================================================================


if (!TextureEditor)
{
	if (!MasterGamePause)
	{
		SatelliteHandler();			//Handler for Mass-Destruction Weapon
		UpdateTeamStats();			//Calculate energy and resources
		LaserHandler();				//Laser specific stuff
		GuidedMissileHandler();		//Guided warhead
		MissileHandler();			//Missile specific stuff (homing, etc)
		AIPoll();					//AI specífic stuff
		AAAHandler();				//AAA handler
		SAMHandler();				//SAM handler
		ObjectAnimationHandler();	//Takeoff, landing and death animations
		ShipHandler();				//Ship death, respawn, etc.
		BuildingHandler();			//Building production, death, etc.
		LimitParticles();			//Limit amount of Particles by leaving only one emitter active per frame
		MissileWarningHandler();	//Sounds the missile warning klaxon
		ShieldHandler();			//Shield regeneration
		LockHandler();				//Lock notification
		MusicHandler(FXFALSE);
	}
}
else
{
	LimitParticles();			//Limit amount of Particles by leaving only one emitter active per frame
	MusicHandler(FXFALSE);
	if (!MasterGamePause)
	{
		UpdateTeamStats();			//Calculate energy and resources
		BuildingHandler();			//Building production, death, etc.
		ShieldHandler();			//Shield regeneration
	}
}


	ThunderHandler();
	SunHandler(FXFALSE);		//Sun Position, Sun Lighting, Fog color
	LightHandler();				//OpenGL dynamic light handling




if (myServer.isActive)
{
	Network_ServerHandler();
}
else if (myServer.isClient)
{
	Network_ClientHandler();
}



    //Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(fogColor[0]*1.5f, fogColor[1]*1.5f, fogColor[2]*1.5f, 1.0);

	if (!CompatibilityMode)
		RenderParticles(FXTRUE);		//Render the Sun and the moon(s)



//============================================================================
// Object movement, translation and rotation, and insertion into render pipe
//============================================================================



/*	if (DemoMode)
	{
		//If we're not looking at a valid object anymore, look for a new one
		if ((Objects[LookAtObject].ObjectId != LookAtObject) || (timeGetTime() - LastLookAtChange > 10000))
			SearchForCoolLookAt();
		if (timeGetTime() - LastModeChange > 5000)
		{
			LastModeChange=timeGetTime();
			CameraMode = random(3) + 1;
		}
	}
	else
		LookAtObject = Player[localplayer].ControlledObject;
*/

	if (Objects[LookAtObject].ObjectId != LookAtObject)
		SearchForCoolLookAt(-1);


	if (CameraMode==0) //set cameraMode=1 for external view
	{
		CamXPos = ControlXPos;
		CamZPos = ControlYPos;
		CamHeight = ControlHeight;
		CamYaw = ControlYaw;
		CamPitch = ControlPitch;
		CamRoll = ControlRoll;
		if 	(CamHeight > -TerrainHeight(&CamXPos, &CamHeight, &CamZPos)-0.2f)
			CamHeight = -TerrainHeight(&CamXPos, &CamHeight, &CamZPos)-0.2f;
	}
	else if (CameraMode==1)
	{
		CamXPos = Objects[LookAtObject].xPos-5.0f;
		CamHeight = Objects[LookAtObject].Height-8.0f;
		CamZPos = Objects[LookAtObject].zPos-5.0f;
		if 	(CamHeight > -TerrainHeight(&CamXPos, &CamHeight, &CamZPos)-0.2f)
			CamHeight = -TerrainHeight(&CamXPos, &CamHeight, &CamZPos)-0.2f;
		GetRelativeAngle(	Objects[LookAtObject].xPos, Objects[LookAtObject].zPos, Objects[LookAtObject].Height,
							CamXPos, CamZPos, CamHeight );
		CamYaw = -(TempYaw / (2*pi))*360;
		CamPitch = (TempPitch / (2*pi))*360;
		CamYaw = FixAngle(CamYaw);
		CamPitch = FixAngle(CamPitch);
	}
	else if (CameraMode==2)
	{
		CamXPos = Objects[LookAtObject].xPos-10.0f;
		CamHeight = Objects[LookAtObject].Height-20.0f;
		CamZPos = Objects[LookAtObject].zPos+10.0f;
		if 	(CamHeight > -TerrainHeight(&CamXPos, &CamHeight, &CamZPos)-0.2f)
			CamHeight = -TerrainHeight(&CamXPos, &CamHeight, &CamZPos)-0.2f;
		GetRelativeAngle(	Objects[LookAtObject].xPos, Objects[LookAtObject].zPos, Objects[LookAtObject].Height,
							CamXPos, CamZPos, CamHeight );
		CamYaw = -(TempYaw / (2*pi))*360;
		CamPitch = (TempPitch / (2*pi))*360;
		CamYaw = FixAngle(CamYaw);
		CamPitch = FixAngle(CamPitch);
	}
	else if (CameraMode==3)
	{
		CamXPos = Objects[LookAtObject].xPos+10.0f;
		CamHeight = Objects[LookAtObject].Height-2.0f;
		CamZPos = Objects[LookAtObject].zPos+10.0f;
		if 	(CamHeight > -TerrainHeight(&CamXPos, &CamHeight, &CamZPos)-0.2f)
			CamHeight = -TerrainHeight(&CamXPos, &CamHeight, &CamZPos)-0.2f;
		GetRelativeAngle(	Objects[LookAtObject].xPos, Objects[LookAtObject].zPos, Objects[LookAtObject].Height,
							CamXPos, CamZPos, CamHeight );
		CamYaw = -(TempYaw / (2*pi))*360;
		CamPitch = (TempPitch / (2*pi))*360;
		CamYaw = FixAngle(CamYaw);
		CamPitch = FixAngle(CamPitch);
	}
	else if (CameraMode==4)
	{
		CamXPos = Objects[LookAtObject].xPos+20.0f;
		CamHeight = Objects[LookAtObject].Height-10.0f;
		CamZPos = Objects[LookAtObject].zPos-30.0f;
		if 	(CamHeight > -TerrainHeight(&CamXPos, &CamHeight, &CamZPos)-0.2f)
			CamHeight = -TerrainHeight(&CamXPos, &CamHeight, &CamZPos)-0.2f;
		GetRelativeAngle(	Objects[LookAtObject].xPos, Objects[LookAtObject].zPos, Objects[LookAtObject].Height,
							CamXPos, CamZPos, CamHeight );
		CamYaw = -(TempYaw / (2*pi))*360;
		CamPitch = (TempPitch / (2*pi))*360;
		CamYaw = FixAngle(CamYaw);
		CamPitch = FixAngle(CamPitch);
	}
	else if (CameraMode==5)
	{
		if 	(CamHeight > -TerrainHeight(&CamXPos, &CamHeight, &CamZPos)-0.2f)
			CamHeight = -TerrainHeight(&CamXPos, &CamHeight, &CamZPos)-0.2f;
		GetRelativeAngle(	Objects[LookAtObject].xPos, Objects[LookAtObject].zPos, Objects[LookAtObject].Height,
							CamXPos, CamZPos, CamHeight );
		CamYaw = -(TempYaw / (2*pi))*360;
		CamPitch = (TempPitch / (2*pi))*360;
		CamYaw = FixAngle(CamYaw);
		CamPitch = FixAngle(CamPitch);
	}




		if (NumObjects>0)
		for (i=NumObjects-1;i>=0;i--)
//		for (i=0;i<NumObjects;i++)	//Use above line if not in TextureEditor
		{
		  if (Objects[i].ObjectId != -1)
		  {	  

			if ((Objects[i].ObjectMesh==OBJECTMESH_COMMANDCENTER) || 
				(Objects[i].ObjectMesh==OBJECTMESH_COMMANDCENTERMENU))
			{
				if (MenuActive)
				{
					Objects[i].ObjectMesh=OBJECTMESH_COMMANDCENTERMENU;
				}
				else
					Objects[i].ObjectMesh=OBJECTMESH_COMMANDCENTER;
			}

		if (Objects[i].isDocked)
			Objects[i].isVisible=FXFALSE;
		else
			Objects[i].isVisible=FXTRUE;

		if (i==Player[localplayer].ControlledObject)
		if (Player[localplayer].ControlledObject!=-1)
		if (CameraMode==0) //set cameraMode=1 for external view
			Objects[Player[localplayer].ControlledObject].isVisible=FXFALSE;
		else
			Objects[Player[localplayer].ControlledObject].isVisible=FXTRUE;


		//Move Objects according to their speed and heading
		if ((Objects[i].ObjectType!=OBJECTTYPE_BUILDING) && //except buildings,
			(Objects[i].ObjectType!=OBJECTTYPE_DOLLY) && //..sky,
			(Player[localplayer].ControlledObject!=i) && // and human ships.
			(!MasterGamePause)) // oh, and also not if we're paused ;)
		MoveObject(i);

		Objects[i].Pitch = FixAngle(Objects[i].Pitch);
		Objects[i].Yaw = FixAngle(Objects[i].Yaw);
		Objects[i].Roll = FixAngle(Objects[i].Roll);
		
		if (Objects[i].xPos > -CamXPos)
			vxdist = (Objects[i].xPos - CamXPos);
		else
			vxdist = (-CamXPos + Objects[i].xPos);

		if (Objects[i].Height > -CamHeight)
			vydist = (Objects[i].Height - CamHeight);
		else
			vydist = (-CamHeight + Objects[i].Height);
           
		if (Objects[i].zPos > -CamZPos)
			vzdist = (Objects[i].zPos - CamZPos);
		else
			vzdist = (-CamZPos + Objects[i].zPos);

		if (vxdist<0)
			vxdist=(-1.0f)*vxdist;
		if (vydist<0)
			vydist=(-1.0f)*vydist;
		if (vzdist<0)
			vzdist=(-1.0f)*vzdist;

			
			if ( (Objects[i].isVisible) && ((vxdist<maxvdist*2) && (vydist<maxvdist*2) && (vzdist<maxvdist*2)) )
				{

			tlSetMatrix( tlIdentity() );
			
			//Check if we are a normal object
			if ((Objects[i].ParentObject==-1) && 
				(Objects[i].ChildObject0==-1) && 
				(Objects[i].ChildObject1==-1) &&
				(Objects[i].ChildObject2==-1))
			{
				tlMultMatrix( tlXRotation( Objects[i].Pitch ) );
				tlMultMatrix( tlZRotation( Objects[i].Roll ) );
				tlMultMatrix( tlYRotation( Objects[i].Yaw ) );
				tlMultMatrix( tlTranslation( -Objects[i].xPos, -Objects[i].Height, -Objects[i].zPos ) );
			}
			//Else, we're a child of some other object, so inherit it's angles
			else if (Objects[i].ParentObject!=-1)
			{
				if (Objects[Objects[i].ParentObject].ChildObject0==i)
				{
					tlMultMatrix( tlYRotation( Objects[Objects[i].ParentObject].Yaw ) );
				}
				else if (Objects[Objects[i].ParentObject].ChildObject1==i)
				{
					tlMultMatrix( tlXRotation( -Objects[Objects[i].ParentObject].Pitch ) );
					tlMultMatrix( tlYRotation( Objects[Objects[i].ParentObject].Yaw ) );
				}
				else if (Objects[Objects[i].ParentObject].ChildObject2==i)
				{
					tlMultMatrix( tlXRotation( -Objects[Objects[i].ParentObject].Pitch ) );
				}
				tlMultMatrix( tlTranslation( -Objects[i].xPos, -Objects[i].Height, -Objects[i].zPos ) );
			}
			//Else we are the parent, so don't mess with the angles!
			else if ((Objects[i].ChildObject0!=-1) || 
					(Objects[i].ChildObject1!=-1) ||
					(Objects[i].ChildObject2!=-1))
			{
				tlMultMatrix( tlTranslation( -Objects[i].xPos, -Objects[i].Height, -Objects[i].zPos ) );
			}

			//Switch to Low-LOD models if farther away
//			Objects[i].ObjectSaveMesh=Objects[i].ObjectMesh;
			if ((vxdist>500.0f) || (vydist>500.0f) || (vzdist>500.0f))
			{
				if ((Objects[i].ObjectMesh==OBJECTMESH_SHIP1) ||
					(Objects[i].ObjectMesh==OBJECTMESH_SHIP2) ||
					(Objects[i].ObjectMesh==OBJECTMESH_SHIP3))
				   Objects[i].ObjectMesh=OBJECTMESH_SHIP;
				if (Objects[i].ObjectMesh==OBJECTMESH_AAA)
					Objects[i].ObjectMesh=OBJECTMESH_AAA1_LOD1_PART1;
				
				if (Objects[i].ObjectMesh==OBJECTMESH_UPLINK)
					Objects[i].ObjectMesh=OBJECTMESH_SAT1_LOD1;
				if (Objects[i].ObjectMesh==OBJECTMESH_UPLINK2)
					Objects[i].ObjectMesh=OBJECTMESH_SAT2_LOD1;

				if (Objects[i].ObjectMesh==OBJECTMESH_LIGHTTANK)
					Objects[i].ObjectMesh=OBJECTMESH_TANK1_LOD3;
				if (Objects[i].ObjectMesh==OBJECTMESH_MEDIUMTANK)
					Objects[i].ObjectMesh=OBJECTMESH_TANK2_LOD3;

			} else if ((vxdist>250.0f) || (vydist>250.0f) || (vzdist>250.0f))
			{

				if (Objects[i].ObjectMesh==OBJECTMESH_AAA)
					Objects[i].ObjectMesh=OBJECTMESH_AAA1_LOD2_PART1;
				if (Objects[i].ObjectMesh==OBJECTMESH_AAA1_LOD1_PART2)
					Objects[i].ObjectMesh=OBJECTMESH_AAA1_LOD2_PART2;
				if (Objects[i].ObjectMesh==OBJECTMESH_AAA1_LOD1_PART3)
					Objects[i].ObjectMesh=OBJECTMESH_AAA1_LOD2_PART3;

				if (Objects[i].ObjectMesh==OBJECTMESH_UPLINK)
					Objects[i].ObjectMesh=OBJECTMESH_SAT1_LOD1;
				if (Objects[i].ObjectMesh==OBJECTMESH_UPLINK2)
					Objects[i].ObjectMesh=OBJECTMESH_SAT2_LOD1;

				if (Objects[i].ObjectMesh==OBJECTMESH_LIGHTTANK)
					Objects[i].ObjectMesh=OBJECTMESH_TANK1_LOD2;
				if (Objects[i].ObjectMesh==OBJECTMESH_MEDIUMTANK)
					Objects[i].ObjectMesh=OBJECTMESH_TANK2_LOD2;

			} else
			{

				if (Objects[i].ObjectMesh==OBJECTMESH_AAA)
					Objects[i].ObjectMesh=OBJECTMESH_AAA1_LOD3_PART1;
				if (Objects[i].ObjectMesh==OBJECTMESH_AAA1_LOD1_PART2)
					Objects[i].ObjectMesh=OBJECTMESH_AAA1_LOD3_PART2;
				if (Objects[i].ObjectMesh==OBJECTMESH_AAA1_LOD1_PART3)
					Objects[i].ObjectMesh=OBJECTMESH_AAA1_LOD3_PART3;

				if (Objects[i].ObjectMesh==OBJECTMESH_UPLINK)
					Objects[i].ObjectMesh=OBJECTMESH_SAT1_LOD2;
				if (Objects[i].ObjectMesh==OBJECTMESH_UPLINK2)
					Objects[i].ObjectMesh=OBJECTMESH_SAT2_LOD2;

				if (Objects[i].ObjectMesh==OBJECTMESH_LIGHTTANK)
					Objects[i].ObjectMesh=OBJECTMESH_TANK1_LOD1;
				if (Objects[i].ObjectMesh==OBJECTMESH_MEDIUMTANK)
					Objects[i].ObjectMesh=OBJECTMESH_TANK2_LOD1;

			}








			//copy object faces into render pipeline, right after the landscape
			for (curface=numfaces; curface<=(numfaces+Meshes[Objects[i].ObjectMesh].numfaces);curface++)
			{
				Faces[curface].OwnerObject = i;
				Faces[curface] = Meshes[Objects[i].ObjectMesh].Faces[curface-numfaces];
				Faces[curface].srcVerts[0] = Meshes[Objects[i].ObjectMesh].Faces[curface-numfaces].srcVerts[0]+numverts;
				Faces[curface].srcVerts[1] = Meshes[Objects[i].ObjectMesh].Faces[curface-numfaces].srcVerts[1]+numverts;
				Faces[curface].srcVerts[2] = Meshes[Objects[i].ObjectMesh].Faces[curface-numfaces].srcVerts[2]+numverts;
				if ((TextureEditor)&&(i==0))  //If we're in texture edit mode, blink the current texture
				{
					// TEXTURE EDITOR FIXME: Expects faces of edit object starting at 0!!!
					if (ActiveFace==(curface-numfaces))
					{
						TextureBlink++;
						if (TextureBlink>5)
						{
							Faces[curface].Texture = 1;
						}
						if (TextureBlink>=10)
						{
							Faces[curface].Texture = Meshes[Objects[i].ObjectMesh].Faces[ActiveFace].Texture;
							TextureBlink=0;
						}
					}
				}
			}				

			//now rotate and translate the object vertices in a temp. array			
			for (o=0; o<Meshes[Objects[i].ObjectMesh].numverts; o++) {
				originalVerts[o] = Meshes[Objects[i].ObjectMesh].Vertices[o].v;
			}
			tlTransformVertices( xfVerts, originalVerts, Meshes[Objects[i].ObjectMesh].numverts );

			//then copy the vertices into the render pipeline, also after the landscape
			for (curvert=numverts; curvert<(numverts+Meshes[Objects[i].ObjectMesh].numverts);curvert++)
			{
				Vertices[curvert].v = xfVerts[curvert-numverts];
				Vertices[curvert].normals = Meshes[Objects[i].ObjectMesh].Vertices[curvert-numverts].normals;
				Vertices[curvert].OwnerObject = i;
			}				

			//Now set up rotation matrix for the vertex normals
			tlSetMatrix( tlIdentity() );
			tlMultMatrix( tlXRotation( Objects[i].Pitch ) );
			tlMultMatrix( tlZRotation( Objects[i].Roll ) );
			tlMultMatrix( tlYRotation( Objects[i].Yaw ) );
			//Copy all normals into a temp array
			for (o=0; o<Meshes[Objects[i].ObjectMesh].numverts; o++) {
				originalVerts[o] = Meshes[Objects[i].ObjectMesh].Vertices[o].normals;
			}
			//rotate them
			tlTransformVertices( xfVerts, originalVerts, Meshes[Objects[i].ObjectMesh].numverts );
			//Then copy normals into the render pipeline
			for (curvert=numverts; curvert<(numverts+Meshes[Objects[i].ObjectMesh].numverts);curvert++)
				Vertices[curvert].normals = xfVerts[curvert-numverts];
			

			numverts+=Meshes[Objects[i].ObjectMesh].numverts;
			numfaces+=Meshes[Objects[i].ObjectMesh].numfaces;

			//Restore from LOD mesh to original mesh
//			Objects[i].ObjectMesh=Objects[i].ObjectSaveMesh;

				}
		}
	  }


	  if (NumObjects>0)
		for (i=0;i<NumObjects;i++)
		  if (Objects[i].ObjectId != -1)
			Objects[i].ObjectMesh=Objects[i].ObjectSaveMesh;	//Restore from LOD mesh to original mesh






// Setup up rendering perspective
		
		xrot=CamPitch;
		yrot=CamYaw;
		zrot=CamRoll;
		xdist=CamXPos;
		ydist=CamZPos;
		height=CamHeight;


//============================================================================
// For OpenGL, we just don't care about far-away surfaces.
//============================================================================
#ifdef USE_GL
	visface=0;
	for (curface=numLSfaces; curface<numfaces; curface++){
  
		Faces[curface].IsVisible=1;

		if (Vertices[Faces[curface].srcVerts[0]].v.x > CamXPos)
			vxdist = (Vertices[Faces[curface].srcVerts[0]].v.x + CamXPos);
		else
			vxdist = (CamXPos + Vertices[Faces[curface].srcVerts[0]].v.x);

		if (Vertices[Faces[curface].srcVerts[0]].v.y > (+CamHeight))
			vydist = (Vertices[Faces[curface].srcVerts[0]].v.y + CamHeight);
		else
			vydist = (CamHeight + Vertices[Faces[curface].srcVerts[0]].v.y);
           
		if (Vertices[Faces[curface].srcVerts[0]].v.z > CamZPos)
			vzdist = (Vertices[Faces[curface].srcVerts[0]].v.z + CamZPos);
		else
			vzdist = (CamZPos + Vertices[Faces[curface].srcVerts[0]].v.z);

		if (vxdist<0)
			vxdist=(-1.0f)*vxdist;
		if (vydist<0)
			vydist=(-1.0f)*vydist;
		if (vzdist<0)
			vzdist=(-1.0f)*vzdist;

		if ((vxdist<maxvdist) && (vydist<maxvdist) && (vzdist<maxvdist))
		{ 
			drawOrder[visface]=curface;
			visface++;
			Faces[curface].IsVisible=1;
		}
		else
		{
			Faces[curface].IsVisible=0;
		} 
	}
#endif



	if (CamXPos>0)
		xGridPos = (int)(CamXPos/scalefactor);
	else
		xGridPos = (int)(CamXPos/scalefactor)-1;

	if (CamZPos>0)
		zGridPos = (int)(CamZPos/scalefactor)+1;
	else
		zGridPos = (int)(CamZPos/scalefactor);


		
//============================================================================
// Main OpenGL rendering loop
//============================================================================


	xrot=CamPitch;
	yrot=CamYaw;
	zrot=CamRoll;
	xdist=CamXPos;
	ydist=CamZPos;
	height=CamHeight;

	glDepthFunc(GL_LEQUAL);				// The Type Of Depth Testing To Do
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);	// Blending Function For Translucency Based On Source Alpha Value

	glEnable(GL_NORMALIZE);
/*	
	//---------------------------------------------------------------------
	//If this block of code is missing, everything goes whacko... And I dunno why!
	
	glDisable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat) winWidth/(GLfloat) winHeight, 1.0, 1000000000000000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glRotatef(-zrot,0.0f,0.0f,1.0f);
	glRotatef(-xrot,1.0f,0.0f,0.0f);
	glRotatef((yrot+180),0.0f,1.0f,0.0f);
	glTranslatef(xdist, height, ydist);
	if (GLTextures)
		glEnable(GL_TEXTURE_2D);
	glDisable(GL_FOG);
	glEnable(GL_NORMALIZE);
	//---------------------------------------------------------------------
*/



	//---------------------------------------------------------------------
	// Now render the world...
	
	glEnable(GL_DEPTH_TEST);

	if (GLFog)
		glEnable(GL_FOG);
	else
		glDisable(GL_FOG);

	glEnable(GL_LIGHTING);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
//	gluPerspective(60.0, (GLfloat) winWidth/(GLfloat) winHeight, 1.0, maxvdist);
	gluPerspective(60.0, (GLfloat) winWidth/(GLfloat) winHeight, 0.1, 1000000000000000.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glRotatef(-zrot,0.0f,0.0f,1.0f);
	glRotatef(-xrot,1.0f,0.0f,0.0f);
	glRotatef((yrot+180),0.0f,1.0f,0.0f);

	glPushMatrix();

	glTranslatef(xdist, height, ydist);


	if (GLTextures)
		glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);

	if (!CompatibilityMode)
	{
		RenderFloor();
		RenderSky();
	}
	glEnable(GL_DEPTH_TEST);

	reset_quad_tree();
	setup_quadtree(64, 64, 64);
	if (WireframeMode)
	{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		glColor3f(0.0, 255.0, 0.0);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
	}
	if (CompatibilityMode)
	{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
	}

	//This is the actual terrain drawing command
	//------------------------------------------
		draw(64, 64, 64, 0);
	//------------------------------------------
	if (WireframeMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if (GLTextures)
			glEnable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
	}



	if (!CompatibilityMode)
		RenderWater();

	//Now render objects
	for (curface=0; curface<visface; curface++)
	{
		curorder=drawOrder[curface];

		//Don't draw objects with sky textures here.
		if (Faces[curorder].OwnerObject!=-1)
		{



		Faces[curorder].Reflective = Faces[curorder].Transparent;
		if ((Faces[curorder].Reflective) && (!CompatibilityMode))
	 	{
/*			glEnable(GL_STENCIL_TEST);
			glEnable(GL_DEPTH_TEST);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

			//Draw the ID 2 into the stencil buffer.
			glStencilOp(GL_ZERO, GL_KEEP, GL_REPLACE);
			glStencilFunc(GL_ALWAYS, 2, 0xffffffff);

			glBegin(GL_TRIANGLES);
			glVertex3f( Vertices[Faces[curorder].srcVerts[0]].v.x , Vertices[Faces[curorder].srcVerts[0]].v.y , Vertices[Faces[curorder].srcVerts[0]].v.z );
			glVertex3f( Vertices[Faces[curorder].srcVerts[1]].v.x , Vertices[Faces[curorder].srcVerts[1]].v.y , Vertices[Faces[curorder].srcVerts[1]].v.z );
			glVertex3f( Vertices[Faces[curorder].srcVerts[2]].v.x , Vertices[Faces[curorder].srcVerts[2]].v.y , Vertices[Faces[curorder].srcVerts[2]].v.z );
			glEnd();

			glDisable(GL_DEPTH_TEST); 
			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

			//Now, only render where stencil is set to 1.
			glStencilFunc(GL_EQUAL, 2, 0xffffffff);  // draw if stencil == 2
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
*/
//			glTexGenf(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);
//			glTexGenf(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_NV);
			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
		
		}
			
		if ((GLTextures) && (!CompatibilityMode))
		{
			if ((unsigned)LastUsedTexture != GLTextureHandle[Faces[curorder].Texture])
			{
				glBindTexture(GL_TEXTURE_2D, GLTextureHandle[Faces[curorder].Texture]);
				LastUsedTexture = GLTextureHandle[Faces[curorder].Texture];
			}

			//If text is being displayed on the menu, swap the menu's screen texture
			if ((Faces[curorder].Texture==37) && (CurrentMenu==5) && (MenuActive))
				glBindTexture(GL_TEXTURE_2D, GLTextureHandle[Faces[curorder].Texture+3]);
			if ((Faces[curorder].Texture==38) && (CurrentMenu==4) && (MenuActive))
				glBindTexture(GL_TEXTURE_2D, GLTextureHandle[Faces[curorder].Texture+3]);
			if ((Faces[curorder].Texture==39) && (CurrentMenu==6) && (MenuActive))
				glBindTexture(GL_TEXTURE_2D, GLTextureHandle[Faces[curorder].Texture+3]);
		}

		if (Faces[curorder].isLight)
			glDisable(GL_LIGHTING);
		else
			glEnable(GL_LIGHTING);

		if ((Faces[curorder].Transparent) && (!CompatibilityMode))
		{
			glEnable(GL_BLEND);					// Switch on the darned blending
			glBlendFunc(GL_SRC_ALPHA,GL_ONE);	// Blending Function For Translucency Based On Source Alpha Value
		}




		glBegin(GL_TRIANGLES);

			if (Faces[curorder].TextureMode==1)
				glTexCoord2f(0.0f, 0.0f);
			else if (Faces[curorder].TextureMode==2)
				glTexCoord2f(0.0f, 1.0f);
			else if (Faces[curorder].TextureMode==3)
				glTexCoord2f(0.0f, 1.0f);
			else if (Faces[curorder].TextureMode==4)
				glTexCoord2f(1.0f, 1.0f);

		glColor4f(Vertices[Faces[curorder].srcVerts[0]].v.r,Vertices[Faces[curorder].srcVerts[0]].v.g,Vertices[Faces[curorder].srcVerts[0]].v.b,1.0f);	//Full Brightness
		if (Faces[curorder].isLight)
				glColor4f(1.0f,1.0f,1.0f,1.0f);	//Full Brightness
		if (Faces[curorder].Transparent)
				glColor4f(1.0f,1.0f,1.0f,0.5f);	//50% Alpha

			    glNormal3f( Vertices[Faces[curorder].srcVerts[0]].normals.x, 
							Vertices[Faces[curorder].srcVerts[0]].normals.y,
							Vertices[Faces[curorder].srcVerts[0]].normals.x);

				glVertex3f( Vertices[Faces[curorder].srcVerts[0]].v.x , 
							Vertices[Faces[curorder].srcVerts[0]].v.y , 
							Vertices[Faces[curorder].srcVerts[0]].v.z );


			if (Faces[curorder].TextureMode==1)
				glTexCoord2f(0.0f, 1.0f);
			else if (Faces[curorder].TextureMode==2)
				glTexCoord2f(1.0f, 1.0f);
			else if (Faces[curorder].TextureMode==3)
				glTexCoord2f(1.0f, 0.0f);
			else if (Faces[curorder].TextureMode==4)
				glTexCoord2f(1.0f, 0.0f);

		glColor4f(Vertices[Faces[curorder].srcVerts[1]].v.r,Vertices[Faces[curorder].srcVerts[1]].v.g,Vertices[Faces[curorder].srcVerts[1]].v.b,1.0f);	//Full Brightness
		if (Faces[curorder].isLight)
				glColor4f(1.0f,1.0f,1.0f,1.0f);	//Full Brightness
		if (Faces[curorder].Transparent)
				glColor4f(1.0f,1.0f,1.0f,0.5f);	//50% Alpha

			    glNormal3f( Vertices[Faces[curorder].srcVerts[1]].normals.x,
							Vertices[Faces[curorder].srcVerts[1]].normals.y, 
							Vertices[Faces[curorder].srcVerts[1]].normals.x);
				glVertex3f( Vertices[Faces[curorder].srcVerts[1]].v.x , 
							Vertices[Faces[curorder].srcVerts[1]].v.y , 
							Vertices[Faces[curorder].srcVerts[1]].v.z );



			if (Faces[curorder].TextureMode==1)
				glTexCoord2f(1.0f, 0.0f);
			else if (Faces[curorder].TextureMode==2)
				glTexCoord2f(1.0f, 0.0f);
			else if (Faces[curorder].TextureMode==3)
				glTexCoord2f(1.0f, 1.0f);
			else if (Faces[curorder].TextureMode==4)
				glTexCoord2f(0.0f, 1.0f);

		glColor4f(Vertices[Faces[curorder].srcVerts[2]].v.r,Vertices[Faces[curorder].srcVerts[2]].v.g,Vertices[Faces[curorder].srcVerts[2]].v.b,1.0f);	//Full Brightness
		if (Faces[curorder].isLight)
				glColor4f(1.0f,1.0f,1.0f,1.0f);	//Full Brightness
		if (Faces[curorder].Transparent)
				glColor4f(1.0f,1.0f,1.0f,0.5f);	//50% Alpha
				
			    glNormal3f( Vertices[Faces[curorder].srcVerts[2]].normals.x, 
							Vertices[Faces[curorder].srcVerts[2]].normals.y, 
							Vertices[Faces[curorder].srcVerts[2]].normals.x);
				glVertex3f( Vertices[Faces[curorder].srcVerts[2]].v.x , 
							Vertices[Faces[curorder].srcVerts[2]].v.y , 
							Vertices[Faces[curorder].srcVerts[2]].v.z );
		
		glEnd();		






		if (Faces[curorder].Transparent)
		{
			glDisable(GL_BLEND);
//			glDisable(GL_STENCIL_TEST);
//			glEnable(GL_DEPTH_TEST); 
		}

		if (Faces[curorder].Reflective)
		{
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T); 
		}


		if (GLTextures)
		    glEnable(GL_TEXTURE_2D);


		}

	}


	if (TextureEditor)
	{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glBegin(GL_LINE_LOOP);
		glVertex3f( Vertices[Faces[ActiveFace+numLSfaces].srcVerts[0]].v.x , 
					Vertices[Faces[ActiveFace+numLSfaces].srcVerts[0]].v.y , 
					Vertices[Faces[ActiveFace+numLSfaces].srcVerts[0]].v.z );
		glVertex3f( Vertices[Faces[ActiveFace+numLSfaces].srcVerts[1]].v.x , 
					Vertices[Faces[ActiveFace+numLSfaces].srcVerts[1]].v.y , 
					Vertices[Faces[ActiveFace+numLSfaces].srcVerts[1]].v.z );
		glVertex3f( Vertices[Faces[ActiveFace+numLSfaces].srcVerts[2]].v.x , 
					Vertices[Faces[ActiveFace+numLSfaces].srcVerts[2]].v.y , 
					Vertices[Faces[ActiveFace+numLSfaces].srcVerts[2]].v.z );
		glEnd();
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
	}



	if (!MasterGamePause)
	{
		ParticleEmitterHandler();	//Particle emitter stuff (Creating Particles, emitter movement, etc.)
		if (!CompatibilityMode)
			ParticleHandler();			//Stuff for the particles themselves (Particle lifetime/movement)
	}

	if (!CompatibilityMode)
	Render3dParticles();		//Render all 3d particles

	if (!CompatibilityMode)
	RenderTrees();				//Render the trees

	if (!CompatibilityMode)
	RenderRain();				//Render all the raindrops (if any)	

	glDisable(GL_NORMALIZE);
//	GLParticlesSetup();
	glDisable(GL_TEXTURE_2D);
	glLoadIdentity();

	if (!CompatibilityMode)
	RenderParticles(FXFALSE);		//Render all 2D-particles except the Sun and the moon(s)


	MessageHandler();


	//Display a small clock
	if ((int)(daytime/14400.0f*24.0f) < 10)
		sprintf(lead1, "%s", "0");
	else
		lead1[0]=0;
	if ((int)(daytime/14400.0f*24.0f*60.0f)%60 < 10)
		sprintf(lead2, "%s", "0");
	else
		lead2[0]=0;
	sprintf(tempstring, "%s%i:%s%i", lead1, (int)(daytime/14400.0f*24.0f), lead2, (int)(daytime/14400.0f*24.0f*60.0f)%60);
	mytext.Size(10);
	mytext.Draw(135, 400, tempstring);



   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();

	if ((!TextureEditor) && (!MenuActive))
	{
		if (CameraMode==0)
			DrawHud();
			if (FramesTillNextMapUpdate>50)
		{
			calcMap();
			FramesTillNextMapUpdate=0;
		}
		FramesTillNextMapUpdate++;
		drawMap();
		DisplayTeamStats();				// Display energy and resources
		DisplayTauntMenu();

	}

   glMatrixMode(GL_PROJECTION);
   glPopMatrix();   
   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();


//============================================================================
// Sky movement
//============================================================================

		SkyRotator1+=0.00005f;
		if (SkyRotator1>=20.0f)
			SkyRotator1-=20.0f;
		SkyRotator2+=0.0001f;
		if (SkyRotator2>=20.0f)
			SkyRotator2-=20.0f;



//============================================================================
// Engine sound handler
//============================================================================

		if (use_midas!=0)
		{
//				FSOUND_SetFrequency(channels[0], 2*(unsigned)(16050+ (4000*setSpeed)));
//				FSOUND_SetFrequency(channels[1], 2*(unsigned)(10550+ (5100*mySpeed)));
//				FSOUND_SetFrequency(channels[2], 2*(unsigned)(16050+ (5100*mySpeed)));
//				if (Player[localplayer].ControlledObject!=-1)
//				if (Objects[Player[localplayer].ControlledObject].isDiving)
//					FSOUND_SetFrequency(channels[19], 2*(unsigned)(18050+ (30*Objects[Player[localplayer].ControlledObject].AnimationPhase)));

		}

		if (Player[localplayer].ControlledObject!=-1)
		if (Objects[Player[localplayer].ControlledObject].isDocked)
			EngineSound=0;

		if (CameraMode!=0)
			EngineSound=0;

		if (EngineSound==0)
		{
//			FSOUND_SetVolume(channels[0],0);
//			FSOUND_SetVolume(channels[1],0);
//			FSOUND_SetVolume(channels[2],0);
		}
		else
		{
//			FSOUND_SetVolume(channels[0],200);
//			FSOUND_SetVolume(channels[1],200);
//			FSOUND_SetVolume(channels[2],200);
		}




//============================================================================
// Keyboard handler
//============================================================================
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) break;
                TranslateMessage(&msg);
    			DispatchMessage(&msg);
	    }

#ifdef USE_GLIDE
			grSstIdle();
#endif

				if (reloadTime>=0)
					reloadTime--;
				if (missileReloadTime>=0)
					missileReloadTime--;


				//Get Camera's grid position
				if (CamXPos>0)
					xGridPos = (int)(CamXPos/scalefactor)+1;
				else
					xGridPos = (int)(CamXPos/scalefactor);
				if (CamZPos>0)
					zGridPos = (int)(CamZPos/scalefactor)+1;
				else
					zGridPos = (int)(CamZPos/scalefactor);

		if (!MenuActive) 
		{

			if (Player[localplayer].ControlledObject!=-1)
			if (!Objects[Player[localplayer].ControlledObject].isDocked)
			{

				if ((mouse_x!=o_mouse_x) ||
					(mouse_y!=o_mouse_y))
				{
					MouseMoved=1;
				}


				if (MouseMoved)
				{
					float TargetPitch;
					if (MOUSE_INVERT==0)
						TargetPitch = (float)(((float)mouse_y/(float)winHeight)-0.5f)*(-60.0f);
					else
						TargetPitch = (float)(((float)mouse_y/(float)winHeight)-0.5f)*(60.0f);
					float TargetYaw = (float)(((float)mouse_x/(float)winWidth)-0.5f)*150.0f;

					if (MOUSE_INVERT==0)
						xpitchTime+=(float)(((float)mouse_y/(float)winHeight)-0.5f)*(-15.0f);
					else
						xpitchTime+=(float)(((float)mouse_y/(float)winHeight)-0.5f)*(15.0f);
					yawTime+=(float)(((float)mouse_x/(float)winWidth)-0.5f)*5.0f;
					

					if (TargetPitch>0)
					{
						if (xpitchTime>TargetPitch)
							if (xpitchTime-TargetPitch > 4.0f)
								xpitchTime-=4.0f;
							else
								xpitchTime=TargetPitch;
					}
					else
					{
						if (xpitchTime<TargetPitch)
							if (TargetPitch-xpitchTime > 4.0f)
								xpitchTime-=-4.0f;
							else
								xpitchTime=TargetPitch;
					}
					if (TargetYaw>0)
					{
						if (yawTime>TargetYaw)
							if (yawTime-TargetYaw > 6.0f)
								yawTime-=6.0f;
							else
								yawTime=TargetYaw;
					}
					else
					{
						if (yawTime<TargetYaw)
							if (TargetYaw-yawTime > 6.0f)
								yawTime-=-6.0f;
							else
								yawTime=TargetYaw;
					}



					if (xpitchTime>40.0f)
						xpitchTime=40.0f;
					if (xpitchTime<-40.0f)
						xpitchTime=-40.0f;
					if (yawTime>90.0f)
						yawTime=90.0f;
					if (yawTime<-90.0f)
						yawTime=-90.0f;

/*
					if (MOUSE_INVERT==0)
						xpitchTime=(float)(((float)mouse_y/(float)winHeight)-0.5f)*(-60.0f);
					else
						xpitchTime=(float)(((float)mouse_y/(float)winHeight)-0.5f)*(60.0f);
					yawTime=(float)(((float)mouse_x/(float)winWidth)-0.5f)*150.0f;
*/
					o_mouse_x=mouse_x;
					o_mouse_y=mouse_y;
				}

				if (keys[VK_DOWN])
				{
					MouseMoved=0;
					if (xpitchTime < 40.0f) {
						xpitchTime=xpitchTime + 8.0f; }
				}
				if (keys[VK_UP])
				{
					MouseMoved=0;
					if (xpitchTime > (-40.0f)) {
						xpitchTime=xpitchTime - 8.0f; }
				}
				if ((!keys[VK_DOWN])&&(!keys[VK_UP])&&(MouseMoved==0)) {
					if (xpitchTime < 0.0f) {
						xpitchTime+=3.0f; }
					if (xpitchTime > 0.0f) {
						xpitchTime-=3.0f; }
					if ((xpitchTime<3.0f) && (xpitchTime>(-3.0f))) {
						xpitchTime=0.0f; }
				}

			
				if (keys[VK_RIGHT])
				{
					MouseMoved=0;
					if (yawTime < 90.0f)
						yawTime+=7.0f;
				} else if (keys[VK_LEFT])
				{
					MouseMoved=0;
					if (yawTime > -90.0f)
						yawTime-=7.0f;
				} else if (MouseMoved==0){
					if (yawTime < 0.0f)
						yawTime+=3.0f;
					if (yawTime > 0.0f)
						yawTime-=3.0f;
					if ((yawTime<3.0f) && (yawTime>-3.0f))
						yawTime=0.0f;
				}

				
				
				
				
				
				if (keys['A'])
				{
					if (accTime < 2.0f)
						accTime+=0.05f*g_ispeedfactor;
				} else if (keys['Y'])
				{
					if (Cheat)
					{
						if (accTime > 0.0f)		//Ships can stop in mid-air while cheating
							accTime-=0.05f*g_ispeedfactor;
						if (accTime < 0.06f)
							accTime=0.0f;
					}
					else
					{
						if (accTime > 0.5f)		//Use this for realism
							accTime-=0.05f*g_ispeedfactor;
					}
				}

				setSpeed = accTime;
				
				if (!Objects[Player[localplayer].ControlledObject].isDiving)
				{
					if (mySpeed < setSpeed)
						mySpeed+=0.02f*g_ispeedfactor;
					if (mySpeed > setSpeed)
						mySpeed-=0.02f*g_ispeedfactor;
					if (Cheat)
						mySpeed = setSpeed;			//Disable momentum for cheaters
				}





				if ((keys[VK_CONTROL]) || (LeftMouseButton))
				{
					if ((Objects[Player[localplayer].ControlledObject].MissileType!=MISSILETYPE_MARKER) && (reloadTime <= 0))
					{
						if (use_midas!=0)
							AddNoiseEmitter( Player[localplayer].ControlledObject, SAMPLE_FIRE, 0.0f, 0.0f, 0.0f, 1000.0f);
//							PlaySoundFX(SAMPLE_FIRE, SAMPLE_FIRE);

						tempint = AddObject( OBJECTTYPE_WEAPON, //weapon
									OBJECTMESH_LASER, //laser bolt mesh
									Player[localplayer].Team, //team
									150, //time to live
									FXFALSE, //not AI controlled
									FXFALSE, //not homing
									FXTRUE,	 //visible
									FXFALSE, //not a marker missile
									FXFALSE,
									ControlXPos, ControlYPos, ControlHeight+1.0f, 
									-ControlYaw, -ControlPitch, -ControlRoll, Player[localplayer].ControlledObject, FXTRUE);
#ifdef USE_GL

						ParticleEmitters[AddParticleEmitter(	tempint,
									1, 2, 1, -1, 0.0f, 0.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.0f, 0.0f, 0.0f, 200.0f, 200.0f, 0.0f,
									30, 30, 0, 30)].ThreeD=FXTRUE; //Laser flare

/*						AddLight( tempint, -1, 
									0.8f, 0.8f, 0.0f,
									0.8f, 0.8f, 0.0f,
									0.8f, 0.8f, 0.0f,
									1.0f, 0.0f, 0.0001f,
									ControlXPos, ControlYPos, ControlHeight+1.0f);*/


#endif
						reloadTime=(int)(10.0f*g_ispeedfactor);
					}

				}


				if ((keys[VK_SPACE]) || (RightMouseButton))
				{
					//Guided missile
					if (Objects[Player[localplayer].ControlledObject].MissileType==MISSILETYPE_GUIDED)
					{
						if ((missileReloadTime <= 0) && (Objects[Player[localplayer].ControlledObject].MissileAmmo>0))
						{
							if (use_midas!=0)
								AddNoiseEmitter( Player[localplayer].ControlledObject, SAMPLE_MISSILE, 0.0f, 0.0f, 0.0f, 1000.0f);
//								PlaySoundFX(4, 4);
								tempint = AddObject(	OBJECTTYPE_WEAPON, //weapon
										OBJECTMESH_MISSILE, //missile mesh
										Player[localplayer].Team, //team
										600, //time to live
										FXFALSE, //not AI controlled
										FXTRUE,  //Homing weapon
										FXTRUE,	 //visible
										FXFALSE, //Not a marker missile
										FXFALSE,
										ControlXPos, ControlYPos, ControlHeight+0.1f,
										180.0f-ControlYaw, ControlPitch, -ControlRoll, Player[localplayer].ControlledObject, FXTRUE);
								tempemitter = AddParticleEmitter( tempint,
										0, (int)(100.0f*g_ispeedfactor), 1, -1, 0.0f, 0.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.1f, 0.0f, 0.0f, 50.0f, 50.0f, 160.0f,
										255, 255, 255, 6);//color
								ParticleEmitters[tempemitter].Slot=1;


/*								AddLight( tempint, -1, 
										0.7f, 0.7f, 0.7f,
										0.7f, 0.7f, 0.7f,
										0.7f, 0.7f, 0.7f,
										1.0f, 0.0f, 0.0001f,
										ControlXPos, ControlYPos, ControlHeight+1.0f);*/

							Objects[tempint].isGuided=FXTRUE;
							if (LocalPlayerLock!=-1)
								Objects[tempint].targetObject = LocalPlayerLock;
							ParticleEmitters[tempemitter].AlwaysActive=FXTRUE;
							if (!Cheat)
							Objects[Player[localplayer].ControlledObject].MissileAmmo--;
							missileReloadTime=(int)(200.0f*g_ispeedfactor);
						}
					} 

					//Marker missile
					if (Objects[Player[localplayer].ControlledObject].MissileType==MISSILETYPE_MARKER)
					{
						if (reloadTime <= 0)
						{
							//Fire marker missile
							if (Objects[Player[localplayer].ControlledObject].MissileAmmo>0)
							{
							if (use_midas!=0)
								AddNoiseEmitter( Player[localplayer].ControlledObject, SAMPLE_MISSILE, 0.0f, 0.0f, 0.0f, 1000.0f);
//							PlaySoundFX(4, 4);
							tempint = AddObject(	OBJECTTYPE_WEAPON, //weapon
										OBJECTMESH_MISSILE, //missile mesh
										Player[localplayer].Team, //team
										600, //time to live
										FXFALSE, //not AI controlled
										FXFALSE,  //not a Homing weapon
										FXTRUE,	 //visible
										FXTRUE, //This is a marker missile!!!
										FXFALSE,
										ControlXPos, ControlYPos, ControlHeight+0.1f,
										180.0f-ControlYaw, ControlPitch, -ControlRoll, Player[localplayer].ControlledObject, FXTRUE);
							tempemitter = AddParticleEmitter( tempint,
										0, (int)(50.0f*g_ispeedfactor), 1, -1, 0.0f, 0.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.1f, 0.1f, 0.0f, 100.0f, 50.0f, 160.0f,
										100, 100, 255, 6);//color blue
								ParticleEmitters[tempemitter].Slot=1;

/*								AddLight( tempint, -1, 
										0.3f, 0.3f, 0.8f,
										0.3f, 0.3f, 0.8f,
										0.3f, 0.3f, 0.8f,
										1.0f, 0.0f, 0.0001f,
										ControlXPos, ControlYPos, ControlHeight+1.0f);
*/
							Objects[tempint].isGuided=FXFALSE;
							ParticleEmitters[tempemitter].AlwaysActive=FXTRUE;
							if (!Cheat)
							Objects[Player[localplayer].ControlledObject].MissileAmmo--;
							reloadTime=(int)(200.0f*g_ispeedfactor);
							}
						}
					}

					//Dumb missile
					if (Objects[Player[localplayer].ControlledObject].MissileType==MISSILETYPE_DUMBFIRE)
					{
						if ((missileReloadTime <= 0) && (Objects[Player[localplayer].ControlledObject].MissileAmmo>0))
						{
							if (use_midas!=0)
								AddNoiseEmitter( Player[localplayer].ControlledObject, SAMPLE_MISSILE, 0.0f, 0.0f, 0.0f, 1000.0f);
//								PlaySoundFX(4, 4);
								tempint = AddObject(	OBJECTTYPE_WEAPON, //weapon
										OBJECTMESH_MISSILE, //missile mesh
										Player[localplayer].Team, //team
										600, //time to live
										FXFALSE, //not AI controlled
										FXFALSE,  //Homing weapon
										FXTRUE,	 //visible
										FXFALSE, //Not a marker missile
										FXFALSE,
										ControlXPos, ControlYPos, ControlHeight+0.1f,
										180.0f-ControlYaw, ControlPitch, -ControlRoll, Player[localplayer].ControlledObject, FXTRUE);
								tempemitter = AddParticleEmitter( tempint,
										0, (int)(50.0f*g_ispeedfactor), 2, -1, 0.0f, 0.0f, 0.0f, -500.0f, 0.0f, 0.0f, -1, 0.1f, 0.0f, 0.0f, 80.0f, 50.0f, 160.0f,
										100, 100, 0, 6);//color
								ParticleEmitters[tempemitter].Slot=1;

/*
								AddLight( tempint, -1, 
										0.7f, 0.7f, 0.0f,
										0.7f, 0.7f, 0.0f,
										0.7f, 0.7f, 0.0f,
										1.0f, 0.0f, 0.0001f,
										ControlXPos, ControlYPos, ControlHeight+1.0f);
*/
							Objects[tempint].isGuided=FXFALSE;
							ParticleEmitters[tempemitter].AlwaysActive=FXTRUE;
							if (!Cheat)
							Objects[Player[localplayer].ControlledObject].MissileAmmo--;
							missileReloadTime=(int)(20.0f*g_ispeedfactor);
						}
					} 

				}



				//Draw colored dots, marking possible building sites
/*				if (!MasterGamePause)
				{
					AddParticleEmitter(	-1,
					1, 2, 1, -1, 00,
					Vertices[Faces[facexy[MapDimension-xGridPos-1][MapDimension-zGridPos]].srcVerts[0]].v.x,
					Vertices[Faces[facexy[MapDimension-xGridPos-1][MapDimension-zGridPos]].srcVerts[0]].v.z,
					-Vertices[Faces[facexy[MapDimension-xGridPos-1][MapDimension-zGridPos]].srcVerts[0]].v.y,
					0, 0, 2, 0.0f, 0.0f, 0.0f, 400, 400, 0,
					200*(1-CheckBuildingSiteClear(xGridPos, zGridPos)), 200*CheckBuildingSiteClear(xGridPos, zGridPos), 0, 33);
					AddParticleEmitter(	-1,
					1, 2, 1, -1, 00,
					Vertices[Faces[facexy[MapDimension-xGridPos-1][MapDimension-zGridPos]].srcVerts[1]].v.x,
					Vertices[Faces[facexy[MapDimension-xGridPos-1][MapDimension-zGridPos]].srcVerts[1]].v.z,
					-Vertices[Faces[facexy[MapDimension-xGridPos-1][MapDimension-zGridPos]].srcVerts[1]].v.y,
					0, 0, 2, 0.0f, 0.0f, 0.0f, 400, 400, 0,
					200*(1-CheckBuildingSiteClear(xGridPos, zGridPos)), 200*CheckBuildingSiteClear(xGridPos, zGridPos), 0, 33);
					AddParticleEmitter(	-1,
					1, 2, 1, -1, 00,
					Vertices[Faces[facexy[MapDimension-xGridPos-1][MapDimension-zGridPos]].srcVerts[2]].v.x,
					Vertices[Faces[facexy[MapDimension-xGridPos-1][MapDimension-zGridPos]].srcVerts[2]].v.z,
					-Vertices[Faces[facexy[MapDimension-xGridPos-1][MapDimension-zGridPos]].srcVerts[2]].v.y,
					0, 0, 2, 0.0f, 0.0f, 0.0f, 400, 400, 0,
					200*(1-CheckBuildingSiteClear(xGridPos, zGridPos)), 200*CheckBuildingSiteClear(xGridPos, zGridPos), 0, 33);
					AddParticleEmitter(	-1,
					1, 2, 1, -1, 00,
					Vertices[Faces[facexyB[MapDimension-xGridPos-1][MapDimension-zGridPos]].srcVerts[1]].v.x,
					Vertices[Faces[facexyB[MapDimension-xGridPos-1][MapDimension-zGridPos]].srcVerts[1]].v.z,
					-Vertices[Faces[facexyB[MapDimension-xGridPos-1][MapDimension-zGridPos]].srcVerts[1]].v.y,
					0, 0, 2, 0.0f, 0.0f, 0.0f, 400, 400, 0,
					200*(1-CheckBuildingSiteClear(xGridPos, zGridPos)), 200*CheckBuildingSiteClear(xGridPos, zGridPos), 0, 33);
				}
*/

				if (keys[VK_F1])
					BuyBuilding(OBJECTMESH_POWERPLANT, Player[localplayer].Team, CamXPos, CamZPos);
				if (keys[VK_F2])
					BuyBuilding(OBJECTMESH_MINE, Player[localplayer].Team, CamXPos, CamZPos);
				if (keys[VK_F3])
					BuyBuilding(OBJECTMESH_AAA, Player[localplayer].Team, CamXPos, CamZPos);
				if (keys[VK_F4])
					BuyBuilding(OBJECTMESH_SAM, Player[localplayer].Team, CamXPos, CamZPos);
				if (keys[VK_F5])	//Sat uplink
					BuyBuilding(OBJECTMESH_UPLINK, Player[localplayer].Team, CamXPos, CamZPos);
				if (keys[VK_F6])	//Command Center
					BuyBuilding(OBJECTMESH_COMMANDCENTER, Player[localplayer].Team, CamXPos, CamZPos);

			} //The rest of the keys work even when docked


				if((keys['X']) && (reloadTime<=0))
				{
					if (NightVision) 
						NightVision=FXFALSE;
					else
						NightVision=FXTRUE;
					reloadTime=(int)(5.0f*g_ispeedfactor);
				}


				// The most powerful key in the game ;-)
				if (keys['L'])
				{
					//We're in the air, so let's land
					if	((!Objects[Player[localplayer].ControlledObject].isDocked) &&
						(!Objects[Player[localplayer].ControlledObject].isDiving) &&
						(!Objects[Player[localplayer].ControlledObject].isLanding) &&
						(!Objects[Player[localplayer].ControlledObject].isStarting))
					{
						//...but only if we're over the command center
						if ((xGridPos==Objects[getObject(Player[localplayer].Team, OBJECTTYPE_BUILDING, OBJECTMESH_COMMANDCENTER, 1)].xGrid) &&
							(zGridPos==Objects[getObject(Player[localplayer].Team, OBJECTTYPE_BUILDING, OBJECTMESH_COMMANDCENTER, 1)].zGrid) &&
							(!Objects[getObject(Player[localplayer].Team, OBJECTTYPE_BUILDING, OBJECTMESH_COMMANDCENTER, 1)].isBusy) )
						{
							Objects[Player[localplayer].ControlledObject].isLanding=FXTRUE;
							Objects[Player[localplayer].ControlledObject].AnimationPhase=0;
							Objects[Player[localplayer].ControlledObject].AnimationSteps= (Objects[Player[localplayer].ControlledObject].Height - (Objects[getObject(Player[localplayer].Team, OBJECTTYPE_BUILDING, OBJECTMESH_COMMANDCENTER, 1)].Height-10.0f)) / 200;
							Objects[Player[localplayer].ControlledObject].InitialHeight= Objects[Player[localplayer].ControlledObject].Height;
							if (use_midas!=0)
								PlaySoundFX(6, 6);

						}
					}
				}

			} //End Non-Menu movement block


			
			if (!TauntMenuActive)
			{
					if ((keys['V']) && (reloadTime <= 0))
					{
						reloadTime=(int)(5.0f*g_ispeedfactor);
						TauntMenuActive=FXTRUE;
						LoadTauntMenu(0);
					}
			}
			else
			{
					if ((keys['V']) && (reloadTime <= 0))
					{
						reloadTime=(int)(5.0f*g_ispeedfactor);
						TauntMenuActive=FXFALSE;
						TauntMenuDisplaySize=0;
						TauntMenuHistory[0]=0;
						TauntMenuHistoryLen=0;
					}
					if ((keys['1']) && (reloadTime <= 0))
					{
						reloadTime=(int)(5.0f*g_ispeedfactor);
						LoadTauntMenu(1);
					}
					if ((keys['2']) && (reloadTime <= 0))
					{
						reloadTime=(int)(5.0f*g_ispeedfactor);
						LoadTauntMenu(2);
					}
					if ((keys['3']) && (reloadTime <= 0))
					{
						reloadTime=(int)(5.0f*g_ispeedfactor);
						LoadTauntMenu(3);
					}
					if ((keys['4']) && (reloadTime <= 0))
					{
						reloadTime=(int)(5.0f*g_ispeedfactor);
						LoadTauntMenu(4);
					}
					if ((keys['5']) && (reloadTime <= 0))
					{
						reloadTime=(int)(5.0f*g_ispeedfactor);
						LoadTauntMenu(5);
					}
					if ((keys['6']) && (reloadTime <= 0))
					{
						reloadTime=(int)(5.0f*g_ispeedfactor);
						LoadTauntMenu(6);
					}
					if ((keys['7']) && (reloadTime <= 0))
					{
						reloadTime=(int)(5.0f*g_ispeedfactor);
						LoadTauntMenu(7);
					}
					if ((keys['8']) && (reloadTime <= 0))
					{
						reloadTime=(int)(5.0f*g_ispeedfactor);
						LoadTauntMenu(8);
					}
					if ((keys['9']) && (reloadTime <= 0))
					{
						reloadTime=(int)(5.0f*g_ispeedfactor);
						LoadTauntMenu(9);
					}
			}





					if ((keys['P']) && (reloadTime <= 0) && (!myServer.isActive) && (!myServer.isClient))
					{
						MasterGamePause = (!MasterGamePause);
						reloadTime=(int)(10.0f*g_ispeedfactor);
					}
					
					if ((keys['I']) && (reloadTime <= 0))
					{
						reloadTime=(int)(10.0f*g_ispeedfactor);
						if (playlist[CurrentSong].type==MUSIC_MOD)
						{
							StopModuleMusic();
							ModuleIsFresh=1;
						}
						else if (playlist[CurrentSong].type==MUSIC_MP3)
						{
							StopStreamMusic();
						}
						MusicHandler(FXTRUE);
					}

					if (!MenuActive)
					{
						if ((keys['Q']) && (reloadTime <= 0))
						{
							reloadTime=(int)(10.0f*g_ispeedfactor);
							if (CameraMode < 5)
								CameraMode++;
							else
								CameraMode = 0;
						}
						if ((keys['W']) && (reloadTime <= 0))
						{
							reloadTime=(int)(10.0f*g_ispeedfactor);
							SearchForCoolLookAt(OBJECTTYPE_SHIP);
						}
						if ((keys['E']) && (reloadTime <= 0))
						{
							reloadTime=(int)(10.0f*g_ispeedfactor);
							SearchForCoolLookAt(OBJECTTYPE_BUILDING);
						}
						if ((keys['R']) && (reloadTime <= 0))
						{
							reloadTime=(int)(10.0f*g_ispeedfactor);
							SearchForCoolLookAt(OBJECTTYPE_WEAPON);
						}
						if ((keys['T']) && (reloadTime <= 0))
						{
							LookAtObject = Player[localplayer].ControlledObject;
							CameraMode = 0;
						}
					}






				if ((Cheat) || (TextureEditor))
				{
					if (keys['C'])
						ControlRoll=ControlRoll+dDelta;
					if (keys['D'])
						ControlRoll=ControlRoll-dDelta;

					
					if (keys['G'])
						ControlXPos=ControlXPos+(dDelta/2.0f)*scalefactor;
					if (keys['B'])
						ControlXPos=ControlXPos-(dDelta/2.0f)*scalefactor;
	
					if (keys['H'])
						ControlYPos=ControlYPos+(dDelta/2.0f)*scalefactor;
					if (keys['N'])
						ControlYPos=ControlYPos-(dDelta/2.0f)*scalefactor;
	
					if (keys['J'])
						ControlHeight=ControlHeight-(dDelta/2.0f)*scalefactor;
					if (keys['M'])
						ControlHeight=ControlHeight+(dDelta/2.0f)*scalefactor;


					if (keys['K'])
						Objects[Player[localplayer].ControlledObject].isDiving=FXTRUE;

				}
/*
				//-------------------------------------------------------
				//Debug block for tank movement
				Objects[Player[localplayer].ControlledObject].Speed=0.0f;
				for (tempint=0; tempint<NumObjects; tempint++)
				{
					if (Objects[tempint].ObjectMesh==OBJECTMESH_LIGHTTANK)
					Objects[tempint].Speed=0.0f;
				}
				if (keys['1'])
					for (tempint=0; tempint<NumObjects; tempint++)
					{
						if (Objects[tempint].ObjectMesh==OBJECTMESH_LIGHTTANK)
						Objects[tempint].xPos+=(dDelta/2.0f)*scalefactor;
					}
				if (keys['2'])
					for (tempint=0; tempint<NumObjects; tempint++)
					{
						if (Objects[tempint].ObjectMesh==OBJECTMESH_LIGHTTANK)
						Objects[tempint].xPos-=(dDelta/2.0f)*scalefactor;
					}
				if (keys['3'])
					for (tempint=0; tempint<NumObjects; tempint++)
					{
						if (Objects[tempint].ObjectMesh==OBJECTMESH_LIGHTTANK)
						Objects[tempint].zPos+=(dDelta/2.0f)*scalefactor;
					}
				if (keys['4'])
					for (tempint=0; tempint<NumObjects; tempint++)
					{
						if (Objects[tempint].ObjectMesh==OBJECTMESH_LIGHTTANK)
						Objects[tempint].zPos-=(dDelta/2.0f)*scalefactor;
					}
				if (keys['5'])
					for (tempint=0; tempint<NumObjects; tempint++)
					{
						if (Objects[tempint].ObjectMesh==OBJECTMESH_LIGHTTANK)
						Objects[tempint].Yaw+=2;
					}
				if (keys['6'])
					for (tempint=0; tempint<NumObjects; tempint++)
					{
						if (Objects[tempint].ObjectMesh==OBJECTMESH_LIGHTTANK)
						Objects[tempint].Yaw-=2;
					}

  
				//-------------------------------------------------------
*/





//	This is the slot editor
/*				if (reloadTime <= 0)
				{
					if (keys['1'])
					{
						ParticleSlots[editSlotMesh].Vertices[editSlot].v.x+=0.25f;
						reloadTime=(int)(2.0f*g_ispeedfactor);
					}
					if (keys['2'])
					{
						ParticleSlots[editSlotMesh].Vertices[editSlot].v.x-=0.25f;
						reloadTime=(int)(2.0f*g_ispeedfactor);
					}

					if (keys['3'])
					{
						ParticleSlots[editSlotMesh].Vertices[editSlot].v.z+=0.25f;
						reloadTime=(int)(2.0f*g_ispeedfactor);
					}
					if (keys['4'])
					{
						ParticleSlots[editSlotMesh].Vertices[editSlot].v.z-=0.25f;
						reloadTime=(int)(2.0f*g_ispeedfactor);
					}

					if (keys['5'])
					{
						ParticleSlots[editSlotMesh].Vertices[editSlot].v.y+=0.25f;
						reloadTime=(int)(2.0f*g_ispeedfactor);
					}
					if (keys['6'])
					{
						ParticleSlots[editSlotMesh].Vertices[editSlot].v.y-=0.25f;
						reloadTime=(int)(2.0f*g_ispeedfactor);
					}


					if ((keys['7']) && (editSlot>0))
					{
						editSlot--;
						reloadTime=(int)(5.0f*g_ispeedfactor);
					}
					if (keys['8'])
					{
						editSlot++;
						if (editSlot>=ParticleSlots[editSlotMesh].numslots)
						{
							ParticleSlots[editSlotMesh].numslots++;
							tempint=AddParticleEmitter(	0,
							1, 2, 1, -1, 0, 0, 0, -400, 0, 0, -1, 0.0f, 0.0f, 0.0f, 100, 100, 0,
							255, 255, 255, 30);
							ParticleEmitters[tempint].Slot=editSlot;
							ParticleEmitters[tempint].ThreeD=FXTRUE;
						}
						reloadTime=(int)(5.0f*g_ispeedfactor);
					}

					if (keys['T'])		//Save Texture data into a file
					{
						SaveSlotPosition(editSlotMesh);
						reloadTime=(int)(5.0f*g_ispeedfactor);
					}
				} 
*/



//	This is the regular texture editor
				if ((TextureEditor) && (reloadTime <= 0))
				{
					if ((keys['2']) && (ActiveFace<Meshes[Objects[editObject].ObjectMesh].numfaces))
					{
						ActiveFace++;
						reloadTime=(int)(5.0f*g_ispeedfactor);
					}
					if ((keys['1']) && (ActiveFace>0))
					{
						ActiveFace--;
						reloadTime=(int)(5.0f*g_ispeedfactor);
					}
	
					if ((keys['4']) && (Meshes[Objects[editObject].ObjectMesh].Faces[ActiveFace].Texture<maxTexture+3))
					{
						Meshes[Objects[editObject].ObjectMesh].Faces[ActiveFace].Texture++;
						reloadTime=(int)(5.0f*g_ispeedfactor);
					}
					if ((keys['3']) && (Meshes[Objects[editObject].ObjectMesh].Faces[ActiveFace].Texture>0))
					{
						Meshes[Objects[editObject].ObjectMesh].Faces[ActiveFace].Texture--;
						reloadTime=(int)(5.0f*g_ispeedfactor);
					}
	
					if (keys['5'])
					{
						if (Meshes[Objects[editObject].ObjectMesh].Faces[ActiveFace].TextureMode>1)
							Meshes[Objects[editObject].ObjectMesh].Faces[ActiveFace].TextureMode--;
						else
							Meshes[Objects[editObject].ObjectMesh].Faces[ActiveFace].TextureMode=1;
						reloadTime=(int)(5.0f*g_ispeedfactor);
					}
					if (keys['6'])
					{
						if (Meshes[Objects[editObject].ObjectMesh].Faces[ActiveFace].TextureMode<4)
							Meshes[Objects[editObject].ObjectMesh].Faces[ActiveFace].TextureMode++;
						else
							Meshes[Objects[editObject].ObjectMesh].Faces[ActiveFace].TextureMode=4;
						reloadTime=(int)(5.0f*g_ispeedfactor);
					}

					if (keys['7'])
					{
						Meshes[Objects[editObject].ObjectMesh].Faces[ActiveFace].Transparent=FXFALSE;
						reloadTime=(int)(5.0f*g_ispeedfactor);
					}
					if (keys['8'])
					{
						Meshes[Objects[editObject].ObjectMesh].Faces[ActiveFace].Transparent=FXTRUE;
						reloadTime=(int)(5.0f*g_ispeedfactor);
					}
/*					if (keys['9'])
					{
						Meshes[Objects[editObject].ObjectMesh].Faces[ActiveFace].isLight=FXFALSE;
						reloadTime=(int)(8.0f*g_ispeedfactor);
					}
					if (keys['0'])
					{
						Meshes[Objects[editObject].ObjectMesh].Faces[ActiveFace].isLight=FXTRUE;
						reloadTime=(int)(8.0f*g_ispeedfactor);
					}
*/					
					if (keys['T'])		//Save Texture data into a file
					{
						SaveTextureData();
						reloadTime=(int)(5.0f*g_ispeedfactor);
					}
						sprintf(tempstring, "Face: %i\nTexture: %i", ActiveFace, Meshes[Objects[editObject].ObjectMesh].Faces[ActiveFace].Texture);
						AddMessage(tempstring, 255,255,0);
				} 
//				else
//				{
					if (keys['9'])
					{
						daytime-=100.0f;
						daytimeframes=5.0f;
					}
					if (keys['0'])
					{
						daytime+=100.0f;
						daytimeframes=5.0f;
					}
//				}





//	This is for the menu Position editor
/*				if ((TextureEditor) && (reloadTime <= 0))
				{
					tempint=-1;
					if (keys['1'])	//Singleplayer
						tempint=0;
					if (keys['2'])	//Multiplayer
						tempint=1;
					if (keys['3'])	//Options
						tempint=2;
					if (keys['4'])	//Quit
						tempint=3;
					if (keys['5'])	//Console: Singleplayer
						tempint=4;
					if (keys['6'])	//Console: Multiplayer
						tempint=5;
					if (keys['7'])	//Console: Options
						tempint=6;
					if (keys['8'])	//Unused
						tempint=7;

					if (tempint!= -1)
					{

/*						MenuPositions[tempint][0]=ControlXPos-Objects[editObject].xPos;
						MenuPositions[tempint][1]=ControlHeight-Objects[editObject].Height;
						MenuPositions[tempint][2]=ControlYPos-Objects[editObject].zPos;
						MenuPositions[tempint][3]=ControlYaw;
						MenuPositions[tempint][4]=ControlPitch;
						MenuPositions[tempint][5]=ControlRoll;
						SaveMenuPositions();

*/
/*
						ControlXPos=MenuPositions[tempint][0]+Objects[editObject].xPos;
						ControlHeight=MenuPositions[tempint][1]+Objects[editObject].Height;
						ControlYPos=MenuPositions[tempint][2]+Objects[editObject].zPos;
						ControlYaw=MenuPositions[tempint][3];
						ControlPitch=MenuPositions[tempint][4];
						ControlRoll=MenuPositions[tempint][5];

						reloadTime=(int)(5.0f*g_ispeedfactor);
					}

				} 
*/


				if (Objects[Player[localplayer].ControlledObject].isDocked)
				if ((keys[27]) && (reloadTime <= 0) && (!ConsoleActive))
				{
					reloadTime=(int)(5.0f*g_ispeedfactor);
					if (!MenuActive)
					{
						MenuActive = FXTRUE;
						LoadoutMenuActive=FXFALSE;
						SavXPos=ControlXPos; //Save Camera Setup
						SavYPos=ControlYPos;
						SavHeight=ControlHeight;
						SavYaw=ControlYaw;
						SavPitch=ControlPitch;
						SavRoll=ControlRoll;
						SavAltYaw=ControlAltYaw;
						savCameraMode=CameraMode;
						CameraMode=6;
					}
					else {
						if (CurrentMenu<=3) {
							MenuActive = FXFALSE;	//Leave Menu, back to game.
							ControlXPos=SavXPos; //Restore Camera Setup
							ControlYPos=SavYPos;
							ControlHeight=SavHeight;
							ControlYaw=SavYaw;
							ControlPitch=SavPitch;
							ControlRoll=SavRoll;
							ControlAltYaw=SavAltYaw;
							CameraMode=savCameraMode;
						}
						else {
							if (CurrentMenu==4)
								CurrentMenu=0;
							if (CurrentMenu==5)
								CurrentMenu=1;
							if (CurrentMenu==6)
								CurrentMenu=2;
						}

					}

//					frames = 0;
				}


				EngineSound=1;

				// Enter Shopping menu
				if ((Objects[Player[localplayer].ControlledObject].isDocked) && 
					(!MenuActive) && 
					(!ConsoleActive) &&
					(!JoinMenuActive) &&
					(!LoadoutMenuActive))
				{
					LoadoutMenuActive=FXTRUE;
					CameraMode = 0;
					EngineSound=0;
					SwitchToMenu(3); //Go shopping
					if (Objects[Player[localplayer].ControlledObject].ObjectMesh==OBJECTMESH_SHIP1)
						menu[0].val=0;
					if (Objects[Player[localplayer].ControlledObject].ObjectMesh==OBJECTMESH_SHIP2)
						menu[0].val=1;
					if (Objects[Player[localplayer].ControlledObject].ObjectMesh==OBJECTMESH_SHIP3)
						menu[0].val=2;
					if (Objects[Player[localplayer].ControlledObject].ObjectMesh==OBJECTMESH_LIGHTTANK)
						menu[0].val=3;
					if (Objects[Player[localplayer].ControlledObject].ObjectMesh==OBJECTMESH_MEDIUMTANK)
						menu[0].val=4;
					menu[1].val=Objects[Player[localplayer].ControlledObject].MissileType;
					menu[2].val=Objects[Player[localplayer].ControlledObject].SpecialType;
				}

				//Don't shop if game's over!
				if ((GameHalted) &&
					(!MenuActive))
				{
					LoadoutMenuActive=FXFALSE;
					mytext.Size(14);
					mytext.Draw(310, 200, GameHaltMsg);
				}


//				Actual Shopping menu handling
//------------------------------------
				if ((!MenuActive) && 
					(!ConsoleActive) &&
					(LoadoutMenuActive) &&
					(!JoinMenuActive) && 
					(frames!=0))
				{
					EngineSound=0;
					menuDisplay();

					if ((keys[VK_RETURN]) && (reloadTime <= 0))
					{
						PlaySoundFX(33,33);
						reloadTime=(int)(5.0f*g_ispeedfactor);
		                menuEnterHandle ();
					}
					if ((keys[VK_DOWN]) && (reloadTime <= 0))
					{
						PlaySoundFX(31,31);
						reloadTime=(int)(5.0f*g_ispeedfactor);
		                menuAdvance (MENU_DOWN);
					}
					if ((keys[VK_UP]) && (reloadTime <= 0))
					{
						PlaySoundFX(31,31);
						reloadTime=(int)(5.0f*g_ispeedfactor);
		           		menuAdvance (MENU_UP);
					}
					if ((keys[VK_RIGHT]) && (reloadTime <= 0))
					{
						PlaySoundFX(34,34);
						reloadTime=(int)(5.0f*g_ispeedfactor);
		                menuOptionAdvance (MENU_UP);
					}
					if ((keys[VK_LEFT]) && (reloadTime <= 0))
					{
						PlaySoundFX(34,34);
						reloadTime=(int)(5.0f*g_ispeedfactor);
		                menuOptionAdvance (MENU_DOWN);
					}

				}
//------------------------------------







//				Ok, here goes all the menu selection stuff
//------------------------------------
				if ((MenuActive) && (!ConsoleActive) && (!JoinMenuActive))
				{
					EngineSound=0;
					if ((keys[VK_RETURN]) && (reloadTime <= 0))
					{
						reloadTime=(int)(5.0f*g_ispeedfactor);
						
						if (CurrentMenu==3)		//Quit selected
						{
							frames = 0;
						}
						else
						{
							PlaySoundFX(30,30);
							SwitchToMenu(CurrentMenu);
							ConsoleActive = FXTRUE;
						}

						if (CurrentMenu==0)
							CurrentMenu=4;
						if (CurrentMenu==1)
							CurrentMenu=5;
						if (CurrentMenu==2)
							CurrentMenu=6;
					}

					if ((keys[VK_RIGHT]) && (reloadTime <= 0))
					{
						reloadTime=(int)(5.0f*g_ispeedfactor);
						if (CurrentMenu<2)
							CurrentMenu++;
					}
					if ((keys[VK_LEFT]) && (reloadTime <= 0))
					{
						reloadTime=(int)(5.0f*g_ispeedfactor);
						if ((CurrentMenu>0) && (CurrentMenu<3))
							CurrentMenu--;
					}
					if ((keys[VK_DOWN]) && (reloadTime <= 0))
					{
						reloadTime=(int)(5.0f*g_ispeedfactor);
						CurrentMenu=3;
					}
					if ((keys[VK_UP]) && (reloadTime <= 0))
					{
						reloadTime=(int)(5.0f*g_ispeedfactor);
						if (CurrentMenu==3)
							CurrentMenu=1;
					}


						tempint = getObject(Player[localplayer].Team, OBJECTTYPE_BUILDING, OBJECTMESH_COMMANDCENTER, 1);
						CameraMode = 6;

						CamXPos=MenuPositions[CurrentMenu][0]+Objects[tempint].xPos;
						CamHeight=MenuPositions[CurrentMenu][1]+Objects[tempint].Height;
						CamZPos=MenuPositions[CurrentMenu][2]+Objects[tempint].zPos;
						CamYaw=MenuPositions[CurrentMenu][3];
						CamPitch=MenuPositions[CurrentMenu][4];
						CamRoll=MenuPositions[CurrentMenu][5];
				}
//------------------------------------




//Draw the menutext and handle keys.
//------------------------------------
				if ((MenuActive) && (ConsoleActive) && (!JoinMenuActive))
				{
					EngineSound=0;
					menuDisplay();

					if ((keys[VK_RETURN]) && (reloadTime <= 0))
					{
						PlaySoundFX(33,33);
						reloadTime=(int)(5.0f*g_ispeedfactor);
		                menuEnterHandle ();
					}
					if ((keys[27]) && (reloadTime <= 0))
					{
						reloadTime=(int)(5.0f * g_ispeedfactor);
						menuCleanUp ();
						ConsoleActive=FXFALSE;
						if (CurrentMenu==4)
							CurrentMenu=0;
						if (CurrentMenu==5)
							CurrentMenu=1;
						if (CurrentMenu==6)
							CurrentMenu=2;
					}
					if ((keys[VK_DOWN]) && (reloadTime <= 0))
					{
						PlaySoundFX(31,31);
						reloadTime=(int)(5.0f * g_ispeedfactor);
		                menuAdvance (MENU_DOWN);
					}
					if ((keys[VK_UP]) && (reloadTime <= 0))
					{
						PlaySoundFX(31,31);
						reloadTime=(int)(5.0f * g_ispeedfactor);
		                menuAdvance (MENU_UP);
					}
					if ((keys[VK_RIGHT]) && (reloadTime <= 0))
					{
						PlaySoundFX(32,32);
						reloadTime=(int)(5.0f * g_ispeedfactor);
		                menuOptionAdvance (MENU_UP);
					}
					if ((keys[VK_LEFT]) && (reloadTime <= 0))
					{
						PlaySoundFX(32,32);
						reloadTime=(int)(5.0f * g_ispeedfactor);
		                menuOptionAdvance (MENU_DOWN);
					}
				}
//------------------------------------


//Server list and join menu
//------------------------------------
				if (JoinMenuActive)
				{
					if (numServers>0)
					for (i=0;i<numServers;i++)
					{
						if ((i>=menuoffset) && (i<=menuoffset + 21))
						{
							if ((menuchoice+menuoffset)==i)
							{
								sprintf(tempstring, "\\a(0 255 0 255)%s", Servers[i].Name);
								mytext.Size(8);
								mytext.Draw(170, 300-(9*(i-menuoffset)), tempstring);
	
								sprintf(tempstring, "\\a(0 255 0 255)%i/%i", Servers[i].CurClients, Servers[i].MaxClients);
								mytext.Size(8);
								mytext.Draw(280, 300-(9*(i-menuoffset)), tempstring);
	
								sprintf(tempstring, "\\a(0 255 0 255)%s", Servers[i].Adress);
								mytext.Size(8);
								mytext.Draw(320, 300-(9*(i-menuoffset)), tempstring);

							}
							else
							{
								sprintf(tempstring, "\\a(0 128 0 255)%s", Servers[i].Name);
								mytext.Size(8);
								mytext.Draw(170, 300-(9*(i-menuoffset)), tempstring);
	
								sprintf(tempstring, "\\a(0 128 0 255)%i/%i", Servers[i].CurClients, Servers[i].MaxClients);
								mytext.Size(8);
								mytext.Draw(280, 300-(9*(i-menuoffset)), tempstring);
	
								sprintf(tempstring, "\\a(0 128 0 255)%s", Servers[i].Adress);
								mytext.Size(8);
								mytext.Draw(320, 300-(9*(i-menuoffset)), tempstring);
							}
						}
					}
	
					if (numServers==0)
					{
						mytext.Size(10);
						mytext.Draw(170, 300-9, "No internet servers found!");
					}
					if (numServers==-1)
					{
						mytext.Size(10);
						mytext.Draw(170, 300-9, "Couldn't contact master server!");
					}

					if (numServers>0)
					if ((keys[VK_RETURN]) && (reloadTime <= 0))
					{
						PlaySoundFX(33,33);
						reloadTime=(int)(5.0f*g_ispeedfactor);

						if ((Servers[menuchoice+menuoffset].CurClients < Servers[menuchoice+menuoffset].MaxClients) && //If there's still a slot free...
							(Servers[menuchoice+menuoffset].Version == myServer.Version)) //...and we're having the same version, join the game
						{
							WriteErrorLog("X) Connecting to Server...");
							RemoveAllObjects();
							Network_EstablishConnection(Servers[menuchoice+menuoffset].Adress);
							WriteErrorLog("X) Connection established!");
						}

					}
					if ((keys[27]) && (reloadTime <= 0))
					{
						reloadTime=(int)(5.0f * g_ispeedfactor);
						JoinMenuActive=FXFALSE;
					}
					if (numServers>0)
					if ((keys[VK_DOWN]) && (reloadTime <= 0))
					{
						PlaySoundFX(31,31);
						reloadTime=(int)(5.0f * g_ispeedfactor);
						menuchoice++;
					}
					if (numServers>0)
					if ((keys[VK_UP]) && (reloadTime <= 0))
					{
						PlaySoundFX(31,31);
						reloadTime=(int)(5.0f * g_ispeedfactor);
						menuchoice--;
					}
					if (menuchoice<0)
					{
						menuchoice=0;
						if (menuoffset>0)
							menuoffset--;
					}
					if (menuchoice>(numServers-1))
					{
						menuchoice=(numServers-1);
					}
					if (menuchoice>21)
					{
						menuchoice=21;
						if (menuoffset<(numServers-22))
							menuoffset++;
					}
				}
//------------------------------------



	if (frames==0)
		return;


//============================================================================
// Camera movement
//============================================================================
				
// Convert the pitchTime and rollTime variables into actual degrees

				tempfloat = xpitchTime;
				while (tempfloat < 0.0f) {
					tempfloat += 360.0f;
				}
				while (tempfloat >= 360.0f) {
					tempfloat -= 360.0f;
				}
				ControlPitch+=dDelta*Sin[(int)(tempfloat*Deg)]*15.0f;

				if ((ControlPitch>90) && (ControlPitch<270))
				{
					tempfloat = yawTime;
					ControlRoll=-dDelta*tempfloat;
					while (tempfloat < 0.0f) {
						tempfloat += 360.0f;
					}
					while (tempfloat >= 360.0f) {
						tempfloat -= 360.0f;
					}
					ControlYaw-=dDelta*Sin[(int)(tempfloat*Deg)]*15.0f;
				}
				else
				{
					tempfloat = yawTime;
					ControlRoll=-dDelta*tempfloat;
					while (tempfloat < 0.0f) {
						tempfloat += 360.0f;
					}
					while (tempfloat >= 360.0f) {
						tempfloat -= 360.0f;
					}
					ControlYaw+=dDelta*Sin[(int)(tempfloat*Deg)]*15.0f;
				}

	// Normalize degrees

	while (ControlPitch < 0.0f) {
		ControlPitch += 360.0f;
	}
	while (ControlPitch >= 360.0f) {
		ControlPitch -= 360.0f;
	}
	while (ControlYaw < 0.0f) {
		ControlYaw += 360.0f;
	}
	while (ControlYaw >= 360.0f) {
		ControlYaw -= 360.0f;
	}
	while (ControlRoll < 0.0f) {
		ControlRoll += 360.0f;
	}
	while (ControlRoll >= 360.0f) {
		ControlRoll -= 360.0f;
	}




// Update position according to rotation
	ControlHeight-=(float)(sin(ControlPitch * DEGREE)* (mySpeed/100.0f)*scalefactor * g_speedfactor);

	ControlXPos+=(float)(sin(ControlYaw * DEGREE)*cos(ControlPitch * DEGREE) * (mySpeed/100.0f)*scalefactor * g_speedfactor);
	ControlYPos-=(float)(cos(ControlYaw * DEGREE)*cos(ControlPitch * DEGREE) * (mySpeed/100.0f)*scalefactor * g_speedfactor);

	xGridPos = (int)(ControlXPos/scalefactor);
	zGridPos = (int)(ControlYPos/scalefactor);

	Objects[Player[localplayer].ControlledObject].xGrid = (int)(Objects[Player[localplayer].ControlledObject].xPos/scalefactor);
	Objects[Player[localplayer].ControlledObject].zGrid = (int)(Objects[Player[localplayer].ControlledObject].zPos/scalefactor);


// What? The player tries to leave the arena? He won't do that.
// This code undoes the latest movement in case the camera has collided
// with the landscape or left the area boundaries.

		if ((xGridPos < -30) || (xGridPos > 27))
		{
			ControlXPos-=(float)(sin(ControlYaw * DEGREE)*cos(ControlPitch * DEGREE) * (mySpeed/100.0f)*scalefactor * g_speedfactor);
			Objects[Player[localplayer].ControlledObject].xPos = ControlXPos;
		}
		if ((zGridPos < -26) || (zGridPos > 29))
		{
			ControlYPos+=(float)(cos(ControlYaw * DEGREE)*cos(ControlPitch * DEGREE) * (mySpeed/100.0f)*scalefactor * g_speedfactor);
			Objects[Player[localplayer].ControlledObject].zPos = ControlYPos;
		}

		i = DetectCollision(&ControlXPos, &ControlHeight, &ControlYPos, 0.0f);
		if ((i==1) && (!Cheat))
		{
			//On crash, kill a diving player
			if (Objects[Player[localplayer].ControlledObject].isDiving)
			{
				StopSoundFX(18);
				StopSoundFX(19);
				Objects[Player[localplayer].ControlledObject].isDiving=FXFALSE;
				Objects[Player[localplayer].ControlledObject].TimeToLive=2;
				KillShip(Player[localplayer].ControlledObject);
			}
			else
			{
				// Else we're just too dumb to not fly into the ground for no reason...
				DamageObject(Player[localplayer].ControlledObject, DAMAGE_MISSILE, FXFALSE, FXTRUE);
			}
		}



	  glFlush();						// flush buffers

	  //Screenshot code. I know it's odd, but it has to be used AFTER glFlush
	  if (keys['O'])
		Screenshot(winWidth, winHeight);

	  SwapBuffers( hDC ); 

	  AmbientSoundHandler();

	  listenerpos[0] = CamXPos;
	  listenerpos[1] = -CamHeight;
	  listenerpos[2] = CamZPos;

	  NoiseEmitterHandler();		// Looping 3d noises of ships, missiles, buildings and stuff.

	  FSOUND_3D_Listener_SetAttributes(listenerpos, NULL, 
										 (float)(sin(CamYaw * DEGREE)*cos(CamPitch * DEGREE)), 
										-(float)(sin(CamPitch * DEGREE)),
										-(float)(cos(CamYaw * DEGREE)*cos(CamPitch * DEGREE)),
										 (float)(sin(CamYaw * DEGREE)*cos((CamPitch+90.0f)* DEGREE)), 
										-(float)(sin((CamPitch+90.0f) * DEGREE)),
										-(float)(cos(CamYaw * DEGREE)*cos((CamPitch+90.0f) * DEGREE)) );

	  FSOUND_3D_Update();			// Needed to update 3d engine, once per frame

	  endFrameTime = TimerGetTime();
	  // If the frametime exceeds normal values, offer a way to quit
	  if ((endFrameTime - startFrameTime) > 4000000)
	  {
		WriteErrorLog("WOOF! Watchdog reports frametime exceeding maximum value. Possible program hang.");
		if (MessageBox(NULL,"Your frametime appears to be abnormally long.\nIf you just switched tasks or did something CPU consuming, disregard this message.\nIf not, the game *might* have crashed.\nPress YES to exit, or NO to try again.","Game crashed?",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
		{
			WriteErrorLog("Used emergency exit on request!");
			return;
		}
		else
			WriteErrorLog("Emergency exit NOT used despite excessive frametime!");
	  }




    }
    
//============================================================================
// End of MAIN GAME LOOP
//============================================================================

WriteErrorLog("X) Exiting game loop");

return;

}








/*

  "Am I dead?"

  -"Far from it."

*/

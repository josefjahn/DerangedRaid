#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <stdlib.h>
#include <stdarg.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include "drnetwork.h"
#include "derangedraid.h"

#include "glide.h"

#if (GLIDE_PLATFORM & GLIDE_HW_CVG)
#include <cvg.h>
#else
#endif

#ifdef __DJGPP__
#include <crt0.h>
int crt0_startup_flags = _CRT0_FLAG_NONMOVE_SBRK;
#endif

#include "tlib.h"

static FxBool okToRender = FXTRUE;

#ifndef M_PI
#define M_PI    3.141592654f
#define M_PIx2  6.283185308f
#define M_PId2  1.570796327f
#endif

const float Deg = 91.522222f;

//int winWidth = 800, winHeight = 600; 


int RestartProgram;
int winWidth, winHeight;
int SCREEN_COLORBITS;
int SCREEN_DEPTHBITS;
int SCREEN_MODE;
int SCREEN_FULLSCREEN;
int SCREEN_TERRAINTEXTURES;
int SCREEN_LOD;
int SCREEN_WIREFRAME;
int SCREEN_FOG;
int SCREEN_LORES;
int SOUND_ENABLED;
int MOUSE_INVERT;

int LeftMouseButton;
int RightMouseButton;
int mouse_x, mouse_y;
int AlreadyUninit=0;


DEVMODE dmode; // This is the display mode that we want to change to.
DEVMODE old_dmode; // Obtain a copy of the original settings.
LONG changeResult;
BOOL modeswitch;
BOOL fullscreen;
FxBool useGlide=FXFALSE;
LONG result;

HDC hDC;
HGLRC hRC;
HFONT hFont;
GLuint	base;
HINSTANCE  hInstance;

extern void initglide(void);
extern void DRCloseSound(void);
extern void DRStartSound(HWND hWnd);

extern int frames;

void WriteCrashLog(char *msg)
{
	FILE *DataFile = NULL;

	DataFile = fopen("DerangedRaid.log","a");
	if (DataFile == NULL)
		return;
	fprintf(DataFile,"%s\n",msg);
	fclose(DataFile);
}

void WriteCFG()
{
	FILE *DataFile = NULL;
//	char  line[80];

	DataFile = fopen("DerangedRaid.cfg","w");
	if (DataFile == NULL)
		return;

	fprintf(DataFile,"SERVER_NAME %s\n",myServer.Name);
	fprintf(DataFile,"PLAYER_NAME %s\n",myClientName);
	fprintf(DataFile,"SCREEN_MODE %i\n",SCREEN_MODE);
	fprintf(DataFile,"SCREEN_COLORBITS %i\n",SCREEN_COLORBITS);
	fprintf(DataFile,"SCREEN_DEPTHBITS %i\n",SCREEN_DEPTHBITS);
	fprintf(DataFile,"SCREEN_FULLSCREEN %i\n",SCREEN_FULLSCREEN);
	fprintf(DataFile,"SCREEN_TERRAINTEXTURES %i\n",SCREEN_TERRAINTEXTURES);
	fprintf(DataFile,"SCREEN_LOD %i\n",SCREEN_LOD);
	fprintf(DataFile,"SCREEN_FOG %i\n",SCREEN_FOG);
	fprintf(DataFile,"SCREEN_LORES %i\n",SCREEN_LORES);
	fprintf(DataFile,"SCREEN_WIREFRAME %i\n",SCREEN_WIREFRAME);
	fprintf(DataFile,"SOUND_ENABLED %i\n",SOUND_ENABLED);
	fprintf(DataFile,"MOUSE_INVERT %i\n",MOUSE_INVERT);
	if (CompatibilityMode)
		fprintf(DataFile,"COMPATIBILITY_MODE 1\n");
	else
		fprintf(DataFile,"COMPATIBILITY_MODE 0\n");

	if (DataFile != NULL)
		fclose(DataFile);
}


void LoadCFG()
{
	FILE *DataFile = NULL;
	char  line[80];
	char key[256];
	char key2[256];
	int value;
	int loadedparams=0;

	DataFile = fopen("DerangedRaid.cfg","r");
	if (DataFile == NULL)
		return;

	while (loadedparams < 14)
	{
		if (fgets(line,80,DataFile) == NULL)
		{
			fclose(DataFile);
			return;
		}
		sprintf(key, " ");
		sprintf(key2, " ");
		sscanf(line,"%s", key);
		if (key!=NULL)
		{
			sscanf(line,"%s %s", key, key2);
			if (strcmp(key, "SERVER_NAME")==0)
			{
				sscanf(line,"%s %s", key, key2);
				sprintf(myServer.Name, "%s", key2);
				loadedparams++;
			}
			if (strcmp(key, "PLAYER_NAME")==0)
			{
				sscanf(line,"%s %s", key, key2);
				sprintf(myClientName, "%s", key2);
				loadedparams++;
			}
			if (strcmp(key, "SCREEN_MODE")==0)
			{
				sscanf(line,"%s %i", key, &value);
				SCREEN_MODE=value;
				loadedparams++;
			}
			if (strcmp(key, "SCREEN_COLORBITS")==0)
			{
				sscanf(line,"%s %i", key, &value);
				SCREEN_COLORBITS=value;
				loadedparams++;
			}
			if (strcmp(key, "SCREEN_DEPTHBITS")==0)
			{
				sscanf(line,"%s %i", key, &value);
				SCREEN_DEPTHBITS=value;
				loadedparams++;
			}
			if (strcmp(key, "SCREEN_FULLSCREEN")==0)
			{
				sscanf(line,"%s %i", key, &value);
				SCREEN_FULLSCREEN=value;
				loadedparams++;
			}
			if (strcmp(key, "SCREEN_TERRAINTEXTURES")==0)
			{
				sscanf(line,"%s %i", key, &value);
				SCREEN_TERRAINTEXTURES=value;
				loadedparams++;
			}
			if (strcmp(key, "SCREEN_LOD")==0)
			{
				sscanf(line,"%s %i", key, &value);
				SCREEN_LOD=value;
				loadedparams++;
			}
			if (strcmp(key, "SCREEN_FOG")==0)
			{
				sscanf(line,"%s %i", key, &value);
				SCREEN_FOG=value;
				loadedparams++;
			}
			if (strcmp(key, "SCREEN_LORES")==0)
			{
				sscanf(line,"%s %i", key, &value);
				SCREEN_LORES=value;
				loadedparams++;
			}
			if (strcmp(key, "SCREEN_WIREFRAME")==0)
			{
				sscanf(line,"%s %i", key, &value);
				SCREEN_WIREFRAME=value;
				loadedparams++;
			}
			if (strcmp(key, "SOUND_ENABLED")==0)
			{
				sscanf(line,"%s %i", key, &value);
				SOUND_ENABLED=value;
				loadedparams++;
			}
			if (strcmp(key, "MOUSE_INVERT")==0)
			{
				sscanf(line,"%s %i", key, &value);
				MOUSE_INVERT=value;
				loadedparams++;
			}
			if (strcmp(key, "COMPATIBILITY_MODE")==0)
			{
				sscanf(line,"%s %i", key, &value);
				if (value==1)
					CompatibilityMode = FXTRUE;
				else
					CompatibilityMode = FXFALSE;
				loadedparams++;
			}
		}
	}
	
	if (DataFile != NULL)
		fclose(DataFile);
}



void SetDefaultCFG()
{
	sprintf(myServer.Name, "Another DR Server");
	sprintf(myClientName, "Player");
	SCREEN_COLORBITS=32;
	SCREEN_DEPTHBITS=24;
	SCREEN_MODE=0;
	SCREEN_FULLSCREEN=0;
	SCREEN_TERRAINTEXTURES=1;
	SCREEN_LOD=8;
	SCREEN_WIREFRAME=0;
	SCREEN_FOG=1;
	SCREEN_LORES=1;
	SOUND_ENABLED=1;
	MOUSE_INVERT=1;
}














GLvoid BuildFont(GLvoid)								// Build Our Bitmap Font
{
	HFONT	font;										// Windows Font ID

	base = glGenLists(96);								// Storage For 96 Characters

	font = CreateFont(	-12,							// Height Of Font
						0,								// Width Of Font
						0,								// Angle Of Escapement
						0,								// Orientation Angle
//						FW_BOLD,						// Font Weight
0,						FALSE,							// Italic
						FALSE,							// Underline
						FALSE,							// Strikeout
						ANSI_CHARSET,					// Character Set Identifier
						OUT_TT_PRECIS,					// Output Precision
						CLIP_DEFAULT_PRECIS,			// Clipping Precision
						ANTIALIASED_QUALITY,			// Output Quality
						FF_DONTCARE|DEFAULT_PITCH,		// Family And Pitch
						"Courier New");					// Font Name

	SelectObject(hDC, font);							// Selects The Font We Want

	wglUseFontBitmaps(hDC, 32, 96, base);				// Builds 96 Characters Starting At Character 32
}

GLvoid KillFont(GLvoid)									// Delete The Font
{
	glDeleteLists(base, 96);							// Delete All 96 Characters
}

GLvoid glPrint(char *fmt, float x, float y, float r, float g, float b)					// Custom GL "Print" Routine
{
	char		text[256];								// Holds Our String
	va_list		ap;										// Pointer To List Of Arguments

	if (fmt == NULL)									// If There's No Text
		return;											// Do Nothing

	va_start(ap, fmt);									// Parses The String For Variables
	    vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
	va_end(ap);											// Results Are Stored In Text

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glLoadIdentity();									// Reset The Current Modelview Matrix
	glTranslatef(0.0f,0.0f,-1.0f);						// Move One Unit Into The Screen
	glColor4f(r,g,b,0.9f);
	glRasterPos2f(x, y);

	glPushAttrib(GL_LIST_BIT);							// Pushes The Display List Bits
	glListBase(base - 32);								// Sets The Base Character to 32
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	// Draws The Display List Text
	glPopAttrib();										// Pops The Display List Bits
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

}

GLvoid glFastPrint(char *fmt, float x, float y, float r, float g, float b)					// Custom GL "Print" Routine
{
	char		text[256];								// Holds Our String
	va_list		ap;										// Pointer To List Of Arguments

	if (fmt == NULL)									// If There's No Text
		return;											// Do Nothing

	va_start(ap, fmt);									// Parses The String For Variables
	    vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
	va_end(ap);											// Results Are Stored In Text

	glColor4f(r,g,b,0.9f);
	glRasterPos2f(x, y);

	glPushAttrib(GL_LIST_BIT);							// Pushes The Display List Bits
	glListBase(base - 32);								// Sets The Base Character to 32
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);	// Draws The Display List Text
	glPopAttrib();										// Pops The Display List Bits

}


static void
initializeFont(HDC hDC)
{
#if 0
#define FONTNAME "-*-fixed-medium-r-normal--10-*-*-*-c-60-iso8859-1"
#define FONTBASE 0x1000

    XFontStruct *fontStruct;
    int firstRow, lastRow, rows;

    if ((fontStruct = XLoadQueryFont(dpy, FONTNAME)) == NULL) {
	fprintf(stderr, "font %s not found\n", FONTNAME);
	exit(EXIT_FAILURE);
    }
    firstRow = fontStruct->min_byte1;
    lastRow = fontStruct->max_byte1;
    rows = lastRow - firstRow + 1;
    
    glXUseXFont(fontStruct->fid, firstRow<<8, rows*256, FONTBASE);
    glListBase(FONTBASE);
#else
#define FONTBASE 0x1000

//    hFont = GetStockObject(SYSTEM_FONT);
/*	hFont = CreateFont(h, w, esc, orient, weight,
		ital, under, strike, set, out, clip, qual, pitch/fam, face);
    */

		hFont = CreateFont(12, 0, 0, 0, FW_NORMAL,
		FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DRAFT_QUALITY,
		FIXED_PITCH | FF_MODERN, "Arial");
	
    if (!hFont) {
	MessageBox(WindowFromDC(hDC),
		"Failed to find acceptable bitmap font.",
		"OpenGL Application Error",
		MB_ICONERROR | MB_OK);
	exit(1);
    }
    (void) SelectObject(hDC, hFont);
    wglUseFontBitmaps(hDC, 0, 255, FONTBASE);
    glListBase(FONTBASE);
#endif
}


























void SetIntelPrecision(){
    int memvar;
	//taken directly from the Glide 2.0 programming Guide
	//dunno how or if this works but here it is anyway
	_asm {
		finit
		fwait
		fstcw memvar
		fwait
		mov eax,memvar
		and eax,0xfffffcff
		mov memvar,eax
		fldcw memvar
		fwait
	}
	memvar=0;
}


FxBool
tlOkToRender()
{
  return okToRender;
} /* tlOkToRender */

/*-------------------------------------------------------------------
  Function:tlGetOpt
  Date: 2/26
  Implementor(s): jdt
  Library: Test Library
  Description:
  Incrementally search an argument list for matches.
  Arguments:
  argc    - first argument to main
  argv    - second argument to main
  tags    - string of non-whitespace characters to be search for in the 
            argument string
  match   - pointer to storage for matched character 
  remArgs - pointer to storage for remaining arglist after any match
  Return:
   1 if matched
   0 if no more arguments
  -1 if unrecognized
  -------------------------------------------------------------------*/
int tlGetOpt( int argc, char *argv[], 
              const char *tags, char *match, 
              char **remArgs[] ){
    static int firstCall;
    static int lastArg;
    
    int rv = 0;

    if ( !firstCall ) {
        lastArg   = 1;
        firstCall = 1;
    } 

    while( (lastArg<argc) && (*argv[lastArg] != '-') ) lastArg++;

    if ( lastArg < argc ) {
        unsigned tag;
        for( tag = 0; tag < strlen( tags ); tag++ ) {
            if ( *(argv[lastArg]+1) == tags[tag] ) {
                *match = tags[tag];
                *remArgs = &argv[++lastArg];
                rv = 2;
                break;
            }
        }
        rv--;
    }
    return rv;
}

/*-------------------------------------------------------------------
  Function: tlGetResolution
  Date: 2/25
  Implementor(s): jdt
  Library: Test Library
  Description:
  Get the integer representing the provided resolution and return the
  necessary floats for screen scaling.
  Arguments:
  identifier - string identifying resolution of the form XRSxYRS
  width - pointer to float storage for width
  height - pointer to float storage for height
  Return:
  int
  -------------------------------------------------------------------*/

typedef struct {
    const char *id;
    int         res;
    float       width;
    float       height;
} ResToRes;

static ResToRes resTable[] = {
    { "320x200", GR_RESOLUTION_320x200, 320.0f, 200.0f },
    { "320x240", GR_RESOLUTION_320x240, 320.0f, 240.0f },
    { "512x256", GR_RESOLUTION_512x256, 512.0f, 256.0f },
    { "512x384", GR_RESOLUTION_512x384, 512.0f, 384.0f },
    { "640x400", GR_RESOLUTION_640x400, 640.0f, 400.0f },
    { "640x480", GR_RESOLUTION_640x480, 640.0f, 480.0f },
    { "800x600", GR_RESOLUTION_800x600, 800.0f, 600.0f },
    { "856x480", GR_RESOLUTION_856x480, 856.0f, 480.0f },
    { "960x720", GR_RESOLUTION_960x720, 960.0f, 720.0f }
#ifdef H3D
   ,{ "640x240", GR_RESOLUTION_640x240_AUTOFLIPPED, 640.0f, 240.0f },
    { "800x300", GR_RESOLUTION_800x300_AUTOFLIPPED, 800.0f, 300.0f }
#endif
};

static long resTableSize = sizeof( resTable ) / sizeof( ResToRes );

void
tlGetDimsByConst(const int resConst, float *w, float *h)
{
  int match;
  
  for( match = 0; match < resTableSize; match++ ) {
    if ( resTable[match].res == resConst ){
      *w  = resTable[match].width;
      *h = resTable[match].height;
      return;
    }
  }
#ifdef __WIN32__
  {
    extern void getWindowSize(float *width, float *height);
    getWindowSize(w, h);
  }
#endif
} /* tlGetDimsByConst */

/*-------------------------------------------------------------------
  Function: tlGetReslutionConstant
  Date: 2/28
  Implementor(s): jdt
  Library: Test Library
  Description:
  Returns the glide constant for a command line resolution argument
  Arguments:
  identifier - command line resolution arg.
  width - storage for floating point screen width
  height - storaget for floating piont screen height
  Return:
  GrResolution_t matching resolution argument
  -------------------------------------------------------------------*/
static int Res = GR_RESOLUTION_640x480;
int 
tlGetResolutionConstant( const char *identifier, float *width, 
                        float *height ) 
{
  int match;
  
  for( match = 0; match < resTableSize; match++ ) {
    if ( !strcmp( identifier, resTable[match].id ) ) {
      Res = resTable[match].res;
      *width  = resTable[match].width;
      *height = resTable[match].height;
      return Res;
    }
  }
#ifdef __WIN32__
  {
    extern void getWindowSize(float *width, float *height);

    getWindowSize(width, height);
  }
#endif
  return Res = GR_RESOLUTION_NONE;
}

/*-------------------------------------------------------------------
  Function: tlGetResolutionString
  Date: 2/28
  Implementor(s): jdt
  Library: Test LIbrary
  Description:
  Returns a string value for a given resolution constant
  Arguments:
  res - resolution constant
  Return:
  string representing resolution
  -------------------------------------------------------------------*/
const char *tlGetResolutionString( int res ) {
    int match;
    const char *str = "unknown";

    for( match = 0; match < resTableSize; match++ ) {
        if ( resTable[match].res == res ) {
            str = resTable[match].id;
            break;
        }
    }
    return str;
}


/*-------------------------------------------------------------------
  Function: tlSetScreen
  Date: 2/28
  Implementor(s): jdt
  Library: Test Library
  Description:
  Set up screen scaling
  Arguments:
  width - width of screen
  height - height of screen
  Return:
  none
  -------------------------------------------------------------------*/
static float scrXScale;
static float scrYScale;

void tlSetScreen( float width, float height ) {
    scrXScale = width;
    scrYScale = height;
    return;
}

/*-------------------------------------------------------------------
  Function: tlScaleX
  Date: 2/28
  Implementor(s): jdt
  Library: Test Lib
  Description:
  Scale X coordinates from normalized device coordinates [ 0.0, 1.0 )
  to Screen Coordinates [ 0.0, WidthOfScreenInPixels )
  Arguments:
  coord - x coordinate to scale
  Return:
  scaled x coordinate
  -------------------------------------------------------------------*/
float tlScaleX( float coord ) {
    return coord * scrXScale;
}

/*-------------------------------------------------------------------
  Function: tlScaleY
  Date: 2/28
  Implementor(s): jdt
  Library: Test Lib
  Description:
  Scale Y coordinates from normalized device coordinates [ 0.0, 1.0 )
  to Screen Coordinates [ 0.0, HeightOfScreenInPixels )
  Arguments:
  coord - y coordinate to scale
  Return:
  scaled y coordinate
  -------------------------------------------------------------------*/
float tlScaleY( float coord ) {
    return coord *scrYScale;
}


/*-------------------------------------------------------------------
  Function: tlSetConsole
  Date:
  Implementor(s):
  Library:
  Description:
  Arguments:
  Return:
  -------------------------------------------------------------------*/
static unsigned char fontTable[128][2];
static GrState state;
static GrTexInfo fontInfo;
static unsigned long fontAddress;
static const char fontString[] = "ABCDEFGHIJKLMN"
                                 "OPQRSTUVWXYZ01"
                                 "23456789.,;:*-"
                                 "+/_()<>|[]{}! ";

static const int fontWidth    = 9*2;
static const int fontHeight   = 12*2;
static const int charsPerLine = 14;

static int fontInitialized;

static void grabTex( FxU32 addr, void *storage );
static void putTex( FxU32 addr, void *storage );
static void consoleScroll( void );
static void drawChar( char character, float x, float y, float w, float h );

#include "tldata.inc"

/*-------------------------------------------------------------------
  Function: tlSleep
  Date: 3/1
  Implementor(s): jdt
  Library: Test Library
  Description:
  Block for a number of seconds 
  Arguments:
  seconds - number of seconds before function returns
  Return:
  none
  -------------------------------------------------------------------*/
void tlSleep( int seconds ) {
    time_t time0 = time( 0 );
    while( (time(0)-time0) < seconds );
}

/*-------------------------------------------------------------------
  Function: tlIdentity
  Date: 3/3
  Implementor(s): jdt
  Library: Test Library
  Description: 
  Return an identity matrix
  Arguments:
  none
  Return:
  const pointer to identity matrix
  -------------------------------------------------------------------*/
static TlMatrix currentMatrix;

#define DEGREE (.01745328f)

const float *tlIdentity( void ) {
    static TlMatrix m;
    m[0][0] = 1.0f, m[0][1] = 0.0f, m[0][2] = 0.0f, m[0][3] = 0.0f;
    m[1][0] = 0.0f, m[1][1] = 1.0f, m[1][2] = 0.0f, m[1][3] = 0.0f;
    m[2][0] = 0.0f, m[2][1] = 0.0f, m[2][2] = 1.0f, m[2][3] = 0.0f;
    m[3][0] = 0.0f, m[3][1] = 0.0f, m[3][2] = 0.0f, m[3][3] = 1.0f;
    return &m[0][0];
}

/*-------------------------------------------------------------------
  Function: tlXRotation
  Date: 3/3
  Implementor(s): jdt
  Library: Test Library
  Description:
  Generate a rotation about the x axis
  Arguments:
  degrees - number of degrees to rotate
  Return:
  const point to rotation matrix
  -------------------------------------------------------------------*/
const float *tlXRotation( float degrees ) {
    static TlMatrix m;
    float c = (float)cos( degrees * DEGREE);
    float s = (float)sin( degrees * DEGREE);
/*	float c, s;
	while (degrees < 0.0f) {
		degrees += 360.0f;
	}
	while (degrees > 359.0f) {
		degrees -= 360.0f;
	}
    c = (float)Cos[(int)(degrees * Deg)];
    s = (float)Sin[(int)(degrees * Deg)];
*/
    m[0][0] = 1.0f, m[0][1] = 0.0f, m[0][2] = 0.0f, m[0][3] = 0.0f;
    m[1][0] = 0.0f, m[1][1] =    c, m[1][2] =    s, m[1][3] = 0.0f;
    m[2][0] = 0.0f, m[2][1] =   -s, m[2][2] =    c, m[2][3] = 0.0f;
    m[3][0] = 0.0f, m[3][1] = 0.0f, m[3][2] = 0.0f, m[3][3] = 1.0f;
    return &m[0][0];
}


/*-------------------------------------------------------------------
  Function: tlYRotation
  Date: 3/3
  Implementor(s): jdt
  Library: Test Library
  Description:
  Generate a rotation about the y axis
  Arguments:
  degrees - number of degrees to rotate
  Return:
  const point to rotation matrix
  -------------------------------------------------------------------*/
const float *tlYRotation( float degrees ) {
    static TlMatrix m;
    float c = (float)cos( degrees * DEGREE);
    float s = (float)sin( degrees * DEGREE);
/*	float c, s;
	while (degrees < 0.0f) {
		degrees += 360.0f;
	}
	while (degrees > 359.0f) {
		degrees -= 360.0f;
	}
    c = (float)Cos[(int)(degrees * Deg)];
    s = (float)Sin[(int)(degrees * Deg)];
*/
    m[0][0] =    c, m[0][1] = 0.0f, m[0][2] =   -s, m[0][3] = 0.0f;
    m[1][0] = 0.0f, m[1][1] = 1.0f, m[1][2] = 0.0f, m[1][3] = 0.0f;
    m[2][0] =    s, m[2][1] = 0.0f, m[2][2] =    c, m[2][3] = 0.0f;
    m[3][0] = 0.0f, m[3][1] = 0.0f, m[3][2] = 0.0f, m[3][3] = 1.0f;
    return &m[0][0];
}


/*-------------------------------------------------------------------
  Function: tlZRotation
  Date: 3/3
  Implementor(s): jdt
  Library: Test Library
  Description:
  Generate about the z axis
  Arguments:
  degrees - number of degrees to rotate
  Return:
  const point to rotation matrix
  -------------------------------------------------------------------*/
const float *tlZRotation( float degrees ) {
    static TlMatrix m;
    float c = (float)cos( degrees * DEGREE);
    float s = (float)sin( degrees * DEGREE);
/*	float c, s;
	while (degrees < 0.0f) {
		degrees += 360.0f;
	}
	while (degrees > 359.0f) {
		degrees -= 360.0f;
	}
    c = (float)Cos[(int)(degrees * Deg)];
    s = (float)Sin[(int)(degrees * Deg)];
*/
    m[0][0] =    c, m[0][1] =    s, m[0][2] = 0.0f, m[0][3] = 0.0f;
    m[1][0] =   -s, m[1][1] =    c, m[1][2] = 0.0f, m[1][3] = 0.0f;
    m[2][0] = 0.0f, m[2][1] = 0.0f, m[2][2] = 1.0f, m[2][3] = 0.0f;
    m[3][0] = 0.0f, m[3][1] = 0.0f, m[3][2] = 0.0f, m[3][3] = 1.0f;
    return &m[0][0];
}

/*-------------------------------------------------------------------
  Function: tlTranslation
  Date: 3/3
  Implementor(s): jdt
  Library: Test Library
  Description:
  Generate a translation matrix
  Arguments:
  x, y, z - offsets to translate origin
  Return:
  const point to translation matrix
  -------------------------------------------------------------------*/
const float *tlTranslation( float x, float y, float z ) {
    static TlMatrix m;
    m[0][0] = 1.0f, m[0][1] = 0.0f, m[0][2] = 0.0f, m[0][3] = 0.0f;
    m[1][0] = 0.0f, m[1][1] = 1.0f, m[1][2] = 0.0f, m[1][3] = 0.0f;
    m[2][0] = 0.0f, m[2][1] = 0.0f, m[2][2] = 1.0f, m[2][3] = 0.0f;
    m[3][0] =    x, m[3][1] =    y, m[3][2] =    z, m[3][3] = 1.0f;
    return &m[0][0];
}

/*-------------------------------------------------------------------
  Function: tlSetMatrix
  Date: 3/3
  Implementor(s): jdt
  Library: Test Library
  Description:
  Set the current matrix.  This matrix translates the object into
  View space from local coordiantes during calls to transformVertices
  All spaces are considered to by -1.0->1.0 normalized.
  Arguments:
  m - pointer to matrix
  Return:
  none
  -------------------------------------------------------------------*/
void tlSetMatrix( const float *m ) {
    memcpy( currentMatrix, m, sizeof( TlMatrix ) );
    return;
}

/*-------------------------------------------------------------------
  Function: tlMultMatrix
  Date: 3/3
  Implementor(s): jdt
  Library: Test Library
  Description:
  Multiply the current matrix by the provided matrix
  Arguments:
  matrix to post-cat to the current matrix
  Return:
  none
  -------------------------------------------------------------------*/
void tlMultMatrix( const float *m ) {
    TlMatrix result;
    TlMatrix mat;
    int i, j;

    memcpy( mat, m, sizeof( TlMatrix ) );

    for( j = 0; j < 4; j++ ) {
        for( i = 0; i < 4; i++ ) {
            result[j][i] = 
                currentMatrix[j][0] * mat[0][i] +
                currentMatrix[j][1] * mat[1][i] +
                currentMatrix[j][2] * mat[2][i] +
                currentMatrix[j][3] * mat[3][i];
        }
    }
    memcpy( currentMatrix, result, sizeof( TlMatrix ) );
    
}

/*-------------------------------------------------------------------
  Function: tlTransformVertices
  Date: 3/3
  Implementor(s): jdt
  Library: Test Library
  Description:
  Transform a list of vertices from model space into view space
  Arguments:
  dstVerts - memory to store transformed vertices
  srcVerts - array of vertices to be transformed
  length - number of vertices to transform
  Return:
  none
  -------------------------------------------------------------------*/
void tlTransformVertices( TlVertex3D *dstVerts, TlVertex3D *srcVerts, unsigned length ) {
    TlVertex3D tmp, v;
    TlMatrix m;
    unsigned i;

    memcpy( m, currentMatrix, sizeof( TlMatrix ) );
    for( i = 0; i < length; i++ ) {
        v = srcVerts[i];
        tmp = v;
//		tmp.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + v.w * m[3][0];
//		tmp.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + v.w * m[3][1];
//		tmp.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + v.w * m[3][2];
//		tmp.w = v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + v.w * m[3][3];
        tmp.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + m[3][0];
        tmp.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + m[3][1];
        tmp.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + m[3][2];

        dstVerts[i] = tmp;
    }
    return;
}

/*-------------------------------------------------------------------
  Function: tlProjectVertices
  Date: 3/3
  Implementor(s): jdt
  Library: Test Library
  Description:
  perspective project a set of vertices into normalized 2D space (0,1)
  Arguments:
  dstVerts - memory to store projected vertices
  srcVerts - array of vertices to be transformed
  length - number of vertices to transform
  Return:
  none
  -------------------------------------------------------------------*/
#define VP_OFFSET 1.0f
#define VP_SCALE  0.5f

void tlProjectVertices( TlVertex3D *dstVerts, TlVertex3D *srcVerts, unsigned length ) {
    TlVertex3D tmp, v;
    TlMatrix m;
    unsigned i;

    /* simplified perspective proj matrix assume unit clip volume */
    m[0][0] = 1.0f, m[0][1] = 0.0f, m[0][2] = 0.0f, m[0][3] = 0.0f;
    m[1][0] = 0.0f, m[1][1] = 1.0f, m[1][2] = 0.0f, m[1][3] = 0.0f;
    m[2][0] = 0.0f, m[2][1] = 0.0f, m[2][2] = 1.0f, m[2][3] = 1.0f;
    m[3][0] = 0.0f, m[3][1] = 0.0f, m[3][2] = 0.0f, m[3][3] = 0.0f;

    for( i = 0; i < length; i++ ) {
//		if ((srcVerts[i].x != 0.0f) && (srcVerts[i].y != 0.0f) && (srcVerts[i].x != 0.0f)) {
        v = srcVerts[i];
        tmp = v;
        tmp.x = v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + v.w * m[3][0];
        tmp.y = v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + v.w * m[3][1];
        tmp.z = v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + v.w * m[3][2];
        tmp.w = v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + v.w * m[3][3];
        tmp.x /= tmp.w, tmp.y /= tmp.w, tmp.z /= tmp.w;
        tmp.x += VP_OFFSET, tmp.x *= VP_SCALE;
        tmp.y += VP_OFFSET, tmp.y *= VP_SCALE;
        dstVerts[i] = tmp;
//		}
    }
}







#ifdef  __DOS32__
/*-------------------------------------------------------------------
  Function: tlKbHit
  Date: 2/28
  Implementor(s): jdt
  Library: test library
  Description:
  Returns true if there are pending characters in the input queue
  Arguments:
  none
  Return:
  nonzero if keys in queue
  -------------------------------------------------------------------*/
int  tlKbHit( void ) {
    return kbhit();
}

/*-------------------------------------------------------------------
  Function: tlGetCH
  Date: 2/28
  Implementor(s): jdt
  Library: test library
  Description:
  Returns character from top of input fifo, blocks if fifo is empty
  Arguments:
  none
  Return:
  character
  -------------------------------------------------------------------*/
char tlGetCH( void ) {
    return getch();
}

FxBool
tlErrorMessage( char *err) {
  fprintf(stderr, err);
} /* tlErrorMessage */

#else   /* __WIN32__ */


/* This segment simulates main() for Windows, creates a window, etc. */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/* Forward declarations */
HWND hWndMain;
char ** commandLineToArgv(LPSTR lpCmdLine, int *pArgc);

/*
 * MainWndproc
 *
 * Callback for all Windows messages
 */
static int qhead = 0;
static int qtail = 0;
static int queue[256] = {0};
static RECT rect;
int cxChar;
int cyChar;
BOOL UseKeys;
HDC hdc;
TEXTMETRIC  tm;
extern int frameCount;
BOOL keys[256];

HINSTANCE  g_hinst;                     /* My instance handle */
BOOL       g_fActive;                   /* Am I the active window? */




GLvoid KillGLWindow(HWND hWnd)								// Properly Kill The Window
{
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL
	}
	if (!UnregisterClass("OpenGL",hInstance))			// Are We Able To Unregister Class
	{
//		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}


}


GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}

	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(60.0f,(GLfloat)width/(GLfloat)height,1.0f,5000.0f);
	winWidth = width;
	winHeight = height;

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}


LRESULT CALLBACK MainWndproc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	switch (iMsg) {
	case WM_CREATE:
		WriteCrashLog("WM_CREATE occured");
	    return 0;
/*	case WM_MOVE:
        if (!grSstControl(GR_CONTROL_MOVE)) {
            PostMessage( hWndMain, WM_CLOSE, 0, 0 );
            return 0;
        }
        break; */
    case WM_SIZE:
		ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));
        break;
    case WM_ACTIVATE:
        {
        WORD fActive = LOWORD(wParam);
        BOOL fMinimized = (BOOL) HIWORD(wParam);

        /*   if ( ( fActive == WA_INACTIVE ) || fMinimized ) {
              grSstControl( GR_CONTROL_DEACTIVATE );
           } else {
              grSstControl( GR_CONTROL_ACTIVATE );
           }*/
       }
       break;
    case WM_KILLFOCUS:
		{
//		DRCloseSound();
		}
		return 0;
	case WM_SETFOCUS:
		{
//		DRStartSound(hwnd);
		}
		return 0;
	case WM_KEYDOWN:
   		keys[wParam] = TRUE;
		return 0;
	case WM_KEYUP:
		keys[wParam] = FALSE;
		return 0;
	case WM_DESTROY:
		{
			WriteCrashLog("WM_DESTROY occured");
			if (AlreadyUninit==0)
			{
				UnloadAllTextures();
				DRCloseSound();
				WSACleanup();
				KillFont();
				if (fullscreen)
					ChangeDisplaySettings (&old_dmode, CDS_FULLSCREEN);
				WriteCrashLog("UnInit complete on WM_DESTROY");
				AlreadyUninit=1;
			}
			PostQuitMessage(0);
			exit(0);
		}
		break;
	case WM_CLOSE:
		{
			WriteCrashLog("WM_CLOSE occured");
			frames=0;
			if (AlreadyUninit==0)
			{
				UnloadAllTextures();
				DRCloseSound();
				WSACleanup();
				KillFont();
				if (fullscreen)
					ChangeDisplaySettings (&old_dmode, CDS_FULLSCREEN);
				WriteCrashLog("UnInit complete on WM_CLOSE");
				AlreadyUninit=1;
			}
			PostQuitMessage(0);
			exit(0);
		}
		break;

	case WM_SYSCOMMAND:
		{
		switch (wParam)
		{
			case SC_SCREENSAVE:
			case SC_MONITORPOWER:
			WriteCrashLog("Prevented Screensave / Powersave");
			return 0;
		}
		break;
		}


	case WM_LBUTTONDOWN:
		LeftMouseButton = 1;
		return 0;
	case WM_LBUTTONUP:
		LeftMouseButton = 0;
		return 0;
	case WM_RBUTTONDOWN:
		RightMouseButton = 1;
		return 0;
	case WM_RBUTTONUP:
		RightMouseButton = 0;
		return 0;
		
	case WM_MOUSEMOVE:
		{
			mouse_x = (int)(LOWORD(lParam));
			mouse_y = (int)(HIWORD(lParam));
			
		} return 0;

	}
    return(DefWindowProc(hwnd,iMsg,wParam,lParam));
}


















/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/
 
BOOL CreateGLWindow(HWND hWnd, char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) MainWndproc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}
	
	if (fullscreenflag)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","Question",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreenflag=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if (fullscreenflag)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
		ShowCursor(FALSE);										// Hide Mouse Pointer
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"OpenGL",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow(hWnd);								// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		16,											// 16Bit Stencil Bugger
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow(hWnd);								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow(hWnd);								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow(hWnd);								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow(hWnd);								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow(hWnd);								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

	return TRUE;									// Success
}










void RestartVideo(void)
{

	KillGLWindow(hWndMain);								// Reset The Display

	if (SCREEN_MODE==0)
	{
		winWidth=640;
		winHeight=480;
	}
	if (SCREEN_MODE==1)
	{
		winWidth=800;
		winHeight=600;
	}
	if (SCREEN_MODE==2)
	{
		winWidth=1024;
		winHeight=768;
	}
	if (SCREEN_MODE==3)
	{
		winWidth=1280;
		winHeight=1024;
	}
	if (SCREEN_MODE==4)
	{
		winWidth=1600;
		winHeight=1200;
	}


	if (SCREEN_FULLSCREEN==0)
		fullscreen=FALSE;
	else
		fullscreen=TRUE;

	if (fullscreen)
	{
		char tempstring[128];
		sprintf(tempstring, "Deranged Raid - Build "__DATE__","__TIME__"");

		if (!CreateGLWindow(hWndMain, tempstring, winWidth,winHeight,SCREEN_COLORBITS,TRUE))
			exit(1);
	}
	else
	{
		char tempstring[128];
		sprintf(tempstring, "Deranged Raid - Build "__DATE__","__TIME__"");
		if (!CreateGLWindow(hWndMain, tempstring, winWidth,winHeight,SCREEN_COLORBITS,FALSE))
			exit(1);
	}

}






/*
 * initApplication
 *
 * Do that Windows initialization stuff...
 */

static FxBool
initApplication( HANDLE hInstance, int nCmdShow )
{
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &old_dmode); 

/*	fullscreen=TRUE;

	// Ask The User Which Screen Mode They Prefer
	if (MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?", "Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		fullscreen=FALSE;							// Windowed Mode
	}
*/

	if (SCREEN_MODE==0)
	{
		winWidth=640;
		winHeight=480;
	}
	if (SCREEN_MODE==1)
	{
		winWidth=800;
		winHeight=600;
	}
	if (SCREEN_MODE==2)
	{
		winWidth=1024;
		winHeight=768;
	}
	if (SCREEN_MODE==3)
	{
		winWidth=1280;
		winHeight=1024;
	}
	if (SCREEN_MODE==4)
	{
		winWidth=1600;
		winHeight=1200;
	}


	if (SCREEN_FULLSCREEN==0)
		fullscreen=FALSE;
	else
		fullscreen=TRUE;

	if (fullscreen)
	{
		char tempstring[128];
		sprintf(tempstring, "Deranged Raid - Build "__DATE__","__TIME__"");

		if (!CreateGLWindow(hWndMain, tempstring, winWidth,winHeight,SCREEN_COLORBITS,TRUE))
			exit(1);
	}
	else
	{
		char tempstring[128];
		sprintf(tempstring, "Deranged Raid - Build "__DATE__","__TIME__"");
		if (!CreateGLWindow(hWndMain, tempstring, winWidth,winHeight,SCREEN_COLORBITS,FALSE))
			exit(1);
	}

  BuildFont();  

  return TRUE;
  
} /* initApplication */






/*
 * WinMain
 */
int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, 
    LPSTR lpCmdLine, int nCmdShow )
{
	int     argc;
    char    **argv;
    extern int main(int argc, char **argv, HDC hDC, HWND hWnd);

	WriteCrashLog("\n-------------------------------------------------------------\nGAME STARTUP");
	
	RestartProgram = 1;

	while (RestartProgram!=0)
	{

		RestartProgram=0;

		SCREEN_MODE=-1;
		LoadCFG();

		if (SCREEN_MODE==-1)
		{
			SetDefaultCFG();
			WriteCFG();
		}

		if( !initApplication(hInstance, nCmdShow) )
		    return FALSE;

		argv = commandLineToArgv(lpCmdLine, &argc);


		WriteCrashLog("Entering Main");
		main(argc, argv, hDC, hWndMain);
		WriteCrashLog("Exiting Main");

		WriteCFG();

		KillGLWindow( hWndMain );

		if (AlreadyUninit==0)
		{
			UnloadAllTextures();
			DRCloseSound();
			WSACleanup();
			KillFont();
			if (fullscreen)
				ChangeDisplaySettings (&old_dmode, CDS_FULLSCREEN);
			WriteCrashLog("UnInit complete on exiting main");
			AlreadyUninit=1;
		}
		
		PostQuitMessage(0);

	}

  return 0;
  
} /* WinMain */

FxBool
tlErrorMessage( char *err)
{
  /* make the cursor visible */
  SetCursor(LoadCursor( NULL, IDC_ARROW ));
  
  /*
   ** warn user if there is one 
   */
  printf("Error %s..\n", err);
  fflush(stdout);
  
  MessageBox( hWndMain, err, "ERROR", MB_OK );
  return FALSE;
} /* tlErrorMessage */

/*
 * Converts lpCmdLine to WinMain into argc, argv
 */
static char    *fakeName = "WinTest";
static char    *argvbuf[32];
static char    cmdLineBuffer[1024];
char **
commandLineToArgv(LPSTR lpCmdLine, int *pArgc)
{
  char    *p, *pEnd;
  int     argc = 0;
  
  argvbuf[argc++] = fakeName;
  
  if (lpCmdLine == NULL) {
    *pArgc = argc;
    return argvbuf;
  }
  
  strcpy(cmdLineBuffer, lpCmdLine);
  p = cmdLineBuffer;
  pEnd = p + strlen(cmdLineBuffer);
  if (pEnd >= &cmdLineBuffer[1022]) pEnd = &cmdLineBuffer[1022];
  
  fflush(stdout);
  
  while (1) {
    /* skip over white space */
    fflush(stdout);

    while (*p == ' ') p++;
    if (p >= pEnd) break;

    argvbuf[argc++] = p;
    if (argc >= 32) break;

    /* skip till there's a 0 or a white space */
    while (*p && (*p != ' ')) p++;

    if (*p == ' ') *p++ = 0;
  }
  
  *pArgc = argc;
  return argvbuf;
}

/*-------------------------------------------------------------------
  Function: tlKbHit
  Date: 2/28
  Implementor(s): jdt
  Library: test library
  Description:
  Returns true if there are pending characters in the input queue
  Arguments:
  none
  Return:
  nonzero if keys in queue
  -------------------------------------------------------------------*/
int
tlKbHit( void ) 
{
  MSG msg;

  if (qhead != qtail) {
    return 1;
  }
  
  while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);      /* this might change qhead */
    if (qhead != qtail) {
      return 1;
    }
  }
  return 0;
}

/*-------------------------------------------------------------------
  Function: tlGetCH
  Date: 2/28
  Implementor(s): jdt
  Library: test library
  Description:
  Returns character from top of input fifo, blocks if fifo is empty
  Arguments:
  none
  Return:
  character
  -------------------------------------------------------------------*/
char
tlGetCH( void ) 
{
  MSG     msg;
  char    rv;
  
  if (qtail != qhead) {
    rv = (char)queue[qtail++];
    qtail &= 255;
    return rv;
  }
  
  while (GetMessage( &msg, NULL, 0, 0 )) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);

    if (qtail != qhead) {
      rv = (char)queue[qtail++];
      qtail &= 255;
      return rv;
    }
  }
  
  /* Should never get here!! */
  /* printf("Bad exit..\n"); */
  /* fflush(stdout); */
  return 0;
}

void
tlExit()
{
  PostMessage( hWndMain, WM_CLOSE, 0, 0 );
}


void 
getWindowSize(float *width, float *height)
{
  RECT    rect;
  
    GetClientRect(hWndMain, &rect);
    *width = (float) (rect.right-rect.left);
    *height = (float) (rect.bottom-rect.top);
}

int Print(char *mytext){
    //return 0;
    int tmp;

    ScrollWindow(hWndMain, 0 , -cyChar,
        &rect,&rect);
    hdc = GetDC(hWndMain);
    SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
    tmp = TextOut (hdc, cxChar<<1, cyChar*(rect.bottom/cyChar -2),mytext,strlen(mytext));
    ReleaseDC(hWndMain,hdc);
    ValidateRect(hWndMain,&rect);

    return tmp;
}
const int MaxAng = 32968;
float Cos[32968];
float Sin[32968];

int initTrigs(){
    int tmp;
    for( tmp = 0;tmp<MaxAng;tmp++){
        Cos[tmp] = (float)cos(M_PIx2*(float)tmp/(float)MaxAng);
        Sin[tmp] = (float)sin(M_PIx2*(float)tmp/(float)MaxAng);
    }
    return 1;
}

#endif  /* __DOS32__ */

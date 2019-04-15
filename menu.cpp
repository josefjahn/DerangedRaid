
//Original Copyright notice:

/*
    Copyright (c) 1999 Nate Miller
    
    Notice: Usage of any code in this file is subject to the rules
    described in the LICENSE.TXT file included in this directory.
    Reading, compiling, or otherwise using this code constitutes
    automatic acceptance of the rules in said text file.

    File        -- menu.h
    Date        -- 8/04/99
    Author      -- Nate 'm|d' Miller
    Contact     -- vandals1@home.com
    Web         -- http://members.home.com/vandals1
*/


#include "menu.h"
#include "text.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "derangedraid.h"
#include "tlib.h"

int MENU_SIZE;

text_t menutext;


item_t menu[]={
    {MENU_VALUE,    0,  0.0f,    0.0f,  "Nothing",NULL,             NULL,   0.0f}, 
};

int selected = 0; /* which item is selected */

item_t menu_clear[]={
    {MENU_VALUE,    0,  0.0f,    0.0f,  "Nothing",NULL,             NULL,   0.0f}, 
    {MENU_VALUE,    0,  0.0f,    0.0f,  "Nothing",NULL,             NULL,   0.0f}, 
    {MENU_VALUE,    0,  0.0f,    0.0f,  "Nothing",NULL,             NULL,   0.0f}, 
    {MENU_VALUE,    0,  0.0f,    0.0f,  "Nothing",NULL,             NULL,   0.0f}, 
    {MENU_VALUE,    0,  0.0f,    0.0f,  "Nothing",NULL,             NULL,   0.0f}, 
    {MENU_VALUE,    0,  0.0f,    0.0f,  "Nothing",NULL,             NULL,   0.0f}, 
    {MENU_VALUE,    0,  0.0f,    0.0f,  "Nothing",NULL,             NULL,   0.0f}, 
    {MENU_VALUE,    0,  0.0f,    0.0f,  "Nothing",NULL,             NULL,   0.0f}, 
    {MENU_VALUE,    0,  0.0f,    0.0f,  "Nothing",NULL,             NULL,   0.0f}, 
    {MENU_VALUE,    0,  0.0f,    0.0f,  "Nothing",NULL,             NULL,   0.0f}, 
    {MENU_VALUE,    0,  0.0f,    0.0f,  "Nothing",NULL,             NULL,   0.0f}, 
    {MENU_VALUE,    0,  0.0f,    0.0f,  "Nothing",NULL,             NULL,   0.0f}, 
    {MENU_VALUE,    0,  0.0f,    0.0f,  "Nothing",NULL,             NULL,   0.0f}, 
    {MENU_VALUE,    0,  0.0f,    0.0f,  "Nothing",NULL,             NULL,   0.0f}, 
    {MENU_VALUE,    0,  0.0f,    0.0f,  "Nothing",NULL,             NULL,   0.0f}, 
    {MENU_VALUE,    0,  0.0f,    0.0f,  "Nothing",NULL,             NULL,   0.0f}, 
    {MENU_VALUE,    0,  0.0f,    0.0f,  "Nothing",NULL,             NULL,   0.0f}, 
    {MENU_VALUE,    0,  0.0f,    0.0f,  "Nothing",NULL,             NULL,   0.0f}, 
    {MENU_VALUE,    0,  0.0f,    0.0f,  "Nothing",NULL,             NULL,   0.0f}, 
    {MENU_VALUE,    0,  0.0f,    0.0f,  "Nothing",NULL,             NULL,   0.0f}, 
    {MENU_VALUE,    0,  0.0f,    0.0f,  "Nothing",NULL,             NULL,   0.0f}, 
    {MENU_VALUE,    0,  0.0f,    0.0f,  "Nothing",NULL,             NULL,   0.0f}, 
    {MENU_VALUE,    0,  0.0f,    0.0f,  "Nothing",NULL,             NULL,   0.0f}, 
    {MENU_VALUE,    0,  0.0f,    0.0f,  "Nothing",NULL,             NULL,   0.0f}, 
    {MENU_VALUE,    0,  0.0f,    0.0f,  "Nothing",NULL,             NULL,   0.0f}, 
};


item_t menu_single[] = 
{
    {MENU_VALUE,    0,  3.0f,    1.0f,  "Team Pilots",		NULL,             menuMove,   4.0f, -1,	0.0f, 16.0f}, 
    {MENU_VALUE,    0,  4.0f,    1.0f,  "Enemy Pilots",		NULL,             menuMove, 5.0f, -1,		0.0f, 16.0f}, 
    {MENU_LIST,     0,  0.0f,    1.0f,  "Terrain",			NULL,			 menuMove,   0.0f, 0,			0.0f, 4.0f}, 
    {MENU_COMMAND,  0,  0.0f,    0.0f,  "Create Terrain",	menuCreateTerrain,  NULL,               0.0f, -1,		0.0f, 0.0f}, 
    {MENU_COMMAND,  0,  0.0f,    0.0f,  "Start Game",		menuStartSingle,  NULL,               0.0f, -1,		0.0f, 0.0f}, 
};
item_t menu_multi[] = 
{
    {MENU_VALUE,    0,  16.0f,  1.0f,  "MaxClients",		NULL,       menuMove,   32.0f, -1,			1.0f, 256.0f}, 
    {MENU_LIST,    0,  0.0f,    1.0f,  "Terrain",			NULL,       menuMove,   0.0f, 0,				0.0f, 4.0f}, 
    {MENU_COMMAND,  0,  0.0f,    0.0f,  "Create Terrain",	menuCreateTerrainMulti,  NULL,               0.0f, -1,		0.0f, 0.0f}, 
    {MENU_COMMAND,  0,  0.0f,    0.0f,  "Host Game",		menuGameHost, NULL,             0.0f, -1,	0.0f, 0.0f}, 
    {MENU_COMMAND,  0,  0.0f,    0.0f,  "Join Game",		menuGameJoin, NULL,             0.0f, -1,	0.0f, 0.0f}, 
};
item_t menu_options[] = 
{
    {MENU_LIST,    0,  1.0f,    1.0f,  "Resolution",        NULL,       menuMove,	2.0f,	1,	0.0f, 4.0f}, 
    {MENU_VALUE,    0,  32.0f,   16.0f, "Color Depth",      NULL,       menuMove,	32.0f,	-1,	16.0f, 32.0f}, 
    {MENU_TOGGLE,   0,  0.0f,    0.0f,  "LoRes Textures",	NULL,       menuToggle,	0.0f,	-1,	0.0f, 0.0f}, 
    {MENU_TOGGLE,   1,  0.0f,    0.0f,  "Terr. Textures",	NULL,       menuToggle,	0.0f,	-1,	0.0f, 0.0f}, 
    {MENU_LIST,		0,  1.0f,    1.0f,  "Terr. Detail",		NULL,       menuMove,	0.0f,	4,	0.0f, 2.0f}, 
    {MENU_TOGGLE,   1,  0.0f,    0.0f,  "Fullscreen",       NULL,       menuToggle,	0.0f,	-1,	0.0f, 0.0f}, 
//	{MENU_TOGGLE,   1,  0.0f,    0.0f,  "Fog",				NULL,       menuToggle,	0.0f,	-1,	0.0f, 0.0f}, 
    {MENU_TOGGLE,   1,  0.0f,    0.0f,  "Sound",			NULL,       menuToggle,	0.0f,	-1,	0.0f, 0.0f}, 
    {MENU_TOGGLE,   1,  0.0f,    0.0f,  "Invert Mouse",		NULL,       menuToggle,	0.0f,	-1,	0.0f, 0.0f}, 
    {MENU_COMMAND,  0,  0.0f,    0.0f,  "AutoUpdate",		menuAutoUpdate,	NULL,	0.0f,	-1,	0.0f, 0.0f}, 
    {MENU_COMMAND,  0,  0.0f,    0.0f,  "Apply Changes",    menuApplyGFX,	NULL,	0.0f,	-1,	0.0f, 0.0f}, 
};
item_t menu_shop[] = 
{
    {MENU_LIST,    0,  0.0f,	1.0f,	"Vehicle",	NULL,       menuMove, 0.0f, 5,						0.0f, 4.0f}, 
    {MENU_LIST,    0,  0.0f,	1.0f,	"Missiles",	NULL,       menuMove,   0.0f, 2,					0.0f, 4.0f}, 
    {MENU_LIST,    0,  0.0f,	1.0f,	"Special",	NULL,       menuMove, 0.0f, 3,						0.0f, 4.0f}, 
    {MENU_COMMAND,  0,  0.0f,	0.0f,	"Launch",    menuLaunch,   NULL,          0.0f, -1,	0.0f, 0.0f}, 
};

char NameList[10][10][256];




void SwitchToMenu(int MenuNum)
{
	memset (menu, '\0', 1024);
	memcpy(menu, menu_clear, sizeof(menu_clear));
	switch (MenuNum)
    {
		case 0:
			memcpy(menu, menu_single, sizeof(menu_single));
			MENU_SIZE=sizeof (menu_single) / sizeof (menu_single[0]);
			break;  
		case 1:
			memcpy(menu, menu_multi, sizeof(menu_multi));
			MENU_SIZE=sizeof (menu_multi) / sizeof (menu_multi[0]);
			break;  
		case 2:
			memcpy(menu, menu_options, sizeof(menu_options));
			MENU_SIZE=sizeof (menu_options) / sizeof (menu_options[0]);

			menu[0].val = (float)SCREEN_MODE;
			menu[1].val = (float)SCREEN_COLORBITS;
			menu[2].state = SCREEN_LORES;
			menu[3].state = SCREEN_TERRAINTEXTURES;
			if (SCREEN_LOD==4)
				menu[4].val = 0.0f;
			if (SCREEN_LOD==8)
				menu[4].val = 1.0f;
			if (SCREEN_LOD==16)
				menu[4].val = 2.0f;
			menu[5].state = SCREEN_FULLSCREEN;
//			menu[6].state = SCREEN_FOG;
			menu[6].state = SOUND_ENABLED;
			menu[7].state = MOUSE_INVERT;

			break;  
		case 3:
			memcpy(menu, menu_shop, sizeof(menu_shop));
			MENU_SIZE=sizeof (menu_shop) / sizeof (menu_shop[0]);
			break;  
	}
	selected=0;
}






/*
=============
menuAdvance
=============
    Moves the selected item through the menu.
*/
void menuAdvance (int n)
{
    selected += -n;
    
    if (selected < 0)
        selected = 0;
    else if (selected >= MENU_SIZE)
        selected = MENU_SIZE - 1;

}
/*
=============
menuOptionAdvance
=============
    Changes the menu items option.  
*/
void menuOptionAdvance (int n)
{
    if (menu[selected].moveFunc)
        menu[selected].moveFunc (n);
}
/*
=============
menuEnterHandle
=============
    Handles the enter keypress.
*/
void menuEnterHandle (void)
{
    if (menu[selected].enterFunc)
        menu[selected].enterFunc ();
}
/*
=============
menuApply
=============
    Resets the terrain and applys changes.
*/
void menuApply (void)
{
    menuMerge ();
}
/*
=============
menuQuit
=============
    Take a guess.
*/
void menuQuit (void)
{
    exit (1);
}
/*
=============
menuDisplay
=============
    Draws our menu.
*/
void menuDisplay (void)
{
    int xPos = (640 / 3) - (4 * MENU_FONT_SIZE);
    int yPos = (480 / 2) + (4 * MENU_FONT_SIZE);
    char *yes; 
    char *no; 
	char akugxc[256];
    int i;

    for (i = 0; i < MENU_SIZE; i ++)
    {
		menutext.Size(MENU_FONT_SIZE);

        switch (menu[i].type)
        {
            case MENU_TOGGLE:
                yes = (menu[i].state == MENU_YES) ? MENU_TEXT_YES_S : MENU_TEXT_YES;
                no = (menu[i].state == MENU_NO) ? MENU_TEXT_NO_S : MENU_TEXT_NO;
		        if (i == selected)
					menutext.Draw (xPos, yPos, "\\c(0 255 0)%15s\t%s %s", menu[i].tag, yes, no);
				else
					menutext.Draw (xPos, yPos, "\\c(0 128 0)%15s\t%s %s", menu[i].tag, yes, no);

            break;  
            case MENU_VALUE:
		        if (i == selected)
	                menutext.Draw (xPos, yPos, "\\c(0 255 0)%15s\t%.0f", menu[i].tag, menu[i].val);
				else
					menutext.Draw (xPos, yPos, "\\c(0 128 0)%15s\t%.0f", menu[i].tag, menu[i].val);
			break;
            case MENU_LIST:
				sprintf(akugxc, "%s", NameList[menu[i].ListName][(int)(menu[i].val)]);
		        if (i == selected)
	                menutext.Draw (xPos, yPos, "\\c(0 255 0)%15s\t%s", menu[i].tag, akugxc);
				else
					menutext.Draw (xPos, yPos, "\\c(0 128 0)%15s\t%s", menu[i].tag, akugxc);
			break;
            case MENU_COMMAND:
                yPos -= (MENU_FONT_SIZE + 6);
		        if (i == selected)
	                menutext.Draw (xPos, yPos, "\\c(0 255 0)\t%15s", menu[i].tag);
				else
					menutext.Draw (xPos, yPos, "\\c(0 128 0)\t%15s", menu[i].tag);
            break;
        }
        yPos -= (MENU_FONT_SIZE + 3);
    }
}
/*
=============
menuCleanUp
=============
    Puts options back to original state if user does not apply them.
*/
void menuCleanUp (void)
{
    int i;

    for (i = 0; i < MENU_SIZE; i ++)
        if (menu[i].temp != menu[i].val)
            menu[i].val = menu[i].temp;
}
/*
=============
menuMerge
=============
    User applied options. 
*/
void menuMerge (void)
{
    int i;

    for (i = 0; i < MENU_SIZE; i ++)
        menu[i].temp = menu[i].val;
}

void menuInit (void)
{
	menutext.Load("fonts\\font.tga");     // load the font image

	//Terrain NameList
	sprintf(NameList[0][0], "Lush");
	sprintf(NameList[0][1], "Stone");
	sprintf(NameList[0][2], "Snow");
	sprintf(NameList[0][3], "Desert");
	sprintf(NameList[0][4], "Lava");

	//Resolution NameList
	sprintf(NameList[1][0], "640x480");
	sprintf(NameList[1][1], "800x600");
	sprintf(NameList[1][2], "1024x768");
	sprintf(NameList[1][3], "1280x1024");
	sprintf(NameList[1][4], "1600x1200");

	//Missile NameList
	sprintf(NameList[2][0], "Guided");
	sprintf(NameList[2][1], "Dumbfire");
	sprintf(NameList[2][2], "Marker");
	sprintf(NameList[2][3], "Swarm");
	sprintf(NameList[2][4], "Cruise");

	//Special NameList
	sprintf(NameList[3][0], "Shields");
	sprintf(NameList[3][1], "Cloak");
	sprintf(NameList[3][2], "ECM");
	sprintf(NameList[3][3], "Radar Jammer");
	sprintf(NameList[3][4], "Kamikaze");

	//LOD Namelist
	sprintf(NameList[4][0], "Low");
	sprintf(NameList[4][1], "Medium");
	sprintf(NameList[4][2], "High");

	//Vehicle Namelist
	sprintf(NameList[5][0], "Light Scout");
	sprintf(NameList[5][1], "Medium Fighter");
	sprintf(NameList[5][2], "Heavy Bomber");
	sprintf(NameList[5][3], "Light Tank");
	sprintf(NameList[5][4], "Heavy Tank");

}


void setMenu(item_t newmenu[])
{
	memcpy(menu, newmenu, sizeof(newmenu));
	MENU_SIZE = sizeof (newmenu) / sizeof (newmenu[0]);
}


void menuToggle(int n)
{
   menu[selected].state = !menu[selected].state;
}

void menuMove(int n)
{
    float end = menu[selected].val + (n * menu[selected].inc);

    if (end < menu[selected].low)
        end = menu[selected].low;
    else if (end > menu[selected].hi)
        end = menu[selected].hi;

    menu[selected].temp = menu[selected].val;
    menu[selected].val = end;


}
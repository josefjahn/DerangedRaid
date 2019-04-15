

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


#ifndef __MENUH__
#define __MENUH__



#define MENU_TOGGLE         1
#define MENU_VALUE          2
#define MENU_COMMAND        3
#define MENU_LIST           4
#define MENU_YES            1
#define MENU_NO             0
#define MENU_TEXT_YES       "Yes"
#define MENU_TEXT_YES_S    "[Yes]"
#define MENU_TEXT_NO       "No"
#define MENU_TEXT_NO_S     "[No]"
#define MENU_FONT_SIZE      8      /* font size to use */
#define MENU_UP             1
#define MENU_DOWN           -1
#define MENU_TEXTURE_O      0
#define MENU_SIZE_O         1
#define MENU_HEIGHT_O       2
#define MENU_H_O            3

extern int MENU_SIZE;

typedef struct _item_t
{
    int type;                   /* type of menu item */
    int state;                  /* state of item, 0 or 1 */
    float val;                  /* value of item */
    float inc;                  /* how much to add or subtract to val */
    char tag[50];               /* items name */
    void (*enterFunc) (void);   /* function called when enter is hit */
    void (*moveFunc) (int n);   /* function called when option is needed to be moved */
    float temp;
	int ListName;
	float low;
	float hi;
} item_t;

extern item_t menu[];
extern int selected;

void menuAdvance (int n);
void menuOptionAdvance (int n);
void menuDisplay (void);
void menuEnterHandle (void);
void menuApply (void);
void menuQuit (void);
void menuCleanUp (void);
void menuMerge (void);
void SwitchToMenu(int MenuNum);
void menuInit (void);
void menuMove (int n);
void menuToggle (int n);


extern void menuCreateTerrain(void);
extern void menuCreateTerrainMulti(void);
extern void menuStartSingle(void);
extern void menuGameHost(void);
extern void menuGameJoin(void);
extern void menuApplyGFX(void);
extern void menuLaunch(void);
extern void menuAutoUpdate(void);









#endif


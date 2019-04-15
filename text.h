/*
   Copyright (C) 2000 Nate Miller nkmiller@calpoly.edu

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

   See gpl.txt for more information regarding the GNU General Public License.
*/

#ifndef __TEXTH__
#define __TEXTH_

#include "tga.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winuser.h>
#include <tchar.h>
#endif
#include <GL/gl.h>

extern float white[4];
extern float gray[4];

#ifndef TEXT_DEFINED
#define TEXT_DEFINED
struct text_t
{
   text_t();
   int Load(char *name);
   int LoadEx(char *name, int row, int col);
   void Draw(int xpos, int ypos, char *s, ...);
   void Region(int x, int y, int w, int h)
    {region = 1; regionX = x; regionY = y - h; regionW = w; regionH = h;}
   void Size(int sze) {size = sze;}
   void Shadow(void) {shadow = 1;}
   void Gradient(void) {gradient = 1;}
   void Color3f(float r, float g, float b)
    {fgColor[0] = r; fgColor[1] = g; fgColor[2] = b; fgColor[3] = 1.0;}
   void Color4f(float r, float g, float b, float a)
    {fgColor[0] = r; fgColor[1] = g; fgColor[2] = b; fgColor[3] = a;}
   void Color3fv(float *clr)
    {fgColor[0] = clr[0]; fgColor[1] = clr[1]; fgColor[2] = clr[2]; fgColor[3] = 1.0;}
   void Color4fv(float *clr)
    {fgColor[0] = clr[0]; fgColor[1] = clr[1]; fgColor[2] = clr[2]; fgColor[3] = clr[3];}
   void ShadowColor3f(float r, float g, float b)
    {bgColor[0] = r; bgColor[1] = g; bgColor[2] = b; bgColor[3] = 1.0;}
   void ShadowColor4f(float r, float g, float b, float a)
    {bgColor[0] = r; bgColor[1] = g; bgColor[2] = b; bgColor[3] = a;}
   void ShadowColor3fv(float *clr)
    {bgColor[0] = clr[0]; bgColor[1] = clr[1]; bgColor[2] = clr[2]; bgColor[3] = 1.0;}
   void ShadowColor4fv(float *clr)
    {bgColor[0] = clr[0]; bgColor[1] = clr[1]; bgColor[2] = clr[2]; bgColor[3] = clr[3];}
   void GradientColor3f(float r, float g, float b)
    {gdColor[0] = r; gdColor[1] = g; gdColor[2] = b; gdColor[3] = 1.0;}
   void GradientColor4f(float r, float g, float b, float a)
    {gdColor[0] = r; gdColor[1] = g; gdColor[2] = b; gdColor[3] = a;}
   void GradientColor3fv(float *clr)
    {gdColor[0] = clr[0]; gdColor[1] = clr[1]; gdColor[2] = clr[2]; gdColor[3] = 1.0;}
   void GradientColor4fv(float *clr)
    {gdColor[0] = clr[0]; gdColor[1] = clr[1]; gdColor[2] = clr[2]; gdColor[3] = clr[3];}

protected:
   void Reset(void);
   void ColorCopy(float *dest, float *src)
    {dest[0] = src[0]; dest[1] = src[1]; dest[2] = src[2]; dest[3] = src[3];}
   void DrawChar(char c, int x, int y, int size, int shadow, int *vPort);
   void RenderChar (char c, int x, int y, int size, int *vPort);
   void ScissorNormal(int xpos, int ypos, int tabs, int carrage, int size, int len);
   void ScissorTextRegion(void)
    {glScissor(regionX, regionY, regionW, regionH);}
   void ForeColorReset(void) {ColorCopy(fgColor, white);}
   void ShadowColorReset(void) {ColorCopy(bgColor, gray);}
   void GradientColorReset(void) {ColorCopy(gdColor, gray);}
   int SlashParser(char *buffPtr, int *x, int *y, int *vPort);
   void WalkString(char *buffPtr, int xpos, int ypos, int *vPort);
   int SetColorFromToken(char *s);
   int ItalicsMode(char *s);
   int BoldMode(char *s);
   int GetCharHits(char *s, char f);
   void MakeMap(void);
   void SetModes(int state);

   static int fontBlockCol;
   static int fontBlockRow;
   static int fontGetModes; 
   static int fontRestoreModes; 
   static const int fontMaxLen;
   static int fontTabSpace;
   static float fontITOF;
   static int fontItalic;

   float fgColor[4];   /* foreground color, default white */
   float gdColor[4];   /* gradient color, default gray */
   float bgColor[4];   /* background color, default gray */
   int size;           /* size of text, default 12 */
   int shadow;         /* shadow text? default 0 */
   int gradient;       /* gradient? default 0 */
   int italic;         /* italic amount, defaul 0 */
   int bold;           /* bold text? */
   int region;         /* do we have a text region */
   int regionX;        /* lower left x */
   int regionY;        /* lower left y */
   int regionW;        /* text region w */
   int regionH;        /* text region h */
   float tIncX;        /* used for texture coords, x axis amount to move */
   float tIncY;        /* used for texture coords, y axis amount to move */
   int blockRow;       /* characters per row */
   int blockCol;       /* characters per col */
   unsigned int texId; /* texture id */
   
};
#endif
#endif
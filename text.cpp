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

/*
   This is just a quick port of my glTexfont Library to C++.  Need to change 
some things, should redo the inner loop for text rendering. This thing is 
too SLOW, need to change that.

BUGS
   - if there is a \n in a text region things break, i havn't seen it but
   someone told me about it, look for it
*/
#include "text.h"

static float white[4] = {1.0, 1.0, 1.0, 1.0};
static float gray[4] = {0.5, 0.5, 0.5, 1.0};

int text_t::fontBlockCol = 16;
int text_t::fontBlockRow = 16;
int text_t::fontGetModes = 1; 
int text_t::fontRestoreModes = 2; 
const int text_t::fontMaxLen = 1024;
int text_t::fontTabSpace = 4;
float text_t::fontITOF = (float) pow (255, -1);
int text_t::fontItalic = 8;

float tPoints[257][2]; 

text_t::text_t()
{
   ColorCopy(fgColor, white); 
   ColorCopy(gdColor, gray);
   ColorCopy(gdColor, gray);
   size = 12;
   shadow = 0;
   gradient = 0;
   italic = 0;
   bold = 0;
   region = 0;;         
   regionX = 0;
   regionY = 0;
   regionW = 0;
   regionH = 0;
   tIncX = 0.0f;
   tIncY = 0.0f;
   blockRow = fontBlockRow;
   blockCol = fontBlockCol;
   texId = 0;
}

void text_t::Reset(void)
{
   size = 12;
   shadow = 0;
   region = 0;
   gradient = 0;
   italic = 0;
   bold = 0;
   regionX = 0;
   regionY = 0;
   regionW = 0;
   regionH = 0;
   ForeColorReset();
   ShadowColorReset();
   GradientColorReset();
}

int text_t::Load(char *name)
{
   tga_t image;

   texId = image.GenId();

   blockRow = fontBlockRow;
   blockCol = fontBlockCol;
   
   MakeMap();

   return image.Load(name, texId, tga_t::uploadYes, GL_ALPHA);
}

int text_t::LoadEx (char *name, int row, int col)
{
   /* make sure we have valid dimensions */
   if (row * col != 256)
      return 0;
   
   tga_t image;

   texId = image.GenId();

   blockRow = row;
   blockCol = col;

   MakeMap ();

   return image.Load(name, texId, tga_t::uploadYes, GL_ALPHA);
}

void text_t::DrawChar(char c, int x, int y, int size, int shadow, int *vPort)
{

//	size = size * (vPort[2]/640 + vPort[3]/480); 

   if (!gradient && !shadow)
      glColor4fv(fgColor);
   else if (!gradient && shadow)
      glColor4fv(bgColor);
   else if (gradient && !shadow)
      glColor4fv(gdColor);

   glBegin (GL_QUADS);
      glTexCoord2f(tPoints[(int) c][0], tPoints[(int) c][1]);
      glVertex2i(x, y);

      glTexCoord2f(tPoints[(int) c][0] + tIncX, tPoints[(int) c][1]);
      glVertex2i(x + size, y);

      if (!shadow)
         glColor4fv(fgColor);
      else glColor4fv(bgColor);

      glTexCoord2f(tPoints[(int) c][0] + tIncX, tPoints[(int) c][1] + tIncY);
      glVertex2i(x + size + italic, y + size);

      glTexCoord2f(tPoints[(int) c][0], tPoints[(int) c][1] + tIncY);
      glVertex2i(x + italic, y + size);
   glEnd ();
}

void text_t::RenderChar (char c, int x, int y, int size, int *vPort)
{
   if (shadow)
   {
      if (!bold)
         DrawChar(c, x + 1,  y + 1, size, 1, vPort);
      else DrawChar(c, x + 2,  y + 1, size, 1, vPort);
   }

   DrawChar(c, x, y, size, 0, vPort);

   if (bold)
      DrawChar(c, x + 1, y, size, 0, vPort);
}

void text_t::ScissorNormal(int xpos, int ypos, int tabs, int carrage, int size, int len)
{
   int sy, ex, ey;
   
   ex = len * size * tabs;

   if (carrage)
   {
      sy = ypos - (size * carrage);
      ey = size * (carrage + 1);
   } 
   else
   {
      sy = ypos;
      ey = size;
   }
   
//   glScissor(xpos, sy, ex, ey);
}

int text_t::SlashParser(char *buffPtr, int *x, int *y, int *vPort)
{
   int ret = 0;

   *buffPtr++;

   if (!*buffPtr)
      return ret;

   switch (*buffPtr)
   {
      case 'a':
      case 'c':
         *x -= size;
         return SetColorFromToken(buffPtr);
      break;
      case 'i':
         *x -= size;
         return ItalicsMode(buffPtr);
      break;
      case 'b':
         *x -= size;
         return BoldMode(buffPtr);
      break;
      default:
         *buffPtr--;
         RenderChar(*buffPtr, *x, *y, size, vPort);
         return ret;
      break;
   }
}

void text_t::WalkString(char *buffPtr, int xpos, int ypos, int *vPort)
{
   int x = xpos;
   int y = ypos;
   int carrage = 0;
   int tabs = 0;
   int len = strlen(buffPtr);
   int xMax;

   xMax = 640;
//   xMax =  vPort[0] + vPort[2];

   carrage = GetCharHits(buffPtr, '\n');
   tabs = GetCharHits(buffPtr, '\t');

   if (!tabs)
      tabs = 1;
   else tabs *= fontTabSpace;

   if (region)
   {
      ScissorTextRegion();
      x = regionX;
      y = (regionY + regionH) - size;
   } else ScissorNormal(xpos, ypos, tabs, carrage, size, len);

   /* lets draw! */
   for ( ; *buffPtr; *buffPtr++, x += (size-2))
   {
      if (x > xMax)
         break;
   
      if (region)
      {
         if (x + size > (regionX + regionW))
         {
            y -= size;
            x = regionX;
         }
         if (y < regionY)
            break;
      }

      switch (*buffPtr)
      {
         case '\n':
            y -= size;
            x = xpos - size;
            continue; 
         break;
         
         case '\t':
            x += (size * fontTabSpace);
            continue; 
         break;
      
         case '\\':
            buffPtr += SlashParser(buffPtr, &x, &y, vPort);
            if (*buffPtr == '\n' || *buffPtr == '\t')
            {
               buffPtr -= 1;
               continue;
            }
         break;

         default:
            RenderChar(*buffPtr, x, y, size, vPort);
         break;
      }
   }
}

void text_t::Draw(int xpos, int ypos, char *s, ...)
{
   va_list	msg;
   char buffer[fontMaxLen] = {'\0'};
   int vPort[4];

   va_start (msg, s);
#ifdef _WIN32
   _vsntprintf (buffer, fontMaxLen - 1, s, msg);	
#else /* linux */
   vsnprintf (buffer, fontMaxLen - 1, s, msg);	
#endif
   va_end (msg);

   /* get current viewport */
   glGetIntegerv(GL_VIEWPORT, vPort);
   /* setup various opengl things that we need */
   SetModes(fontGetModes);

   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadIdentity();

//   glOrtho(0, vPort[2], 0, vPort[3], -1, 1);
   glOrtho(0, 640, 0, 480, -1, 1);
   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadIdentity();
   glBindTexture(GL_TEXTURE_2D, texId);

   /* draw the string */
   WalkString(buffer, xpos, ypos, vPort);

   glMatrixMode(GL_PROJECTION);
   glPopMatrix();   
   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();

   SetModes (fontRestoreModes);
   Reset ();
}

int text_t::SetColorFromToken (char *s)
{
   int clr[4];
   int ret = 1;

   if (*s == 'c')
   {
      s += 1;
      if (sscanf(s, "(%d %d %d)", &clr[0], &clr[1], &clr[2]) != 3)
         return -1;
      Color3f(clr[0] * fontITOF, clr[1] * fontITOF, clr[2] * fontITOF);
   }
   else if (*s == 'a')
   {
      s += 1;
      if (sscanf(s, "(%d %d %d %d)", &clr[0], &clr[1], &clr[2], &clr[3]) != 4)
         return -1;
      Color4f(clr[0] * fontITOF, clr[1] * fontITOF, clr[2] * fontITOF, clr[3] * fontITOF);
   }

   while (*s != ')' && ret ++)
      s += 1;

   return ret + 1;
}

int text_t::ItalicsMode (char *s)
{
   s += 1;

   if (*s == '+')
      italic = fontItalic;
   else if (*s == '-')
      italic = 0;
   else return -1;

   return 2;
}

int text_t::BoldMode (char *s)
{
   s += 1;

   if (*s == '+')
      bold = 1;
   else if (*s == '-')
      bold = 0;
   else return -1;

   return 2;
}

int text_t::GetCharHits(char *s, char f)
{
   for (int hits = 0; *s; *s++)
      if (*s == f)
         hits ++;

	return hits;
}

void text_t::MakeMap(void)
{
#define vCopy(d,x,y) {d[0] = x; d[1] = y;}
   int i = 0;
   float x, y;

   tIncX = (float)pow (blockCol, -1);
   tIncY = (float)pow (blockRow, -1);


   for (y = 1 - tIncY; y >= 0; y -= tIncY)
      for (x = 0; x <= 1 - tIncX; x += tIncX, i ++)
         vCopy(tPoints[i], x, y);
#undef vCopy
}

void text_t::SetModes(int state)
{
   static int matrixMode;
   static int polyMode[2];
   static int lightingOn;
   static int blendOn;
   static int depthOn;
   static int textureOn;
   static int scissorOn;
   static int blendSrc;
   static int blendDst;

   /* grab the modes that we might need to change */
   if (state == fontGetModes)
   {
      glGetIntegerv(GL_POLYGON_MODE, polyMode);

      if (polyMode[0] != GL_FILL)
         glPolygonMode(GL_FRONT, GL_FILL);
      if (polyMode[1] != GL_FILL)
         glPolygonMode(GL_BACK, GL_FILL);

      textureOn = glIsEnabled(GL_TEXTURE_2D);
         
      if (!textureOn)
         glEnable(GL_TEXTURE_2D);

      depthOn = glIsEnabled(GL_DEPTH_TEST);
   
      if (depthOn)
         glDisable(GL_DEPTH_TEST);

      lightingOn = glIsEnabled(GL_LIGHTING);        

      if (lightingOn) 
         glDisable(GL_LIGHTING);

      scissorOn= glIsEnabled(GL_SCISSOR_TEST);        

      if (!scissorOn) 
         glEnable(GL_SCISSOR_TEST);

      glGetIntegerv(GL_MATRIX_MODE, &matrixMode); 

      /* i don't know if this is correct */
      blendOn= glIsEnabled(GL_BLEND);        
      glGetIntegerv(GL_BLEND_SRC, &blendSrc);
      glGetIntegerv(GL_BLEND_DST, &blendDst);
   
      if (!blendOn)
         glEnable(GL_BLEND);

      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   }
   else if (state == fontRestoreModes)
   {
      /* put everything back where it was before */
      if (polyMode[0] != GL_FILL)
         glPolygonMode(GL_FRONT, polyMode[0]);
      if (polyMode[1] != GL_FILL)
         glPolygonMode(GL_BACK, polyMode[1]);

      if (lightingOn)
         glEnable(GL_LIGHTING);

      /* i don't know if this is correct */
      if (!blendOn)
      {
         glDisable(GL_BLEND);
         glBlendFunc(blendSrc, blendDst);
      }
      else glBlendFunc(blendSrc, blendDst);

      if (depthOn)
         glEnable(GL_DEPTH_TEST);

      if (!textureOn)
         glDisable(GL_TEXTURE_2D);

      if (!scissorOn) 
         glDisable(GL_SCISSOR_TEST);

      glMatrixMode(matrixMode);
   }
}

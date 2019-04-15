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

#ifndef TGA_H
#define TGA_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <GL/gl.h>

struct tga_t
{
   tga_t();
   tga_t(char *name);
   tga_t(char *name, int texId, int upload = uploadYes, GLenum nf = 0);
   int Load(char *name);
   int Load(char *name, int texId, int upload = uploadYes, GLenum nf = 0);
   void Free(void)        {delete [] data;}
   void Id(int texId)     {id = texId;}
   void Upload(void);
   void Reset(void);
   int LastError(void)    {return lastError;}
   int Width(void)        {return width;}
   int Height(void)       {return height;}
   int Bits(void)         {return bits;}
   int Id(void)           {return id;}
   void Format(GLenum nf) {format = nf;}
   GLenum Format(void)    {return format;}
   unsigned char *Data(void) {return data;}
   unsigned char operator[](int ndx) {return data[ndx];}
   int GenId(void)     
    {unsigned int temp; glGenTextures(1, &temp); return temp;}

   enum
   {
      notFound = 1000,
      badType,
      badDimension,
      badBits,    
      badData,    
      badUpload,
      uploadNo = 1,
      uploadYes = 0,    
      maxTexSize = 512
   };

protected:
   int Error(int errNum, FILE *strm);
   int CheckSize(int x);
   unsigned char *GetRGB(FILE *strm, int size);
   unsigned char *GetRGBA(FILE *strm, int size);
   unsigned char *GetGray(FILE *strm, int size);
   unsigned char *GrabData(FILE *strm, int size);

   int lastError;
   int bits;
   int width;
   int height;
   int id;
   int size;
   GLenum format;
   unsigned char *data;
   
};
#endif
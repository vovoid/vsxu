/**
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Lesser General Public License (LGPL)
*
* VSXu Engine is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU Lesser General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


//#ifdef _WIN32 /* Stupid Windows needs to include windows.h before gl.h */
	//#undef FAR
	//#include <windows.h>
//#endif

#include <string/vsx_string.h>
#include "vsxg.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <png.h>

/* Used to decide if GL/gl.h supports the paletted extension */
//#ifdef GL_COLOR_INDEX1_EXT
//#define SUPPORTS_PALETTE_EXT
//#endif

#define png_check_sig(sig, n) !png_sig_cmp((sig), 0, (n))

/*static unsigned char DefaultAlphaCallback(unsigned char red, unsigned char green, unsigned char blue) {
	return 255;
}*/

//static unsigned char StencilRed = 0, StencilGreen = 0, StencilBlue = 0;
//static unsigned char (*AlphaCallback)(unsigned char red, unsigned char green, unsigned char blue) = DefaultAlphaCallback;
static int StandardOrientation = 0;

#ifdef SUPPORTS_PALETTE_EXT
#ifdef _WIN32
PFNGLCOLORTABLEEXTPROC glColorTableEXT = NULL;
#endif
#endif

//static int PalettedTextures = -1;
//static GLint MaxTextureSize = 0;

/* screenGamma = displayGamma/viewingGamma
 * displayGamma = CRT has gamma of ~2.2
 * viewingGamma depends on platform. PC is 1.0, Mac is 1.45, SGI defaults
 * to 1.7, but this can be checked and changed w/ /usr/sbin/gamma command.
 * If the environment variable VIEWING_GAMMA is set, adjust gamma per this value.
 */
#ifdef _MAC
	static double screenGamma = 2.2 / 1.45;
#elif SGI
	static double screenGamma = 2.2 / 1.7;
#else /* PC/default */
	static double screenGamma = 2.2 / 1.0;
#endif

static char gammaExplicit = 0;	/*if  */

static void checkForGammaEnv()
{
	double viewingGamma;
	char *gammaEnv = getenv("VIEWING_GAMMA");

	if(gammaEnv && !gammaExplicit)
	{
		sscanf(gammaEnv, "%lf", &viewingGamma);
		screenGamma = 2.2/viewingGamma;
	}
}

typedef struct {
  vsx::filesystem* filesystem;
  vsx::file* fp;
} filesystem_info;


static void png_vsx_filesystem_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
  png_size_t check;
  filesystem_info* a = (filesystem_info*)(png_get_io_ptr(png_ptr));
   /* fread() returns 0 on error, so it is OK to store this in a png_size_t
    * instead of an int, which is what fread() actually returns.
    */
   check = (png_size_t)a->filesystem->f_read((void*)data, length,a->fp);

   if (check != length)
   {
     printf("Error reading png file. 'Check'' is not equal to 'length' in glpng.cpp line %d",__LINE__);
     //png_error(png_ptr, "Read Error");
   }
}



//int APIENTRY pngLoadRawF(FILE *fp, pngRawInfo *pinfo) {
int  pngLoadRaw(const char* filename, pngRawInfo *pinfo, vsx::filesystem* filesystem) {
	unsigned char header[8];
	png_structp png;
	png_infop   info;
	png_infop   endinfo;
	png_bytep   data;
  png_bytep  *row_p;
  double fileGamma;
  filesystem_info i_filesystem;

	png_uint_32 width, height;
	int depth, color;

	png_uint_32 i;

	if (pinfo == NULL) {
    printf("error in png loader: pinfo is NULL %d\n",__LINE__);
    return 0;
  }
	i_filesystem.filesystem = filesystem;
	i_filesystem.fp = filesystem->f_open(filename,"rb");
	if (!i_filesystem.fp) {
    printf("error in png loader when loading %s: i_filesystem.fp not valid on line %d\n",filename, __LINE__);
    return 0;
  }
  
	filesystem->f_read(header, 8, i_filesystem.fp);
	if (!png_check_sig(header, 8)) {
    printf("error in %s on line %d\n",__FILE__,__LINE__);
    return 0;
  }

	png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png) {
    printf("error in %s on line %d\n",__FILE__,__LINE__);
    return 0;
  }
	info = png_create_info_struct(png);
  if (!info)
  {
      png_destroy_read_struct(&png,(png_infopp)NULL,(png_infopp)NULL);
      printf("error in %s on line %d\n",__FILE__,__LINE__);
      return 0;
  }
  endinfo = png_create_info_struct(png);
  if (!endinfo)
  {
    png_destroy_read_struct(&png, &info, (png_infopp)NULL);
    printf("error in %s on line %d\n",__FILE__,__LINE__);
    return 0;
  }

  if (setjmp(png_jmpbuf(png)))
  {
    printf("error in png_jmpbuf %s on line %d\n",__FILE__,__LINE__);
    png_destroy_read_struct(&png, &info,&endinfo);
    filesystem->f_close(i_filesystem.fp);
    return 0;
  }

  png_set_read_fn(png, (png_bytep)(&i_filesystem), png_vsx_filesystem_read_data);
  png_set_sig_bytes(png, 8);
	png_read_info(png, info);
	png_get_IHDR(png, info, &width, &height, &depth, &color, NULL, NULL, NULL);

	pinfo->Width  = width;
	pinfo->Height = height;
	pinfo->Depth  = depth;

	if (color == PNG_COLOR_TYPE_GRAY || color == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png);
		
  if (color == PNG_COLOR_TYPE_PALETTE)
			png_set_expand(png);

	/*--GAMMA--*/
	checkForGammaEnv();
	if (png_get_gAMA(png, info, &fileGamma))
		png_set_gamma(png, screenGamma, fileGamma);
	else
		png_set_gamma(png, screenGamma, 1.0/2.2);

	png_read_update_info(png, info);

	data = (png_bytep) malloc(png_get_rowbytes(png, info)*height);
	row_p = (png_bytep *) malloc(sizeof(png_bytep)*height);

	for (i = 0; i < height; i++) {
		if (StandardOrientation)
			row_p[height - 1 - i] = &data[png_get_rowbytes(png, info)*i];
		else
			row_p[i] = &data[png_get_rowbytes(png, info)*i];
	}

	png_read_image(png, row_p);
	free(row_p);

	if (color == PNG_COLOR_TYPE_PALETTE) {
		int cols;
		png_get_PLTE(png, info, (png_colorp *) &pinfo->Palette, &cols);
	}
	else {
		pinfo->Palette = NULL;
	}

	if (color&PNG_COLOR_MASK_ALPHA) {
		if (color&PNG_COLOR_MASK_PALETTE || color == PNG_COLOR_TYPE_GRAY_ALPHA)
			pinfo->Components = 2;
		else
			pinfo->Components = 4;
		pinfo->Alpha = 8;
	}
	else {
		if (color&PNG_COLOR_MASK_PALETTE || color == PNG_COLOR_TYPE_GRAY)
			pinfo->Components = 1;
		else
			pinfo->Components = 3;
		pinfo->Alpha = 0;
	}

  pinfo->Data = data;

  png_read_end(png, endinfo);
  png_destroy_read_struct(&png, &info, &endinfo);
  filesystem->f_close(i_filesystem.fp);
  return 1;
}


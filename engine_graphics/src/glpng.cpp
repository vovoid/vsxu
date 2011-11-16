/*
 * PNG loader library for OpenGL v1.45 (10/07/00)
 * by Ben Wyatt ben@wyatt100.freeserve.co.uk
 * Using LibPNG 1.0.2 and ZLib 1.1.3
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the author be held liable for any damages arising from the
 * use of this software.
 *
 * Permission is hereby granted to use, copy, modify, and distribute this
 * source code, or portions hereof, for any purpose, without fee, subject to
 * the following restrictions:
 *
 * 1. The origin of this source code must not be misrepresented. You must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered versions must be plainly marked as such and must not be
 *    misrepresented as being the original source.
 * 3. This notice must not be removed or altered from any source distribution.
 */

//#ifdef _WIN32 /* Stupid Windows needs to include windows.h before gl.h */
	//#undef FAR
	//#include <windows.h>
//#endif

#include "vsx_string.h"
#include "vsxg.h"
//#include "glew.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <png.h>

/* Used to decide if GL/gl.h supports the paletted extension */
//#ifdef GL_COLOR_INDEX1_EXT
//#define SUPPORTS_PALETTE_EXT
//#endif

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
  vsxf* filesystem;
  vsxf_handle* fp;
} vsxf_info;


static void png_vsxf_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
  png_size_t check;
  vsxf_info* a = (vsxf_info*)(png_get_io_ptr(png_ptr));
   /* fread() returns 0 on error, so it is OK to store this in a png_size_t
    * instead of an int, which is what fread() actually returns.
    */
   check = (png_size_t)a->filesystem->f_read((void*)data, length,a->fp);

   if (check != length)
   {
      png_error(png_ptr, "Read Error");
   }
}



//int APIENTRY pngLoadRawF(FILE *fp, pngRawInfo *pinfo) {
int  pngLoadRaw(const char* filename, pngRawInfo *pinfo, vsxf* filesystem) {
  printf("%s line %d\n",__FILE__,__LINE__);
	unsigned char header[8];
	png_structp png;
	png_infop   info;
	png_infop   endinfo;
	png_bytep   data;
  png_bytep  *row_p;
  double fileGamma;
  vsxf_info i_filesystem;

	png_uint_32 width, height;
	int depth, color;

	png_uint_32 i;

	if (pinfo == NULL) {
    return 0;
  }
	i_filesystem.filesystem = filesystem;
	i_filesystem.fp = filesystem->f_open(filename,"rb");
	if (!i_filesystem.fp) {
    printf("error in png loader: i_filesystem.fp not valid on line %d\n",__LINE__);
    return 0;
  }
  
	filesystem->f_read(header, 8, i_filesystem.fp);
	if (!png_check_sig(header, 8)) {
    printf("error in %s on line %d\n",__FILE__,__LINE__);
    return 0;
  }

	png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	info = png_create_info_struct(png);
	endinfo = png_create_info_struct(png);
	
	png_set_read_fn(png, (png_bytep)(&i_filesystem), png_vsxf_read_data);
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


#pragma once

#include <bitmap/vsx_bitmap.h>
#include "vsx_texture_gl.h"
#include <bitmap/vsx_bitmap.h>
#include <bitmap/vsx_bitmap_transform.h>
#include <vsx_gl_global.h>

namespace vsx_texture_gl_loader
{

  GLenum get_compression_format(vsx_bitmap* bitmap);

/**
 * @brief upload_1d
 * @param texture_gl
 * @param data
 * @param size
 * @param mipmaps
 * @param bpp
 * @param bpp2
 */
void upload_1d(vsx_texture_gl* texture_gl, void* data, unsigned long size, bool mipmaps = false, int bpp = 4, int bpp2 = GL_BGRA_EXT);

void handle_anisotropic_mip_map_min_mag(vsx_texture_gl* texture_gl);

/**
 * @brief upload_2d
 * @param texture_gl
 * @param bitmap
 */
void upload_2d( vsx_texture_gl* texture_gl );


/**
 * @brief upload_cube
 * @param texture_gl
 * @param bitmap
 */
void upload_cube( vsx_texture_gl* texture_gl );

void upload_bitmap_2d(vsx_texture_gl* texture_gl, vsx_bitmap* bitmap, bool flip_vertical);

}

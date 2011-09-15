/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @see The Lesser GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the Lesser GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the Lesser GNU General Public License
* for more details.
*
* You should have received a copy of the Lesser GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef     __FTTextureGlyph__
#define     __FTTextureGlyph__


#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "FTGL.h"
#include "FTGlyph.h"


/**
 * FTTextureGlyph is a specialisation of FTGlyph for creating texture
 * glyphs.
 * 
 * @see FTGlyphContainer
 *
 */
class FTGL_EXPORT FTTextureGlyph : public FTGlyph
{
    public:
        /**
         * Constructor
         *
         * @param glyph     The Freetype glyph to be processed
         * @param id        The id of the texture that this glyph will be
         *                  drawn in
         * @param xOffset   The x offset into the parent texture to draw
         *                  this glyph
         * @param yOffset   The y offset into the parent texture to draw
         *                  this glyph
         * @param width     The width of the parent texture
         * @param height    The height (number of rows) of the parent texture
         */
        FTTextureGlyph( FT_GlyphSlot glyph, int id, int xOffset, int yOffset, GLsizei width, GLsizei height);

        /**
         * Destructor
         */
        virtual ~FTTextureGlyph();

        /**
         * Renders this glyph at the current pen position.
         *
         * @param pen   The current pen position.
         * @return      The advance distance for this glyph.
         */
        virtual float Render( const FTPoint& pen);
        
    private:
        /**
         * The width of the glyph 'image'
         */
        int destWidth;

        /**
         * The height of the glyph 'image'
         */
        int destHeight;

        /**
         * Vector from the pen position to the topleft corner of the pixmap
         */
        FTPoint pos;
        
        /**
         * The texture co-ords of this glyph within the texture.
         */
        FTPoint uv[2];
        
        /**
         * The texture index that this glyph is contained in.
         */
        int glTextureID;

        /**
         * The texture index of the currently active texture
         *
         * We call glGetIntegerv( GL_TEXTURE_2D_BINDING, activeTextureID);
         * to get the currently active texture to try to reduce the number
         * of texture bind operations
         */
        GLint activeTextureID;
        
};


#endif  //  __FTTextureGlyph__

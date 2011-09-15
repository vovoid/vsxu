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

#ifndef     __FTBitmapGlyph__
#define     __FTBitmapGlyph__


#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "FTGL.h"
#include "FTGlyph.h"


/**
 * FTBitmapGlyph is a specialisation of FTGlyph for creating bitmaps.
 *
 * It provides the interface between Freetype glyphs and their openGL
 * Renderable counterparts. This is an abstract class and derived classes
 * must implement the <code>Render</code> function. 
 * 
 * @see FTGlyphContainer
 *
 */
class FTGL_EXPORT FTBitmapGlyph : public FTGlyph
{
    public:
        /**
         * Constructor
         *
         * @param glyph The Freetype glyph to be processed
         */
        FTBitmapGlyph( FT_GlyphSlot glyph);

        /**
         * Destructor
         */
        virtual ~FTBitmapGlyph();

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
        unsigned int destWidth;

        /**
         * The height of the glyph 'image'
         */
        unsigned int destHeight;

        /**
         * The pitch of the glyph 'image'
         */
        unsigned int destPitch;

        /**
         * Vector from the pen position to the topleft corner of the bitmap
         */
        FTPoint pos;
        
        /**
         * Pointer to the 'image' data
         */
        unsigned char* data;
        
};


#endif  //  __FTBitmapGlyph__


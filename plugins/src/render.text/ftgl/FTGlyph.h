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

#ifndef     __FTGlyph__
#define     __FTGlyph__

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "FTBBox.h"
#include "FTPoint.h"
#include "FTGL.h"


/**
 * FTGlyph is the base class for FTGL glyphs.
 *
 * It provides the interface between Freetype glyphs and their openGL
 * renderable counterparts. This is an abstract class and derived classes
 * must implement the <code>render</code> function. 
 * 
 * @see FTGlyphContainer
 * @see FTBBox
 * @see FTPoint
 *
 */
class FTGL_EXPORT FTGlyph
{
    public:
        /**
         * Constructor
         */
        FTGlyph( FT_GlyphSlot glyph);

        /**
         * Destructor
         */
        virtual ~FTGlyph();

        /**
         * Renders this glyph at the current pen position.
         *
         * @param pen   The current pen position.
         * @return      The advance distance for this glyph.
         */
        virtual float Render( const FTPoint& pen) = 0;
        
        /**
         * Return the advance width for this glyph.
         *
         * @return  advance width.
         */
        float Advance() const { return advance;}
        
        /**
         * Return the bounding box for this glyph.
         *
         * @return  bounding box.
         */
        const FTBBox& BBox() const { return bBox;}
        
        /**
         * Queries for errors.
         *
         * @return  The current error code.
         */
        FT_Error Error() const { return err;}
        
    protected:
        /**
         * The advance distance for this glyph
         */
        float advance;

        /**
         * The bounding box of this glyph.
         */
        FTBBox bBox;
        
        /**
         * Current error code. Zero means no error.
         */
        FT_Error err;
        
    private:

};


#endif  //  __FTGlyph__


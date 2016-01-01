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

#ifndef     __FTExtrdGlyph__
#define     __FTExtrdGlyph__

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "FTGL.h"
#include "FTGlyph.h"

class FTVectoriser;

/**
 * FTExtrdGlyph is a specialisation of FTGlyph for creating tessellated
 * extruded polygon glyphs.
 * 
 * @see FTGlyphContainer
 * @see FTVectoriser
 *
 */
class FTGL_EXPORT FTExtrdGlyph : public FTGlyph
{
    public:
        /**
         * Constructor. Sets the Error to Invalid_Outline if the glyphs isn't an outline.
         *
         * @param glyph The Freetype glyph to be processed
         * @param depth The distance along the z axis to extrude the glyph
         */
        FTExtrdGlyph( FT_GlyphSlot glyph, float depth);

        /**
         * Destructor
         */
        virtual ~FTExtrdGlyph();

        /**
         * Renders this glyph at the current pen position.
         *
         * @param pen   The current pen position.
         * @return      The advance distance for this glyph.
         */
        virtual float Render( const FTPoint& pen);
        
    private:
        /**
         * Calculate the normal vector to 2 points. This is 2D and ignores
         * the z component. The normal will be normalised
         *
         * @param a
         * @param b
         * @return
         */
        FTPoint GetNormal( const FTPoint &a, const FTPoint &b);
        
        
        /**
         * OpenGL display list
         */
        GLuint glList;
        
        /**
         * Distance to extrude the glyph
         */
        float depth;
    
};


#endif  //  __FTExtrdGlyph__


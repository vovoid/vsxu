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

#ifndef		__FTOutlineGlyph__
#define		__FTOutlineGlyph__

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "FTGL.h"
#include "FTGlyph.h"

class FTVectoriser;


/**
 * FTOutlineGlyph is a specialisation of FTGlyph for creating outlines.
 * 
 * @see FTGlyphContainer
 * @see FTVectoriser
 *
 */
class FTGL_EXPORT FTOutlineGlyph : public FTGlyph
{
    public:
        /**
         * Constructor. Sets the Error to Invalid_Outline if the glyphs isn't an outline.
         *
         * @param glyph The Freetype glyph to be processed
         */
        FTOutlineGlyph( FT_GlyphSlot glyph);

        /**
         * Destructor
         */
        virtual ~FTOutlineGlyph();

        /**
         * Renders this glyph at the current pen position.
         *
         * @param pen	The current pen position.
         * @return		The advance distance for this glyph.
         */
        virtual float Render( const FTPoint& pen);
        
    private:		
        /**
         * OpenGL display list
         */
        GLuint glList;
	
};


#endif	//	__FTOutlineGlyph__


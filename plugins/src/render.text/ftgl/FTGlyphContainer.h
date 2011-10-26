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

#ifndef     __FTGlyphContainer__
#define     __FTGlyphContainer__

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "FTGL.h"
#include "FTBBox.h"
#include "FTPoint.h"
#include "FTVector.h"

class FTFace;
class FTGlyph;
class FTCharmap;

/**
 * FTGlyphContainer holds the post processed FTGlyph objects.
 *
 * @see FTGlyph
 */
class FTGL_EXPORT FTGlyphContainer
{
        typedef FTVector<FTGlyph*> GlyphVector;
    public:
        /**
         * Constructor
         *
         * @param face      The Freetype face
         */
        FTGlyphContainer( FTFace* face);

        /**
         * Destructor
         */
        ~FTGlyphContainer();

        /**
         * Sets the character map for the face.
         *
         * @param encoding      the Freetype encoding symbol. See above.
         * @return              <code>true</code> if charmap was valid
         *                      and set correctly
         */
        bool CharMap( FT_Encoding encoding);

        /**
         * Get the font index of the input character.
         *
         * @param characterCode The character code of the requested glyph in the
         *                      current encoding eg apple roman.
         * @return      The font index for the character.
         */
        unsigned int FontIndex( const unsigned int characterCode ) const;
        
        /**
         * Adds a glyph to this glyph list.
         *
         * @param glyph         The FTGlyph to be inserted into the container
         * @param characterCode The char code of the glyph NOT the glyph index.
         */
        void Add( FTGlyph* glyph, const unsigned int characterCode);

        /**
         * Get a glyph from the glyph list
         *
         * @param characterCode The char code of the glyph NOT the glyph index      
         * @return              An FTGlyph or <code>null</code> is it hasn't been
         * loaded.
         */
        const FTGlyph* const Glyph( const unsigned int characterCode) const;

        /**
         * Get the bounding box for a character.
         * @param characterCode The char code of the glyph NOT the glyph index      
         */
        FTBBox BBox( const unsigned int characterCode) const;
        
        /**
        * Returns the kerned advance width for a glyph.
        *
        * @param characterCode     glyph index of the character
        * @param nextCharacterCode the next glyph in a string
        * @return                  advance width
        */
        float Advance( const unsigned int characterCode, const unsigned int nextCharacterCode);
        
        /**
         * Renders a character
         * @param characterCode      the glyph to be Rendered
         * @param nextCharacterCode  the next glyph in the string. Used for kerning.
         * @param penPosition        the position to Render the glyph
         * @return                   The distance to advance the pen position after Rendering
         */
        FTPoint Render( const unsigned int characterCode, const unsigned int nextCharacterCode, FTPoint penPosition);
        
        /**
         * Queries the Font for errors.
         *
         * @return  The current error code.
         */
        FT_Error Error() const { return err;}

    private:
        /**
         * The FTGL face
         */
        FTFace* face;

        /**
         * The Character Map object associated with the current face
         */
        FTCharmap* charMap;

        /**
         * A structure to hold the glyphs
         */
        GlyphVector glyphs;

        /**
         * Current error code. Zero means no error.
         */
        FT_Error err;
};


#endif  //  __FTGlyphContainer__

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

#include    "FTGLExtrdFont.h"
#include    "FTExtrdGlyph.h"


FTGLExtrdFont::FTGLExtrdFont( const char* fontname)
:   FTFont( fontname),
    depth( 0.0f)
{}


FTGLExtrdFont::FTGLExtrdFont( const unsigned char *pBufferBytes, size_t bufferSizeInBytes)
:   FTFont( pBufferBytes, bufferSizeInBytes),
    depth( 0.0f)
{}


FTGLExtrdFont::~FTGLExtrdFont()
{}


FTGlyph* FTGLExtrdFont::MakeGlyph( unsigned int glyphIndex)
{
    FT_GlyphSlot ftGlyph = face.Glyph( glyphIndex, FT_LOAD_NO_HINTING);

    if( ftGlyph)
    {
        FTExtrdGlyph* tempGlyph = new FTExtrdGlyph( ftGlyph, depth);
        return tempGlyph;
    }

    err = face.Error();
    return NULL;
}



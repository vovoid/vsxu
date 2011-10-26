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

#include    "FTGLBufferFont.h"
#include    "FTBufferGlyph.h"


FTGLBufferFont::FTGLBufferFont( const char* fontname)
:   FTFont( fontname),
    buffer(0)
{}


FTGLBufferFont::FTGLBufferFont( const unsigned char *pBufferBytes, size_t bufferSizeInBytes)
:   FTFont( pBufferBytes, bufferSizeInBytes),
    buffer(0)
{}


FTGLBufferFont::~FTGLBufferFont()
{}


FTGlyph* FTGLBufferFont::MakeGlyph( unsigned int g)
{
    FT_GlyphSlot ftGlyph = face.Glyph( g, FT_LOAD_NO_HINTING);

    if( ftGlyph)
    {
        FTBufferGlyph* tempGlyph = new FTBufferGlyph( ftGlyph, buffer);
        return tempGlyph;
    }

    err = face.Error();
    return NULL;
}


void FTGLBufferFont::Render( const char* string)
{   
    if( NULL != buffer)
    {
        FTFont::Render( string);
    }
}


void FTGLBufferFont::Render( const wchar_t* string)
{   
    if( NULL != buffer)
    {
        FTFont::Render( string);
    }
}



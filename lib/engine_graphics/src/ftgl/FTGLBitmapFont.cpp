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

#include "FTGLBitmapFont.h"
#include "FTBitmapGlyph.h"


FTGLBitmapFont::FTGLBitmapFont( const char* fontname)
:   FTFont( fontname)
{}


FTGLBitmapFont::FTGLBitmapFont( const unsigned char *pBufferBytes, size_t bufferSizeInBytes)
:   FTFont( pBufferBytes, bufferSizeInBytes)
{}


FTGLBitmapFont::~FTGLBitmapFont()
{}


FTGlyph* FTGLBitmapFont::MakeGlyph( unsigned int g)
{
    FT_GlyphSlot ftGlyph = face.Glyph( g, FT_LOAD_DEFAULT);

    if( ftGlyph)
    {
        FTBitmapGlyph* tempGlyph = new FTBitmapGlyph( ftGlyph);
        return tempGlyph;
    }

    err = face.Error();
    return NULL;
}


void FTGLBitmapFont::Render( const char* string)
{   
    glPushClientAttrib( GL_CLIENT_PIXEL_STORE_BIT);
    glPushAttrib( GL_ENABLE_BIT);
    
    glPixelStorei( GL_UNPACK_LSB_FIRST, GL_FALSE);
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1);

    glDisable( GL_BLEND);

    FTFont::Render( string);

    glPopAttrib();
    glPopClientAttrib();
}


void FTGLBitmapFont::Render( const wchar_t* string)
{   
    glPushClientAttrib( GL_CLIENT_PIXEL_STORE_BIT);
    glPushAttrib( GL_ENABLE_BIT);
    
    glPixelStorei( GL_UNPACK_LSB_FIRST, GL_FALSE);
    glPixelStorei( GL_UNPACK_ALIGNMENT, 1);
    
    glDisable( GL_BLEND);

    FTFont::Render( string);

    glPopAttrib();
    glPopClientAttrib();
}


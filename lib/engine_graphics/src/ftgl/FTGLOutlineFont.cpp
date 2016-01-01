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

#include    "FTGLOutlineFont.h"
#include    "FTOutlineGlyph.h"


FTGLOutlineFont::FTGLOutlineFont( const char* fontname)
:   FTFont( fontname)
{}


FTGLOutlineFont::FTGLOutlineFont( const unsigned char *pBufferBytes, size_t bufferSizeInBytes)
:   FTFont( pBufferBytes, bufferSizeInBytes)
{}


FTGLOutlineFont::~FTGLOutlineFont()
{}


FTGlyph* FTGLOutlineFont::MakeGlyph( unsigned int g)
{
    FT_GlyphSlot ftGlyph = face.Glyph( g, FT_LOAD_NO_HINTING);

    if( ftGlyph)
    {
        FTOutlineGlyph* tempGlyph = new FTOutlineGlyph( ftGlyph);
        return tempGlyph;
    }

    err = face.Error();
    return NULL;
}


void FTGLOutlineFont::Render( const char* string)
{   
    glPushAttrib( GL_ENABLE_BIT | GL_HINT_BIT | GL_LINE_BIT | GL_COLOR_BUFFER_BIT);
    
    glDisable( GL_TEXTURE_2D);
    
    glEnable( GL_LINE_SMOOTH);
    glHint( GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
    glEnable(GL_BLEND);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // GL_ONE

    FTFont::Render( string);

    glPopAttrib();
}


void FTGLOutlineFont::Render( const wchar_t* string)
{   
    glPushAttrib( GL_ENABLE_BIT | GL_HINT_BIT | GL_LINE_BIT | GL_COLOR_BUFFER_BIT);
    
    glDisable( GL_TEXTURE_2D);
    
    glEnable( GL_LINE_SMOOTH);
    glHint( GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
    glEnable(GL_BLEND);
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // GL_ONE

    FTFont::Render( string);

    glPopAttrib();
}


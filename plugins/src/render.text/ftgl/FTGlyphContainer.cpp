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

#include    "FTGlyphContainer.h"
#include    "FTGlyph.h"
#include    "FTFace.h"
#include    "FTCharmap.h"


FTGlyphContainer::FTGlyphContainer( FTFace* f)
:   face(f),
    err(0)
{
    glyphs.push_back( NULL);
    charMap = new FTCharmap( face);
}


FTGlyphContainer::~FTGlyphContainer()
{
    GlyphVector::iterator glyphIterator;
    for( glyphIterator = glyphs.begin(); glyphIterator != glyphs.end(); ++glyphIterator)
    {
        delete *glyphIterator;
    }
    
    glyphs.clear();
    delete charMap;
}


bool FTGlyphContainer::CharMap( FT_Encoding encoding)
{
    bool result = charMap->CharMap( encoding);
    err = charMap->Error();
    return result;
}


unsigned int FTGlyphContainer::FontIndex( const unsigned int characterCode) const
{
    return charMap->FontIndex( characterCode);
}


void FTGlyphContainer::Add( FTGlyph* tempGlyph, const unsigned int characterCode)
{
    charMap->InsertIndex( characterCode, glyphs.size());
    glyphs.push_back( tempGlyph);
}


const FTGlyph* const FTGlyphContainer::Glyph( const unsigned int characterCode) const
{
    signed int index = charMap->GlyphListIndex( characterCode);
    return glyphs[index];
}


FTBBox FTGlyphContainer::BBox( const unsigned int characterCode) const
{
    return glyphs[charMap->GlyphListIndex( characterCode)]->BBox();
}


float FTGlyphContainer::Advance( const unsigned int characterCode, const unsigned int nextCharacterCode)
{
    unsigned int left = charMap->FontIndex( characterCode);
    unsigned int right = charMap->FontIndex( nextCharacterCode);

    float width = face->KernAdvance( left, right).x;
    width += glyphs[charMap->GlyphListIndex( characterCode)]->Advance();
    
    return width;
}


FTPoint FTGlyphContainer::Render( const unsigned int characterCode, const unsigned int nextCharacterCode, FTPoint penPosition)
{
    FTPoint kernAdvance;
    float advance = 0;
    
    unsigned int left = charMap->FontIndex( characterCode);
    unsigned int right = charMap->FontIndex( nextCharacterCode);

    kernAdvance = face->KernAdvance( left, right);
        
    if( !face->Error())
    {
        advance = glyphs[charMap->GlyphListIndex( characterCode)]->Render( penPosition);
    }
    
    kernAdvance.x = advance + kernAdvance.x;
//  kernAdvance.y = advance.y + kernAdvance.y;
    return kernAdvance;
}

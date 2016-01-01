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

#include    "FTOutlineGlyph.h"
#include    "FTVectoriser.h"


FTOutlineGlyph::FTOutlineGlyph( FT_GlyphSlot glyph)
:   FTGlyph( glyph),
    glList(0)
{
    if( ft_glyph_format_outline != glyph->format)
    {
        err = 0x14; // Invalid_Outline
        return;
    }

    FTVectoriser vectoriser( glyph);

    size_t numContours = vectoriser.ContourCount();
    if ( ( numContours < 1) || ( vectoriser.PointCount() < 3))
    {
        return;
    }

    glList = glGenLists(1);
    glNewList( glList, GL_COMPILE);
        for( unsigned int c = 0; c < numContours; ++c)
        {
            const FTContour* contour = vectoriser.Contour(c);
            
            glBegin( GL_LINE_LOOP);
                for( unsigned int p = 0; p < contour->PointCount(); ++p)
                {
                    glVertex2f( contour->Point(p).x / 64.0f, contour->Point(p).y / 64.0f);
                }
            glEnd();
        }
    glEndList();
}


FTOutlineGlyph::~FTOutlineGlyph()
{
    glDeleteLists( glList, 1);
}


float FTOutlineGlyph::Render( const FTPoint& pen)
{
    if( glList)
    {
        glTranslatef( pen.x, pen.y, 0);
            glCallList( glList);
        glTranslatef( -pen.x, -pen.y, 0);
    }
    
    return advance;
}


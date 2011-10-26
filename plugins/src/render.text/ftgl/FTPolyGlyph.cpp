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

#include "FTPolyGlyph.h"
#include "FTVectoriser.h"


FTPolyGlyph::FTPolyGlyph( FT_GlyphSlot glyph)
:   FTGlyph( glyph),
    glList(0)
{
    if( ft_glyph_format_outline != glyph->format)
    {
        err = 0x14; // Invalid_Outline
        return;
    }

    FTVectoriser vectoriser( glyph);

    if(( vectoriser.ContourCount() < 1) || ( vectoriser.PointCount() < 3))
    {
        return;
    }

    vectoriser.MakeMesh( 1.0);
    
    glList = glGenLists( 1);
    glNewList( glList, GL_COMPILE);

        const FTMesh* mesh = vectoriser.GetMesh();
        for( unsigned int index = 0; index < mesh->TesselationCount(); ++index)
        {
            const FTTesselation* subMesh = mesh->Tesselation( index);
            unsigned int polyonType = subMesh->PolygonType();

            glBegin( polyonType);
                for( unsigned int x = 0; x < subMesh->PointCount(); ++x)
                {
                    glVertex3f( subMesh->Point(x).x / 64.0f,
                                subMesh->Point(x).y / 64.0f,
                                0.0f);
                }
            glEnd();
        }
    glEndList();
}


FTPolyGlyph::~FTPolyGlyph()
{
    glDeleteLists( glList, 1);
}


float FTPolyGlyph::Render( const FTPoint& pen)
{
    if( glList)
    {
        glTranslatef(  pen.x,  pen.y, 0);
        glCallList( glList);    
        glTranslatef( -pen.x, -pen.y, 0);
    }
    
    return advance;
}

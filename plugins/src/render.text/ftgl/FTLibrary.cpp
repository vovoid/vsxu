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

#include    "FTLibrary.h"


FTLibrary&  FTLibrary::Instance()
{
    static FTLibrary ftlib;
    return ftlib;
}


FTLibrary::~FTLibrary()
{
    if( library != 0)
    {
        FT_Done_FreeType( *library);

        delete library;
        library= 0;
    }

//  if( manager != 0)
//  {
//      FTC_Manager_Done( manager );
//
//      delete manager;
//      manager= 0;
//  }
}


FTLibrary::FTLibrary()
:   library(0),
    err(0)
{
    Initialise();
}


bool FTLibrary::Initialise()
{
    if( library != 0)
        return true;

    library = new FT_Library;
    
    err = FT_Init_FreeType( library);
    if( err)
    {
        delete library;
        library = 0;
        return false;
    }
    
//  FTC_Manager* manager;
//  
//  if( FTC_Manager_New( lib, 0, 0, 0, my_face_requester, 0, manager )
//  {
//      delete manager;
//      manager= 0;
//      return false;
//  }

    return true;
}

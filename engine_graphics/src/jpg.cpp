/**
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Lesser General Public License (LGPL)
*
* VSXu Engine is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU Lesser General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


//#include <iostream>

// Need extern "C", or you'll get linker errors saying jpeg_* functions are undefined


#include <string/vsx_string.h>
#include "vsxg.h"

extern "C" {
#include <jpeglib.h>
#include <jerror.h>
}



/* Expanded data source object for stdio input */

typedef struct {
  struct jpeg_source_mgr pub;	/* public fields */

  vsx_filesystem::file_handle* infile;		/* source stream */
  vsx_filesystem::filesystem* filesystem;
  JOCTET * buffer;		/* start of buffer */
  boolean start_of_file;	/* have we gotten any data yet? */
} my_source_mgr2;

typedef my_source_mgr2 * my_src_ptr;

#define INPUT_BUF_SIZE  4096	/* choose an efficiently fread'able size */


/*
 * Initialize source --- called by jpeg_read_header
 * before any data is actually read.
 */

METHODDEF(void)
init_source (j_decompress_ptr cinfo)
{
  my_src_ptr src = (my_src_ptr) cinfo->src;

  /* We reset the empty-input-file flag for each image,
   * but we don't clear the input buffer.
   * This is correct behavior for reading a series of images from one source.
   */
  src->start_of_file = TRUE;
}


/*
 * Fill the input buffer --- called whenever buffer is emptied.
 *
 * In typical applications, this should read fresh data into the buffer
 * (ignoring the current state of next_input_byte & bytes_in_buffer),
 * reset the pointer & count to the start of the buffer, and return TRUE
 * indicating that the buffer has been reloaded.  It is not necessary to
 * fill the buffer entirely, only to obtain at least one more byte.
 *
 * There is no such thing as an EOF return.  If the end of the file has been
 * reached, the routine has a choice of ERREXIT() or inserting fake data into
 * the buffer.  In most cases, generating a warning message and inserting a
 * fake EOI marker is the best course of action --- this will allow the
 * decompressor to output however much of the image is there.  However,
 * the resulting error message is misleading if the real problem is an empty
 * input file, so we handle that case specially.
 *
 * In applications that need to be able to suspend compression due to input
 * not being available yet, a FALSE return indicates that no more data can be
 * obtained right now, but more may be forthcoming later.  In this situation,
 * the decompressor will return to its caller (with an indication of the
 * number of scanlines it has read, if any).  The application should resume
 * decompression after it has loaded more data into the input buffer.  Note
 * that there are substantial restrictions on the use of suspension --- see
 * the documentation.
 *
 * When suspending, the decompressor will back up to a convenient restart point
 * (typically the start of the current MCU). next_input_byte & bytes_in_buffer
 * indicate where the restart point will be if the current call returns FALSE.
 * Data beyond this point must be rescanned after resumption, so move it to
 * the front of the buffer rather than discarding it.
 */

METHODDEF(boolean)
fill_input_buffer (j_decompress_ptr cinfo)
{
  my_src_ptr src = (my_src_ptr) cinfo->src;
  size_t nbytes;

  nbytes = src->filesystem->f_read(src->buffer,INPUT_BUF_SIZE * sizeof(JOCTET),src->infile);
  //printf("nbytes: %d\n",nbytes);
  //JFREAD(src->infile, src->buffer, INPUT_BUF_SIZE);

  if (nbytes <= 0) {
    if (src->start_of_file)	/* Treat empty input file as fatal error */
      ERREXIT(cinfo, JERR_INPUT_EMPTY);
    WARNMS(cinfo, JWRN_JPEG_EOF);
    /* Insert a fake EOI marker */
    src->buffer[0] = (JOCTET) 0xFF;
    src->buffer[1] = (JOCTET) JPEG_EOI;
    nbytes = 2;
  }

  src->pub.next_input_byte = src->buffer;
  src->pub.bytes_in_buffer = nbytes;
  src->start_of_file = FALSE;

  return TRUE;
}


/*
 * Skip data --- used to skip over a potentially large amount of
 * uninteresting data (such as an APPn marker).
 *
 * Writers of suspendable-input applications must note that skip_input_data
 * is not granted the right to give a suspension return.  If the skip extends
 * beyond the data currently in the buffer, the buffer can be marked empty so
 * that the next read will cause a fill_input_buffer call that can suspend.
 * Arranging for additional bytes to be discarded before reloading the input
 * buffer is the application writer's problem.
 */

METHODDEF(void)
skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
  my_src_ptr src = (my_src_ptr) cinfo->src;

  /* Just a dumb implementation for now.  Could use fseek() except
   * it doesn't work on pipes.  Not clear that being smart is worth
   * any trouble anyway --- large skips are infrequent.
   */
  if (num_bytes > 0) {
    while (num_bytes > (long) src->pub.bytes_in_buffer) {
      num_bytes -= (long) src->pub.bytes_in_buffer;
      (void) fill_input_buffer(cinfo);
      /* note we assume that fill_input_buffer will never return FALSE,
       * so suspension need not be handled.
       */
    }
    src->pub.next_input_byte += (size_t) num_bytes;
    src->pub.bytes_in_buffer -= (size_t) num_bytes;
  }
}


/*
 * An additional method that can be provided by data source modules is the
 * resync_to_restart method for error recovery in the presence of RST markers.
 * For the moment, this source module just uses the default resync method
 * provided by the JPEG library.  That method assumes that no backtracking
 * is possible.
 */


/*
 * Terminate source --- called by jpeg_finish_decompress
 * after all data has been read.  Often a no-op.
 *
 * NB: *not* called by jpeg_abort or jpeg_destroy; surrounding
 * application must deal with any cleanup that should happen even
 * for error exit.
 */

METHODDEF(void)
term_source (j_decompress_ptr cinfo)
{
  VSX_UNUSED(cinfo);
  /* no work necessary here */
}


/*
 * Prepare for input from a stdio stream.
 * The caller must have already opened the stream, and is responsible
 * for closing it after finishing decompression.
 */

GLOBAL(void)
jpeg_stdio2_src (j_decompress_ptr cinfo, vsx_filesystem::file_handle* infile, vsx_filesystem::filesystem* filesystem)
{
  my_src_ptr src;

  /* The source object and input buffer are made permanent so that a series
   * of JPEG images can be read from the same file by calling jpeg_stdio_src
   * only before the first one.  (If we discarded the buffer at the end of
   * one image, we'd likely lose the start of the next one.)
   * This makes it unsafe to use this manager and a different source
   * manager serially with the same JPEG object.  Caveat programmer.
   */
  if (cinfo->src == NULL) {	/* first time for this JPEG object? */
    cinfo->src = (struct jpeg_source_mgr *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
				  sizeof(my_source_mgr2));
    src = (my_src_ptr) cinfo->src;
    src->buffer = (JOCTET *)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
				  INPUT_BUF_SIZE * sizeof(JOCTET));
  }

  src = (my_src_ptr) cinfo->src;
  src->pub.init_source = init_source;
  src->pub.fill_input_buffer = fill_input_buffer;
  src->pub.skip_input_data = skip_input_data;
  src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
  src->pub.term_source = term_source;
  src->infile = infile;
  src->filesystem = filesystem;
  src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
  src->pub.next_input_byte = NULL; /* until buffer loaded */
}




//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************
//*****************************************************************************

CJPEGTest::CJPEGTest()
: m_pBuf(0), m_nResX(0), m_nResY(0) 
{}

bool CJPEGTest::LoadJPEG( const vsx_string<>& strFile, vsx_string<>& strErr,vsx_filesystem::filesystem* filesystem)
{
    // If there's already an image in this object, ditch it
    
    if( m_pBuf )
    {
        free(m_pBuf);
        m_pBuf = 0; 
        m_nResX = 0; 
        m_nResY = 0;     
    }
    // Decode image
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    
    cinfo.err = jpeg_std_error( &jerr );
    jpeg_create_decompress( &cinfo );
    //printf("opening jpeg: %s\n",strFile.c_str());
    vsx_filesystem::file_handle* fp = filesystem->f_open( strFile.c_str(), "rb" );
    if( ! fp )
    {
        strErr = "Failed to open file for reading.";
        return false;
    }
    
    jpeg_stdio2_src( &cinfo, fp, filesystem );
    jpeg_read_header( &cinfo, TRUE );
    
    jpeg_start_decompress( &cinfo );
    
    m_nResX = cinfo.output_width;
    m_nResY = cinfo.output_height;
    
    if( cinfo.out_color_components != 3 && cinfo.out_color_components != 1 )
    {
        strErr = "Image does not have either 1 or 3 color components.";
        return false;
    }
        
    m_pBuf = (unsigned char*)malloc( cinfo.output_width * cinfo.output_height * 3 );
    
    if( cinfo.out_color_components == 3 )
    {
        unsigned char * pWrite = m_pBuf;
        
        // Straightforward - we are supplied with RGB data, which can be written right into our pixel buffer
        
        while( cinfo.output_scanline < cinfo.output_height )
        {            
            int nLinesRead = jpeg_read_scanlines( &cinfo, &pWrite, 1 );
    
            pWrite += nLinesRead * cinfo.output_width * cinfo.output_components;
        };
    }    
    else if( cinfo.out_color_components == 1 )
    {
        // We are reading greyscale data, one (grey) value per pixel. We store
        // three values per pixel (RGB), so we store each grey value three times.
        
        unsigned char * pGreyBuf = new unsigned char[ cinfo.output_width ]; // Buffer for grey values
        
        unsigned char * pWrite = m_pBuf;
        
        while( cinfo.output_scanline < cinfo.output_height )
        {
            int nLinesRead = jpeg_read_scanlines( &cinfo, &pGreyBuf, 1 );
            
            for(unsigned long n = 0 ; n < cinfo.output_width ; ++n )
            {
                pWrite[3*n+0] = pGreyBuf[n];
                pWrite[3*n+1] = pGreyBuf[n];
                pWrite[3*n+2] = pGreyBuf[n];
            }    
            
            pWrite += nLinesRead * cinfo.output_width * 3; // NB the '3'
        };
        
        delete [] pGreyBuf;
    }    
    
    jpeg_finish_decompress( &cinfo );
    
    jpeg_destroy_decompress( &cinfo );
    
    filesystem->f_close( fp );

    return true;
}
    
bool CJPEGTest::SaveJPEG( const vsx_string<>& strFile, vsx_string<>& strErr, const int nQFactor )
{
	if( ! m_pBuf )
	{
		return false;
    }	
    
    // Initialization
    
    struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	
	cinfo.err = jpeg_std_error( &jerr );
	jpeg_create_compress( &cinfo );
	
	// Set output file
	
    FILE * fp = fopen( strFile.c_str(), "wb" );
    if( ! fp )
    {
        strErr = "Unable to open file for writing.";
	    return false;
	}
	
	jpeg_stdio_dest( &cinfo, fp );
	
	//  Set parameters for writing
	
	cinfo.image_width = m_nResX;
	cinfo.image_height = m_nResY;
	cinfo.input_components = 3;
	cinfo.in_color_space = JCS_RGB;
	
    jpeg_set_defaults( &cinfo );
    
    jpeg_set_quality( &cinfo, nQFactor, TRUE );
    
    // Start encoding
    
    jpeg_start_compress( &cinfo, TRUE );
    
    JSAMPLE * pJ = m_pBuf;
    
    while( cinfo.next_scanline < cinfo.image_height )
    {
	    int nRowsWritten = jpeg_write_scanlines( &cinfo, &pJ, 1 );
	    pJ += 3*m_nResX*nRowsWritten;
	}    
	
    jpeg_finish_compress( &cinfo );
    jpeg_destroy_compress( &cinfo );
    
    fclose( fp );
    
    return true;
}    

/*bool CJPEGTest::LoadTGA( const vsx_string<>& strFile, vsx_string<>& strErr )
{
    // If there's already an image in object, ditch it
    
    if( m_pBuf )
    {
        delete [] m_pBuf; 
        m_pBuf = 0; 
        m_nResX = 0; 
        m_nResY = 0;     
    }
    
    // Read header into header struct
    
    FILE *fp;
    tgahdr_t hdr;
    
    fp = fopen( strFile.c_str(), "rb" );
    if( ! fp )
    {
        strErr = "Failed to open input file for reading.";
        return false;
    }
    
    ZeroMemory( &hdr, sizeof( tgahdr_t ) );

    if( fread( &hdr, sizeof( tgahdr_t ), 1, fp ) != 1 )
    {
        strErr = "Failed to read targa header.";
        return false;
    }
    
    m_nResX = hdr.width;
    m_nResY = hdr.height;

    // Data stored as top row to bottom row, or vice versa?
    
    bool bFlipped = (hdr.descbits & 1<<5)? true : false;

    // Allocate pixel data array
    
    m_pBuf = new unsigned char[ 3*m_nResX*m_nResY ];

    for( int y = 0 ; y < m_nResY; ++y )
    {
        // Read into appropriate row, depending on whether file stores rows from top to bottom, or from bottom to top
        
        unsigned char * pWrite = bFlipped ? ( m_pBuf + 3*(y*m_nResX) ) : ( m_pBuf + 3*((m_nResY-1-y)*m_nResX) );
        
        for( int x = 0 ; x < m_nResX ; ++x )
        {
            // Stored in file as BGR, but we store in this object as RGB, so swap about:
            
            *(pWrite+2) = fgetc( fp );
            *(pWrite+1) = fgetc( fp );
            *(pWrite+0) = fgetc( fp );
            
            pWrite += 3;
        }
    }

    fclose( fp );
    
    return true;
}*/

/*bool CJPEGTest::SaveTGA( const vsx_string<>& strFile, vsx_string<>& strErr ) const
{
	if( ! m_pBuf )
	{
		return false;
    }		

    // Fill and write header structure
    
	FILE *fp;
	tgahdr_t hdr;
	int x,y;
	
	fp = fopen( strFile.c_str(), "wb" );
	if( ! fp )
	{
        strErr = "Unable to open file for writing.";
		return false;
	}
	
	ZeroMemory( &hdr, sizeof( tgahdr_t ) );
	
	hdr.idlen = 0;
	hdr.imgtype = 2;
	hdr.width = m_nResX;
	hdr.height = m_nResY;
	hdr.bpp = 24;
	hdr.descbits = 0x20;         //top-bottom, left-right
	
	fwrite( &hdr, sizeof( tgahdr_t ), 1, fp );
	
	// Write data out by rows, bearing in mind that we have RGB data, but store as BGR
	
	for( y = 0 ; y < m_nResY; ++y )
	{
	    unsigned char * pWrite = m_pBuf + 3*(y*m_nResX);
	    
		for( x = 0 ; x < m_nResX ; ++x )
		{
            fputc( *(pWrite+2), fp );
            fputc( *(pWrite+1), fp );
            fputc( *(pWrite+0), fp );
            
            pWrite += 3;
		}
	}	
	    
	fclose( fp );
	return true;    
}    
		*/
int CJPEGTest::GetResX( void ) const
{ 
    return m_nResX; 
}
    
int CJPEGTest::GetResY( void ) const
{ 
    return m_nResY; 
}


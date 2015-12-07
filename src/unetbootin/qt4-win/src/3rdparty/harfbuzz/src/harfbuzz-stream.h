/*******************************************************************
 *
 *  Copyright 2005  David Turner, The FreeType Project (www.freetype.org)
 *  Copyright 2007  Trolltech ASA
 *
 *  This is part of HarfBuzz, an OpenType Layout engine library.
 *
 *  See the file name COPYING for licensing information.
 *
 ******************************************************************/
#ifndef HARFBUZZ_STREAM_H
#define HARFBUZZ_STREAM_H

#include <harfbuzz-global.h>

HB_BEGIN_HEADER

typedef struct HB_StreamRec_
{
    HB_Byte*       base;
    HB_UInt        size;
    HB_UInt        pos;
    
    HB_Byte*       cursor;
} HB_StreamRec;

void HB_close_stream(HB_Stream stream);


/* stream macros used by the OpenType parser */
#define  FILE_Pos()      stream->pos
#define  FILE_Seek(pos)  (error = (_hb_stream_seek( stream, pos )))
#define  ACCESS_Frame(size)  (error = (_hb_stream_frame_enter( stream, size ))) != 0
#define  FORGET_Frame()      _hb_stream_frame_exit( stream )

#define  GET_Byte()      (*stream->cursor++)
#define  GET_Short()     (stream->cursor += 2, (HB_Short)( \
                              (*((stream->cursor)-2) << 8) |    \
                               *((stream->cursor)-1)             \
                              ))
#define  GET_Long()      (stream->cursor += 4, (HB_Int)( \
				(*((stream->cursor)-4) << 24) | \
				(*((stream->cursor)-3) << 16) | \
				(*((stream->cursor)-2) << 8) | \
				 *((stream->cursor)-1) \
			 ))


#define  GET_Char()      ((HB_Char)GET_Byte())
#define  GET_UShort()    ((HB_UShort)GET_Short())
#define  GET_ULong()     ((HB_UInt)GET_Long())
#define  GET_Tag4()      GET_ULong()

HB_Int _hb_stream_pos( HB_Stream stream );

HB_Error _hb_stream_seek( HB_Stream stream, HB_UInt pos );

HB_Error _hb_stream_frame_enter( HB_Stream stream, HB_UInt size );

void _hb_stream_frame_exit( HB_Stream  stream );

HB_END_HEADER

#endif

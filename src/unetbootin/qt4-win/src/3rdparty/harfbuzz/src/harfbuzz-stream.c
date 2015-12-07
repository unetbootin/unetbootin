/*******************************************************************
 *
 *  Copyright 2007  Trolltech ASA
 *
 *  This is part of HarfBuzz, an OpenType Layout engine library.
 *
 *  See the file name COPYING for licensing information.
 *
 ******************************************************************/
#include "harfbuzz-stream.h"
#include <stdlib.h>

#if 0
#include <stdio.h>
#define  LOG(x)  _hb_log x

static void
_hb_log( const char*   format, ... )
{
  va_list  ap;
 
  va_start( ap, format );
  vfprintf( stderr, format, ap );
  va_end( ap );
}

#else
#define  LOG(x)  do {} while (0)
#endif

void HB_close_stream(HB_Stream stream)
{
    if (!stream)
        return;
    free(stream->base);
    free(stream);
}


HB_Int _hb_stream_pos(HB_Stream stream)
{
  LOG(( "stream:pos() -> %ld\n", stream->pos ));
  return stream->pos;
}


HB_Error _hb_stream_seek(HB_Stream stream, HB_UInt pos)
{
    HB_Error  error = 0;
    
    stream->pos = pos;
    if (pos > stream->size)
        error = HB_Err_Invalid_Stream_Operation;
    
    LOG(( "stream:seek(%ld) -> %d\n", pos, error ));
    return error;
}


HB_Error _hb_stream_frame_enter(HB_Stream stream, HB_UInt count)
{
    /* check current and new position */
    if (stream->pos + count > stream->size) 
        return HB_Err_Invalid_Stream_Operation;

    /* set cursor */
    stream->cursor = stream->base + stream->pos;
    stream->pos   += count;

    LOG(( "stream:frame_enter(%ld) -> OK\n", count ));
    return HB_Err_Ok;
}


void _hb_stream_frame_exit(HB_Stream  stream)
{
  stream->cursor = NULL;

  LOG(( "stream:frame_exit()\n" ));
}

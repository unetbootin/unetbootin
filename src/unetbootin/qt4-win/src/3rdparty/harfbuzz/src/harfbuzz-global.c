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
#include "harfbuzz-global.h"

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


HB_Pointer _hb_alloc(HB_UInt   size,
                            HB_Error  *perror )
{
  HB_Error    error = 0;
  HB_Pointer  block = NULL;

  if ( size > 0 )
  {
    block = malloc( size );
    if ( !block )
      error = HB_Err_Out_Of_Memory;
    else
      memset( (char*)block, 0, (size_t)size );
  }

  *perror = error;
  return block;
}


HB_Pointer _hb_realloc(HB_Pointer  block,
                       HB_UInt    old_size,
                       HB_UInt    new_size,
                       HB_Error   *perror )
{
    HB_Pointer  block2 = NULL;
    HB_Error    error  = 0;

    block2 = realloc( block, new_size );
    if ( block2 == NULL )
        error = HB_Err_Out_Of_Memory;
    else if ( new_size > old_size )
        memset( (char*)block2 + old_size, 0, (size_t)(new_size - old_size) );

    if ( !error )
        block = block2;
    
    *perror = error;
    return block;
}


void _hb_free( HB_Pointer  block )
{
  if ( block )
    free( block );
}


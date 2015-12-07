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
#ifndef HARFBUZZ_GLOBAL_H
#define HARFBUZZ_GLOBAL_H

#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
#define HB_BEGIN_HEADER  extern "C" {
#define HB_END_HEADER  }
#else
#define HB_BEGIN_HEADER  /* nothing */
#define HB_END_HEADER  /* nothing */
#endif

HB_BEGIN_HEADER

typedef char hb_int8;
typedef unsigned char hb_uint8;
typedef short hb_int16;
typedef unsigned short hb_uint16;
typedef int hb_int32;
typedef unsigned int hb_uint32;

typedef hb_uint8 HB_Bool;

typedef hb_uint8 HB_Byte;
typedef hb_uint16 HB_UShort;
typedef hb_uint32 HB_UInt;
typedef hb_int8 HB_Char;
typedef hb_int16 HB_Short;
typedef hb_int32 HB_Int;

typedef hb_uint16 HB_UChar16;
typedef hb_uint32 HB_UChar32;
typedef hb_uint32 HB_Glyph;
typedef hb_int32 HB_Fixed; /* 26.6 */

#define HB_FIXED_CONSTANT(v) ((v) * 64)
#define HB_FIXED_ROUND(v) (((v)+32) & -64)

typedef hb_int32 HB_16Dot16; /* 16.16 */

typedef void * HB_Pointer;
typedef hb_uint32 HB_Tag;

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif

typedef enum {
    HB_Err_Ok = 0,
    HB_Err_Invalid_Stream_Operation,
    HB_Err_Invalid_Argument,
    HB_Err_Out_Of_Memory,
    HB_Err_Invalid_Face_Handle, 
    HB_Err_Table_Missing,
    HB_Err_Invalid_SubTable_Format  = 0x1000,
    HB_Err_Invalid_SubTable         = 0x1001,
    HB_Err_Not_Covered              = 0x1002,
    HB_Err_Too_Many_Nested_Contexts = 0x1003,
    HB_Err_No_MM_Interpreter        = 0x1004,
    HB_Err_Empty_Script             = 0x1005,
    HB_Err_Invalid_GSUB_SubTable_Format = 0x1010,
    HB_Err_Invalid_GSUB_SubTable        = 0x1011,
    HB_Err_Invalid_GPOS_SubTable_Format = 0x1020,
    HB_Err_Invalid_GPOS_SubTable        = 0x1021,
    HB_Err_Invalid_GDEF_SubTable_Format = 0x1030,
    HB_Err_Invalid_GDEF_SubTable        = 0x1031
} HB_Error;

typedef struct {
    HB_Fixed x;
    HB_Fixed y;
} HB_FixedPoint;

typedef struct HB_Font_ *HB_Font;
typedef struct HB_StreamRec_ *HB_Stream;
typedef struct HB_FaceRec_ *HB_Face;

#define HB_IsHighSurrogate(ucs) \
    (((ucs) & 0xfc00) == 0xd800)

#define HB_IsLowSurrogate(ucs) \
    (((ucs) & 0xfc00) == 0xdc00)

#define HB_SurrogateToUcs4(high, low) \
    (((HB_UChar32)(high))<<10) + (low) - 0x35fdc00;

#define HB_MAKE_TAG( _x1, _x2, _x3, _x4 ) \
          ( ( (HB_UInt)_x1 << 24 ) |     \
            ( (HB_UInt)_x2 << 16 ) |     \
            ( (HB_UInt)_x3 <<  8 ) |     \
              (HB_UInt)_x4         )

/* memory macros used by the OpenType parser */
#define  ALLOC(_ptr,_size)   \
           ( (_ptr) = _hb_alloc( _size, &error ), error != 0 )

#define  REALLOC(_ptr,_oldsz,_newsz)  \
           ( (_ptr) = _hb_realloc( (_ptr), (_oldsz), (_newsz), &error ), error != 0 )

#define  FREE(_ptr)                    \
  do {                                 \
    if ( (_ptr) )                      \
    {                                  \
      _hb_free( _ptr );     \
      _ptr = NULL;                     \
    }                                  \
  } while (0)

#define  ALLOC_ARRAY(_ptr,_count,_type)   \
           ALLOC(_ptr,(_count)*sizeof(_type))

#define  REALLOC_ARRAY(_ptr,_oldcnt,_newcnt,_type) \
           REALLOC(_ptr,(_oldcnt)*sizeof(_type),(_newcnt)*sizeof(_type))

#define  MEM_Copy(dest,source,count)   memcpy( (char*)(dest), (const char*)(source), (size_t)(count) )


HB_Pointer _hb_alloc( HB_UInt   size,
                      HB_Error  *perror_ );

HB_Pointer _hb_realloc( HB_Pointer  block,
                        HB_UInt    old_size,
                        HB_UInt    new_size,
                        HB_Error   *perror_ );

void _hb_free( HB_Pointer  block );

HB_END_HEADER

#endif

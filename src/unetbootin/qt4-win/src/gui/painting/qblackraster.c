/****************************************************************************
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** This file contains third party code which is not governed by the Qt
** Commercial License Agreement. Please read the license headers below
** for more information.
** 
** Further information about Qt licensing is available at:
** http://www.trolltech.com/products/qt/licensing.html or by
** contacting info@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

/***************************************************************************/
/*                                                                         */
/*  qblackraster.c, derived from ftraster.c                                */
/*                                                                         */
/*    The FreeType glyph rasterizer (body).                                */
/*                                                                         */
/*  Copyright 1996-2001, 2002, 2003 by                                     */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, ../../3rdparty/freetype/docs/FTL.TXT.  By continuing to use,  */
/*  modify, or distribute this file you indicate that you have read        */
/*  the license and understand and accept it fully.                        */
/*                                                                         */
/***************************************************************************/

/*************************************************************************/
/*                                                                       */
/* This is a rewrite of the FreeType 1.x scan-line converter             */
/*                                                                       */
/*************************************************************************/

/* #define Q_RASTER_DEBUG */

#include <qglobal.h>
#include <stdlib.h>

typedef int QT_FT_Error;
typedef int QT_FT_Int;
typedef unsigned int QT_FT_UInt;

#if defined(Q_WS_WIN64)
typedef signed __int64	 QT_FT_F26Dot6;
typedef signed __int64   QT_FT_Long;
typedef unsigned __int64 QT_FT_ULong;
#else
typedef signed long		 QT_FT_F26Dot6;
typedef signed long      QT_FT_Long;
typedef unsigned long    QT_FT_ULong;
#endif

#define QT_FT_Int64  qint64
#define QT_FT_Byte uchar

#define QT_FT_TRACE1 if (0) printf
#define QT_FT_TRACE6 if (0) printf

#define Raster_Err_Ok 0
#define Raster_Err_Cannot_Render_Glyph 1

#define QT_FT_LOCAL_DEF(x) static x

#define QT_FT_BEGIN_HEADER
#define QT_FT_END_HEADER
#include <private/qrasterdefs_p.h>
#include <private/qblackraster_p.h>

static QT_FT_Long QT_FT_MulDiv(QT_FT_Long  a, QT_FT_Long  b, QT_FT_Long  c)
{
    QT_FT_Int   s;
    QT_FT_Long  d;

    s = 1;
    if ( a < 0 ) { a = -a; s = -1; }
    if ( b < 0 ) { b = -b; s = -s; }
    if ( c < 0 ) { c = -c; s = -s; }

    d = (QT_FT_Long)( c > 0 ? ( (QT_FT_Int64)a * b + ( c >> 1 ) ) / c
                   : 0x7FFFFFFFL );

    return ( s > 0 ) ? d : -d;
}

#include <string.h>
#define QT_FT_MEM_ZERO(x, len) memset(x, 0, len);
#include <stdio.h>

#define MAX(x, y) (x > y ? x : y)
#define MIN(x, y) (x < y ? x : y)
#define MAX_SPANS 256

/*************************************************************************/
/*                                                                       */
/* A simple technical note on how the raster works                       */
/* -----------------------------------------------                       */
/*                                                                       */
/*   Converting an outline into a bitmap is achieved in several steps:   */
/*                                                                       */
/*   1 - Decomposing the outline into successive `profiles'.  Each       */
/*       profile is simply an array of scanline intersections on a given */
/*       dimension.  A profile's main attributes are                     */
/*                                                                       */
/*       o its scanline position boundaries, i.e. `Ymin' and `Ymax'.     */
/*                                                                       */
/*       o an array of intersection coordinates for each scanline        */
/*         between `Ymin' and `Ymax'.                                    */
/*                                                                       */
/*       o a direction, indicating whether it was built going `up' or    */
/*         `down', as this is very important for filling rules.          */
/*                                                                       */
/*   2 - Sweeping the target map's scanlines in order to compute segment */
/*       `spans' which are then filled.  Additionally, this pass         */
/*       performs drop-out control.                                      */
/*                                                                       */
/*   The outline data is parsed during step 1 only.  The profiles are    */
/*   built from the bottom of the render pool, used as a stack.  The     */
/*   following graphics shows the profile list under construction:       */
/*                                                                       */
/*     ____________________________________________________________ _ _  */
/*    |         |                   |         |                 |        */
/*    | profile | coordinates for   | profile | coordinates for |-->     */
/*    |    1    |  profile 1        |    2    |  profile 2      |-->     */
/*    |_________|___________________|_________|_________________|__ _ _  */
/*                                                                       */
/*    ^                                                         ^        */
/*    |                                                         |        */
/*  start of render pool                                       top       */
/*                                                                       */
/*   The top of the profile stack is kept in the `top' variable.         */
/*                                                                       */
/*   As you can see, a profile record is pushed on top of the render     */
/*   pool, which is then followed by its coordinates/intersections.  If  */
/*   a change of direction is detected in the outline, a new profile is  */
/*   generated until the end of the outline.                             */
/*                                                                       */
/*   Note that when all profiles have been generated, the function       */
/*   Finalize_Profile_Table() is used to record, for each profile, its   */
/*   bottom-most scanline as well as the scanline above its upmost       */
/*   boundary.  These positions are called `y-turns' because they (sort  */
/*   of) correspond to local extrema.  They are stored in a sorted list  */
/*   built from the top of the render pool as a downwards stack:         */
/*                                                                       */
/*      _ _ _______________________________________                      */
/*                            |                    |                     */
/*                         <--| sorted list of     |                     */
/*                         <--|  extrema scanlines |                     */
/*      _ _ __________________|____________________|                     */
/*                                                                       */
/*                            ^                    ^                     */
/*                            |                    |                     */
/*                         maxBuff           sizeBuff = end of pool      */
/*                                                                       */
/*   This list is later used during the sweep phase in order to          */
/*   optimize performance (see technical note on the sweep below).       */
/*                                                                       */
/*   Of course, the raster detects whether the two stacks collide and    */
/*   handles the situation propertly.                                    */
/*                                                                       */
/*************************************************************************/


/*************************************************************************/
/*************************************************************************/
/**                                                                     **/
/**  CONFIGURATION MACROS                                               **/
/**                                                                     **/
/*************************************************************************/
/*************************************************************************/

/* define DEBUG_RASTER if you want to compile a debugging version */
#define xxxDEBUG_RASTER

/* The default render pool size in bytes */
#define RASTER_RENDER_POOL  8192


/* The size of the two-lines intermediate bitmap used */
/* for anti-aliasing, in bytes.                       */
#define RASTER_GRAY_LINES  2048


/*************************************************************************/
/*************************************************************************/
/**                                                                     **/
/**  OTHER MACROS (do not change)                                       **/
/**                                                                     **/
/*************************************************************************/
/*************************************************************************/

/*************************************************************************/
/*                                                                       */
/* The macro QT_FT_COMPONENT is used in trace mode.  It is an implicit      */
/* parameter of the QT_FT_TRACE() and QT_FT_ERROR() macros, used to print/log  */
/* messages during execution.                                            */
/*                                                                       */
#undef  QT_FT_COMPONENT
#define QT_FT_COMPONENT  trace_raster


/* This macro is used to indicate that a function parameter is unused. */
/* Its purpose is simply to reduce compiler warnings.  Note also that  */
/* simply defining it as `(void)x' doesn't avoid warnings with certain */
/* ANSI compilers (e.g. LCC).                                          */
#define QT_FT_UNUSED( x )  (x) = (x)

/* Disable the tracing mechanism for simplicity -- developers can      */
/* activate it easily by redefining these two macros.                  */
#ifndef QT_FT_ERROR
#define QT_FT_ERROR( x )  do ; while ( 0 )     /* nothing */
#endif

#ifndef QT_FT_TRACE
#define QT_FT_TRACE( x )  do ; while ( 0 )     /* nothing */
#endif

#define Raster_Err_None          0
#define Raster_Err_Not_Ini      -1
#define Raster_Err_Overflow     -2
#define Raster_Err_Neg_Height   -3
#define Raster_Err_Invalid      -4
#define Raster_Err_Unsupported  -5
#define Raster_Err_OutOfMemory  -6


#ifndef QT_FT_MEM_SET
#define QT_FT_MEM_SET( d, s, c )  qt_ft_memset( d, s, c )
#endif


/* FMulDiv means `Fast MulDiv'; it is used in case where `b' is       */
/* typically a small value and the result of a*b is known to fit into */
/* 32 bits.                                                           */
#define FMulDiv( a, b, c )  ( (a) * (b) / (c) )

/* On the other hand, SMulDiv means `Slow MulDiv', and is used typically */
/* for clipping computations.  It simply uses the QT_FT_MulDiv() function   */
/* defined in `ftcalc.h'.                                                */
#define SMulDiv  QT_FT_MulDiv

/* The rasterizer is a very general purpose component; please leave */
/* the following redefinitions there (you never know your target    */
/* environment).                                                    */

#ifndef TRUE
#define TRUE   1
#endif

#ifndef FALSE
#define FALSE  0
#endif

#ifndef NULL
#define NULL  (void*)0
#endif

#ifndef SUCCESS
#define SUCCESS  0
#endif

#ifndef FAILURE
#define FAILURE  1
#endif


#define MaxBezier  32   /* The maximum number of stacked Bezier curves. */
                        /* Setting this constant to more than 32 is a   */
                        /* pure waste of space.                         */

#define Pixel_Bits  6   /* fractional bits of *input* coordinates */


/*************************************************************************/
/*************************************************************************/
/**                                                                     **/
/**  SIMPLE TYPE DECLARATIONS                                           **/
/**                                                                     **/
/*************************************************************************/
/*************************************************************************/

typedef int             Int;
typedef unsigned int    UInt;
typedef short           Short;
typedef unsigned short  UShort, *PUShort;

#if defined(Q_WS_WIN64)
typedef __int64          Long, *PLong;
typedef unsigned __int64 ULong;
#else
typedef long             Long, *PLong;
typedef unsigned long    ULong;
#endif

typedef unsigned char   Byte, *PByte;
typedef char            Bool;


typedef union  Alignment_
{
    long    l;
    void*   p;
    void  (*f)(void);

} Alignment, *PAlignment;


typedef struct  TPoint_
{
    Long  x;
    Long  y;

} TPoint;


typedef enum  TFlow_
{
    Flow_None = 0,
    Flow_Up   = 1,
    Flow_Down = -1

} TFlow;


/* States of each line, arc, and profile */
typedef enum  TStates_
{
    Unknown_State,
    Ascending_State,
    Descending_State,
    Flat_State

} TStates;


typedef struct TProfile_  TProfile;
typedef TProfile*         PProfile;

struct  TProfile_
{
    QT_FT_F26Dot6  X;           /* current coordinate during sweep        */
    PProfile    link;        /* link to next profile - various purpose */
    PLong       offset;      /* start of profile's data in render pool */
    int         flow;        /* Profile orientation: Asc/Descending    */
    long        height;      /* profile's height in scanlines          */
    long        start;       /* profile's starting scanline            */

    unsigned    countL;      /* number of lines to step before this    */
                             /* profile becomes drawable               */
};

typedef PProfile   TProfileList;
typedef PProfile*  PProfileList;


/* Simple record used to implement a stack of bands, required */
/* by the sub-banding mechanism                               */
typedef struct  TBand_
{
    Short  y_min;   /* band's minimum */
    Short  y_max;   /* band's maximum */

} TBand;


#define AlignProfileSize                                                \
    ( ( sizeof ( TProfile ) + ( sizeof ( TProfile ) % sizeof ( Alignment ) ) ) / sizeof ( Long ) )


typedef struct TRaster_Instance_  TRaster_Instance;

#define precision_bits 8
#define precision_step (1 << (precision_bits - 1))
#define precision (1 << precision_bits)
#define precision_shift (precision_bits - Pixel_Bits)

/* only used for debugging */
#define FIXED_TO_FLOAT(x) ((x)/(float) precision)
#define INT_TO_FIXED(x) ((x) << precision_bits)

/* NOTE: These operations are only valid on 2's complement processors */

#define FLOOR( x )    ( (x) & -precision )
#define CEILING( x )  ( ( (x) + precision - 1 ) & -precision )
#define TRUNC( x )    ( (signed long)(x) >> precision_bits )
#define FRAC( x )     ( (x) & ( precision - 1 ) )
#define SCALED( x )   ( ( (x) << precision_shift ) )
#define ROUND( x )    ( TRUNC((x) + precision_step) )
#define HALF          ( (Int)(0.5 * precision) )

/* Note that I have moved the location of some fields in the */
/* structure to ensure that the most used variables are used */
/* at the top.  Thus, their offset can be coded with less    */
/* opcodes, and it results in a smaller executable.          */

struct  TRaster_Instance_
{
    PLong     buff;                 /* The profiles buffer                 */
    PLong     sizeBuff;             /* Render pool size                    */
    PLong     maxBuff;              /* Profiles buffer size                */
    PLong     top;                  /* Current cursor in buffer            */

    QT_FT_Error  error;

    Int       numTurns;             /* number of Y-turns in outline        */

    TPoint*   arc;                  /* current Bezier arc pointer          */

    Long      lastX, lastY, minY, maxY;
    Short     minX_dev, maxX_dev;   /* vertical bounds in device coords    */

    UShort    num_Profs;            /* current number of profiles          */

    Bool      fresh;                /* signals a fresh new profile which   */
                                    /* 'start' field must be completed     */
    PProfile  cProfile;             /* current profile                     */
    PProfile  fProfile;             /* head of linked list of profiles     */
    PProfile  gProfile;             /* contour's first profile in case     */
                                    /* of impact                           */

    TStates   state;                /* rendering state                     */

    QT_FT_Outline  outline;

    TPoint    arcs[3 * MaxBezier + 1]; /* The Bezier stack                 */

    TBand     band_stack[16];       /* band stack used for sub-banding     */
    Int       band_top;             /* band stack top                      */

    QT_FT_SpanFunc black_spans;
    void *      user_data;
    Bool        odd_even;           /* True for odd even fills             */
    QT_FT_BBox     clip_box;           /* The clipping box */
};


#define ras  (*raster)


/*************************************************************************/
/*************************************************************************/
/**                                                                     **/
/**  PROFILES COMPUTATION                                               **/
/**                                                                     **/
/*************************************************************************/
/*************************************************************************/

/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    New_Profile                                                        */
/*                                                                       */
/* <Description>                                                         */
/*    Creates a new profile in the render pool.                          */
/*                                                                       */
/* <Input>                                                               */
/*    aState :: The state/orientation of the new profile.                */
/*                                                                       */
/* <Return>                                                              */
/*   SUCCESS on success.  FAILURE in case of overflow or of incoherent   */
/*   profile.                                                            */
/*                                                                       */
static Bool
New_Profile( TRaster_Instance*  raster, TStates  aState )
{
    if ( !ras.fProfile )
    {
        ras.cProfile  = (PProfile)ras.top;
        ras.fProfile  = ras.cProfile;
        ras.top      += AlignProfileSize;
    }

    if ( ras.top >= ras.maxBuff )
    {
        ras.error = Raster_Err_Overflow;
        return FAILURE;
    }

    switch ( aState )
    {
    case Ascending_State:
        ras.cProfile->flow = Flow_Up;
        QT_FT_TRACE6( "\nNew ascending profile = %p\n", ras.cProfile );
        break;

    case Descending_State:
        ras.cProfile->flow = Flow_Down;
        QT_FT_TRACE6( "\nNew descending profile = %p\n", ras.cProfile );
        break;

    default:
        QT_FT_ERROR(( "New_Profile: invalid profile direction!\n" ));
        ras.error = Raster_Err_Invalid;
        return FAILURE;
    }

    ras.cProfile->start  = 0;
    ras.cProfile->height = 0;
    ras.cProfile->offset = ras.top;
    ras.cProfile->link   = (PProfile)0;

    if ( !ras.gProfile )
        ras.gProfile = ras.cProfile;

    ras.state = aState;
    ras.fresh = TRUE;

    return SUCCESS;
}


/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    End_Profile                                                        */
/*                                                                       */
/* <Description>                                                         */
/*    Finalizes the current profile.                                     */
/*                                                                       */
/* <Return>                                                              */
/*    SUCCESS on success.  FAILURE in case of overflow or incoherency.   */
/*                                                                       */
static Bool
End_Profile( TRaster_Instance*  raster )
{
    Long      h;

    h = (Long)( ras.top - ras.cProfile->offset );

    if ( h < 0 )
    {
        QT_FT_ERROR(( "End_Profile: negative height encountered!\n" ));
        ras.error = Raster_Err_Neg_Height;
        return FAILURE;
    }

    if ( h > 0 )
    {
        QT_FT_TRACE6( "Ending profile %p, start = %ld, height = %ld top=%p\n\n",
                   ras.cProfile, ras.cProfile->start, h,ras.top );

        ras.cProfile->height = (long)h;
        ras.cProfile         = (PProfile)ras.top;

        ras.top             += AlignProfileSize;

        ras.cProfile->height = 0;
        ras.cProfile->offset = ras.top;
        ras.num_Profs++;
    }

    if ( ras.top >= ras.maxBuff )
    {
        QT_FT_TRACE1( "overflow in End_Profile\n" );
        ras.error = Raster_Err_Overflow;
        return FAILURE;
    }

    return SUCCESS;
}


/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    Insert_Y_Turn                                                      */
/*                                                                       */
/* <Description>                                                         */
/*    Inserts a salient into the sorted list placed on top of the render */
/*    pool.                                                              */
/*                                                                       */
/* <Input>                                                               */
/*    New y scanline position.                                           */
/*                                                                       */
/* <Return>                                                              */
/*    SUCCESS on success.  FAILURE in case of overflow.                  */
/*                                                                       */
static Bool
Insert_Y_Turn( TRaster_Instance*  raster, Int  y )
{
    PLong  y_turns;
    Int    y2, n;


    n       = ras.numTurns - 1;
    y_turns = ras.sizeBuff - ras.numTurns;

    /* look for first y value that is <= */
    while ( n >= 0 && y < y_turns[n] )
        n--;

    /* if it is <, simply insert it, ignore if == */
    if ( n >= 0 && y > y_turns[n] )
        while ( n >= 0 )
        {
            y2 = (Int)y_turns[n];
            y_turns[n] = y;
            y = y2;
            n--;
        }

    if ( n < 0 )
    {
        ras.maxBuff--;
        if ( ras.maxBuff <= ras.top )
        {
            ras.error = Raster_Err_Overflow;
            return FAILURE;
        }
        ras.numTurns++;
        ras.sizeBuff[-ras.numTurns] = y;
    }

    return SUCCESS;
}


/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    Finalize_Profile_Table                                             */
/*                                                                       */
/* <Description>                                                         */
/*    Adjusts all links in the profiles list.                            */
/*                                                                       */
/* <Return>                                                              */
/*    SUCCESS on success.  FAILURE in case of overflow.                  */
/*                                                                       */
static Bool
Finalize_Profile_Table( TRaster_Instance*  raster )
{
    Int       bottom, top;
    UShort    n;
    PProfile  p;


    n = ras.num_Profs;

    if ( n > 1 )
    {
        p = ras.fProfile;
        while ( n > 0 )
        {
            if ( n > 1 )
                p->link = (PProfile)( p->offset + p->height );
            else
                p->link = NULL;

            switch ( p->flow )
            {
            case Flow_Down:
                bottom     = (Int)( p->start - p->height + 1 );
                top        = (Int)p->start;
                p->start   = bottom;
                p->offset += p->height - 1;
                break;

            case Flow_Up:
            default:
                bottom = (Int)p->start;
                top    = (Int)( p->start + p->height - 1 );
            }

            if ( Insert_Y_Turn( raster, bottom )   ||
                 Insert_Y_Turn( raster, top + 1 )  )
                return FAILURE;

            p = p->link;
            n--;
        }
    }
    else
        ras.fProfile = NULL;

    return SUCCESS;
}


/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    Split_Conic                                                        */
/*                                                                       */
/* <Description>                                                         */
/*    Subdivides one conic Bezier into two joint sub-arcs in the Bezier  */
/*    stack.                                                             */
/*                                                                       */
/* <Input>                                                               */
/*    None (subdivided Bezier is taken from the top of the stack).       */
/*                                                                       */
/* <Note>                                                                */
/*    This routine is the `beef' of this component.  It is  _the_ inner  */
/*    loop that should be optimized to get the best performance.         */
/*                                                                       */
static void
Split_Conic( TPoint*  base )
{
    Long  a, b;


    base[4].x = base[2].x;
    b = base[1].x;
    a = base[3].x = ( base[2].x + b ) / 2;
    b = base[1].x = ( base[0].x + b ) / 2;
    base[2].x = ( a + b ) / 2;

    base[4].y = base[2].y;
    b = base[1].y;
    a = base[3].y = ( base[2].y + b ) / 2;
    b = base[1].y = ( base[0].y + b ) / 2;
    base[2].y = ( a + b ) / 2;

    /* hand optimized.  gcc doesn't seem to be too good at common      */
    /* expression substitution and instruction scheduling ;-)          */
}


/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    Split_Cubic                                                        */
/*                                                                       */
/* <Description>                                                         */
/*    Subdivides a third-order Bezier arc into two joint sub-arcs in the */
/*    Bezier stack.                                                      */
/*                                                                       */
/* <Note>                                                                */
/*    This routine is the `beef' of the component.  It is one of _the_   */
/*    inner loops that should be optimized to get the best performance.  */
/*                                                                       */
static void
Split_Cubic( TPoint*  base )
{
    Long  a, b, c, d;


    base[6].x = base[3].x;
    c = base[1].x;
    d = base[2].x;
    base[1].x = a = ( base[0].x + c + 1 ) >> 1;
    base[5].x = b = ( base[3].x + d + 1 ) >> 1;
    c = ( c + d + 1 ) >> 1;
    base[2].x = a = ( a + c + 1 ) >> 1;
    base[4].x = b = ( b + c + 1 ) >> 1;
    base[3].x = ( a + b + 1 ) >> 1;

    base[6].y = base[3].y;
    c = base[1].y;
    d = base[2].y;
    base[1].y = a = ( base[0].y + c + 1 ) >> 1;
    base[5].y = b = ( base[3].y + d + 1 ) >> 1;
    c = ( c + d + 1 ) >> 1;
    base[2].y = a = ( a + c + 1 ) >> 1;
    base[4].y = b = ( b + c + 1 ) >> 1;
    base[3].y = ( a + b + 1 ) >> 1;
}


/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    Line_Up                                                            */
/*                                                                       */
/* <Description>                                                         */
/*    Computes the x-coordinates of an ascending line segment and stores */
/*    them in the render pool.                                           */
/*                                                                       */
/* <Input>                                                               */
/*    x1   :: The x-coordinate of the segment's start point.             */
/*                                                                       */
/*    y1   :: The y-coordinate of the segment's start point.             */
/*                                                                       */
/*    x2   :: The x-coordinate of the segment's end point.               */
/*                                                                       */
/*    y2   :: The y-coordinate of the segment's end point.               */
/*                                                                       */
/*    miny :: A lower vertical clipping bound value.                     */
/*                                                                       */
/*    maxy :: An upper vertical clipping bound value.                    */
/*                                                                       */
/* <Return>                                                              */
/*    SUCCESS on success, FAILURE on render pool overflow.               */
/*                                                                       */
static Bool
Line_Up( TRaster_Instance*  raster, Long  x1,
         Long  y1,
         Long  x2,
         Long  y2,
         Long  miny,
         Long  maxy )
{
    Long   Dx, Dy;
    Int    e1, e2, f1, size;     /* XXX: is `Short' sufficient? */
    Long   Ix, Rx, Ax;
    Int clipped;
    PLong  top;

    clipped = 0;

    Dx = x2 - x1;
    Dy = y2 - y1;

    if ( Dy <= 0 || y2 < miny || y1 > maxy )
        return SUCCESS;

    if ( y1 < miny )
    {
        /* Take care: miny-y1 can be a very large value; we use     */
        /*            a slow MulDiv function to avoid clipping bugs */
        x1 += SMulDiv( Dx, miny - y1, Dy );
        e1  = (Int)TRUNC( CEILING(miny) );
        f1  = 0;
        clipped = 1;
    }
    else
    {
        e1 = (Int)TRUNC( CEILING(y1) );
        f1 = (Int)FRAC( y1 );
    }

    if ( y2 > maxy )
    {
        /* x2 += FMulDiv( Dx, maxy - y2, Dy );  UNNECESSARY */
        e2  = (Int)TRUNC( FLOOR(maxy) );
    }
    else
    {
        e2 = (Int)TRUNC( FLOOR(y2) );
        if (FRAC(y2) == 0 && ras.cProfile->flow == Flow_Up)
            --e2;
    }

    QT_FT_TRACE6("Line_Up y1=%f, y2=%f, e1=%d, e2=%d f1=%d\n", FIXED_TO_FLOAT(y1),  FIXED_TO_FLOAT(y2), e1, e2, f1);

    if ( f1 > 0 )
    {
        x1 += FMulDiv( Dx, precision - f1, Dy );
    } else if (ras.cProfile->flow == Flow_Down && !clipped ) {
        e1++;
        x1 += FMulDiv( Dx, precision, Dy);
    }

    if ( ras.fresh )
    {
        ras.cProfile->start = e1;
        ras.fresh           = FALSE;
    }
    QT_FT_TRACE6("e1 = start=%d, e2=%d\n", e1, e2);

    size = e2 - e1 + 1;
    if ( ras.top + size >= ras.maxBuff )
    {
        ras.error = Raster_Err_Overflow;
        return FAILURE;
    }

    if ( Dx > 0 )
    {
        Ix = (Long) ((precision * ((qint64)Dx)) / Dy);
        Rx = (Long) ((precision * ((qint64)Dx)) % Dy);
        Dx = 1;
    }
    else
    {
        Ix = (Long) -(precision * ((qint64)-Dx) / Dy);
        Rx = (Long)  (precision * ((qint64)-Dx) % Dy);
        Dx = -1;
    }

    Ax  = -Dy;
    top = ras.top;

    QT_FT_TRACE6("line_up (%f/%f)->(%f/%f), flow=%s\n",
                 FIXED_TO_FLOAT(x1), FIXED_TO_FLOAT(y1), FIXED_TO_FLOAT(x2), FIXED_TO_FLOAT(y2),
		 ras.cProfile->flow == Flow_Up ? "Flow_Up" : "Flow_Down");

    while ( size > 0 )
    {
        *top++ = x1;
        QT_FT_TRACE6("    x=%f y=%d\n", FIXED_TO_FLOAT(x1), e2+1-size);

        x1 += Ix;
        Ax += Rx;
        if ( Ax >= 0 )
        {
            Ax -= Dy;
            x1 += Dx;
        }
        size--;
    }

    ras.top = top;

    return SUCCESS;
}


/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    Line_Down                                                          */
/*                                                                       */
/* <Description>                                                         */
/*    Computes the x-coordinates of an descending line segment and       */
/*    stores them in the render pool.                                    */
/*                                                                       */
/* <Input>                                                               */
/*    x1   :: The x-coordinate of the segment's start point.             */
/*                                                                       */
/*    y1   :: The y-coordinate of the segment's start point.             */
/*                                                                       */
/*    x2   :: The x-coordinate of the segment's end point.               */
/*                                                                       */
/*    y2   :: The y-coordinate of the segment's end point.               */
/*                                                                       */
/*    miny :: A lower vertical clipping bound value.                     */
/*                                                                       */
/*    maxy :: An upper vertical clipping bound value.                    */
/*                                                                       */
/* <Return>                                                              */
/*    SUCCESS on success, FAILURE on render pool overflow.               */
/*                                                                       */
static Bool
Line_Down( TRaster_Instance*  raster, Long  x1,
           Long  y1,
           Long  x2,
           Long  y2,
           Long  miny,
           Long  maxy )
{
    Bool  result, fresh;


    fresh  = ras.fresh;

    result = Line_Up( raster, x1, -y1, x2, -y2, -maxy, -miny );

    if ( fresh && !ras.fresh )
        ras.cProfile->start = -ras.cProfile->start;

    return result;
}


/* A function type describing the functions used to split Bezier arcs */
typedef void  (*TSplitter)( TPoint*  base );


/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    Bezier_Up                                                          */
/*                                                                       */
/* <Description>                                                         */
/*    Computes the x-coordinates of an ascending Bezier arc and stores   */
/*    them in the render pool.                                           */
/*                                                                       */
/* <Input>                                                               */
/*    degree   :: The degree of the Bezier arc (either 2 or 3).          */
/*                                                                       */
/*    splitter :: The function to split Bezier arcs.                     */
/*                                                                       */
/*    miny     :: A lower vertical clipping bound value.                 */
/*                                                                       */
/*    maxy     :: An upper vertical clipping bound value.                */
/*                                                                       */
/* <Return>                                                              */
/*    SUCCESS on success, FAILURE on render pool overflow.               */
/*                                                                       */
static Bool
Bezier_Up( TRaster_Instance*  raster, Int        degree,
           TSplitter  splitter,
           Long       miny,
           Long       maxy )
{
    Long   y1, y2, e, e2;

    TPoint*  arc;
    TPoint*  start_arc;

    PLong top;


    arc = ras.arc;
    y1  = arc[degree].y;
    y2  = arc[0].y;
    top = ras.top;

    if ( y2 < miny || y1 > maxy )
        goto Fin;

    if ( y2 > maxy ) {
        e2 = FLOOR(maxy);
    } else {
        e2 = FLOOR( y2 );
        if (FRAC(y2) == 0  && ras.cProfile->flow == Flow_Up)
            e2 -= precision;
    }

    if ( y1 < miny ) {
        e = CEILING(miny);
    } else {
        e  = CEILING(y1);
        if (FRAC(y1) == 0 && ras.cProfile->flow == Flow_Down)
            e += precision;
    }

    if ( ras.fresh )
    {
        ras.cProfile->start = TRUNC( e );
        ras.fresh = FALSE;
    }

    QT_FT_TRACE6("bezier_up: y1=%f, y2=%f, e1=%f, e2=%f\n", FIXED_TO_FLOAT(y1),  FIXED_TO_FLOAT(y2), FIXED_TO_FLOAT(e), FIXED_TO_FLOAT(e2));

    if ( e2 < e )
        goto Fin;

    if ( ( top + TRUNC( e2 - e ) + 1 ) >= ras.maxBuff )
    {
        ras.top   = top;
        ras.error = Raster_Err_Overflow;
        return FAILURE;
    }

    QT_FT_TRACE6("       Flow = %s start=%ld, e=%f, e2=%f y1=%f, y2=%f\n",
              ras.cProfile->flow == Flow_Up ? "Flow_Up" : "Flow_Down",
                 ras.cProfile->start, FIXED_TO_FLOAT(e), FIXED_TO_FLOAT(e2),FIXED_TO_FLOAT(y1),FIXED_TO_FLOAT(y2));

    start_arc = arc;

    while ( arc >= start_arc && e <= e2 )
    {
        y2 = arc[0].y;

        if ( y2 > e )
        {
            y1 = arc[degree].y;
            if ( y2 - y1 >= precision_step )
            {
                splitter( arc );
                arc += degree;
            }
            else
            {
                *top++ = arc[degree].x + FMulDiv( arc[0].x-arc[degree].x,
                                                  e - y1, y2 - y1 );
                QT_FT_TRACE6("  x=%f y=%f\n",
                          (arc[degree].x + FMulDiv( arc[0].x-arc[degree].x,
                                                    e - y1, y2 - FIXED_TO_FLOAT(y1) )), FIXED_TO_FLOAT(e));
                arc -= degree;
                e   += precision;
            }
        }
        else
        {
            if ( y2 == e )
            {
                *top++     = arc[0].x;
                QT_FT_TRACE6("  x=%f y=%f\n", FIXED_TO_FLOAT(arc[0].x), FIXED_TO_FLOAT(e));

                e += precision;
            }
            arc -= degree;
        }
    }

Fin:
    ras.top  = top;
    ras.arc -= degree;
    QT_FT_TRACE6("    currently %d points in profile, start=%ld top=%p\n", (int)(ras.top - ras.cProfile->offset), ras.cProfile->start, ras.top);
    return SUCCESS;
}


/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    Bezier_Down                                                        */
/*                                                                       */
/* <Description>                                                         */
/*    Computes the x-coordinates of an descending Bezier arc and stores  */
/*    them in the render pool.                                           */
/*                                                                       */
/* <Input>                                                               */
/*    degree   :: The degree of the Bezier arc (either 2 or 3).          */
/*                                                                       */
/*    splitter :: The function to split Bezier arcs.                     */
/*                                                                       */
/*    miny     :: A lower vertical clipping bound value.                 */
/*                                                                       */
/*    maxy     :: An upper vertical clipping bound value.                */
/*                                                                       */
/* <Return>                                                              */
/*    SUCCESS on success, FAILURE on render pool overflow.               */
/*                                                                       */
static Bool
Bezier_Down( TRaster_Instance*  raster, Int        degree,
             TSplitter  splitter,
             Long       miny,
             Long       maxy )
{
    TPoint*  arc = ras.arc;
    Bool     result, fresh;


    arc[0].y = -arc[0].y;
    arc[1].y = -arc[1].y;
    arc[2].y = -arc[2].y;
    if ( degree > 2 )
        arc[3].y = -arc[3].y;

    fresh = ras.fresh;

    result = Bezier_Up( raster, degree, splitter, -maxy, -miny );

    if ( fresh && !ras.fresh )
        ras.cProfile->start = -ras.cProfile->start;

    arc[0].y = -arc[0].y;
    return result;
}


/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    Line_To                                                            */
/*                                                                       */
/* <Description>                                                         */
/*    Injects a new line segment and adjusts Profiles list.              */
/*                                                                       */
/* <Input>                                                               */
/*   x :: The x-coordinate of the segment's end point (its start point   */
/*        is stored in `LastX').                                         */
/*                                                                       */
/*   y :: The y-coordinate of the segment's end point (its start point   */
/*        is stored in `LastY').                                         */
/*                                                                       */
/* <Return>                                                              */
/*   SUCCESS on success, FAILURE on render pool overflow or incorrect    */
/*   profile.                                                            */
/*                                                                       */
static Bool
Line_To( TRaster_Instance*  raster, Long  x,
         Long  y )
{
    /* First, detect a change of direction */
    QT_FT_TRACE6( "Line_To (%f/%f)->(%f/%f)\n", FIXED_TO_FLOAT(ras.lastX), FIXED_TO_FLOAT(ras.lastY), FIXED_TO_FLOAT(x), FIXED_TO_FLOAT(y) );

    switch ( ras.state )
    {
    case Unknown_State:
        if ( y > ras.lastY )
        {
            if ( New_Profile( raster, Ascending_State ) )
                return FAILURE;
        }
        else
        {
            if ( y < ras.lastY )
                if ( New_Profile( raster, Descending_State ) )
                    return FAILURE;
        }
        break;

    case Ascending_State:
        if ( y < ras.lastY )
        {
            if ( End_Profile( raster )                   ||
                 New_Profile( raster, Descending_State ) )
                return FAILURE;
        }
        break;

    case Descending_State:
        if ( y > ras.lastY )
        {
            if ( End_Profile( raster )                  ||
                 New_Profile( raster, Ascending_State ) )
                return FAILURE;
        }
        break;

    default:
        ;
    }

    /* Then compute the lines */

    switch ( ras.state )
    {
    case Ascending_State:
        if ( Line_Up( raster, ras.lastX, ras.lastY,
                      x, y, ras.minY, ras.maxY ) )
            return FAILURE;
        break;

    case Descending_State:
        if ( Line_Down( raster, ras.lastX, ras.lastY,
                        x, y, ras.minY, ras.maxY ) )
            return FAILURE;
        break;

    default:
        ;
    }

    ras.lastX = x;
    ras.lastY = y;

    return SUCCESS;
}


/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    Conic_To                                                           */
/*                                                                       */
/* <Description>                                                         */
/*    Injects a new conic arc and adjusts the profile list.              */
/*                                                                       */
/* <Input>                                                               */
/*   cx :: The x-coordinate of the arc's new control point.              */
/*                                                                       */
/*   cy :: The y-coordinate of the arc's new control point.              */
/*                                                                       */
/*   x  :: The x-coordinate of the arc's end point (its start point is   */
/*         stored in `LastX').                                           */
/*                                                                       */
/*   y  :: The y-coordinate of the arc's end point (its start point is   */
/*         stored in `LastY').                                           */
/*                                                                       */
/* <Return>                                                              */
/*   SUCCESS on success, FAILURE on render pool overflow or incorrect    */
/*   profile.                                                            */
/*                                                                       */
static Bool
Conic_To( TRaster_Instance*  raster, Long  cx,
          Long  cy,
          Long  x,
          Long  y )
{
    Long     y1, y2, y3, x3, ymin, ymax;
    TStates  state_bez;

    QT_FT_TRACE6( "Conic_To (%f/%f)-(%f/%f)\n", FIXED_TO_FLOAT(cx), FIXED_TO_FLOAT(cy), FIXED_TO_FLOAT(x), FIXED_TO_FLOAT(y) );

    ras.arc      = ras.arcs;
    ras.arc[2].x = ras.lastX;
    ras.arc[2].y = ras.lastY;
    ras.arc[1].x = cx; ras.arc[1].y = cy;
    ras.arc[0].x = x;  ras.arc[0].y = y;

    do
    {
        y1 = ras.arc[2].y;
        y2 = ras.arc[1].y;
        y3 = ras.arc[0].y;
        x3 = ras.arc[0].x;

        /* first, categorize the Bezier arc */

        if ( y1 <= y3 )
        {
            ymin = y1;
            ymax = y3;
        }
        else
        {
            ymin = y3;
            ymax = y1;
        }

        if ( y2 < ymin || y2 > ymax )
        {
            /* this arc has no given direction, split it! */
            Split_Conic( ras.arc );
            ras.arc += 2;
        }
        else if ( y1 == y3 )
        {
            /* this arc is flat, ignore it and pop it from the Bezier stack */
            ras.arc -= 2;
        }
        else
        {
            /* the arc is y-monotonous, either ascending or descending */
            /* detect a change of direction                            */
            state_bez = y1 < y3 ? Ascending_State : Descending_State;
            if ( ras.state != state_bez )
            {
                /* finalize current profile if any */
                if ( ras.state != Unknown_State   &&
                     End_Profile( raster ) )
                    goto Fail;

                /* create a new profile */
                if ( New_Profile( raster, state_bez ) )
                    goto Fail;
            }

            /* now call the appropriate routine */
            if ( state_bez == Ascending_State )
            {
                if ( Bezier_Up( raster, 2, Split_Conic, ras.minY, ras.maxY ) )
                    goto Fail;
            }
            else
                if ( Bezier_Down( raster, 2, Split_Conic, ras.minY, ras.maxY ) )
                    goto Fail;
        }

    } while ( ras.arc >= ras.arcs );

    ras.lastX = x3;
    ras.lastY = y3;

    return SUCCESS;

Fail:
    return FAILURE;
}


/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    Cubic_To                                                           */
/*                                                                       */
/* <Description>                                                         */
/*    Injects a new cubic arc and adjusts the profile list.              */
/*                                                                       */
/* <Input>                                                               */
/*   cx1 :: The x-coordinate of the arc's first new control point.       */
/*                                                                       */
/*   cy1 :: The y-coordinate of the arc's first new control point.       */
/*                                                                       */
/*   cx2 :: The x-coordinate of the arc's second new control point.      */
/*                                                                       */
/*   cy2 :: The y-coordinate of the arc's second new control point.      */
/*                                                                       */
/*   x   :: The x-coordinate of the arc's end point (its start point is  */
/*          stored in `LastX').                                          */
/*                                                                       */
/*   y   :: The y-coordinate of the arc's end point (its start point is  */
/*          stored in `LastY').                                          */
/*                                                                       */
/* <Return>                                                              */
/*   SUCCESS on success, FAILURE on render pool overflow or incorrect    */
/*   profile.                                                            */
/*                                                                       */
static Bool
Cubic_To( TRaster_Instance*  raster, Long  cx1,
          Long  cy1,
          Long  cx2,
          Long  cy2,
          Long  x,
          Long  y )
{
    Long     y1, y2, y3, y4, x4, ymin1, ymax1, ymin2, ymax2;
    TStates  state_bez;

    QT_FT_TRACE6( "Cubic_To (%f/%f)-(%f/%f)-(%f/%f)-(%f/%f)\n",
                  FIXED_TO_FLOAT(ras.lastX), FIXED_TO_FLOAT(ras.lastY), FIXED_TO_FLOAT(cx1), FIXED_TO_FLOAT(cy1), FIXED_TO_FLOAT(cx2), FIXED_TO_FLOAT(cy2), FIXED_TO_FLOAT(x), FIXED_TO_FLOAT(y) );

    ras.arc      = ras.arcs;
    ras.arc[3].x = ras.lastX;
    ras.arc[3].y = ras.lastY;
    ras.arc[2].x = cx1; ras.arc[2].y = cy1;
    ras.arc[1].x = cx2; ras.arc[1].y = cy2;
    ras.arc[0].x = x;   ras.arc[0].y = y;

    do
    {
        y1 = ras.arc[3].y;
        y2 = ras.arc[2].y;
        y3 = ras.arc[1].y;
        y4 = ras.arc[0].y;
        x4 = ras.arc[0].x;

        /* first, categorize the Bezier arc */

        if ( y1 <= y4 )
        {
            ymin1 = y1;
            ymax1 = y4;
        }
        else
        {
            ymin1 = y4;
            ymax1 = y1;
        }

        if ( y2 <= y3 )
        {
            ymin2 = y2;
            ymax2 = y3;
        }
        else
        {
            ymin2 = y3;
            ymax2 = y2;
        }

        if ( ymin2 < ymin1 || ymax2 > ymax1 )
        {
            /* this arc has no given direction, split it! */
            Split_Cubic( ras.arc );
            ras.arc += 3;
        }
        else if ( y1 == y4 )
        {
            /* this arc is flat, ignore it and pop it from the Bezier stack */
            ras.arc -= 3;
        }
        else
        {
            state_bez = ( y1 <= y4 ) ? Ascending_State : Descending_State;

            /* detect a change of direction */
            if ( ras.state != state_bez )
            {
                if ( ras.state != Unknown_State   &&
                     End_Profile( raster ) )
                    goto Fail;

                if ( New_Profile( raster, state_bez ) )
                    goto Fail;
            }

            /* compute intersections */
            if ( state_bez == Ascending_State )
            {
                if ( Bezier_Up( raster, 3, Split_Cubic, ras.minY, ras.maxY ) )
                    goto Fail;
            }
            else
                if ( Bezier_Down( raster, 3, Split_Cubic, ras.minY, ras.maxY ) )
                    goto Fail;
        }

    } while ( ras.arc >= ras.arcs );

    ras.lastX = x4;
    ras.lastY = y4;

    return SUCCESS;

Fail:
    return FAILURE;
}


#undef  SWAP_
#define SWAP_( x, y )  do                       \
    {                                           \
        Long  swap = x;                         \
                                                \
                                                \
        x = y;                                  \
        y = swap;                               \
    } while ( 0 )


/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    Decompose_Curve                                                    */
/*                                                                       */
/* <Description>                                                         */
/*    Scans the outline arays in order to emit individual segments and   */
/*    Beziers by calling Line_To() and Bezier_To().  It handles all      */
/*    weird cases, like when the first point is off the curve, or when   */
/*    there are simply no `on' points in the contour!                    */
/*                                                                       */
/* <Input>                                                               */
/*    first   :: The index of the first point in the contour.            */
/*                                                                       */
/*    last    :: The index of the last point in the contour.             */
/*                                                                       */
/*    flipped :: If set, flip the direction of the curve.                */
/*                                                                       */
/* <Return>                                                              */
/*    SUCCESS on success, FAILURE on error.                              */
/*                                                                       */
static Bool
Decompose_Curve( TRaster_Instance*  raster, UInt first,
                 UInt last,
                 int     flipped )
{
    QT_FT_Vector   v_last;
    QT_FT_Vector   v_control;
    QT_FT_Vector   v_start;

    QT_FT_Vector*  points;
    QT_FT_Vector*  point;
    QT_FT_Vector*  limit;
    char*       tags;

    unsigned    tag;       /* current point's state           */


    points = ras.outline.points;
    limit  = points + last;

    v_start.x = SCALED( points[first].x );
    v_start.y = SCALED( points[first].y );
    v_last.x  = SCALED( points[last].x );
    v_last.y  = SCALED( points[last].y );

    if ( flipped )
    {
        SWAP_( v_start.x, v_start.y );
        SWAP_( v_last.x, v_last.y );
    }

    v_control = v_start;

    point = points + first;
    tags  = ras.outline.tags  + first;
    tag   = QT_FT_CURVE_TAG( tags[0] );

    /* A contour cannot start with a cubic control point! */
    if ( tag == QT_FT_CURVE_TAG_CUBIC )
        goto Invalid_Outline;

    /* check first point to determine origin */
    if ( tag == QT_FT_CURVE_TAG_CONIC )
    {
        /* first point is conic control.  Yes, this happens. */
        if ( QT_FT_CURVE_TAG( ras.outline.tags[last] ) == QT_FT_CURVE_TAG_ON )
        {
            /* start at last point if it is on the curve */
            v_start = v_last;
            limit--;
        }
        else
        {
            /* if both first and last points are conic,         */
            /* start at their middle and record its position    */
            /* for closure                                      */
            v_start.x = ( v_start.x + v_last.x ) / 2;
            v_start.y = ( v_start.y + v_last.y ) / 2;

            v_last = v_start;
        }
        point--;
        tags--;
    }

    ras.lastX = v_start.x;
    ras.lastY = v_start.y;

    while ( point < limit )
    {
        point++;
        tags++;

        tag = QT_FT_CURVE_TAG( tags[0] );

        switch ( tag )
        {
        case QT_FT_CURVE_TAG_ON:  /* emit a single line_to */
        {
            Long  x, y;


            x = SCALED( point->x );
            y = SCALED( point->y );
            if ( flipped )
                SWAP_( x, y );

            if ( Line_To( raster, x, y ) )
                goto Fail;
            continue;
        }

        case QT_FT_CURVE_TAG_CONIC:  /* consume conic arcs */
            v_control.x = SCALED( point[0].x );
            v_control.y = SCALED( point[0].y );

            if ( flipped )
                SWAP_( v_control.x, v_control.y );

        Do_Conic:
            if ( point < limit )
            {
                QT_FT_Vector  v_middle;
                Long       x, y;


                point++;
                tags++;
                tag = QT_FT_CURVE_TAG( tags[0] );

                x = SCALED( point[0].x );
                y = SCALED( point[0].y );

                if ( flipped )
                    SWAP_( x, y );

                if ( tag == QT_FT_CURVE_TAG_ON )
                {
                    if ( Conic_To( raster, v_control.x, v_control.y, x, y ) )
                        goto Fail;
                    continue;
                }

                if ( tag != QT_FT_CURVE_TAG_CONIC )
                    goto Invalid_Outline;

                v_middle.x = ( v_control.x + x ) / 2;
                v_middle.y = ( v_control.y + y ) / 2;

                if ( Conic_To( raster, v_control.x, v_control.y,
                               v_middle.x,  v_middle.y ) )
                    goto Fail;

                v_control.x = x;
                v_control.y = y;

                goto Do_Conic;
            }

            if ( Conic_To( raster, v_control.x, v_control.y,
                           v_start.x,   v_start.y ) )
                goto Fail;

            goto Close;

        default:  /* QT_FT_CURVE_TAG_CUBIC */
        {
            Long  x1, y1, x2, y2, x3, y3;


            if ( point + 1 > limit                             ||
                 QT_FT_CURVE_TAG( tags[1] ) != QT_FT_CURVE_TAG_CUBIC )
                goto Invalid_Outline;

            point += 2;
            tags  += 2;

            x1 = SCALED( point[-2].x );
            y1 = SCALED( point[-2].y );
            x2 = SCALED( point[-1].x );
            y2 = SCALED( point[-1].y );
            x3 = SCALED( point[ 0].x );
            y3 = SCALED( point[ 0].y );

            if ( flipped )
            {
                SWAP_( x1, y1 );
                SWAP_( x2, y2 );
                SWAP_( x3, y3 );
            }

            if ( point <= limit )
            {
                if ( Cubic_To( raster, x1, y1, x2, y2, x3, y3 ) )
                    goto Fail;
                continue;
            }

            if ( Cubic_To( raster, x1, y1, x2, y2, v_start.x, v_start.y ) )
                goto Fail;
            goto Close;
        }
        }
    }

    /* close the contour with a line segment */
    if ( Line_To( raster, v_start.x, v_start.y ) )
        goto Fail;

Close:
    return SUCCESS;

Invalid_Outline:
    ras.error = Raster_Err_Invalid;

Fail:
    return FAILURE;
}


/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    Convert_Glyph                                                      */
/*                                                                       */
/* <Description>                                                         */
/*    Converts a glyph into a series of segments and arcs and makes a    */
/*    profiles list with them.                                           */
/*                                                                       */
/* <Input>                                                               */
/*    flipped :: If set, flip the direction of curve.                    */
/*                                                                       */
/* <Return>                                                              */
/*    SUCCESS on success, FAILURE if any error was encountered during    */
/*    rendering.                                                         */
/*                                                                       */
static Bool
Convert_Glyph( TRaster_Instance*  raster, int  flipped )
{
    int       i;
    unsigned  start;

    PProfile  lastProfile;


    ras.fProfile = NULL;
    ras.fresh    = FALSE;

    ras.maxBuff  = ras.sizeBuff - AlignProfileSize;

    ras.numTurns = 0;

    ras.cProfile         = (PProfile)ras.top;
    ras.cProfile->offset = ras.top;
    ras.num_Profs        = 0;

    start = 0;

    for ( i = 0; i < ras.outline.n_contours; i++ )
    {
        ras.state    = Unknown_State;
        ras.gProfile = NULL;

        if ( Decompose_Curve( raster, start,
                              ras.outline.contours[i],
                              flipped ) )
            return FAILURE;

        start = ras.outline.contours[i] + 1;

        /* Note that ras.gProfile can be nil if the contour was too small */
        /* to be drawn.                                                   */

        lastProfile = ras.cProfile;
        if ( End_Profile( raster ) )
            return FAILURE;

    }

    if ( Finalize_Profile_Table( raster ) )
        return FAILURE;

    return (Bool)( ras.top < ras.maxBuff ? SUCCESS : FAILURE );
}


/*************************************************************************/
/*************************************************************************/
/**                                                                     **/
/**  SCAN-LINE SWEEPS AND DRAWING                                       **/
/**                                                                     **/
/*************************************************************************/
/*************************************************************************/


/*************************************************************************/
/*                                                                       */
/*  Init_Linked                                                          */
/*                                                                       */
/*    Initializes an empty linked list.                                  */
/*                                                                       */
static void
Init_Linked( TProfileList*  l )
{
    *l = NULL;
}


/*************************************************************************/
/*                                                                       */
/*  InsNew                                                               */
/*                                                                       */
/*    Inserts a new profile in a linked list.                            */
/*                                                                       */
static void
InsNew( PProfileList  list,
        PProfile      profile )
{
    PProfile  *old, current;
    Long       x;


    old     = list;
    current = *old;
    x       = profile->X;

    while ( current )
    {
        if ( x < current->X )
            break;
        old     = &current->link;
        current = *old;
    }

    profile->link = current;
    *old          = profile;
}


/*************************************************************************/
/*                                                                       */
/*  DelOld                                                               */
/*                                                                       */
/*    Removes an old profile from a linked list.                         */
/*                                                                       */
static void
DelOld( PProfileList  list,
        PProfile      profile )
{
    PProfile  *old, current;


    old     = list;
    current = *old;

    while ( current )
    {
        if ( current == profile )
        {
            *old = current->link;
            return;
        }

        old     = &current->link;
        current = *old;
    }

    /* we should never get there, unless the profile was not part of */
    /* the list.                                                     */
}

/*************************************************************************/
/*                                                                       */
/*  Sort                                                                 */
/*                                                                       */
/*    Sorts a trace list.  In 95%, the list is already sorted.  We need  */
/*    an algorithm which is fast in this case.  Bubble sort is enough    */
/*    and simple.                                                        */
/*                                                                       */
static void
Sort( PProfileList  list )
{
    PProfile  *old, current, next;
    PProfile q, p, e, tail;
    int insize, nmerges, psize, qsize, i;

    /* First, set the new X coordinate of each profile */
    int lsize = 0;
    current = *list;
    while ( current )
    {
        current->X       = *current->offset;
        current->offset += current->flow;
        current->height--;
        current = current->link;
	++lsize;
    }

    if ( !*list )
	return;

    /* If we have 20 or less elements in the list, use
       Bubblesort, otherwise use Mergesort */
    if (lsize < 20) {
	old     = list;
	current = *old;

	next = current->link;

	while ( next )
	{
	    if ( current->X <= next->X )
	    {
		old     = &current->link;
		current = *old;

		if ( !current )
		    return;
	    }
	    else
	    {
		*old = next;
		current->link = next->link;
		next->link    = current;

		old     = list;
		current = *old;
	    }

	    next = current->link;
	}
    } else {

	/* The below algoritm is a modified version of a Mergesort
	   algorithm described by Simon Tatham, which can be found
	   here: http://www.chiark.greenend.org.uk/~sgtatham/algorithms */

	insize = 1;
	while (1) {
	    p = *list;
	    *list = 0;
	    tail = 0;
	    nmerges = 0;

	    while (p) {
		++nmerges;
		q = p;
		psize = 0;
		for (i = 0; i < insize; ++i) {
		    ++psize;
		    q = q->link;
		    if (!q)
			break;
		}
		qsize = insize;

		while (psize > 0 || (qsize > 0 && q)) {
		    if (psize == 0) {
			e = q; q = q->link; --qsize;
		    } else if ((qsize == 0 || !q) || (p->X <= q->X)) {
			e = p; p = p->link; --psize;
		    } else {
			e = q; q = q->link; --qsize;
		    }

		    if (tail)
			tail->link = e;
		    else
			*list = e;
		    tail = e;
		}
		p = q;
	    }
	    if (tail)
		tail->link = 0;

	    if (nmerges <= 1)
		break;
	    insize *= 2;
	}
    }
}

/*************************************************************************/
/*                                                                       */
/*  Generic Sweep Drawing routine                                        */
/*                                                                       */
/*************************************************************************/

static Bool
Draw_Sweep( TRaster_Instance*  raster )
{
    Short         y, y_change, y_height;

    PProfile      P, Q, P_Left, P_Right;

    Short         min_Y, max_Y, top, bottom;

    Long          x1, x2, xs, e1, e2, ix1, ix2;

    short winding;

    TProfileList  waiting;
/*     TProfileList  draw_left, draw_right; */

    TProfileList profile_list;

    QT_FT_Span spans[MAX_SPANS];
    Int span_count = 0;

    /* Init empty linked lists */

    Init_Linked( &waiting );

/*     Init_Linked( &draw_left  );*/
/*     Init_Linked( &draw_right );*/

    Init_Linked( &profile_list );

    /* first, compute min and max Y */

    P     = ras.fProfile;
    max_Y = (Short)TRUNC( ras.minY );
    min_Y = (Short)TRUNC( ras.maxY );

    while ( P )
    {
        Q = P->link;

        bottom = (Short)P->start;
        top    = (Short)( P->start + P->height - 1 );

        if ( min_Y > bottom ) min_Y = bottom;
        if ( max_Y < top    ) max_Y = top;

        P->X = 0;
        InsNew( &waiting, P );

        P = Q;
    }

    /* Check the Y-turns */
    if ( ras.numTurns == 0 )
    {
        ras.error = Raster_Err_Invalid;
        return FAILURE;
    }

    /* Then compute the distance of each profile from min_Y */

    P = waiting;

    while ( P )
    {
        P->countL = (UShort)( P->start - min_Y );
        P = P->link;
    }

    /* Let's go */

    y        = min_Y;
    y_height = 0;

    if ( ras.numTurns > 0 &&
         ras.sizeBuff[-ras.numTurns] == min_Y )
        ras.numTurns--;

    while ( ras.numTurns > 0 )
    {
        /* look in the waiting list for new activations */

        P = waiting;

        while ( P )
        {
            Q = P->link;
            P->countL -= y_height;
            if ( P->countL == 0 )
            {
                DelOld( &waiting, P );
                if (P->flow) {
                    InsNew( &profile_list, P );
                }
            }

            P = Q;
        }

        /* Sort the drawing lists */
        Sort(&profile_list);

        y_change = (Short)ras.sizeBuff[-ras.numTurns--];
        y_height = (Short)( y_change - y );

        while ( y < y_change )
        {

#ifdef Q_RASTER_DEBUG
            Q = profile_list;
            printf("y=%d ::", y);
            while (Q) {
                printf(" %.2f", FIXED_TO_FLOAT(Q->X));
                Q = Q->link;
            }
            printf("\n");
#endif

            /* Let's trace */
            P_Left = profile_list;

            while (P_Left) {
                winding = P_Left->flow;

                if (ras.odd_even) {
                    P_Right = P_Left->link;
                } else { /* Winding fill; */
                    P_Right = P_Left;
                    while (winding && P_Right->link) {
                        P_Right = P_Right->link;
                        winding += P_Right->flow;
                    }
                }

                /* We need an even number of intersections, otherwise there is something */
                /* wrong... */
                if (!P_Right)
                    return Raster_Err_Invalid;

                x1 = P_Left ->X;
                x2 = P_Right->X;

                if ( x1 > x2 ) {
                    xs = x1;
                    x1 = x2;
                    x2 = xs;
                }

                if ( x2 - x1 <= precision ) {
                    e1 = FLOOR( x1 );
                    e2 = CEILING( x2 );
                }

                ix1 = MAX(TRUNC(CEILING(x1)), ras.minX_dev);
                ix2 = MIN(TRUNC(FLOOR(x2-1)) + 1, ras.maxX_dev);
                if (ix2 > ix1) {
                    if (span_count && spans[span_count-1].y == y
                        && spans[span_count-1].x + spans[span_count-1].len == ix1) {
                        /* merge spans */
                        spans[span_count-1].len = (short) (ix2 - spans[span_count-1].x);
                    } else {
                        spans[span_count].x = (short) ix1;
                        spans[span_count].len = (short) (ix2 - ix1);
                        spans[span_count].y = y;
                        spans[span_count].coverage = 255;
                        ++span_count;

                        if (span_count == MAX_SPANS) {
                            ras.black_spans(span_count, spans, ras.user_data);
                            span_count = 0;
                        }
                    }
                }
                P_Left = P_Right->link;
            }

            y++;

            if (y < y_change)
                Sort( &profile_list );
        }

        /* Now finalize the profiles that needs it */

        P = profile_list;
        while ( P )
        {
            Q = P->link;
            if ( P->height == 0 )
                DelOld( &profile_list, P );
            P = Q;
        }

    }

    if (span_count > 0)
        ras.black_spans(span_count, spans, ras.user_data);

    return SUCCESS;
}


/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    Render_Single_Pass                                                 */
/*                                                                       */
/* <Description>                                                         */
/*    Performs one sweep with sub-banding.                               */
/*                                                                       */
/* <Input>                                                               */
/*    flipped :: If set, flip the direction of the outline.              */
/*                                                                       */
/* <Return>                                                              */
/*    Renderer error code.                                               */
/*                                                                       */
static int
Render_Single_Pass( TRaster_Instance*  raster, Bool  flipped )
{
    Short  i, j, k;


    while ( ras.band_top >= 0 )
    {
        ras.maxY = (Long)ras.band_stack[ras.band_top].y_max * precision;
        ras.minY = (Long)ras.band_stack[ras.band_top].y_min * precision;

        ras.top = ras.buff;

        ras.error = Raster_Err_None;

        if ( Convert_Glyph( raster, flipped ) )
        {
            if ( ras.error != Raster_Err_Overflow )
                return FAILURE;

            ras.error = Raster_Err_None;

            /* sub-banding */

#ifdef DEBUG_RASTER
            ClearBand( raster, TRUNC( ras.minY ), TRUNC( ras.maxY ) );
#endif

            i = ras.band_stack[ras.band_top].y_min;
            j = ras.band_stack[ras.band_top].y_max;

            k = (Short)( ( i + j ) / 2 );

            if ( ras.band_top >= 7 || k <= i )
            {
                ras.band_top = 0;
                ras.error    = Raster_Err_OutOfMemory;

                return ras.error;
            }

            ras.band_stack[ras.band_top].y_min = k;

            ras.band_stack[ras.band_top+1].y_min = i;
            ras.band_stack[ras.band_top+1].y_max = (Short)( k - 1 );

            ras.band_top++;
        }
        else
        {
            if ( ras.fProfile )
                if ( Draw_Sweep( raster ) )
                    return ras.error;
            ras.band_top--;
        }
    }

    return SUCCESS;
}


/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    Render_Glyph                                                       */
/*                                                                       */
/* <Description>                                                         */
/*    Renders a glyph in a bitmap.  Sub-banding if needed.               */
/*                                                                       */
/* <Return>                                                              */
/*    FreeType error code.  0 means success.                             */
/*                                                                       */
QT_FT_LOCAL_DEF( QT_FT_Error )
    Render_Glyph( TRaster_Instance*  raster )
{
    QT_FT_Error  error;

    QT_FT_TRACE6("Render_Glyph:\n");

    ras.band_top            = 0;
    ras.band_stack[0].y_min = (short) (ras.clip_box.yMin);
    ras.band_stack[0].y_max = (short) (ras.clip_box.yMax - ras.clip_box.yMin - 1);

    if ( ( error = Render_Single_Pass( raster, 0 ) ) != 0 )
        return error;

    QT_FT_TRACE6("End Render_Glyph:\n\n");

    return Raster_Err_Ok;
}


/**** RASTER OBJECT CREATION: In standalone mode, we simply use *****/
/****                         a static object.                  *****/


/* #### a static instance might be a bad idea... */
static int
qt_ft_black_new( void*      memory,
              QT_FT_Raster  *araster )
{
    if (memory)
        fprintf(stderr, "qt_ft_black_new(), memory ignored");

    memory = malloc(sizeof(TRaster_Instance));
    QT_FT_MEM_ZERO(memory, sizeof(TRaster_Instance));

    *araster = (QT_FT_Raster) memory;

    return 0;
}


static void
qt_ft_black_done( QT_FT_Raster  raster )
{
    free(raster);
}


static void
qt_ft_black_reset( TRaster_Instance*  raster,
                const char*        pool_base,
                long               pool_size )
{
    if ( raster && pool_base && pool_size >= 4096 )
    {
        /* save the pool */
        raster->buff     = (PLong)pool_base;
        raster->sizeBuff = raster->buff + pool_size / sizeof ( Long );
    }
}


static void
qt_ft_black_set_mode( TRaster_Instance*  raster,
                   unsigned long      mode,
                   const char*        palette )
{

    QT_FT_UNUSED( raster );
    QT_FT_UNUSED( mode );
    QT_FT_UNUSED( palette );

}


static int
qt_ft_black_render( TRaster_Instance*  raster,
                 QT_FT_Raster_Params*  params )
{
    QT_FT_Outline*  outline    = (QT_FT_Outline*)params->source;

    if ( !raster || !raster->buff || !raster->sizeBuff )
        return Raster_Err_Not_Ini;

    if ( !outline || !outline->contours || !outline->points )
        return Raster_Err_Invalid;

    /* return immediately if the outline is empty */
    if ( outline->n_points == 0 || outline->n_contours <= 0 )
        return Raster_Err_None;

    if ( outline->n_points != outline->contours[outline->n_contours - 1] + 1 )
        return Raster_Err_Invalid;

    if (!params->black_spans)
        return Raster_Err_Invalid;
    ras.black_spans = params->black_spans;
    ras.user_data = params->user;
    ras.outline  = *outline;

    if (!(params->flags & QT_FT_RASTER_FLAG_CLIP))
        return Raster_Err_Invalid;

    ras.minX_dev = (short)params->clip_box.xMin;
    ras.maxX_dev = (short)params->clip_box.xMax;

    ras.clip_box = params->clip_box;

    ras.odd_even = (outline->flags & QT_FT_OUTLINE_EVEN_ODD_FILL);

    return Render_Glyph( raster );
}


const QT_FT_Raster_Funcs  qt_ft_standard_raster =
{
    QT_FT_GLYPH_FORMAT_OUTLINE,
    (QT_FT_Raster_New_Func)     qt_ft_black_new,
    (QT_FT_Raster_Reset_Func)   qt_ft_black_reset,
    (QT_FT_Raster_Set_Mode_Func)qt_ft_black_set_mode,
    (QT_FT_Raster_Render_Func)  qt_ft_black_render,
    (QT_FT_Raster_Done_Func)    qt_ft_black_done
};

/* END */

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
/*  qgrayraster_p.h, derived from ftgrays.h                                */
/*                                                                         */
/*    FreeType smooth renderer declaration                                 */
/*                                                                         */
/*  Copyright 1996-2001 by                                                 */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, ../../3rdparty/freetype/docs/FTL.TXT.  By continuing to use,  */
/*  modify, or distribute this file you indicate that you have read        */
/*  the license and understand and accept it fully.                        */
/***************************************************************************/


#ifndef __FTGRAYS_H__
#define __FTGRAYS_H__

/*
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
*/

#ifdef __cplusplus
  extern "C" {
#endif


#include <private/qrasterdefs_p.h>

  /*************************************************************************/
  /*                                                                       */
  /* To make ftgrays.h independent from configuration files we check       */
  /* whether QT_FT_EXPORT_VAR has been defined already.                    */
  /*                                                                       */
  /* On some systems and compilers (Win32 mostly), an extra keyword is     */
  /* necessary to compile the library as a DLL.                            */
  /*                                                                       */
#ifndef QT_FT_EXPORT_VAR
#define QT_FT_EXPORT_VAR( x )  extern  x
#endif

  QT_FT_EXPORT_VAR( const QT_FT_Raster_Funcs )  qt_ft_grays_raster;


#ifdef __cplusplus
  }
#endif

#endif /* __FTGRAYS_H__ */

/* END */

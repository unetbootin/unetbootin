/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the tools applications of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#if (defined(_WIN32) || defined(__NT__))
#  define QT_UNDEF_MACROS_IN_PCH
#  define _WINSCARD_H_
#  define _POSIX_         /* Make sure PATH_MAX et al. are defined    */
#  include <limits.h>
#  undef _POSIX_          /* Don't polute                             */

   /* Make sure IP v6 is defined first of all, before windows.h     */
#  ifndef QT_NO_IPV6
#     include <winsock2.h>
#  endif
#  include <stdlib.h>
#endif

#if defined __cplusplus
#include <qglobal.h>
#include <qlist.h>
#include <qvariant.h>  // All moc genereated code has this include
#include <qplatformdefs.h>
#include <qregexp.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qtextcodec.h>

#include <limits.h>
#include <stdlib.h>
#endif

#if defined(QT_UNDEF_MACROS_IN_PCH)
#  undef max /*  These are defined in windef.h, but                   */
#  undef min /*  we don't want them when building Qt                  */
#  undef _WINSCARD_H_
#endif

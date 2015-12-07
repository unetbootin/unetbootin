/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtGui module of the Qt Toolkit.
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

// the miscrosoft platform SDK says that the Unicode versions of
// TextOut and GetTextExtentsPoint32 are supported on all platforms, so we use them
// exclusively to simplify code, save a lot of conversions into the local encoding
// and have generally better unicode support :)

#include "qfont.h"
#include "qfont_p.h"
#include "qfontengine_p.h"
#include "qtextengine_p.h"
#include "qfontmetrics.h"
#include "qfontinfo.h"

#include "qwidget.h"
#include "qpainter.h"
#include <limits.h>
#include "qt_windows.h"
#include <private/qapplication_p.h>
#include "qapplication.h"
#include <private/qunicodetables_p.h>
#include <qfontdatabase.h>

QT_BEGIN_NAMESPACE
        
extern HDC   shared_dc();                // common dc for all fonts

// ### maybe move to qapplication_win
QFont qt_LOGFONTtoQFont(LOGFONT& lf, bool /*scale*/)
{
    QString family = QT_WA_INLINE(QString::fromUtf16((ushort*)lf.lfFaceName),
                                   QString::fromLocal8Bit((char*)lf.lfFaceName));
    QFont qf(family);
    qf.setItalic(lf.lfItalic);
    if (lf.lfWeight != FW_DONTCARE) {
        int weight;
        if (lf.lfWeight < 400)
            weight = QFont::Light;
        else if (lf.lfWeight < 600)
            weight = QFont::Normal;
        else if (lf.lfWeight < 700)
            weight = QFont::DemiBold;
        else if (lf.lfWeight < 800)
            weight = QFont::Bold;
        else
            weight = QFont::Black;
        qf.setWeight(weight);
    }
    int lfh = qAbs(lf.lfHeight);
    qf.setPointSizeF(lfh * 72.0 / GetDeviceCaps(shared_dc(),LOGPIXELSY));
    qf.setUnderline(false);
    qf.setOverline(false);
    qf.setStrikeOut(false);
    return qf;
}


static inline float pixelSize(const QFontDef &request, int dpi)
{
    float pSize;
    if (request.pointSize != -1)
        pSize = request.pointSize * dpi/ 72.;
    else
        pSize = request.pixelSize;
    return pSize;
}

static inline float pointSize(const QFontDef &fd, int dpi)
{
    float pSize;
    if (fd.pointSize < 0)
        pSize = fd.pixelSize * 72. / ((float)dpi);
    else
        pSize = fd.pointSize;
    return pSize;
}

/*****************************************************************************
  QFont member functions
 *****************************************************************************/

void QFont::initialize()
{
}

void QFont::cleanup()
{
    QFontCache::cleanup();
}

HFONT QFont::handle() const
{
    QFontEngine *engine = d->engineForScript(QUnicodeTables::Common);
    Q_ASSERT(engine != 0);
    if (engine->type() == QFontEngine::Multi)
        engine = static_cast<QFontEngineMulti *>(engine)->engine(0);
    if (engine->type() == QFontEngine::Win)
	return static_cast<QFontEngineWin *>(engine)->hfont;
    return 0;
}

QString QFont::rawName() const
{
    return family();
}

void QFont::setRawName(const QString &name)
{
    setFamily(name);
}

QString QFont::defaultFamily() const
{
    switch(d->request.styleHint) {
        case QFont::Times:
            return QString::fromLatin1("Times New Roman");
        case QFont::Courier:
            return QString::fromLatin1("Courier New");
        case QFont::Decorative:
            return QString::fromLatin1("Bookman Old Style");
        case QFont::Helvetica:
            return QString::fromLatin1("Arial");
        case QFont::System:
        default:
            return QString::fromLatin1("MS Sans Serif");
    }
}

QString QFont::lastResortFamily() const
{
    return QString::fromLatin1("helvetica");
}

QString QFont::lastResortFont() const
{
    return QString::fromLatin1("arial");
}

QT_END_NAMESPACE

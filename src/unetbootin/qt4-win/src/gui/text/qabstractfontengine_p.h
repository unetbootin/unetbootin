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

#ifndef QABSTRACTFONTENGINE_P_H
#define QABSTRACTFONTENGINE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qfontengine_p.h"
#include "qabstractfontengine_qws.h"

QT_BEGIN_NAMESPACE

class QCustomFontEngine;

class QProxyFontEngine : public QFontEngine
{
    Q_OBJECT
public:
    QProxyFontEngine(QAbstractFontEngine *engine, const QFontDef &def);
    virtual ~QProxyFontEngine();

    virtual bool stringToCMap(const QChar *str, int len, QGlyphLayout *glyphs, int *nglyphs, QTextEngine::ShaperFlags flags) const;
    virtual QImage alphaMapForGlyph(glyph_t);
    virtual void addGlyphsToPath(glyph_t *glyphs, QFixedPoint *positions, int nglyphs, QPainterPath *path, QTextItem::RenderFlags flags);
    virtual glyph_metrics_t boundingBox(const QGlyphLayout *glyphs, int numGlyphs);
    virtual glyph_metrics_t boundingBox(glyph_t glyph);

    virtual QFixed ascent() const;
    virtual QFixed descent() const;
    virtual QFixed leading() const;
    virtual QFixed xHeight() const;
    virtual QFixed averageCharWidth() const;
    virtual QFixed lineThickness() const;
    virtual QFixed underlinePosition() const;
    virtual qreal maxCharWidth() const;
    virtual qreal minLeftBearing() const;
    virtual qreal minRightBearing() const;
    virtual int glyphCount() const;

    virtual bool canRender(const QChar *string, int len);

    virtual Type type() const { return Proxy; }
    virtual const char *name() const { return "proxy engine"; }

#if !defined(Q_WS_X11) && !defined(Q_WS_WIN) && !defined(Q_WS_MAC)
    virtual void draw(QPaintEngine *, qreal, qreal, const QTextItemInt &);
#endif

    inline QAbstractFontEngine::Capabilities capabilities() const
    { return engineCapabilities; }

    bool drawAsOutline() const;

private:
    QAbstractFontEngine *engine;
    QAbstractFontEngine::Capabilities engineCapabilities;
};

QT_END_NAMESPACE

#endif

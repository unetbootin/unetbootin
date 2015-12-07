/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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

#include "qpixmapdatafactory_p.h"

#ifdef Q_WS_QWS
# include <QtGui/qscreen_qws.h>
#endif
#ifdef Q_WS_X11
# include <private/qpixmap_x11_p.h>
#endif
#ifdef Q_WS_WIN
# include <private/qpixmap_raster_p.h>
#endif
#ifdef Q_WS_MAC
# include <private/qpixmap_mac_p.h>
#endif

QT_BEGIN_NAMESPACE

#if !defined(Q_WS_QWS)

class QSimplePixmapDataFactory : public QPixmapDataFactory
{
public:
    ~QSimplePixmapDataFactory() {}
    QPixmapData* create(QPixmapData::PixelType type);
};

QPixmapData* QSimplePixmapDataFactory::create(QPixmapData::PixelType type)
{
#if defined(Q_WS_X11)
    return new QX11PixmapData(type);
#elif defined(Q_WS_WIN)
    return new QRasterPixmapData(type);
#elif defined(Q_WS_MAC)
    return new QMacPixmapData(type);
#else
#error QSimplePixmapDataFactory::create() not implemented
#endif
}

Q_GLOBAL_STATIC(QSimplePixmapDataFactory, factory);

#endif // !defined(Q_WS_QWS)

QPixmapDataFactory::~QPixmapDataFactory()
{
}

QPixmapDataFactory* QPixmapDataFactory::instance(int screen)
{
    Q_UNUSED(screen);
#ifdef Q_WS_QWS
    return QScreen::instance()->pixmapDataFactory();
#else
    return factory();
#endif
}

QT_END_NAMESPACE

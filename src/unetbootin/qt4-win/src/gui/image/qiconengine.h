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

#ifndef QICONENGINE_H
#define QICONENGINE_H

#include <QtCore/qglobal.h>
#include <QtGui/qicon.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Gui)

class Q_GUI_EXPORT QIconEngine
{
public:
    virtual ~QIconEngine();
    virtual void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) = 0;
    virtual QSize actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state);
    virtual QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state);

    virtual void addPixmap(const QPixmap &pixmap, QIcon::Mode mode, QIcon::State state);
    virtual void addFile(const QString &fileName, const QSize &size, QIcon::Mode mode, QIcon::State state);

#if 0
    virtual int frameCount(QIcon::Mode fromMode, QIcon::State fromState, QIcon::Mode toMode, QIcon::State toState);
    virtual void paintFrame(QPainter *painter, const QRect &rect, int frameNumber, QIcon::Mode fromMode, QIcon::State fromState, QIcon::Mode toMode, QIcon::State toState);
#endif
};

// ### Qt 5: move the below into QIconEngine
class Q_GUI_EXPORT QIconEngineV2 : public QIconEngine
{
public:
    virtual QString key() const;
    virtual QIconEngineV2 *clone() const;
    virtual bool read(QDataStream &in);
    virtual bool write(QDataStream &out) const;
    virtual void virtual_hook(int id, void *data);
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // QICONENGINE_H

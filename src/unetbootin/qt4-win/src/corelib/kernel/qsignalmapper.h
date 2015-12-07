/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#ifndef QSIGNALMAPPER_H
#define QSIGNALMAPPER_H

#include <QtCore/qobject.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

#ifndef QT_NO_SIGNALMAPPER
class QSignalMapperPrivate;

class Q_CORE_EXPORT QSignalMapper : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QSignalMapper)
public:
    explicit QSignalMapper(QObject *parent = 0);
    ~QSignalMapper();

    void setMapping(QObject *sender, int id);
    void setMapping(QObject *sender, const QString &text);
    void setMapping(QObject *sender, QWidget *widget);
    void setMapping(QObject *sender, QObject *object);
    void removeMappings(QObject *sender);

    QObject *mapping(int id) const;
    QObject *mapping(const QString &text) const;
    QObject *mapping(QWidget *widget) const;
    QObject *mapping(QObject *object) const;

Q_SIGNALS:
    void mapped(int);
    void mapped(const QString &);
    void mapped(QWidget *);
    void mapped(QObject *);

public Q_SLOTS:
    void map();
    void map(QObject *sender);

private:
    Q_DISABLE_COPY(QSignalMapper)
    Q_PRIVATE_SLOT(d_func(), void _q_senderDestroyed())

#ifdef QT3_SUPPORT
public:
    QT3_SUPPORT_CONSTRUCTOR QSignalMapper(QObject *parent, const char *name);
#endif
};
#endif // QT_NO_SIGNALMAPPER

QT_END_NAMESPACE

QT_END_HEADER

#endif // QSIGNALMAPPER_H

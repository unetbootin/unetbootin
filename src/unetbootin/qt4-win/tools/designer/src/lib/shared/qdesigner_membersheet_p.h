/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of Qt Designer.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#ifndef QDESIGNER_MEMBERSHEET_H
#define QDESIGNER_MEMBERSHEET_H

#include "shared_global_p.h"

#include <QtDesigner/membersheet.h>
#include <QtDesigner/default_extensionfactory.h>
#include <QtCore/QStringList>

QT_BEGIN_NAMESPACE

class QDesignerMemberSheetPrivate;

class QDESIGNER_SHARED_EXPORT QDesignerMemberSheet: public QObject, public QDesignerMemberSheetExtension
{
    Q_OBJECT
    Q_INTERFACES(QDesignerMemberSheetExtension)

public:
    explicit QDesignerMemberSheet(QObject *object, QObject *parent = 0);
    virtual ~QDesignerMemberSheet();

    virtual int indexOf(const QString &name) const;

    virtual int count() const;
    virtual QString memberName(int index) const;

    virtual QString memberGroup(int index) const;
    virtual void setMemberGroup(int index, const QString &group);

    virtual bool isVisible(int index) const;
    virtual void setVisible(int index, bool b);

    virtual bool isSignal(int index) const;
    virtual bool isSlot(int index) const;
    virtual bool inheritedFromWidget(int index) const;

    static bool signalMatchesSlot(const QString &signal, const QString &slot);

    virtual QString declaredInClass(int index) const;

    virtual QString signature(int index) const;
    virtual QList<QByteArray> parameterTypes(int index) const;
    virtual QList<QByteArray> parameterNames(int index) const;

private:
    QDesignerMemberSheetPrivate *d;
};

class QDESIGNER_SHARED_EXPORT QDesignerMemberSheetFactory: public QExtensionFactory
{
    Q_OBJECT
    Q_INTERFACES(QAbstractExtensionFactory)

public:
    QDesignerMemberSheetFactory(QExtensionManager *parent = 0);

protected:
    virtual QObject *createExtension(QObject *object, const QString &iid, QObject *parent) const;
};

QT_END_NAMESPACE

#endif // QDESIGNER_MEMBERSHEET_H

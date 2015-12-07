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

#ifndef SIGNALSLOTEDITOR_H
#define SIGNALSLOTEDITOR_H

#include "signalsloteditor_global.h"

#include <QtDesigner/private/connectionedit_p.h>

#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>

QT_BEGIN_NAMESPACE

class DomConnections;

namespace qdesigner_internal {

class SignalSlotConnection;

class QT_SIGNALSLOTEDITOR_EXPORT SignalSlotEditor : public ConnectionEdit
{
    Q_OBJECT

public:
    SignalSlotEditor(QDesignerFormWindowInterface *form_window, QWidget *parent);

    virtual void setSignal(SignalSlotConnection *con, const QString &member);
    virtual void setSlot(SignalSlotConnection *con, const QString &member);
    virtual void setSource(Connection *con, const QString &obj_name);
    virtual void setTarget(Connection *con, const QString &obj_name);

    DomConnections *toUi() const;
    void fromUi(const DomConnections *connections, QWidget *parent);

    QDesignerFormWindowInterface *formWindow() const { return m_form_window; }

    QObject *objectByName(QWidget *topLevel, const QString &name) const;

    void addEmptyConnection();

protected:
    virtual QWidget *widgetAt(const QPoint &pos) const;

private:
    virtual Connection *createConnection(QWidget *source, QWidget *destination);
    virtual void modifyConnection(Connection *con);

    QDesignerFormWindowInterface *m_form_window;
    bool m_showAllSignalsSlots;

    friend class SetMemberCommand;
};

} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // SIGNALSLOTEDITOR_H

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

#ifndef METADATABASE_H
#define METADATABASE_H

#include "shared_global_p.h"

#include <QtDesigner/QDesignerMetaDataBaseInterface>

#include <QtCore/QHash>
#include <QtCore/QStringList>
#include <QtGui/QCursor>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

class QDESIGNER_SHARED_EXPORT MetaDataBaseItem: public QDesignerMetaDataBaseItemInterface
{
public:
    explicit MetaDataBaseItem(QObject *object);
    virtual ~MetaDataBaseItem();

    virtual QString name() const;
    virtual void setName(const QString &name);
    
    typedef QList<QWidget*> TabOrder;
    virtual TabOrder tabOrder() const;
    virtual void setTabOrder(const TabOrder &tabOrder);

    virtual bool enabled() const;
    virtual void setEnabled(bool b);
    
    QString customClassName() const;
    void setCustomClassName(const QString &customClassName);

    QString propertyComment(const QString &name) const;
    void setPropertyComment(const QString &name, const QString &comment);

    typedef QHash<QString, QString> PropertyComments;
    
    const PropertyComments &comments() const { return m_comments; }

    QString script() const;
    void setScript(const QString &script);

    QStringList fakeSlots() const;
    void setFakeSlots(const QStringList &);

    QStringList fakeSignals() const;
    void setFakeSignals(const QStringList &);

private:
    QObject *m_object;
    TabOrder m_tabOrder;
    PropertyComments m_comments;
    bool m_enabled;
    QString m_customClassName;
    QString m_script;
    QStringList m_fakeSlots;
    QStringList m_fakeSignals;
};

class QDESIGNER_SHARED_EXPORT MetaDataBase: public QDesignerMetaDataBaseInterface
{
    Q_OBJECT
public:
    explicit MetaDataBase(QDesignerFormEditorInterface *core, QObject *parent = 0);
    virtual ~MetaDataBase();

    virtual QDesignerFormEditorInterface *core() const;

    virtual QDesignerMetaDataBaseItemInterface *item(QObject *object) const { return metaDataBaseItem(object); }
    virtual MetaDataBaseItem *metaDataBaseItem(QObject *object) const;
    virtual void add(QObject *object);
    virtual void remove(QObject *object);

    virtual QList<QObject*> objects() const;

    void dump();

private slots:
    void slotDestroyed(QObject *object);

private:
    QDesignerFormEditorInterface *m_core;
    typedef QHash<QObject *, MetaDataBaseItem*> ItemMap;
    ItemMap m_items;
};

    // promotion convenience
    QDESIGNER_SHARED_EXPORT bool promoteWidget(QDesignerFormEditorInterface *core,QWidget *widget,const QString &customClassName);
    QDESIGNER_SHARED_EXPORT void demoteWidget(QDesignerFormEditorInterface *core,QWidget *widget); 
    QDESIGNER_SHARED_EXPORT bool isPromoted(QDesignerFormEditorInterface *core, QWidget* w);
    QDESIGNER_SHARED_EXPORT QString promotedCustomClassName(QDesignerFormEditorInterface *core, QWidget* w);
    QDESIGNER_SHARED_EXPORT QString promotedExtends(QDesignerFormEditorInterface *core, QWidget* w);
    
    // Property comment helpers
    QDESIGNER_SHARED_EXPORT QString propertyComment(QDesignerFormEditorInterface* core, QObject *o, const QString &propertyName);
    QDESIGNER_SHARED_EXPORT bool setPropertyComment(QDesignerFormEditorInterface* core, QObject *o, const QString &propertyName, const QString &value);
} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // METADATABASE_H

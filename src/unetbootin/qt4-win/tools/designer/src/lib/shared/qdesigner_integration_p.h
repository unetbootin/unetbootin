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

#ifndef QDESIGNER_INTEGRATION_H
#define QDESIGNER_INTEGRATION_H

#include "shared_global_p.h"
#include <QtDesigner/QDesignerIntegrationInterface>

#include <QtCore/QObject>

QT_BEGIN_NAMESPACE

class QDesignerFormEditorInterface;
class QDesignerFormWindowInterface;
class QDesignerResourceBrowserInterface;

class QVariant;
class QWidget;

namespace qdesigner_internal {

struct Selection;
class QDesignerIntegrationPrivate;

class QDESIGNER_SHARED_EXPORT QDesignerIntegration: public QDesignerIntegrationInterface
{
    Q_OBJECT
public:
    explicit QDesignerIntegration(QDesignerFormEditorInterface *core, QObject *parent = 0);
    virtual ~QDesignerIntegration();

    static void requestHelp(const QDesignerFormEditorInterface *core, const QString &manual, const QString &document);

    virtual QWidget *containerWindow(QWidget *widget) const;

    // Load plugins into widget database and factory.
    static void initializePlugins(QDesignerFormEditorInterface *formEditor);
    void emitObjectNameChanged(QDesignerFormWindowInterface *formWindow, QObject *object, const QString &newName, const QString &oldName);

    // Create a resource browser specific to integration. Language integration takes precedence
    virtual QDesignerResourceBrowserInterface *createResourceBrowser(QWidget *parent = 0);

signals:
    void propertyChanged(QDesignerFormWindowInterface *formWindow, const QString &name, const QVariant &value);
    void objectNameChanged(QDesignerFormWindowInterface *formWindow, QObject *object, const QString &newName, const QString &oldName);
    void helpRequested(const QString &manual, const QString &document);

public slots:
    virtual void updateProperty(const QString &name, const QVariant &value, bool enableSubPropertyHandling);
    // Additional signals of designer property editor
    virtual void updatePropertyComment(const QString &name, const QString &value);
    virtual void resetProperty(const QString &name);
    virtual void addDynamicProperty(const QString &name, const QVariant &value);
    virtual void removeDynamicProperty(const QString &name);


    virtual void updateActiveFormWindow(QDesignerFormWindowInterface *formWindow);
    virtual void setupFormWindow(QDesignerFormWindowInterface *formWindow);
    virtual void updateSelection();
    virtual void updateGeometry();
    virtual void activateWidget(QWidget *widget);

    void updateCustomWidgetPlugins();

private slots:
    void updatePropertyPrivate(const QString &name, const QVariant &value);

private:
    void initialize();
    void getSelection(Selection &s);
    QObject *propertyEditorObject();

    QDesignerIntegrationPrivate *m_d;
};

} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // QDESIGNER_INTEGRATION_H

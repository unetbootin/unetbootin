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

#ifndef QDESIGNER_FORMBUILDER_H
#define QDESIGNER_FORMBUILDER_H

#include "shared_global_p.h"

#include <QtDesigner/private/formscriptrunner_p.h>
#include <QtDesigner/formbuilder.h>

#include <QtCore/QMap>
#include <QtCore/QSet>

QT_BEGIN_NAMESPACE

class QDesignerFormEditorInterface;
class QDesignerFormWindowInterface;

class QPixmap;
class QtResourceSet;

namespace qdesigner_internal {

class DesignerPixmapCache;
class DesignerIconCache;

// Form builder used for previewing forms
class QDESIGNER_SHARED_EXPORT QDesignerFormBuilder: public QFormBuilder
{
public:
    enum Mode {
        DisableScripts,
        EnableScripts
    };

    QDesignerFormBuilder(QDesignerFormEditorInterface *core, Mode mode);

    QWidget *createWidgetFromContents(const QString &contents, QWidget *parentWidget = 0);

    virtual QWidget *createWidget(DomWidget *ui_widget, QWidget *parentWidget = 0)
    { return QFormBuilder::create(ui_widget, parentWidget); }

    inline QDesignerFormEditorInterface *core() const
    { return m_core; }

    typedef QFormScriptRunner::Errors ScriptErrors;
    // Create a preview widget (for integrations) or return 0. The widget has to be embedded into a main window.
    // Experimental, depending on script support.
    static QWidget *createPreview(const QDesignerFormWindowInterface *fw, const QString &styleName /* ="" */,
                                  const QString &appStyleSheet  /* ="" */,
                                  ScriptErrors *scriptErrors, QString *errorMessage);
    // Convenience that pops up message boxes in case of failures.
    static QWidget *createPreview(const QDesignerFormWindowInterface *fw, const QString &styleName = QString());
    //  Create a preview widget (for integrations) or return 0. The widget has to be embedded into a main window.
    static QWidget *createPreview(const QDesignerFormWindowInterface *fw, const QString &styleName, const QString &appStyleSheet, QString *errorMessage);
    // Convenience that pops up message boxes in case of failures.
    static QWidget *createPreview(const QDesignerFormWindowInterface *fw, const QString &styleName, const QString &appStyleSheet);

    // Create a preview image
    static QPixmap createPreviewPixmap(const QDesignerFormWindowInterface *fw, const QString &styleName = QString(), const QString &appStyleSheet = QString());

protected:
    using QFormBuilder::createDom;
    using QFormBuilder::create;

    virtual QWidget *create(DomUI *ui, QWidget *parentWidget);
    virtual DomWidget *createDom(QWidget *widget, DomWidget *ui_parentWidget, bool recursive = true);
    virtual QWidget *create(DomWidget *ui_widget, QWidget *parentWidget);
    virtual QLayout *create(DomLayout *ui_layout, QLayout *layout, QWidget *parentWidget);
    virtual void createResources(DomResources *resources);

    virtual QWidget *createWidget(const QString &widgetName, QWidget *parentWidget, const QString &name);
    virtual bool addItem(DomWidget *ui_widget, QWidget *widget, QWidget *parentWidget);
    virtual bool addItem(DomLayoutItem *ui_item, QLayoutItem *item, QLayout *layout);

    virtual QIcon nameToIcon(const QString &filePath, const QString &qrcPath);
    virtual QPixmap nameToPixmap(const QString &filePath, const QString &qrcPath);

    virtual void applyProperties(QObject *o, const QList<DomProperty*> &properties);

    virtual void loadExtraInfo(DomWidget *ui_widget, QWidget *widget, QWidget *parentWidget);

    QtResourceSet *internalResourceSet() const { return m_tempResourceSet; }

private:
    QDesignerFormEditorInterface *m_core;
    const Mode m_mode;

    typedef QSet<QWidget *> WidgetSet;
    WidgetSet m_customWidgetsWithScript;

    DesignerPixmapCache *m_pixmapCache;
    DesignerIconCache *m_iconCache;
    bool m_ignoreCreateResources;
    QtResourceSet *m_tempResourceSet;
};

} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // QDESIGNER_FORMBUILDER_H

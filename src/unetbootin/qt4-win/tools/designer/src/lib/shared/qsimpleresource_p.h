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

#ifndef QSIMPLERESOURCE_H
#define QSIMPLERESOURCE_H

#include "shared_global_p.h"
#include "abstractformbuilder.h"

QT_BEGIN_NAMESPACE

class DomScript;

class QDesignerFormEditorInterface;

namespace qdesigner_internal {

class QDESIGNER_SHARED_EXPORT QSimpleResource : public QAbstractFormBuilder
{
public:
    explicit QSimpleResource(QDesignerFormEditorInterface *core);
    virtual ~QSimpleResource();

    QBrush setupBrush(DomBrush *brush);
    DomBrush *saveBrush(const QBrush &brush);

    inline QDesignerFormEditorInterface *core() const
    { return m_core; }

    // Query extensions for additional data
    static void addExtensionDataToDOM(QAbstractFormBuilder *afb,
                                      QDesignerFormEditorInterface *core,
                                      DomWidget *ui_widget, QWidget *widget);
    static void applyExtensionDataFromDOM(QAbstractFormBuilder *afb,
                                          QDesignerFormEditorInterface *core,
                                          DomWidget *ui_widget, QWidget *widget,
                                          bool applyState);
    // Enable warnings while saving. Turn off for backups.
    static bool setWarningsEnabled(bool warningsEnabled);
    static bool warningsEnabled();
    // Return the script returned by the CustomWidget codeTemplate API
    static QString customWidgetScript(QDesignerFormEditorInterface *core, QObject *object);
    static QString customWidgetScript(QDesignerFormEditorInterface *core, const QString &className);
    static bool hasCustomWidgetScript(QDesignerFormEditorInterface *core, QObject *object);

protected:
    virtual QIcon nameToIcon(const QString &filePath, const QString &qrcPath);
    virtual QString iconToFilePath(const QIcon &pm) const;
    virtual QString iconToQrcPath(const QIcon &pm) const;
    virtual QPixmap nameToPixmap(const QString &filePath, const QString &qrcPath);
    virtual QString pixmapToFilePath(const QPixmap &pm) const;
    virtual QString pixmapToQrcPath(const QPixmap &pm) const;

    enum ScriptSource { ScriptDesigner, ScriptExtension, ScriptCustomWidgetPlugin };
    static DomScript*createScript(const QString &script, ScriptSource source);
    typedef QList<DomScript*> DomScripts;
    static void addScript(const QString &script, ScriptSource source, DomScripts &domScripts);

private:
    static bool m_warningsEnabled;
    QDesignerFormEditorInterface *m_core;
};

// Contents of clipboard for formbuilder copy and paste operations
// (Actions and widgets)
struct QDESIGNER_SHARED_EXPORT FormBuilderClipboard {
    typedef QList<QAction*> ActionList;

    FormBuilderClipboard() {}
    FormBuilderClipboard(QWidget *w);

    bool empty() const;

    QWidgetList m_widgets;
    ActionList m_actions;
};

// Base class for a form builder used in the editor that
// provides copy and paste.(move into base interface)
class QDESIGNER_SHARED_EXPORT QEditorFormBuilder : public QSimpleResource
{
public:
    explicit QEditorFormBuilder(QDesignerFormEditorInterface *core) : QSimpleResource(core) {}

    virtual bool copy(QIODevice *dev, const FormBuilderClipboard &selection) = 0;
    virtual DomUI *copy(const FormBuilderClipboard &selection) = 0;

    // A widget parent needs to be specified, otherwise, the widget factory cannot locate the form window via parent
    // and thus is not able to construct special widgets (QLayoutWidget).
    virtual FormBuilderClipboard paste(DomUI *ui, QWidget *widgetParent, QObject *actionParent = 0) = 0;
    virtual FormBuilderClipboard paste(QIODevice *dev, QWidget *widgetParent, QObject *actionParent = 0) = 0;
};

} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif

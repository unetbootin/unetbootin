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

#ifndef QDESIGNER_ABTRACT_LANGUAGE_H
#define QDESIGNER_ABTRACT_LANGUAGE_H

#include <QtDesigner/extension.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class QDialog;
class QWidget;
class QDesignerFormWindowInterface;
class QDesignerFormEditorInterface;
class QDesignerResourceBrowserInterface;

class QDesignerLanguageExtension
{
public:
    virtual ~QDesignerLanguageExtension() {}

    virtual QDialog *createFormWindowSettingsDialog(QDesignerFormWindowInterface *formWindow, QWidget *parentWidget) = 0;
    virtual QDesignerResourceBrowserInterface *createResourceBrowser(QWidget *parentWidget) = 0;

    virtual QDialog *createPromotionDialog(QDesignerFormEditorInterface *formEditor, QWidget *parentWidget = 0) = 0;

    virtual QDialog *createPromotionDialog(QDesignerFormEditorInterface *formEditor,
                                           const QString &promotableWidgetClassName,
                                           QString *promoteToClassName,
                                           QWidget *parentWidget = 0) = 0;

    virtual bool isLanguageResource(const QString &path) const = 0;

    virtual QString classNameOf(QObject *object) const = 0;

    virtual bool signalMatchesSlot(const QString &signal, const QString &slot) const = 0;

    virtual QString widgetBoxContents() const = 0;

    virtual QString uiExtension() const = 0;
};

Q_DECLARE_EXTENSION_INTERFACE(QDesignerLanguageExtension, "com.trolltech.Qt.Designer.Language.3")

QT_END_NAMESPACE

QT_END_HEADER

#endif // QDESIGNER_ABTRACT_LANGUAGE_H

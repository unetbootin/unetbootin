/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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


#ifndef ICONSELECTOR_H
#define ICONSELECTOR_H

#include "shared_global_p.h"
#include <QtGui/QWidget>
#include <QtGui/QDialog>

QT_BEGIN_NAMESPACE

class QtResourceModel;
class QDesignerFormEditorInterface;
class QDesignerDialogGuiInterface;
class QDesignerResourceBrowserInterface;

namespace qdesigner_internal {

class DesignerIconCache;
class DesignerPixmapCache;
class PropertySheetIconValue;

// Resource Dialog that embeds the language-dependent resource widget as returned by the language extension
class QDESIGNER_SHARED_EXPORT LanguageResourceDialog : public QDialog
{
    Q_OBJECT

    LanguageResourceDialog(QDesignerResourceBrowserInterface *rb, QWidget *parent = 0);

public:
    virtual ~LanguageResourceDialog();
    // Factory: Returns 0 if the language extension does not provide a resource browser.
    static LanguageResourceDialog* create(QDesignerFormEditorInterface *core, QWidget *parent);

    void setCurrentPath(const QString &filePath);
    QString currentPath() const;

private:
    class LanguageResourceDialogPrivate *d_ptr;
    Q_DECLARE_PRIVATE(LanguageResourceDialog)
    Q_DISABLE_COPY(LanguageResourceDialog)
    Q_PRIVATE_SLOT(d_func(), void slotAccepted())
    Q_PRIVATE_SLOT(d_func(), void slotPathChanged(QString))

};

class QDESIGNER_SHARED_EXPORT IconSelector: public QWidget
{
    Q_OBJECT
public:
    IconSelector(QWidget *parent = 0);
    virtual ~IconSelector();

    void setFormEditor(QDesignerFormEditorInterface *core); // required for dialog gui.
    void setIconCache(DesignerIconCache *iconCache);
    void setPixmapCache(DesignerPixmapCache *pixmapCache);

    void setIcon(const PropertySheetIconValue &icon);
    PropertySheetIconValue icon() const;

    // Check whether a pixmap may be read
    enum CheckMode { CheckFast, CheckFully };
    static bool checkPixmap(const QString &fileName, CheckMode cm = CheckFully, QString *errorMessage = 0);
    // Choose a pixmap from file
    static QString choosePixmapFile(const QString &directory, QDesignerDialogGuiInterface *dlgGui, QWidget *parent);
    // Choose a pixmap from resource; use language-dependent resource browser if present
    static QString choosePixmapResource(QDesignerFormEditorInterface *core, QtResourceModel *resourceModel, const QString &oldPath, QWidget *parent);

signals:
    void iconChanged(const PropertySheetIconValue &icon);
private:
    class IconSelectorPrivate *d_ptr;
    Q_DECLARE_PRIVATE(IconSelector)
    Q_DISABLE_COPY(IconSelector)

    Q_PRIVATE_SLOT(d_func(), void slotStateActivated())
    Q_PRIVATE_SLOT(d_func(), void slotSetActivated())
    Q_PRIVATE_SLOT(d_func(), void slotSetResourceActivated())
    Q_PRIVATE_SLOT(d_func(), void slotSetFileActivated())
    Q_PRIVATE_SLOT(d_func(), void slotResetActivated())
    Q_PRIVATE_SLOT(d_func(), void slotResetAllActivated())
    Q_PRIVATE_SLOT(d_func(), void slotUpdate())
};


} // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // ICONSELECTOR_H


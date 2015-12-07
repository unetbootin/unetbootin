/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef FORMWINDOWSETTINGS_H
#define FORMWINDOWSETTINGS_H

#include <QDialog>

QT_BEGIN_NAMESPACE
    
namespace Ui {
    class FormWindowSettings;
}

class QDesignerFormWindowInterface;

namespace qdesigner_internal {

struct FormWindowData;
class FormWindowBase;

/* Dialog to edit the settings of a QDesignerFormWindowInterface.
 * It sets the dirty flag on the form window if something was changed. */

class FormWindowSettings: public QDialog
{
    Q_DISABLE_COPY(FormWindowSettings)
    Q_OBJECT
public:
    explicit FormWindowSettings(QDesignerFormWindowInterface *formWindow);
    virtual ~FormWindowSettings();

    virtual void accept();

private:
    FormWindowData data() const;
    void setData(const FormWindowData&);

    Ui::FormWindowSettings *m_ui;
    FormWindowBase *m_formWindow;
    FormWindowData *m_oldData;
};
}

QT_END_NAMESPACE

#endif // FORMWINDOWSETTINGS_H

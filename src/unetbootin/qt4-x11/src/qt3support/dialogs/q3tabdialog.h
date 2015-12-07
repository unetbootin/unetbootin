/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#ifndef Q3TABDIALOG_H
#define Q3TABDIALOG_H

#include <QtGui/qdialog.h>
#include <QtGui/qicon.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

class  QTabBar;
class  QTab;
class  Q3TabDialogPrivate;

class Q_COMPAT_EXPORT Q3TabDialog : public QDialog
{
    Q_OBJECT
public:
    Q3TabDialog(QWidget* parent=0, const char* name=0, bool modal=false, Qt::WindowFlags f=0);
    ~Q3TabDialog();

    void show();
    void setFont(const QFont & font);

    void addTab(QWidget *, const QString &);
    void addTab(QWidget *child, const QIcon& iconset, const QString &label);

    void insertTab(QWidget *, const QString &, int index = -1);
    void insertTab(QWidget *child, const QIcon& iconset, const QString &label, int index = -1);

    void changeTab(QWidget *, const QString &);
    void changeTab(QWidget *child, const QIcon& iconset, const QString &label);

    bool isTabEnabled( QWidget *) const;
    void setTabEnabled(QWidget *, bool);
    bool isTabEnabled(const char*) const; // compatibility
    void setTabEnabled(const char*, bool); // compatibility

    void showPage(QWidget *);
    void removePage(QWidget *);
    QString tabLabel(QWidget *);

    QWidget * currentPage() const;

    void setDefaultButton(const QString &text);
    void setDefaultButton();
    bool hasDefaultButton() const;

    void setHelpButton(const QString &text);
    void setHelpButton();
    bool hasHelpButton() const;

    void setCancelButton(const QString &text);
    void setCancelButton();
    bool hasCancelButton() const;

    void setApplyButton(const QString &text);
    void setApplyButton();
    bool hasApplyButton() const;

#ifndef qdoc
    void setOKButton(const QString &text = QString());
#endif
    void setOkButton(const QString &text);
    void setOkButton();
    bool hasOkButton() const;

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
    void styleChange(QStyle&);
    void setTabBar(QTabBar*);
    QTabBar* tabBar() const;

Q_SIGNALS:
    void aboutToShow();

    void applyButtonPressed();
    void cancelButtonPressed();
    void defaultButtonPressed();
    void helpButtonPressed();

    void currentChanged(QWidget *);
    void selected(const QString&); // obsolete

private:
    void setSizes();
    void setUpLayout();

    Q3TabDialogPrivate *d;

    Q_DISABLE_COPY(Q3TabDialog)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3TABDIALOG_H

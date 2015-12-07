/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

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

#ifndef Q3WIZARD_H
#define Q3WIZARD_H

#include <QtGui/qdialog.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

class QHBoxLayout;
class Q3WizardPrivate;

class Q_COMPAT_EXPORT Q3Wizard : public QDialog
{
    Q_OBJECT
    Q_PROPERTY( QFont titleFont READ titleFont WRITE setTitleFont )

public:
    Q3Wizard( QWidget* parent=0, const char* name=0, bool modal=false, Qt::WindowFlags f=0 );
    ~Q3Wizard();

    void setVisible(bool);

    void setFont( const QFont & font );

    virtual void addPage( QWidget *, const QString & );
    virtual void insertPage( QWidget*, const QString&, int );
    virtual void removePage( QWidget * );

    QString title( QWidget * ) const;
    void setTitle( QWidget *, const QString & );
    QFont titleFont() const;
    void setTitleFont( const QFont & );

    virtual void showPage( QWidget * );

    QWidget * currentPage() const;

    QWidget* page( int ) const;
    int pageCount() const;
    int indexOf( QWidget* ) const;

    virtual bool appropriate( QWidget * ) const;
    virtual void setAppropriate( QWidget *, bool );

    QPushButton * backButton() const;
    QPushButton * nextButton() const;
    QPushButton * finishButton() const;
    QPushButton * cancelButton() const;
    QPushButton * helpButton() const;

    bool eventFilter( QObject *, QEvent * );

public Q_SLOTS:
    virtual void setBackEnabled( QWidget *, bool );
    virtual void setNextEnabled( QWidget *, bool );
    virtual void setFinishEnabled( QWidget *, bool );

    virtual void setHelpEnabled( QWidget *, bool );

    // obsolete
    virtual void setFinish(  QWidget *, bool ) {}

protected Q_SLOTS:
    virtual void back();
    virtual void next();
    virtual void help();

Q_SIGNALS:
    void helpClicked();
    void selected( const QString& );

protected:
    virtual void layOutButtonRow( QHBoxLayout * );
    virtual void layOutTitleRow( QHBoxLayout *, const QString & );

private:
    void setBackEnabled( bool );
    void setNextEnabled( bool );

    void setHelpEnabled( bool );

    void setNextPage( QWidget * );

    void updateButtons();

    void layOut();

    Q3WizardPrivate *d;

    Q_DISABLE_COPY(Q3Wizard)
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3WIZARD_H

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

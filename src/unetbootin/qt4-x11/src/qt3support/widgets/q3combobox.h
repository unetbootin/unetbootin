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

#ifndef Q3COMBOBOX_H
#define Q3COMBOBOX_H

#include <QtGui/qwidget.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

#ifndef QT_NO_COMBOBOX

class Q3StrList;
class QStringList;
class QLineEdit;
class QValidator;
class Q3ListBox;
class Q3ComboBoxData;
class QWheelEvent;

class Q_COMPAT_EXPORT Q3ComboBox : public QWidget
{
    Q_OBJECT
    Q_ENUMS( Policy )
    Q_PROPERTY( bool editable READ editable WRITE setEditable )
    Q_PROPERTY( int count READ count )
    Q_PROPERTY( QString currentText READ currentText WRITE setCurrentText DESIGNABLE false )
    Q_PROPERTY( int currentItem READ currentItem WRITE setCurrentItem )
    Q_PROPERTY( bool autoResize READ autoResize WRITE setAutoResize DESIGNABLE false )
    Q_PROPERTY( int sizeLimit READ sizeLimit WRITE setSizeLimit )
    Q_PROPERTY( int maxCount READ maxCount WRITE setMaxCount )
    Q_PROPERTY( Policy insertionPolicy READ insertionPolicy WRITE setInsertionPolicy )
    Q_PROPERTY( bool autoCompletion READ autoCompletion WRITE setAutoCompletion )
    Q_PROPERTY( bool duplicatesEnabled READ duplicatesEnabled WRITE setDuplicatesEnabled )

public:
    Q3ComboBox( QWidget* parent=0, const char* name=0 );
    Q3ComboBox( bool rw, QWidget* parent=0, const char* name=0 );
    ~Q3ComboBox();

    int		count() const;

    void	insertStringList( const QStringList &, int index=-1 );
    void	insertStrList( const Q3StrList &, int index=-1 );
    void	insertStrList( const Q3StrList *, int index=-1 );
    void	insertStrList( const char **, int numStrings=-1, int index=-1);

    void	insertItem( const QString &text, int index=-1 );
    void	insertItem( const QPixmap &pixmap, int index=-1 );
    void	insertItem( const QPixmap &pixmap, const QString &text, int index=-1 );

    void	removeItem( int index );

    int		currentItem() const;
    virtual void setCurrentItem( int index );

    QString 	currentText() const;
    virtual void setCurrentText( const QString& );

    QString 	text( int index ) const;
    const QPixmap *pixmap( int index ) const;

    void	changeItem( const QString &text, int index );
    void	changeItem( const QPixmap &pixmap, int index );
    void	changeItem( const QPixmap &pixmap, const QString &text, int index );

    bool	autoResize()	const;
    virtual void setAutoResize( bool );
    QSize	sizeHint() const;

    void	setPalette( const QPalette & );
    void	setFont( const QFont & );
    void	setEnabled( bool );

    virtual void setSizeLimit( int );
    int		sizeLimit() const;

    virtual void setMaxCount( int );
    int		maxCount() const;

    enum Policy { NoInsertion,
                  AtTop,
                  AtCurrent,
                  AtBottom,
		  AfterCurrent,
                  BeforeCurrent,
                  NoInsert = NoInsertion,
                  InsertAtTop = AtTop,
                  InsertAtCurrent = AtCurrent,
                  InsertAtBottom = AtBottom,
                  InsertAfterCurrent = AfterCurrent,
                  InsertBeforeCurrent = BeforeCurrent
    };

    virtual void setInsertionPolicy( Policy policy );
    Policy	insertionPolicy() const;

    virtual void setValidator( const QValidator * );
    const QValidator * validator() const;

    virtual void setListBox( Q3ListBox * );
    Q3ListBox *	listBox() const;

    virtual void setLineEdit( QLineEdit *edit );
    QLineEdit*	lineEdit() const;

    virtual void setAutoCompletion( bool );
    bool	autoCompletion() const;

    bool	eventFilter( QObject *object, QEvent *event );

    void	setDuplicatesEnabled( bool enable );
    bool	duplicatesEnabled() const;

    bool	editable() const;
    void	setEditable( bool );

    virtual void popup();

    void	hide();

public Q_SLOTS:
    void	clear();
    void	clearValidator();
    void	clearEdit();
    virtual void setEditText( const QString &);

Q_SIGNALS:
    void	activated( int index );
    void	highlighted( int index );
    void	activated( const QString &);
    void	highlighted( const QString &);
    void	textChanged( const QString &);

private Q_SLOTS:
    void	internalActivate( int );
    void	internalHighlight( int );
    void	internalClickTimeout();
    void	returnPressed();

protected:
    void	paintEvent( QPaintEvent * );
    void	resizeEvent( QResizeEvent * );
    void	mousePressEvent( QMouseEvent * );
    void	mouseMoveEvent( QMouseEvent * );
    void	mouseReleaseEvent( QMouseEvent * );
    void	mouseDoubleClickEvent( QMouseEvent * );
    void	keyPressEvent( QKeyEvent *e );
    void	focusInEvent( QFocusEvent *e );
    void	focusOutEvent( QFocusEvent *e );
#ifndef QT_NO_WHEELEVENT
    void	wheelEvent( QWheelEvent *e );
#endif
    void	styleChange( QStyle& );

    void	updateMask();

private:
    void	setUpListBox();
    void	setUpLineEdit();
    void	popDownListBox();
    void	reIndex();
    void	currentChanged();
    int		completionIndex( const QString &, int ) const;

    Q3ComboBoxData	*d;

private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    Q3ComboBox( const Q3ComboBox & );
    Q3ComboBox &operator=( const Q3ComboBox & );
#endif
};


#endif // QT_NO_COMBOBOX

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3COMBOBOX_H

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

#ifndef Q3DATATABLE_H
#define Q3DATATABLE_H

#include <QtCore/qstring.h>
#include <QtCore/qvariant.h>
#include <Qt3Support/q3table.h>
#include <QtSql/qsql.h>
#include <Qt3Support/q3sqlcursor.h>
#include <QtSql/qsqlindex.h>
#include <Qt3Support/q3sqleditorfactory.h>
#include <Qt3Support/qiconset.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3Support)

#ifndef QT_NO_SQL_VIEW_WIDGETS

class QPainter;
class QSqlField;
class Q3SqlPropertyMap;
class Q3DataTablePrivate;

class Q_COMPAT_EXPORT Q3DataTable : public Q3Table
{
    Q_OBJECT

    Q_PROPERTY( QString nullText READ nullText WRITE setNullText )
    Q_PROPERTY( QString trueText READ trueText WRITE setTrueText )
    Q_PROPERTY( QString falseText READ falseText WRITE setFalseText )
    Q_PROPERTY( Qt::DateFormat dateFormat READ dateFormat WRITE setDateFormat )
    Q_PROPERTY( bool confirmEdits READ confirmEdits WRITE setConfirmEdits )
    Q_PROPERTY( bool confirmInsert READ confirmInsert WRITE setConfirmInsert )
    Q_PROPERTY( bool confirmUpdate READ confirmUpdate WRITE setConfirmUpdate )
    Q_PROPERTY( bool confirmDelete READ confirmDelete WRITE setConfirmDelete )
    Q_PROPERTY( bool confirmCancels READ confirmCancels WRITE setConfirmCancels )
    Q_PROPERTY( bool autoEdit READ autoEdit WRITE setAutoEdit )
    Q_PROPERTY( QString filter READ filter WRITE setFilter )
    Q_PROPERTY( QStringList sort READ sort WRITE setSort )
    Q_PROPERTY( int numCols READ numCols )
    Q_PROPERTY( int numRows READ numRows )

public:
    Q3DataTable ( QWidget* parent=0, const char* name=0 );
    Q3DataTable ( Q3SqlCursor* cursor, bool autoPopulate = false, QWidget* parent=0, const char* name=0 );
    ~Q3DataTable();

    virtual void addColumn( const QString& fieldName,
			    const QString& label = QString(),
			    int width = -1,
			    const QIconSet& iconset = QIconSet() );
    virtual void removeColumn( int col );
    virtual void setColumn( uint col, const QString& fieldName,
			    const QString& label = QString(),
			    int width = -1,
			    const QIconSet& iconset = QIconSet() );

    QString      nullText() const;
    QString      trueText() const;
    QString      falseText() const;
    Qt::DateFormat dateFormat() const;
    bool         confirmEdits() const;
    bool         confirmInsert() const;
    bool         confirmUpdate() const;
    bool         confirmDelete() const;
    bool         confirmCancels() const;
    bool         autoDelete() const;
    bool         autoEdit() const;
    QString      filter() const;
    QStringList  sort() const;

    virtual void setSqlCursor( Q3SqlCursor* cursor = 0,
			    bool autoPopulate = false, bool autoDelete = false );
    Q3SqlCursor* sqlCursor() const;

    virtual void setNullText( const QString& nullText );
    virtual void setTrueText( const QString& trueText );
    virtual void setFalseText( const QString& falseText );
    virtual void setDateFormat( const Qt::DateFormat f );
    virtual void setConfirmEdits( bool confirm );
    virtual void setConfirmInsert( bool confirm );
    virtual void setConfirmUpdate( bool confirm );
    virtual void setConfirmDelete( bool confirm );
    virtual void setConfirmCancels( bool confirm );
    virtual void setAutoDelete( bool enable );
    virtual void setAutoEdit( bool autoEdit );
    virtual void setFilter( const QString& filter );
    virtual void setSort( const QStringList& sort );
    virtual void setSort( const QSqlIndex& sort );

    enum Refresh {
	RefreshData = 1,
	RefreshColumns = 2,
	RefreshAll = 3
    };
    void         refresh( Refresh mode );
    void         sortColumn ( int col, bool ascending = true,
			      bool wholeRows = false );
    QString      text ( int row, int col ) const;
    QVariant     value ( int row, int col ) const;
    QSqlRecord*  currentRecord() const;

    void         installEditorFactory( Q3SqlEditorFactory * f );
    void         installPropertyMap( Q3SqlPropertyMap* m );

    int          numCols() const;
    int          numRows() const;
    void         setNumCols( int c );
    void         setNumRows ( int r );
    bool         findBuffer( const QSqlIndex& idx, int atHint = 0 );

    void	 hideColumn( int col );
    void	 showColumn( int col );
    int          indexOf( uint i ) const;
    void selectRow(int row);

Q_SIGNALS:
    void         currentChanged( QSqlRecord* record );
    void         primeInsert( QSqlRecord* buf );
    void         primeUpdate( QSqlRecord* buf );
    void         primeDelete( QSqlRecord* buf );
    void         beforeInsert( QSqlRecord* buf );
    void         beforeUpdate( QSqlRecord* buf );
    void         beforeDelete( QSqlRecord* buf );
    void         cursorChanged( QSql::Op mode );

public Q_SLOTS:
    virtual void find( const QString & str, bool caseSensitive,
			     bool backwards );
    virtual void sortAscending( int col );
    virtual void sortDescending( int col );
    virtual void refresh();
    void setColumnWidth( int col, int w );
    void adjustColumn( int col );
    void setColumnStretchable( int col, bool stretch );
    void swapColumns( int col1, int col2, bool swapHeaders = false );

protected:
    virtual bool insertCurrent();
    virtual bool updateCurrent();
    virtual bool deleteCurrent();

    virtual QSql::Confirm confirmEdit( QSql::Op m );
    virtual QSql::Confirm confirmCancel( QSql::Op m );

    virtual void handleError( const QSqlError& e );

    virtual bool beginInsert();
    virtual QWidget* beginUpdate ( int row, int col, bool replace );

    bool         eventFilter( QObject *o, QEvent *e );
    void         keyPressEvent( QKeyEvent* );
    void         resizeEvent ( QResizeEvent * );
    void         contentsMousePressEvent( QMouseEvent* e );
    void         contentsContextMenuEvent( QContextMenuEvent* e );
    void         endEdit( int row, int col, bool accept, bool replace );
    QWidget *    createEditor( int row, int col, bool initFromCell ) const;
    void         activateNextCell();
    void         reset();
    void         setSize( Q3SqlCursor* sql );
    void         repaintCell( int row, int col );
    void         paintCell ( QPainter * p, int row, int col, const QRect & cr,
			     bool selected, const QColorGroup &cg );
    virtual void paintField( QPainter * p, const QSqlField* field, const QRect & cr,
			     bool selected );
    void	 drawContents( QPainter * p, int cx, int cy, int cw, int ch );
    virtual int  fieldAlignment( const QSqlField* field );
    void         columnClicked ( int col );
    void         resizeData ( int len );

    Q3TableItem * item ( int row, int col ) const;
    void         setItem ( int row, int col, Q3TableItem * item );
    void         clearCell ( int row, int col ) ;
    void         setPixmap ( int row, int col, const QPixmap & pix );
    void         takeItem ( Q3TableItem * i );

private Q_SLOTS:
    void         loadNextPage();
    void         setCurrentSelection( int row, int col );
    void         updateCurrentSelection();
    void	 sliderPressed();
    void	 sliderReleased();
    void	 doInsertCurrent();
    void	 doUpdateCurrent();

private:
    void         drawContents( QPainter *p);
    QString	 fieldToString( const QSqlField * field );
    void         init();
    QWidget*     beginEdit ( int row, int col, bool replace );
    void         updateRow( int row );
    void         endInsert();
    void         endUpdate();
    Q3DataTablePrivate* d;

#if defined(Q_DISABLE_COPY) // Disabled copy constructor and operator=
    Q3DataTable( const Q3DataTable & );
    Q3DataTable &operator=( const Q3DataTable & );
#endif
};

#endif // QT_NO_SQL_VIEW_WIDGETS

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3DATATABLE_H

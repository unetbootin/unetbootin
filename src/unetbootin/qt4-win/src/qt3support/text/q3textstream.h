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

#ifndef Q3TEXTSTREAM_H
#define Q3TEXTSTREAM_H

#include <QtCore/qiodevice.h>
#include <QtCore/qstring.h>
#ifndef QT_NO_TEXTCODEC
#include <QtCore/qtextcodec.h>
#endif
#include <Qt3Support/q3cstring.h>

#include <stdio.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

class Q3TextStreamPrivate;

class Q_COMPAT_EXPORT Q3TextStream				// text stream class
{
public:
    enum Encoding { Locale, Latin1, Unicode, UnicodeNetworkOrder,
		    UnicodeReverse, RawUnicode, UnicodeUTF8 };

    void	setEncoding( Encoding );
#ifndef QT_NO_TEXTCODEC
    void	setCodec( QTextCodec* );
    QTextCodec *codec();
#endif

    Q3TextStream();
    Q3TextStream( QIODevice * );
    Q3TextStream( QString*, int mode );
    Q3TextStream( QString&, int mode );		// obsolete
    Q3TextStream( QByteArray&, int mode );
    Q3TextStream( FILE *, int mode );
    virtual ~Q3TextStream();

    QIODevice	*device() const;
    void	 setDevice( QIODevice * );
    void	 unsetDevice();

    bool	 atEnd() const;
    bool	 eof() const;

    Q3TextStream &operator>>( QChar & );
    Q3TextStream &operator>>( char & );
    Q3TextStream &operator>>( signed short & );
    Q3TextStream &operator>>( unsigned short & );
    Q3TextStream &operator>>( signed int & );
    Q3TextStream &operator>>( unsigned int & );
    Q3TextStream &operator>>( signed long & );
    Q3TextStream &operator>>( unsigned long & );
    Q3TextStream &operator>>( float & );
    Q3TextStream &operator>>( double & );
    Q3TextStream &operator>>( char * );
    Q3TextStream &operator>>( QString & );
    Q3TextStream &operator>>( Q3CString & );

    Q3TextStream &operator<<( QChar );
    Q3TextStream &operator<<( char );
    Q3TextStream &operator<<( signed short );
    Q3TextStream &operator<<( unsigned short );
    Q3TextStream &operator<<( signed int );
    Q3TextStream &operator<<( unsigned int );
    Q3TextStream &operator<<( signed long );
    Q3TextStream &operator<<( unsigned long );
    Q3TextStream &operator<<( float );
    Q3TextStream &operator<<( double );
    Q3TextStream &operator<<( const char* );
    Q3TextStream &operator<<( const QString & );
    Q3TextStream &operator<<( const Q3CString & );
    Q3TextStream &operator<<( void * );		// any pointer

    Q3TextStream &readRawBytes( char *, uint len );
    Q3TextStream &writeRawBytes( const char* , uint len );

    QString	readLine();
    QString	read();
    void	skipWhiteSpace();

    enum {
	skipws	  = 0x0001,			// skip whitespace on input
	left	  = 0x0002,			// left-adjust output
	right	  = 0x0004,			// right-adjust output
	internal  = 0x0008,			// pad after sign
	bin	  = 0x0010,			// binary format integer
	oct	  = 0x0020,			// octal format integer
	dec	  = 0x0040,			// decimal format integer
	hex	  = 0x0080,			// hex format integer
	showbase  = 0x0100,			// show base indicator
	showpoint = 0x0200,			// force decimal point (float)
	uppercase = 0x0400,			// upper-case hex output
	showpos	  = 0x0800,			// add '+' to positive integers
	scientific= 0x1000,			// scientific float output
	fixed	  = 0x2000			// fixed float output
    };

    static const int basefield;			// bin | oct | dec | hex
    static const int adjustfield;		// left | right | internal
    static const int floatfield;		// scientific | fixed

    int	  flags() const;
    int	  flags( int f );
    int	  setf( int bits );
    int	  setf( int bits, int mask );
    int	  unsetf( int bits );

    void  reset();

    int	  width()	const;
    int	  width( int );
    int	  fill()	const;
    int	  fill( int );
    int	  precision()	const;
    int	  precision( int );

private:
    long	input_int();
    void	init();
    Q3TextStream &output_int( int, ulong, bool );
    QIODevice	*dev;

    int		fflags;
    int		fwidth;
    int		fillchar;
    int		fprec;
    bool	doUnicodeHeader;
    bool	owndev;
    QTextCodec 	*mapper;
    QTextCodec::ConverterState mapperReadState;
    QTextCodec::ConverterState mapperWriteState;
    Q3TextStreamPrivate * d;
    QChar	unused1; // ### remove in Qt 4.0
    bool	latin1;
    bool 	internalOrder;
    bool	networkOrder;
    void	*unused2; // ### remove in Qt 4.0

    QChar	eat_ws();
    uint 	ts_getline( QChar* );
    void	ts_ungetc( QChar );
    QChar	ts_getc();
    uint	ts_getbuf( QChar*, uint );
    void	ts_putc(int);
    void	ts_putc(QChar);
    bool	ts_isspace(QChar);
    bool	ts_isdigit(QChar);
    ulong	input_bin();
    ulong	input_oct();
    ulong	input_dec();
    ulong	input_hex();
    double	input_double();
    Q3TextStream &writeBlock( const char* p, uint len );
    Q3TextStream &writeBlock( const QChar* p, uint len );

private:	// Disabled copy constructor and operator=
#if defined(Q_DISABLE_COPY)
    Q3TextStream( const Q3TextStream & );
    Q3TextStream &operator=( const Q3TextStream & );
#endif
};

/*****************************************************************************
  Q3TextStream inline functions
 *****************************************************************************/

inline QIODevice *Q3TextStream::device() const
{ return dev; }

inline bool Q3TextStream::atEnd() const
{ return dev ? dev->atEnd() : FALSE; }

inline bool Q3TextStream::eof() const
{ return atEnd(); }

inline int Q3TextStream::flags() const
{ return fflags; }

inline int Q3TextStream::flags( int f )
{ int oldf = fflags;  fflags = f;  return oldf; }

inline int Q3TextStream::setf( int bits )
{ int oldf = fflags;  fflags |= bits;  return oldf; }

inline int Q3TextStream::setf( int bits, int mask )
{ int oldf = fflags;  fflags = (fflags & ~mask) | (bits & mask); return oldf; }

inline int Q3TextStream::unsetf( int bits )
{ int oldf = fflags;  fflags &= ~bits;	return oldf; }

inline int Q3TextStream::width() const
{ return fwidth; }

inline int Q3TextStream::width( int w )
{ int oldw = fwidth;  fwidth = w;  return oldw;	 }

inline int Q3TextStream::fill() const
{ return fillchar; }

inline int Q3TextStream::fill( int f )
{ int oldc = fillchar;	fillchar = f;  return oldc;  }

inline int Q3TextStream::precision() const
{ return fprec; }

inline int Q3TextStream::precision( int p )
{ int oldp = fprec;  fprec = p;	 return oldp;  }

/*!
  Returns one character from the stream, or EOF.
*/
inline QChar Q3TextStream::ts_getc()
{ QChar r; return ( ts_getbuf( &r,1 ) == 1 ? r : QChar((ushort)0xffff) ); }

/*****************************************************************************
  Q3TextStream manipulators
 *****************************************************************************/

typedef Q3TextStream & (*Q3TSFUNC)(Q3TextStream &);// manipulator function
typedef int (Q3TextStream::*Q3TSMFI)(int);	// manipulator w/int argument

class Q_COMPAT_EXPORT Q3TSManip {			// text stream manipulator
public:
    Q3TSManip( Q3TSMFI m, int a ) { mf=m; arg=a; }
    void exec( Q3TextStream &s ) { (s.*mf)(arg); }
private:
    Q3TSMFI mf;					// Q3TextStream member function
    int	   arg;					// member function argument
};

Q_COMPAT_EXPORT inline Q3TextStream &operator>>( Q3TextStream &s, Q3TSFUNC f )
{ return (*f)( s ); }

Q_COMPAT_EXPORT inline Q3TextStream &operator<<( Q3TextStream &s, Q3TSFUNC f )
{ return (*f)( s ); }

Q_COMPAT_EXPORT inline Q3TextStream &operator<<( Q3TextStream &s, Q3TSManip m )
{ m.exec(s); return s; }

Q_COMPAT_EXPORT Q3TextStream &bin( Q3TextStream &s );	// set bin notation
Q_COMPAT_EXPORT Q3TextStream &oct( Q3TextStream &s );	// set oct notation
Q_COMPAT_EXPORT Q3TextStream &dec( Q3TextStream &s );	// set dec notation
Q_COMPAT_EXPORT Q3TextStream &hex( Q3TextStream &s );	// set hex notation
Q_COMPAT_EXPORT Q3TextStream &endl( Q3TextStream &s );	// insert EOL ('\n')
Q_COMPAT_EXPORT Q3TextStream &flush( Q3TextStream &s );	// flush output
Q_COMPAT_EXPORT Q3TextStream &ws( Q3TextStream &s );	// eat whitespace on input
Q_COMPAT_EXPORT Q3TextStream &reset( Q3TextStream &s );	// set default flags

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3TEXTSTREAM_H

/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include "ui3reader.h"
#include <QFile>
#include <QImage>
#include <QStringList>
#include <QDateTime>
#include <QFileInfo>
#include <QVector>
#include <stdio.h>
#include <ctype.h>

QT_BEGIN_NAMESPACE

// on embedded, we do not compress image data. Rationale: by mapping
// the ready-only data directly into memory we are both faster and
// more memory efficient
#if defined(Q_WS_QWS) && !defined(QT_NO_IMAGE_COLLECTION_COMPRESSION)
#  define QT_NO_IMAGE_COLLECTION_COMPRESSION
#elif defined (QT_NO_COMPRESS)
#  define QT_NO_IMAGE_COLLECTION_COMPRESSION
#endif

struct EmbedImage
{
    ~EmbedImage() { delete[] colorTable; }

    int width, height, depth;
    int numColors;
    QRgb* colorTable;
    QString name;
    QString cname;
    bool alpha;
#ifndef QT_NO_IMAGE_COLLECTION_COMPRESSION
    ulong compressed;
#endif
};

static QString convertToCIdentifier( const char *s )
{
    QByteArray r = s;
    int len = r.length();
    if ( len > 0 && !isalpha( (char)r[0] ) )
        r[0] = '_';
    for ( int i=1; i<len; i++ ) {
        if ( !isalnum( (char)r[i] ) )
            r[i] = '_';
    }

    return QString::fromAscii(r);
}


static ulong embedData( QTextStream& out, const uchar* input, int nbytes )
{
#ifndef QT_NO_IMAGE_COLLECTION_COMPRESSION
    QByteArray bazip( qCompress( input, nbytes ) );
    ulong len = bazip.size();
#else
    ulong len = nbytes;
#endif
    static const char hexdigits[] = "0123456789abcdef";
    QString s;
    for ( int i=0; i<(int)len; i++ ) {
        if ( (i%14) == 0 ) {
            s += QLatin1String("\n    ");
            out << s.latin1();
            s.truncate( 0 );
        }
        uint v = (uchar)
#ifndef QT_NO_IMAGE_COLLECTION_COMPRESSION
                 bazip
#else
                 input
#endif
                 [i];
        s += QLatin1String("0x");
        s += QLatin1Char(hexdigits[(v >> 4) & 15]);
        s += QLatin1Char(hexdigits[v & 15]);
        if ( i < (int)len-1 )
            s += QLatin1Char(',');
    }
    if ( s.length() )
        out << s.latin1();
    return len;
}

static void embedData( QTextStream& out, const QRgb* input, int n )
{
    out << hex;
    const QRgb *v = input;
    for ( int i=0; i<n; i++ ) {
        if ( (i%14) == 0  )
            out << "\n    ";
        out << "0x";
        out << hex << *v++;
        if ( i < n-1 )
            out << ',';
    }
    out << dec; // back to decimal mode
}

void Ui3Reader::embed(const char *project, const QStringList &images)
{

    QString cProject = convertToCIdentifier( project );

    QStringList::ConstIterator it;
    out << "/****************************************************************************\n";
    out << "** Image collection for project '" << project << "'.\n";
    out << "**\n";
    out << "** Generated from reading image files: \n";
    for ( it = images.begin(); it != images.end(); ++it )
        out << "**      " << *it << "\n";
    out << "**\n";
    out << "** Created: " << QDateTime::currentDateTime().toString() << "\n";
    out << "**      by: The User Interface Compiler for Qt version " << QT_VERSION_STR << "\n";
    out << "**\n";
    out << "** WARNING! All changes made in this file will be lost!\n";
    out << "****************************************************************************/\n";
    out << "\n";

    out << "#include <qimage.h>\n";
    out << "#include <qmime.h>\n";
    out << "#include <q3mimefactory.h>\n";
    out << "#include <q3dragobject.h>\n";
    out << "\n";

    QList<EmbedImage*> list_image;
    int image_count = 0;
    for ( it = images.begin(); it != images.end(); ++it ) {
        QImage img;
        if ( !img.load( *it ) ) {
            fprintf( stderr, "uic: cannot load image file %s\n", (*it).latin1() );
            continue;
        }
        EmbedImage *e = new EmbedImage;
        e->width = img.width();
        e->height = img.height();
        e->depth = img.depth();
        e->numColors = img.numColors();
        e->colorTable = new QRgb[e->numColors];
        e->alpha = img.hasAlphaBuffer();
        QVector<QRgb> ct = img.colorTable();
        memcpy(e->colorTable, ct.constData(), e->numColors*sizeof(QRgb));
        QFileInfo fi( *it );
        e->name = fi.fileName();
        e->cname = QString::fromLatin1("image_%1").arg( image_count++);
        list_image.append( e );
        out << "// " << *it << "\n";
        QString s;
        if ( e->depth == 1 )
            img = img.convertBitOrder(QImage::BigEndian);
        out << s.sprintf( "static const unsigned char %s_data[] = {",
                          e->cname.latin1() );
#ifndef QT_NO_IMAGE_COLLECTION_COMPRESSION
        e->compressed =
#endif
            embedData( out, img.bits(), img.numBytes() );
        out << "\n};\n\n";
        if ( e->numColors ) {
            out << s.sprintf( "static const QRgb %s_ctable[] = {",
                              e->cname.latin1() );
            embedData( out, e->colorTable, e->numColors );
            out << "\n};\n\n";
        }
    }

    if ( !list_image.isEmpty() ) {
        out << "static const struct EmbedImage {\n"
            "    int width, height, depth;\n"
            "    const unsigned char *data;\n"
#ifndef QT_NO_IMAGE_COLLECTION_COMPRESSION
            "    ulong compressed;\n"
#endif
            "    int numColors;\n"
            "    const QRgb *colorTable;\n"
            "    bool alpha;\n"
            "    const char *name;\n"
            "} embed_image_vec[] = {\n";
        EmbedImage *e = 0;
        int i;
        for (i = 0; i < list_image.count(); ++i) {
            e = list_image.at(i);
            out << "    { "
                << e->width << ", "
                << e->height << ", "
                << e->depth << ", "
                << "(const unsigned char*)" << e->cname << "_data, "
#ifndef QT_NO_IMAGE_COLLECTION_COMPRESSION
                << e->compressed << ", "
#endif
                << e->numColors << ", ";
            if ( e->numColors )
                out << e->cname << "_ctable, ";
            else
                out << "0, ";
            if ( e->alpha )
                out << "true, ";
            else
                out << "false, ";
            out << "\"" << e->name << "\" },\n";
            delete e;
        }
#ifndef QT_NO_IMAGE_COLLECTION_COMPRESSION
        out << "    { 0, 0, 0, 0, 0, 0, 0, 0, 0 }\n};\n";
#else
        out << "    { 0, 0, 0, 0, 0, 0, 0, 0 }\n};\n";
#endif

        out << "\n"
            "static QImage uic_findImage( const QString& name )\n"
            "{\n"
            "    for ( int i=0; embed_image_vec[i].data; i++ ) {\n"
            "        if ( QString::fromUtf8(embed_image_vec[i].name) == name ) {\n"
#ifndef QT_NO_IMAGE_COLLECTION_COMPRESSION
            "            QByteArray baunzip;\n"
            "            baunzip = qUncompress( embed_image_vec[i].data, \n"
            "                embed_image_vec[i].compressed );\n"
            "            QImage img((uchar*)baunzip.data(),\n"
            "                        embed_image_vec[i].width,\n"
            "                        embed_image_vec[i].height,\n"
            "                        embed_image_vec[i].depth,\n"
            "                        (QRgb*)embed_image_vec[i].colorTable,\n"
            "                        embed_image_vec[i].numColors,\n"
            "                        QImage::BigEndian\n"
            "                );\n"
            "            img = img.copy();\n"
#else
            "            QImage img((uchar*)embed_image_vec[i].data,\n"
            "                        embed_image_vec[i].width,\n"
            "                        embed_image_vec[i].height,\n"
            "                        embed_image_vec[i].depth,\n"
            "                        (QRgb*)embed_image_vec[i].colorTable,\n"
            "                        embed_image_vec[i].numColors,\n"
            "                        QImage::BigEndian\n"
            "                );\n"
#endif
            "            if ( embed_image_vec[i].alpha )\n"
            "                img.setAlphaBuffer(true);\n"
            "            return img;\n"
            "        }\n"
            "    }\n"
            "    return QImage();\n"
            "}\n\n";

        out << "class MimeSourceFactory_" << cProject << " : public Q3MimeSourceFactory\n";
        out << "{\n";
        out << "public:\n";
        out << "    MimeSourceFactory_" << cProject << "() {}\n";
        out << "    ~MimeSourceFactory_" << cProject << "() {}\n";
        out << "    const QMimeSource* data( const QString& abs_name ) const {\n";
        out << "\tconst QMimeSource* d = Q3MimeSourceFactory::data( abs_name );\n";
        out << "\tif ( d || abs_name.isNull() ) return d;\n";
        out << "\tQImage img = uic_findImage( abs_name );\n";
        out << "\tif ( !img.isNull() )\n";
        out << "\t    ((Q3MimeSourceFactory*)this)->setImage( abs_name, img );\n";
        out << "\treturn Q3MimeSourceFactory::data( abs_name );\n";
        out << "    };\n";
        out << "};\n\n";

        out << "static Q3MimeSourceFactory* factory = 0;\n";
        out << "\n";

        out << "void qInitImages_" << cProject << "()\n";
        out << "{\n";
        out << "    if ( !factory ) {\n";
        out << "\tfactory = new MimeSourceFactory_" << cProject << ";\n";
        out << "\tQ3MimeSourceFactory::defaultFactory()->addFactory( factory );\n";
        out << "    }\n";
        out << "}\n\n";

        out << "void qCleanupImages_" << cProject << "()\n";
        out << "{\n";
        out << "    if ( factory ) {\n";
        out << "\tQ3MimeSourceFactory::defaultFactory()->removeFactory( factory );\n";
        out << "\tdelete factory;\n";
        out << "\tfactory = 0;\n";
        out << "    }\n";
        out << "}\n\n";

        out << "class StaticInitImages_" << cProject << "\n";
        out << "{\n";
        out << "public:\n";
        out << "    StaticInitImages_" << cProject << "() { qInitImages_" << cProject << "(); }\n";
        out << "#if defined(Q_OS_SCO) || defined(Q_OS_UNIXWARE)\n";
        out << "    ~StaticInitImages_" << cProject << "() { }\n";
        out << "#else\n";
        out << "    ~StaticInitImages_" << cProject << "() { qCleanupImages_" << cProject << "(); }\n";
        out << "#endif\n";
        out << "};\n\n";

        out << "static StaticInitImages_" << cProject << " staticImages;\n";
    }
}

QT_END_NAMESPACE

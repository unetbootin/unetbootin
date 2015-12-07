/*******************************************************************
 *
 *  Copyright 2007  Trolltech ASA
 *
 *  This is part of HarfBuzz, an OpenType Layout engine library.
 *
 *  See the file name COPYING for licensing information.
 *
 ******************************************************************/

#if 0
#include <qtextcodec.h>
#include <qlibrary.h>


static void thaiWordBreaks(const QChar *string, const int len, QCharAttributes *attributes)
{
#ifdef QT_NO_TEXTCODEC
    Q_UNUSED(string);
    Q_UNUSED(len);
    Q_UNUSED(attributes);
#else
    typedef int (*th_brk_def)(const char*, int[], int);
    static QTextCodec *thaiCodec = QTextCodec::codecForMib(2259);
    static th_brk_def th_brk = 0;

#ifndef QT_NO_LIBRARY
    /* load libthai dynamically */
    if (!th_brk && thaiCodec) {
        th_brk = (th_brk_def)QLibrary::resolve(QLatin1String("thai"), "th_brk");
        if (!th_brk)
            thaiCodec = 0;
    }
#endif

    if (!th_brk)
        return;

    QByteArray cstr = thaiCodec->fromUnicode(QString(string, len));

    int brp[128];
    int *break_positions = brp;
    int numbreaks = th_brk(cstr.constData(), break_positions, 128);
    if (numbreaks > 128) {
        break_positions = new int[numbreaks];
        numbreaks = th_brk(cstr.data(),break_positions, numbreaks);
    }

    for (int i = 0; i < len - 1; ++i)
        attributes[i].lineBreakType = QCharAttributes::NoBreak;

    for (int i = 0; i < numbreaks; ++i) {
        if (break_positions[i] > 0)
            attributes[break_positions[i]-1].lineBreakType = QCharAttributes::Break;
    }

    if (break_positions != brp)
        delete [] break_positions;
#endif // QT_NO_TEXTCODEC
}


static void thai_attributes( int script, const QString &text, int from, int len, QCharAttributes *attributes )
{
    Q_UNUSED(script);
    Q_ASSERT(script == QUnicodeTables::Thai);
    thaiWordBreaks(text.unicode() + from, len, attributes);
}


#endif

/*******************************************************************
 *
 *  Copyright 2007  Trolltech ASA
 *
 *  This is part of HarfBuzz, an OpenType Layout engine library.
 *
 *  See the file name COPYING for licensing information.
 *
 ******************************************************************/
/*
    !!!!!! Warning !!!!!
    Please don't save this file in emacs. It contains utf8 text sequences emacs will
    silently convert to a series of question marks.
 */
#include <QtTest/QtTest>
#include <QtCore/qdebug.h>

#include <harfbuzz-shaper.h>

static QVector<HB_CharAttributes> getCharAttributes(const QString &str)
{
    QVector<HB_CharAttributes> attrs(str.length());
    HB_ScriptItem item;
    item.pos = 0;
    item.length = str.length();
    item.script = HB_Script_Common;
    HB_GetCharAttributes(str.utf16(), str.length(),
                         &item, 1,
                         attrs.data());
    return attrs;
}

class tst_CharAttributes : public QObject
{
    Q_OBJECT

public:
    tst_CharAttributes();
    virtual ~tst_CharAttributes();

public slots:
    void init();
    void cleanup();
private slots:
    void lineBreaking();
    void charWordStopOnLineSeparator();
    void charStopForSurrogatePairs();
};


tst_CharAttributes::tst_CharAttributes()
{
}

tst_CharAttributes::~tst_CharAttributes()
{
}

void tst_CharAttributes::init()
{
}

void tst_CharAttributes::cleanup()
{
}


void tst_CharAttributes::lineBreaking()
{
    struct Breaks {
	const char *utf8;
	uchar breaks[32];
    };
    Breaks brks[] = {
	{ "11", { false, 0xff } },
	{ "aa", { false, 0xff } },
	{ "++", { false, 0xff } },
	{ "--", { false, 0xff } },
	{ "((", { false, 0xff } },
	{ "))", { false, 0xff } },
	{ "..", { false, 0xff } },
	{ "\"\"", { false, 0xff } },
	{ "$$", { false, 0xff } },
	{ "!!", { false, 0xff } },
	{ "??", { false, 0xff } },
	{ ",,", { false, 0xff } },

	{ ")()", { true, false, 0xff } },
	{ "?!?", { false, false, 0xff } },
	{ ".,.", { false, false, 0xff } },
	{ "+-+", { false, false, 0xff } },
	{ "+=+", { false, false, 0xff } },
	{ "+(+", { false, false, 0xff } },
	{ "+)+", { false, false, 0xff } },

	{ "a b", { false, true, 0xff } },
	{ "a(b", { false, false, 0xff } },
	{ "a)b", { false, false, 0xff } },
	{ "a-b", { false, true, 0xff } },
	{ "a.b", { false, false, 0xff } },
	{ "a+b", { false, false, 0xff } },
	{ "a?b", { false, false, 0xff } },
	{ "a!b", { false, false, 0xff } },
	{ "a$b", { false, false, 0xff } },
	{ "a,b", { false, false, 0xff } },
	{ "a/b", { false, false, 0xff } },
	{ "1/2", { false, false, 0xff } },
	{ "./.", { false, false, 0xff } },
	{ ",/,", { false, false, 0xff } },
	{ "!/!", { false, false, 0xff } },
	{ "\\/\\", { false, false, 0xff } },
	{ "1 2", { false, true, 0xff } },
	{ "1(2", { false, false, 0xff } },
	{ "1)2", { false, false, 0xff } },
	{ "1-2", { false, false, 0xff } },
	{ "1.2", { false, false, 0xff } },
	{ "1+2", { false, false, 0xff } },
	{ "1?2", { false, true, 0xff } },
	{ "1!2", { false, true, 0xff } },
	{ "1$2", { false, false, 0xff } },
	{ "1,2", { false, false, 0xff } },
	{ "1/2", { false, false, 0xff } },
	{ "\330\260\331\216\331\204\331\220\331\203\331\216", { false, false, false, false, false, 0xff } },
	{ "\330\247\331\204\331\205 \330\247\331\204\331\205", { false, false, false, true, false, false, 0xff } },
	{ "1#2", { false, false, 0xff } },
	{ "!#!", { false, false, 0xff } },
	{ 0, {} }
    };
    Breaks *b = brks;
    while (b->utf8) {
        QString str = QString::fromUtf8(b->utf8);

        QVector<HB_CharAttributes> attrs = getCharAttributes(str);

        int i;
        for (i = 0; i < (int)str.length() - 1; ++i) {
            QVERIFY(b->breaks[i] != 0xff);
            if ( (attrs[i].lineBreakType != HB_NoBreak) != (bool)b->breaks[i] ) {
                qDebug("test case \"%s\" failed at char %d; break type: %d", b->utf8, i, attrs[i].lineBreakType);
                QCOMPARE( (attrs[i].lineBreakType != HB_NoBreak), (bool)b->breaks[i] );
            }
        }
        QVERIFY(attrs[i].lineBreakType == HB_ForcedBreak);
        QCOMPARE(b->breaks[i], (uchar)0xff);
        ++b;
    }
}

void tst_CharAttributes::charWordStopOnLineSeparator()
{
    const QChar lineSeparator(QChar::LineSeparator);
    QString txt;
    txt.append(lineSeparator);
    txt.append(lineSeparator);
    QVector<HB_CharAttributes> attrs = getCharAttributes(txt);
    QVERIFY(attrs[1].charStop);
}

void tst_CharAttributes::charStopForSurrogatePairs()
{
    QString txt;
    txt.append("a");
    txt.append(0xd87e);
    txt.append(0xdc25);
    txt.append("b");
    QVector<HB_CharAttributes> attrs = getCharAttributes(txt);
    QVERIFY(attrs[0].charStop);
    QVERIFY(attrs[1].charStop);
    QVERIFY(!attrs[2].charStop);
    QVERIFY(attrs[3].charStop);
}

QTEST_MAIN(tst_CharAttributes)
#include "main.moc"

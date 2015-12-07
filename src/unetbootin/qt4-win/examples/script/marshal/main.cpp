/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
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

#include <QtCore>
#include <QtScript>

template <class Container>
QScriptValue toScriptValue(QScriptEngine *eng, const Container &cont)
{
    QScriptValue a = eng->newArray();
    typename Container::const_iterator begin = cont.begin();
    typename Container::const_iterator end = cont.end();
    typename Container::const_iterator it;
    for (it = begin; it != end; ++it)
        a.setProperty(quint32(it - begin), qScriptValueFromValue(eng, *it));
    return a;
}

template <class Container>
void fromScriptValue(const QScriptValue &value, Container &cont)
{
    quint32 len = value.property("length").toUInt32();
    for (quint32 i = 0; i < len; ++i) {
        QScriptValue item = value.property(i);
        typedef typename Container::value_type ContainerValue;
        cont.push_back(qscriptvalue_cast<ContainerValue>(item));
    }
}

typedef QVector<int> IntVector;
typedef QVector<QString> StringVector;

Q_DECLARE_METATYPE(IntVector)
Q_DECLARE_METATYPE(StringVector)

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QScriptEngine eng;
    // register our custom types
    qScriptRegisterMetaType<IntVector>(&eng, toScriptValue, fromScriptValue);
    qScriptRegisterMetaType<StringVector>(&eng, toScriptValue, fromScriptValue);

    QScriptValue val = eng.evaluate("[1, 4, 7, 11, 50, 3, 19, 60]");

    fprintf(stdout, "Script array: %s\n", qPrintable(val.toString()));

    IntVector iv = qscriptvalue_cast<IntVector>(val);

    fprintf(stdout, "qscriptvalue_cast to QVector<int>: ");
    for (int i = 0; i < iv.size(); ++i)
        fprintf(stdout, "%s%d", (i > 0) ? "," : "", iv.at(i));
    fprintf(stdout, "\n");

    val = eng.evaluate("[9, 'foo', 46.5, 'bar', 'Qt', 555, 'hello']");

    fprintf(stdout, "Script array: %s\n", qPrintable(val.toString()));

    StringVector sv = qscriptvalue_cast<StringVector>(val);

    fprintf(stdout, "qscriptvalue_cast to QVector<QString>: ");
    for (int i = 0; i < sv.size(); ++i)
        fprintf(stdout, "%s%s", (i > 0) ? "," : "", qPrintable(sv.at(i)));
    fprintf(stdout, "\n");

    return 0;
}

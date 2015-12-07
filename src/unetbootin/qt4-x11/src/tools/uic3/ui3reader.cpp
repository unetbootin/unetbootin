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
#include "parser.h"
#include "domtool.h"
#include "ui4.h"
#include "widgetinfo.h"
#include "globaldefs.h"
#include "qt3to4.h"

#include <QFile>
#include <QDateTime>
#include <QRegExp>
#include <QXmlStreamWriter>
#include <QtDebug>
#include <stdio.h>
#include <stdlib.h>

QT_BEGIN_NAMESPACE

bool Ui3Reader::isMainWindow = false;

static QString lineColDebug(int line, int col)
{
    if (line >= 0) {
        const QString ret = QString::fromLatin1("Line: %1%2");
        return ret.arg(line).arg(col >= 0 ? QString::fromLatin1(" Column: %1").arg(col) : QString());
    }
    return QString();
}

void Ui3Reader::errorInvalidProperty(const QString &propertyName, const QString &widgetName, const QString &widgetClass, int line, int col)
{
    fprintf(stderr, "uic3: property `%s' for widget `%s' of type `%s' is not supported. %s\n",
            propertyName.toLatin1().constData(),
            widgetName.toLatin1().constData(),
            widgetClass.toLatin1().constData(),
            lineColDebug(line, col).toLocal8Bit().constData());
}

void Ui3Reader::errorInvalidSignal(const QString &signal, const QString &widgetName, const QString &widgetClass, int line, int col)
{
    fprintf(stderr, "uic3: signal `%s' for widget `%s' of type `%s' is not supported; connection may fail. %s\n",
            signal.toLatin1().constData(), widgetName.toLatin1().constData(),
            widgetClass.toLatin1().constData(),
            lineColDebug(line, col).toLocal8Bit().constData());
}

void Ui3Reader::errorInvalidSlot(const QString &slot, const QString &widgetName, const QString &widgetClass, int line, int col)
{
    fprintf(stderr, "uic3: slot `%s' for widget `%s' of type `%s' is not supported; connection may fail. %s\n",
            slot.toLatin1().constData(),
            widgetName.toLatin1().constData(),
            widgetClass.toLatin1().constData(),
            lineColDebug(line, col).toLocal8Bit().constData());
}

QString Ui3Reader::getComment(const QDomNode& n)
{
    QDomNode child = n.firstChild();
    while (!child.isNull()) {
        if (child.toElement().tagName() == QLatin1String("comment"))
            return child.toElement().firstChild().toText().data();
        child = child.nextSibling();
    }
    return QString();
}

QString Ui3Reader::mkBool(bool b)
{
    return b ? QLatin1String("true") : QLatin1String("false");
}

QString Ui3Reader::mkBool(const QString& s)
{
    return mkBool(s == QLatin1String("true") || s == QLatin1String("1"));
}

bool Ui3Reader::toBool(const QString& s)
{
    return s == QLatin1String("true") || s.toInt() != 0;
}

QString Ui3Reader::fixString(const QString &str, bool encode)
{
    QString s;
    if (!encode) {
        s = str;
        s.replace(QLatin1String("\\"), QLatin1String("\\\\"));
        s.replace(QLatin1String("\""), QLatin1String("\\\""));
        s.replace(QLatin1String("\r"), QLatin1String(""));
        s.replace(QLatin1String("\n"), QLatin1String("\\n\"\n\""));
    } else {
        QByteArray utf8 = str.utf8();
        const int l = utf8.length();
        for (int i = 0; i < l; ++i)
            s += QLatin1String("\\x") + QString::number((uchar)utf8[i], 16);
    }

    return QLatin1String("\"") + s + QLatin1String("\"");
}

QString Ui3Reader::trcall(const QString& sourceText, const QString& comment)
{
    if (sourceText.isEmpty() && comment.isEmpty())
        return QLatin1String("QString()");

    QString t = trmacro;
    bool encode = false;
    if (t.isNull()) {
        t = QLatin1String("tr");
        for (int i = 0; i < (int) sourceText.length(); i++) {
            if (sourceText[i].unicode() >= 0x80) {
                t = QLatin1String("trUtf8");
                encode = true;
                break;
            }
        }
    }

    if (comment.isEmpty()) {
        return t + QLatin1String("(") + fixString(sourceText, encode) + QLatin1String(")");
    } else {
        return t + QLatin1String("(")
            + fixString(sourceText, encode)
            + QLatin1String(", ")
            + fixString(comment, encode) + QLatin1String(")");
    }
}

QString Ui3Reader::mkStdSet(const QString& prop)
{
    return QLatin1String("set") + prop[0].toUpper() + prop.mid(1);
}

void Ui3Reader::init()
{
    outputFileName.clear();
    trmacro.clear();
    nofwd = false;

    fileName.clear();
    writeFunctImpl = true;
    defMargin = BOXLAYOUT_DEFAULT_MARGIN;
    defSpacing = BOXLAYOUT_DEFAULT_SPACING;
    externPixmaps = false;
    indent = QLatin1String("    "); // default indent

    item_used = cg_used = pal_used = 0;

    layouts.clear();
    layouts << QLatin1String("hbox") << QLatin1String("vbox") << QLatin1String("grid");
    tags = layouts;
    tags << QLatin1String("widget");

    nameOfClass.clear();
    namespaces.clear();
    bareNameOfClass.clear();
}

QDomElement Ui3Reader::parse(const QDomDocument &doc)
{
    root = doc.firstChild().toElement();
    widget = QDomElement();

    pixmapLoaderFunction = getPixmapLoaderFunction(doc.firstChild().toElement());
    nameOfClass = getFormClassName(doc.firstChild().toElement());

    uiFileVersion = doc.firstChild().toElement().attribute(QLatin1String("version"));
    stdsetdef = toBool(doc.firstChild().toElement().attribute(QLatin1String("stdsetdef")));

    if (doc.firstChild().isNull() || doc.firstChild().firstChild().isNull())
        return widget;

    QDomElement e = doc.firstChild().firstChild().toElement();
    while (!e.isNull()) {
        if (e.tagName() == QLatin1String("widget")) {
            widget = e;
        } else if (e.tagName() == QLatin1String("pixmapinproject")) {
            externPixmaps = true;
        } else if (e.tagName() == QLatin1String("layoutdefaults")) {
            defSpacing = e.attribute(QLatin1String("spacing"), defSpacing.toString());
            defMargin = e.attribute(QLatin1String("margin"), defMargin.toString());
        } else if (e.tagName() == QLatin1String("layoutfunctions")) {
            defSpacing = e.attribute(QLatin1String("spacing"), defSpacing.toString());
            bool ok;
            defSpacing.toInt(&ok);
            if (!ok) {
                QString buf = defSpacing.toString();
                defSpacing = buf.append(QLatin1String("()"));
            }
            defMargin = e.attribute(QLatin1String("margin"), defMargin.toString());
            defMargin.toInt(&ok);
            if (!ok) {
                QString buf = defMargin.toString();
                defMargin = buf.append(QLatin1String("()"));
            }
        }
        e = e.nextSibling().toElement();
    }

    return widget;
}

Ui3Reader::Ui3Reader(QTextStream &outStream)
   : out(outStream), trout(&languageChangeBody)
{
    m_porting = new Porting();
    m_extractImages = false;
}

Ui3Reader::~Ui3Reader()
{
    delete m_porting;
}

void Ui3Reader::generate(const QString &fn, const QString &outputFn,
          QDomDocument doc, bool decl, bool subcl, const QString &trm,
          const QString& subClass, bool omitForwardDecls, bool implicitIncludes, const QString &convertedUiFile)
{
    init();

    fileName = fn;
    outputFileName = outputFn;
    trmacro = trm;
    nofwd = omitForwardDecls;

    QDomElement e = parse(doc);

    if (nameOfClass.isEmpty())
        nameOfClass = getObjectName(e);
    namespaces = nameOfClass.split(QLatin1String("::"));
    bareNameOfClass = namespaces.last();
    namespaces.removeLast();

    if (!convertedUiFile.isEmpty()) {
        createWrapperDecl(e, convertedUiFile);
    } else if (subcl) {
        if (decl)
            createSubDecl(e, subClass);
        else
            createSubImpl(e, subClass);
    } else {
        if (decl)
            createFormDecl(e, implicitIncludes);
        else
            createFormImpl(e);
    }

}

void Ui3Reader::generateUi4(const QString &fn, const QString &outputFn, QDomDocument doc, bool implicitIncludes)
{
    init();

    fileName = fn;
    outputFileName = outputFn;

    DomUI *ui = generateUi4(parse(doc), implicitIncludes);
    if (!ui)
        return;

    if (pixmapLoaderFunction.size())
        ui->setElementPixmapFunction(pixmapLoaderFunction);

    QXmlStreamWriter writer(out.device());
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(2);
    writer.writeStartDocument();
    ui->write(writer);
    writer.writeEndDocument();

    delete ui;
}

void Ui3Reader::setTrMacro(const QString &trmacro)
{
    this->trmacro = trmacro;
}

void Ui3Reader::setForwardDeclarationsEnabled(bool b)
{
    nofwd = !b;
}

void Ui3Reader::setOutputFileName(const QString &fileName)
{
    outputFileName = fileName;
}

/*! Extracts a pixmap loader function from \a e
 */
QString Ui3Reader::getPixmapLoaderFunction(const QDomElement& e)
{
    QDomElement n;
    for (n = e.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement()) {
        if (n.tagName() == QLatin1String("pixmapfunction"))
            return n.firstChild().toText().data();
    }
    return QString();
}


/*! Extracts the forms class name from \a e
 */
QString Ui3Reader::getFormClassName(const QDomElement& e)
{
    QDomElement n;
    QString cn;
    for (n = e.firstChild().toElement(); !n.isNull(); n = n.nextSibling().toElement()) {
        if (n.tagName() == QLatin1String("class")) {
            QString s = n.firstChild().toText().data();
            int i;
            while ((i = s.indexOf(QLatin1Char(' '))) != -1)
                s[i] = QLatin1Char('_');
            cn = s;
        }
    }
    return cn;
}

/*! Extracts a class name from \a e.
 */
QString Ui3Reader::getClassName(const QDomElement& e)
{
    QString s = e.attribute(QLatin1String("class"));
    if (s.isEmpty() && e.tagName() == QLatin1String("toolbar"))
        s = QLatin1String(QLatin1String("QToolBar"));
    else if (s.isEmpty() && e.tagName() == QLatin1String("menubar"))
        s = QLatin1String("QMenuBar");

    return fixClassName(s);
}

/*! Returns true if database framework code is generated, else false.
*/

bool Ui3Reader::isFrameworkCodeGenerated(const QDomElement& e)
{
    QDomElement n = getObjectProperty(e, QLatin1String("frameworkCode"));
    if (n.attribute(QLatin1String("name")) == QLatin1String("frameworkCode") &&
         !DomTool::elementToVariant(n.firstChild().toElement(), QVariant(true)).toBool())
        return false;
    return true;
}

/*! Extracts an object name from \a e. It's stored in the 'name'
 property.
 */
QString Ui3Reader::getObjectName(const QDomElement& e)
{
    QDomElement n = getObjectProperty(e, QLatin1String("name"));
    if (n.firstChild().toElement().tagName() == QLatin1String("cstring"))
        return n.firstChild().toElement().firstChild().toText().data();
    return QString();
}

/*! Extracts an layout name from \a e. It's stored in the 'name'
 property of the preceding sibling (the first child of a QLayoutWidget).
 */
QString Ui3Reader::getLayoutName(const QDomElement& e)
{
    QDomElement p = e.parentNode().toElement();
    QString name;

    if (getClassName(p) != QLatin1String("QLayoutWidget"))
        name = QLatin1String("Layout");

    QDomElement n = getObjectProperty(p, QLatin1String("name"));
    if (n.firstChild().toElement().tagName() == QLatin1String("cstring")) {
        name.prepend(n.firstChild().toElement().firstChild().toText().data());
        return name.split(QLatin1String("::")).last();
    }
    return e.tagName();
}


QString Ui3Reader::getDatabaseInfo(const QDomElement& e, const QString& tag)
{
    QDomElement n;
    QDomElement n1;
    int child = 0;
    // database info is a stringlist stored in this order
    if (tag == QLatin1String("connection"))
        child = 0;
    else if (tag == QLatin1String("table"))
        child = 1;
    else if (tag == QLatin1String("field"))
        child = 2;
    else
        return QString();
    n = getObjectProperty(e, QLatin1String("database"));
    if (n.firstChild().toElement().tagName() == QLatin1String("stringlist")) {
            // find correct stringlist entry
            QDomElement n1 = n.firstChild().firstChild().toElement();
            for (int i = 0; i < child && !n1.isNull(); ++i)
                n1 = n1.nextSibling().toElement();
            if (n1.isNull())
                return QString();
            return n1.firstChild().toText().data();
    }
    return QString();
}

static const char* const ColorRole[] = {
    "Foreground", "Button", "Light", "Midlight", "Dark", "Mid",
    "Text", "BrightText", "ButtonText", "Base", "Background", "Shadow",
    "Highlight", "HighlightedText", "Link", "LinkVisited", 0
};


/*!
  Creates a colorgroup with name \a name from the color group \a cg
 */
void Ui3Reader::createColorGroupImpl(const QString& name, const QDomElement& e)
{
    int r = -1;
    QDomElement n = e.firstChild().toElement();
    QString color;

    Color white;
    white.init(255, 255, 255);

    Color black;
    black.init(0, 0, 0);

    while (!n.isNull()) {
        if (n.tagName() == QLatin1String("color")) {
            r++;
            Color col = DomTool::readColor(n);
            color = QLatin1String("QColor(%1, %2, %3)");
            color = color.arg(col.red).arg(col.green).arg(col.blue);
            if (col == white)
                color = QLatin1String("white");
            else if (col == black)
                color = QLatin1String("black");
            if (n.nextSibling().toElement().tagName() != QLatin1String("pixmap")) {
                out << indent << name << ".setColor(QColorGroup::" << ColorRole[r] << ", " << color << ");" << endl;
            }
        } else if (n.tagName() == QLatin1String("pixmap")) {
            QString pixmap = n.firstChild().toText().data();
            if (!pixmapLoaderFunction.isEmpty()) {
                pixmap.prepend(pixmapLoaderFunction
                    + QLatin1String("(")
                    + QLatin1String(externPixmaps ? "\"" : ""));

                pixmap.append(QLatin1String(externPixmaps ? "\"" : "") + QLatin1String(")"));
            }
            out << indent << name << ".setBrush(QColorGroup::"
                << ColorRole[r] << ", QBrush(" << color << ", " << pixmap << "));" << endl;
        }
        n = n.nextSibling().toElement();
    }
}

/*!
  Auxiliary function to load a color group. The colorgroup must not
  contain pixmaps.
 */
ColorGroup Ui3Reader::loadColorGroup(const QDomElement &e)
{
    ColorGroup cg;
    int r = -1;
    QDomElement n = e.firstChild().toElement();
    Color col;
    while (!n.isNull()) {
        if (n.tagName() == QLatin1String("color")) {
            r++;
            col = DomTool::readColor(n);
            cg.append(qMakePair(r, col));
        }
        n = n.nextSibling().toElement();
    }
    return cg;
}

/*!  Returns true if the widget properties specify that it belongs to
  the database \a connection and \a table.
*/

bool Ui3Reader::isWidgetInTable(const QDomElement& e, const QString& connection, const QString& table)
{
    QString conn = getDatabaseInfo(e, QLatin1String("connection"));
    QString tab = getDatabaseInfo(e, QLatin1String("table"));
    if (conn == connection && tab == table)
        return true;
    return false;
}

/*!
  Registers all database connections, cursors and forms.
*/

void Ui3Reader::registerDatabases(const QDomElement& e)
{
    QDomElement n;
    QDomNodeList nl;
    int i;
    nl = e.parentNode().toElement().elementsByTagName(QLatin1String("widget"));
    for (i = 0; i < (int) nl.length(); ++i) {
        n = nl.item(i).toElement();
        QString conn = getDatabaseInfo(n, QLatin1String("connection"));
        QString tab = getDatabaseInfo(n, QLatin1String("table"));
        QString fld = getDatabaseInfo(n, QLatin1String("field"));
        if (!conn.isNull()) {
            dbConnections += conn;
            if (!tab.isNull()) {
                dbCursors[conn] += tab;
                if (!fld.isNull())
                    dbForms[conn] += tab;
            }
        }
    }
}

/*!
  Registers an object with name \a name.

  The returned name is a valid variable identifier, as similar to \a
  name as possible and guaranteed to be unique within the form.

  \sa registeredName(), isObjectRegistered()
 */
QString Ui3Reader::registerObject(const QString& name)
{
    if (objectNames.isEmpty()) {
        // some temporary variables we need
        objectNames += QLatin1String("img");
        objectNames += QLatin1String("item");
        objectNames += QLatin1String("cg");
        objectNames += QLatin1String("pal");
    }

    QString result = name;
    int i;
    while ((i = result.indexOf(QLatin1Char(' '))) != -1 ) {
        result[i] = QLatin1Char('_');
    }

    if (objectNames.contains(result)) {
        int i = 2;
        while (objectNames.contains(result + QLatin1String("_") + QString::number(i)))
            i++;
        result += QLatin1String("_");
        result += QString::number(i);
    }
    objectNames += result;
    objectMapper.insert(name, result);
    return result;
}

/*!
  Returns the registered name for the original name \a name
  or \a name if \a name  wasn't registered.

  \sa registerObject(), isObjectRegistered()
 */
QString Ui3Reader::registeredName(const QString& name)
{
    if (!objectMapper.contains(name))
        return name;
    return objectMapper[name];
}

/*!
  Returns whether the object \a name was registered yet or not.
 */
bool Ui3Reader::isObjectRegistered(const QString& name)
{
    return objectMapper.contains(name);
}

/*!
  Unifies the entries in stringlist \a list. Should really be a QStringList feature.
 */
QStringList Ui3Reader::unique(const QStringList& list)
{
    if (list.isEmpty())
        return list;

    QStringList result;
    for (QStringList::ConstIterator it = list.begin(); it != list.end(); ++it) {
        if (!result.contains(*it))
            result += *it;
    }
    return result;
}

bool Ui3Reader::isLayout(const QString& name) const
{
    return layoutObjects.contains(name);
}

void Ui3Reader::setExtractImages(bool extract, const QString &qrcOutputFile)
{
    m_extractImages = extract;
    m_qrcOutputFile = qrcOutputFile;
}

QT_END_NAMESPACE

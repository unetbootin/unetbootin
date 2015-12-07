/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the qmake application of the Qt Toolkit.
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

#ifndef XMLOUTPUT_H
#define XMLOUTPUT_H

#include <qtextstream.h>
#include <qstack.h>

QT_BEGIN_NAMESPACE

class XmlOutput
{
public:
    enum ConverstionType {
        NoConversion,       // No change
        EscapeConversion,   // Use '\"'
        XMLConversion       // Use &quot;
    };
    enum XMLFormat {
        NoNewLine,          // No new lines, unless added manually
        NewLine             // All properties & tags indented on new lines
    };
    enum XMLState {
        Bare,               // Not in tag or attribute
        Tag,                // <tagname attribute1="value"
        Attribute           //  attribute2="value">
    };
    enum XMLType {
        tNothing,           // No XML output, and not state change
        tRaw,               // Raw text (no formating)
        tDeclaration,       // <?xml version="x.x" encoding="xxx"?>
        tTag,               // <tagname attribute1="value"
        tCloseTag,          // Closes an open tag
        tAttribute,         //  attribute2="value">
        tData,              // Tag data (formating done)
        tComment,           // <!-- Comment -->
        tCDATA              // <![CDATA[ ... ]]>
    };

    XmlOutput(QTextStream &file, ConverstionType type = XMLConversion);
    ~XmlOutput();

    // Settings
    void setIndentString(const QString &indentString);
    QString indentString();
    void setIndentLevel(int level);
    int indentLevel();
    void setState(XMLState state);
    XMLState state();


    struct xml_output {
        XMLType xo_type;    // Type of struct instance
        QString xo_text;    // Tag/Attribute name/xml version
        QString xo_value;   // Value of attributes/xml encoding

        xml_output(XMLType type, const QString &text, const QString &value)
            : xo_type(type), xo_text(text), xo_value(value) {}
        xml_output(const xml_output &xo)
            : xo_type(xo.xo_type), xo_text(xo.xo_text), xo_value(xo.xo_value) {}
    };

    // Streams
    XmlOutput& operator<<(const QString& o);
    XmlOutput& operator<<(const xml_output& o);

private:
    void increaseIndent();
    void decreaseIndent();
    void updateIndent();

    QString doConversion(const QString &text);

    // Output functions
    void newTag(const QString &tag);
    void newTagOpen(const QString &tag);
    void closeOpen();
    void closeTag();
    void closeTo(const QString &tag);
    void closeAll();

    void addDeclaration(const QString &version, const QString &encoding);
    void addRaw(const QString &rawText);
    void addAttribute(const QString &attribute, const QString &value);
    void addData(const QString &data);

    // Data
    QTextStream &xmlFile;
    QString indent;

    QString currentIndent;
    int currentLevel;
    XMLState currentState;

    XMLFormat format;
    ConverstionType conversion;
    QStack<QString> tagStack;
};

inline XmlOutput::xml_output noxml()
{
    return XmlOutput::xml_output(XmlOutput::tNothing, QString(), QString());
}

inline XmlOutput::xml_output raw(const QString &rawText)
{
    return XmlOutput::xml_output(XmlOutput::tRaw, rawText, QString());
}

inline XmlOutput::xml_output declaration(const QString &version = QString("1.0"),
                                         const QString &encoding = QString())
{
    return XmlOutput::xml_output(XmlOutput::tDeclaration, version, encoding);
}

inline XmlOutput::xml_output decl(const QString &version = QString("1.0"),
                                  const QString &encoding = QString())
{
    return declaration(version, encoding);
}

inline XmlOutput::xml_output tag(const QString &name)
{
    return XmlOutput::xml_output(XmlOutput::tTag, name, QString());
}

inline XmlOutput::xml_output closetag()
{
    return XmlOutput::xml_output(XmlOutput::tCloseTag, QString(), QString());
}

inline XmlOutput::xml_output closetag(const QString &toTag)
{
    return XmlOutput::xml_output(XmlOutput::tCloseTag, toTag, QString());
}

inline XmlOutput::xml_output closeall()
{
    return XmlOutput::xml_output(XmlOutput::tCloseTag, QString(), QString("all"));
}

inline XmlOutput::xml_output attribute(const QString &name,
                                       const QString &value)
{
    return XmlOutput::xml_output(XmlOutput::tAttribute, name, value);
}

inline XmlOutput::xml_output attr(const QString &name,
                                  const QString &value)
{
    return attribute(name, value);
}

inline XmlOutput::xml_output data(const QString &text = QString())
{
    return XmlOutput::xml_output(XmlOutput::tData, text, QString());
}

inline XmlOutput::xml_output comment(const QString &text)
{
    return XmlOutput::xml_output(XmlOutput::tComment, text, QString());
}

inline XmlOutput::xml_output cdata(const QString &text)
{
    return XmlOutput::xml_output(XmlOutput::tCDATA, text, QString());
}

QT_END_NAMESPACE

#endif // XMLOUTPUT_H

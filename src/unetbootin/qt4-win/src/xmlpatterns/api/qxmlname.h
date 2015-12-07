/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtXMLPatterns module of the Qt Toolkit.
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

#ifndef QXMLNAME_H
#define QXMLNAME_H

#include <QtCore/QString>
#include <QtCore/QMetaType>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(XmlPatterns)

class QXmlName;
class QXmlNamePool;
Q_XMLPATTERNS_EXPORT uint qHash(const QXmlName &name);

class Q_XMLPATTERNS_EXPORT QXmlName
{
private:
    enum Constant
    {
        LocalNameOffset     = 0,
        LocalNameLength     = 11,
        NamespaceOffset     = 11,
        NamespaceLength     = 9,
        PrefixLength        = 10,
        InvalidCode         = 1 << 31,
        NamespaceMask       = ((1 << ((NamespaceOffset + NamespaceLength) - NamespaceOffset)) - 1) << NamespaceOffset,
        LocalNameMask       = ((1 << ((LocalNameOffset + LocalNameLength) - LocalNameOffset)) - 1) << LocalNameOffset,
        PrefixOffset        = 21,
        PrefixMask          = ((1 << ((PrefixOffset + PrefixLength) - PrefixOffset)) - 1) << PrefixOffset,
        MaximumPrefixes     = (PrefixMask >> PrefixOffset) - 1,
        MaximumLocalNames   = (LocalNameMask >> LocalNameOffset) - 1,
        MaximumNamespaces   = (NamespaceMask >> NamespaceOffset) - 1,
        ExpandedNameMask    = LocalNameMask | NamespaceMask,
        LexicalQNameMask    = LocalNameMask | PrefixMask
    };

public:

    typedef qint16 NamespaceCode;
    typedef NamespaceCode PrefixCode;
    typedef NamespaceCode LocalNameCode;

    QXmlName();

    QXmlName(QXmlNamePool &namePool,
             const QString &localName,
             const QString &namespaceURI = QString(),
             const QString &prefix = QString());

    QString namespaceUri(const QXmlNamePool &query) const;
    QString prefix(const QXmlNamePool &query) const;
    QString localName(const QXmlNamePool &query) const;
    QString toClarkName(const QXmlNamePool &query) const;
    bool operator==(const QXmlName &other) const;
    bool operator!=(const QXmlName &other) const;
    QXmlName &operator=(const QXmlName &other);
    bool isNull() const;
    static bool isNCName(const QString &candidate);

    /* The members below are internal, not part of the public API, and
     * unsupported. Using them leads to undefined behavior. */
    typedef qint64 Code;

    inline QXmlName(const NamespaceCode uri,
                    const LocalNameCode ln,
                    const PrefixCode p = 0);
    /* The implementation for these functions are in utils/qnamepool_p.h. */
    inline LocalNameCode localName() const;
    inline PrefixCode prefix() const;
    inline bool hasPrefix() const;
    inline bool hasNamespace() const;
    inline NamespaceCode namespaceURI() const;
    inline bool isLexicallyEqual(const QXmlName &other) const;
    inline void setPrefix(const PrefixCode c);
    inline void setNamespaceURI(const NamespaceCode c);
    inline void setLocalName(const LocalNameCode c);
    inline Code code() const;

    friend Q_XMLPATTERNS_EXPORT uint qHash(const QXmlName &);

private:
    inline QXmlName(const int c) : m_qNameCode(c)
    {
    }

    Code m_qNameCode;
};

Q_DECLARE_TYPEINFO(QXmlName, Q_MOVABLE_TYPE);

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QXmlName); /* This macro must appear after QT_END_NAMESPACE. */

QT_END_HEADER

#endif

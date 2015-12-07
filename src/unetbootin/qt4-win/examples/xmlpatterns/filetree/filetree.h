/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
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

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QVector>
#include <QtXmlPatterns/QSimpleXmlNodeModel>

class FileTree : public QSimpleXmlNodeModel
{
public:
    FileTree(const QXmlNamePool &namePool);

    QXmlNodeModelIndex nodeFor(const QString &fileName) const;

    /* Reimplement pure functions in QAbstractXmlNodeModel. */
    virtual QXmlNodeModelIndex::DocumentOrder compareOrder(const QXmlNodeModelIndex&,
                                                           const QXmlNodeModelIndex&) const;
    virtual QXmlName name(const QXmlNodeModelIndex &node) const;
    virtual QUrl documentUri(const QXmlNodeModelIndex &node) const;
    virtual QXmlNodeModelIndex::NodeKind kind(const QXmlNodeModelIndex &node) const;
    virtual QXmlNodeModelIndex root(const QXmlNodeModelIndex &node) const;
    virtual QVariant typedValue(const QXmlNodeModelIndex &node) const;
    virtual QVector<QXmlNodeModelIndex> attributes(const QXmlNodeModelIndex &element) const;
protected:
    virtual QXmlNodeModelIndex nextFromSimpleAxis(SimpleAxis, const QXmlNodeModelIndex&) const;

private:
    enum Type {
        File,
        Directory,
        AttributeFileName,
        AttributeFilePath,
        AttributeSize,
        AttributeMIMEType,
        AttributeSuffix
    };

    inline QXmlNodeModelIndex nextSibling(const QXmlNodeModelIndex &nodeIndex,
                                          const QFileInfo &from,
                                          qint8 offset) const;
    inline const QFileInfo &toFileInfo(const QXmlNodeModelIndex &index) const;
    inline QXmlNodeModelIndex toNodeIndex(const QFileInfo &index,
                                          Type attributeName) const;
    /**
     * Same as above, but derives the type from the QFileInfo.
     */
    inline QXmlNodeModelIndex toNodeIndex(const QFileInfo &index) const;

    /*
     One possible improvement is to use a hash, and use the &*&value()
     trick to get a pointer, which would be stored in data() instead
     of the index.
    */
    mutable QVector<QFileInfo>  m_fileInfos;
    QVector<QXmlName>           m_names;
    const QDir::Filters         m_filterAllowAll;
    const QDir::SortFlags       m_sortFlags;
};


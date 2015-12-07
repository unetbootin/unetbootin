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

#include <QtCore/QUrl>
#include <QtCore/QVariant>
#include <QtXmlPatterns/QXmlNamePool>
#include "filetree.h"

/*
The model has two types of nodes: elements & attributes.

    <directory name="">
        <file name="">
        </file>
    </directory>

  In QXmlNodeModelIndex we store two values. QXmlNodeIndex::data()
  is treated as a signed int, and it is an index into m_fileInfos
  unless it is -1, in which case it has no meaning and the value
  of QXmlNodeModelIndex::additionalData() is a Type name instead.
 */

/*!
  The constructor passes \a pool to the base class, then loads an
  internal vector with an instance of QXmlName for each of the
  strings "file", "directory", "fileName", "filePath", "size",
  "mimeType", and "suffix".
 */
FileTree::FileTree(const QXmlNamePool& pool) : QSimpleXmlNodeModel(pool)
                                             , m_filterAllowAll(QDir::AllEntries
                                                                | QDir::AllDirs
                                                                | QDir::NoDotAndDotDot
                                                                | QDir::Hidden)
                                             , m_sortFlags(QDir::Name)
{
    QXmlNamePool np = namePool();
    m_names.resize(7);
    m_names[File]               = QXmlName(np, QLatin1String("file"));
    m_names[Directory]          = QXmlName(np, QLatin1String("directory"));
    m_names[AttributeFileName]  = QXmlName(np, QLatin1String("fileName"));
    m_names[AttributeFilePath]  = QXmlName(np, QLatin1String("filePath"));
    m_names[AttributeSize]      = QXmlName(np, QLatin1String("size"));
    m_names[AttributeMIMEType]  = QXmlName(np, QLatin1String("mimeType"));
    m_names[AttributeSuffix]    = QXmlName(np, QLatin1String("suffix"));
}

/*!
 */
QXmlNodeModelIndex FileTree::nodeFor(const QString& dirName) const
{
    /* We call QDir::cleanPath() because a QFileInfo constructed on a
     * path ending with a slash, will return the empty string in fileName(),
     * instead of the directory name. */
    QFileInfo dirInfo(QDir::cleanPath(dirName));
    Q_ASSERT(dirInfo.exists());
    return toNodeIndex(dirInfo);
}

/*!
  Since the value will always be in m_fileInfos, it is safe for
  us to return a const reference to it.
 */
const QFileInfo&
FileTree::toFileInfo(const QXmlNodeModelIndex &nodeIndex) const
{
    return m_fileInfos.at(nodeIndex.data());
}

/*!
 */
QXmlNodeModelIndex
FileTree::toNodeIndex(const QFileInfo &fileInfo, Type attributeName) const
{
    const int indexOf = m_fileInfos.indexOf(fileInfo);

    if (indexOf == -1) {
        m_fileInfos.append(fileInfo);
        return createIndex(m_fileInfos.count()-1, attributeName);
    }
    else
        return createIndex(indexOf, attributeName);
}

/*!
 */
QXmlNodeModelIndex FileTree::toNodeIndex(const QFileInfo &fileInfo) const
{
    return toNodeIndex(fileInfo, fileInfo.isDir() ? Directory : File);
}

/*!
 */
QXmlNodeModelIndex FileTree::nextSibling(const QXmlNodeModelIndex &nodeIndex,
                                         const QFileInfo &fileInfo,
                                         qint8 offset) const
{
    Q_ASSERT(offset == -1 || offset == 1);

    /* First, get our parent. */
    const QXmlNodeModelIndex parent(nextFromSimpleAxis(Parent, nodeIndex));

    if (parent.isNull())
        return QXmlNodeModelIndex();

    /* Now, get all the parent's children. That is, all our siblings. */
    const QFileInfo parentFI(toFileInfo(parent));
    Q_ASSERT(Type(parent.additionalData()) == Directory);
    const QFileInfoList
    siblings(QDir(parentFI.absoluteFilePath()).entryInfoList(QStringList(), m_filterAllowAll, m_sortFlags));
    Q_ASSERT_X(!siblings.isEmpty(),
               Q_FUNC_INFO,
               "This would be a contradiction, there's at least one.");

    const int indexOfMe = siblings.indexOf(fileInfo);
    Q_ASSERT_X(indexOfMe != -1, Q_FUNC_INFO, "We're there, somewhere.");

    const int siblingIndex = indexOfMe + offset;
    if (siblingIndex < 0 || siblingIndex > siblings.count() - 1)
        return QXmlNodeModelIndex();
    else
        return toNodeIndex(siblings.at(siblingIndex));
}

/*!
 */
QXmlNodeModelIndex
FileTree::nextFromSimpleAxis(QSimpleXmlNodeModel::SimpleAxis axis,
                             const QXmlNodeModelIndex &nodeIndex) const
{
    const QFileInfo fi(toFileInfo(nodeIndex));
    const Type type = Type(nodeIndex.additionalData());

    if (type != File && type != Directory) {
        /* We're an attribute. */
        Q_ASSERT_X(axis == Parent,
                   Q_FUNC_INFO,
                   "QSimpleXmlNodeModel guarantees that it only "
                   "asks for Parent for attributes.");
        return toNodeIndex(fi, Directory);
    }

    switch(axis) {
        case Parent:
            return toNodeIndex(QFileInfo(fi.path()), Directory);
        case FirstChild:
        {
            if (type == File) {
                /* This guy doesn't have children. */
                return QXmlNodeModelIndex();
            }
            else {
                Q_ASSERT(type == Directory);
                Q_ASSERT_X(fi.isDir(),
                           Q_FUNC_INFO,
                           "If we're a directory, we really should be one!");

                const QDir dir(fi.absoluteFilePath());
                Q_ASSERT(dir.exists());

                const QFileInfoList children(dir.entryInfoList(QStringList(), m_filterAllowAll, m_sortFlags));

                if (children.isEmpty())
                    return QXmlNodeModelIndex();

                const QFileInfo firstChild(children.first());
                return toNodeIndex(firstChild);
            }
        }
        case PreviousSibling:
            return nextSibling(nodeIndex, fi, -1);
        case NextSibling:
            return nextSibling(nodeIndex, fi, 1);
    }

    Q_ASSERT_X(false, Q_FUNC_INFO, "This line should never be reached.");
    return QXmlNodeModelIndex();
}

/*!
 */
QUrl FileTree::documentUri(const QXmlNodeModelIndex &node) const
{
    /*
      We always have the same document URI,
      regardless of what node it is.
    */
    Q_UNUSED(node);

    /*
      No matter what instance this FileTree represents, it will always
     have file:/// as the root.
    */
    return QUrl("file:///");
}

/*!
 */
QXmlNodeModelIndex::NodeKind
FileTree::kind(const QXmlNodeModelIndex &node) const
{
    switch(Type(node.additionalData())) {
        case Directory:
        /* Fallthrough. */
        case File:
            return QXmlNodeModelIndex::Element;
        default:
            return QXmlNodeModelIndex::Attribute;
    }
}

/*!
 */
QXmlNodeModelIndex::DocumentOrder
FileTree::compareOrder(const QXmlNodeModelIndex&,
                       const QXmlNodeModelIndex&) const
{
    /* There is no order between files, so just return an arbitrary value. */
    return QXmlNodeModelIndex::Precedes;
}

/*!
 */
QXmlName FileTree::name(const QXmlNodeModelIndex &node) const
{
    return m_names.at(node.additionalData());
}

/*!
 */
QXmlNodeModelIndex FileTree::root(const QXmlNodeModelIndex &node) const
{
    /* We always have the same root. */
    Q_UNUSED(node);

    return toNodeIndex(QFileInfo(QLatin1String("/")));
}

/*!
 */
QVariant FileTree::typedValue(const QXmlNodeModelIndex &node) const
{
    const QFileInfo &fi = toFileInfo(node);

    switch(Type(node.additionalData())) {
        case Directory:
        /* Fallthrough. */
        case File:
            return QString();
        case AttributeFileName:
            return fi.fileName();
        case AttributeFilePath:
            return fi.filePath();
        case AttributeSize:
            return fi.size();
        case AttributeMIMEType:
        {
            /* We don't have any MIME detection code currently, so return
             * the most generic one. */
            return QLatin1String("application/octet-stream");
        }
        case AttributeSuffix:
            return fi.suffix();
    }

    Q_ASSERT_X(false, Q_FUNC_INFO, "This line should never be reached.");
    return QString();
}

/*!
 */
QVector<QXmlNodeModelIndex>
FileTree::attributes(const QXmlNodeModelIndex &element) const
{
    QVector<QXmlNodeModelIndex> result;

    /* Both elements has this attribute. */
    const QFileInfo &forElement = toFileInfo(element);
    result.append(toNodeIndex(forElement, AttributeFilePath));
    result.append(toNodeIndex(forElement, AttributeFileName));

    if (Type(element.additionalData() == File)) {
        result.append(toNodeIndex(forElement, AttributeSize));
        result.append(toNodeIndex(forElement, AttributeSuffix));
        result.append(toNodeIndex(forElement, AttributeMIMEType));
    }
    else {
        Q_ASSERT(element.additionalData() == Directory);
    }

    return result;
}


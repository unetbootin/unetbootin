/****************************************************************************
**
** Copyright (C) 2006-2008 Trolltech ASA. All rights reserved.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Patternist_QObjectPropertyToAttributeIterator_h
#define Patternist_QObjectPropertyToAttributeIterator_h

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    /**
     * Remember that the XPath Data Models index starts from 1, while
     * QMetaObject::propertyOffset() starts from 0.
     */
    class QObjectPropertyToAttributeIterator : public QAbstractXmlForwardIterator<QXmlNodeModelIndex>
    {
    public:
        inline QObjectPropertyToAttributeIterator(const QObjectNodeModel *const nm,
                                                  QObject *const object) : m_nodeModel(nm)
                                                                         , m_object(object)
                                                                         , m_propertyCount(object->metaObject()->propertyCount())
                                                                         , m_currentPos(0)
        {
        }

        virtual QXmlNodeModelIndex next()
        {
            if(m_currentPos == -1 || m_currentPos == m_propertyCount)
            {
                m_currentPos = -1;
                return QXmlNodeModelIndex();
            }

            QXmlNodeModelIndex retval(m_nodeModel->createIndex(m_object, QObjectNodeModel::IsAttribute | m_currentPos));
            ++m_currentPos;

            return retval;
        }

        virtual QXmlNodeModelIndex current() const
        {
            if(m_currentPos == -1)
                return QXmlNodeModelIndex();
            else
                return m_nodeModel->createIndex(m_object, QObjectNodeModel::IsAttribute | (m_currentPos - 1));
        }

        virtual xsInteger position() const
        {
            return m_currentPos;
        }

        virtual xsInteger count()
        {
            return m_propertyCount;
        }

        virtual QXmlNodeModelIndex::Iterator::Ptr copy() const
        {
            return QXmlNodeModelIndex::Iterator::Ptr(new QObjectPropertyToAttributeIterator(m_nodeModel, m_object));
        }

    private:
        const QObjectNodeModel *const   m_nodeModel;
        QObject *const                  m_object;
        const int                       m_propertyCount;
        xsInteger                       m_currentPos;
    };
}

QT_END_NAMESPACE

QT_END_HEADER

#endif

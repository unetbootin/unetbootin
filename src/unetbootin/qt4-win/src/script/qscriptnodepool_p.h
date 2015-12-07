/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtScript module of the Qt Toolkit.
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

#ifndef QSCRIPTNODEPOOL_P_H
#define QSCRIPTNODEPOOL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QtCore/QHash>
#include <QtCore/QString>

#include "qscriptmemorypool_p.h"
#include "qscriptenginefwd_p.h"

QT_BEGIN_NAMESPACE

namespace QScript {

namespace AST {
class Node;
} // namespace AST

class Code;
class CompilationUnit;

template <typename NodeType>
inline NodeType *makeAstNode(MemoryPool *storage)
{
    NodeType *node = new (storage->allocate(sizeof(NodeType))) NodeType();
    return node;
}

template <typename NodeType, typename Arg1>
inline NodeType *makeAstNode(MemoryPool *storage, Arg1 arg1)
{
    NodeType *node = new (storage->allocate(sizeof(NodeType))) NodeType(arg1);
    return node;
}

template <typename NodeType, typename Arg1, typename Arg2>
inline NodeType *makeAstNode(MemoryPool *storage, Arg1 arg1, Arg2 arg2)
{
    NodeType *node = new (storage->allocate(sizeof(NodeType))) NodeType(arg1, arg2);
    return node;
}

template <typename NodeType, typename Arg1, typename Arg2, typename Arg3>
inline NodeType *makeAstNode(MemoryPool *storage, Arg1 arg1, Arg2 arg2, Arg3 arg3)
{
    NodeType *node = new (storage->allocate(sizeof(NodeType))) NodeType(arg1, arg2, arg3);
    return node;
}

template <typename NodeType, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
inline NodeType *makeAstNode(MemoryPool *storage, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4)
{
    NodeType *node = new (storage->allocate(sizeof(NodeType))) NodeType(arg1, arg2, arg3, arg4);
    return node;
}

class NodePool : public MemoryPool
{
public:
    NodePool(const QString &fileName, QScriptEnginePrivate *engine)
        : m_fileName(fileName), m_engine(engine)
       {
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
           m_id = engine->nextScriptId();
#endif
       }
    virtual ~NodePool();

    Code *createCompiledCode(AST::Node *node, CompilationUnit &compilation);

    inline QString fileName() const { return m_fileName; }
    inline QScriptEnginePrivate *engine() const { return m_engine; }
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    inline qint64 id() const { return m_id; }
#endif

private:
    QHash<AST::Node*, Code*> m_codeCache;
    QString m_fileName;
    QScriptEnginePrivate *m_engine;
#ifndef Q_SCRIPT_NO_EVENT_NOTIFY
    qint64 m_id;
#endif

private:
    Q_DISABLE_COPY(NodePool)
};

} // namespace QScript

QT_END_NAMESPACE

#endif

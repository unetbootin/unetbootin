/****************************************************************************
**
** Copyright (C) 2006-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#include <QtCore/QFile>
#include <QtCore/QStringList>

#include "proitems.h"
#include "proreader.h"

QT_BEGIN_NAMESPACE

ProReader::ProReader()
{
    m_fixBackSlashes = false;
}

void ProReader::setEnableBackSlashFixing(bool enable)
{
    m_fixBackSlashes = enable;
}

void ProReader::cleanup()
{
    m_commentItem = 0;
    m_block = 0;
    m_proitem.clear();
    m_blockstack.clear();
    m_pendingComment.clear();
}

ProFile *ProReader::read(QIODevice *device, const QString &name)
{
    m_syntaxerror = false;
    m_currentLineNumber = 1;
    ProFile *pf = new ProFile(name);
    m_blockstack.push(pf);

    while (!device->atEnd()) {
        QByteArray line = device->readLine();
        if (m_fixBackSlashes) {
            line.replace('\\', '/');
        }
        if (!parseline(line)) {
            cleanup();
            return 0;
        }
        ++m_currentLineNumber;
    }

    cleanup();
    return pf;
}

ProFile *ProReader::read(const QString &fileName)
{
    cleanup();

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        cleanup();
        return 0;
    }
    ProFile *pf = read(&file, fileName);
    file.close();
    return pf;
}

bool ProReader::parseline(QByteArray line)
{
    if (m_blockstack.isEmpty())
        return false;

    char quote = 0;
    int parens = 0;
    bool contNextLine = false;
    line = line.simplified();

    for(int i=0; !m_syntaxerror && i<line.length(); ++i) {
        const char c = line.at(i);
        if (quote && c == quote)
            quote = 0;
        else if (c == '(')
            ++parens;
        else if (c == ')')
            --parens;
        else if (c == '"'
            && ((i == 0) || (line.at(i - 1) != '\\')))
            quote = c;
        else if (!parens && !quote) {
            if (c == '#') {
                insertComment(line.mid(i + 1));
                contNextLine = m_contNextLine;
                break;
            } else if (c == '\\' && (i >= line.count() - 1)) {
                updateItem();
                contNextLine = true;
                continue;
            } else if (m_block && (m_block->blockKind() & ProBlock::VariableKind)) {
                if (c == ' ') {
                    updateItem();
                } else {
                    m_proitem += c;
                }
                continue;
            } else if (c == ':') {
                enterScope(false);
                continue;
            } else if (c == '{') {
                enterScope(true);
                continue;
            } else if (c == '}') {
                leaveScope();
                continue;
            } else if (c == '=') {
                insertVariable(line, &i);
                continue;
            } else if (c == '|' || c == '!') {
                insertOperator(c);
                continue;
            }
        }

        m_proitem += c;
    }
    m_contNextLine = contNextLine;

    if (!m_syntaxerror) {
        updateItem();
        if (!m_contNextLine)
            finalizeBlock();
    }
    return !m_syntaxerror;
}

void ProReader::finalizeBlock()
{
    if (m_blockstack.isEmpty()) {
        m_syntaxerror = true;
    } else {
        if (m_blockstack.top()->blockKind() & ProBlock::SingleLine)
            leaveScope();
        m_block = 0;
        m_commentItem = 0;
    }
}

void ProReader::insertVariable(const QByteArray &line, int *i)
{
    ProVariable::VariableOperator opkind;

    switch(m_proitem.at(m_proitem.length()-1)) {
        case '+':
            m_proitem.chop(1);
            opkind = ProVariable::AddOperator;
            break;
        case '-':
            m_proitem.chop(1);
            opkind = ProVariable::RemoveOperator;
            break;
        case '*':
            m_proitem.chop(1);
            opkind = ProVariable::UniqueAddOperator;
            break;
        case '~':
            m_proitem.chop(1);
            opkind = ProVariable::ReplaceOperator;
            break;
        default:
            opkind = ProVariable::SetOperator;
    }

    ProBlock *block = m_blockstack.top();
    m_proitem = m_proitem.trimmed();
    ProVariable *variable = new ProVariable(m_proitem, block);
    variable->setLineNumber(m_currentLineNumber);
    variable->setVariableOperator(opkind);
    block->appendItem(variable);
    m_block = variable;

    if (!m_pendingComment.isEmpty()) {
        m_block->setComment(m_pendingComment);
        m_pendingComment.clear();
    }
    m_commentItem = variable;

    m_proitem.clear();

    if (opkind == ProVariable::ReplaceOperator) {
        // skip util end of line or comment
        while (1) {
            ++(*i);

            // end of line?
            if (*i >= line.count())
                break;

            // comment?
            if (line.at(*i) == '#') {
                --(*i);
                break;
            }

            m_proitem += line.at(*i);
        }
        m_proitem = m_proitem.trimmed();
    }
}

void ProReader::insertOperator(const char op)
{
    updateItem();

    ProOperator::OperatorKind opkind;
    switch(op) {
        case '!':
            opkind = ProOperator::NotOperator;
            break;
        case '|':
            opkind = ProOperator::OrOperator;
            break;
        default:
            opkind = ProOperator::OrOperator;
    }

    ProBlock *block = currentBlock();
    ProOperator *proOp = new ProOperator(opkind);
    proOp->setLineNumber(m_currentLineNumber);
    block->appendItem(proOp);
    m_commentItem = proOp;
}

void ProReader::insertComment(const QByteArray &comment)
{
    updateItem();

    QByteArray strComment;
    if (!m_commentItem)
        strComment = m_pendingComment;
    else
        strComment = m_commentItem->comment();

    if (strComment.isEmpty())
        strComment = comment;
    else
        strComment += '\n' + comment.trimmed();

    strComment = strComment.trimmed();

    if (!m_commentItem)
        m_pendingComment = strComment;
    else
        m_commentItem->setComment(strComment);
}

void ProReader::enterScope(bool multiLine)
{
    updateItem();

    ProBlock *parent = currentBlock();
    ProBlock *block = new ProBlock(parent);
    block->setLineNumber(m_currentLineNumber);
    parent->setBlockKind(ProBlock::ScopeKind);

    parent->appendItem(block);

    if (multiLine)
        block->setBlockKind(ProBlock::ScopeContentsKind);
    else
        block->setBlockKind(ProBlock::ScopeContentsKind|ProBlock::SingleLine);

    m_blockstack.push(block);
    m_block = 0;
}

void ProReader::leaveScope()
{
    updateItem();
    m_blockstack.pop();
    finalizeBlock();
}

ProBlock *ProReader::currentBlock()
{
    if (m_block)
        return m_block;

    ProBlock *parent = m_blockstack.top();
    m_block = new ProBlock(parent);
    m_block->setLineNumber(m_currentLineNumber);
    parent->appendItem(m_block);

    if (!m_pendingComment.isEmpty()) {
        m_block->setComment(m_pendingComment);
        m_pendingComment.clear();
    }

    m_commentItem = m_block;

    return m_block;
}

void ProReader::updateItem()
{
    m_proitem = m_proitem.trimmed();
    if (m_proitem.isEmpty())
        return;

    ProBlock *block = currentBlock();
    if (block->blockKind() & ProBlock::VariableKind) {
        m_commentItem = new ProValue(m_proitem,
            static_cast<ProVariable*>(block));
    } else if (m_proitem.endsWith(')')) {
        m_commentItem = new ProFunction(m_proitem);
    } else {
        m_commentItem = new ProCondition(m_proitem);
    }
    m_commentItem->setLineNumber(m_currentLineNumber);
    block->appendItem(m_commentItem);

    m_proitem.clear();
}

QT_END_NAMESPACE

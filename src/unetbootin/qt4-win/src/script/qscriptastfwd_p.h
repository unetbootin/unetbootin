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

#ifndef QSCRIPTAST_FWD_P_H
#define QSCRIPTAST_FWD_P_H

#include <QtCore/qglobal.h>

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

QT_BEGIN_NAMESPACE

namespace QScript { namespace AST {

class Visitor;
class Node;
class ExpressionNode;
class Statement;
class ThisExpression;
class IdentifierExpression;
class NullExpression;
class TrueLiteral;
class FalseLiteral;
class NumericLiteral;
class StringLiteral;
class RegExpLiteral;
class ArrayLiteral;
class ObjectLiteral;
class ElementList;
class Elision;
class PropertyNameAndValueList;
class PropertyName;
class IdentifierPropertyName;
class StringLiteralPropertyName;
class NumericLiteralPropertyName;
class ArrayMemberExpression;
class FieldMemberExpression;
class NewMemberExpression;
class NewExpression;
class CallExpression;
class ArgumentList;
class PostIncrementExpression;
class PostDecrementExpression;
class DeleteExpression;
class VoidExpression;
class TypeOfExpression;
class PreIncrementExpression;
class PreDecrementExpression;
class UnaryPlusExpression;
class UnaryMinusExpression;
class TildeExpression;
class NotExpression;
class BinaryExpression;
class ConditionalExpression;
class Expression; // ### rename
class Block;
class StatementList;
class VariableStatement;
class VariableDeclarationList;
class VariableDeclaration;
class EmptyStatement;
class ExpressionStatement;
class IfStatement;
class DoWhileStatement;
class WhileStatement;
class ForStatement;
class LocalForStatement;
class ForEachStatement;
class LocalForEachStatement;
class ContinueStatement;
class BreakStatement;
class ReturnStatement;
class WithStatement;
class SwitchStatement;
class CaseBlock;
class CaseClauses;
class CaseClause;
class DefaultClause;
class LabelledStatement;
class ThrowStatement;
class TryStatement;
class Catch;
class Finally;
class FunctionDeclaration;
class FunctionExpression;
class FormalParameterList;
class FunctionBody;
class Program;
class SourceElements;
class SourceElement;
class FunctionSourceElement;
class StatementSourceElement;

} } // namespace AST

QT_END_NAMESPACE

#endif

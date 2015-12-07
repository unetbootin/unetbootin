/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtXMLPatterns module of the Qt Toolkit.
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

#include "qcommonsequencetypes_p.h"
#include "qoptimizerblocks_p.h"

#include "qoptimizationpasses_p.h"

QT_BEGIN_NAMESPACE

using namespace QPatternist;

OptimizationPass::List OptimizationPasses::comparisonPasses;
OptimizationPass::List OptimizationPasses::forPasses;
OptimizationPass::List OptimizationPasses::ifThenPasses;
OptimizationPass::List OptimizationPasses::notFN;

void OptimizationPasses::Coordinator::init()
{
    static bool isInitialized = false; // STATIC DATA

    if(isInitialized)
        return;

    isInitialized = true;

    /* Note, below is many of the building blocks re-used in several passes
     * in order to reduce memory use. Thus, when changing one building block
     * it potentially affects many passes. */

    /* ****************************************************** */
    /* Rewrite "count(<expr>) ge 1" into "exists(<expr>)" */
    OptimizationPass::ExpressionMarker firstFirstChild;
    firstFirstChild.append(0);
    firstFirstChild.append(0);

    ExpressionIdentifier::List geOpIDs;
    const ExpressionIdentifier::Ptr countFN(new ByIDIdentifier(Expression::IDCountFN));
    geOpIDs.append(countFN);
    geOpIDs.append(ExpressionIdentifier::Ptr(new IntegerIdentifier(1)));

    QVector<Expression::ID> geMatcher;
    geMatcher.append(Expression::IDValueComparison);
    geMatcher.append(Expression::IDGeneralComparison);

    const ExpressionIdentifier::Ptr ge(new ComparisonIdentifier(geMatcher,
                                                                AtomicComparator::OperatorGreaterOrEqual));

    const ExpressionCreator::Ptr existsFN(new ByIDCreator(Expression::IDExistsFN));
    const OptimizationPass::Ptr geToExists(new OptimizationPass(ge, geOpIDs, firstFirstChild, existsFN));
    comparisonPasses.append(geToExists);
    /* ****************************************************** */

    /* ****************************************************** */
    /* Rewrite "count(<expr>) gt 0" into "exists(<expr>)" */
    ExpressionIdentifier::List countAndIntZero;
    countAndIntZero.append(countFN);
    const ExpressionIdentifier::Ptr zeroInteger(new IntegerIdentifier(0));
    countAndIntZero.append(zeroInteger);

    const ExpressionIdentifier::Ptr gt(new ComparisonIdentifier(geMatcher,
                                                                AtomicComparator::OperatorGreaterThan));

    const OptimizationPass::Ptr gtToExists(new OptimizationPass(gt, countAndIntZero,
                                                                firstFirstChild, existsFN));
    comparisonPasses.append(gtToExists);
    /* ****************************************************** */

    /* ****************************************************** */
    /* Rewrite "count(<expr>) ne 0" into "exists(<expr>)" */

    const ExpressionIdentifier::Ptr ne(new ComparisonIdentifier(geMatcher,
                                                                AtomicComparator::OperatorNotEqual));
    const OptimizationPass::Ptr neToExists(new OptimizationPass(ne, countAndIntZero, firstFirstChild,
                                                                existsFN,
                                                                OptimizationPass::AnyOrder));
    comparisonPasses.append(neToExists);
    /* ****************************************************** */

    /* ****************************************************** */
    /* Rewrite "count(<expr>) eq 0" into "empty(<expr>)" */
    ExpressionIdentifier::List eqOpIDs;
    eqOpIDs.append(countFN);
    eqOpIDs.append(zeroInteger);
    const ExpressionCreator::Ptr emptyFN(new ByIDCreator(Expression::IDEmptyFN));
    const ExpressionIdentifier::Ptr eq(new ComparisonIdentifier(geMatcher,
                                                                AtomicComparator::OperatorEqual));
    const OptimizationPass::Ptr eqToEmpty(new OptimizationPass(eq, eqOpIDs, firstFirstChild,
                                                               emptyFN,
                                                               OptimizationPass::AnyOrder));
    comparisonPasses.append(eqToEmpty);

    /* ****************************************************** */

    /* ****************************************************** */
    /* Rewrite "for $var in <expr> return $var" into "<expr>" */
    ExpressionIdentifier::List forOps;
    OptimizationPass::ExpressionMarker firstChild;
    firstChild.append(0);

    forOps.append(ExpressionIdentifier::Ptr());
    forOps.append(ExpressionIdentifier::Ptr(new ByIDIdentifier(Expression::IDRangeVariableReference)));
    const OptimizationPass::Ptr simplifyFor(new OptimizationPass(ExpressionIdentifier::Ptr(), forOps,
                                                                 firstChild, ExpressionCreator::Ptr()));
    forPasses.append(simplifyFor);
    /* ****************************************************** */

    /* ****************************************************** */
    /* Rewrite "if(<expr>) then true() else false()" to "<expr>" */
    OptimizationPass::ExpressionMarker marker;
    marker.append(0);

    ExpressionIdentifier::List opIDs;
    opIDs.append(ExpressionIdentifier::Ptr(new BySequenceTypeIdentifier(
                        CommonSequenceTypes::ExactlyOneBoolean)));
    opIDs.append(ExpressionIdentifier::Ptr(new BooleanIdentifier(true)));
    opIDs.append(ExpressionIdentifier::Ptr(new BooleanIdentifier(false)));

    const OptimizationPass::Ptr pass(new OptimizationPass(ExpressionIdentifier::Ptr(), opIDs, marker));
    ifThenPasses.append(pass);
    /* ****************************************************** */

    /* ****************************************************** */
    /* Rewrite "not(exists(X))" into "empty(X)" */
    ExpressionIdentifier::List idExistsFN;
    idExistsFN.append(ExpressionIdentifier::Ptr(new ByIDIdentifier(Expression::IDExistsFN)));

    notFN.append(OptimizationPass::Ptr(new OptimizationPass(ExpressionIdentifier::Ptr(),
                                                            idExistsFN,
                                                            firstFirstChild,
                                                            emptyFN)));

    /* Rewrite "not(empty(X))" into "exists(X)" */
    ExpressionIdentifier::List idEmptyFN;
    idEmptyFN.append(ExpressionIdentifier::Ptr(new ByIDIdentifier(Expression::IDEmptyFN)));

    notFN.append(OptimizationPass::Ptr(new OptimizationPass(ExpressionIdentifier::Ptr(),
                                                            idEmptyFN,
                                                            firstFirstChild,
                                                            existsFN)));
    /* ****************************************************** */
}

QT_END_NAMESPACE

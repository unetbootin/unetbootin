/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Designer of the Qt Toolkit.
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

#ifndef QLONGLONGVALIDATOR_H
#define QLONGLONGVALIDATOR_H

#include <QtGui/QValidator>

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

class QLongLongValidator : public QValidator
{
    Q_OBJECT
    Q_PROPERTY(qlonglong bottom READ bottom WRITE setBottom)
    Q_PROPERTY(qlonglong top READ top WRITE setTop)

public:
    explicit QLongLongValidator(QObject * parent);
    QLongLongValidator(qlonglong bottom, qlonglong top, QObject * parent);
    ~QLongLongValidator();

    QValidator::State validate(QString &, int &) const;

    void setBottom(qlonglong);
    void setTop(qlonglong);
    virtual void setRange(qlonglong bottom, qlonglong top);

    qlonglong bottom() const { return b; }
    qlonglong top() const { return t; }

private:
    Q_DISABLE_COPY(QLongLongValidator)

    qlonglong b;
    qlonglong t;
};

// ----------------------------------------------------------------------------
class QULongLongValidator : public QValidator
{
    Q_OBJECT
    Q_PROPERTY(qulonglong bottom READ bottom WRITE setBottom)
    Q_PROPERTY(qulonglong top READ top WRITE setTop)

public:
    explicit QULongLongValidator(QObject * parent);
    QULongLongValidator(qulonglong bottom, qulonglong top, QObject * parent);
    ~QULongLongValidator();

    QValidator::State validate(QString &, int &) const;

    void setBottom(qulonglong);
    void setTop(qulonglong);
    virtual void setRange(qulonglong bottom, qulonglong top);

    qulonglong bottom() const { return b; }
    qulonglong top() const { return t; }

private:
    Q_DISABLE_COPY(QULongLongValidator)

    qulonglong b;
    qulonglong t;
};

}  // namespace qdesigner_internal

QT_END_NAMESPACE

#endif // QLONGLONGVALIDATOR_H

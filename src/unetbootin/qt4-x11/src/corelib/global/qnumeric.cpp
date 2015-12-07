/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "qnumeric.h"
#include "qnumeric_p.h"

QT_BEGIN_NAMESPACE

Q_CORE_EXPORT bool qIsInf(double d) { return qt_is_inf(d); }
Q_CORE_EXPORT bool qIsNaN(double d) { return qt_is_nan(d); }
Q_CORE_EXPORT bool qIsFinite(double d) { return qt_is_finite(d); }
Q_CORE_EXPORT bool qIsInf(float f) { return qt_is_inf(f); }
Q_CORE_EXPORT bool qIsNaN(float f) { return qt_is_nan(f); }
Q_CORE_EXPORT bool qIsFinite(float f) { return qt_is_finite(f); }
Q_CORE_EXPORT double qSNaN() { return qt_snan(); }
Q_CORE_EXPORT double qQNaN() { return qt_qnan(); }
Q_CORE_EXPORT double qInf() { return qt_inf(); }


QT_END_NAMESPACE

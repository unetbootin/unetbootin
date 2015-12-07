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

#include "qplatformdefs.h"

#include <QtCore/qatomic.h>

static pthread_mutex_t qAtomicMutex = PTHREAD_MUTEX_INITIALIZER;

Q_CORE_EXPORT
bool QBasicAtomicInt_testAndSetOrdered(volatile int *_q_value, int expectedValue, int newValue)
{
    bool returnValue = false;
    pthread_mutex_lock(&qAtomicMutex);
    if (*_q_value == expectedValue) {
        *_q_value = newValue;
        returnValue = true;
    }
    pthread_mutex_unlock(&qAtomicMutex);
    return returnValue;
}

Q_CORE_EXPORT
int QBasicAtomicInt_fetchAndStoreOrdered(volatile int *_q_value, int newValue)
{
    int returnValue;
    pthread_mutex_lock(&qAtomicMutex);
    returnValue = *_q_value;
    *_q_value = newValue;
    pthread_mutex_unlock(&qAtomicMutex);
    return returnValue;
}

Q_CORE_EXPORT
int QBasicAtomicInt_fetchAndAddOrdered(volatile int *_q_value, int valueToAdd)
{
    int returnValue;
    pthread_mutex_lock(&qAtomicMutex);
    returnValue = *_q_value;
    *_q_value += valueToAdd;
    pthread_mutex_unlock(&qAtomicMutex);
    return returnValue;
}

Q_CORE_EXPORT
bool QBasicAtomicPointer_testAndSetOrdered(void * volatile *_q_value,
                                           void *expectedValue,
                                           void *newValue)
{
    bool returnValue = false;
    pthread_mutex_lock(&qAtomicMutex);
    if (*_q_value == expectedValue) {
        *_q_value = newValue;
        returnValue = true;
    }
    pthread_mutex_unlock(&qAtomicMutex);
    return returnValue;
}

Q_CORE_EXPORT
void *QBasicAtomicPointer_fetchAndStoreOrdered(void * volatile *_q_value, void *newValue)
{
    void *returnValue;
    pthread_mutex_lock(&qAtomicMutex);
    returnValue = *_q_value;
    *_q_value = newValue;
    pthread_mutex_unlock(&qAtomicMutex);
    return returnValue;
}

Q_CORE_EXPORT
void *QBasicAtomicPointer_fetchAndAddOrdered(void * volatile *_q_value, qptrdiff valueToAdd)
{
    void *returnValue;
    pthread_mutex_lock(&qAtomicMutex);
    returnValue = *_q_value;
    *_q_value = reinterpret_cast<char *>(returnValue) + valueToAdd;
    pthread_mutex_unlock(&qAtomicMutex);
    return returnValue;
}

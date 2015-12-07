/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

#ifndef Q3VALUEVECTOR_H
#define Q3VALUEVECTOR_H

#include <QtCore/qvector.h>

#ifndef QT_NO_STL
#include <vector>
#endif

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Qt3SupportLight)

template <typename T>
class Q3ValueVector : public QVector<T>
{
public:
    inline Q3ValueVector() : QVector<T>() {}
    inline Q3ValueVector(const Q3ValueVector<T>& v) : QVector<T>(v) {}
    inline Q3ValueVector(typename QVector<T>::size_type n,
                         const T& val = T()) : QVector<T>(n, val) {}

#ifndef QT_NO_STL
    inline Q3ValueVector(const std::vector<T>& v) : QVector<T>()
        { this->resize(v.size()); qCopy(v.begin(), v.end(), this->begin()); }
#endif

    Q3ValueVector<T>& operator= (const Q3ValueVector<T>& v)
        { QVector<T>::operator=(v); return *this; }

#ifndef QT_NO_STL
    Q3ValueVector<T>& operator= (const std::vector<T>& v)
    {
        this->clear();
        this->resize(v.size());
        qCopy(v.begin(), v.end(), this->begin());
        return *this;
    }
#endif

    void resize(int n, const T& val = T())
    {
        if (n < this->size())
            erase(this->begin() + n, this->end());
        else
            insert(this->end(), n - this->size(), val);
    }


    T& at(int i, bool* ok = 0)
    {
        this->detach();
        if (ok)
            *ok = (i >= 0 && i < this->size());
        return *(this->begin() + i);
    }

    const T&at(int i, bool* ok = 0) const
    {
        if (ok)
            *ok = (i >= 0 && i < this->size());
        return *(this->begin() + i);
    }
};

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q3VALUEVECTOR_H

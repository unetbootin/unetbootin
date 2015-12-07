/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt3Support module of the Qt Toolkit.
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

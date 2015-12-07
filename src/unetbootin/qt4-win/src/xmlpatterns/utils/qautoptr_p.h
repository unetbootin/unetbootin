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

#ifndef QPatternist_AutoPtr_p_h
#define QPatternist_AutoPtr_p_h

#include <QtGlobal>

QT_BEGIN_NAMESPACE

namespace QPatternist
{
    template<typename T>
    class AutoPtrRef
    {
    public:
        explicit AutoPtrRef(T *const other) : m_ptr(other)
        {
        }

        T* m_ptr;
    };

    template<typename T>
    class AutoPtr
    {
    public:
        explicit inline AutoPtr(T *pointer = 0) : m_ptr(pointer)
        {
        }

        inline AutoPtr(AutoPtr<T> &other) : m_ptr(other.release())
        {
        }

        inline ~AutoPtr()
        {
            delete m_ptr;
        }

        inline T &operator*() const
        {
            Q_ASSERT_X(m_ptr, Q_FUNC_INFO, "");
            return *m_ptr;
        }

        T *operator->() const
        {
            Q_ASSERT_X(m_ptr, Q_FUNC_INFO, "");
            return m_ptr;
        }

        inline bool operator!() const
        {
            return !m_ptr;
        }

        inline operator bool() const
        {
            return m_ptr != 0;
        }

        AutoPtr(AutoPtrRef<T> ref) : m_ptr(ref.m_ptr)
        {
        }

        AutoPtr& operator=(AutoPtrRef<T> ref)
        {
            if(ref.m_ptr != this->get())
            {
                delete m_ptr;
                m_ptr = ref.m_ptr;
            }
            return *this;
        }

#ifndef QT_NO_MEMBER_TEMPLATES
        template<typename L>
        operator AutoPtrRef<L>()
        {
            return AutoPtrRef<L>(this->release());
        }

        template<typename L>
        operator AutoPtr<L>()
        {
            return AutoPtr<L>(this->release());
        }

        template<typename L>
        inline AutoPtr(AutoPtr<L>& other) : m_ptr(other.release())
        {
        }
#endif

        inline T *release()
        {
            T *const retval = m_ptr;
            m_ptr = 0;
            return retval;
        }

        inline T *data() const
        {
            return m_ptr;
        }

        void reset(T * other = 0)
        {
            if(other != m_ptr)
            {
                delete m_ptr;
                m_ptr = other;
            }
        }

        inline AutoPtr &operator=(AutoPtr &other)
        {
            reset(other.release());
            return *this;
        }

    private:
        T *m_ptr;
    };
}

QT_END_NAMESPACE
#endif

/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtTest module of the Qt Toolkit.
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

#ifndef QTESTCASE_H
#define QTESTCASE_H

#include <QtTest/qtest_global.h>

#include <QtCore/qnamespace.h>
#include <QtCore/qmetatype.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Test)

#define QVERIFY(statement) \
do {\
    if (!QTest::qVerify((statement), #statement, "", __FILE__, __LINE__))\
        return;\
} while (0)

#define QFAIL(message) \
do {\
    QTest::qFail(message, __FILE__, __LINE__);\
    return;\
} while (0)

#define QVERIFY2(statement, description) \
do {\
    if (statement) {\
        if (!QTest::qVerify(true, #statement, (description), __FILE__, __LINE__))\
            return;\
    } else {\
        if (!QTest::qVerify(false, #statement, (description), __FILE__, __LINE__))\
            return;\
    }\
} while (0)

#define QCOMPARE(actual, expected) \
do {\
    if (!QTest::qCompare(actual, expected, #actual, #expected, __FILE__, __LINE__))\
        return;\
} while (0)

#define QSKIP(statement, mode) \
do {\
    QTest::qSkip(statement, QTest::mode, __FILE__, __LINE__);\
    return;\
} while (0)

#define QEXPECT_FAIL(dataIndex, comment, mode)\
do {\
    if (!QTest::qExpectFail(dataIndex, comment, QTest::mode, __FILE__, __LINE__))\
        return;\
} while (0)

#define QFETCH(type, name)\
    type name = *static_cast<type *>(QTest::qData(#name, ::qMetaTypeId<type >()))

#define QFETCH_GLOBAL(type, name)\
    type name = *static_cast<type *>(QTest::qGlobalData(#name, ::qMetaTypeId<type >()))

#define DEPENDS_ON(funcName)

#define QTEST(actual, testElement)\
do {\
    if (!QTest::qTest(actual, testElement, #actual, #testElement, __FILE__, __LINE__))\
        return;\
} while (0)

#define QWARN(msg)\
    QTest::qWarn(msg)

class QObject;
class QTestData;

#define QTEST_COMPARE_DECL(KLASS)\
    template<> Q_TESTLIB_EXPORT char *toString<KLASS >(const KLASS &);

namespace QTest
{
    template <typename T>
    inline char *toString(const T &)
    {
        return 0;
    }

    Q_TESTLIB_EXPORT char *toHexRepresentation(const char *ba, int length);
    Q_TESTLIB_EXPORT char *toString(const char *);
    Q_TESTLIB_EXPORT char *toString(const void *);

    Q_TESTLIB_EXPORT int qExec(QObject *testObject, int argc = 0, char **argv = 0);
    Q_TESTLIB_EXPORT int qExec(QObject *testObject, const QStringList &arguments);

    Q_TESTLIB_EXPORT bool qVerify(bool statement, const char *statementStr, const char *description,
                                 const char *file, int line);
    Q_TESTLIB_EXPORT void qFail(const char *statementStr, const char *file, int line);
    Q_TESTLIB_EXPORT void qSkip(const char *message, SkipMode mode, const char *file, int line);
    Q_TESTLIB_EXPORT bool qExpectFail(const char *dataIndex, const char *comment, TestFailMode mode,
                           const char *file, int line);
    Q_TESTLIB_EXPORT void qWarn(const char *message);
    Q_TESTLIB_EXPORT void ignoreMessage(QtMsgType type, const char *message);

    Q_TESTLIB_EXPORT void *qData(const char *tagName, int typeId);
    Q_TESTLIB_EXPORT void *qGlobalData(const char *tagName, int typeId);
    Q_TESTLIB_EXPORT void *qElementData(const char *elementName, int metaTypeId);
    Q_TESTLIB_EXPORT QObject *testObject();

    Q_TESTLIB_EXPORT const char *currentTestFunction();
    Q_TESTLIB_EXPORT const char *currentDataTag();
    Q_TESTLIB_EXPORT bool currentTestFailed();

    Q_TESTLIB_EXPORT Qt::Key asciiToKey(char ascii);
    Q_TESTLIB_EXPORT char keyToAscii(Qt::Key key);

    Q_TESTLIB_EXPORT bool compare_helper(bool success, const char *msg, const char *file,
                                          int line);
    Q_TESTLIB_EXPORT bool compare_helper(bool success, const char *msg, char *val1, char *val2,
                                         const char *expected, const char *actual,
                                         const char *file, int line);
    Q_TESTLIB_EXPORT void qSleep(int ms);
    Q_TESTLIB_EXPORT void addColumnInternal(int id, const char *name);

    template <typename T>
    inline void addColumn(const char *name, T * = 0)
    {
        addColumnInternal(qMetaTypeId<T>(), name);
    }
    Q_TESTLIB_EXPORT QTestData &newRow(const char *dataTag);

    template <typename T>
    inline bool qCompare(T const &t1, T const &t2, const char *actual, const char *expected,
                        const char *file, int line)
    {
        return (t1 == t2)
            ? compare_helper(true, "COMPARE()", file, line)
            : compare_helper(false, "Compared values are not the same",
                             toString<T>(t1), toString<T>(t2), actual, expected, file, line);
    }

    template <>
    Q_TESTLIB_EXPORT bool qCompare<float>(float const &t1, float const &t2,
                    const char *actual, const char *expected, const char *file, int line);

    template <>
    Q_TESTLIB_EXPORT bool qCompare<double>(double const &t1, double const &t2,
                    const char *actual, const char *expected, const char *file, int line);

    inline bool compare_ptr_helper(const void *t1, const void *t2, const char *actual,
                                   const char *expected, const char *file, int line)
    {
        return (t1 == t2)
            ? compare_helper(true, "COMPARE()", file, line)
            : compare_helper(false, "Compared pointers are not the same",
                             toString(t1), toString(t2), actual, expected, file, line);
    }

    Q_TESTLIB_EXPORT bool compare_string_helper(const char *t1, const char *t2, const char *actual,
                                      const char *expected, const char *file, int line);

#ifndef qdoc
    QTEST_COMPARE_DECL(short)
    QTEST_COMPARE_DECL(ushort)
    QTEST_COMPARE_DECL(int)
    QTEST_COMPARE_DECL(uint)
    QTEST_COMPARE_DECL(long)
    QTEST_COMPARE_DECL(ulong)
    QTEST_COMPARE_DECL(qint64)
    QTEST_COMPARE_DECL(quint64)

    QTEST_COMPARE_DECL(float)
    QTEST_COMPARE_DECL(double)
    QTEST_COMPARE_DECL(char)
    QTEST_COMPARE_DECL(bool)
#endif

#ifndef QTEST_NO_SPECIALIZATIONS
    template <typename T1, typename T2>
    bool qCompare(T1 const &, T2 const &, const char *, const char *, const char *, int);

#if defined(QT_COORD_TYPE) || defined(QT_ARCH_ARM) || defined(QT_ARCH_WINDOWSCE) || defined(QT_NO_FPU)
    template <>
    inline bool qCompare<qreal, double>(qreal const &t1, double const &t2, const char *actual,
                                 const char *expected, const char *file, int line)
    {
        return qCompare<float>(float(t1), float(t2), actual, expected, file, line);
    }

    template <>
    inline bool qCompare<double, qreal>(double const &t1, qreal const &t2, const char *actual,
                                 const char *expected, const char *file, int line)
    {
        return qCompare<float>(float(t1), float(t2), actual, expected, file, line);
    }

#endif

    template <typename T>
    inline bool qCompare(const T *t1, const T *t2, const char *actual, const char *expected,
                        const char *file, int line)
    {
        return compare_ptr_helper(t1, t2, actual, expected, file, line);
    }
    template <typename T>
    inline bool qCompare(T *t1, T *t2, const char *actual, const char *expected,
                        const char *file, int line)
    {
        return compare_ptr_helper(t1, t2, actual, expected, file, line);
    }

    template <typename T1, typename T2>
    inline bool qCompare(const T1 *t1, const T2 *t2, const char *actual, const char *expected,
                        const char *file, int line)
    {
        return compare_ptr_helper(t1, static_cast<const T1 *>(t2), actual, expected, file, line);
    }
    template <typename T1, typename T2>
    inline bool qCompare(T1 *t1, T2 *t2, const char *actual, const char *expected,
                        const char *file, int line)
    {
        return compare_ptr_helper(const_cast<const T1 *>(t1),
                static_cast<const T1 *>(const_cast<const T2 *>(t2)), actual, expected, file, line);
    }
    template<>
    inline bool qCompare<char>(const char *t1, const char *t2, const char *actual,
                                       const char *expected, const char *file, int line)
    {
        return compare_string_helper(t1, t2, actual, expected, file, line);
    }
    template<>
    inline bool qCompare<char>(char *t1, char *t2, const char *actual, const char *expected,
                        const char *file, int line)
    {
        return compare_string_helper(t1, t2, actual, expected, file, line);
    }
#else  /* QTEST_NO_SPECIALIZATIONS */
    inline bool qCompare(const char *t1, const char *t2, const char *actual,
                         const char *expected, const char *file, int line)
    {
        return compare_string_helper(t1, t2, actual, expected, file, line);
    }

    inline bool qCompare(char *t1, char *t2, const char *actual, const char *expected,
                         const char *file, int line)
    {
        return compare_string_helper(t1, t2, actual, expected, file, line);
    }
#endif

    /* The next two specializations are for MSVC that shows problems with implicit
       conversions
     */
#ifndef QTEST_NO_SPECIALIZATIONS
    template<>
#endif
    inline bool qCompare(char *t1, const char *t2, const char *actual,
                         const char *expected, const char *file, int line)
    {
        return compare_string_helper(t1, t2, actual, expected, file, line);
    }
#ifndef QTEST_NO_SPECIALIZATIONS
    template<>
#endif
    inline bool qCompare(const char *t1, char *t2, const char *actual,
                         const char *expected, const char *file, int line)
    {
        return compare_string_helper(t1, t2, actual, expected, file, line);
    }

    template <class T>
    inline bool qTest(const T& actual, const char *elementName, const char *actualStr,
                     const char *expected, const char *file, int line)
    {
        return qCompare(actual, *static_cast<const T *>(QTest::qElementData(elementName,
                       qMetaTypeId<T>())), actualStr, expected, file, line);
    }
}

#undef QTEST_COMPARE_DECL

QT_END_NAMESPACE

QT_END_HEADER

#endif

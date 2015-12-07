/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
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

// Generated code, do not edit! Use generator at tools/qtconcurrent/generaterun/
#ifndef QTCONCURRENT_RUN_H
#define QTCONCURRENT_RUN_H

#include <QtCore/qglobal.h>

#ifndef QT_NO_CONCURRENT

#include <QtCore/qtconcurrentrunbase.h>
#include <QtCore/qtconcurrentstoredfunctioncall.h>

QT_BEGIN_HEADER
QT_BEGIN_NAMESPACE

QT_MODULE(Core)

#ifdef qdoc

namespace QtConcurrent {

    template <typename T>
    QFuture<T> run(Function function, ...);

} // namespace QtConcurrent

#else

namespace QtConcurrent {

template <typename T>
QFuture<T> run(T (*functionPointer)())
{
    return (new QT_TYPENAME SelectStoredFunctorCall0<T, T (*)()>::type(functionPointer))->start();
}
template <typename T, typename Param1, typename Arg1>
QFuture<T> run(T (*functionPointer)(Param1), const Arg1 &arg1)
{
    return (new QT_TYPENAME SelectStoredFunctorCall1<T, T (*)(Param1), Arg1>::type(functionPointer, arg1))->start();
}
template <typename T, typename Param1, typename Arg1, typename Param2, typename Arg2>
QFuture<T> run(T (*functionPointer)(Param1, Param2), const Arg1 &arg1, const Arg2 &arg2)
{
    return (new QT_TYPENAME SelectStoredFunctorCall2<T, T (*)(Param1, Param2), Arg1, Arg2>::type(functionPointer, arg1, arg2))->start();
}
template <typename T, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3>
QFuture<T> run(T (*functionPointer)(Param1, Param2, Param3), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3)
{
    return (new QT_TYPENAME SelectStoredFunctorCall3<T, T (*)(Param1, Param2, Param3), Arg1, Arg2, Arg3>::type(functionPointer, arg1, arg2, arg3))->start();
}
template <typename T, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4>
QFuture<T> run(T (*functionPointer)(Param1, Param2, Param3, Param4), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4)
{
    return (new QT_TYPENAME SelectStoredFunctorCall4<T, T (*)(Param1, Param2, Param3, Param4), Arg1, Arg2, Arg3, Arg4>::type(functionPointer, arg1, arg2, arg3, arg4))->start();
}
template <typename T, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4, typename Param5, typename Arg5>
QFuture<T> run(T (*functionPointer)(Param1, Param2, Param3, Param4, Param5), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5)
{
    return (new QT_TYPENAME SelectStoredFunctorCall5<T, T (*)(Param1, Param2, Param3, Param4, Param5), Arg1, Arg2, Arg3, Arg4, Arg5>::type(functionPointer, arg1, arg2, arg3, arg4, arg5))->start();
}

template <typename FunctionObject>
QFuture<typename FunctionObject::result_type> run(FunctionObject functionObject)
{
    return (new QT_TYPENAME SelectStoredFunctorCall0<QT_TYPENAME FunctionObject::result_type, FunctionObject>::type(functionObject))->start();
}
template <typename FunctionObject, typename Arg1>
QFuture<typename FunctionObject::result_type> run(FunctionObject functionObject, const Arg1 &arg1)
{
    return (new QT_TYPENAME SelectStoredFunctorCall1<QT_TYPENAME FunctionObject::result_type, FunctionObject, Arg1>::type(functionObject, arg1))->start();
}
template <typename FunctionObject, typename Arg1, typename Arg2>
QFuture<typename FunctionObject::result_type> run(FunctionObject functionObject, const Arg1 &arg1, const Arg2 &arg2)
{
    return (new QT_TYPENAME SelectStoredFunctorCall2<QT_TYPENAME FunctionObject::result_type, FunctionObject, Arg1, Arg2>::type(functionObject, arg1, arg2))->start();
}
template <typename FunctionObject, typename Arg1, typename Arg2, typename Arg3>
QFuture<typename FunctionObject::result_type> run(FunctionObject functionObject, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3)
{
    return (new QT_TYPENAME SelectStoredFunctorCall3<QT_TYPENAME FunctionObject::result_type, FunctionObject, Arg1, Arg2, Arg3>::type(functionObject, arg1, arg2, arg3))->start();
}
template <typename FunctionObject, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
QFuture<typename FunctionObject::result_type> run(FunctionObject functionObject, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4)
{
    return (new QT_TYPENAME SelectStoredFunctorCall4<QT_TYPENAME FunctionObject::result_type, FunctionObject, Arg1, Arg2, Arg3, Arg4>::type(functionObject, arg1, arg2, arg3, arg4))->start();
}
template <typename FunctionObject, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
QFuture<typename FunctionObject::result_type> run(FunctionObject functionObject, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5)
{
    return (new QT_TYPENAME SelectStoredFunctorCall5<QT_TYPENAME FunctionObject::result_type, FunctionObject, Arg1, Arg2, Arg3, Arg4, Arg5>::type(functionObject, arg1, arg2, arg3, arg4, arg5))->start();
}

template <typename FunctionObject>
QFuture<typename FunctionObject::result_type> run(FunctionObject *functionObject)
{
    return (new QT_TYPENAME SelectStoredFunctorPointerCall0<QT_TYPENAME FunctionObject::result_type, FunctionObject>::type(functionObject))->start();
}
template <typename FunctionObject, typename Arg1>
QFuture<typename FunctionObject::result_type> run(FunctionObject *functionObject, const Arg1 &arg1)
{
    return (new QT_TYPENAME SelectStoredFunctorPointerCall1<QT_TYPENAME FunctionObject::result_type, FunctionObject, Arg1>::type(functionObject, arg1))->start();
}
template <typename FunctionObject, typename Arg1, typename Arg2>
QFuture<typename FunctionObject::result_type> run(FunctionObject *functionObject, const Arg1 &arg1, const Arg2 &arg2)
{
    return (new QT_TYPENAME SelectStoredFunctorPointerCall2<QT_TYPENAME FunctionObject::result_type, FunctionObject, Arg1, Arg2>::type(functionObject, arg1, arg2))->start();
}
template <typename FunctionObject, typename Arg1, typename Arg2, typename Arg3>
QFuture<typename FunctionObject::result_type> run(FunctionObject *functionObject, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3)
{
    return (new QT_TYPENAME SelectStoredFunctorPointerCall3<QT_TYPENAME FunctionObject::result_type, FunctionObject, Arg1, Arg2, Arg3>::type(functionObject, arg1, arg2, arg3))->start();
}
template <typename FunctionObject, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
QFuture<typename FunctionObject::result_type> run(FunctionObject *functionObject, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4)
{
    return (new QT_TYPENAME SelectStoredFunctorPointerCall4<QT_TYPENAME FunctionObject::result_type, FunctionObject, Arg1, Arg2, Arg3, Arg4>::type(functionObject, arg1, arg2, arg3, arg4))->start();
}
template <typename FunctionObject, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
QFuture<typename FunctionObject::result_type> run(FunctionObject *functionObject, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5)
{
    return (new QT_TYPENAME SelectStoredFunctorPointerCall5<QT_TYPENAME FunctionObject::result_type, FunctionObject, Arg1, Arg2, Arg3, Arg4, Arg5>::type(functionObject, arg1, arg2, arg3, arg4, arg5))->start();
}

template <typename T, typename Class>
QFuture<T> run(const Class &object, T (Class::*fn)())
{
    return (new QT_TYPENAME SelectStoredMemberFunctionCall0<T, Class>::type(fn, object))->start();
}
template <typename T, typename Class, typename Param1, typename Arg1>
QFuture<T> run(const Class &object, T (Class::*fn)(Param1), const Arg1 &arg1)
{
    return (new QT_TYPENAME SelectStoredMemberFunctionCall1<T, Class, Param1, Arg1>::type(fn, object, arg1))->start();
}
template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2>
QFuture<T> run(const Class &object, T (Class::*fn)(Param1, Param2), const Arg1 &arg1, const Arg2 &arg2)
{
    return (new QT_TYPENAME SelectStoredMemberFunctionCall2<T, Class, Param1, Arg1, Param2, Arg2>::type(fn, object, arg1, arg2))->start();
}
template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3>
QFuture<T> run(const Class &object, T (Class::*fn)(Param1, Param2, Param3), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3)
{
    return (new QT_TYPENAME SelectStoredMemberFunctionCall3<T, Class, Param1, Arg1, Param2, Arg2, Param3, Arg3>::type(fn, object, arg1, arg2, arg3))->start();
}
template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4>
QFuture<T> run(const Class &object, T (Class::*fn)(Param1, Param2, Param3, Param4), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4)
{
    return (new QT_TYPENAME SelectStoredMemberFunctionCall4<T, Class, Param1, Arg1, Param2, Arg2, Param3, Arg3, Param4, Arg4>::type(fn, object, arg1, arg2, arg3, arg4))->start();
}
template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4, typename Param5, typename Arg5>
QFuture<T> run(const Class &object, T (Class::*fn)(Param1, Param2, Param3, Param4, Param5), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5)
{
    return (new QT_TYPENAME SelectStoredMemberFunctionCall5<T, Class, Param1, Arg1, Param2, Arg2, Param3, Arg3, Param4, Arg4, Param5, Arg5>::type(fn, object, arg1, arg2, arg3, arg4, arg5))->start();
}

template <typename T, typename Class>
QFuture<T> run(const Class &object, T (Class::*fn)() const)
{
    return (new QT_TYPENAME SelectStoredConstMemberFunctionCall0<T, Class>::type(fn, object))->start();
}
template <typename T, typename Class, typename Param1, typename Arg1>
QFuture<T> run(const Class &object, T (Class::*fn)(Param1) const, const Arg1 &arg1)
{
    return (new QT_TYPENAME SelectStoredConstMemberFunctionCall1<T, Class, Param1, Arg1>::type(fn, object, arg1))->start();
}
template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2>
QFuture<T> run(const Class &object, T (Class::*fn)(Param1, Param2) const, const Arg1 &arg1, const Arg2 &arg2)
{
    return (new QT_TYPENAME SelectStoredConstMemberFunctionCall2<T, Class, Param1, Arg1, Param2, Arg2>::type(fn, object, arg1, arg2))->start();
}
template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3>
QFuture<T> run(const Class &object, T (Class::*fn)(Param1, Param2, Param3) const, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3)
{
    return (new QT_TYPENAME SelectStoredConstMemberFunctionCall3<T, Class, Param1, Arg1, Param2, Arg2, Param3, Arg3>::type(fn, object, arg1, arg2, arg3))->start();
}
template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4>
QFuture<T> run(const Class &object, T (Class::*fn)(Param1, Param2, Param3, Param4) const, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4)
{
    return (new QT_TYPENAME SelectStoredConstMemberFunctionCall4<T, Class, Param1, Arg1, Param2, Arg2, Param3, Arg3, Param4, Arg4>::type(fn, object, arg1, arg2, arg3, arg4))->start();
}
template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4, typename Param5, typename Arg5>
QFuture<T> run(const Class &object, T (Class::*fn)(Param1, Param2, Param3, Param4, Param5) const, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5)
{
    return (new QT_TYPENAME SelectStoredConstMemberFunctionCall5<T, Class, Param1, Arg1, Param2, Arg2, Param3, Arg3, Param4, Arg4, Param5, Arg5>::type(fn, object, arg1, arg2, arg3, arg4, arg5))->start();
}

template <typename T, typename Class>
QFuture<T> run(Class *object, T (Class::*fn)())
{
    return (new QT_TYPENAME SelectStoredMemberFunctionPointerCall0<T, Class>::type(fn, object))->start();
}
template <typename T, typename Class, typename Param1, typename Arg1>
QFuture<T> run(Class *object, T (Class::*fn)(Param1), const Arg1 &arg1)
{
    return (new QT_TYPENAME SelectStoredMemberFunctionPointerCall1<T, Class, Param1, Arg1>::type(fn, object, arg1))->start();
}
template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2>
QFuture<T> run(Class *object, T (Class::*fn)(Param1, Param2), const Arg1 &arg1, const Arg2 &arg2)
{
    return (new QT_TYPENAME SelectStoredMemberFunctionPointerCall2<T, Class, Param1, Arg1, Param2, Arg2>::type(fn, object, arg1, arg2))->start();
}
template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3>
QFuture<T> run(Class *object, T (Class::*fn)(Param1, Param2, Param3), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3)
{
    return (new QT_TYPENAME SelectStoredMemberFunctionPointerCall3<T, Class, Param1, Arg1, Param2, Arg2, Param3, Arg3>::type(fn, object, arg1, arg2, arg3))->start();
}
template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4>
QFuture<T> run(Class *object, T (Class::*fn)(Param1, Param2, Param3, Param4), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4)
{
    return (new QT_TYPENAME SelectStoredMemberFunctionPointerCall4<T, Class, Param1, Arg1, Param2, Arg2, Param3, Arg3, Param4, Arg4>::type(fn, object, arg1, arg2, arg3, arg4))->start();
}
template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4, typename Param5, typename Arg5>
QFuture<T> run(Class *object, T (Class::*fn)(Param1, Param2, Param3, Param4, Param5), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5)
{
    return (new QT_TYPENAME SelectStoredMemberFunctionPointerCall5<T, Class, Param1, Arg1, Param2, Arg2, Param3, Arg3, Param4, Arg4, Param5, Arg5>::type(fn, object, arg1, arg2, arg3, arg4, arg5))->start();
}

template <typename T, typename Class>
QFuture<T> run(const Class *object, T (Class::*fn)() const)
{
    return (new QT_TYPENAME SelectStoredConstMemberFunctionPointerCall0<T, Class>::type(fn, object))->start();
}
template <typename T, typename Class, typename Param1, typename Arg1>
QFuture<T> run(const Class *object, T (Class::*fn)(Param1) const, const Arg1 &arg1)
{
    return (new QT_TYPENAME SelectStoredConstMemberFunctionPointerCall1<T, Class, Param1, Arg1>::type(fn, object, arg1))->start();
}
template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2>
QFuture<T> run(const Class *object, T (Class::*fn)(Param1, Param2) const, const Arg1 &arg1, const Arg2 &arg2)
{
    return (new QT_TYPENAME SelectStoredConstMemberFunctionPointerCall2<T, Class, Param1, Arg1, Param2, Arg2>::type(fn, object, arg1, arg2))->start();
}
template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3>
QFuture<T> run(const Class *object, T (Class::*fn)(Param1, Param2, Param3) const, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3)
{
    return (new QT_TYPENAME SelectStoredConstMemberFunctionPointerCall3<T, Class, Param1, Arg1, Param2, Arg2, Param3, Arg3>::type(fn, object, arg1, arg2, arg3))->start();
}
template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4>
QFuture<T> run(const Class *object, T (Class::*fn)(Param1, Param2, Param3, Param4) const, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4)
{
    return (new QT_TYPENAME SelectStoredConstMemberFunctionPointerCall4<T, Class, Param1, Arg1, Param2, Arg2, Param3, Arg3, Param4, Arg4>::type(fn, object, arg1, arg2, arg3, arg4))->start();
}
template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4, typename Param5, typename Arg5>
QFuture<T> run(const Class *object, T (Class::*fn)(Param1, Param2, Param3, Param4, Param5) const, const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5)
{
    return (new QT_TYPENAME SelectStoredConstMemberFunctionPointerCall5<T, Class, Param1, Arg1, Param2, Arg2, Param3, Arg3, Param4, Arg4, Param5, Arg5>::type(fn, object, arg1, arg2, arg3, arg4, arg5))->start();
}

} //namespace QtConcurrent

#endif // qdoc

QT_END_NAMESPACE
QT_END_HEADER

#endif // QT_NO_CONCURRENT

#endif

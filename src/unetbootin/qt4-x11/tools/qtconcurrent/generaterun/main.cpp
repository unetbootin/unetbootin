/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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
#include <QApplication>
#include <QDebug>
#include <QFile>

#include "codegenerator.h"
using namespace CodeGenerator;

const Item argument = "arg" + Counter();
const Item argumentRef = "&arg" + Counter();
const Item argumentType  = "Arg" + Counter();
const Item constArgumentType  = "const Arg" + Counter();
const Item parameterType  = "Param" + Counter();

Group argumentTypes(argumentType);          // expands to ",Arg1, Arg2, ..."
Group argumentTypesNoPrefix(argumentType);  // expands to "Arg1, Arg2, ..."
Group arguments(argument);                  // expands to ",arg1, arg2, ..."
Group argumentsNoPrefix(argument);          // expands to "arg1, arg2, ..."
Group parameterTypes(parameterType);        // expands to ",Param1, Param2, ..."
Group parameterTypesNoPrefix(parameterType); // expands to "Param1, Param2, ..."
Group typenameTypes("typename " + parameterType + ", typename " + argumentType); // expands to " ,typename Param1, typename Arg1, ..."
Group types(parameterType + ", " + argumentType); // expands to ", Param1, Arg1, ..."
Group functionParameters(constArgumentType + " " + argumentRef);
Group typenameArgumentTypes("typename " + argumentType);

Group initializers(argument + "(" + argument + ")");
Group classData(argumentType +" "  + argument + ";");
Group arglist(argument);
Group typeList(argumentTypes);

void init()
{
    argumentTypes.setPrefix(", ");
    arguments.setPrefix(", ");
    parameterTypes.setPrefix(", ");
    typenameTypes.setPrefix(", ");
    types.setPrefix(", ");
    functionParameters.setPrefix(", ");
    typenameArgumentTypes.setPrefix(", ");

    initializers.setPrefix(", ");
    classData.setSeparator(" ");
    classData.setPrefix("    ");
    arglist.setPrefix(", ");
    typeList.setPrefix(", ");
}


Item Line(Item item)
{
    return item + "\n";
}

Item generateRunFunctions(int repeats)
{
    Item functionPointerType = "T (*)(" + parameterTypesNoPrefix + ")";

    Item functionPointerParameter = "T (*functionPointer)(" + parameterTypesNoPrefix + ")";



    // plain functions
    Repeater functions = Line ("template <typename T" + typenameTypes + ">") +
                         Line ("QFuture<T> run(" + functionPointerParameter + functionParameters + ")")  +
                         Line("{") +
                         Line("    return (new QT_TYPENAME SelectStoredFunctorCall" + Counter() + "<T, " +
                                   functionPointerType + argumentTypes + ">::type(functionPointer" + arguments + "))->start();") +
                         Line("}");
    functions.setRepeatCount(repeats);

    // function objects by value
    Repeater functionObjects =  Line ("template <typename FunctionObject" + typenameArgumentTypes + ">") +
                                Line ("QFuture<typename FunctionObject::result_type> run(FunctionObject functionObject" + functionParameters + ")") +
                                Line("{") +
                                Line("    return (new QT_TYPENAME SelectStoredFunctorCall" + Counter() +
                                     "<QT_TYPENAME FunctionObject::result_type, FunctionObject" +
                                     argumentTypes + ">::type(functionObject" + arguments + "))->start();") +
                                Line("}");
    functionObjects.setRepeatCount(repeats);

    // function objects by pointer
    Repeater functionObjectsPointer =  Line ("template <typename FunctionObject" + typenameArgumentTypes + ">") +
                                Line ("QFuture<typename FunctionObject::result_type> run(FunctionObject *functionObject" + functionParameters + ")") +
                                Line("{") +
                                Line("    return (new QT_TYPENAME SelectStoredFunctorPointerCall" + Counter() +
                                     "<QT_TYPENAME FunctionObject::result_type, FunctionObject" +
                                     argumentTypes + ">::type(functionObject" + arguments + "))->start();") +
                                Line("}");
    functionObjectsPointer.setRepeatCount(repeats);

    // member functions by value
    Repeater memberFunction =  Line ("template <typename T, typename Class" + typenameTypes + ">") +
                                Line ("QFuture<T> run(const Class &object, T (Class::*fn)(" + parameterTypesNoPrefix  + ")" + functionParameters + ")") +
                                Line("{") +
                                Line("    return (new QT_TYPENAME SelectStoredMemberFunctionCall" + Counter() +
                                     "<T, Class" +
                                     types + ">::type(fn, object" + arguments + "))->start();") +
                                Line("}");
    memberFunction.setRepeatCount(repeats);

    // const member functions by value
    Repeater constMemberFunction =  Line ("template <typename T, typename Class" + typenameTypes + ">") +
                                Line ("QFuture<T> run(const Class &object, T (Class::*fn)(" + parameterTypesNoPrefix  + ") const" + functionParameters + ")") +
                                Line("{") +
                                Line("    return (new QT_TYPENAME SelectStoredConstMemberFunctionCall" + Counter() +
                                     "<T, Class" +
                                     types + ">::type(fn, object" + arguments + "))->start();") +
                                Line("}");
    constMemberFunction.setRepeatCount(repeats);

    // member functions by class pointer
    Repeater memberFunctionPointer =  Line ("template <typename T, typename Class" + typenameTypes + ">") +
                                Line ("QFuture<T> run(Class *object, T (Class::*fn)(" + parameterTypesNoPrefix  + ")" + functionParameters + ")") +
                                Line("{") +
                                Line("    return (new QT_TYPENAME SelectStoredMemberFunctionPointerCall" + Counter() +
                                     "<T, Class" +
                                     types + ">::type(fn, object" + arguments + "))->start();") +
                                Line("}");
    memberFunctionPointer.setRepeatCount(repeats);

    // const member functions by class pointer
    Repeater constMemberFunctionPointer =  Line ("template <typename T, typename Class" + typenameTypes + ">") +
                                Line ("QFuture<T> run(const Class *object, T (Class::*fn)(" + parameterTypesNoPrefix  + ") const" + functionParameters + ")") +
                                Line("{") +
                                Line("    return (new QT_TYPENAME SelectStoredConstMemberFunctionPointerCall" + Counter() +
                                     "<T, Class" +
                                     types + ">::type(fn, object" + arguments + "))->start();") +
                                Line("}");
    constMemberFunctionPointer.setRepeatCount(repeats);


    Item interfaceFunctionPointerType = "void (*)(QFutureInterface<T> &" + argumentTypes + ")";
    Item interfaceFunctionPointerParameter = "void (*functionPointer)(QFutureInterface<T> &" + argumentTypes + ")";
/*
    // QFutureInterface functions
    Repeater interfaceFunctions = Line ("template <typename T" + typenameTypes + ">") +
                         Line ("QFuture<T> run(" + interfaceFunctionPointerParameter + functionParameters + ")")  +
                         Line("{") +
                         Line("    return (new StoredInterfaceFunctionCall" + Counter() + "<T, " +
                                   interfaceFunctionPointerType + typenameArgumentTypes + ">(functionPointer" + arguments + "))->start();") +
                         Line("}");
    functions.setRepeatCount(repeats);
    interfaceFunctions.setRepeatCount(repeats);

    // member functions by class pointer
    Repeater interfaceMemberFunction =  Line ("template <typename Class, typename T" + typenameTypes + ">") +
                                Line ("QFuture<T> run(void (Class::*fn)(QFutureInterface<T> &), Class *object" + functionParameters + ")") +
                                Line("{") +
                                Line("    return (new StoredInterfaceMemberFunctionCall" + Counter() +
                                     "<T, void (Class::*)(QFutureInterface<T> &), Class" +
                                     typenameArgumentTypes + ">(fn, object" + arguments + "))->start();") +
                                Line("}");
    memberFunctionPointer.setRepeatCount(repeats);
*/
    return functions + Line("") + functionObjects + Line("") + functionObjectsPointer + Line("")
          + memberFunction + Line("") + constMemberFunction + Line("")
          + memberFunctionPointer + Line("") + constMemberFunctionPointer + Line("")
  /*        + interfaceFunctions + Line("") + interfaceMemberFunction + Line("")*/
    ;
}


Item functions(Item className, Item functorType, Item callLine)
{
    return
    Line("template <typename T, typename FunctionPointer" +  typenameArgumentTypes + ">") +
    Line("struct " + className +  Counter() +": public RunFunctionTask<T>") +
    Line("{") +
    Line("    inline " + className + Counter() + "(" + functorType + " function" + functionParameters +")") +
    Line("      : function(function)" + initializers + " {}") +
    Line("    void runFunctor() {" + callLine + argumentsNoPrefix + "); }") +
    Line("    " + functorType + " function;") +
    Line(       classData) +
    Line("};") +
    Line("");
}

Item functionSelector(Item classNameBase)
{
    return
    Line("template <typename T, typename FunctionPointer" + typenameArgumentTypes + ">") +
    Line("struct Select" + classNameBase + Counter()) +
    Line("{") +
    Line("    typedef typename SelectSpecialization<T>::template") +
    Line("        Type<" + classNameBase + Counter() + "    <T, FunctionPointer" + argumentTypes + ">,") +
    Line("             Void" + classNameBase + Counter() + "<T, FunctionPointer" + argumentTypes + "> >::type type;") +
    Line("};");
}

Item memberFunctions(Item className, Item constFunction, Item objectArgument, Item objectMember, Item callLine)
{
    return
    Line("template <typename T, typename Class"  + typenameTypes + ">") +
    Line("class " + className + Counter() + " : public RunFunctionTask<T>") +
    Line("{") +
    Line("public:")+
    Line("    " + className + Counter() + "(T (Class::*fn)(" + parameterTypesNoPrefix  + ") " + constFunction + ", " + objectArgument + functionParameters + ")") +
    Line("    : fn(fn), object(object)" + initializers + "{ }" ) +
    Line("")+
    Line("    void runFunctor()")+
    Line("    {")+
    Line("        " + callLine + argumentsNoPrefix +  ");")+
    Line("    }")+
    Line("private:")+
    Line("    T (Class::*fn)(" + parameterTypesNoPrefix  + ")" + constFunction + ";")+
    Line("    " + objectMember + ";") +
    Line(        classData) +
    Line("};");
}

Item memberFunctionSelector(Item classNameBase)
{
    return
    Line("template <typename T, typename Class" + typenameTypes + ">") +
    Line("struct Select" + classNameBase + Counter()) +
    Line("{") +
    Line("    typedef typename SelectSpecialization<T>::template") +
    Line("        Type<" + classNameBase + Counter() + "    <T, Class" + types + ">,") +
    Line("             Void" + classNameBase + Counter() + "<T, Class" + types + "> >::type type;") +
    Line("};");
}

Item generateSFCs(int repeats)
{

    Item functionPointerTypedef = "typedef void (*FunctionPointer)(" + argumentTypesNoPrefix + ");";

    Repeater dataStructures =

    // Function objects by value
    functions(Item("StoredFunctorCall"), Item("FunctionPointer"), Item(" this->result = function(")) +
    functions(Item("VoidStoredFunctorCall"), Item("FunctionPointer"), Item(" function(")) +
    functionSelector(Item("StoredFunctorCall")) +

    // Function objects by pointer
    functions(Item("StoredFunctorPointerCall"), Item("FunctionPointer *"), Item(" this->result =(*function)(")) +
    functions(Item("VoidStoredFunctorPointerCall"), Item("FunctionPointer *"), Item("(*function)(")) +
    functionSelector(Item("StoredFunctorPointerCall")) +

    // Member functions by value
    memberFunctions(Item("StoredMemberFunctionCall"), Item(""), Item("const Class &object"), Item("Class object"), Item("this->result = (object.*fn)(")) +
    memberFunctions(Item("VoidStoredMemberFunctionCall"), Item(""), Item("const Class &object"), Item("Class object"), Item("(object.*fn)(")) +
    memberFunctionSelector(Item("StoredMemberFunctionCall")) +

    // Const Member functions by value
    memberFunctions(Item("StoredConstMemberFunctionCall"), Item("const"), Item("const Class &object"), Item("const Class object"), Item("this->result = (object.*fn)(")) +
    memberFunctions(Item("VoidStoredConstMemberFunctionCall"), Item("const"), Item("const Class &object"), Item("const Class object"), Item("(object.*fn)(")) +
    memberFunctionSelector(Item("StoredConstMemberFunctionCall")) +

    // Member functions by pointer
    memberFunctions(Item("StoredMemberFunctionPointerCall"), Item(""), Item("Class *object"), Item("Class *object"), Item("this->result = (object->*fn)(")) +
    memberFunctions(Item("VoidStoredMemberFunctionPointerCall"), Item(""), Item("Class *object"), Item("Class *object"), Item("(object->*fn)(")) +
    memberFunctionSelector(Item("StoredMemberFunctionPointerCall")) +

    // const member functions by pointer
    memberFunctions(Item("StoredConstMemberFunctionPointerCall"), Item("const"), Item("Class const *object"), Item("Class const *object"), Item("this->result = (object->*fn)(")) +
    memberFunctions(Item("VoidStoredConstMemberFunctionPointerCall"), Item("const"), Item("Class const *object"), Item("Class const *object"), Item("(object->*fn)(")) +
    memberFunctionSelector(Item("StoredConstMemberFunctionPointerCall"));

    dataStructures.setRepeatCount(repeats);
    return dataStructures;
}

void writeFile(QString fileName, QByteArray contents)
{
    QFile runFile(fileName);
    if (runFile.open(QIODevice::WriteOnly) == false) {
        qDebug() << "Write to" << fileName << "failed";
        return;
    }

    runFile.write(contents);
    runFile.close();
    qDebug() << "Write to" << fileName << "Ok";
}

Item dollarQuote(Item item)
{
    return Item("$") + item + Item("$");
}

int main()
{
    const int repeats = 6;
    init();
    Item run =  (
                       Line("/****************************************************************************") +
                       Line("**") +
                       Line("** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).") +
                       Line("** Contact: Qt Software Information (qt-info@nokia.com)") +
                       Line("**") +
                       Line("** This file is part of the " + dollarQuote("MODULE") + " of the Qt Toolkit.") +
                       Line("**") +
                       Line("** " + dollarQuote("TROLLTECH_DUAL_LICENSE")) +
                       Line("**") +
                       Line("****************************************************************************/") +
                       Line("") +
                       Line("// Generated code, do not edit! Use generator at tools/qtconcurrent/generaterun/") +
                       Line("#ifndef QTCONCURRENT_RUN_H") +
                       Line("#define QTCONCURRENT_RUN_H") +
                       Line("") +
                       Line("#ifndef QT_NO_CONCURRENT") +
                       Line("") +
                       Line("#include <QtCore/qtconcurrentrunbase.h>") +
                       Line("#include <QtCore/qtconcurrentstoredfunctioncall.h>") +
                       Line("") +
                       Line("QT_BEGIN_HEADER") +
                       Line("QT_BEGIN_NAMESPACE") +
                       Line("") +
                       Line("QT_MODULE(Core)") +
                       Line("") +
                       Line("#ifdef qdoc") +
                       Line("") +
                       Line("namespace QtConcurrent {") +
                       Line("") +
                       Line("    template <typename T>") +
                       Line("    QFuture<T> run(Function function, ...);") +
                       Line("") +
                       Line("} // namespace QtConcurrent") +
                       Line("") +
                       Line("#else") +
                       Line("") +
                       Line("namespace QtConcurrent {") +
                       Line("") +
                       generateRunFunctions(repeats) +
                       Line("} //namespace QtConcurrent") +
                       Line("") +
                       Line("#endif // qdoc") +
                       Line("") +
                       Line("QT_END_NAMESPACE") +
                       Line("QT_END_HEADER") +
                       Line("") +
                       Line("#endif")
                      );

    writeFile("../../../src/corelib/concurrent/qtconcurrentrun.h", run.generate());

    Item storedFunctionCall = (
                                     Line("/****************************************************************************") +
                                     Line("**") +
                                     Line("** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).") +
                                     Line("** Contact: Qt Software Information (qt-info@nokia.com)") +
                                     Line("**") +
                                     Line("** This file is part of the " + dollarQuote("MODULE") + " of the Qt Toolkit.") +
                                     Line("**") +
                                     Line("** " + dollarQuote("TROLLTECH_DUAL_LICENSE")) +
                                     Line("**") +
                                     Line("****************************************************************************/") +
                                     Line("") +
                                     Line("// Generated code, do not edit! Use generator at tools/qtconcurrent/generaterun/") +
                                     Line("#ifndef QTCONCURRENT_STOREDFUNCTIONCALL_H") +
                                     Line("#define QTCONCURRENT_STOREDFUNCTIONCALL_H") +
                                     Line("") +
                                     Line("#include <QtCore/qglobal.h>") +
                                     Line("") +
                                     Line("#ifndef QT_NO_CONCURRENT") +
                                     Line("#include <QtCore/qtconcurrentrunbase.h>") +
                                     Line("") +
                                     Line("QT_BEGIN_HEADER") +
                                     Line("QT_BEGIN_NAMESPACE") +
                                     Line("") +
                                     Line("QT_MODULE(Core)") +
                                     Line("") +
                                     Line("#ifndef qdoc") +
                                     Line("") +
                                     Line("namespace QtConcurrent {") +
                                     generateSFCs(repeats) +
                                     Line("} //namespace QtConcurrent") +
                                     Line("") +
                                     Line("#endif // qdoc") +
                                     Line("") +
                                     Line("QT_END_NAMESPACE") +
                                     Line("QT_END_HEADER") +
                                     Line("") +
                                     Line("#endif // QT_NO_CONCURRENT") +
                                     Line("") +
                                     Line("#endif")
                                    );

    writeFile("../../../src/corelib/concurrent/qtconcurrentstoredfunctioncall.h", storedFunctionCall.generate());
}



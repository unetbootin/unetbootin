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

#include "generator.h"
#include "outputrevision.h"
#include "utils.h"
#include <QtCore/qmetatype.h>
#include <stdio.h>

QT_BEGIN_NAMESPACE

// if the flags change, you MUST to change it in qmetaobject.cpp too
enum PropertyFlags  {
    Invalid = 0x00000000,
    Readable = 0x00000001,
    Writable = 0x00000002,
    Resettable = 0x00000004,
    EnumOrFlag = 0x00000008,
    StdCppSet = 0x00000100,
//     Override = 0x00000200,
    Designable = 0x00001000,
    ResolveDesignable = 0x00002000,
    Scriptable = 0x00004000,
    ResolveScriptable = 0x00008000,
    Stored = 0x00010000,
    ResolveStored = 0x00020000,
    Editable = 0x00040000,
    ResolveEditable = 0x00080000,
    User = 0x00100000,
    ResolveUser = 0x00200000,
    Notify = 0x00400000
};
enum MethodFlags {
    AccessPrivate = 0x00,
    AccessProtected = 0x01,
    AccessPublic = 0x02,
    MethodMethod = 0x00,
    MethodSignal = 0x04,
    MethodSlot = 0x08,
    MethodConstructor = 0x0c,
    MethodCompatibility = 0x10,
    MethodCloned = 0x20,
    MethodScriptable = 0x40
};

uint qvariant_nameToType(const char* name)
{
    if (!name)
        return 0;

    if (strcmp(name, "QVariant") == 0)
        return 0xffffffff;
    if (strcmp(name, "QCString") == 0)
        return QMetaType::QByteArray;
    if (strcmp(name, "Q_LLONG") == 0)
        return QMetaType::LongLong;
    if (strcmp(name, "Q_ULLONG") == 0)
        return QMetaType::ULongLong;
    if (strcmp(name, "QIconSet") == 0)
        return QMetaType::QIcon;

    uint tp = QMetaType::type(name);
    return tp < QMetaType::User ? tp : 0;
}

/*
  Returns true if the type is a QVariant types.
*/
bool isVariantType(const char* type)
{
    return qvariant_nameToType(type) != 0;
}

Generator::Generator(ClassDef *classDef, const QList<QByteArray> &metaTypes, FILE *outfile)
    : out(outfile), cdef(classDef), metaTypes(metaTypes)
{
    if (cdef->superclassList.size())
        purestSuperClass = cdef->superclassList.first().first;
}

static inline int lengthOfEscapeSequence(const QByteArray &s, int i)
{
    if (s.at(i) != '\\' || i >= s.length() - 1)
        return 1;
    const int startPos = i;
    ++i;
    char ch = s.at(i);
    if (ch == 'x') {
        ++i;
        while (i < s.length() && is_hex_char(s.at(i)))
            ++i;
    } else if (is_octal_char(ch)) {
        while (i < startPos + 4
               && i < s.length()
               && is_octal_char(s.at(i))) {
            ++i;
        }
    } else { // single character escape sequence
        i = qMin(i + 1, s.length());
    }
    return i - startPos;
}

int Generator::strreg(const char *s)
{
    int idx = 0;
    if (!s)
        s = "";
    for (int i = 0; i < strings.size(); ++i) {
        const QByteArray &str = strings.at(i);
        if (str == s)
            return idx;
        idx += str.length() + 1;
        for (int i = 0; i < str.length(); ++i) {
            if (str.at(i) == '\\') {
                int cnt = lengthOfEscapeSequence(str, i) - 1;
                idx -= cnt;
                i += cnt;
            }
        }
    }
    strings.append(s);
    return idx;
}

void Generator::generateCode()
{
    bool isQt = (cdef->classname == "Qt");
    bool isQObject = (cdef->classname == "QObject");
    bool isConstructible = !cdef->constructorList.isEmpty();

//
// build the data array
//
    int i = 0;


    // filter out undeclared enumerators and sets
    {
        QList<EnumDef> enumList;
        for (i = 0; i < cdef->enumList.count(); ++i) {
            EnumDef def = cdef->enumList.at(i);
            if (cdef->enumDeclarations.contains(def.name)) {
                enumList += def;
            }
            QByteArray alias = cdef->flagAliases.value(def.name);
            if (cdef->enumDeclarations.contains(alias)) {
                def.name = alias;
                enumList += def;
            }
        }
        cdef->enumList = enumList;
    }


    QByteArray qualifiedClassNameIdentifier = cdef->qualified;
    qualifiedClassNameIdentifier.replace(':', '_');

    int index = 12;
    fprintf(out, "static const uint qt_meta_data_%s[] = {\n", qualifiedClassNameIdentifier.constData());
    fprintf(out, "\n // content:\n");
    fprintf(out, "    %4d,       // revision\n", 2);
    fprintf(out, "    %4d,       // classname\n", strreg(cdef->qualified));
    fprintf(out, "    %4d, %4d, // classinfo\n", cdef->classInfoList.count(), cdef->classInfoList.count() ? index : 0);
    index += cdef->classInfoList.count() * 2;

    int methodCount = cdef->signalList.count() + cdef->slotList.count() + cdef->methodList.count();
    fprintf(out, "    %4d, %4d, // methods\n", methodCount, methodCount ? index : 0);
    index += methodCount * 5;
    fprintf(out, "    %4d, %4d, // properties\n", cdef->propertyList.count(), cdef->propertyList.count() ? index : 0);
    index += cdef->propertyList.count() * 3;
    if(cdef->notifyableProperties)
        index += cdef->propertyList.count();
    fprintf(out, "    %4d, %4d, // enums/sets\n", cdef->enumList.count(), cdef->enumList.count() ? index : 0);

    int enumsIndex = index;
    for (i = 0; i < cdef->enumList.count(); ++i)
        index += 4 + (cdef->enumList.at(i).values.count() * 2);
    fprintf(out, "    %4d, %4d, // constructors\n", isConstructible ? cdef->constructorList.count() : 0,
            isConstructible ? index : 0);

//
// Build classinfo array
//
    generateClassInfos();

//
// Build signals array first, otherwise the signal indices would be wrong
//
    generateFunctions(cdef->signalList, "signal", MethodSignal);

//
// Build slots array
//
    generateFunctions(cdef->slotList, "slot", MethodSlot);

//
// Build method array
//
    generateFunctions(cdef->methodList, "method", MethodMethod);


//
// Build property array
//
    generateProperties();

//
// Build enums array
//
    generateEnums(enumsIndex);

//
// Build constructors array
//
    if (isConstructible)
        generateFunctions(cdef->constructorList, "constructor", MethodConstructor);

//
// Terminate data array
//
    fprintf(out, "\n       0        // eod\n};\n\n");

//
// Build stringdata array
//
    fprintf(out, "static const char qt_meta_stringdata_%s[] = {\n", qualifiedClassNameIdentifier.constData());
    fprintf(out, "    \"");
    int col = 0;
    int len = 0;
    for (i = 0; i < strings.size(); ++i) {
        QByteArray s = strings.at(i);
        len = s.length();
        if (col && col + len >= 72) {
            fprintf(out, "\"\n    \"");
            col = 0;
        } else if (len && s.at(0) >= '0' && s.at(0) <= '9') {
            fprintf(out, "\"\"");
            len += 2;
        }
        int idx = 0;
        while (idx < s.length()) {
            if (idx > 0) {
                col = 0;
                fprintf(out, "\"\n    \"");
            }
            int spanLen = qMin(70, s.length() - idx);
            // don't cut escape sequences at the end of a line
            int backSlashPos = s.lastIndexOf('\\', idx + spanLen - 1);
            if (backSlashPos >= idx) {
                int escapeLen = lengthOfEscapeSequence(s, backSlashPos);
                spanLen = qBound(spanLen, backSlashPos + escapeLen - idx, s.length() - idx);
            }
            fwrite(s.constData() + idx, 1, spanLen, out);
            idx += spanLen;
            col += spanLen;
        }

        fputs("\\0", out);
        col += len + 2;
    }
    fprintf(out, "\"\n};\n\n");


//
// Generate internal qt_static_metacall() function
//
    if (isConstructible)
        generateStaticMetacall(qualifiedClassNameIdentifier);

//
// Build extra array
//
    QList<QByteArray> extraList;
    for (int i = 0; i < cdef->propertyList.count(); ++i) {
        const PropertyDef &p = cdef->propertyList.at(i);
        if (!isVariantType(p.type) && !metaTypes.contains(p.type)) {
            int s = p.type.lastIndexOf("::");
            if (s > 0) {
                QByteArray scope = p.type.left(s);
                if (scope != "Qt" && scope != cdef->classname && !extraList.contains(scope))
                    extraList += scope;
            }
        }
    }
    if (!extraList.isEmpty()) {
        fprintf(out, "static const QMetaObject *qt_meta_extradata_%s[] = {\n    ", qualifiedClassNameIdentifier.constData());
        for (int i = 0; i < extraList.count(); ++i) {
            if (i)
                fprintf(out, ",\n    ");
            fprintf(out, "    &%s::staticMetaObject", extraList.at(i).constData());
        }
        fprintf(out, ",0\n};\n\n");
    }

    if (isConstructible || !extraList.isEmpty()) {
        fprintf(out, "static const QMetaObjectExtraData qt_meta_extradata2_%s = {\n    ",
                qualifiedClassNameIdentifier.constData());
        if (extraList.isEmpty())
            fprintf(out, "0, ");
        else
            fprintf(out, "qt_meta_extradata_%s, ", qualifiedClassNameIdentifier.constData());
        if (!isConstructible)
            fprintf(out, "0");
        else
            fprintf(out, "%s_qt_static_metacall", qualifiedClassNameIdentifier.constData());
        fprintf(out, " \n};\n\n");
    }

//
// Finally create and initialize the static meta object
//

    if (isQt)
        fprintf(out, "const QMetaObject QObject::staticQtMetaObject = {\n");
    else
        fprintf(out, "const QMetaObject %s::staticMetaObject = {\n", cdef->qualified.constData());

    if (isQObject)
        fprintf(out, "    { 0, ");
    else if (cdef->superclassList.size())
        fprintf(out, "    { &%s::staticMetaObject, ", purestSuperClass.constData());
    else
        fprintf(out, "    { 0, ");
    fprintf(out, "qt_meta_stringdata_%s,\n      qt_meta_data_%s, ",
             qualifiedClassNameIdentifier.constData(), qualifiedClassNameIdentifier.constData());
    if (!isConstructible && extraList.isEmpty())
        fprintf(out, "0 }\n");
    else
        fprintf(out, "&qt_meta_extradata2_%s }\n", qualifiedClassNameIdentifier.constData());
    fprintf(out, "};\n");

    if (isQt || !cdef->hasQObject)
        return;

    fprintf(out, "\nconst QMetaObject *%s::metaObject() const\n{\n    return &staticMetaObject;\n}\n",
            cdef->qualified.constData());
//
// Generate smart cast function
//
    fprintf(out, "\nvoid *%s::qt_metacast(const char *_clname)\n{\n", cdef->qualified.constData());
    fprintf(out, "    if (!_clname) return 0;\n");
    fprintf(out, "    if (!strcmp(_clname, qt_meta_stringdata_%s))\n"
                  "        return static_cast<void*>(const_cast< %s*>(this));\n",
            qualifiedClassNameIdentifier.constData(), cdef->classname.constData());
    for (int i = 1; i < cdef->superclassList.size(); ++i) { // for all superclasses but the first one
        if (cdef->superclassList.at(i).second == FunctionDef::Private)
            continue;
        const char *cname = cdef->superclassList.at(i).first;
        fprintf(out, "    if (!strcmp(_clname, \"%s\"))\n        return static_cast< %s*>(const_cast< %s*>(this));\n",
                cname, cname, cdef->classname.constData());
    }
    for (int i = 0; i < cdef->interfaceList.size(); ++i) {
        const QList<ClassDef::Interface> &iface = cdef->interfaceList.at(i);
        for (int j = 0; j < iface.size(); ++j) {
            fprintf(out, "    if (!strcmp(_clname, %s))\n        return ", iface.at(j).interfaceId.constData());
            for (int k = j; k >= 0; --k)
                fprintf(out, "static_cast< %s*>(", iface.at(k).className.constData());
            fprintf(out, "const_cast< %s*>(this)%s;\n",
                    cdef->classname.constData(), QByteArray(j+1, ')').constData());
        }
    }
    if (!purestSuperClass.isEmpty() && !isQObject) {
        QByteArray superClass = purestSuperClass;
        // workaround for VC6
        if (superClass.contains("::")) {
            fprintf(out, "    typedef %s QMocSuperClass;\n", superClass.constData());
            superClass = "QMocSuperClass";
        }
        fprintf(out, "    return %s::qt_metacast(_clname);\n", superClass.constData());
    } else {
        fprintf(out, "    return 0;\n");
    }
    fprintf(out, "}\n");

//
// Generate internal qt_metacall()  function
//
    generateMetacall();

//
// Generate internal signal functions
//
    for (int signalindex = 0; signalindex < cdef->signalList.size(); ++signalindex)
        generateSignal(&cdef->signalList[signalindex], signalindex);
}


void Generator::generateClassInfos()
{
    if (cdef->classInfoList.isEmpty())
        return;

    fprintf(out, "\n // classinfo: key, value\n");

    for (int i = 0; i < cdef->classInfoList.size(); ++i) {
        const ClassInfoDef &c = cdef->classInfoList.at(i);
        fprintf(out, "    %4d, %4d,\n", strreg(c.name), strreg(c.value));
    }
}

void Generator::generateFunctions(QList<FunctionDef>& list, const char *functype, int type)
{
    if (list.isEmpty())
        return;
    fprintf(out, "\n // %ss: signature, parameters, type, tag, flags\n", functype);

    for (int i = 0; i < list.count(); ++i) {
        const FunctionDef &f = list.at(i);

        QByteArray sig = f.name + '(';
        QByteArray arguments;

        for (int j = 0; j < f.arguments.count(); ++j) {
            const ArgumentDef &a = f.arguments.at(j);
            if (j) {
                sig += ",";
                arguments += ",";
            }
            sig += a.normalizedType;
            arguments += a.name;
        }
        sig += ')';

        char flags = type;
        if (f.access == FunctionDef::Private)
            flags |= AccessPrivate;
        else if (f.access == FunctionDef::Public)
            flags |= AccessPublic;
        else if (f.access == FunctionDef::Protected)
            flags |= AccessProtected;
        if (f.access == FunctionDef::Private)
            flags |= AccessPrivate;
        else if (f.access == FunctionDef::Public)
            flags |= AccessPublic;
        else if (f.access == FunctionDef::Protected)
            flags |= AccessProtected;
        if (f.isCompat)
            flags |= MethodCompatibility;
        if (f.wasCloned)
            flags |= MethodCloned;
        if (f.isScriptable)
            flags |= MethodScriptable;
        fprintf(out, "    %4d, %4d, %4d, %4d, 0x%02x,\n", strreg(sig),
                strreg(arguments), strreg(f.normalizedType), strreg(f.tag), flags);
    }
}

void Generator::generateProperties()
{
    //
    // specify get function, for compatibiliy we accept functions
    // returning pointers, or const char * for QByteArray.
    //
    for (int i = 0; i < cdef->propertyList.count(); ++i) {
        PropertyDef &p = cdef->propertyList[i];
        if (p.read.isEmpty())
            continue;
        for (int j = 0; j < cdef->publicList.count(); ++j) {
            const FunctionDef &f = cdef->publicList.at(j);
            if (f.name != p.read)
                continue;
            if (!f.isConst) // get  functions must be const
                continue;
            if (f.arguments.size()) // and must not take any arguments
                continue;
            PropertyDef::Specification spec = PropertyDef::ValueSpec;
            QByteArray tmp = f.normalizedType;
            if (p.type == "QByteArray" && tmp == "const char *")
                    tmp = "QByteArray";
            if (tmp.left(6) == "const ")
                tmp = tmp.mid(6);
            if (p.type != tmp && tmp.endsWith('*')) {
                tmp.chop(1);
                spec = PropertyDef::PointerSpec;
            } else if (f.type.name.endsWith('&')) { // raw type, not normalized type
                spec = PropertyDef::ReferenceSpec;
            }
            if (p.type != tmp)
                continue;
            p.gspec = spec;
            break;
        }
        if(!p.notify.isEmpty()) {
            int notifyId = -1;
            for (int j = 0; j < cdef->signalList.count(); ++j) {
                const FunctionDef &f = cdef->signalList.at(j);
                if(f.name != p.notify) {
                    continue;
                } else {
                    notifyId = j /* Signal indexes start from 0 */;
                    break;
                }
            }
            p.notifyId = notifyId;
        }
    }

    //
    // Create meta data
    //

    if (cdef->propertyList.count())
        fprintf(out, "\n // properties: name, type, flags\n");
    for (int i = 0; i < cdef->propertyList.count(); ++i) {
        const PropertyDef &p = cdef->propertyList.at(i);
        uint flags = Invalid;
        if (!isVariantType(p.type)) {
            flags |= EnumOrFlag;
        } else {
            flags |= qvariant_nameToType(p.type) << 24;
        }
        if (!p.read.isEmpty())
            flags |= Readable;
        if (!p.write.isEmpty()) {
            flags |= Writable;
            if (p.stdCppSet())
                flags |= StdCppSet;
        }
        if (!p.reset.isEmpty())
            flags |= Resettable;

//         if (p.override)
//             flags |= Override;

        if (p.designable.isEmpty())
            flags |= ResolveDesignable;
        else if (p.designable != "false")
            flags |= Designable;

        if (p.scriptable.isEmpty())
            flags |= ResolveScriptable;
        else if (p.scriptable != "false")
            flags |= Scriptable;

        if (p.stored.isEmpty())
            flags |= ResolveStored;
        else if (p.stored != "false")
            flags |= Stored;

        if (p.editable.isEmpty())
            flags |= ResolveEditable;
        else if (p.editable != "false")
            flags |= Editable;

        if (p.user.isEmpty())
            flags |= ResolveUser;
        else if (p.user != "false")
            flags |= User;

        if (p.notifyId != -1)
            flags |= Notify;

        fprintf(out, "    %4d, %4d, 0x%.8x,\n",
                 strreg(p.name),
                 strreg(p.type),
                 flags);
    }

    if(cdef->notifyableProperties) {
        fprintf(out, "\n // properties: notify_signal_id\n");
        for (int i = 0; i < cdef->propertyList.count(); ++i) {
            const PropertyDef &p = cdef->propertyList.at(i);
            if(p.notifyId == -1)
                fprintf(out, "    %4d,\n",
                        0);
            else
                fprintf(out, "    %4d,\n",
                        p.notifyId);
        }
    }
}

void Generator::generateEnums(int index)
{
    if (cdef->enumDeclarations.isEmpty())
        return;

    fprintf(out, "\n // enums: name, flags, count, data\n");
    index += 4 * cdef->enumList.count();
    int i;
    for (i = 0; i < cdef->enumList.count(); ++i) {
        const EnumDef &e = cdef->enumList.at(i);
        fprintf(out, "    %4d, 0x%.1x, %4d, %4d,\n",
                 strreg(e.name),
                 cdef->enumDeclarations.value(e.name) ? 1 : 0,
                 e.values.count(),
                 index);
        index += e.values.count() * 2;
    }

    fprintf(out, "\n // enum data: key, value\n");
    for (i = 0; i < cdef->enumList.count(); ++i) {
        const EnumDef &e = cdef->enumList.at(i);
        for (int j = 0; j < e.values.count(); ++j) {
            const QByteArray &val = e.values.at(j);
            fprintf(out, "    %4d, uint(%s::%s),\n",
                    strreg(val),
                    cdef->qualified.constData(),
                    val.constData());
        }
    }
}

void Generator::generateMetacall()
{
    bool isQObject = (cdef->classname == "QObject");

    fprintf(out, "\nint %s::qt_metacall(QMetaObject::Call _c, int _id, void **_a)\n{\n",
             cdef->qualified.constData());

    if (!purestSuperClass.isEmpty() && !isQObject) {
        QByteArray superClass = purestSuperClass;
        // workaround for VC6
        if (superClass.contains("::")) {
            fprintf(out, "    typedef %s QMocSuperClass;\n", superClass.constData());
            superClass = "QMocSuperClass";
        }
        fprintf(out, "    _id = %s::qt_metacall(_c, _id, _a);\n", superClass.constData());
    }

    fprintf(out, "    if (_id < 0)\n        return _id;\n");
    fprintf(out, "    ");

    bool needElse = false;
    QList<FunctionDef> methodList;
    methodList += cdef->signalList;
    methodList += cdef->slotList;
    methodList += cdef->methodList;

    if (methodList.size()) {
        needElse = true;
        fprintf(out, "if (_c == QMetaObject::InvokeMetaMethod) {\n        ");
        fprintf(out, "switch (_id) {\n");
        for (int methodindex = 0; methodindex < methodList.size(); ++methodindex) {
            const FunctionDef &f = methodList.at(methodindex);
            fprintf(out, "        case %d: ", methodindex);
            if (f.normalizedType.size())
                fprintf(out, "{ %s _r = ", noRef(f.normalizedType).constData());
            if (f.inPrivateClass.size())
                fprintf(out, "%s->", f.inPrivateClass.constData());
            fprintf(out, "%s(", f.name.constData());
            int offset = 1;
            for (int j = 0; j < f.arguments.count(); ++j) {
                const ArgumentDef &a = f.arguments.at(j);
                if (j)
                    fprintf(out, ",");
                fprintf(out, "(*reinterpret_cast< %s>(_a[%d]))",a.typeNameForCast.constData(), offset++);
            }
            fprintf(out, ");");
            if (f.normalizedType.size())
                fprintf(out, "\n            if (_a[0]) *reinterpret_cast< %s*>(_a[0]) = _r; } ",
                        noRef(f.normalizedType).constData());
            fprintf(out, " break;\n");
        }
        fprintf(out, "        default: ;\n");
        fprintf(out, "        }\n");
    }
    if (methodList.size())
        fprintf(out, "        _id -= %d;\n    }", methodList.size());

    if (cdef->propertyList.size()) {
        bool needGet = false;
        bool needTempVarForGet = false;
        bool needSet = false;
        bool needReset = false;
        bool needDesignable = false;
        bool needScriptable = false;
        bool needStored = false;
        bool needEditable = false;
        bool needUser = false;
        for (int i = 0; i < cdef->propertyList.size(); ++i) {
            const PropertyDef &p = cdef->propertyList.at(i);
            needGet |= !p.read.isEmpty();
            if (!p.read.isEmpty())
                needTempVarForGet |= (p.gspec != PropertyDef::PointerSpec
                                      && p.gspec != PropertyDef::ReferenceSpec);

            needSet |= !p.write.isEmpty();
            needReset |= !p.reset.isEmpty();
            needDesignable |= p.designable.endsWith(')');
            needScriptable |= p.scriptable.endsWith(')');
            needStored |= p.stored.endsWith(')');
            needEditable |= p.editable.endsWith(')');
            needUser |= p.user.endsWith(')');
        }
        bool needAnything = needGet
                            | needSet
                            | needReset
                            | needDesignable
                            | needScriptable
                            | needStored
                            | needEditable
                            | needUser;
        if (!needAnything)
            goto skip_properties;
        fprintf(out, "\n#ifndef QT_NO_PROPERTIES\n     ");

        if (needElse)
            fprintf(out, " else ");
        fprintf(out, "if (_c == QMetaObject::ReadProperty) {\n");
        if (needGet) {
            if (needTempVarForGet)
                fprintf(out, "        void *_v = _a[0];\n");
            fprintf(out, "        switch (_id) {\n");
            for (int propindex = 0; propindex < cdef->propertyList.size(); ++propindex) {
                const PropertyDef &p = cdef->propertyList.at(propindex);
                if (p.read.isEmpty())
                    continue;
                if (p.gspec == PropertyDef::PointerSpec)
                    fprintf(out, "        case %d: _a[0] = const_cast<void*>(reinterpret_cast<const void*>(%s())); break;\n",
                            propindex, p.read.constData());
                else if (p.gspec == PropertyDef::ReferenceSpec)
                    fprintf(out, "        case %d: _a[0] = const_cast<void*>(reinterpret_cast<const void*>(&%s())); break;\n",
                            propindex, p.read.constData());
                else if (cdef->enumDeclarations.value(p.type, false))
                    fprintf(out, "        case %d: *reinterpret_cast<int*>(_v) = QFlag(%s()); break;\n",
                            propindex, p.read.constData());
                else
                    fprintf(out, "        case %d: *reinterpret_cast< %s*>(_v) = %s(); break;\n",
                            propindex, p.type.constData(), p.read.constData());
            }
            fprintf(out, "        }\n");
        }

        fprintf(out,
                "        _id -= %d;\n"
                "    }", cdef->propertyList.count());

        fprintf(out, " else ");
        fprintf(out, "if (_c == QMetaObject::WriteProperty) {\n");

        if (needSet) {
            fprintf(out, "        void *_v = _a[0];\n");
            fprintf(out, "        switch (_id) {\n");
            for (int propindex = 0; propindex < cdef->propertyList.size(); ++propindex) {
                const PropertyDef &p = cdef->propertyList.at(propindex);
                if (p.write.isEmpty())
                    continue;
                if (cdef->enumDeclarations.value(p.type, false)) {
                    fprintf(out, "        case %d: %s(QFlag(*reinterpret_cast<int*>(_v))); break;\n",
                            propindex, p.write.constData());
                } else {
                    fprintf(out, "        case %d: %s(*reinterpret_cast< %s*>(_v)); break;\n",
                            propindex, p.write.constData(), p.type.constData());
                }
            }
            fprintf(out, "        }\n");
        }

        fprintf(out,
                "        _id -= %d;\n"
                "    }", cdef->propertyList.count());

        fprintf(out, " else ");
        fprintf(out, "if (_c == QMetaObject::ResetProperty) {\n");
        if (needReset) {
            fprintf(out, "        switch (_id) {\n");
            for (int propindex = 0; propindex < cdef->propertyList.size(); ++propindex) {
                const PropertyDef &p = cdef->propertyList.at(propindex);
                if (!p.reset.endsWith(')'))
                    continue;
                fprintf(out, "        case %d: %s; break;\n",
                        propindex, p.reset.constData());
            }
            fprintf(out, "        }\n");
        }
        fprintf(out,
                "        _id -= %d;\n"
                "    }", cdef->propertyList.count());

        fprintf(out, " else ");
        fprintf(out, "if (_c == QMetaObject::QueryPropertyDesignable) {\n");
        if (needDesignable) {
            fprintf(out, "        bool *_b = reinterpret_cast<bool*>(_a[0]);\n");
            fprintf(out, "        switch (_id) {\n");
            for (int propindex = 0; propindex < cdef->propertyList.size(); ++propindex) {
                const PropertyDef &p = cdef->propertyList.at(propindex);
                if (!p.designable.endsWith(')'))
                    continue;
                fprintf(out, "        case %d: *_b = %s; break;\n",
                         propindex, p.designable.constData());
            }
            fprintf(out, "        }\n");
        }
        fprintf(out,
                "        _id -= %d;\n"
                "    }", cdef->propertyList.count());

        fprintf(out, " else ");
        fprintf(out, "if (_c == QMetaObject::QueryPropertyScriptable) {\n");
        if (needScriptable) {
            fprintf(out, "        bool *_b = reinterpret_cast<bool*>(_a[0]);\n");
            fprintf(out, "        switch (_id) {\n");
            for (int propindex = 0; propindex < cdef->propertyList.size(); ++propindex) {
                const PropertyDef &p = cdef->propertyList.at(propindex);
                if (!p.scriptable.endsWith(')'))
                    continue;
                fprintf(out, "        case %d: *_b = %s; break;\n",
                         propindex, p.scriptable.constData());
            }
            fprintf(out, "        }\n");
        }
        fprintf(out,
                "        _id -= %d;\n"
                "    }", cdef->propertyList.count());

        fprintf(out, " else ");
        fprintf(out, "if (_c == QMetaObject::QueryPropertyStored) {\n");
        if (needStored) {
            fprintf(out, "        bool *_b = reinterpret_cast<bool*>(_a[0]);\n");
            fprintf(out, "        switch (_id) {\n");
            for (int propindex = 0; propindex < cdef->propertyList.size(); ++propindex) {
                const PropertyDef &p = cdef->propertyList.at(propindex);
                if (!p.stored.endsWith(')'))
                    continue;
                fprintf(out, "        case %d: *_b = %s; break;\n",
                         propindex, p.stored.constData());
            }
            fprintf(out, "        }\n");
        }
        fprintf(out,
                "        _id -= %d;\n"
                "    }", cdef->propertyList.count());

        fprintf(out, " else ");
        fprintf(out, "if (_c == QMetaObject::QueryPropertyEditable) {\n");
        if (needEditable) {
            fprintf(out, "        bool *_b = reinterpret_cast<bool*>(_a[0]);\n");
            fprintf(out, "        switch (_id) {\n");
            for (int propindex = 0; propindex < cdef->propertyList.size(); ++propindex) {
                const PropertyDef &p = cdef->propertyList.at(propindex);
                if (!p.editable.endsWith(')'))
                    continue;
                fprintf(out, "        case %d: *_b = %s; break;\n",
                         propindex, p.editable.constData());
            }
            fprintf(out, "        }\n");
        }
        fprintf(out,
                "        _id -= %d;\n"
                "    }", cdef->propertyList.count());


        fprintf(out, " else ");
        fprintf(out, "if (_c == QMetaObject::QueryPropertyUser) {\n");
        if (needUser) {
            fprintf(out, "        bool *_b = reinterpret_cast<bool*>(_a[0]);\n");
            fprintf(out, "        switch (_id) {\n");
            for (int propindex = 0; propindex < cdef->propertyList.size(); ++propindex) {
                const PropertyDef &p = cdef->propertyList.at(propindex);
                if (!p.user.endsWith(')'))
                    continue;
                fprintf(out, "        case %d: *_b = %s; break;\n",
                         propindex, p.user.constData());
            }
            fprintf(out, "        }\n");
        }
        fprintf(out,
                "        _id -= %d;\n"
                "    }", cdef->propertyList.count());


        fprintf(out, "\n#endif // QT_NO_PROPERTIES");
    }
 skip_properties:
    if (methodList.size() || cdef->signalList.size() || cdef->propertyList.size())
        fprintf(out, "\n    ");
    fprintf(out,"return _id;\n}\n");
}

void Generator::generateStaticMetacall(const QByteArray &prefix)
{
    bool isQObject = (cdef->classname == "QObject");

    fprintf(out, "static int %s_qt_static_metacall(QMetaObject::Call _c, int _id, void **_a)\n{\n",
            prefix.constData());

    fprintf(out, "    if (_c == QMetaObject::CreateInstance) {\n");
    fprintf(out, "        switch (_id) {\n");
    for (int ctorindex = 0; ctorindex < cdef->constructorList.count(); ++ctorindex) {
        fprintf(out, "        case %d: { %s *_r = new %s(", ctorindex,
                cdef->classname.constData(), cdef->classname.constData());
        const FunctionDef &f = cdef->constructorList.at(ctorindex);
        int offset = 1;
        for (int j = 0; j < f.arguments.count(); ++j) {
            const ArgumentDef &a = f.arguments.at(j);
            if (j)
                fprintf(out, ",");
            fprintf(out, "(*reinterpret_cast< %s>(_a[%d]))", a.typeNameForCast.constData(), offset++);
        }
        fprintf(out, ");\n");
        fprintf(out, "            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = _r; } break;\n");
    }
    fprintf(out, "        }\n");
    fprintf(out, "        _id -= %d;\n", cdef->constructorList.count());
    fprintf(out, "        return _id;\n");
    fprintf(out, "    }\n");

    if (!isQObject)
        fprintf(out, "    _id = %s::staticMetaObject.superClass()->static_metacall(_c, _id, _a);\n", cdef->classname.constData());

    fprintf(out, "    if (_id < 0)\n        return _id;\n");

    fprintf(out, "    return _id;\n");
    fprintf(out, "}\n\n");
}

void Generator::generateSignal(FunctionDef *def,int index)
{
    if (def->wasCloned || def->isAbstract)
        return;
    fprintf(out, "\n// SIGNAL %d\n%s %s::%s(",
            index, def->type.name.constData(), cdef->qualified.constData(), def->name.constData());

    QByteArray thisPtr = "this";
    const char *constQualifier = "";

    if (def->isConst) {
        thisPtr = "const_cast< ";
        thisPtr += cdef->qualified;
        thisPtr += " *>(this)";
        constQualifier = "const";
    }

    if (def->arguments.isEmpty() && def->normalizedType.isEmpty()) {
        fprintf(out, ")%s\n{\n"
                "    QMetaObject::activate(%s, &staticMetaObject, %d, 0);\n"
                "}\n", constQualifier, thisPtr.constData(), index);
        return;
    }

    int offset = 1;
    for (int j = 0; j < def->arguments.count(); ++j) {
        const ArgumentDef &a = def->arguments.at(j);
        if (j)
            fprintf(out, ", ");
        fprintf(out, "%s _t%d%s", a.type.name.constData(), offset++, a.rightType.constData());
    }
    fprintf(out, ")%s\n{\n", constQualifier);
    if (def->type.name.size() && def->normalizedType.size())
        fprintf(out, "    %s _t0;\n", noRef(def->normalizedType).constData());

    fprintf(out, "    void *_a[] = { ");
    if (def->normalizedType.isEmpty()) {
        fprintf(out, "0");
    } else {
        if (def->returnTypeIsVolatile)
             fprintf(out, "const_cast<void*>(reinterpret_cast<const volatile void*>(&_t0))");
        else
             fprintf(out, "const_cast<void*>(reinterpret_cast<const void*>(&_t0))");
    }
    int i;
    for (i = 1; i < offset; ++i)
        if (def->arguments.at(i - 1).type.isVolatile)
            fprintf(out, ", const_cast<void*>(reinterpret_cast<const volatile void*>(&_t%d))", i);
        else
            fprintf(out, ", const_cast<void*>(reinterpret_cast<const void*>(&_t%d))", i);
    fprintf(out, " };\n");
    int n = 0;
    for (i = 0; i < def->arguments.count(); ++i)
        if (def->arguments.at(i).isDefault)
            ++n;
    if (n)
        fprintf(out, "    QMetaObject::activate(%s, &staticMetaObject, %d, %d, _a);\n", thisPtr.constData(), index, index + n);
    else
        fprintf(out, "    QMetaObject::activate(%s, &staticMetaObject, %d, _a);\n", thisPtr.constData(), index);
    if (def->normalizedType.size())
        fprintf(out, "    return _t0;\n");
    fprintf(out, "}\n");
}

//
// Functions used when generating QMetaObject directly
//
// Much of this code is copied from the corresponding
// C++ code-generating functions; we can change the
// two generators so that more of the code is shared.
// The key difference from the C++ code generator is
// that instead of calling fprintf(), we append bytes
// to a buffer.
//

QMetaObject *Generator::generateMetaObject(bool ignoreProperties)
{
//
// build the data array
//

    // filter out undeclared enumerators and sets
    {
        QList<EnumDef> enumList;
        for (int i = 0; i < cdef->enumList.count(); ++i) {
            EnumDef def = cdef->enumList.at(i);
            if (cdef->enumDeclarations.contains(def.name)) {
                enumList += def;
            }
            QByteArray alias = cdef->flagAliases.value(def.name);
            if (cdef->enumDeclarations.contains(alias)) {
                def.name = alias;
                enumList += def;
            }
        }
        cdef->enumList = enumList;
    }

    int index = 10;
    meta_data
        << 1 // revision
        << strreg(cdef->qualified) // classname
        << cdef->classInfoList.count() << (cdef->classInfoList.count() ? index : 0) // classinfo
        ;
    index += cdef->classInfoList.count() * 2;

    int methodCount = cdef->signalList.count() + cdef->slotList.count() + cdef->methodList.count();
    meta_data << methodCount << (methodCount ? index : 0); // methods
    index += methodCount * 5;
    if (!ignoreProperties) {
        meta_data << cdef->propertyList.count() << (cdef->propertyList.count() ? index : 0); // properties
        index += cdef->propertyList.count() * 3;
    } else {
        meta_data << 0 << 0; // properties
    }
    meta_data << cdef->enumList.count() << (cdef->enumList.count() ? index : 0); // enums/sets

//
// Build classinfo array
//
    _generateClassInfos();

//
// Build signals array first, otherwise the signal indices would be wrong
//
    _generateFunctions(cdef->signalList, MethodSignal);

//
// Build slots array
//
    _generateFunctions(cdef->slotList, MethodSlot);

//
// Build method array
//
    _generateFunctions(cdef->methodList, MethodMethod);


//
// Build property array
//
    if (!ignoreProperties)
        _generateProperties();

//
// Build enums array
//
    _generateEnums(index);

//
// Terminate data array
//
    meta_data << 0;

//
// Build stringdata array
//
    QVector<char> string_data;
    for (int i = 0; i < strings.size(); ++i) {
        const char *s = strings.at(i).constData();
        char c;
        do {
            c = *(s++);
            string_data << c;
        } while (c != '\0');
    }

//
// Finally create and initialize the static meta object
//
    const int meta_object_offset = 0;
    const int meta_object_size = sizeof(QMetaObject);
    const int meta_data_offset = meta_object_offset + meta_object_size;
    const int meta_data_size = meta_data.count() * sizeof(uint);
    const int string_data_offset = meta_data_offset + meta_data_size;
    const int string_data_size = string_data.count();
    const int total_size = string_data_offset + string_data_size;

    char *blob = new char[total_size];

    char *string_data_output = blob + string_data_offset;
    const char *string_data_src = string_data.constData();
    for (int i = 0; i < string_data.count(); ++i)
        string_data_output[i] = string_data_src[i];

    uint *meta_data_output = reinterpret_cast<uint *>(blob + meta_data_offset);
    const uint *meta_data_src = meta_data.constData();
    for (int i = 0; i < meta_data.count(); ++i)
        meta_data_output[i] = meta_data_src[i];

    QMetaObject *meta_object = new (blob + meta_object_offset)QMetaObject;
    meta_object->d.superdata = 0;
    meta_object->d.stringdata = string_data_output;
    meta_object->d.data = meta_data_output;
    meta_object->d.extradata = 0;
    return meta_object;
}

void Generator::_generateClassInfos()
{
    for (int i = 0; i < cdef->classInfoList.size(); ++i) {
        const ClassInfoDef &c = cdef->classInfoList.at(i);
        meta_data << strreg(c.name) << strreg(c.value);
    }
}

void Generator::_generateFunctions(QList<FunctionDef> &list, int type)
{
    for (int i = 0; i < list.count(); ++i) {
        const FunctionDef &f = list.at(i);

        QByteArray sig = f.name + '(';
        QByteArray arguments;

        for (int j = 0; j < f.arguments.count(); ++j) {
            const ArgumentDef &a = f.arguments.at(j);
            if (j) {
                sig += ",";
                arguments += ",";
            }
            sig += a.normalizedType;
            arguments += a.name;
        }
        sig += ')';

        char flags = type;
        if (f.access == FunctionDef::Private)
            flags |= AccessPrivate;
        else if (f.access == FunctionDef::Public)
            flags |= AccessPublic;
        else if (f.access == FunctionDef::Protected)
            flags |= AccessProtected;
        if (f.access == FunctionDef::Private)
            flags |= AccessPrivate;
        else if (f.access == FunctionDef::Public)
            flags |= AccessPublic;
        else if (f.access == FunctionDef::Protected)
            flags |= AccessProtected;
        if (f.isCompat)
            flags |= MethodCompatibility;
        if (f.wasCloned)
            flags |= MethodCloned;
        if (f.isScriptable)
            flags |= MethodScriptable;

        meta_data << strreg(sig)
                  << strreg(arguments)
                  << strreg(f.normalizedType)
                  << strreg(f.tag)
                  << flags;
    }
}

void Generator::_generateEnums(int index)
{
    index += 4 * cdef->enumList.count();
    int i;
    for (i = 0; i < cdef->enumList.count(); ++i) {
        const EnumDef &e = cdef->enumList.at(i);
        meta_data << strreg(e.name) << (cdef->enumDeclarations.value(e.name) ? 1 : 0)
                  << e.values.count() << index;
        index += e.values.count() * 2;
    }

    for (i = 0; i < cdef->enumList.count(); ++i) {
        const EnumDef &e = cdef->enumList.at(i);
        for (int j = 0; j < e.values.count(); ++j) {
            const QByteArray &val = e.values.at(j);
            meta_data << strreg(val) << 0; // we don't know the value itself
        }
    }
}

void Generator::_generateProperties()
{
    //
    // specify get function, for compatibiliy we accept functions
    // returning pointers, or const char * for QByteArray.
    //
    for (int i = 0; i < cdef->propertyList.count(); ++i) {
        PropertyDef &p = cdef->propertyList[i];
        if (p.read.isEmpty())
            continue;
        for (int j = 0; j < cdef->publicList.count(); ++j) {
            const FunctionDef &f = cdef->publicList.at(j);
            if (f.name != p.read)
                continue;
            if (!f.isConst) // get  functions must be const
                continue;
            if (f.arguments.size()) // and must not take any arguments
                continue;
            PropertyDef::Specification spec = PropertyDef::ValueSpec;
            QByteArray tmp = f.normalizedType;
            if (p.type == "QByteArray" && tmp == "const char *")
                    tmp = "QByteArray";
            if (tmp.left(6) == "const ")
                tmp = tmp.mid(6);
            if (p.type != tmp && tmp.endsWith('*')) {
                tmp.chop(1);
                spec = PropertyDef::PointerSpec;
            } else if (f.type.name.endsWith('&')) { // raw type, not normalized type
                spec = PropertyDef::ReferenceSpec;
            }
            if (p.type != tmp)
                continue;
            p.gspec = spec;
            break;
        }
    }


    //
    // Create meta data
    //

    for (int i = 0; i < cdef->propertyList.count(); ++i) {
        const PropertyDef &p = cdef->propertyList.at(i);
        uint flags = Invalid;
        if (!isVariantType(p.type)) {
            flags |= EnumOrFlag;
        } else {
            flags |= qvariant_nameToType(p.type) << 24;
        }
        if (!p.read.isEmpty())
            flags |= Readable;
        if (!p.write.isEmpty()) {
            flags |= Writable;
            if (p.stdCppSet())
                flags |= StdCppSet;
        }
        if (!p.reset.isEmpty())
            flags |= Resettable;

//         if (p.override)
//             flags |= Override;

        if (p.designable.isEmpty())
            flags |= ResolveDesignable;
        else if (p.designable != "false")
            flags |= Designable;

        if (p.scriptable.isEmpty())
            flags |= ResolveScriptable;
        else if (p.scriptable != "false")
            flags |= Scriptable;

        if (p.stored.isEmpty())
            flags |= ResolveStored;
        else if (p.stored != "false")
            flags |= Stored;

        if (p.editable.isEmpty())
            flags |= ResolveEditable;
        else if (p.editable != "false")
            flags |= Editable;

        if (p.user.isEmpty())
            flags |= ResolveUser;
        else if (p.user != "false")
            flags |= User;

        meta_data << strreg(p.name) << strreg(p.type) << flags;
    }
}

QT_END_NAMESPACE

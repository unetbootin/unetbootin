/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the qmake application of the Qt Toolkit.
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

#include "makefile.h"
#include "option.h"
#include "cachekeys.h"
#include "meta.h"
#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <qhash.h>
#include <qdebug.h>
#include <qbuffer.h>
#include <qsettings.h>
#include <qdatetime.h>
#if defined(Q_OS_UNIX)
#include <unistd.h>
#else
#include <io.h>
#endif
#include <qdebug.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

QT_BEGIN_NAMESPACE

// Well, Windows doesn't have this, so here's the macro
#ifndef S_ISDIR
#  define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif

bool MakefileGenerator::canExecute(const QStringList &cmdline, int *a) const
{
    int argv0 = -1;
    for(int i = 0; i < cmdline.count(); ++i) {
        if(!cmdline.at(i).contains('=')) {
            argv0 = i;
            break;
        }
    }
    if(a)
        *a = argv0;
    if(argv0 != -1) {
        const QString c = Option::fixPathToLocalOS(cmdline.at(argv0), true);
        if(exists(c))
            return true;
    }
    return false;
}

QString MakefileGenerator::mkdir_p_asstring(const QString &dir, bool escape) const
{
    QString ret =  "@$(CHK_DIR_EXISTS) ";
    if(escape)
        ret += escapeFilePath(dir);
    else
        ret += dir;
    ret += " ";
    if(isWindowsShell())
        ret += "$(MKDIR)";
    else
        ret += "|| $(MKDIR)";
    ret += " ";
    if(escape)
        ret += escapeFilePath(dir);
    else
        ret += dir;
    ret += " ";
    return ret;
}

bool MakefileGenerator::mkdir(const QString &in_path) const
{
    QString path = Option::fixPathToLocalOS(in_path);
    if(QFile::exists(path))
        return true;

    QDir d;
    if(path.startsWith(QDir::separator())) {
        d.cd(QString(QDir::separator()));
        path = path.right(path.length() - 1);
    }
    bool ret = true;
#ifdef Q_OS_WIN
    bool driveExists = true;
    if(!QDir::isRelativePath(path)) {
        if(QFile::exists(path.left(3))) {
            d.cd(path.left(3));
            path = path.right(path.length() - 3);
        } else {
            warn_msg(WarnLogic, "Cannot access drive '%s' (%s)",
                     path.left(3).toLatin1().data(), path.toLatin1().data());
            driveExists = false;
        }
    }
    if(driveExists)
#endif
    {
        QStringList subs = path.split(QDir::separator());
        for(QStringList::Iterator subit = subs.begin(); subit != subs.end(); ++subit) {
            if(!d.cd(*subit)) {
                d.mkdir((*subit));
                if(d.exists((*subit))) {
                    d.cd((*subit));
                } else {
                    ret = false;
                    break;
                }
            }
        }
    }
    return ret;
}

// ** base makefile generator
MakefileGenerator::MakefileGenerator() :
    init_opath_already(false), init_already(false), no_io(false), project(0)
{
}


void
MakefileGenerator::verifyCompilers()
{
    QMap<QString, QStringList> &v = project->variables();
    QStringList &quc = v["QMAKE_EXTRA_COMPILERS"];
    for(int i = 0; i < quc.size(); ) {
        bool error = false;
        QString comp = quc.at(i);
        if(v[comp + ".output"].isEmpty()) {
            if(!v[comp + ".output_function"].isEmpty()) {
                v[comp + ".output"].append("${QMAKE_FUNC_FILE_IN_" + v[comp + ".output_function"].first() + "}");
            } else {
                error = true;
                warn_msg(WarnLogic, "Compiler: %s: No output file specified", comp.toLatin1().constData());
            }
        } else if(v[comp + ".input"].isEmpty()) {
            error = true;
            warn_msg(WarnLogic, "Compiler: %s: No input variable specified", comp.toLatin1().constData());
        }
        if(error)
            quc.removeAt(i);
        else
            ++i;
    }
}

void
MakefileGenerator::initOutPaths()
{
    if(init_opath_already)
        return;
    verifyCompilers();
    init_opath_already = true;
    QMap<QString, QStringList> &v = project->variables();
    //for shadow builds
    if(!v.contains("QMAKE_ABSOLUTE_SOURCE_PATH")) {
        if(Option::mkfile::do_cache && !Option::mkfile::cachefile.isEmpty() &&
           v.contains("QMAKE_ABSOLUTE_SOURCE_ROOT")) {
            QString root = v["QMAKE_ABSOLUTE_SOURCE_ROOT"].first();
            root = Option::fixPathToTargetOS(root);
            if(!root.isEmpty()) {
                QFileInfo fi = fileInfo(Option::mkfile::cachefile);
                if(!fi.makeAbsolute()) {
                    QString cache_r = fi.path(), pwd = Option::output_dir;
                    if(pwd.startsWith(cache_r) && !pwd.startsWith(root)) {
                        pwd = Option::fixPathToTargetOS(root + pwd.mid(cache_r.length()));
                        if(exists(pwd))
                            v.insert("QMAKE_ABSOLUTE_SOURCE_PATH", QStringList(pwd));
                    }
                }
            }
        }
    }
    if(!v["QMAKE_ABSOLUTE_SOURCE_PATH"].isEmpty()) {
        QString &asp = v["QMAKE_ABSOLUTE_SOURCE_PATH"].first();
        asp = Option::fixPathToTargetOS(asp);
        if(asp.isEmpty() || asp == Option::output_dir) //if they're the same, why bother?
            v["QMAKE_ABSOLUTE_SOURCE_PATH"].clear();
    }

    QString currentDir = qmake_getpwd(); //just to go back to

    //some builtin directories
    if(project->isEmpty("PRECOMPILED_DIR") && !project->isEmpty("OBJECTS_DIR"))
        v["PRECOMPILED_DIR"] = v["OBJECTS_DIR"];
    QString dirs[] = { QString("OBJECTS_DIR"), QString("DESTDIR"), QString("QMAKE_PKGCONFIG_DESTDIR"),
                       QString("SUBLIBS_DIR"), QString("DLLDESTDIR"), QString("QMAKE_LIBTOOL_DESTDIR"),
                       QString("PRECOMPILED_DIR"), QString() };
    for(int x = 0; !dirs[x].isEmpty(); x++) {
        if(v[dirs[x]].isEmpty())
            continue;
        const QString orig_path = v[dirs[x]].first();

        QString &pathRef = v[dirs[x]].first();
        pathRef = fileFixify(pathRef, Option::output_dir, Option::output_dir);

#ifdef Q_OS_WIN
        // We don't want to add a separator for DLLDESTDIR on Windows (###why?)
        if(!(dirs[x] == "DLLDESTDIR"))
#endif
        {
            if(pathRef.right(Option::dir_sep.length()) != Option::dir_sep)
                pathRef += Option::dir_sep;
        }

        if(noIO())
            continue;

        QString path = project->first(dirs[x]); //not to be changed any further
        path = fileFixify(path, currentDir, Option::output_dir);
        debug_msg(3, "Fixed output_dir %s (%s) into %s", dirs[x].toLatin1().constData(),
                  orig_path.toLatin1().constData(), path.toLatin1().constData());
        if(!mkdir(path))
            warn_msg(WarnLogic, "%s: Cannot access directory '%s'", dirs[x].toLatin1().constData(),
                     path.toLatin1().constData());
    }

    //out paths from the extra compilers
    const QStringList &quc = project->values("QMAKE_EXTRA_COMPILERS");
    for(QStringList::ConstIterator it = quc.begin(); it != quc.end(); ++it) {
        QString tmp_out = project->values((*it) + ".output").first();
        if(tmp_out.isEmpty())
            continue;
        const QStringList &tmp = project->values((*it) + ".input");
        for(QStringList::ConstIterator it2 = tmp.begin(); it2 != tmp.end(); ++it2) {
            QStringList &inputs = project->values((*it2));
            for(QStringList::Iterator input = inputs.begin(); input != inputs.end(); ++input) {
                (*input) = fileFixify((*input), Option::output_dir, Option::output_dir);
                QString path = unescapeFilePath(replaceExtraCompilerVariables(tmp_out, (*input), QString()));
                path = Option::fixPathToTargetOS(path);
                int slash = path.lastIndexOf(Option::dir_sep);
                if(slash != -1) {
                    path = path.left(slash);
                    if(path != "." &&
                       !mkdir(fileFixify(path, qmake_getpwd(), Option::output_dir)))
                        warn_msg(WarnLogic, "%s: Cannot access directory '%s'",
                                 (*it).toLatin1().constData(), path.toLatin1().constData());
                }
            }
        }
    }

    if(!v["DESTDIR"].isEmpty()) {
        QDir d(v["DESTDIR"].first());
        if(Option::fixPathToLocalOS(d.absolutePath()) == Option::fixPathToLocalOS(Option::output_dir))
            v.remove("DESTDIR");
    }
    QDir::current().cd(currentDir);
}

QMakeProject
*MakefileGenerator::projectFile() const
{
    return project;
}

void
MakefileGenerator::setProjectFile(QMakeProject *p)
{
    if(project)
        return;
    project = p;
    init();
    usePlatformDir();
    findLibraries();
    if(Option::qmake_mode == Option::QMAKE_GENERATE_MAKEFILE &&
       project->isActiveConfig("link_prl")) //load up prl's'
        processPrlFiles();
}

QStringList
MakefileGenerator::findFilesInVPATH(QStringList l, uchar flags, const QString &vpath_var)
{
    QStringList vpath;
    QMap<QString, QStringList> &v = project->variables();
    for(int val_it = 0; val_it < l.count(); ) {
        bool remove_file = false;
        QString &val = l[val_it];
        if(!val.isEmpty()) {
            QString file = fixEnvVariables(val);
            if(!(flags & VPATH_NoFixify))
                file = fileFixify(file, qmake_getpwd(), Option::output_dir);
            if (file.at(0) == '\"' && file.at(file.length() - 1) == '\"')
                file = file.mid(1, file.length() - 2);

            if(exists(file)) {
                ++val_it;
                continue;
            }
            bool found = false;
            if(QDir::isRelativePath(val)) {
                if(vpath.isEmpty()) {
                    if(!vpath_var.isEmpty())
                        vpath = v[vpath_var];
                    vpath += v["VPATH"] + v["QMAKE_ABSOLUTE_SOURCE_PATH"] + v["DEPENDPATH"];
                    if(Option::output_dir != qmake_getpwd())
                        vpath += Option::output_dir;
                }
                for(QStringList::Iterator vpath_it = vpath.begin();
                    vpath_it != vpath.end(); ++vpath_it) {
                    QString real_dir = Option::fixPathToLocalOS((*vpath_it));
                    if(exists(real_dir + QDir::separator() + val)) {
                        QString dir = (*vpath_it);
                        if(dir.right(Option::dir_sep.length()) != Option::dir_sep)
                            dir += Option::dir_sep;
                        val = dir + val;
                        if(!(flags & VPATH_NoFixify))
                            val = fileFixify(val);
                        found = true;
                        debug_msg(1, "Found file through vpath %s -> %s",
                                  file.toLatin1().constData(), val.toLatin1().constData());
                        break;
                    }
                }
            }
            if(!found) {
                QString dir, regex = val, real_dir;
                if(regex.lastIndexOf(Option::dir_sep) != -1) {
                    dir = regex.left(regex.lastIndexOf(Option::dir_sep) + 1);
                    real_dir = dir;
                    if(!(flags & VPATH_NoFixify))
                        real_dir = fileFixify(real_dir, qmake_getpwd(), Option::output_dir);
                    regex = regex.right(regex.length() - dir.length());
                }
                if(real_dir.isEmpty() || exists(real_dir)) {
                    QStringList files = QDir(real_dir).entryList(QStringList(regex));
                    if(files.isEmpty()) {
                        debug_msg(1, "%s:%d Failure to find %s in vpath (%s)",
                                  __FILE__, __LINE__,
                                  val.toLatin1().constData(), vpath.join("::").toLatin1().constData());
                        if(flags & VPATH_RemoveMissingFiles)
                            remove_file = true;
                        else if(flags & VPATH_WarnMissingFiles)
                            warn_msg(WarnLogic, "Failure to find: %s", val.toLatin1().constData());
                    } else {
                        l.removeAt(val_it);
                        QString a;
                        for(int i = (int)files.count()-1; i >= 0; i--) {
                            if(files[i] == "." || files[i] == "..")
                                continue;
                            a = dir + files[i];
                            if(!(flags & VPATH_NoFixify))
                                a = fileFixify(a);
                            l.insert(val_it, a);
                        }
                    }
                } else {
                    debug_msg(1, "%s:%d Cannot match %s%c%s, as %s does not exist.",
                              __FILE__, __LINE__, real_dir.toLatin1().constData(),
                              QDir::separator().toLatin1(),
                              regex.toLatin1().constData(), real_dir.toLatin1().constData());
                    if(flags & VPATH_RemoveMissingFiles)
                        remove_file = true;
                    else if(flags & VPATH_WarnMissingFiles)
                        warn_msg(WarnLogic, "Failure to find: %s", val.toLatin1().constData());
                }
            }
        }
        if(remove_file)
            l.removeAt(val_it);
        else
            ++val_it;
    }
    return l;
}

void
MakefileGenerator::initCompiler(const MakefileGenerator::Compiler &comp)
{
    QMap<QString, QStringList> &v = project->variables();
    QStringList &l = v[comp.variable_in];
    // find all the relevant file inputs
    if(!init_compiler_already.contains(comp.variable_in)) {
        init_compiler_already.insert(comp.variable_in, true);
        if(!noIO())
            l = findFilesInVPATH(l, (comp.flags & Compiler::CompilerRemoveNoExist) ?
                                 VPATH_RemoveMissingFiles : VPATH_WarnMissingFiles, "VPATH_" + comp.variable_in);
    }
}

void
MakefileGenerator::init()
{
    initOutPaths();
    if(init_already)
        return;
    verifyCompilers();
    init_already = true;

    QMap<QString, QStringList> &v = project->variables();
    QStringList &quc = v["QMAKE_EXTRA_COMPILERS"];

    //make sure the COMPILERS are in the correct input/output chain order
    for(int comp_out = 0, jump_count = 0; comp_out < quc.size(); ++comp_out) {
    continue_compiler_chain:
        if(jump_count > quc.size()) //just to avoid an infinite loop here
            break;
        if(project->variables().contains(quc.at(comp_out) + ".variable_out")) {
            const QStringList &outputs = project->variables().value(quc.at(comp_out) + ".variable_out");
            for(int out = 0; out < outputs.size(); ++out) {
                for(int comp_in = 0; comp_in < quc.size(); ++comp_in) {
                    if(comp_in == comp_out)
                        continue;
                    if(project->variables().contains(quc.at(comp_in) + ".input")) {
                        const QStringList &inputs = project->variables().value(quc.at(comp_in) + ".input");
                        for(int in = 0; in < inputs.size(); ++in) {
                            if(inputs.at(in) == outputs.at(out) && comp_out > comp_in) {
                                ++jump_count;
                                //move comp_out to comp_in and continue the compiler chain
                                quc.move(comp_out, comp_in);
                                comp_out = comp_in;
                                goto continue_compiler_chain;
                            }
                        }
                    }
                }
            }
        }
    }

    if(!project->isEmpty("QMAKE_SUBSTITUTES")) {
        const QStringList &subs = v["QMAKE_SUBSTITUTES"];
        for(int i = 0; i < subs.size(); ++i) {
            if(!subs.at(i).endsWith(".in")) {
                warn_msg(WarnLogic, "Substitute '%s' does not end with '.in'",
                         subs.at(i).toLatin1().constData());
                continue;
            }
            QFile in(fileFixify(subs.at(i))), out(fileInfo(subs.at(i)).fileName());
            if(out.fileName().endsWith(".in"))
                out.setFileName(out.fileName().left(out.fileName().length()-3));
            if(in.open(QFile::ReadOnly)) {
                QString contents;
                QStack<int> state;
                enum { IN_CONDITION, MET_CONDITION, PENDING_CONDITION };
                for(int count = 1; !in.atEnd(); ++count) {
                    QString line = QString::fromUtf8(in.readLine());
                    if(line.startsWith("!!IF ")) {
                        if(state.isEmpty() || state.top() == IN_CONDITION) {
                            QString test = line.mid(5, line.length()-(5+1));
                            if(project->test(test))
                                state.push(IN_CONDITION);
                            else
                                state.push(PENDING_CONDITION);
                        } else {
                            state.push(MET_CONDITION);
                        }
                    } else if(line.startsWith("!!ELIF ")) {
                        if(state.isEmpty()) {
                            warn_msg(WarnLogic, "(%s:%d): Unexpected else condition",
                                     in.fileName().toLatin1().constData(), count);
                        } else if(state.top() == PENDING_CONDITION) {
                            QString test = line.mid(7, line.length()-(7+1));
                            if(project->test(test))  {
                                state.pop();
                                state.push(IN_CONDITION);
                            }
                        } else if(state.top() == IN_CONDITION) {
                            state.pop();
                            state.push(MET_CONDITION);
                        }
                    } else if(line.startsWith("!!ELSE")) {
                        if(state.isEmpty()) {
                            warn_msg(WarnLogic, "(%s:%d): Unexpected else condition",
                                     in.fileName().toLatin1().constData(), count);
                        } else if(state.top() == PENDING_CONDITION) {
                            state.pop();
                            state.push(IN_CONDITION);
                        } else if(state.top() == IN_CONDITION) {
                            state.pop();
                            state.push(MET_CONDITION);
                        }
                    } else if(line.startsWith("!!ENDIF")) {
                        if(state.isEmpty())
                            warn_msg(WarnLogic, "(%s:%d): Unexpected endif",
                                     in.fileName().toLatin1().constData(), count);
                        else
                            state.pop();
                    } else if(state.isEmpty() || state.top() == IN_CONDITION) {
                        contents += project->expand(line).join(QString(Option::field_sep));
                    }
                }
                if(out.exists() && out.open(QFile::ReadOnly)) {
                    QString old = QString::fromUtf8(out.readAll());
                    if(contents == old) {
                        v["QMAKE_INTERNAL_INCLUDED_FILES"].append(subs.at(i));
                        continue;
                    }
                    out.close();
                    if(!out.remove()) {
                        warn_msg(WarnLogic, "Cannot clear substitute '%s'",
                                 out.fileName().toLatin1().constData());
                        continue;
                    }
                }
                if(out.open(QFile::WriteOnly)) {
                    v["QMAKE_INTERNAL_INCLUDED_FILES"].append(subs.at(i));
                    out.write(contents.toUtf8());
                } else {
                    warn_msg(WarnLogic, "Cannot open substitute for output '%s'",
                             out.fileName().toLatin1().constData());
                }
            } else {
                warn_msg(WarnLogic, "Cannot open substitute for input '%s'",
                         in.fileName().toLatin1().constData());
            }
        }
    }

    int x;

    //build up a list of compilers
    QList<Compiler> compilers;
    {
        const char *builtins[] = { "OBJECTS", "SOURCES", "PRECOMPILED_HEADER", 0 };
        for(x = 0; builtins[x]; ++x) {
            Compiler compiler;
            compiler.variable_in = builtins[x];
            compiler.flags = Compiler::CompilerBuiltin;
            compiler.type = QMakeSourceFileInfo::TYPE_C;
            if(!strcmp(builtins[x], "OBJECTS"))
                compiler.flags |= Compiler::CompilerNoCheckDeps;
            compilers.append(compiler);
        }
        for(QStringList::ConstIterator it = quc.begin(); it != quc.end(); ++it) {
            const QStringList &inputs = v[(*it) + ".input"];
            for(x = 0; x < inputs.size(); ++x) {
                Compiler compiler;
                compiler.variable_in = inputs.at(x);
                compiler.flags = Compiler::CompilerNoFlags;
                if(v[(*it) + ".CONFIG"].indexOf("ignore_no_exist") != -1)
                    compiler.flags |= Compiler::CompilerRemoveNoExist;
                if(v[(*it) + ".CONFIG"].indexOf("no_dependencies") != -1)
                    compiler.flags |= Compiler::CompilerNoCheckDeps;

                QString dep_type;
                if(!project->isEmpty((*it) + ".dependency_type"))
                    dep_type = project->first((*it) + ".dependency_type");
                if (dep_type.isEmpty())
                    compiler.type = QMakeSourceFileInfo::TYPE_UNKNOWN;
                else if(dep_type == "TYPE_UI")
                    compiler.type = QMakeSourceFileInfo::TYPE_UI;
                else
                    compiler.type = QMakeSourceFileInfo::TYPE_C;
                compilers.append(compiler);
            }
        }
    }
    { //do the path fixifying
        QStringList paths;
        for(x = 0; x < compilers.count(); ++x) {
            if(!paths.contains(compilers.at(x).variable_in))
                paths << compilers.at(x).variable_in;
        }
        paths << "INCLUDEPATH" << "QMAKE_INTERNAL_INCLUDED_FILES" << "PRECOMPILED_HEADER";
        for(int y = 0; y < paths.count(); y++) {
            QStringList &l = v[paths[y]];
            for(QStringList::Iterator it = l.begin(); it != l.end(); ++it) {
                if((*it).isEmpty())
                    continue;
                if(exists((*it)))
                    (*it) = fileFixify((*it));
            }
        }
    }

    if(noIO() || !doDepends())
        QMakeSourceFileInfo::setDependencyMode(QMakeSourceFileInfo::NonRecursive);
    for(x = 0; x < compilers.count(); ++x)
        initCompiler(compilers.at(x));

    //merge actual compiler outputs into their variable_out. This is done last so that
    //files are already properly fixified.
    for(QStringList::Iterator it = quc.begin(); it != quc.end(); ++it) {
        QString tmp_out = project->values((*it) + ".output").first();
        if(tmp_out.isEmpty())
            continue;
        if(project->values((*it) + ".CONFIG").indexOf("combine") != -1) {
            QStringList &compilerInputs = project->values((*it) + ".input");
            // Don't generate compiler output if it doesn't have input.
            if (compilerInputs.isEmpty() || project->values(compilerInputs.first()).isEmpty())
                continue;
            if(tmp_out.indexOf("$") == -1) {
                if(!verifyExtraCompiler((*it), QString())) //verify
                    continue;
                QString out = fileFixify(tmp_out, Option::output_dir, Option::output_dir);
                bool pre_dep = (project->values((*it) + ".CONFIG").indexOf("target_predeps") != -1);
                if(project->variables().contains((*it) + ".variable_out")) {
                    const QStringList &var_out = project->variables().value((*it) + ".variable_out");
                    for(int i = 0; i < var_out.size(); ++i) {
                        QString v = var_out.at(i);
                        if(v == QLatin1String("SOURCES"))
                            v = "GENERATED_SOURCES";
                        else if(v == QLatin1String("OBJECTS"))
                            pre_dep = false;
                        QStringList &list = project->values(v);
                        if(!list.contains(out))
                            list.append(out);
                    }
                } else if(project->values((*it) + ".CONFIG").indexOf("no_link") == -1) {
                    QStringList &list = project->values("OBJECTS");
                    pre_dep = false;
                    if(!list.contains(out))
                        list.append(out);
                } else {
                        QStringList &list = project->values("UNUSED_SOURCES");
                        if(!list.contains(out))
                            list.append(out);
                }
                if(pre_dep) {
                    QStringList &list = project->variables()["PRE_TARGETDEPS"];
                    if(!list.contains(out))
                        list.append(out);
                }
            }
        } else {
            QStringList &tmp = project->values((*it) + ".input");
            for(QStringList::Iterator it2 = tmp.begin(); it2 != tmp.end(); ++it2) {
                const QStringList inputs = project->values((*it2));
                for(QStringList::ConstIterator input = inputs.constBegin(); input != inputs.constEnd(); ++input) {
                    if((*input).isEmpty())
                        continue;
                    QString in = Option::fixPathToTargetOS((*input), false);
                    if(!verifyExtraCompiler((*it), in)) //verify
                        continue;
                    QString out = replaceExtraCompilerVariables(tmp_out, (*input), QString());
                    out = fileFixify(out, Option::output_dir, Option::output_dir);
                    bool pre_dep = (project->values((*it) + ".CONFIG").indexOf("target_predeps") != -1);
                    if(project->variables().contains((*it) + ".variable_out")) {
                        const QStringList &var_out = project->variables().value((*it) + ".variable_out");
                        for(int i = 0; i < var_out.size(); ++i) {
                            QString v = var_out.at(i);
                            if(v == QLatin1String("SOURCES"))
                                v = "GENERATED_SOURCES";
                            else if(v == QLatin1String("OBJECTS"))
                                pre_dep = false;
                            QStringList &list = project->values(v);
                            if(!list.contains(out))
                                list.append(out);
                        }
                    } else if(project->values((*it) + ".CONFIG").indexOf("no_link") == -1) {
                        pre_dep = false;
                        QStringList &list = project->values("OBJECTS");
                        if(!list.contains(out))
                            list.append(out);
                    } else {
                        QStringList &list = project->values("UNUSED_SOURCES");
                        if(!list.contains(out))
                            list.append(out);
                    }
                    if(pre_dep) {
                        QStringList &list = project->variables()["PRE_TARGETDEPS"];
                        if(!list.contains(out))
                            list.append(out);
                    }
                }
            }
        }
    }

    //handle dependencies
    depHeuristicsCache.clear();
    if(!noIO()) {
        // dependency paths
        QStringList incDirs = v["DEPENDPATH"] + v["QMAKE_ABSOLUTE_SOURCE_PATH"];
        if(project->isActiveConfig("depend_includepath"))
            incDirs += v["INCLUDEPATH"];
        if(!project->isActiveConfig("no_include_pwd")) {
            QString pwd = qmake_getpwd();
            if(pwd.isEmpty())
                pwd = ".";
            incDirs += pwd;
        }
        QList<QMakeLocalFileName> deplist;
        for(QStringList::Iterator it = incDirs.begin(); it != incDirs.end(); ++it)
            deplist.append(QMakeLocalFileName(unescapeFilePath((*it))));
        QMakeSourceFileInfo::setDependencyPaths(deplist);
        debug_msg(1, "Dependency Directories: %s", incDirs.join(" :: ").toLatin1().constData());
        //cache info
        if(project->isActiveConfig("qmake_cache")) {
            QString cache_file;
            if(!project->isEmpty("QMAKE_INTERNAL_CACHE_FILE")) {
                cache_file = Option::fixPathToLocalOS(project->first("QMAKE_INTERNAL_CACHE_FILE"));
            } else {
                cache_file = ".qmake.internal.cache";
                if(project->isActiveConfig("build_pass"))
                    cache_file += ".BUILD." + project->first("BUILD_PASS");
            }
            if(cache_file.indexOf(QDir::separator()) == -1)
                cache_file.prepend(Option::output_dir + QDir::separator());
            QMakeSourceFileInfo::setCacheFile(cache_file);
        }

        //add to dependency engine
        for(x = 0; x < compilers.count(); ++x) {
            const MakefileGenerator::Compiler &comp = compilers.at(x);
            if(!(comp.flags & Compiler::CompilerNoCheckDeps))
                addSourceFiles(v[comp.variable_in], QMakeSourceFileInfo::SEEK_DEPS,
                               (QMakeSourceFileInfo::SourceFileType)comp.type);
        }
    }

    processSources(); //remove anything in SOURCES which is included (thus it need not be linked in)

    //all sources and generated sources must be turned into objects at some point (the one builtin compiler)
    v["OBJECTS"] += createObjectList(v["SOURCES"]) + createObjectList(v["GENERATED_SOURCES"]);

    //Translation files
    if(!project->isEmpty("TRANSLATIONS")) {
        QStringList &trf = project->values("TRANSLATIONS");
        for(QStringList::Iterator it = trf.begin(); it != trf.end(); ++it)
            (*it) = Option::fixPathToLocalOS((*it));
    }

    { //get the output_dir into the pwd
        if(fileFixify(Option::output_dir) != fileFixify(qmake_getpwd()))
            project->values("INCLUDEPATH").append(fileFixify(Option::output_dir,
                                                                  Option::output_dir,
                                                                  Option::output_dir));
    }

    //fix up the target deps
    QString fixpaths[] = { QString("PRE_TARGETDEPS"), QString("POST_TARGETDEPS"), QString() };
    for(int path = 0; !fixpaths[path].isNull(); path++) {
        QStringList &l = v[fixpaths[path]];
        for(QStringList::Iterator val_it = l.begin(); val_it != l.end(); ++val_it) {
            if(!(*val_it).isEmpty())
                (*val_it) = escapeDependencyPath(Option::fixPathToTargetOS((*val_it), false, false));
        }
    }

    //extra depends
    if(!project->isEmpty("DEPENDS")) {
        QStringList &l = v["DEPENDS"];
        for(QStringList::Iterator it = l.begin(); it != l.end(); ++it) {
            QStringList files = v[(*it) + ".file"] + v[(*it) + ".files"]; //why do I support such evil things?
            for(QStringList::Iterator file_it = files.begin(); file_it != files.end(); ++file_it) {
                QStringList &out_deps = findDependencies(*file_it);
                QStringList &in_deps = v[(*it) + ".depends"]; //even more evilness..
                for(QStringList::Iterator dep_it = in_deps.begin(); dep_it != in_deps.end(); ++dep_it) {
                    if(exists(*dep_it)) {
                        out_deps.append(*dep_it);
                    } else {
                        QString dir, regex = Option::fixPathToLocalOS((*dep_it));
                        if(regex.lastIndexOf(Option::dir_sep) != -1) {
                            dir = regex.left(regex.lastIndexOf(Option::dir_sep) + 1);
                            regex = regex.right(regex.length() - dir.length());
                        }
                        QStringList files = QDir(dir).entryList(QStringList(regex));
                        if(files.isEmpty()) {
                            warn_msg(WarnLogic, "Dependency for [%s]: Not found %s", (*file_it).toLatin1().constData(),
                                     (*dep_it).toLatin1().constData());
                        } else {
                            for(int i = 0; i < files.count(); i++)
                                out_deps.append(dir + files[i]);
                        }
                    }
                }
            }
        }
    }

    // escape qmake command
    if (!project->isEmpty("QMAKE_QMAKE")) {
        project->values("QMAKE_QMAKE") = escapeFilePaths(project->values("QMAKE_QMAKE"));
    }
}

bool
MakefileGenerator::processPrlFile(QString &file)
{
    bool ret = false, try_replace_file=false;
    QString meta_file, orig_file = file;
    if(QMakeMetaInfo::libExists(file)) {
        try_replace_file = true;
        meta_file = file;
        file = "";
    } else {
        QString tmp = file;
        int ext = tmp.lastIndexOf('.');
        if(ext != -1)
            tmp = tmp.left(ext);
        meta_file = tmp;
    }
//    meta_file = fileFixify(meta_file);
    QString real_meta_file = Option::fixPathToLocalOS(meta_file);
    if(!meta_file.isEmpty()) {
        QString f = fileFixify(real_meta_file, qmake_getpwd(), Option::output_dir);
        if(QMakeMetaInfo::libExists(f)) {
            QMakeMetaInfo libinfo;
            debug_msg(1, "Processing PRL file: %s", real_meta_file.toLatin1().constData());
            if(!libinfo.readLib(f)) {
                fprintf(stderr, "Error processing meta file: %s\n", real_meta_file.toLatin1().constData());
            } else if(project->isActiveConfig("no_read_prl_" + libinfo.type().toLower())) {
                debug_msg(2, "Ignored meta file %s [%s]", real_meta_file.toLatin1().constData(), libinfo.type().toLatin1().constData());
            } else {
                ret = true;
                QMap<QString, QStringList> &vars = libinfo.variables();
                for(QMap<QString, QStringList>::Iterator it = vars.begin(); it != vars.end(); ++it)
                    processPrlVariable(it.key(), it.value());
                if(try_replace_file && !libinfo.isEmpty("QMAKE_PRL_TARGET")) {
                    QString dir;
                    int slsh = real_meta_file.lastIndexOf(Option::dir_sep);
                    if(slsh != -1)
                        dir = real_meta_file.left(slsh+1);
                    file = libinfo.first("QMAKE_PRL_TARGET");
                    if(QDir::isRelativePath(file))
                        file.prepend(dir);
                }
            }
        }
        if(ret) {
            QString mf = QMakeMetaInfo::findLib(meta_file);
            if(project->values("QMAKE_PRL_INTERNAL_FILES").indexOf(mf) == -1)
               project->values("QMAKE_PRL_INTERNAL_FILES").append(mf);
            if(project->values("QMAKE_INTERNAL_INCLUDED_FILES").indexOf(mf) == -1)
               project->values("QMAKE_INTERNAL_INCLUDED_FILES").append(mf);
        }
    }
    if(try_replace_file && file.isEmpty()) {
#if 0
        warn_msg(WarnLogic, "Found prl [%s] file with no target [%s]!", meta_file.toLatin1().constData(),
                 orig_file.toLatin1().constData());
#endif
        file = orig_file;
    }
    return ret;
}

void
MakefileGenerator::filterIncludedFiles(const QString &var)
{
    QStringList &inputs = project->values(var);
    for(QStringList::Iterator input = inputs.begin(); input != inputs.end(); ) {
        if(QMakeSourceFileInfo::included((*input)) > 0)
            input = inputs.erase(input);
        else
            ++input;
    }
}

void
MakefileGenerator::processPrlVariable(const QString &var, const QStringList &l)
{
    if(var == "QMAKE_PRL_LIBS") {
        QString where = "QMAKE_LIBS";
        if(!project->isEmpty("QMAKE_INTERNAL_PRL_LIBS"))
            where = project->first("QMAKE_INTERNAL_PRL_LIBS");
        QStringList &out = project->values(where);
        for(QStringList::ConstIterator it = l.begin(); it != l.end(); ++it) {
            if(out.indexOf((*it)) == -1)
                out.append((*it));
        }
    } else if(var == "QMAKE_PRL_DEFINES") {
        QStringList &out = project->values("DEFINES");
        for(QStringList::ConstIterator it = l.begin(); it != l.end(); ++it) {
            if(out.indexOf((*it)) == -1 &&
               project->values("PRL_EXPORT_DEFINES").indexOf((*it)) == -1)
                out.append((*it));
        }
    }
}

void
MakefileGenerator::processPrlFiles()
{
    QHash<QString, bool> processed;
    for(bool ret = false; true; ret = false) {
        //read in any prl files included..
        QStringList l_out;
        QString where = "QMAKE_LIBS";
        if(!project->isEmpty("QMAKE_INTERNAL_PRL_LIBS"))
            where = project->first("QMAKE_INTERNAL_PRL_LIBS");
        QStringList &l = project->values(where);
        for(QStringList::Iterator it = l.begin(); it != l.end(); ++it) {
            QString file = (*it);
            if(!processed.contains(file) && processPrlFile(file)) {
                processed.insert(file, true);
                ret = true;
            }
            if(!file.isEmpty())
                l_out.append(file);
        }
        if(ret)
            l = l_out;
        else
            break;
    }
}

void
MakefileGenerator::writePrlFile(QTextStream &t)
{
    QString target = project->first("TARGET");
    int slsh = target.lastIndexOf(Option::dir_sep);
    if(slsh != -1)
        target = target.right(target.length() - slsh - 1);
    QString bdir = Option::output_dir;
    if(bdir.isEmpty())
        bdir = qmake_getpwd();
    t << "QMAKE_PRL_BUILD_DIR = " << bdir << endl;

    if(!project->projectFile().isEmpty() && project->projectFile() != "-")
        t << "QMAKE_PRO_INPUT = " << project->projectFile().section('/', -1) << endl;

    if(!project->isEmpty("QMAKE_ABSOLUTE_SOURCE_PATH"))
        t << "QMAKE_PRL_SOURCE_DIR = " << project->first("QMAKE_ABSOLUTE_SOURCE_PATH") << endl;
    t << "QMAKE_PRL_TARGET = " << target << endl;
    if(!project->isEmpty("PRL_EXPORT_DEFINES"))
        t << "QMAKE_PRL_DEFINES = " << project->values("PRL_EXPORT_DEFINES").join(" ") << endl;
    if(!project->isEmpty("PRL_EXPORT_CFLAGS"))
        t << "QMAKE_PRL_CFLAGS = " << project->values("PRL_EXPORT_CFLAGS").join(" ") << endl;
    if(!project->isEmpty("PRL_EXPORT_CXXFLAGS"))
        t << "QMAKE_PRL_CXXFLAGS = " << project->values("PRL_EXPORT_CXXFLAGS").join(" ") << endl;
    if(!project->isEmpty("CONFIG"))
        t << "QMAKE_PRL_CONFIG = " << project->values("CONFIG").join(" ") << endl;
    if(!project->isEmpty("TARGET_VERSION_EXT"))
        t << "QMAKE_PRL_VERSION = " << project->first("TARGET_VERSION_EXT") << endl;
    else if(!project->isEmpty("VERSION"))
        t << "QMAKE_PRL_VERSION = " << project->first("VERSION") << endl;
    if(project->isActiveConfig("staticlib") || project->isActiveConfig("explicitlib")) {
        QStringList libs;
        if(!project->isEmpty("QMAKE_INTERNAL_PRL_LIBS"))
            libs = project->values("QMAKE_INTERNAL_PRL_LIBS");
        else
            libs << "QMAKE_LIBS"; //obvious one
        t << "QMAKE_PRL_LIBS = ";
        for(QStringList::Iterator it = libs.begin(); it != libs.end(); ++it)
            t << project->values((*it)).join(" ") << " ";
        t << endl;
    }
}

bool
MakefileGenerator::writeProjectMakefile()
{
    usePlatformDir();
    QTextStream t(&Option::output);

    //header
    writeHeader(t);

    QList<SubTarget*> targets;
    {
        QStringList builds = project->values("BUILDS");
        for(QStringList::Iterator it = builds.begin(); it != builds.end(); ++it) {
            SubTarget *st = new SubTarget;
            targets.append(st);
            st->makefile = "$(MAKEFILE)." + (*it);
            st->name = (*it);
            st->target = project->isEmpty((*it) + ".target") ? (*it) : project->first((*it) + ".target");
        }
    }
    if(project->isActiveConfig("build_all")) {
        t << "first: all" << endl;
        QList<SubTarget*>::Iterator it;

        //install
        t << "install: ";
        for(it = targets.begin(); it != targets.end(); ++it)
            t << (*it)->target << "-install ";
        t << endl;

        //uninstall
        t << "uninstall: ";
        for(it = targets.begin(); it != targets.end(); ++it)
            t << (*it)->target << "-uninstall ";
        t << endl;
    } else {
        t << "first: " << targets.first()->target << endl
          << "install: " << targets.first()->target << "-install" << endl
          << "uninstall: " << targets.first()->target << "-uninstall" << endl;
    }

    writeSubTargets(t, targets, SubTargetsNoFlags);
    if(!project->isActiveConfig("no_autoqmake")) {
        for(QList<SubTarget*>::Iterator it = targets.begin(); it != targets.end(); ++it)
            t << (*it)->makefile << ": " <<
                Option::fixPathToTargetOS(fileFixify(Option::output.fileName())) << endl;
    }
    qDeleteAll(targets);
    return true;
}

bool
MakefileGenerator::write()
{
    if(!project)
        return false;
    writePrlFile();
    if(Option::qmake_mode == Option::QMAKE_GENERATE_MAKEFILE || //write makefile
       Option::qmake_mode == Option::QMAKE_GENERATE_PROJECT) {
        QTextStream t(&Option::output);
        if(!writeMakefile(t)) {
#if 1
            warn_msg(WarnLogic, "Unable to generate output for: %s [TEMPLATE %s]",
                     Option::output.fileName().toLatin1().constData(),
                     project->first("TEMPLATE").toLatin1().constData());
            if(Option::output.exists())
                Option::output.remove();
#endif
        }
    }
    return true;
}

QString
MakefileGenerator::prlFileName(bool fixify)
{
    QString ret = project->first("TARGET_PRL");;
    if(ret.isEmpty())
        ret = project->first("TARGET");
    int slsh = ret.lastIndexOf(Option::dir_sep);
    if(slsh != -1)
        ret = ret.right(ret.length() - slsh);
    if(!ret.endsWith(Option::prl_ext)) {
        int dot = ret.indexOf('.');
        if(dot != -1)
            ret = ret.left(dot);
        ret += Option::prl_ext;
    }
    if(!project->isEmpty("QMAKE_BUNDLE"))
        ret.prepend(project->first("QMAKE_BUNDLE") + Option::dir_sep);
    if(fixify) {
        if(!project->isEmpty("DESTDIR"))
            ret.prepend(project->first("DESTDIR"));
        ret = Option::fixPathToLocalOS(fileFixify(ret, qmake_getpwd(), Option::output_dir));
    }
    return ret;
}

void
MakefileGenerator::writePrlFile()
{
    if((Option::qmake_mode == Option::QMAKE_GENERATE_MAKEFILE ||
            Option::qmake_mode == Option::QMAKE_GENERATE_PRL)
       && project->values("QMAKE_FAILED_REQUIREMENTS").isEmpty()
       && project->isActiveConfig("create_prl")
       && (project->first("TEMPLATE") == "lib"
       || project->first("TEMPLATE") == "vclib")
       && !project->isActiveConfig("plugin")) { //write prl file
        QString local_prl = prlFileName();
        QString prl = fileFixify(local_prl);
        mkdir(fileInfo(local_prl).path());
        QFile ft(local_prl);
        if(ft.open(QIODevice::WriteOnly)) {
            project->values("ALL_DEPS").append(prl);
            project->values("QMAKE_INTERNAL_PRL_FILE").append(prl);
            QTextStream t(&ft);
            writePrlFile(t);
        }
    }
}

// Manipulate directories, so it's possible to build
// several cross-platform targets concurrently
void
MakefileGenerator::usePlatformDir()
{
    QString pltDir(project->first("QMAKE_PLATFORM_DIR"));
    if(pltDir.isEmpty())
        return;
    QChar sep = QDir::separator();
    QString slashPltDir = sep + pltDir;

    QString dirs[] = { QString("OBJECTS_DIR"), QString("DESTDIR"), QString("QMAKE_PKGCONFIG_DESTDIR"),
                       QString("SUBLIBS_DIR"), QString("DLLDESTDIR"), QString("QMAKE_LIBTOOL_DESTDIR"),
                       QString("PRECOMPILED_DIR"), QString("QMAKE_LIBDIR_QT"), QString() };
    for(int i = 0; !dirs[i].isEmpty(); ++i) {
        QString filePath = project->first(dirs[i]);
        project->values(dirs[i]) = QStringList(filePath + (filePath.isEmpty() ? pltDir : slashPltDir));
    }

    QString libs[] = { QString("QMAKE_LIBS_QT"), QString("QMAKE_LIBS_QT_THREAD"), QString("QMAKE_LIBS_QT_ENTRY"), QString() };
    for(int i = 0; !libs[i].isEmpty(); ++i) {
        QString filePath = project->first(libs[i]);
        int fpi = filePath.lastIndexOf(sep);
        if(fpi == -1)
            project->values(libs[i]).prepend(pltDir + sep);
        else
            project->values(libs[i]) = QStringList(filePath.left(fpi) + slashPltDir + filePath.mid(fpi));
    }
}

void
MakefileGenerator::writeObj(QTextStream &t, const QString &src)
{
    QStringList &srcl = project->values(src);
    QStringList objl = createObjectList(srcl);

    QStringList::Iterator oit = objl.begin();
    QStringList::Iterator sit = srcl.begin();
    QString stringSrc("$src");
    QString stringObj("$obj");
    for(;sit != srcl.end() && oit != objl.end(); ++oit, ++sit) {
        if((*sit).isEmpty())
            continue;

        t << escapeDependencyPath((*oit)) << ": " << escapeDependencyPath((*sit)) << " " << escapeDependencyPaths(findDependencies((*sit))).join(" \\\n\t\t");

        QString comp, cimp;
        for(QStringList::Iterator cppit = Option::cpp_ext.begin(); cppit != Option::cpp_ext.end(); ++cppit) {
            if((*sit).endsWith((*cppit))) {
                comp = "QMAKE_RUN_CXX";
                cimp = "QMAKE_RUN_CXX_IMP";
                break;
            }
        }
        if(comp.isEmpty()) {
            comp = "QMAKE_RUN_CC";
            cimp = "QMAKE_RUN_CC_IMP";
        }
        bool use_implicit_rule = !project->isEmpty(cimp);
        use_implicit_rule = false;
        if(use_implicit_rule) {
            if(!project->isEmpty("OBJECTS_DIR")) {
                use_implicit_rule = false;
            } else {
                int dot = (*sit).lastIndexOf('.');
                if(dot == -1 || ((*sit).left(dot) + Option::obj_ext != (*oit)))
                    use_implicit_rule = false;
            }
        }
        if (!use_implicit_rule && !project->isEmpty(comp)) {
            QString p = var(comp), srcf(*sit);
            p.replace(stringSrc, escapeFilePath(srcf));
            p.replace(stringObj, escapeFilePath((*oit)));
            t << "\n\t" << p;
        }
        t << endl << endl;
    }
}

QString
MakefileGenerator::filePrefixRoot(const QString &root, const QString &path)
{
    QString ret(root + path);
    if(path.length() > 2 && path[1] == ':') //c:\foo
        ret = QString(path.mid(0, 2) + root + path.mid(2));
    while(ret.endsWith("\\"))
        ret = ret.left(ret.length()-1);
    return ret;
}

void
MakefileGenerator::writeInstalls(QTextStream &t, const QString &installs, bool noBuild)
{
    QString rm_dir_contents("-$(DEL_FILE)");
    if (!isWindowsShell()) //ick
        rm_dir_contents = "-$(DEL_FILE) -r";

    QString all_installs, all_uninstalls;
    QStringList &l = project->values(installs);
    for(QStringList::Iterator it = l.begin(); it != l.end(); ++it) {
        QString pvar = (*it) + ".path";
        if(project->values((*it) + ".CONFIG").indexOf("no_path") == -1 &&
           project->values((*it) + ".CONFIG").indexOf("dummy_install") == -1 &&
           project->values(pvar).isEmpty()) {
            warn_msg(WarnLogic, "%s is not defined: install target not created\n", pvar.toLatin1().constData());
            continue;
        }

        bool do_default = true;
        const QString root = "$(INSTALL_ROOT)";
        QString target, dst;
        if(project->values((*it) + ".CONFIG").indexOf("no_path") == -1 &&
           project->values((*it) + ".CONFIG").indexOf("dummy_install") == -1) {
            dst = fileFixify(unescapeFilePath(project->values(pvar).first()), FileFixifyAbsolute, false);
            if(dst.right(1) != Option::dir_sep)
                dst += Option::dir_sep;
        }
        dst = escapeFilePath(dst);

        QStringList tmp, uninst = project->values((*it) + ".uninstall");
        //other
        tmp = project->values((*it) + ".extra");
        if(tmp.isEmpty())
            tmp = project->values((*it) + ".commands"); //to allow compatible name
        if(!tmp.isEmpty()) {
            do_default = false;
            if(!target.isEmpty())
                target += "\n\t";
            target += tmp.join(" ");
        }
        //masks
        tmp = findFilesInVPATH(project->values((*it) + ".files"), VPATH_NoFixify);
        tmp = fileFixify(tmp, FileFixifyAbsolute);
        if(!tmp.isEmpty()) {
            if(!target.isEmpty())
                target += "\n";
            do_default = false;
            for(QStringList::Iterator wild_it = tmp.begin(); wild_it != tmp.end(); ++wild_it) {
                QString wild = Option::fixPathToLocalOS((*wild_it), false, false);
                QString dirstr = qmake_getpwd(), filestr = wild;
                int slsh = filestr.lastIndexOf(Option::dir_sep);
                if(slsh != -1) {
                    dirstr = filestr.left(slsh+1);
                    filestr = filestr.right(filestr.length() - slsh - 1);
                }
                if(dirstr.right(Option::dir_sep.length()) != Option::dir_sep)
                    dirstr += Option::dir_sep;
                if(exists(wild)) { //real file
                    QString file = wild;
                    QFileInfo fi(fileInfo(wild));
                    if(!target.isEmpty())
                        target += "\t";
                    QString dst_file = filePrefixRoot(root, dst);
                    if(fi.isDir() && project->isActiveConfig("copy_dir_files")) {
                        if(!dst_file.endsWith(Option::dir_sep))
                            dst_file += Option::dir_sep;
                        dst_file += fi.fileName();
                    }
                    QString cmd;
                    if (fi.isDir())
                       cmd = "-$(INSTALL_DIR)";
                    else if (fi.isExecutable())
                       cmd = "-$(INSTALL_PROGRAM)";
                    else
                       cmd = "-$(INSTALL_FILE)";
                    cmd += " " + wild + " " + dst_file + "\n";
                    target += cmd;
                    if(!project->isActiveConfig("debug") && !project->isActiveConfig("nostrip") &&
                       !fi.isDir() && fi.isExecutable() && !project->isEmpty("QMAKE_STRIP"))
                        target += QString("\t-") + var("QMAKE_STRIP") + " " +
                                  filePrefixRoot(root, fileFixify(dst + filestr, FileFixifyAbsolute, false)) + "\n";
                    if(!uninst.isEmpty())
                        uninst.append("\n\t");
                    uninst.append(rm_dir_contents + " " + filePrefixRoot(root, fileFixify(dst + filestr, FileFixifyAbsolute, false)));
                    continue;
                }
                QString local_dirstr = Option::fixPathToLocalOS(dirstr, true);
                QStringList files = QDir(local_dirstr).entryList(QStringList(filestr));
                if(project->values((*it) + ".CONFIG").indexOf("no_check_exist") != -1 && files.isEmpty()) {
                    if(!target.isEmpty())
                        target += "\t";
                    QString dst_file = filePrefixRoot(root, dst);
                    QFileInfo fi(fileInfo(wild));
                    QString cmd =  QString(fi.isExecutable() ? "-$(INSTALL_PROGRAM)" : "-$(INSTALL_FILE)") + " " +
                                   wild + " " + dst_file + "\n";
                    target += cmd;
                    if(!uninst.isEmpty())
                        uninst.append("\n\t");
                    uninst.append(rm_dir_contents + " " + filePrefixRoot(root, fileFixify(dst + filestr, FileFixifyAbsolute, false)));
                }
                for(int x = 0; x < files.count(); x++) {
                    QString file = files[x];
                    if(file == "." || file == "..") //blah
                        continue;
                    if(!uninst.isEmpty())
                        uninst.append("\n\t");
                    uninst.append(rm_dir_contents + " " + filePrefixRoot(root, fileFixify(dst + file, FileFixifyAbsolute, false)));
                    QFileInfo fi(fileInfo(dirstr + file));
                    if(!target.isEmpty())
                        target += "\t";
                    QString dst_file = filePrefixRoot(root, fileFixify(dst, FileFixifyAbsolute, false));
                    if(fi.isDir() && project->isActiveConfig("copy_dir_files")) {
                        if(!dst_file.endsWith(Option::dir_sep))
                            dst_file += Option::dir_sep;
                        dst_file += fi.fileName();
                    }
                    QString cmd = QString(fi.isDir() ? "-$(INSTALL_DIR)" : "-$(INSTALL_FILE)") + " " +
                                  dirstr + file + " " + dst_file + "\n";
                    target += cmd;
                    if(!project->isActiveConfig("debug") && !project->isActiveConfig("nostrip") &&
                       !fi.isDir() && fi.isExecutable() && !project->isEmpty("QMAKE_STRIP"))
                        target += QString("\t-") + var("QMAKE_STRIP") + " " +
                                  filePrefixRoot(root, fileFixify(dst + file, FileFixifyAbsolute, false)) +
                                  "\n";
                }
            }
        }
        //default?
        if(do_default) {
            target = defaultInstall((*it));
            uninst = project->values((*it) + ".uninstall");
        }

        if(!target.isEmpty() || project->values((*it) + ".CONFIG").indexOf("dummy_install") != -1) {
            if(noBuild || project->values((*it) + ".CONFIG").indexOf("no_build") != -1)
                t << "install_" << (*it) << ":";
            else if(project->isActiveConfig("build_all"))
                t << "install_" << (*it) << ": all";
            else
                t << "install_" << (*it) << ": first";
            const QStringList &deps = project->values((*it) + ".depends");
            if(!deps.isEmpty()) {
                for(QStringList::ConstIterator dep_it = deps.begin(); dep_it != deps.end(); ++dep_it) {
                    QString targ = var((*dep_it) + ".target");
                    if(targ.isEmpty())
                        targ = (*dep_it);
                    t << " " << escapeDependencyPath(targ);
                }
            }
            if(project->isEmpty("QMAKE_NOFORCE"))
                t <<  " FORCE";
            t << "\n\t";
            const QStringList &dirs = project->values(pvar);
            for(QStringList::ConstIterator pit = dirs.begin(); pit != dirs.end(); ++pit) {
                QString tmp_dst = fileFixify((*pit), FileFixifyAbsolute, false);
                if (!isWindowsShell() && tmp_dst.right(1) != Option::dir_sep)
                    tmp_dst += Option::dir_sep;
                t << mkdir_p_asstring(filePrefixRoot(root, tmp_dst)) << "\n\t";
            }
            t << target << endl << endl;
            if(!uninst.isEmpty()) {
                t << "uninstall_" << (*it) << ": ";
                if(project->isEmpty("QMAKE_NOFORCE"))
                    t <<  " FORCE";
                t << "\n\t"
                  << uninst.join(" ") << "\n\t"
                  << "-$(DEL_DIR) " << filePrefixRoot(root, dst) << " " << endl << endl;
            }
            t << endl;

            if(project->values((*it) + ".CONFIG").indexOf("no_default_install") == -1) {
                all_installs += QString("install_") + (*it) + " ";
                if(!uninst.isEmpty())
                    all_uninstalls += "uninstall_" + (*it) + " ";
            }
        }   else {
            debug_msg(1, "no definition for install %s: install target not created",(*it).toLatin1().constData());
        }
    }
    t << "install: " << var("INSTALLDEPS") << " " << all_installs;
    if(project->isEmpty("QMAKE_NOFORCE"))
        t <<  " FORCE";
    t << "\n\n";
    t << "uninstall: " << all_uninstalls << " " << var("UNINSTALLDEPS");
    if(project->isEmpty("QMAKE_NOFORCE"))
        t <<  " FORCE";
    t << "\n\n";
}

QString
MakefileGenerator::var(const QString &var)
{
    return val(project->values(var));
}

QString
MakefileGenerator::val(const QStringList &varList)
{
    return valGlue(varList, "", " ", "");
}

QString
MakefileGenerator::varGlue(const QString &var, const QString &before, const QString &glue, const QString &after)
{
    return valGlue(project->values(var), before, glue, after);
}

QString
MakefileGenerator::valGlue(const QStringList &varList, const QString &before, const QString &glue, const QString &after)
{
    QString ret;
    for(QStringList::ConstIterator it = varList.begin(); it != varList.end(); ++it) {
        if(!(*it).isEmpty()) {
            if(!ret.isEmpty())
                ret += glue;
            ret += (*it);
        }
    }
    return ret.isEmpty() ? QString("") : before + ret + after;
}


QString
MakefileGenerator::varList(const QString &var)
{
    return valList(project->values(var));
}

QString
MakefileGenerator::valList(const QStringList &varList)
{
    return valGlue(varList, "", " \\\n\t\t", "");
}

QStringList
MakefileGenerator::createObjectList(const QStringList &sources)
{
    QStringList ret;
    QString objdir;
    if(!project->values("OBJECTS_DIR").isEmpty())
        objdir = project->first("OBJECTS_DIR");
    for(QStringList::ConstIterator it = sources.begin(); it != sources.end(); ++it) {
        QFileInfo fi(fileInfo(Option::fixPathToLocalOS((*it))));
        QString dir;
        if(objdir.isEmpty() && project->isActiveConfig("object_with_source")) {
            QString fName = Option::fixPathToTargetOS((*it), false);
            int dl = fName.lastIndexOf(Option::dir_sep);
            if(dl != -1)
                dir = fName.left(dl + 1);
        } else {
            dir = objdir;
        }
        ret.append(dir + fi.completeBaseName() + Option::obj_ext);
    }
    return ret;
}

ReplaceExtraCompilerCacheKey::ReplaceExtraCompilerCacheKey(const QString &v, const QStringList &i, const QStringList &o)
{
    hash = 0;
    pwd = qmake_getpwd();
    var = v;
    {
        QStringList il = i;
        il.sort();
        in = il.join("::");
    }
    {
        QStringList ol = o;
        ol.sort();
        out = ol.join("::");
    }
}

bool ReplaceExtraCompilerCacheKey::operator==(const ReplaceExtraCompilerCacheKey &f) const
{
    return (hashCode() == f.hashCode() &&
            f.in == in &&
            f.out == out &&
            f.var == var &&
            f.pwd == pwd);
}


QString
MakefileGenerator::replaceExtraCompilerVariables(const QString &orig_var, const QStringList &in, const QStringList &out)
{
    //lazy cache
    ReplaceExtraCompilerCacheKey cacheKey(orig_var, in, out);
    QString cacheVal = extraCompilerVariablesCache.value(cacheKey);
    if(!cacheVal.isNull())
        return cacheVal;

    //do the work
    QString ret = orig_var;
    QRegExp reg_var("\\$\\{.*\\}");
    reg_var.setMinimal(true);
    for(int rep = 0; (rep = reg_var.indexIn(ret, rep)) != -1; ) {
        QStringList val;
        const QString var = ret.mid(rep + 2, reg_var.matchedLength() - 3);
        bool filePath = false;
        if(val.isEmpty() && var.startsWith(QLatin1String("QMAKE_VAR_"))) {
            const QString varname = var.mid(10);
            val += project->values(varname);
        }
        if(val.isEmpty() && var.startsWith(QLatin1String("QMAKE_VAR_FIRST_"))) {
            const QString varname = var.mid(12);
            val += project->first(varname);
        }

        if(val.isEmpty() && !in.isEmpty()) {
            if(var.startsWith(QLatin1String("QMAKE_FUNC_FILE_IN_"))) {
                filePath = true;
                const QString funcname = var.mid(19);
                val += project->expand(funcname, QList<QStringList>() << in);
            } else if(var == QLatin1String("QMAKE_FILE_BASE") || var == QLatin1String("QMAKE_FILE_IN_BASE")) {
                //filePath = true;
                for(int i = 0; i < in.size(); ++i) {
                    QFileInfo fi(fileInfo(Option::fixPathToLocalOS(in.at(i))));
                    QString base = fi.completeBaseName();
                    if(base.isNull())
                        base = fi.fileName();
                    val += base;
                }
            } else if(var == QLatin1String("QMAKE_FILE_PATH") || var == QLatin1String("QMAKE_FILE_IN_PATH")) {
                filePath = true;
                for(int i = 0; i < in.size(); ++i)
                    val += fileInfo(Option::fixPathToLocalOS(in.at(i))).path();
            } else if(var == QLatin1String("QMAKE_FILE_NAME") || var == QLatin1String("QMAKE_FILE_IN")) {
                filePath = true;
                for(int i = 0; i < in.size(); ++i)
                    val += fileInfo(Option::fixPathToLocalOS(in.at(i))).filePath();

            }
        }
        if(val.isEmpty() && !out.isEmpty()) {
            if(var.startsWith(QLatin1String("QMAKE_FUNC_FILE_OUT_"))) {
                filePath = true;
                const QString funcname = var.mid(20);
                val += project->expand(funcname, QList<QStringList>() << out);
            } else if(var == QLatin1String("QMAKE_FILE_OUT")) {
                filePath = true;
                for(int i = 0; i < out.size(); ++i)
                    val += fileInfo(Option::fixPathToLocalOS(out.at(i))).filePath();
            } else if(var == QLatin1String("QMAKE_FILE_OUT_BASE")) {
                //filePath = true;
                for(int i = 0; i < out.size(); ++i) {
                    QFileInfo fi(fileInfo(Option::fixPathToLocalOS(out.at(i))));
                    QString base = fi.completeBaseName();
                    if(base.isNull())
                        base = fi.fileName();
                    val += base;
                }
            }
        }
        if(val.isEmpty() && var.startsWith(QLatin1String("QMAKE_FUNC_"))) {
            const QString funcname = var.mid(11);
            val += project->expand(funcname, QList<QStringList>() << in << out);
        }

        if(!val.isEmpty()) {
            QString fullVal;
            if(filePath) {
                for(int i = 0; i < val.size(); ++i) {
                    const QString file = Option::fixPathToTargetOS(unescapeFilePath(val.at(i)), false);
                    if(!fullVal.isEmpty())
                        fullVal += " ";
                    fullVal += escapeFilePath(file);
                }
            } else {
                fullVal = val.join(" ");
            }
            ret.replace(rep, reg_var.matchedLength(), fullVal);
            rep += fullVal.length();
        } else {
            rep += reg_var.matchedLength();
        }
    }

    //cache the value
    extraCompilerVariablesCache.insert(cacheKey, ret);
    return ret;
}

bool
MakefileGenerator::verifyExtraCompiler(const QString &comp, const QString &file_unfixed)
{
    if(noIO())
        return false;
    const QString file = Option::fixPathToLocalOS(file_unfixed);

    if(project->values(comp + ".CONFIG").indexOf("moc_verify") != -1) {
        if(!file.isNull()) {
            QMakeSourceFileInfo::addSourceFile(file, QMakeSourceFileInfo::SEEK_MOCS);
            if(!mocable(file))
                return false;
        }
    } else if(project->values(comp + ".CONFIG").indexOf("function_verify") != -1) {
        QString tmp_out = project->values(comp + ".output").first();
        if(tmp_out.isEmpty())
            return false;
        QStringList verify_function = project->values(comp + ".verify_function");
        if(verify_function.isEmpty())
            return false;

        for(int i = 0; i < verify_function.size(); ++i) {
            bool invert = false;
            QString verify = verify_function.at(i);
            if(verify.at(0) == QLatin1Char('!')) {
                invert = true;
                verify = verify.mid(1);
            }

            if(project->values(comp + ".CONFIG").indexOf("combine") != -1) {
                bool pass = project->test(verify, QList<QStringList>() << QStringList(tmp_out) << QStringList(file));
                if(invert)
                    pass = !pass;
                if(!pass)
                    return false;
            } else {
                QStringList &tmp = project->values(comp + ".input");
                for(QStringList::Iterator it = tmp.begin(); it != tmp.end(); ++it) {
                    QStringList &inputs = project->values((*it));
                    for(QStringList::Iterator input = inputs.begin(); input != inputs.end(); ++input) {
                        if((*input).isEmpty())
                            continue;
                        QString in = fileFixify(Option::fixPathToTargetOS((*input), false));
                        if(in == file) {
                            bool pass = project->test(verify,
                                                      QList<QStringList>() << QStringList(replaceExtraCompilerVariables(tmp_out, (*input), QString())) <<
                                                      QStringList(file));
                            if(invert)
                                pass = !pass;
                            if(!pass)
                                return false;
                            break;
                        }
                    }
                }
            }
        }
    } else if(project->values(comp + ".CONFIG").indexOf("verify") != -1) {
        QString tmp_out = project->values(comp + ".output").first();
        if(tmp_out.isEmpty())
            return false;
        QString tmp_cmd;
        if(!project->isEmpty(comp + ".commands")) {
            int argv0 = -1;
            QStringList cmdline = project->values(comp + ".commands");
            for(int i = 0; i < cmdline.count(); ++i) {
                if(!cmdline.at(i).contains('=')) {
                    argv0 = i;
                    break;
                }
            }
            if(argv0 != -1) {
                cmdline[argv0] = Option::fixPathToTargetOS(cmdline.at(argv0), false);
                tmp_cmd = cmdline.join(" ");
            }
        }

        if(project->values(comp + ".CONFIG").indexOf("combine") != -1) {
            QString cmd = replaceExtraCompilerVariables(tmp_cmd, QString(), tmp_out);
            if(system(cmd.toLatin1().constData()))
                return false;
        } else {
            QStringList &tmp = project->values(comp + ".input");
            for(QStringList::Iterator it = tmp.begin(); it != tmp.end(); ++it) {
                QStringList &inputs = project->values((*it));
                for(QStringList::Iterator input = inputs.begin(); input != inputs.end(); ++input) {
                    if((*input).isEmpty())
                        continue;
                    QString in = fileFixify(Option::fixPathToTargetOS((*input), false));
                    if(in == file) {
                        QString out = replaceExtraCompilerVariables(tmp_out, (*input), QString());
                        QString cmd = replaceExtraCompilerVariables(tmp_cmd, in, out);
                        if(system(cmd.toLatin1().constData()))
                            return false;
                        break;
                    }
                }
            }
        }
    }
    return true;
}

void
MakefileGenerator::writeExtraTargets(QTextStream &t)
{
    QStringList &qut = project->values("QMAKE_EXTRA_TARGETS");
    for(QStringList::Iterator it = qut.begin(); it != qut.end(); ++it) {
        QString targ = var((*it) + ".target"),
                 cmd = var((*it) + ".commands"), deps;
        if(targ.isEmpty())
            targ = (*it);
        QStringList &deplist = project->values((*it) + ".depends");
        for(QStringList::Iterator dep_it = deplist.begin(); dep_it != deplist.end(); ++dep_it) {
            QString dep = var((*dep_it) + ".target");
            if(dep.isEmpty())
                dep = (*dep_it);
            deps += " " + escapeDependencyPath(dep);
        }
        if(project->values((*it) + ".CONFIG").indexOf("fix_target") != -1)
            targ = fileFixify(targ);
        if(project->isEmpty("QMAKE_NOFORCE") &&
           project->values((*it) + ".CONFIG").indexOf("phony") != -1)
            deps += QString(" ") + "FORCE";
        t << escapeDependencyPath(targ) << ":" << deps;
        if(!cmd.isEmpty())
            t << "\n\t" << cmd;
        t << endl << endl;
    }
}

void
MakefileGenerator::writeExtraCompilerTargets(QTextStream &t)
{
    QString clean_targets;
    const QStringList &quc = project->values("QMAKE_EXTRA_COMPILERS");
    for(QStringList::ConstIterator it = quc.begin(); it != quc.end(); ++it) {
        QString tmp_out = fileFixify(project->values((*it) + ".output").first(),
                                     Option::output_dir, Option::output_dir);
        QString tmp_cmd;
        if(!project->isEmpty((*it) + ".commands")) {
            QStringList cmdline = project->values((*it) + ".commands");
            int argv0 = findExecutable(cmdline);
            if(argv0 != -1) {
                cmdline[argv0] = escapeFilePath(Option::fixPathToTargetOS(cmdline.at(argv0), false));
                tmp_cmd = cmdline.join(" ");
            }
        }
        QStringList tmp_dep = project->values((*it) + ".depends");
        QString tmp_dep_cmd;
        if(!project->isEmpty((*it) + ".depend_command")) {
            int argv0 = -1;
            QStringList cmdline = project->values((*it) + ".depend_command");
            for(int i = 0; i < cmdline.count(); ++i) {
                if(!cmdline.at(i).contains('=')) {
                    argv0 = i;
                    break;
                }
            }
            if(argv0 != -1) {
                const QString c = Option::fixPathToLocalOS(cmdline.at(argv0), true);
                if(exists(c)) {
                    cmdline[argv0] = escapeFilePath(Option::fixPathToLocalOS(cmdline.at(argv0), false));
                    tmp_dep_cmd = cmdline.join(" ");
                } else {
                    cmdline[argv0] = escapeFilePath(cmdline.at(argv0));
                }
            }
        }
        QStringList &vars = project->values((*it) + ".variables");
        if(tmp_out.isEmpty() || tmp_cmd.isEmpty())
            continue;
        QStringList tmp_inputs;
        {
            const QStringList &comp_inputs = project->values((*it) + ".input");
            for(QStringList::ConstIterator it2 = comp_inputs.begin(); it2 != comp_inputs.end(); ++it2) {
                const QStringList &tmp = project->values((*it2));
                for(QStringList::ConstIterator input = tmp.begin(); input != tmp.end(); ++input) {
                    QString in = Option::fixPathToTargetOS((*input), false);
                    if(verifyExtraCompiler((*it), in))
                        tmp_inputs.append((*input));
                }
            }
        }

        t << "compiler_" << (*it) << "_make_all:";
        if(project->values((*it) + ".CONFIG").indexOf("combine") != -1) {
            // compilers with a combined input only have one output
            QString input = project->values((*it) + ".output").first();
            t << " " << escapeDependencyPath(replaceExtraCompilerVariables(tmp_out, input, QString()));
        } else {
            for(QStringList::ConstIterator input = tmp_inputs.begin(); input != tmp_inputs.end(); ++input) {
                QString in = Option::fixPathToTargetOS((*input), false);
                t << " " << escapeDependencyPath(replaceExtraCompilerVariables(tmp_out, (*input), QString()));
            }
        }
        t << endl;

        if(project->values((*it) + ".CONFIG").indexOf("no_clean") == -1) {
            QString tmp_clean = project->values((*it) + ".clean").join(" ");
            QString tmp_clean_cmds = project->values((*it) + ".clean_commands").join(" ");
            if(!tmp_inputs.isEmpty())
                clean_targets += QString("compiler_" + (*it) + "_clean ");
            t << "compiler_" << (*it) << "_clean:";
            bool wrote_clean_cmds = false, wrote_clean = false;
            if(tmp_clean_cmds.isEmpty()) {
                wrote_clean_cmds = true;
            } else if(tmp_clean_cmds.indexOf("${QMAKE_") == -1) {
                t << "\n\t" << tmp_clean_cmds;
                wrote_clean_cmds = true;
            }
            if(tmp_clean.isEmpty())
                tmp_clean = tmp_out;
            if(tmp_clean.indexOf("${QMAKE_") == -1) {
                t << "\n\t" << "-$(DEL_FILE) " << tmp_clean;
                wrote_clean = true;
            }
            if(!wrote_clean_cmds || !wrote_clean) {
                QStringList cleans;
                const QString del_statement("-$(DEL_FILE)");
                if(!wrote_clean) {
                    if(project->isActiveConfig("no_delete_multiple_files")) {
                        for(QStringList::ConstIterator input = tmp_inputs.begin(); input != tmp_inputs.end(); ++input)
                            cleans.append(" " + replaceExtraCompilerVariables(tmp_clean, (*input),
                                                replaceExtraCompilerVariables(tmp_out, (*input), QString())));
                    } else {
                        QString files, file;
                        const int commandlineLimit = 2047; // NT limit, expanded
                        for(int input = 0; input < tmp_inputs.size(); ++input) {
                            file = " " + replaceExtraCompilerVariables(tmp_clean, tmp_inputs.at(input),
                                           replaceExtraCompilerVariables(tmp_out, tmp_inputs.at(input), QString()));
                            if(del_statement.length() + files.length() +
                               qMax(fixEnvVariables(file).length(), file.length()) > commandlineLimit) {
                                cleans.append(files);
                                files.clear();
                            }
                            files += file;
                        }
                        if(!files.isEmpty())
                            cleans.append(files);
                    }
                }
                if(!cleans.isEmpty())
                    t << valGlue(cleans, "\n\t" + del_statement, "\n\t" + del_statement, "");
                if(!wrote_clean_cmds) {
                    for(QStringList::ConstIterator input = tmp_inputs.begin(); input != tmp_inputs.end(); ++input) {
                        t << "\n\t" << replaceExtraCompilerVariables(tmp_clean_cmds, (*input),
                                         replaceExtraCompilerVariables(tmp_out, (*input), QString()));
                    }
                }
            }
            t << endl;
        }
        if(project->values((*it) + ".CONFIG").indexOf("combine") != -1) {
            if(tmp_out.indexOf("${QMAKE_") != -1) {
                warn_msg(WarnLogic, "QMAKE_EXTRA_COMPILERS(%s) with combine has variable output.",
                         (*it).toLatin1().constData());
                continue;
            }
            QStringList deps, inputs;
            if(!tmp_dep.isEmpty())
                deps += fileFixify(tmp_dep, Option::output_dir, Option::output_dir);
            for(QStringList::ConstIterator input = tmp_inputs.begin(); input != tmp_inputs.end(); ++input) {
                deps += findDependencies((*input));
                inputs += Option::fixPathToTargetOS((*input), false);
                if(!tmp_dep_cmd.isEmpty() && doDepends()) {
                    char buff[256];
                    QString dep_cmd = replaceExtraCompilerVariables(tmp_dep_cmd, (*input),
                                                                    tmp_out);
                    dep_cmd = fixEnvVariables(dep_cmd);
                    if(FILE *proc = QT_POPEN(dep_cmd.toLatin1().constData(), "r")) {
                        QString indeps;
                        while(!feof(proc)) {
                            int read_in = (int)fread(buff, 1, 255, proc);
                            if(!read_in)
                                break;
                            indeps += QByteArray(buff, read_in);
                        }
                        QT_PCLOSE(proc);
                        if(!indeps.isEmpty()) {
                            QStringList dep_cmd_deps = indeps.replace('\n', ' ').simplified().split(' ');
                            for(int i = 0; i < dep_cmd_deps.count(); ++i) {
                                QString &file = dep_cmd_deps[i];
                                if(!exists(file)) {
                                    QString localFile;
                                    QList<QMakeLocalFileName> depdirs = QMakeSourceFileInfo::dependencyPaths();
                                    for(QList<QMakeLocalFileName>::Iterator it = depdirs.begin();
                                        it != depdirs.end(); ++it) {
                                        if(exists((*it).real() + Option::dir_sep + file)) {
                                            localFile = (*it).local() + Option::dir_sep + file;
                                            break;
                                        }
                                    }
                                    file = localFile;
                                }
                                if(!file.isEmpty())
                                    file = fileFixify(file);
                            }
                            deps += dep_cmd_deps;
                        }
                    }
                }
            }
            for(int i = 0; i < inputs.size(); ) {
                if(tmp_out == inputs.at(i))
                    inputs.removeAt(i);
                else
                    ++i;
            }
            for(int i = 0; i < deps.size(); ) {
                if(tmp_out == deps.at(i))
                    deps.removeAt(i);
                else
                    ++i;
            }
            if (inputs.isEmpty())
                continue;

            QString cmd = replaceExtraCompilerVariables(tmp_cmd, escapeFilePaths(inputs), QStringList(tmp_out));
            t << escapeDependencyPath(tmp_out) << ":";
            // compiler.CONFIG+=explicit_dependencies means that ONLY compiler.depends gets to cause Makefile dependencies
            if(project->values((*it) + ".CONFIG").indexOf("explicit_dependencies") != -1) {
                t << " " << valList(escapeDependencyPaths(fileFixify(tmp_dep, Option::output_dir, Option::output_dir)));
            } else {
                t << " " << valList(escapeDependencyPaths(inputs)) << " " << valList(escapeDependencyPaths(deps));
            }
            t << "\n\t" << cmd << endl << endl;
            continue;
        }
        for(QStringList::ConstIterator input = tmp_inputs.begin(); input != tmp_inputs.end(); ++input) {
            QString in = Option::fixPathToTargetOS((*input), false);
            QStringList deps = findDependencies((*input));
            deps += escapeDependencyPath(in);
            QString out = replaceExtraCompilerVariables(tmp_out, (*input), QString());
            if(!tmp_dep.isEmpty()) {
                QStringList pre_deps = fileFixify(tmp_dep, Option::output_dir, Option::output_dir);
                for(int i = 0; i < pre_deps.size(); ++i)
                   deps += replaceExtraCompilerVariables(pre_deps.at(i), (*input), out);
            }
            QString cmd = replaceExtraCompilerVariables(tmp_cmd, (*input), out);
            for(QStringList::ConstIterator it3 = vars.constBegin(); it3 != vars.constEnd(); ++it3)
                cmd.replace("$(" + (*it3) + ")", "$(QMAKE_COMP_" + (*it3)+")");
            if(!tmp_dep_cmd.isEmpty() && doDepends()) {
                char buff[256];
                QString dep_cmd = replaceExtraCompilerVariables(tmp_dep_cmd, (*input), out);
                dep_cmd = fixEnvVariables(dep_cmd);
                if(FILE *proc = QT_POPEN(dep_cmd.toLatin1().constData(), "r")) {
                    QString indeps;
                    while(!feof(proc)) {
                        int read_in = (int)fread(buff, 1, 255, proc);
                        if(!read_in)
                            break;
                        indeps += QByteArray(buff, read_in);
                    }
                    QT_PCLOSE(proc);
                    if(!indeps.isEmpty()) {
                        QStringList dep_cmd_deps = indeps.replace('\n', ' ').simplified().split(' ');
                        for(int i = 0; i < dep_cmd_deps.count(); ++i) {
                            QString &file = dep_cmd_deps[i];
                            if(!exists(file)) {
                                QString localFile;
                                QList<QMakeLocalFileName> depdirs = QMakeSourceFileInfo::dependencyPaths();
                                for(QList<QMakeLocalFileName>::Iterator it = depdirs.begin();
                                    it != depdirs.end(); ++it) {
                                    if(exists((*it).real() + Option::dir_sep + file)) {
                                        localFile = (*it).local() + Option::dir_sep + file;
                                        break;
                                    }
                                }
                                file = localFile;
                            }
                            if(!file.isEmpty())
                                file = fileFixify(file);
                        }
                        deps += dep_cmd_deps;
                    }
                }
                //use the depend system to find includes of these included files
                QStringList inc_deps;
                for(int i = 0; i < deps.size(); ++i) {
                    const QString dep = deps.at(i);
                    if(QFile::exists(dep)) {
                        SourceFileType type = TYPE_UNKNOWN;
                        if(type == TYPE_UNKNOWN) {
                            for(QStringList::Iterator cit = Option::c_ext.begin();
                                cit != Option::c_ext.end(); ++cit) {
                                if(dep.endsWith((*cit))) {
                                   type = TYPE_C;
                                   break;
                                }
                            }
                        }
                        if(type == TYPE_UNKNOWN) {
                            for(QStringList::Iterator cppit = Option::cpp_ext.begin();
                                cppit != Option::cpp_ext.end(); ++cppit) {
                                if(dep.endsWith((*cppit))) {
                                    type = TYPE_C;
                                    break;
                                }
                            }
                        }
                        if(type == TYPE_UNKNOWN) {
                            for(QStringList::Iterator hit = Option::h_ext.begin();
                                type == TYPE_UNKNOWN && hit != Option::h_ext.end(); ++hit) {
                                if(dep.endsWith((*hit))) {
                                    type = TYPE_C;
                                    break;
                                }
                            }
                        }
                        if(type != TYPE_UNKNOWN) {
                            if(!QMakeSourceFileInfo::containsSourceFile(dep, type))
                                QMakeSourceFileInfo::addSourceFile(dep, type);
                            inc_deps += QMakeSourceFileInfo::dependencies(dep);
                        }
                    }
                }
                deps += inc_deps;
            }
            for(int i = 0; i < deps.size(); ) {
                QString &dep = deps[i];
                dep = Option::fixPathToTargetOS(unescapeFilePath(dep), false);
                if(out == dep)
                    deps.removeAt(i);
                else
                    ++i;
            }
            t << escapeDependencyPath(out) << ": " << valList(escapeDependencyPaths(deps)) << "\n\t"
              << cmd << endl << endl;
        }
    }
    t << "compiler_clean: " << clean_targets << endl << endl;
}

void
MakefileGenerator::writeExtraCompilerVariables(QTextStream &t)
{
    bool first = true;
    const QStringList &quc = project->values("QMAKE_EXTRA_COMPILERS");
    for(QStringList::ConstIterator it = quc.begin(); it != quc.end(); ++it) {
        const QStringList &vars = project->values((*it) + ".variables");
        for(QStringList::ConstIterator varit = vars.begin(); varit != vars.end(); ++varit) {
            if(first) {
                t << "\n####### Custom Compiler Variables" << endl;
                first = false;
            }
            t << "QMAKE_COMP_" << (*varit) << " = "
              << valList(project->values((*varit))) << endl;
        }
    }
    if(!first)
        t << endl;
}

void
MakefileGenerator::writeExtraVariables(QTextStream &t)
{
    bool first = true;
    QMap<QString, QStringList> &vars = project->variables();
    QStringList &exports = project->values("QMAKE_EXTRA_VARIABLES");
    for(QMap<QString, QStringList>::Iterator it = vars.begin(); it != vars.end(); ++it) {
        for(QStringList::Iterator exp_it = exports.begin(); exp_it != exports.end(); ++exp_it) {
            QRegExp rx((*exp_it), Qt::CaseInsensitive, QRegExp::Wildcard);
            if(rx.exactMatch(it.key())) {
                if(first) {
                    t << "\n####### Custom Variables" << endl;
                    first = false;
                }
                t << "EXPORT_" << it.key() << " = " << it.value().join(" ") << endl;
            }
        }
    }
    if(!first)
        t << endl;
}

bool
MakefileGenerator::writeStubMakefile(QTextStream &t)
{
    t << "QMAKE    = "        << (project->isEmpty("QMAKE_QMAKE") ? QString("qmake") : var("QMAKE_QMAKE")) << endl;
    QStringList &qut = project->values("QMAKE_EXTRA_TARGETS");
    for(QStringList::ConstIterator it = qut.begin(); it != qut.end(); ++it)
        t << *it << " ";
    //const QString ofile = Option::fixPathToTargetOS(fileFixify(Option::output.fileName()));
    t << "first all clean install distclean uninstall: " << "qmake" << endl
      << "qmake_all:" << endl;
    writeMakeQmake(t);
    if(project->isEmpty("QMAKE_NOFORCE"))
        t << "FORCE:" << endl << endl;
    return true;
}

bool
MakefileGenerator::writeMakefile(QTextStream &t)
{
    t << "####### Compile" << endl << endl;
    writeObj(t, "SOURCES");
    writeObj(t, "GENERATED_SOURCES");

    t << "####### Install" << endl << endl;
    writeInstalls(t, "INSTALLS");

    if(project->isEmpty("QMAKE_NOFORCE"))
        t << "FORCE:" << endl << endl;
    return true;
}

QString MakefileGenerator::buildArgs(const QString &outdir)
{
    QString ret;
    //special variables
    if(!project->isEmpty("QMAKE_ABSOLUTE_SOURCE_PATH"))
        ret += " QMAKE_ABSOLUTE_SOURCE_PATH=" + escapeFilePath(project->first("QMAKE_ABSOLUTE_SOURCE_PATH"));

    //warnings
    else if(Option::warn_level == WarnNone)
        ret += " -Wnone";
    else if(Option::warn_level == WarnAll)
        ret += " -Wall";
    else if(Option::warn_level & WarnParser)
        ret += " -Wparser";
    //other options
    if(!Option::user_template.isEmpty())
        ret += " -t " + Option::user_template;
    if(!Option::user_template_prefix.isEmpty())
        ret += " -tp " + Option::user_template_prefix;
    if(!Option::mkfile::do_cache)
        ret += " -nocache";
    if(!Option::mkfile::do_deps)
        ret += " -nodepend";
    if(!Option::mkfile::do_dep_heuristics)
        ret += " -nodependheuristics";
    if(!Option::mkfile::qmakespec_commandline.isEmpty())
        ret += " -spec " + specdir(outdir);
    if(Option::target_mode == Option::TARG_MAC9_MODE)
        ret += " -mac9";
    else if(Option::target_mode == Option::TARG_MACX_MODE)
        ret += " -macx";
    else if(Option::target_mode == Option::TARG_UNIX_MODE)
        ret += " -unix";
    else if(Option::target_mode == Option::TARG_WIN_MODE)
        ret += " -win32";
    else if(Option::target_mode == Option::TARG_QNX6_MODE)
        ret += " -qnx6";

    //configs
    for(QStringList::Iterator it = Option::user_configs.begin();
        it != Option::user_configs.end(); ++it)
        ret += " -config " + (*it);
    //arguments
    for(QStringList::Iterator it = Option::before_user_vars.begin();
        it != Option::before_user_vars.end(); ++it) {
        if((*it).left(qstrlen("QMAKE_ABSOLUTE_SOURCE_PATH")) != "QMAKE_ABSOLUTE_SOURCE_PATH")
            ret += " " + escapeFilePath((*it));
    }
    if(Option::after_user_vars.count()) {
        ret += " -after ";
        for(QStringList::Iterator it = Option::after_user_vars.begin();
            it != Option::after_user_vars.end(); ++it) {
            if((*it).left(qstrlen("QMAKE_ABSOLUTE_SOURCE_PATH")) != "QMAKE_ABSOLUTE_SOURCE_PATH")
                ret += " " + escapeFilePath((*it));
        }
    }
    return ret;
}

//could get stored argv, but then it would have more options than are
//probably necesary this will try to guess the bare minimum..
QString MakefileGenerator::build_args(const QString &outdir)
{
    QString ret = "$(QMAKE)";

    // general options and arguments
    ret += buildArgs(outdir);

    //output
    QString ofile = Option::fixPathToTargetOS(fileFixify(Option::output.fileName()));
    if(!ofile.isEmpty() && ofile != project->first("QMAKE_MAKEFILE"))
        ret += " -o " + escapeFilePath(ofile);

    //inputs
    ret += " " + escapeFilePath(fileFixify(project->projectFile(), outdir));

    return ret;
}

void
MakefileGenerator::writeHeader(QTextStream &t)
{
    t << "#############################################################################" << endl;
    t << "# Makefile for building: " << escapeFilePath(var("TARGET")) << endl;
    t << "# Generated by qmake (" << qmake_version() << ") (Qt " << QT_VERSION_STR << ") on: ";
    t << QDateTime::currentDateTime().toString() << endl;
    t << "# Project:  " << fileFixify(project->projectFile()) << endl;
    t << "# Template: " << var("TEMPLATE") << endl;
    if(!project->isActiveConfig("build_pass"))
        t << "# Command: " << build_args().replace("$(QMAKE)",
                      (project->isEmpty("QMAKE_QMAKE") ? QString("qmake") : var("QMAKE_QMAKE"))) << endl;
    t << "#############################################################################" << endl;
    t << endl;
}

void
MakefileGenerator::writeSubDirs(QTextStream &t)
{
    QList<SubTarget*> targets;
    {
        const QStringList subdirs = project->values("SUBDIRS");
        for(int subdir = 0; subdir < subdirs.size(); ++subdir) {
            QString fixedSubdir = subdirs[subdir];
            fixedSubdir = fixedSubdir.replace(QRegExp("[^a-zA-Z0-9_]"),"-");

            SubTarget *st = new SubTarget;
            st->name = subdirs[subdir];
            targets.append(st);

            bool fromFile = false;
            QString file = subdirs[subdir];
            if(!project->isEmpty(fixedSubdir + ".file")) {
                if(!project->isEmpty(fixedSubdir + ".subdir"))
                    warn_msg(WarnLogic, "Cannot assign both file and subdir for subdir %s",
                             subdirs[subdir].toLatin1().constData());
                file = project->first(fixedSubdir + ".file");
                fromFile = true;
            } else if(!project->isEmpty(fixedSubdir + ".subdir")) {
                file = project->first(fixedSubdir + ".subdir");
                fromFile = false;
            } else {
                fromFile = file.endsWith(Option::pro_ext);
            }
            file = Option::fixPathToTargetOS(file);

            if(fromFile) {
                int slsh = file.lastIndexOf(Option::dir_sep);
                if(slsh != -1) {
                    st->in_directory = file.left(slsh+1);
                    st->profile = file.mid(slsh+1);
                } else {
                    st->profile = file;
                }
            } else {
                if(!file.isEmpty() && !project->isActiveConfig("subdir_first_pro"))
                    st->profile = file.section(Option::dir_sep, -1) + Option::pro_ext;
                st->in_directory = file;
            }
            while(st->in_directory.right(1) == Option::dir_sep)
                st->in_directory = st->in_directory.left(st->in_directory.length() - 1);
            if(fileInfo(st->in_directory).isRelative())
                st->out_directory = st->in_directory;
            else
                st->out_directory = fileFixify(st->in_directory, qmake_getpwd(), Option::output_dir);
            if(!project->isEmpty(fixedSubdir + ".makefile")) {
                st->makefile = project->first(fixedSubdir + ".makefile");
            } else {
                st->makefile = "$(MAKEFILE)";
                if(!st->profile.isEmpty()) {
                    QString basename = st->in_directory;
                    int new_slsh = basename.lastIndexOf(Option::dir_sep);
                    if(new_slsh != -1)
                        basename = basename.mid(new_slsh+1);
                    if(st->profile != basename + Option::pro_ext)
                        st->makefile += "." + st->profile.left(st->profile.length() - Option::pro_ext.length());
                }
            }
            if(!project->isEmpty(fixedSubdir + ".depends")) {
                const QStringList depends = project->values(fixedSubdir + ".depends");
                for(int depend = 0; depend < depends.size(); ++depend) {
                    bool found = false;
                    for(int subDep = 0; subDep < subdirs.size(); ++subDep) {
                        if(subdirs[subDep] == depends.at(depend)) {
                            QString fixedSubDep = subdirs[subDep];
                            fixedSubDep = fixedSubDep.replace(QRegExp("[^a-zA-Z0-9_]"),"-");
                            if(!project->isEmpty(fixedSubDep + ".target")) {
                                st->depends += project->first(fixedSubDep + ".target");
                            } else {
                                QString d = Option::fixPathToLocalOS(subdirs[subDep]);
                                if(!project->isEmpty(fixedSubDep + ".file"))
                                    d = project->first(fixedSubDep + ".file");
                                else if(!project->isEmpty(fixedSubDep + ".subdir"))
                                    d = project->first(fixedSubDep + ".subdir");
                                st->depends += "sub-" + d.replace(QRegExp("[^a-zA-Z0-9_]"),"-");
                            }
                            found = true;
                            break;
                        }
                    }
                    if(!found) {
                        QString depend_str = depends.at(depend);
                        st->depends += depend_str.replace(QRegExp("[^a-zA-Z0-9_]"),"-");
                    }
                }
            }
            if(!project->isEmpty(fixedSubdir + ".target")) {
                st->target = project->first(fixedSubdir + ".target");
            } else {
                st->target = "sub-" + file;
        st->target = st->target.replace(QRegExp("[^a-zA-Z0-9_]"),"-");
            }
        }
    }
    t << "first: make_default" << endl;
    int flags = SubTargetInstalls;
    if(project->isActiveConfig("ordered"))
        flags |= SubTargetOrdered;
    writeSubTargets(t, targets, flags);
    qDeleteAll(targets);
}

void
MakefileGenerator::writeSubTargets(QTextStream &t, QList<MakefileGenerator::SubTarget*> targets, int flags)
{
    // blasted includes
    QStringList &qeui = project->values("QMAKE_EXTRA_INCLUDES");
    for(QStringList::Iterator qeui_it = qeui.begin(); qeui_it != qeui.end(); ++qeui_it)
        t << "include " << (*qeui_it) << endl;

    QString ofile = Option::fixPathToTargetOS(Option::output.fileName());
    if(ofile.lastIndexOf(Option::dir_sep) != -1)
        ofile = ofile.right(ofile.length() - ofile.lastIndexOf(Option::dir_sep) -1);
    t << "MAKEFILE      = " << ofile << endl;
    /* Calling Option::fixPathToTargetOS() is necessary for MinGW/MSYS, which requires
     * back-slashes to be turned into slashes. */
    t << "QMAKE         = " << Option::fixPathToTargetOS(var("QMAKE_QMAKE")) << endl;
    t << "DEL_FILE      = " << var("QMAKE_DEL_FILE") << endl;
    t << "CHK_DIR_EXISTS= " << var("QMAKE_CHK_DIR_EXISTS") << endl;
    t << "MKDIR         = " << var("QMAKE_MKDIR") << endl;
    t << "COPY          = " << var("QMAKE_COPY") << endl;
    t << "COPY_FILE     = " << var("QMAKE_COPY_FILE") << endl;
    t << "COPY_DIR      = " << var("QMAKE_COPY_DIR") << endl;
    t << "INSTALL_FILE  = " << var("QMAKE_INSTALL_FILE") << endl;
    t << "INSTALL_PROGRAM = " << var("QMAKE_INSTALL_PROGRAM") << endl;
    t << "INSTALL_DIR   = " << var("QMAKE_INSTALL_DIR") << endl;
    t << "DEL_FILE      = " << var("QMAKE_DEL_FILE") << endl;
    t << "SYMLINK       = " << var("QMAKE_SYMBOLIC_LINK") << endl;
    t << "DEL_DIR       = " << var("QMAKE_DEL_DIR") << endl;
    t << "MOVE          = " << var("QMAKE_MOVE") << endl;
    t << "CHK_DIR_EXISTS= " << var("QMAKE_CHK_DIR_EXISTS") << endl;
    t << "MKDIR         = " << var("QMAKE_MKDIR") << endl;
    writeExtraVariables(t);
    t << "SUBTARGETS    = ";     // subtargets are sub-directory
    for(int target = 0; target < targets.size(); ++target)
        t << " \\\n\t\t" << targets.at(target)->target;
    t << endl << endl;

    QStringList targetSuffixes;
    const QString abs_source_path = project->first("QMAKE_ABSOLUTE_SOURCE_PATH");
    targetSuffixes << "make_default" << "make_first" << "all" << "clean" << "distclean"
                   << QString((flags & SubTargetInstalls) ? "install_subtargets" : "install")
                   << QString((flags & SubTargetInstalls) ? "uninstall_subtargets" : "uninstall");

    // generate target rules
    for(int target = 0; target < targets.size(); ++target) {
        SubTarget *subtarget = targets.at(target);
        QString in_directory = subtarget->in_directory;
        if(!in_directory.isEmpty() && !in_directory.endsWith(Option::dir_sep))
            in_directory += Option::dir_sep;
        QString out_directory = subtarget->out_directory;
        if(!out_directory.isEmpty() && !out_directory.endsWith(Option::dir_sep))
            out_directory += Option::dir_sep;
        if(!abs_source_path.isEmpty() && out_directory.startsWith(abs_source_path))
            out_directory = Option::output_dir + out_directory.mid(abs_source_path.length());

        QString mkfile = subtarget->makefile;
        if(!in_directory.isEmpty())
            mkfile.prepend(out_directory);

        QString in_directory_cdin, in_directory_cdout, out_directory_cdin, out_directory_cdout;
#define MAKE_CD_IN_AND_OUT(directory) \
        if(!directory.isEmpty()) {               \
            if(project->isActiveConfig("cd_change_global")) { \
                directory ## _cdin = "\n\tcd " + directory + "\n\t";        \
                QDir pwd(Option::output_dir); \
                QStringList in = directory.split(Option::dir_sep), out; \
                for(int i = 0; i < in.size(); i++) { \
                    if(in.at(i) == "..") \
                        out.prepend(fileInfo(pwd.path()).fileName()); \
                    else if(in.at(i) != ".") \
                        out.prepend(".."); \
                    pwd.cd(in.at(i)); \
                } \
                directory ## _cdout = "\n\t@cd " + out.join(Option::dir_sep); \
            } else { \
                directory ## _cdin = "\n\tcd " + directory + " && ";  \
            } \
        } else { \
            directory ## _cdin = "\n\t"; \
        }
        MAKE_CD_IN_AND_OUT(in_directory);
        MAKE_CD_IN_AND_OUT(out_directory);

        //qmake it
        if(!subtarget->profile.isEmpty()) {
            QString out = out_directory + subtarget->makefile,
                     in = fileFixify(in_directory + subtarget->profile, in_directory);
            if(in.startsWith(in_directory))
                in = in.mid(in_directory.length());
            if(out.startsWith(in_directory))
                out = out.mid(in_directory.length());
            t << mkfile << ": " << "\n\t";
            if(!in_directory.isEmpty()) {
                t << mkdir_p_asstring(in_directory)
                  << in_directory_cdin
                  << "$(QMAKE) " << in << buildArgs(in_directory) << " -o " << out
                  << in_directory_cdout << endl;
            } else {
                t << "$(QMAKE) " << in << buildArgs(in_directory) << " -o " << out << endl;
            }
            t << subtarget->target << "-qmake_all: ";
            if(project->isEmpty("QMAKE_NOFORCE"))
                t <<  " FORCE";
            t << "\n\t";
            if(!in_directory.isEmpty()) {
                t << mkdir_p_asstring(in_directory)
                  << in_directory_cdin
                  << "$(QMAKE) " << in << buildArgs(in_directory) << " -o " << out
                  << in_directory_cdout << endl;
            } else {
                t << "$(QMAKE) " << in << buildArgs(in_directory) << " -o " << out << endl;
            }
        }

        QString makefilein = " -f " + subtarget->makefile;

        { //actually compile
            t << subtarget->target << ": " << mkfile;
            if(!subtarget->depends.isEmpty())
                t << " " << valList(subtarget->depends);
            if(project->isEmpty("QMAKE_NOFORCE"))
                t <<  " FORCE";
            t << out_directory_cdin
              << "$(MAKE)" << makefilein
              << out_directory_cdout << endl;
        }

        for(int suffix = 0; suffix < targetSuffixes.size(); ++suffix) {
            QString s = targetSuffixes.at(suffix);
            if(s == "install_subtargets")
                s = "install";
            else if(s == "uninstall_subtargets")
                s = "uninstall";
            else if(s == "make_first")
                s = "first";
            else if(s == "make_default")
                s = QString();

            if(flags & SubTargetOrdered) {
                t << subtarget->target << "-" << targetSuffixes.at(suffix) << "-ordered: " << mkfile;
                if(target)
                    t << " " << targets.at(target-1)->target << "-" << targetSuffixes.at(suffix) << "-ordered ";
                if(project->isEmpty("QMAKE_NOFORCE"))
                    t <<  " FORCE";
                t << out_directory_cdin
                  << "$(MAKE)" << makefilein << " " << s
                  << out_directory_cdout << endl;
            }
            t << subtarget->target << "-" << targetSuffixes.at(suffix) << ": " << mkfile;
            if(!subtarget->depends.isEmpty())
                t << " " << valGlue(subtarget->depends, QString(), "-" + targetSuffixes.at(suffix) + " ",
                                    "-"+targetSuffixes.at(suffix));
            if(project->isEmpty("QMAKE_NOFORCE"))
                t <<  " FORCE";
            t << out_directory_cdin
              << "$(MAKE)" << makefilein << " " << s
              << out_directory_cdout << endl;
        }
    }
    t << endl;

    if(project->values("QMAKE_INTERNAL_QMAKE_DEPS").indexOf("qmake_all") == -1)
        project->values("QMAKE_INTERNAL_QMAKE_DEPS").append("qmake_all");

    writeMakeQmake(t);

    t << "qmake_all:";
    if(!targets.isEmpty()) {
        for(QList<SubTarget*>::Iterator it = targets.begin(); it != targets.end(); ++it) {
            if(!(*it)->profile.isEmpty())
                t << " " << (*it)->target << "-" << "qmake_all";
        }
    }
    if(project->isEmpty("QMAKE_NOFORCE"))
        t <<  " FORCE";
    if(project->isActiveConfig("no_empty_targets"))
        t << "\n\t" << "@cd .";
    t << endl << endl;

    for(int s = 0; s < targetSuffixes.size(); ++s) {
        QString suffix = targetSuffixes.at(s);
        if(!(flags & SubTargetInstalls) && suffix.endsWith("install"))
            continue;

        t << suffix << ":";
        for(int target = 0; target < targets.size(); ++target) {
            QString targetRule = targets.at(target)->target + "-" + suffix;
            if(flags & SubTargetOrdered)
                targetRule += "-ordered";
            t << " " << targetRule;
        }
        if(suffix == "all" || suffix == "make_first")
            t << varGlue("ALL_DEPS"," "," ","");
        if(suffix == "clean")
            t << varGlue("CLEAN_DEPS"," "," ","");
        if(project->isEmpty("QMAKE_NOFORCE"))
            t <<  " FORCE";
        t << endl;
        if(suffix == "clean") {
            t << varGlue("QMAKE_CLEAN","\t-$(DEL_FILE) ","\n\t-$(DEL_FILE) ", "\n");
        } else if(suffix == "distclean") {
            QString ofile = Option::fixPathToTargetOS(fileFixify(Option::output.fileName()));
            if(!ofile.isEmpty())
                t << "\t-$(DEL_FILE) " << ofile << endl;
        } else if(project->isActiveConfig("no_empty_targets")) {
            t << "\t" << "@cd ." << endl;
        }
    }

    // user defined targets
    QStringList &qut = project->values("QMAKE_EXTRA_TARGETS");
    for(QStringList::Iterator qut_it = qut.begin(); qut_it != qut.end(); ++qut_it) {
        QString targ = var((*qut_it) + ".target"),
                 cmd = var((*qut_it) + ".commands"), deps;
        if(targ.isEmpty())
            targ = (*qut_it);
        t << endl;

        QStringList &deplist = project->values((*qut_it) + ".depends");
        for(QStringList::Iterator dep_it = deplist.begin(); dep_it != deplist.end(); ++dep_it) {
            QString dep = var((*dep_it) + ".target");
            if(dep.isEmpty())
                dep = Option::fixPathToTargetOS(*dep_it, false);
            deps += " " + dep;
        }
        if(project->values((*qut_it) + ".CONFIG").indexOf("recursive") != -1) {
            QSet<QString> recurse;
            if(project->isSet((*qut_it) + ".recurse")) {
                recurse = project->values((*qut_it) + ".recurse").toSet();
            } else {
                for(int target = 0; target < targets.size(); ++target)
                    recurse.insert(targets.at(target)->name);
            }
            for(int target = 0; target < targets.size(); ++target) {
                SubTarget *subtarget = targets.at(target);
                QString in_directory = subtarget->in_directory;
                if(!in_directory.isEmpty() && !in_directory.endsWith(Option::dir_sep))
                    in_directory += Option::dir_sep;
                QString out_directory = subtarget->out_directory;
                if(!out_directory.isEmpty() && !out_directory.endsWith(Option::dir_sep))
                    out_directory += Option::dir_sep;
                if(!abs_source_path.isEmpty() && out_directory.startsWith(abs_source_path))
                    out_directory = Option::output_dir + out_directory.mid(abs_source_path.length());

                if(!recurse.contains(subtarget->name))
                    continue;
                QString mkfile = subtarget->makefile;
                if(!in_directory.isEmpty()) {
                    if(!out_directory.endsWith(Option::dir_sep))
                        mkfile.prepend(out_directory + Option::dir_sep);
                    else
                        mkfile.prepend(out_directory);
                }
                QString out_directory_cdin, out_directory_cdout;
                MAKE_CD_IN_AND_OUT(out_directory);

                //don't need the makefile arg if it isn't changed
                QString makefilein;
                if(subtarget->makefile != "$(MAKEFILE)")
                    makefilein = " -f " + subtarget->makefile;

                //write the rule/depends
                if(flags & SubTargetOrdered) {
                    const QString dep = subtarget->target + "-" + (*qut_it) + "_ordered";
                    t << dep << ": " << mkfile;
                    if(target)
                        t << " " << targets.at(target-1)->target << "-" << (*qut_it) << "_ordered ";
                    deps += " " + dep;
                } else {
                    const QString dep = subtarget->target + "-" + (*qut_it);
                    t << dep << ": " << mkfile;
                    if(!subtarget->depends.isEmpty())
                        t << " " << valGlue(subtarget->depends, QString(), "-" + (*qut_it) + " ", "-" + (*qut_it));
                    deps += " " + dep;
                }

                QString sub_targ = targ;
                if(project->isSet((*qut_it) + ".recurse_target"))
                    sub_targ = project->first((*qut_it) + ".recurse_target");

                //write the commands
                if(!out_directory.isEmpty()) {
                    t << out_directory_cdin
                      << "$(MAKE)" << makefilein << " " << sub_targ
                      << out_directory_cdout << endl;
                } else {
                    t << "\n\t"
                      << "$(MAKE)" << makefilein << " " << sub_targ << endl;
                }
            }
        }
        if(project->isEmpty("QMAKE_NOFORCE") &&
           project->values((*qut_it) + ".CONFIG").indexOf("phony") != -1)
            deps += " FORCE";
        t << targ << ":" << deps << "\n";
        if(!cmd.isEmpty())
            t << "\t" << cmd << endl;
    }

    if(flags & SubTargetInstalls) {
        project->values("INSTALLDEPS")   += "install_subtargets";
        project->values("UNINSTALLDEPS") += "uninstall_subtargets";
        writeInstalls(t, "INSTALLS", true);
    }

    if(project->isEmpty("QMAKE_NOFORCE"))
        t << "FORCE:" << endl << endl;
}

void
MakefileGenerator::writeMakeQmake(QTextStream &t)
{
    QString ofile = Option::fixPathToTargetOS(fileFixify(Option::output.fileName()));
    if(project->isEmpty("QMAKE_FAILED_REQUIREMENTS") && !project->isEmpty("QMAKE_INTERNAL_PRL_FILE")) {
        QStringList files = fileFixify(Option::mkfile::project_files);
        t << escapeDependencyPath(project->first("QMAKE_INTERNAL_PRL_FILE")) << ": " << "\n\t"
          << "@$(QMAKE) -prl " << buildArgs() << " " << files.join(" ") << endl;
    }

    QString pfile = project->projectFile();
    if(pfile != "(stdin)") {
        QString qmake = build_args();
        if(!ofile.isEmpty() && !project->isActiveConfig("no_autoqmake")) {
            t << escapeFilePath(ofile) << ": " << escapeDependencyPath(fileFixify(pfile)) << " ";
            if(Option::mkfile::do_cache)
                t <<  escapeDependencyPath(fileFixify(Option::mkfile::cachefile)) << " ";
            if(!specdir().isEmpty()) {
                if(exists(Option::fixPathToLocalOS(specdir()+QDir::separator()+"qmake.conf")))
                    t << escapeDependencyPath(specdir() + Option::dir_sep + "qmake.conf") << " ";
                else if(exists(Option::fixPathToLocalOS(specdir()+QDir::separator()+"tmake.conf")))
                    t << escapeDependencyPath(specdir() + Option::dir_sep + "tmake.conf") << " ";
            }
            const QStringList &included = project->values("QMAKE_INTERNAL_INCLUDED_FILES");
            t << escapeDependencyPaths(included).join(" \\\n\t\t") << "\n\t"
              << qmake << endl;
            for(int include = 0; include < included.size(); ++include) {
                const QString i(included.at(include));
                if(!i.isEmpty())
                    t << i << ":" << endl;
            }
        }
        if(project->first("QMAKE_ORIG_TARGET") != "qmake") {
            t << "qmake: " <<
                project->values("QMAKE_INTERNAL_QMAKE_DEPS").join(" \\\n\t\t");
            if(project->isEmpty("QMAKE_NOFORCE"))
                t <<  " FORCE";
            t << "\n\t" << "@" << qmake << endl << endl;
        }
    }
}

QFileInfo
MakefileGenerator::fileInfo(QString file) const
{
    static QHash<FileInfoCacheKey, QFileInfo> *cache = 0;
    static QFileInfo noInfo = QFileInfo();
    if(!cache) {
        cache = new QHash<FileInfoCacheKey, QFileInfo>;
        qmakeAddCacheClear(qmakeDeleteCacheClear_QHashFileInfoCacheKeyQFileInfo, (void**)&cache);
    }
    FileInfoCacheKey cacheKey(file);
    QFileInfo value = cache->value(cacheKey, noInfo);
    if (value != noInfo)
        return value;

    QFileInfo fi(file);
    if (fi.exists())
        cache->insert(cacheKey, fi);
    return fi;
}

QString
MakefileGenerator::unescapeFilePath(const QString &path) const
{
    QString ret = path;
    if(!ret.isEmpty()) {
        if(ret.contains(QLatin1String("\\ ")))
            ret.replace(QLatin1String("\\ "), QLatin1String(" "));
        if(ret.contains(QLatin1Char('\"')))
            ret.remove(QLatin1Char('\"'));
    }
    return ret;
}

QStringList
MakefileGenerator::escapeFilePaths(const QStringList &paths) const
{
    QStringList ret;
    for(int i = 0; i < paths.size(); ++i)
        ret.append(escapeFilePath(paths.at(i)));
    return ret;
}

QStringList
MakefileGenerator::escapeDependencyPaths(const QStringList &paths) const
{
    QStringList ret;
    for(int i = 0; i < paths.size(); ++i)
        ret.append(escapeDependencyPath(paths.at(i)));
    return ret;
}

QStringList
MakefileGenerator::unescapeFilePaths(const QStringList &paths) const
{
    QStringList ret;
    for(int i = 0; i < paths.size(); ++i)
        ret.append(unescapeFilePath(paths.at(i)));
    return ret;
}

QStringList
MakefileGenerator::fileFixify(const QStringList& files, const QString &out_dir, const QString &in_dir,
                              FileFixifyType fix, bool canon) const
{
    if(files.isEmpty())
        return files;
    QStringList ret;
    for(QStringList::ConstIterator it = files.begin(); it != files.end(); ++it) {
        if(!(*it).isEmpty())
            ret << fileFixify((*it), out_dir, in_dir, fix, canon);
    }
    return ret;
}

QString
MakefileGenerator::fileFixify(const QString& file, const QString &out_d, const QString &in_d,
                              FileFixifyType fix, bool canon) const
{
    if(file.isEmpty())
        return file;
    QString ret = unescapeFilePath(file);

    //setup the cache
    static QHash<FileFixifyCacheKey, QString> *cache = 0;
    if(!cache) {
        cache = new QHash<FileFixifyCacheKey, QString>;
        qmakeAddCacheClear(qmakeDeleteCacheClear_QHashFileFixifyCacheKeyQString, (void**)&cache);
    }
    FileFixifyCacheKey cacheKey(ret, out_d, in_d, fix, canon);
    QString cacheVal = cache->value(cacheKey);
    if(!cacheVal.isNull())
        return cacheVal;

    //do the fixin'
    const QString pwd = qmake_getpwd() + "/";
    QString orig_file = ret;
    if(ret.startsWith(QLatin1Char('~'))) {
        if(ret.startsWith(QLatin1String("~/")))
            ret = QDir::homePath() + Option::dir_sep + ret.mid(1);
        else
            warn_msg(WarnLogic, "Unable to expand ~ in %s", ret.toLatin1().constData());
    }
    if(fix == FileFixifyAbsolute || (fix == FileFixifyDefault && project->isActiveConfig("no_fixpath"))) {
        if(fix == FileFixifyAbsolute && QDir::isRelativePath(ret)) //already absolute
            ret.prepend(pwd);
        ret = Option::fixPathToTargetOS(ret, false, canon);
    } else { //fix it..
        QString qfile(Option::fixPathToLocalOS(ret, true, canon)), in_dir(in_d), out_dir(out_d);
        {
            if(out_dir.isNull() || QDir::isRelativePath(out_dir))
                out_dir.prepend(Option::output_dir + "/");
            else if(out_dir == ".")
                out_dir = pwd;
            if(in_dir.isEmpty() || QDir::isRelativePath(in_dir))
                in_dir.prepend(pwd);
            else if(in_dir == ".")
                in_dir = pwd;
            QFileInfo in_fi(fileInfo(in_dir));
            if(in_fi.exists())
                in_dir = in_fi.canonicalFilePath();
            QFileInfo out_fi(fileInfo(out_dir));
            if(out_fi.exists())
                out_dir = out_fi.canonicalFilePath();
        }

        QFileInfo qfileinfo(fileInfo(qfile));
        if(out_dir != in_dir || !qfileinfo.isRelative()) {
            if(qfileinfo.isRelative()) {
                ret = in_dir + "/" + qfile;
                qfileinfo.setFile(ret);
            }
            ret = Option::fixPathToTargetOS(ret, false, canon);
            if(canon && qfileinfo.exists() &&
               file == Option::fixPathToTargetOS(ret, true, canon))
                ret = Option::fixPathToTargetOS(qfileinfo.canonicalFilePath());
            QString match_dir = Option::fixPathToTargetOS(out_dir, false, canon);
            if(ret == match_dir) {
                ret = "";
            } else if(ret.startsWith(match_dir + Option::dir_sep)) {
                ret = ret.mid(match_dir.length() + Option::dir_sep.length());
            } else {
                //figure out the depth
                int depth = 4;
                if(Option::qmake_mode == Option::QMAKE_GENERATE_MAKEFILE ||
                   Option::qmake_mode == Option::QMAKE_GENERATE_PRL) {
                    if(project && !project->isEmpty("QMAKE_PROJECT_DEPTH"))
                        depth = project->first("QMAKE_PROJECT_DEPTH").toInt();
                    else if(Option::mkfile::cachefile_depth != -1)
                        depth = Option::mkfile::cachefile_depth;
                }
                //calculate how much can be removed
                QString dot_prefix;
                for(int i = 1; i <= depth; i++) {
                    int sl = match_dir.lastIndexOf(Option::dir_sep);
                    if(sl == -1)
                        break;
                    match_dir = match_dir.left(sl);
                    if(match_dir.isEmpty())
                        break;
                    if(ret.startsWith(match_dir + Option::dir_sep)) {
                        //concat
                        int remlen = ret.length() - (match_dir.length() + 1);
                        if(remlen < 0)
                            remlen = 0;
                        ret = ret.right(remlen);
                        //prepend
                        for(int o = 0; o < i; o++)
                            dot_prefix += ".." + Option::dir_sep;
                    }
                }
                ret.prepend(dot_prefix);
            }
        } else {
            ret = Option::fixPathToTargetOS(ret, false, canon);
        }
    }
    if(ret.isEmpty())
        ret = ".";
    debug_msg(3, "Fixed[%d] %s :: to :: %s [%s::%s] [%s::%s]", fix, orig_file.toLatin1().constData(),
              ret.toLatin1().constData(), in_d.toLatin1().constData(), out_d.toLatin1().constData(),
              pwd.toLatin1().constData(), Option::output_dir.toLatin1().constData());
    cache->insert(cacheKey, ret);
    return ret;
}

void
MakefileGenerator::checkMultipleDefinition(const QString &f, const QString &w)
{
    if(!(Option::warn_level & WarnLogic))
        return;
    QString file = f;
    int slsh = f.lastIndexOf(Option::dir_sep);
    if(slsh != -1)
        file = file.right(file.length() - slsh - 1);
    QStringList &l = project->values(w);
    for(QStringList::Iterator val_it = l.begin(); val_it != l.end(); ++val_it) {
        QString file2((*val_it));
        slsh = file2.lastIndexOf(Option::dir_sep);
        if(slsh != -1)
            file2 = file2.right(file2.length() - slsh - 1);
        if(file2 == file) {
            warn_msg(WarnLogic, "Found potential symbol conflict of %s (%s) in %s",
                     file.toLatin1().constData(), (*val_it).toLatin1().constData(), w.toLatin1().constData());
            break;
        }
    }
}

QMakeLocalFileName
MakefileGenerator::fixPathForFile(const QMakeLocalFileName &file, bool forOpen)
{
    if(forOpen)
        return QMakeLocalFileName(fileFixify(file.real(), qmake_getpwd(), Option::output_dir));
    return QMakeLocalFileName(fileFixify(file.real()));
}

QFileInfo
MakefileGenerator::findFileInfo(const QMakeLocalFileName &file)
{
    return fileInfo(file.local());
}

QMakeLocalFileName
MakefileGenerator::findFileForDep(const QMakeLocalFileName &dep, const QMakeLocalFileName &file)
{
    QMakeLocalFileName ret;
    if(!project->isEmpty("SKIP_DEPENDS")) {
        bool found = false;
        QStringList &nodeplist = project->values("SKIP_DEPENDS");
        for(QStringList::Iterator it = nodeplist.begin();
            it != nodeplist.end(); ++it) {
            QRegExp regx((*it));
            if(regx.indexIn(dep.local()) != -1) {
                found = true;
                break;
            }
        }
        if(found)
            return ret;
    }

    ret = QMakeSourceFileInfo::findFileForDep(dep, file);
    if(!ret.isNull())
        return ret;

    //these are some "hacky" heuristics it will try to do on an include
    //however these can be turned off at runtime, I'm not sure how
    //reliable these will be, most likely when problems arise turn it off
    //and see if they go away..
    if(Option::mkfile::do_dep_heuristics) {
        if(depHeuristicsCache.contains(dep.real()))
            return depHeuristicsCache[dep.real()];

        if(Option::output_dir != qmake_getpwd()
           && QDir::isRelativePath(dep.real())) { //is it from the shadow tree
            QList<QMakeLocalFileName> depdirs = QMakeSourceFileInfo::dependencyPaths();
            depdirs.prepend(fileInfo(file.real()).absoluteDir().path());
            QString pwd = qmake_getpwd();
            if(pwd.at(pwd.length()-1) != '/')
                pwd += '/';
            for(int i = 0; i < depdirs.count(); i++) {
                QString dir = depdirs.at(i).real();
                if(!QDir::isRelativePath(dir) && dir.startsWith(pwd))
                    dir = dir.mid(pwd.length());
                if(QDir::isRelativePath(dir)) {
                    if(!dir.endsWith(Option::dir_sep))
                        dir += Option::dir_sep;
                    QString shadow = fileFixify(dir + dep.local(), pwd, Option::output_dir);
                    if(exists(shadow)) {
                        ret = QMakeLocalFileName(shadow);
                        goto found_dep_from_heuristic;
                    }
                }
            }
        }
        { //is it from an EXTRA_TARGET
            const QString dep_basename = dep.local().section(Option::dir_sep, -1);
            QStringList &qut = project->values("QMAKE_EXTRA_TARGETS");
            for(QStringList::Iterator it = qut.begin(); it != qut.end(); ++it) {
                QString targ = var((*it) + ".target");
                if(targ.isEmpty())
                    targ = (*it);
                QString out = Option::fixPathToTargetOS(targ);
                if(out == dep.real() || out.section(Option::dir_sep, -1) == dep_basename) {
                    ret = QMakeLocalFileName(out);
                    goto found_dep_from_heuristic;
                }
            }
        }
        { //is it from an EXTRA_COMPILER
            const QString dep_basename = dep.local().section(Option::dir_sep, -1);
            const QStringList &quc = project->values("QMAKE_EXTRA_COMPILERS");
            for(QStringList::ConstIterator it = quc.begin(); it != quc.end(); ++it) {
                QString tmp_out = project->values((*it) + ".output").first();
                if(tmp_out.isEmpty())
                    continue;
                QStringList &tmp = project->values((*it) + ".input");
                for(QStringList::Iterator it2 = tmp.begin(); it2 != tmp.end(); ++it2) {
                    QStringList &inputs = project->values((*it2));
                    for(QStringList::Iterator input = inputs.begin(); input != inputs.end(); ++input) {
                        QString out = Option::fixPathToTargetOS(unescapeFilePath(replaceExtraCompilerVariables(tmp_out, (*input), QString())));
              if(out == dep.real() || out.section(Option::dir_sep, -1) == dep_basename) {
                            ret = QMakeLocalFileName(fileFixify(out, qmake_getpwd(), Option::output_dir));
                            goto found_dep_from_heuristic;
                        }
                    }
                }
            }
        }
    found_dep_from_heuristic:
        depHeuristicsCache.insert(dep.real(), ret);
    }
    return ret;
}

QStringList
&MakefileGenerator::findDependencies(const QString &file)
{
    const QString fixedFile = fileFixify(file);
    if(!dependsCache.contains(fixedFile)) {
#if 1
        QStringList deps = QMakeSourceFileInfo::dependencies(file);
        if(file != fixedFile)
            deps += QMakeSourceFileInfo::dependencies(fixedFile);
#else
        QStringList deps = QMakeSourceFileInfo::dependencies(fixedFile);
#endif
        dependsCache.insert(fixedFile, deps);
    }
    return dependsCache[fixedFile];
}

QString
MakefileGenerator::specdir(const QString &outdir)
{
#if 0
    if(!spec.isEmpty())
        return spec;
#endif
    spec = fileFixify(Option::mkfile::qmakespec, outdir);
    return spec;
}

bool
MakefileGenerator::openOutput(QFile &file, const QString &build) const
{
    {
        QString outdir;
        if(!file.fileName().isEmpty()) {
            if(QDir::isRelativePath(file.fileName()))
                file.setFileName(Option::output_dir + "/" + file.fileName()); //pwd when qmake was run
            QFileInfo fi(fileInfo(file.fileName()));
            if(fi.isDir())
                outdir = file.fileName() + QDir::separator();
        }
        if(!outdir.isEmpty() || file.fileName().isEmpty()) {
            QString fname = "Makefile";
            if(!project->isEmpty("MAKEFILE"))
               fname = project->first("MAKEFILE");
            file.setFileName(outdir + fname);
        }
    }
    if(QDir::isRelativePath(file.fileName())) {
        QString fname = Option::output_dir;  //pwd when qmake was run
        if(!fname.endsWith("/"))
            fname += "/";
        fname += file.fileName();
        file.setFileName(fname);
    }
    if(!build.isEmpty())
        file.setFileName(file.fileName() + "." + build);
    if(project->isEmpty("QMAKE_MAKEFILE"))
        project->values("QMAKE_MAKEFILE").append(file.fileName());
    int slsh = file.fileName().lastIndexOf(Option::dir_sep);
    if(slsh != -1)
        mkdir(file.fileName().left(slsh));
    if(file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        QFileInfo fi(fileInfo(Option::output.fileName()));
        QString od;
        if(fi.isSymLink())
            od = fileInfo(fi.readLink()).absolutePath();
        else
            od = fi.path();
        od = Option::fixPathToTargetOS(od);
        if(QDir::isRelativePath(od))
            od.prepend(Option::output_dir);
        Option::output_dir = od;
        return true;
    }
    return false;
}

QT_END_NAMESPACE

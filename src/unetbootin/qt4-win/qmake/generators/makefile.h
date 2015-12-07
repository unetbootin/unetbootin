/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the qmake application of the Qt Toolkit.
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

#ifndef MAKEFILE_H
#define MAKEFILE_H

#include "option.h"
#include "project.h"
#include "makefiledeps.h"
#include <qtextstream.h>
#include <qlist.h>
#include <qhash.h>
#include <qfileinfo.h>

QT_BEGIN_NAMESPACE

#ifdef Q_OS_WIN32
#define QT_POPEN _popen
#else
#define QT_POPEN popen
#endif

struct ReplaceExtraCompilerCacheKey
{
    mutable uint hash;
    QString var, in, out, pwd;
    ReplaceExtraCompilerCacheKey(const QString &v, const QStringList &i, const QStringList &o);
    bool operator==(const ReplaceExtraCompilerCacheKey &f) const;
    inline uint hashCode() const {
        if(!hash)
            hash = qHash(var) | qHash(in) | qHash(out) /*| qHash(pwd)*/;
        return hash;
    }
};
inline uint qHash(const ReplaceExtraCompilerCacheKey &f) { return f.hashCode(); }

struct ReplaceExtraCompilerCacheKey;

class MakefileGenerator : protected QMakeSourceFileInfo
{
    QString spec;
    bool init_opath_already, init_already, no_io;
    QHash<QString, bool> init_compiler_already;
    QString build_args(const QString &outdir=QString());
    void checkMultipleDefinition(const QString &, const QString &);

    //internal caches
    mutable QHash<QString, QMakeLocalFileName> depHeuristicsCache;
    mutable QHash<QString, QStringList> dependsCache;
    mutable QHash<ReplaceExtraCompilerCacheKey, QString> extraCompilerVariablesCache;

protected:
    QStringList createObjectList(const QStringList &sources);

    //makefile style generator functions
    void writeObj(QTextStream &, const QString &src);
    void writeInstalls(QTextStream &t, const QString &installs, bool noBuild=false);
    void writeHeader(QTextStream &t);
    void writeSubDirs(QTextStream &t);
    void writeMakeQmake(QTextStream &t);
    void writeExtraVariables(QTextStream &t);
    void writeExtraTargets(QTextStream &t);
    void writeExtraCompilerTargets(QTextStream &t);
    void writeExtraCompilerVariables(QTextStream &t);
    virtual bool writeStubMakefile(QTextStream &t);
    virtual bool writeMakefile(QTextStream &t);

    //generating subtarget makefiles
    struct SubTarget
    {
        QString name;
	QString in_directory, out_directory;
        QString profile, target, makefile;
        QStringList depends;
    };
    enum SubTargetFlags {
        SubTargetInstalls=0x01,
        SubTargetOrdered=0x02,

        SubTargetsNoFlags=0x00
    };
    void writeSubTargets(QTextStream &t, QList<SubTarget*> subtargets, int flags);

    //extra compiler interface
    bool verifyExtraCompiler(const QString &c, const QString &f);
    virtual QString replaceExtraCompilerVariables(const QString &, const QStringList &, const QStringList &);
    inline QString replaceExtraCompilerVariables(const QString &val, const QString &in, const QString &out)
    { return replaceExtraCompilerVariables(val, QStringList(in), QStringList(out)); }

    //interface to the source file info
    QMakeLocalFileName fixPathForFile(const QMakeLocalFileName &, bool);
    QMakeLocalFileName findFileForDep(const QMakeLocalFileName &, const QMakeLocalFileName &);
    QFileInfo findFileInfo(const QMakeLocalFileName &);
    QMakeProject *project;

    //escape
    virtual QString unescapeFilePath(const QString &path) const;
    virtual QStringList unescapeFilePaths(const QStringList &path) const;
    virtual QString escapeFilePath(const QString &path) const { return path; }
    virtual QString escapeDependencyPath(const QString &path) const { return escapeFilePath(path); }
    QStringList escapeFilePaths(const QStringList &paths) const;
    QStringList escapeDependencyPaths(const QStringList &paths) const;

    //initialization
    void verifyCompilers();
    virtual void init();
    void initOutPaths();
    struct Compiler
    {
        QString variable_in;
        enum CompilerFlag {
            CompilerNoFlags       = 0x00,
            CompilerBuiltin       = 0x01,
            CompilerNoCheckDeps   = 0x02,
            CompilerRemoveNoExist = 0x04
        };
        uint flags, type;
    };
    void initCompiler(const Compiler &comp);
    enum VPATHFlag {
        VPATH_NoFlag             = 0x00,
        VPATH_WarnMissingFiles   = 0x01,
        VPATH_RemoveMissingFiles = 0x02,
        VPATH_NoFixify           = 0x04
    };
    QStringList findFilesInVPATH(QStringList l, uchar flags, const QString &var="");

    inline int findExecutable(const QStringList &cmdline)
    { int ret; canExecute(cmdline, &ret); return ret; }
    bool canExecute(const QStringList &cmdline, int *argv0) const;
    inline bool canExecute(const QString &cmdline) const
    { return canExecute(cmdline.split(' '), 0); }

    bool mkdir(const QString &dir) const;
    QString mkdir_p_asstring(const QString &dir, bool escape=true) const;

    //subclasses can use these to query information about how the generator was "run"
    QString buildArgs(const QString &outdir=QString());
    QString specdir(const QString &outdir=QString());

    virtual QStringList &findDependencies(const QString &file);
    virtual bool doDepends() const { return Option::mkfile::do_deps; }

    void filterIncludedFiles(const QString &);
    virtual void processSources() {
	filterIncludedFiles("SOURCES");
	filterIncludedFiles("GENERATED_SOURCES");
    }

    //for cross-platform dependent directories
    virtual void usePlatformDir();

    //for installs
    virtual QString defaultInstall(const QString &);

    //for prl
    QString prlFileName(bool fixify=true);
    void writePrlFile();
    bool processPrlFile(QString &);
    virtual void processPrlVariable(const QString &, const QStringList &);
    virtual void processPrlFiles();
    virtual void writePrlFile(QTextStream &);

    //make sure libraries are found
    virtual bool findLibraries();

    //for retrieving values and lists of values
    virtual QString var(const QString &var);
    QString varGlue(const QString &var, const QString &before, const QString &glue, const QString &after);
    QString varList(const QString &var);
    QString val(const QStringList &varList);
    QString valGlue(const QStringList &varList, const QString &before, const QString &glue, const QString &after);
    QString valList(const QStringList &varList);

    QString filePrefixRoot(const QString &, const QString &);

    //file fixification to unify all file names into a single pattern
    enum FileFixifyType { FileFixifyAbsolute, FileFixifyRelative, FileFixifyDefault };
    QString fileFixify(const QString& file, const QString &out_dir=QString(),
                       const QString &in_dir=QString(), FileFixifyType fix=FileFixifyDefault, bool canon=true) const;
    inline QString fileFixify(const QString& file, FileFixifyType fix, bool canon=true) const
    { return fileFixify(file, QString(), QString(), fix, canon); }
    QStringList fileFixify(const QStringList& files, const QString &out_dir=QString(),
                           const QString &in_dir=QString(), FileFixifyType fix=FileFixifyDefault, bool canon=true) const;
    inline QStringList fileFixify(const QStringList& files, FileFixifyType fix, bool canon=true) const
    { return fileFixify(files, QString(), QString(), fix, canon); }

public:
    MakefileGenerator();
    virtual ~MakefileGenerator();
    QMakeProject *projectFile() const;
    void setProjectFile(QMakeProject *p);

    void setNoIO(bool o);
    bool noIO() const;

    inline bool exists(QString file) const { return fileInfo(file).exists(); }
    QFileInfo fileInfo(QString file) const;

    static MakefileGenerator *create(QMakeProject *);
    virtual bool write();
    virtual bool writeProjectMakefile();
    virtual bool supportsMetaBuild() { return true; }
    virtual bool supportsMergedBuilds() { return false; }
    virtual bool mergeBuildProject(MakefileGenerator * /*other*/) { return false; }
    virtual bool openOutput(QFile &, const QString &build) const;
    virtual bool isWindowsShell() const { return Option::target_mode == Option::TARG_WIN_MODE; }
};

inline void MakefileGenerator::setNoIO(bool o)
{ no_io = o; }

inline bool MakefileGenerator::noIO() const
{ return no_io; }

inline QString MakefileGenerator::defaultInstall(const QString &)
{ return QString(""); }

inline bool MakefileGenerator::findLibraries()
{ return true; }

inline MakefileGenerator::~MakefileGenerator()
{ }

QT_END_NAMESPACE

#endif // MAKEFILE_H

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

#ifndef PROJECT_H
#define PROJECT_H

#include <qstringlist.h>
#include <qtextstream.h>
#include <qstring.h>
#include <qstack.h>
#include <qmap.h>
#include <qmetatype.h>

#ifndef QT_BUILD_QMAKE_LIBRARY
#  define QTSCRIPT_SUPPORT
#endif

#ifdef QTSCRIPT_SUPPORT
# include <qscriptengine.h>
#endif

QT_BEGIN_NAMESPACE

class QMakeProperty;

struct ParsableBlock;
struct IteratorBlock;
struct FunctionBlock;

class QMakeProject
{
    struct ScopeBlock
    {
        enum TestStatus { TestNone, TestFound, TestSeek };
        ScopeBlock() : iterate(0), ignore(false), else_status(TestNone) { }
        ScopeBlock(bool i) : iterate(0), ignore(i), else_status(TestNone) { }
        ~ScopeBlock();
        IteratorBlock *iterate;
        uint ignore : 1, else_status : 2;
    };
    friend struct ParsableBlock;
    friend struct IteratorBlock;
    friend struct FunctionBlock;

#ifdef QTSCRIPT_SUPPORT
    QScriptEngine eng;
#endif
    QStack<ScopeBlock> scope_blocks;
    QStack<FunctionBlock *> function_blocks;
    IteratorBlock *iterator;
    FunctionBlock *function;
    QMap<QString, FunctionBlock*> testFunctions, replaceFunctions;

    bool own_prop;
    QString pfile, cfile;
    QMakeProperty *prop;
    void reset();
    QMap<QString, QStringList> vars, base_vars, cache;
    bool parse(const QString &text, QMap<QString, QStringList> &place, int line_count=1);

    enum IncludeStatus {
        IncludeSuccess,
        IncludeFeatureAlreadyLoaded,
        IncludeFailure,
        IncludeNoExist,
        IncludeParseFailure
    };
    enum IncludeFlags {
        IncludeFlagNone = 0x00,
        IncludeFlagFeature = 0x01,
        IncludeFlagNewParser = 0x02,
        IncludeFlagNewProject = 0x04
    };
    IncludeStatus doProjectInclude(QString file, uchar flags, QMap<QString, QStringList> &place);

    bool doProjectCheckReqs(const QStringList &deps, QMap<QString, QStringList> &place);
    bool doVariableReplace(QString &str, QMap<QString, QStringList> &place);
    QStringList doVariableReplaceExpand(const QString &str, QMap<QString, QStringList> &place, bool *ok=0);
    void init(QMakeProperty *, const QMap<QString, QStringList> *);
    QStringList &values(const QString &v, QMap<QString, QStringList> &place);

public:
    QMakeProject() { init(0, 0); }
    QMakeProject(QMakeProperty *p) { init(p, 0); }
    QMakeProject(QMakeProject *p, const QMap<QString, QStringList> *nvars=0);
    QMakeProject(const QMap<QString, QStringList> &nvars) { init(0, &nvars); }
    QMakeProject(QMakeProperty *p, const QMap<QString, QStringList> &nvars) { init(p, &nvars); }
    ~QMakeProject();

    enum { ReadCache=0x01, ReadConf=0x02, ReadCmdLine=0x04, ReadProFile=0x08,
           ReadPostFiles=0x10, ReadFeatures=0x20, ReadConfigs=0x40, ReadAll=0xFF };
    inline bool parse(const QString &text) { return parse(text, vars); }
    bool read(const QString &project, uchar cmd=ReadAll);
    bool read(uchar cmd=ReadAll);

    QStringList userExpandFunctions() { return replaceFunctions.keys(); }
    QStringList userTestFunctions() { return testFunctions.keys(); }

    QString projectFile();
    QString configFile();
    inline QMakeProperty *properties() { return prop; }

    bool doProjectTest(QString str, QMap<QString, QStringList> &place);
    bool doProjectTest(QString func, const QString &params,
                       QMap<QString, QStringList> &place);
    bool doProjectTest(QString func, QStringList args,
                       QMap<QString, QStringList> &place);
    bool doProjectTest(QString func, QList<QStringList> args,
                       QMap<QString, QStringList> &place);
    QStringList doProjectExpand(QString func, const QString &params,
                                QMap<QString, QStringList> &place);
    QStringList doProjectExpand(QString func, QStringList args,
                                QMap<QString, QStringList> &place);
    QStringList doProjectExpand(QString func, QList<QStringList> args,
                                QMap<QString, QStringList> &place);

    QStringList expand(const QString &v);
    QStringList expand(const QString &func, const QList<QStringList> &args);
    bool test(const QString &v);
    bool test(const QString &func, const QList<QStringList> &args);
    bool isActiveConfig(const QString &x, bool regex=false,
                        QMap<QString, QStringList> *place=NULL);

    bool isSet(const QString &v);
    bool isEmpty(const QString &v);
    QStringList &values(const QString &v);
    QString first(const QString &v);
    QMap<QString, QStringList> &variables();

protected:
    friend class MakefileGenerator;
    bool read(const QString &file, QMap<QString, QStringList> &place);
    bool read(QTextStream &file, QMap<QString, QStringList> &place);

};
Q_DECLARE_METATYPE(QMakeProject*)

inline QString QMakeProject::projectFile()
{
    if (pfile == "-")
        return QString("(stdin)");
    return pfile;
}

inline QString QMakeProject::configFile()
{ return cfile; }

inline QStringList &QMakeProject::values(const QString &v)
{ return values(v, vars); }

inline bool QMakeProject::isEmpty(const QString &v)
{ return !isSet(v) || values(v).isEmpty(); }

inline bool QMakeProject::isSet(const QString &v)
{ return vars.contains(v); }

inline QString QMakeProject::first(const QString &v)
{
    const QStringList vals = values(v);
    if(vals.isEmpty())
        return QString("");
    return vals.first();
}

inline QMap<QString, QStringList> &QMakeProject::variables()
{ return vars; }

QT_END_NAMESPACE

#endif // PROJECT_H

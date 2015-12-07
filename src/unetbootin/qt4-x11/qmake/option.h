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

#ifndef OPTION_H
#define OPTION_H

#include "project.h"
#include <qstring.h>
#include <qstringlist.h>
#include <qfile.h>

QT_BEGIN_NAMESPACE

#define QMAKE_VERSION_MAJOR 2
#define QMAKE_VERSION_MINOR 1
#define QMAKE_VERSION_PATCH 0
const char *qmake_version();

QString qmake_getpwd();
bool qmake_setpwd(const QString &p);

#define debug_msg if(Option::debug_level) debug_msg_internal
void debug_msg_internal(int level, const char *fmt, ...); //don't call directly, use debug_msg
enum QMakeWarn {
    WarnNone    = 0x00,
    WarnParser  = 0x01,
    WarnLogic   = 0x02,
    WarnAll     = 0xFF
};
void warn_msg(QMakeWarn t, const char *fmt, ...);

struct Option
{
    //simply global convenience
    static QString js_ext;
    static QString libtool_ext;
    static QString pkgcfg_ext;
    static QString prf_ext;
    static QString prl_ext;
    static QString ui_ext;
    static QStringList h_ext;
    static QStringList cpp_ext;
    static QStringList c_ext;
    static QString h_moc_ext;
    static QString cpp_moc_ext;
    static QString obj_ext;
    static QString lex_ext;
    static QString yacc_ext;
    static QString h_moc_mod;
    static QString cpp_moc_mod;
    static QString lex_mod;
    static QString yacc_mod;
    static QString dir_sep;
    static QString dirlist_sep;
    static QString sysenv_mod;
    static QString pro_ext;
    static QString res_ext;
    static char field_sep;
    static const char *application_argv0;

    enum CmdLineFlags {
        QMAKE_CMDLINE_SUCCESS       = 0x00,
        QMAKE_CMDLINE_SHOW_USAGE    = 0x01,
        QMAKE_CMDLINE_BAIL          = 0x02,
        QMAKE_CMDLINE_ERROR         = 0x04
    };

    //both of these must be called..
    static int init(int argc=0, char **argv=0); //parse cmdline
    static bool postProcessProject(QMakeProject *);

    enum StringFixFlags {
        FixNone                 = 0x00,
        FixEnvVars              = 0x01,
        FixPathCanonicalize     = 0x02,
        FixPathToLocalSeparators  = 0x04,
        FixPathToTargetSeparators = 0x08
    };
    static QString fixString(QString string, uchar flags);

    //and convenience functions
    inline static QString fixPathToLocalOS(const QString &in, bool fix_env=true, bool canonical=true)
    {
        uchar flags = FixPathToLocalSeparators;
        if(fix_env)
            flags |= FixEnvVars;
        if(canonical)
            flags |= FixPathCanonicalize;
        return fixString(in, flags);
    }
    inline static QString fixPathToTargetOS(const QString &in, bool fix_env=true, bool canonical=true)
    {
        uchar flags = FixPathToTargetSeparators;
        if(fix_env)
            flags |= FixEnvVars;
        if(canonical)
            flags |= FixPathCanonicalize;
        return fixString(in, flags);
    }

    //global qmake mode, can only be in one mode per invocation!
    enum QMAKE_MODE { QMAKE_GENERATE_NOTHING, QMAKE_GENERATE_PROJECT, QMAKE_GENERATE_MAKEFILE,
                      QMAKE_GENERATE_PRL, QMAKE_SET_PROPERTY, QMAKE_QUERY_PROPERTY };
    static QMAKE_MODE qmake_mode;

    //all modes
    static QString qmake_abslocation;
    static QFile output;
    static QString output_dir;
    static int debug_level;
    static int warn_level;
    static bool recursive;
    static QStringList before_user_vars, after_user_vars, user_configs, after_user_configs;
    enum TARG_MODE { TARG_UNIX_MODE, TARG_WIN_MODE, TARG_MACX_MODE, TARG_MAC9_MODE, TARG_QNX6_MODE };
    static TARG_MODE target_mode;
    static QString user_template, user_template_prefix;
    static QStringList shellPath;

    //QMAKE_*_PROPERTY options
    struct prop {
        static QStringList properties;
    };

    //QMAKE_GENERATE_PROJECT options
    struct projfile {
        static bool do_pwd;
        static QStringList project_dirs;
    };

    //QMAKE_GENERATE_MAKEFILE options
    struct mkfile {
        static QString qmakespec;
        static bool do_cache;
        static bool do_deps;
        static bool do_mocs;
        static bool do_dep_heuristics;
        static bool do_preprocess;
        static bool do_stub_makefile;
        static QString cachefile;
        static int cachefile_depth;
        static QStringList project_files;
        static QString qmakespec_commandline;
    };

private:
    static int parseCommandLine(int, char **, int=0);
};

inline QString fixEnvVariables(const QString &x) { return Option::fixString(x, Option::FixEnvVars); }
inline QStringList splitPathList(const QString &paths) { return paths.split(Option::dirlist_sep); }


// this is a stripped down version of the one found in QtCore
class QLibraryInfo
{
public:
    enum LibraryLocation
    {
        PrefixPath,
        DocumentationPath,
        HeadersPath,
        LibrariesPath,
        BinariesPath,
        PluginsPath,
        DataPath,
        TranslationsPath,
        SettingsPath,
        DemosPath,
        ExamplesPath
    };
    static QString location(LibraryLocation);
};

QT_END_NAMESPACE

#endif // OPTION_H

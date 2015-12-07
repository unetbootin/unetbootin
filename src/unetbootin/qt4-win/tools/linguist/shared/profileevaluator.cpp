/****************************************************************************
**
** Copyright (C) 2006-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the Qt Linguist of the Qt Toolkit.
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

#include "profileevaluator.h"
#include "proreader.h"
#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtCore/QSet>
#include <QtCore/QRegExp>
#include <QtCore/QByteArray>
#include <QtCore/QFileInfo>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include "proparserutils.h"

#ifdef Q_OS_UNIX
#include <unistd.h>
#endif

#ifdef Q_OS_WIN32
#define QT_POPEN _popen
#else
#define QT_POPEN popen
#endif

QT_BEGIN_NAMESPACE

QStringList ProFileEvaluator::qmake_feature_paths(/*QMakeProperty *prop=0*/)
{
    QStringList concat;
    {
        const QString base_concat = QDir::separator() + QString(QLatin1String("features"));
        concat << base_concat + QDir::separator() + QLatin1String("mac");
        concat << base_concat + QDir::separator() + QLatin1String("macx");
        concat << base_concat + QDir::separator() + QLatin1String("unix");
        concat << base_concat + QDir::separator() + QLatin1String("win32");
        concat << base_concat + QDir::separator() + QLatin1String("mac9");
        concat << base_concat + QDir::separator() + QLatin1String("qnx6");
        concat << base_concat;
    }
    const QString mkspecs_concat = QDir::separator() + QString(QLatin1String("mkspecs"));
    QStringList feature_roots;
    QByteArray mkspec_path = qgetenv("QMAKEFEATURES");
    if(!mkspec_path.isNull())
        feature_roots += splitPathList(QString::fromLocal8Bit(mkspec_path));
    /*
    if(prop)
        feature_roots += splitPathList(prop->value("QMAKEFEATURES"));
    if(!Option::mkfile::cachefile.isEmpty()) {
        QString path;
        int last_slash = Option::mkfile::cachefile.lastIndexOf(Option::dir_sep);
        if(last_slash != -1)
            path = Option::fixPathToLocalOS(Option::mkfile::cachefile.left(last_slash));
        for(QStringList::Iterator concat_it = concat.begin();
            concat_it != concat.end(); ++concat_it)
            feature_roots << (path + (*concat_it));
    }
    */

    QByteArray qmakepath = qgetenv("QMAKEPATH");
    if (!qmakepath.isNull()) {
        const QStringList lst = splitPathList(QString::fromLocal8Bit(qmakepath));
        for(QStringList::ConstIterator it = lst.begin(); it != lst.end(); ++it) {
            for(QStringList::Iterator concat_it = concat.begin();
                concat_it != concat.end(); ++concat_it)
                    feature_roots << ((*it) + mkspecs_concat + (*concat_it));
        }
    }
    //if(!Option::mkfile::qmakespec.isEmpty())
    //    feature_roots << Option::mkfile::qmakespec + QDir::separator() + "features";
    //if(!Option::mkfile::qmakespec.isEmpty()) {
    //    QFileInfo specfi(Option::mkfile::qmakespec);
    //    QDir specdir(specfi.absoluteFilePath());
    //    while(!specdir.isRoot()) {
    //        if(!specdir.cdUp() || specdir.isRoot())
    //            break;
    //        if(QFile::exists(specdir.path() + QDir::separator() + "features")) {
    //            for(QStringList::Iterator concat_it = concat.begin();
    //                concat_it != concat.end(); ++concat_it)
    //                feature_roots << (specdir.path() + (*concat_it));
    //            break;
    //        }
    //    }
    //}
    for(QStringList::Iterator concat_it = concat.begin();
        concat_it != concat.end(); ++concat_it)
        feature_roots << (propertyValue(QLatin1String("QT_INSTALL_PREFIX")) +
                          mkspecs_concat + (*concat_it));
    for(QStringList::Iterator concat_it = concat.begin();
        concat_it != concat.end(); ++concat_it)
        feature_roots << (propertyValue(QLatin1String("QT_INSTALL_DATA")) +
                          mkspecs_concat + (*concat_it));
    return feature_roots;
}

ProFile *ProFileEvaluator::currentProFile() const
{
    if (m_profileStack.count() > 0) {
        return m_profileStack.top();
    }
    return 0;
}

QString ProFileEvaluator::currentFileName() const
{
    ProFile *pro = currentProFile();
    if (pro) return pro->fileName();
    return QString();
}

QString ProFileEvaluator::getcwd() const
{
    ProFile *cur = m_profileStack.top();
    QFileInfo fi(cur->fileName());
    return fi.absolutePath();
}

ProFileEvaluator::ProFileEvaluator()
{
    Option::init();
}

ProFileEvaluator::~ProFileEvaluator()
{
}

bool ProFileEvaluator::visitBeginProFile(ProFile * pro)
{
    PRE(pro);
    bool ok = true;
    m_lineNo = pro->getLineNumber();
    if (m_oldPath.isEmpty()) {
        // change the working directory for the initial profile we visit, since
        // that is *the* profile. All the other times we reach this function will be due to
        // include(file) or load(file)
        m_oldPath = QDir::currentPath();
        m_profileStack.push(pro);
        QString fn = pro->fileName();
        ok = QDir::setCurrent(QFileInfo(fn).absolutePath());
    }

    if (m_origfile.isEmpty())
        m_origfile = pro->fileName();

    return ok;
}

bool ProFileEvaluator::visitEndProFile(ProFile * pro)
{
    PRE(pro);
    bool ok = true;
    m_lineNo = pro->getLineNumber();
    if (m_profileStack.count() == 1 && !m_oldPath.isEmpty()) {
        m_profileStack.pop();
        ok = QDir::setCurrent(m_oldPath);
    }
    return ok;
}

bool ProFileEvaluator::visitProValue(ProValue *value)
{
    PRE(value);
    m_lineNo = value->getLineNumber();
    QString val(QString::fromLatin1(value->value()));

    QByteArray varName = m_lastVarName;

    QStringList v = expandVariableReferences(val);

    // The following two blocks fix bug 180128 by making all "interesting"
    // file name absolute in each .pro file, not just the top most one
    if (varName == QByteArray("SOURCES")
            || varName == QByteArray("HEADERS")
            || varName == QByteArray("INTERFACES")
            || varName == QByteArray("FORMS")
            || varName == QByteArray("FORMS3")) {
        // matches only existent files, expand certain(?) patterns
        QStringList vv;
        for (int i = v.count(); --i >= 0; )
            vv << expandPattern(v[i]);
        v = vv;
    }

    if (varName == QByteArray("TRANSLATIONS")) {
        // also matches non-existent files, but does not expand pattern
        QString dir = QFileInfo(currentFileName()).absolutePath();
        dir += '/';
        for (int i = v.count(); --i >= 0; )
            v[i] = QFileInfo(dir, v[i]).absoluteFilePath();
    }

    switch (m_variableOperator) {
        case ProVariable::UniqueAddOperator:    // *
            insertUnique(&m_valuemap, varName, v, true);
            break;
        case ProVariable::SetOperator:          // =
        case ProVariable::AddOperator:          // +
            insertUnique(&m_valuemap, varName, v, false);
            break;
        case ProVariable::RemoveOperator:       // -
            // fix me: interaction between AddOperator and RemoveOperator
            insertUnique(&m_valuemap, QByteArray(varName).prepend(QLatin1Char('-').toLatin1()), v, false);
            break;
        case ProVariable::ReplaceOperator:      // ~
            {
                // DEFINES ~= s/a/b/?[gqi]
                QChar sep = val.at(1);
                QStringList func = val.split(sep);
                if (func.count() < 3 || func.count() > 4) {
                    logMessage(QString::fromAscii(
                        "~= operator (function s///) expects 3 or 4 arguments.\n"),
                        MT_DebugLevel1);
                    return false;
                }
                if (func[0] != QLatin1String("s")) {
                    logMessage(QString::fromAscii("~= operator can only handle s/// function.\n"),
                        MT_DebugLevel1);
                    return false;
                }
                bool global = false, quote = false, case_sense = false;

                if (func.count() == 4) {
                    global = func[3].indexOf(QLatin1Char('g')) != -1;
                    case_sense = func[3].indexOf(QLatin1Char('i')) == -1;
                    quote = func[3].indexOf(QLatin1Char('q')) != -1;
                }
                QString pattern = func[1];
                QString replace = func[2];
                if(quote)
                    pattern = QRegExp::escape(pattern);
                QRegExp regexp(pattern, case_sense ? Qt::CaseSensitive : Qt::CaseInsensitive);

                QStringList varlist = m_valuemap.value(varName);
                for(QStringList::Iterator varit = varlist.begin(); varit != varlist.end();) {
                    if((*varit).contains(regexp)) {
                        (*varit) = (*varit).replace(regexp, replace);
                        if ((*varit).isEmpty())
                            varit = varlist.erase(varit);
                        else
                            ++varit;
                        if(!global)
                            break;
                    } else
                        ++varit;
                }
            }
            break;

    }
    return true;
}


bool ProFileEvaluator::visitProFunction(ProFunction *func)
{
    m_lineNo = func->getLineNumber();
    bool result = true;
    bool ok = true;
    QByteArray text = func->text();
    int lparen = text.indexOf('(');
    int rparen = text.lastIndexOf(')');
    Q_ASSERT(lparen < rparen);

    QString arguments(QString::fromLatin1(text.mid(lparen + 1, rparen - lparen - 1)));
    QByteArray funcName = text.left(lparen);
    ok &= evaluateConditionalFunction(funcName.trimmed(), arguments, &result);
    return ok;
}

bool ProFileEvaluator::visitBeginProBlock(ProBlock * block)
{
    if (block->blockKind() == ProBlock::ScopeKind) {
        m_invertNext = false;
        m_condition = false;
    }
    return true;
}
bool ProFileEvaluator::visitEndProBlock(ProBlock * /*block*/)
{
    return true;
}

bool ProFileEvaluator::visitBeginProVariable(ProVariable *variable)
{
    m_lastVarName = variable->variable();
    m_variableOperator = variable->variableOperator();
    return true;
}
bool ProFileEvaluator::visitEndProVariable(ProVariable * /*variable*/)
{
    m_lastVarName.clear();
    return true;
}

bool ProFileEvaluator::visitProOperator(ProOperator * oper)
{
    m_invertNext = (oper->operatorKind() == ProOperator::NotOperator);
    return true;
}
bool ProFileEvaluator::visitProCondition(ProCondition * cond)
{
    if (!m_condition) {
        if (m_invertNext) {
            m_condition |= !isActiveConfig(cond->text(), true);
        } else {
            m_condition |= isActiveConfig(cond->text(), true);
        }
    }
    return true;
}


QStringList ProFileEvaluator::expandVariableReferences(const QString &str)
{
    bool fOK;
    bool *ok = &fOK;
    QStringList ret;
    if(ok)
        *ok = true;
    if(str.isEmpty())
        return ret;

    const ushort LSQUARE = '[';
    const ushort RSQUARE = ']';
    const ushort LCURLY = '{';
    const ushort RCURLY = '}';
    const ushort LPAREN = '(';
    const ushort RPAREN = ')';
    const ushort DOLLAR = '$';
    const ushort BACKSLASH = '\\';
    const ushort UNDERSCORE = '_';
    const ushort DOT = '.';
    const ushort SPACE = ' ';
    const ushort TAB = '\t';

    ushort unicode;
    const QChar *str_data = str.data();
    const int str_len = str.length();

    ushort term;
    QString var, args;

    int replaced = 0;
    QString current;
    for(int i = 0; i < str_len; ++i) {
        unicode = (str_data+i)->unicode();
        const int start_var = i;
        if (unicode == BACKSLASH) {
            bool escape = false;
            const char *symbols = "[]{}()$\\";
            for(const char *s = symbols; *s; ++s) {
                if(*(str_data+i+1) == (ushort)*s) {
                    i++;
                    escape = true;
                    if(!(replaced++))
                        current = str.left(start_var);
                    current.append(str.at(i));
                    break;
                }
            }
            if(!escape && replaced)
                current.append(QChar(unicode));
            continue;
        }
        if(unicode == SPACE || unicode == TAB) {
            unicode = 0;
            if(!current.isEmpty()) {
                unquote(&current);
                ret.append(current);
                current.clear();
            }
        } else if(unicode == DOLLAR && str_len > i+2) {
            unicode = (str_data+(++i))->unicode();
            if(unicode == DOLLAR) {
                term = 0;
                var.clear();
                args.clear();
                enum { VAR, ENVIRON, FUNCTION, PROPERTY } var_type = VAR;
                unicode = (str_data+(++i))->unicode();
                if(unicode == LSQUARE) {
                    unicode = (str_data+(++i))->unicode();
                    term = RSQUARE;
                    var_type = PROPERTY;
                } else if(unicode == LCURLY) {
                    unicode = (str_data+(++i))->unicode();
                    var_type = VAR;
                    term = RCURLY;
                } else if(unicode == LPAREN) {
                    unicode = (str_data+(++i))->unicode();
                    var_type = ENVIRON;
                    term = RPAREN;
                }
                while(1) {
                    if(!(unicode & (0xFF<<8)) &&
                       unicode != DOT && unicode != UNDERSCORE &&
                       (unicode < 'a' || unicode > 'z') && (unicode < 'A' || unicode > 'Z') &&
                       (unicode < '0' || unicode > '9'))
                        break;
                    var.append(QChar(unicode));
                    if(++i == str_len)
                        break;
                    unicode = (str_data+i)->unicode();
                }
                if(var_type == VAR && unicode == LPAREN) {
                    var_type = FUNCTION;
                    int depth = 0;
                    while(1) {
                        if(++i == str_len)
                            break;
                        unicode = (str_data+i)->unicode();
                        if(unicode == LPAREN) {
                            depth++;
                        } else if(unicode == RPAREN) {
                            if(!depth)
                                break;
                            --depth;
                        }
                        args.append(QChar(unicode));
                    }
                    if(i < str_len-1)
                        unicode = (str_data+(++i))->unicode();
                    else
                        unicode = 0;
                }
                if(term) {
                    if(unicode != term) {
                        logMessage(QString::fromAscii(
                            "Missing %1 terminator [found %2]").arg(QChar(term)).arg(QChar(unicode)),
                            MT_DebugLevel1);
                        if(ok)
                            *ok = false;
                        return QStringList();
                    }
                    unicode = 0;
                } else if(i > str_len-1) {
                    unicode = 0;
                }

                QStringList replacement;
                if (var_type == ENVIRON) {
                    replacement << QString::fromLocal8Bit(qgetenv(var.toLatin1().constData()));
                } else if(var_type == PROPERTY) {
                    replacement << propertyValue(var);
                    //if(prop)
                    //    replacement = QStringList(prop->value(var));
                } else if(var_type == FUNCTION) {
                    replacement << evaluateExpandFunction( var.toAscii(), args );
                } else if(var_type == VAR) {
                    replacement += values(var);
                }
                if (!(replaced++) && start_var)
                    current = str.left(start_var);
                if (!replacement.isEmpty()) {
                    /* If a list is beteen two strings make sure it expands in such a way
                     * that the string to the left is prepended to the first string and
                     * the string to the right is appended to the last string, example:
                     *  LIST = a b c
                     *  V3 = x/$$LIST/f.cpp
                     *  message($$member(V3,0))     # Outputs "x/a"
                     *  message($$member(V3,1))     # Outputs "b"
                     *  message($$member(V3,2))     # Outputs "c/f.cpp"
                     */
                    current.append(replacement.at(0));
                    for (int i = 1; i < replacement.count(); ++i) {
                        unquote(&current);
                        ret.append(current);
                        current = replacement.at(i);
                    }
                }
            } else {
                if(replaced)
                    current.append(QLatin1String("$"));
            }
        }
        if(replaced && unicode)
            current.append(QChar(unicode));
    }
    if (!replaced) {
        current = str;
        unquote(&current);
        ret.append(current);
    } else if(!current.isEmpty()) {
        unquote(&current);
        ret.append(current);
        logMessage(MT_DebugLevel2, "Project Parser [var replace]: %s -> [%s]\n",
                  str.toLatin1().constData(), ret.join(QLatin1String(",")).toLatin1().constData());

    }
    return ret;
}

bool ProFileEvaluator::isActiveConfig(const QByteArray &config, bool regex)
{
    //magic types for easy flipping
    if(config == "true")
        return true;
    else if(config == "false")
        return false;

    //mkspecs
    if((Option::target_mode == Option::TARG_MACX_MODE || Option::target_mode == Option::TARG_QNX6_MODE ||
        Option::target_mode == Option::TARG_UNIX_MODE) && config == "unix")
        return true;
    else if(Option::target_mode == Option::TARG_MACX_MODE && config == "macx")
        return true;
    else if(Option::target_mode == Option::TARG_QNX6_MODE && config == "qnx6")
        return true;
    else if(Option::target_mode == Option::TARG_MAC9_MODE && config == "mac9")
        return true;
    else if((Option::target_mode == Option::TARG_MAC9_MODE || Option::target_mode == Option::TARG_MACX_MODE) &&
            config == "mac")
        return true;
    else if(Option::target_mode == Option::TARG_WIN_MODE && config == "win32")
        return true;

    QRegExp re(QString::fromLatin1(config), Qt::CaseSensitive, QRegExp::Wildcard);
    QString spec = Option::qmakespec;
    if((regex && re.exactMatch(spec)) || (!regex && spec == QString::fromLatin1(config)))
        return true;

    return false;
}

QStringList ProFileEvaluator::evaluateExpandFunction(const QByteArray &func, const QString &arguments)
{
    QStringList argumentsList = split_arg_list(arguments);
    QStringList args;
    for (int i = 0; i < argumentsList.count(); ++i) {
        args +=expandVariableReferences(argumentsList[i]);
    }
    enum ExpandFunc { E_MEMBER=1, E_FIRST, E_LAST, E_CAT, E_FROMFILE, E_EVAL, E_LIST,
                      E_SPRINTF, E_JOIN, E_SPLIT, E_BASENAME, E_DIRNAME, E_SECTION,
                      E_FIND, E_SYSTEM, E_UNIQUE, E_QUOTE, E_ESCAPE_EXPAND,
                      E_UPPER, E_LOWER, E_FILES, E_PROMPT, E_RE_ESCAPE,
                      E_REPLACE };

    static QMap<QByteArray, int> *expands = 0;
    if(!expands) {
        expands = new QMap<QByteArray, int>;
        expands->insert("member", E_MEMBER);                //v (implemented)
        expands->insert("first", E_FIRST);                  //v
        expands->insert("last", E_LAST);                    //v
        expands->insert("cat", E_CAT);
        expands->insert("fromfile", E_FROMFILE);
        expands->insert("eval", E_EVAL);
        expands->insert("list", E_LIST);
        expands->insert("sprintf", E_SPRINTF);
        expands->insert("join", E_JOIN);                    //v
        expands->insert("split", E_SPLIT);                  //v
        expands->insert("basename", E_BASENAME);            //v
        expands->insert("dirname", E_DIRNAME);              //v
        expands->insert("section", E_SECTION);
        expands->insert("find", E_FIND);
        expands->insert("system", E_SYSTEM);                //v
        expands->insert("unique", E_UNIQUE);
        expands->insert("quote", E_QUOTE);                  //v
        expands->insert("escape_expand", E_ESCAPE_EXPAND);
        expands->insert("upper", E_UPPER);
        expands->insert("lower", E_LOWER);
        expands->insert("re_escape", E_RE_ESCAPE);
        expands->insert("files", E_FILES);
        expands->insert("prompt", E_PROMPT);
        expands->insert("replace", E_REPLACE);
    }
    ExpandFunc func_t = (ExpandFunc)expands->value(func.toLower());

    QStringList ret;

    switch(func_t) {
        case E_BASENAME:
        case E_DIRNAME:
        case E_SECTION: {
            bool regexp = false;
            QString sep, var;
            int beg=0, end=-1;
            if(func_t == E_SECTION) {
                if(args.count() != 3 && args.count() != 4) {
                    logMessage(QString::fromAscii(
                        "%2(var) section(var, sep, begin, end) requires three arguments.\n")
                        .arg(QString::fromLatin1(func.constData())));
                } else {
                    var = args[0];
                    sep = args[1];
                    beg = args[2].toInt();
                    if(args.count() == 4)
                        end = args[3].toInt();
                }
            } else {
                if(args.count() != 1) {
                    logMessage(QString::fromAscii("%2(var) requires one argument.\n").arg(
                        QString::fromLatin1(func.constData())));
                } else {
                    var = args[0];
                    regexp = true;
                    sep = QLatin1String("[\\\\/]");
                    if(func_t == E_DIRNAME)
                        end = -2;
                    else
                        beg = -1;
                }
            }
            if(!var.isNull()) {
                const QStringList l = values(var);
                for(QStringList::ConstIterator it = l.begin(); it != l.end(); ++it) {
                    if(regexp)
                        ret += (*it).section(QRegExp(sep), beg, end);
                    else
                        ret += (*it).section(sep, beg, end);
                }
            }
            break; }
        case E_JOIN: {
            if(args.count() < 1 || args.count() > 4) {
                logMessage(QString::fromAscii("join(var, glue, before, after) requires four arguments.\n"));
            } else {
                QString glue, before, after;
                if(args.count() >= 2)
                    glue = args[1];
                if(args.count() >= 3)
                    before = args[2];
                if(args.count() == 4)
                    after = args[3];
                const QStringList &var = values(args.first());
                if(!var.isEmpty())
                    ret.append(before + var.join(glue) + after);
            }
            break; }
        case E_SPLIT: {
            if(args.count() < 2 || args.count() > 2) {
                logMessage(QString::fromAscii("split(var, sep) requires two arguments\n"));
            } else {
                QString sep = args[1];
                QStringList var = values(args.first());
                for(QStringList::ConstIterator vit = var.begin(); vit != var.end(); ++vit) {
                    QStringList lst = (*vit).split(sep);
                    for(QStringList::ConstIterator spltit = lst.begin(); spltit != lst.end(); ++spltit) {
                        ret.append(*spltit);
                    }
                }
            }
            break; }

        case E_MEMBER: {
            if(args.count() < 1 || args.count() > 3) {
                logMessage(QString::fromAscii("member(var, start, end) requires three arguments.\n"));
            } else {
                bool ok = true;
                const QStringList var = values(args.first());
                int start = 0, end = 0;
                if(args.count() >= 2) {
                    QString start_str = args[1];
                    start = start_str.toInt(&ok);
                    if(!ok) {
                        if(args.count() == 2) {
                            int dotdot = start_str.indexOf(QLatin1String(".."));
                            if(dotdot != -1) {
                                start = start_str.left(dotdot).toInt(&ok);
                                if(ok)
                                    end = start_str.mid(dotdot+2).toInt(&ok);
                            }
                        }
                        if(!ok)
                            logMessage(QString::fromAscii("member() argument 2 (start) '%2' invalid.\n").arg(
                                                        start_str), MT_DebugLevel1 );
                    } else {
                        end = start;
                        if(args.count() == 3)
                            end = args[2].toInt(&ok);
                        if(!ok)
                            logMessage(QString::fromAscii("member() argument 3 (end) '%2' invalid.\n").arg(
                            args[2]), MT_DebugLevel1 );
                    }
                }
                if(ok) {
                    if(start < 0)
                        start += var.count();
                    if(end < 0)
                        end += var.count();
                    if(start < 0 || start >= var.count() || end < 0 || end >= var.count()) {
                        //nothing
                    } else if(start < end) {
                        for(int i = start; i <= end && (int)var.count() >= i; i++) {
                            ret.append(var[i]);
                        }
                    } else {
                        for(int i = start; i >= end && (int)var.count() >= i && i >= 0; i--) {
                            ret += var[i];
                        }
                    }
                }
            }
            break; }
        case E_FIRST:
        case E_LAST: {
            if(args.count() != 1) {
                logMessage(QString::fromAscii("%2(var) requires one argument.\n").arg(
                                            QString::fromLatin1(func.constData())));
            } else {
                const QStringList var = values(args.first());
                if(!var.isEmpty()) {
                    if(func_t == E_FIRST)
                        ret.append(var[0]);
                    else
                        ret.append(var.last());
                }
            }
            break; }

        case E_SYSTEM: {
            if (m_condition) {
                if(args.count() < 1 || args.count() > 2) {
                    logMessage(QString::fromAscii("system(execut) requires one or two arguments.\n"));
                } else {
                    char buff[256];
                    FILE *proc = QT_POPEN(args[0].toLatin1(), "r");
                    bool singleLine = true;
                    if(args.count() > 1)
                        singleLine = (args[1].toLower() == QLatin1String("true"));
                    QString output;
                    while(proc && !feof(proc)) {
                        int read_in = int(fread(buff, 1, 255, proc));
                        if(!read_in)
                            break;
                        for(int i = 0; i < read_in; i++) {
                            if((singleLine && buff[i] == '\n') || buff[i] == '\t')
                                buff[i] = ' ';
                        }
                        buff[read_in] = '\0';
                        output += QLatin1String(buff);
                    }
                    ret += split_value_list(output);
                }
            }
            break; }
        case E_QUOTE:
            for (int i = 0; i < args.count(); ++i) {
                ret += (QStringList)args.at(i);
            }
            break;
        case 0: {
            logMessage(MT_DebugLevel2, "'%s' is not a function\n", func.data());
            break; }
        default: {
            logMessage(MT_DebugLevel2, "Function '%s' is not implemented\n", func.data());
            break; }
    }

    return ret;
}

bool ProFileEvaluator::evaluateConditionalFunction(const QByteArray &function, const QString &arguments, bool *result)
{
    QStringList argumentsList = split_arg_list(arguments);
    QString sep;
    sep.append(QLatin1Char(Option::field_sep));

    QStringList args;
    for (int i = 0; i < argumentsList.count(); ++i) {
        args += expandVariableReferences(argumentsList[i]).join(sep);
    }
    enum ConditionFunc { CF_CONFIG = 1, CF_CONTAINS, CF_COUNT, CF_EXISTS, CF_INCLUDE,
        CF_LOAD, CF_ISEMPTY, CF_SYSTEM, CF_MESSAGE};

    static QMap<QByteArray, int> *functions = 0;
    if(!functions) {
        functions = new QMap<QByteArray, int>;
        functions->insert("load", CF_LOAD);         //v
        functions->insert("include", CF_INCLUDE);   //v
        functions->insert("message", CF_MESSAGE);   //v
        functions->insert("warning", CF_MESSAGE);   //v
        functions->insert("error", CF_MESSAGE);     //v
    }

    bool cond = false;
    bool ok = true;

    ConditionFunc func_t = (ConditionFunc)functions->value(function);

    switch (func_t) {
        case CF_CONFIG: {
            if(args.count() < 1 || args.count() > 2) {
                logMessage(QString::fromAscii("CONFIG(config) requires one or two arguments.\n"), MT_DebugLevel1);
                ok = false;
                break;
            }
            if(args.count() == 1) {
                //cond = isActiveConfig(args.first());
                break;
            }
            const QStringList mutuals = args[1].split(QLatin1Char('|'));
            const QStringList &configs = m_valuemap.value("CONFIG");
            for(int i = configs.size() - 1 && ok; i >= 0; i--) {
                for(int mut = 0; mut < mutuals.count(); mut++) {
                    if(configs[i] == mutuals[mut].trimmed()) {
                        cond = (configs[i] == args[0]);
                        break;
                    }
                }
            }
            break; }
        case CF_CONTAINS: {
            if(args.count() < 2 || args.count() > 3) {
                logMessage(QString::fromAscii("contains(var, val) requires at least two arguments.\n"), MT_DebugLevel1);
                ok = false;
                break;
            }

            QRegExp regx(args[1]);
            const QStringList &l = values(args.first());
            if(args.count() == 2) {
                for(int i = 0; i < l.size(); ++i) {
                    const QString val = l[i];
                    if(regx.exactMatch(val) || val == args[1]) {
                        cond = true;
                        break;
                    }
                }
            } else {
                const QStringList mutuals = args[2].split(QLatin1Char('|'));
                for(int i = l.size()-1; i >= 0; i--) {
                    const QString val = l[i];
                    for(int mut = 0; mut < mutuals.count(); mut++) {
                        if(val == mutuals[mut].trimmed()) {
                            cond = (regx.exactMatch(val) || val == args[1]);
                            break;
                        }
                    }
                }
            }

            break; }
        case CF_COUNT: {
            if(args.count() != 2 && args.count() != 3) {
                logMessage(QString::fromAscii("count(var, count) requires at least two arguments.\n"), MT_DebugLevel1);
                ok = false;
                break;
            }
            if(args.count() == 3) {
                QString comp = args[2];
                if(comp == QLatin1String(">") || comp == QLatin1String("greaterThan")) {
                    cond = values(args.first()).count() > args[1].toInt();
                } else if(comp == QLatin1String(">=")) {
                    cond = values(args.first()).count() >= args[1].toInt();
                } else if(comp == QLatin1String("<") || comp == QLatin1String("lessThan")) {
                    cond = values(args.first()).count() < args[1].toInt();
                } else if(comp == QLatin1String("<=")) {
                    cond = values(args.first()).count() <= args[1].toInt();
                } else if(comp == QLatin1String("equals") || comp == QLatin1String("isEqual") || comp == QLatin1String("=") || comp == QLatin1String("==")) {
                    cond = values(args.first()).count() == args[1].toInt();
                } else {
                    ok = false;
                    logMessage(QString::fromAscii("unexpected modifier to count(%2)\n").arg(
                        comp), MT_DebugLevel1);
                }
                break;
            }
            cond = values(args.first()).count() == args[1].toInt();
            break; }
        case CF_INCLUDE: {
            QString parseInto;
            if(args.count() == 2) {
                parseInto = args[1];
            } else if(args.count() != 1) {
                logMessage(QString::fromAscii("include(file) requires one argument.\n"), MT_DebugLevel1);
                ok = false;
                break;
            }
            QString fileName = args.first();
            // ### this breaks if we have include(c:/reallystupid.pri) but IMHO that's really bad style.
            QDir currentProPath(getcwd());
            fileName = currentProPath.absoluteFilePath(fileName);
            ok = evaluateFile(fileName, &ok);
            break; }
        case CF_LOAD: {
            QString parseInto;
            bool ignore_error = false;
            if(args.count() == 2) {
                QString sarg = args[1];
                ignore_error = (sarg.toLower() == QLatin1String("true") || sarg.toInt());
            } else if(args.count() != 1) {
                logMessage(QString::fromAscii("load(feature) requires one argument.\n"), MT_DebugLevel1);
                ok = false;
                break;
            }
            ok = evaluateFeatureFile( args.first(), &cond);
            break; }

        case CF_MESSAGE: {
            if(args.count() != 1) {
                logMessage(QString::fromAscii("%2(message) requires one argument.\n").arg(
                    QString(QString::fromLatin1(function.constData()))), MT_DebugLevel1);
                ok = false;
                break;
            }
            QString msg = args.first();
            bool isError = (function == "error");
            logMessage(QString::fromAscii("%2\n").arg(msg), isError ? MT_ProError : MT_ProMessage);
            break; }

        case CF_SYSTEM: {
            if(args.count() != 1) {
                logMessage(QString::fromAscii("system(exec) requires one argument.\n"), MT_DebugLevel1);
                ok = false;
                break;
            }
            ok = system(args.first().toLatin1().constData()) == 0;
            break; }

        case CF_ISEMPTY: {
            if(args.count() != 1) {
                logMessage(QString::fromAscii("isEmpty(var) requires one argument.\n"), MT_DebugLevel1);
                ok = false;
                break;
            }
            QStringList sl = values(args.first());
            if (sl.count() == 0) {
                cond = true;
            }else if (sl.count() > 0) {
                QString var = sl.first();
                cond = (var.isEmpty());
            }
            break; }
        case CF_EXISTS: {
            if(args.count() != 1) {
                logMessage(QString::fromAscii("exists(file) requires one argument.\n"), MT_DebugLevel1);
                ok = false;
                break;
            }
            QString file = args.first();

            file = QDir::cleanPath(file);

            if (QFile::exists(file)) {
                cond = true;
                break;
            }
            //regular expression I guess
            QString dirstr = getcwd();
            int slsh = file.lastIndexOf(Option::dir_sep);
            if(slsh != -1) {
                dirstr = file.left(slsh+1);
                file = file.right(file.length() - slsh - 1);
            }
            cond = QDir(dirstr).entryList(QStringList(file)).count();

            break; }

    }

    if (result) *result = cond;

    return ok;
}

bool ProFileEvaluator::contains(const QString &variableName) const
{
    return m_valuemap.contains(variableName.toAscii());
}

QStringList ProFileEvaluator::values(const QString &variableName) const
{
    if (variableName == QLatin1String("PWD")) {
        return QStringList(getcwd());
    }
    return m_valuemap.value(variableName.toAscii());
}

bool ProFileEvaluator::evaluateFile(const QString &fileName, bool *result)
{
    bool ok = true;

    QString fn = fileName;

    QFileInfo fi(fn);
    if (fi.exists()) {
        logMessage(QString::fromAscii("Reading %2\n").arg(fileName), MT_DebugLevel3);
        ProFile *pro = queryProFile(fi.absoluteFilePath());
        if (pro) {
            m_profileStack.push_back(pro);
            ok &= currentProFile() ? pro->Accept(this) : false;
            if (ok) {
                if (m_profileStack.count() > 0) {
                    ProFile *pro = m_profileStack.pop();
                    releaseProFile(pro);
                }
            }
        }
        if (result) *result = true;
    }else{
        if (result) *result = false;
    }

/*    if (ok && readFeatures) {
        QStringList configs = values("CONFIG");
        QSet<QString> processed;
        for (QStringList::iterator it = configs.begin(); it != configs.end(); ++it) {
            QString fn = *it;
            if (!processed.contains(fn)) {
                processed.insert(fn);
                evaluateFeatureFile(fn, 0);
            }
        }
    } */

    return ok;
}


bool ProFileEvaluator::evaluateFeatureFile(const QString &fileName, bool *result)
{
    QString fn;
    QStringList feature_paths = qmake_feature_paths();
    for(QStringList::ConstIterator it = feature_paths.begin(); it != feature_paths.end(); ++it) {
        QString fname = *it + QLatin1Char('/') + fileName;
        if (QFileInfo(fname).exists()) {
            fn = fname;
            break;
        }
        fname += QLatin1String(".prf");
        if (QFileInfo(fname).exists()) {
            fn = fname;
            break;
        }
    }
    return fn.isEmpty() ? false : evaluateFile(fn, result);
}

ProFileEvaluator::TemplateType ProFileEvaluator::templateType()
{
    QStringList templ = m_valuemap.value("TEMPLATE");
    if (templ.count() >= 1) {
        QByteArray t = templ.last().toAscii().toLower();
        if (t == "app") return TT_Application;
        if (t == "lib") return TT_Library;
        if (t == "subdirs") return TT_Subdirs;
    }
    return TT_Unknown;
}

/*
 * Lookup of files are done in this order:
 *  1. look in pwd
 *  2. look in vpaths
 *  3. expand wild card files relative from the profiles folder
 **/

// FIXME: This code supports something that I'd consider a flaw in .pro file syntax
// which is not even documented. So arguably this can be ditched completely...
QStringList ProFileEvaluator::expandPattern(const QString& pattern)
{
    QStringList vpaths = values(QLatin1String("VPATH"))
        + values(QLatin1String("QMAKE_ABSOLUTE_SOURCE_PATH"))
        + values(QLatin1String("DEPENDPATH"))
        + values(QLatin1String("VPATH_SOURCES"));

    QStringList sources_out;
    ProFile *pro = currentProFile();
    if (!pro)
        return QStringList();
    QFileInfo fi0(pro->fileName());
    QDir dir0(fi0.absoluteDir());
    QString absName = QDir::cleanPath(dir0.absoluteFilePath(pattern));
    QFileInfo fi(absName);
    bool found = fi.exists();
    // Search in all vpaths
    for(QStringList::Iterator vpath_it = vpaths.begin(); vpath_it != vpaths.end() && !found; ++vpath_it) {
        QDir vpath(*vpath_it);
        fi.setFile(*vpath_it + QDir::separator() + pattern);
        if (fi.exists()) {
            absName = fi.absoluteFilePath();
            found = true;
            break;
        }
    }
    if (found) {
        sources_out+=fi.canonicalFilePath();
    } else {
        QString val = pattern;
        QString dir, regex = val, real_dir;
        if(regex.lastIndexOf(QLatin1Char('/')) != -1) {
            dir = regex.left(regex.lastIndexOf(QLatin1Char('/')) + 1);
            real_dir = dir;
            regex = regex.right(regex.length() - dir.length());
        }
        if(real_dir.isEmpty() || QFileInfo(real_dir).exists()) {
            QStringList files = QDir(real_dir).entryList(QStringList(regex));
            if(files.isEmpty()) {
                logMessage(MT_DebugLevel2, "%s:%d Failure to find %s",
                          __FILE__, __LINE__,
                          val.toLatin1().constData());
            } else {
                QString a;
                for(int i = files.count() - 1; i >= 0; --i) {
                    if(files[i] == QLatin1String(".") || files[i] == QLatin1String(".."))
                        continue;
                    a = dir + files[i];
                    sources_out += a;
                }
            }
        } else {
            logMessage(MT_DebugLevel2, "%s:%d Cannot match %s%c%s, as %s does not exist.",
                      __FILE__, __LINE__, real_dir.toLatin1().constData(),
                      '/',
                      regex.toLatin1().constData(), real_dir.toLatin1().constData());
        }

    }
    return sources_out;
}

ProFile *ProFileEvaluator::queryProFile(const QString &filename)
{
    ProReader pr;
    pr.setEnableBackSlashFixing(false);

    ProFile *pro = pr.read(filename);
    if (!pro) {
        LogMessage msg;
        msg.m_msg = QLatin1String("parse failure.");
        msg.m_filename = filename;
        msg.m_linenumber = pr.currentLine();
        msg.m_type = MT_Error;
        logMessage(msg);
    }

    return pro;
}

void ProFileEvaluator::releaseProFile(ProFile *pro)
{
    delete pro;
}

QString ProFileEvaluator::propertyValue(const QString &val) const
{
    return getPropertyValue(val);
}

void ProFileEvaluator::logMessage(const ProFileEvaluator::LogMessage &msg)
{
    QByteArray locstr = QString(QLatin1String("%1(%2):")).arg(msg.m_filename).arg(msg.m_linenumber).toAscii();
    QByteArray text = msg.m_msg.toAscii();
    switch (msg.m_type) {
        case MT_DebugLevel3:
            qWarning("%s profileevaluator information:    %s", locstr.data(), text.data());
            break;
        case MT_DebugLevel2:
            qWarning("%s profileevaluator warning:        %s", locstr.data(), text.data());
            break;
        case MT_DebugLevel1:
            qWarning("%s profileevaluator critical error: %s", locstr.data(), text.data());
            break;
        case MT_ProMessage:
            qWarning("%s Project MESSAGE: %s", locstr.data(), text.data());
            break;
        case MT_ProError:
            qWarning("%s Project ERROR: %s", locstr.data(), text.data());
            break;
        case MT_Error:
            qWarning("%s ERROR: %s", locstr.data(), text.data());
            break;
    }
}

void ProFileEvaluator::logMessage(const QString &message, MessageType mt)
{
    LogMessage msg;
    msg.m_msg = message;
    msg.m_type = mt;

    ProFile *pro = currentProFile();
    if (pro) {
        msg.m_filename = pro->fileName();
        msg.m_linenumber = m_lineNo;
    } else {
        msg.m_filename = QLatin1String("Not a file");
        msg.m_linenumber = 0;
    }

    logMessage(msg);
}

void ProFileEvaluator::logMessage(MessageType mt, const char *msg, ...)
{
#define MAX_MESSAGE_LENGTH 1024
    char buf[MAX_MESSAGE_LENGTH];
    va_list ap;
    va_start(ap, msg); // use variable arg list
    qvsnprintf(buf, MAX_MESSAGE_LENGTH - 1, msg, ap);
    va_end(ap);
    buf[MAX_MESSAGE_LENGTH - 1] = '\0';
    logMessage(QString::fromAscii(buf), mt);
}

// This function is unneeded and still retained. See log message for reason.
QStringList ProFileEvaluator::absFileNames(const QString &variableName)
{
    QStringList vpaths = values(QLatin1String("VPATH"))
        + values(QLatin1String("QMAKE_ABSOLUTE_SOURCE_PATH"))
        + values(QLatin1String("DEPENDPATH"))
        + values(QLatin1String("VPATH_SOURCES"));

    QStringList sources_out;
    QStringList sources = values(variableName);
    QFileInfo fi(m_origfile);
    QDir dir(fi.absoluteDir());
    for (int i = 0; i < sources.count(); ++i) {
        QString fn = sources[i];
        QString absName = QDir::cleanPath(dir.absoluteFilePath(sources[i]));
        QFileInfo fi(absName);
        bool found = fi.exists();
        // Search in all vpaths
        for(QStringList::Iterator vpath_it = vpaths.begin(); vpath_it != vpaths.end() && !found; ++vpath_it) {
            QDir vpath(*vpath_it);
            fi.setFile(*vpath_it + QDir::separator() + fn);
            if (fi.exists()) {
                absName = fi.absoluteFilePath();
                found = true;
                break;
            }
        }
        if (found) {
            sources_out+=fi.canonicalFilePath();
        } else {
            QString val = fn;
            QString dir, regex = val, real_dir;
            if(regex.lastIndexOf(QLatin1Char('/')) != -1) {
                dir = regex.left(regex.lastIndexOf(QLatin1Char('/')) + 1);
                real_dir = dir;
                regex = regex.right(regex.length() - dir.length());
            }
            if(real_dir.isEmpty() || QFileInfo(real_dir).exists()) {
                QStringList files = QDir(real_dir).entryList(QStringList(regex));
                if(files.isEmpty()) {
                    logMessage(MT_DebugLevel2, "%s:%d Failure to find %s",
                              __FILE__, __LINE__,
                              val.toLatin1().constData());
                } else {
                    QString a;
                    for(int i = (int)files.count()-1; i >= 0; i--) {
                        if(files[i] == QLatin1String(".") || files[i] == QLatin1String(".."))
                            continue;
                        a = dir + files[i];
                        sources_out+=a;
                    }
                }
            } else {
                logMessage(MT_DebugLevel2, "%s:%d Cannot match %s%c%s, as %s does not exist.",
                          __FILE__, __LINE__, real_dir.toLatin1().constData(),
                          '/',
                          regex.toLatin1().constData(), real_dir.toLatin1().constData());
            }

        }
    }
    return sources_out;
}
QT_END_NAMESPACE

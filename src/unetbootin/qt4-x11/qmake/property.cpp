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

#include "property.h"
#include "option.h"

#include <qdir.h>
#include <qmap.h>
#include <qsettings.h>
#include <qstringlist.h>
#include <stdio.h>

QT_BEGIN_NAMESPACE

QStringList qmake_mkspec_paths(); //project.cpp

QMakeProperty::QMakeProperty() : settings(0)
{
}

QMakeProperty::~QMakeProperty()
{
    delete settings;
    settings = 0;
}

void QMakeProperty::initSettings()
{
    if(!settings) {
        settings = new QSettings(QSettings::UserScope, "Trolltech", "QMake");
        settings->setFallbacksEnabled(false);
    }
}

QString
QMakeProperty::keyBase(bool version) const
{
    if (version)
        return QString(qmake_version()) + "/";
    return QString();
}

QString
QMakeProperty::value(QString v, bool just_check)
{
    if(v == "QT_INSTALL_PREFIX")
        return QLibraryInfo::location(QLibraryInfo::PrefixPath);
    else if(v == "QT_INSTALL_DATA")
        return QLibraryInfo::location(QLibraryInfo::DataPath);
    else if(v == "QT_INSTALL_DOCS")
        return QLibraryInfo::location(QLibraryInfo::DocumentationPath);
    else if(v == "QT_INSTALL_HEADERS")
        return QLibraryInfo::location(QLibraryInfo::HeadersPath);
    else if(v == "QT_INSTALL_LIBS")
        return QLibraryInfo::location(QLibraryInfo::LibrariesPath);
    else if(v == "QT_INSTALL_BINS")
        return QLibraryInfo::location(QLibraryInfo::BinariesPath);
    else if(v == "QT_INSTALL_PLUGINS")
        return QLibraryInfo::location(QLibraryInfo::PluginsPath);
    else if(v == "QT_INSTALL_TRANSLATIONS")
        return QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    else if(v == "QT_INSTALL_CONFIGURATION")
        return QLibraryInfo::location(QLibraryInfo::SettingsPath);
    else if(v == "QT_INSTALL_EXAMPLES")
        return QLibraryInfo::location(QLibraryInfo::ExamplesPath);
    else if(v == "QT_INSTALL_DEMOS")
        return QLibraryInfo::location(QLibraryInfo::DemosPath);
    else if(v == "QMAKE_MKSPECS")
        return qmake_mkspec_paths().join(Option::target_mode == Option::TARG_WIN_MODE ? ";" : ":");
    else if(v == "QMAKE_VERSION")
        return qmake_version();
#ifdef QT_VERSION_STR
    else if(v == "QT_VERSION")
        return QT_VERSION_STR;
#endif

    initSettings();
    int slash = v.lastIndexOf('/');
    QVariant var = settings->value(keyBase(slash == -1) + v);
    bool ok = var.isValid();
    QString ret = var.toString();
    if(!ok) {
        QString version = qmake_version();
        if(slash != -1) {
            version = v.left(slash-1);
            v = v.mid(slash+1);
        }
        settings->beginGroup(keyBase(false));
        QStringList subs = settings->childGroups();
        settings->endGroup();
        subs.sort();
        for (int x = subs.count() - 1; x >= 0; x--) {
            QString s = subs[x];
            if(s.isEmpty() || s > version)
                continue;
            var = settings->value(keyBase(false) + s + "/" + v);
            ok = var.isValid();
            ret = var.toString();
            if (ok) {
                if(!just_check)
                    debug_msg(1, "Fell back from %s -> %s for '%s'.", version.toLatin1().constData(),
                              s.toLatin1().constData(), v.toLatin1().constData());
                return ret;
            }
        }
    }
    return ok ? ret : QString();
}

bool
QMakeProperty::hasValue(QString v)
{
    return !value(v, true).isNull();
}

void
QMakeProperty::setValue(QString var, const QString &val)
{
    initSettings();
    settings->setValue(keyBase() + var, val);
}

bool
QMakeProperty::exec()
{
    bool ret = true;
    if(Option::qmake_mode == Option::QMAKE_QUERY_PROPERTY) {
        if(Option::prop::properties.isEmpty()) {
            initSettings();
            settings->beginGroup(keyBase(false));
            QStringList subs = settings->childGroups();
            settings->endGroup();
            subs.sort();
            for(int x = subs.count() - 1; x >= 0; x--) {
                QString s = subs[x];
                if(s.isEmpty())
                    continue;
                settings->beginGroup(keyBase(false) + s);
                QStringList keys = settings->childKeys();
                settings->endGroup();
                for(QStringList::ConstIterator it2 = keys.begin(); it2 != keys.end(); it2++) {
                    QString ret = settings->value(keyBase(false) + s + "/" + (*it2)).toString();
                    if(s != qmake_version())
                        fprintf(stdout, "%s/", s.toLatin1().constData());
                    fprintf(stdout, "%s:%s\n", (*it2).toLatin1().constData(), ret.toLatin1().constData());
                }
            }
            return true;
        }
        for(QStringList::ConstIterator it = Option::prop::properties.begin();
            it != Option::prop::properties.end(); it++) {
            if(Option::prop::properties.count() > 1)
                fprintf(stdout, "%s:", (*it).toLatin1().constData());
            if(!hasValue((*it))) {
                ret = false;
                fprintf(stdout, "**Unknown**\n");
            } else {
                fprintf(stdout, "%s\n", value((*it)).toLatin1().constData());
            }
        }
    } else if(Option::qmake_mode == Option::QMAKE_SET_PROPERTY) {
        for(QStringList::ConstIterator it = Option::prop::properties.begin();
            it != Option::prop::properties.end(); it++) {
            QString var = (*it);
            it++;
            if(it == Option::prop::properties.end()) {
                ret = false;
                break;
            }
            if(!var.startsWith("."))
                setValue(var, (*it));
        }
    }
    return ret;
}

QT_END_NAMESPACE

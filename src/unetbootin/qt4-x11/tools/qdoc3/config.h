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

/*
  config.h
*/

#ifndef CONFIG_H
#define CONFIG_H

#include <QMap>
#include <QSet>
#include <QStringList>

#include "location.h"

QT_BEGIN_NAMESPACE

class Config
{
 public:
    Config(const QString& programName);
    ~Config();

    void load(const QString& fileName);
    void setStringList(const QString& var, const QStringList& values);

    const QString& programName() const { return prog; }
    const Location& location() const { return loc; }
    const Location& lastLocation() const { return lastLoc; }
    bool getBool(const QString& var) const;
    int getInt(const QString& var) const;
    QString getString(const QString& var) const;
    QSet<QString> getStringSet(const QString& var) const;
    QStringList getStringList(const QString& var) const;
    QRegExp getRegExp(const QString& var) const;
    QList<QRegExp> getRegExpList(const QString& var) const;
    QSet<QString> subVars(const QString& var) const;
    QStringList getAllFiles(const QString& filesVar, 
                            const QString& dirsVar,
                            const QString& defaultNameFilter,
                            const QSet<QString> &excludedDirs = QSet<QString>());

    static QStringList getFilesHere(const QString& dir,
                                    const QString& nameFilter,
                                    const QSet<QString> &excludedDirs = QSet<QString>());
    static QString findFile(const Location& location, 
                            const QStringList &files,
                            const QStringList& dirs, 
                            const QString& fileName,
                            QString& userFriendlyFilePath);
    static QString findFile(const Location &location, 
                            const QStringList &files,
                            const QStringList &dirs, 
                            const QString &fileBase,
                            const QStringList &fileExtensions,
                            QString &userFriendlyFilePath);
    static QString copyFile(const Location& location,
                            const QString& sourceFilePath,
                            const QString& userFriendlySourceFilePath,
                            const QString& targetDirPath);
    static int numParams(const QString& value);
    static bool removeDirContents(const QString& dir);

    QT_STATIC_CONST QString dot;

 private:
    static bool isMetaKeyChar(QChar ch);
    void load(Location location, const QString& fileName);

    QString prog;
    Location loc;
    Location lastLoc;
    QMap<QString, Location> locMap;
    QMap<QString, QStringList> stringListValueMap;
    QMap<QString, QString> stringValueMap;

    static QMap<QString, QString> uncompressedFiles;
    static QMap<QString, QString> extractedDirs;
    static int numInstances;
};

#define CONFIG_ALIAS                    "alias"
#define CONFIG_BASE                     "base"      // ### don't document for now
#define CONFIG_CODEINDENT               "codeindent"
#define CONFIG_DEFINES                  "defines"
#define CONFIG_DESCRIPTION              "description"
#define CONFIG_EDITION                  "edition"
#define CONFIG_EXAMPLEDIRS              "exampledirs"
#define CONFIG_EXAMPLES                 "examples"
#define CONFIG_EXCLUDEDIRS              "excludedirs"
#define CONFIG_EXTRAIMAGES              "extraimages"
#define CONFIG_FALSEHOODS               "falsehoods"
#define CONFIG_FORMATTING               "formatting"
#define CONFIG_GENERATEINDEX            "generateindex"
#define CONFIG_HEADERDIRS               "headerdirs"
#define CONFIG_HEADERS                  "headers"
#define CONFIG_IGNOREDIRECTIVES         "ignoredirectives"
#define CONFIG_IGNORETOKENS             "ignoretokens"
#define CONFIG_IMAGEDIRS                "imagedirs"
#define CONFIG_IMAGES                   "images"
#define CONFIG_INDEXES                  "indexes"
#define CONFIG_LANGUAGE                 "language"
#define CONFIG_MACRO                    "macro"
#define CONFIG_OUTPUTDIR                "outputdir"
#define CONFIG_OUTPUTLANGUAGE           "outputlanguage"
#define CONFIG_OUTPUTFORMATS            "outputformats"
#define CONFIG_PROJECT                  "project"
#define CONFIG_QHP                      "qhp"
#define CONFIG_QUOTINGINFORMATION       "quotinginformation"
#define CONFIG_SLOW                     "slow"
#define CONFIG_SOURCEDIRS               "sourcedirs"
#define CONFIG_SOURCES                  "sources"
#define CONFIG_SPURIOUS                 "spurious"
#define CONFIG_STYLESHEETS              "stylesheets"
#define CONFIG_TABSIZE                  "tabsize"
#define CONFIG_TAGFILE                  "tagfile"
#define CONFIG_TRANSLATORS              "translators" // ### don't document for now
#define CONFIG_URL                      "url"
#define CONFIG_VERSION                  "version"
#define CONFIG_VERSIONSYM               "versionsym"

#define CONFIG_FILEEXTENSIONS           "fileextensions"

QT_END_NAMESPACE

#endif

/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
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

#ifndef QSETTINGS_P_H
#define QSETTINGS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "QtCore/qdatetime.h"
#include "QtCore/qmap.h"
#include "QtCore/qmutex.h"
#include "QtCore/qiodevice.h"
#include "QtCore/qstack.h"
#include "QtCore/qstringlist.h"
#ifndef QT_NO_QOBJECT
#include "private/qobject_p.h"
#endif

#ifdef Q_OS_WIN
#include "QtCore/qt_windows.h"
#endif

QT_BEGIN_NAMESPACE

#if defined(Q_WS_QWS)
#define QT_QSETTINGS_ALWAYS_CASE_SENSITIVE_AND_FORGET_ORIGINAL_KEY_ORDER
#endif

// used in testing framework
#define QSETTINGS_P_H_VERSION 2

#ifdef QT_QSETTINGS_ALWAYS_CASE_SENSITIVE_AND_FORGET_ORIGINAL_KEY_ORDER
static const Qt::CaseSensitivity IniCaseSensitivity = Qt::CaseSensitive;

class QSettingsKey : public QString
{
public:
    inline QSettingsKey(const QString &key, Qt::CaseSensitivity cs, int /* position */ = -1)
        : QString(key) { Q_ASSERT(cs == Qt::CaseSensitive); Q_UNUSED(cs); }

    inline QString originalCaseKey() const { return *this; }
    inline int originalKeyPosition() const { return -1; }
};
#else
static const Qt::CaseSensitivity IniCaseSensitivity = Qt::CaseInsensitive;

class QSettingsKey : public QString
{
public:
    inline QSettingsKey(const QString &key, Qt::CaseSensitivity cs, int position = -1)
         : QString(key), theOriginalKey(key), theOriginalKeyPosition(position)
    {
        if (cs == Qt::CaseInsensitive)
            QString::operator=(toLower());
    }

    inline QString originalCaseKey() const { return theOriginalKey; }
    inline int originalKeyPosition() const { return theOriginalKeyPosition; }

private:
    QString theOriginalKey;
    int theOriginalKeyPosition;
};
#endif

typedef QMap<QSettingsKey, QByteArray> UnparsedSettingsMap;
typedef QMap<QSettingsKey, QVariant> ParsedSettingsMap;

class QSettingsGroup
{
public:
    inline QSettingsGroup()
        : num(-1), maxNum(-1) {}
    inline QSettingsGroup(const QString &s)
        : str(s), num(-1), maxNum(-1) {}
    inline QSettingsGroup(const QString &s, bool guessArraySize)
        : str(s), num(0), maxNum(guessArraySize ? 0 : -1) {}

    inline QString name() const { return str; }
    inline QString toString() const;
    inline bool isArray() const { return num != -1; }
    inline int arraySizeGuess() const { return maxNum; }
    inline void setArrayIndex(int i)
    { num = i + 1; if (maxNum != -1 && num > maxNum) maxNum = num; }

    QString str;
    int num;
    int maxNum;
};

inline QString QSettingsGroup::toString() const
{
    QString result;
    result = str;
    if (num > 0) {
        result += QLatin1Char('/');
        result += QString::number(num);
    }
    return result;
}

class Q_CORE_EXPORT QConfFile
{
public:
    ParsedSettingsMap mergedKeyMap() const;
    bool isWritable() const;

    static QConfFile *fromName(const QString &name, bool _userPerms);
    static void clearCache();

    QString name;
    QDateTime timeStamp;
    qint64 size;
    UnparsedSettingsMap unparsedIniSections;
    ParsedSettingsMap originalKeys;
    ParsedSettingsMap addedKeys;
    ParsedSettingsMap removedKeys;
    QAtomicInt ref;
    QMutex mutex;
    bool userPerms;

private:
#ifdef Q_DISABLE_COPY
    QConfFile(const QConfFile &);
    QConfFile &operator=(const QConfFile &);
#endif
    QConfFile(const QString &name, bool _userPerms);

    friend class QConfFile_createsItself; // silences compiler warning
};

class Q_AUTOTEST_EXPORT QSettingsPrivate
#ifndef QT_NO_QOBJECT
    : public QObjectPrivate
#endif
{
#ifdef QT_NO_QOBJECT
    QSettings *q_ptr;
#endif
    Q_DECLARE_PUBLIC(QSettings)

public:
    QSettingsPrivate(QSettings::Format format);
    QSettingsPrivate(QSettings::Format format, QSettings::Scope scope,
                     const QString &organization, const QString &application);
    virtual ~QSettingsPrivate();

    virtual void remove(const QString &key) = 0;
    virtual void set(const QString &key, const QVariant &value) = 0;
    virtual bool get(const QString &key, QVariant *value) const = 0;

    enum ChildSpec { AllKeys, ChildKeys, ChildGroups };
    virtual QStringList children(const QString &prefix, ChildSpec spec) const = 0;

    virtual void clear() = 0;
    virtual void sync() = 0;
    virtual void flush() = 0;
    virtual bool isWritable() const = 0;
    virtual QString fileName() const = 0;

    QString actualKey(const QString &key) const;
    void beginGroupOrArray(const QSettingsGroup &group);
    void setStatus(QSettings::Status status) const;
    void requestUpdate();
    void update();

    static QString normalizedKey(const QString &key);
    static QSettingsPrivate *create(QSettings::Format format, QSettings::Scope scope,
                                        const QString &organization, const QString &application);
    static QSettingsPrivate *create(const QString &fileName, QSettings::Format format);

    static void processChild(QString key, ChildSpec spec, QMap<QString, QString> &result);

    // Variant streaming functions
    static QStringList variantListToStringList(const QVariantList &l);
    static QVariant stringListToVariantList(const QStringList &l);

    // parser functions
    static QString variantToString(const QVariant &v);
    static QVariant stringToVariant(const QString &s);
    static void iniEscapedKey(const QString &key, QByteArray &result);
    static bool iniUnescapedKey(const QByteArray &key, int from, int to, QString &result);
    static void iniEscapedString(const QString &str, QByteArray &result);
    static void iniEscapedStringList(const QStringList &strs, QByteArray &result);
    static bool iniUnescapedStringList(const QByteArray &str, int from, int to,
                                       QString &stringResult, QStringList &stringListResult);
    static QStringList splitArgs(const QString &s, int idx);

    /*
    The numeric values of these enums define their search order. For example,
    F_User | F_Organization is searched before F_System | F_Application,
    because their values are respectively 1 and 2.
    */
    enum {
       F_Application = 0x0,
       F_Organization = 0x1,
       F_User = 0x0,
       F_System = 0x2,
       NumConfFiles = 4
    };

    QSettings::Format format;
    QSettings::Scope scope;
    QString organizationName;
    QString applicationName;

protected:
    QStack<QSettingsGroup> groupStack;
    QString groupPrefix;
    int spec;
    bool fallbacks;
    bool pendingChanges;
    mutable QSettings::Status status;
};

class QConfFileSettingsPrivate : public QSettingsPrivate
{
public:
    QConfFileSettingsPrivate(QSettings::Format format, QSettings::Scope scope,
                             const QString &organization, const QString &application);
    QConfFileSettingsPrivate(const QString &fileName, QSettings::Format format);
    ~QConfFileSettingsPrivate();

    void remove(const QString &key);
    void set(const QString &key, const QVariant &value);
    bool get(const QString &key, QVariant *value) const;

    QStringList children(const QString &prefix, ChildSpec spec) const;

    void clear();
    void sync();
    void flush();
    bool isWritable() const;
    QString fileName() const;

    static bool readIniFile(const QByteArray &data, UnparsedSettingsMap *unparsedIniSections);
    static bool readIniSection(const QSettingsKey &section, const QByteArray &data,
                               ParsedSettingsMap *settingsMap);
    static bool readIniLine(const QByteArray &data, int &dataPos, int &lineStart, int &lineLen,
                            int &equalsPos);

private:
    void initFormat();
    void initAccess();
    void syncConfFile(int confFileNo);
    bool writeIniFile(QIODevice &device, const ParsedSettingsMap &map);
#ifdef Q_OS_MAC
    bool readPlistFile(const QString &fileName, ParsedSettingsMap *map) const;
    bool writePlistFile(const QString &fileName, const ParsedSettingsMap &map) const;
#endif
    void ensureAllSectionsParsed(QConfFile *confFile) const;
    void ensureSectionParsed(QConfFile *confFile, const QSettingsKey &key) const;

    QConfFile *confFiles[NumConfFiles];
    QSettings::ReadFunc readFunc;
    QSettings::WriteFunc writeFunc;
    QString extension;
    Qt::CaseSensitivity caseSensitivity;
    int nextPosition;
};

QT_END_NAMESPACE

#endif // QSETTINGS_P_H

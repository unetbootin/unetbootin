/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the tools applications of the Qt Toolkit.
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

#ifndef RCC_H
#define RCC_H

#include <QtCore/QStringList>
#include <QtCore/QFileInfo>
#include <QtCore/QLocale>
#include <QtCore/QHash>
#include <QtCore/QString>

QT_BEGIN_NAMESPACE

struct RCCFileInfo;
class QIODevice;
class RCCResourceLibrary;
class QTextStream;

class RCCBuilder
{
public:
    RCCBuilder();

    void initializeLibrary(RCCResourceLibrary &lib) const;
    bool processResourceFile(const QStringList &filenamesIn, const QString &filenameOut, bool list) const;
    bool processResourceFile(const QStringList &filenamesIn, const QString &filenameOut, bool list, QIODevice &errorDevice) const;


public:
    bool writeBinary;
    QString initName;
    bool verbose;
    int compressLevel;
    int compressThreshold;
    bool useNameSpace;
    QString resourceRoot;
};

class RCCResourceLibrary
{
    RCCResourceLibrary(const RCCResourceLibrary &);
    RCCResourceLibrary &operator=(const RCCResourceLibrary &);

public:
    RCCResourceLibrary();
    ~RCCResourceLibrary();

    bool output(QIODevice &out, QIODevice &errorDevice);

    bool readFiles(bool ignoreErrors, QIODevice &errorDevice);

    enum Format { Binary, C_Code };
    inline void setFormat(Format f) { mFormat = f; }
    inline Format format() const { return mFormat; }

    inline void setInputFiles(QStringList files) { mFileNames = files; }
    inline QStringList inputFiles() const { return mFileNames; }

    QStringList dataFiles() const;

    // Return a map of resource identifier (':/newPrefix/images/p1.png') to file.
    typedef QHash<QString, QString> ResourceDataFileMap;
    ResourceDataFileMap resourceDataFileMap() const;

    inline void setVerbose(bool b) { mVerbose = b; }
    inline bool verbose() const { return mVerbose; }

    inline void setInitName(const QString &n) { mInitName = n; }
    inline QString initName() const { return mInitName; }

    inline void setCompressLevel(int c) { mCompressLevel = c; }
    inline int compressLevel() const { return mCompressLevel; }

    inline void setCompressThreshold(int t) { mCompressThreshold = t; }
    inline int compressThreshold() const { return mCompressThreshold; }

    inline void setResourceRoot(const QString &str) { mResourceRoot = str; }
    inline QString resourceRoot() const { return mResourceRoot; }
    
    inline void setUseNameSpace(bool v) { mUseNameSpace = v; }
    inline bool useNameSpace() const { return mUseNameSpace; }
    
    QStringList failedResources() const { return mFailedResources; }

private:
    struct Strings {
        Strings();
        const QString TAG_RCC;
        const QString TAG_RESOURCE;
        const QString TAG_FILE;
        const QString ATTRIBUTE_LANG;
        const QString ATTRIBUTE_PREFIX;
        const QString ATTRIBUTE_ALIAS;
        const QString ATTRIBUTE_THRESHOLD;
        const QString ATTRIBUTE_COMPRESS;
    };
    const Strings m_strings;
    RCCFileInfo *root;
    void reset();
    bool addFile(const QString &alias, const RCCFileInfo &file);
    bool interpretResourceFile(QIODevice *inputDevice, const QString &file,
        QString currentPath = QString(), bool ignoreErrors = false);
    bool writeHeader(QIODevice &out);
    bool writeDataBlobs(QIODevice &out);
    bool writeDataNames(QIODevice &out);
    bool writeDataStructure(QIODevice &out);
    bool writeInitializer(QIODevice &out);

    QStringList mFileNames;
    QString mResourceRoot, mInitName;
    Format mFormat;
    bool mVerbose;
    int mCompressLevel;
    int mCompressThreshold;
    int mTreeOffset, mNamesOffset, mDataOffset;
    bool mUseNameSpace;
    QStringList mFailedResources;
    QIODevice *m_errorDevice;
};

QT_END_NAMESPACE

#endif // RCC_H

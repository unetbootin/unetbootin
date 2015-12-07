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

#include "rcc.h"

#include <QtCore/qglobal.h>
#include <QtCore/QIODevice>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QDateTime>
#include <QtCore/QByteArray>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QStack>
#include <QtXml/QDomDocument>

QT_BEGIN_NAMESPACE

enum { CONSTANT_USENAMESPACE = 1,
       CONSTANT_COMPRESSLEVEL_DEFAULT = -1,
       CONSTANT_COMPRESSTHRESHOLD_DEFAULT = 70 };

static bool qt_rcc_write_number(QIODevice &out, quint32 number,
    int width, RCCResourceLibrary::Format format)
{
    static QByteArray buf;
    int dividend = 1;
    switch (width) {
    case 2:
        dividend = 256;
        break;
    case 3:
        dividend = 65536;
        break;
    case 4:
        dividend = 16777216;
        break;
    default:
        break;
    }

    // Write <width> bytes
    while (dividend >= 1) {
        const quint8 tmp = number / dividend;
        if (format == RCCResourceLibrary::Binary) {
            out.putChar(tmp);
        } else if (format == RCCResourceLibrary::C_Code) {
            buf.setNum(tmp, 16);
            out.write("0x");
            out.write(buf);
            if (dividend > 1)
                out.write(",");
        }
        number -= tmp * dividend;
        dividend /= 256;
    }
    return true;
}

static inline QString msgOpenReadFailed(const QString &fname, const QString &why)
{
    return QString::fromUtf8("Unable to open %1 for reading: %2\n").arg(fname).arg(why);
}

///////////////////////////////////////////////////////////
//
// RCCBuilder
//
///////////////////////////////////////////////////////////

RCCBuilder::RCCBuilder() :
    writeBinary(false),
    verbose(false),
    compressLevel(CONSTANT_COMPRESSLEVEL_DEFAULT),
    compressThreshold(CONSTANT_COMPRESSTHRESHOLD_DEFAULT),
    useNameSpace(CONSTANT_USENAMESPACE)
{
}

void RCCBuilder::initializeLibrary(RCCResourceLibrary &library) const
{
    library.setFormat(writeBinary ? RCCResourceLibrary::Binary : RCCResourceLibrary::C_Code);
    library.setInitName(initName);
    library.setVerbose(verbose);
    library.setCompressLevel(compressLevel);
    library.setCompressThreshold(compressThreshold);
    library.setResourceRoot(resourceRoot);
    library.setUseNameSpace(useNameSpace);
}

bool RCCBuilder::processResourceFile(const QStringList &filenamesIn,
                                     const QString &filenameOut, bool list) const
{
    QFile standardError;
    standardError.open(stderr,  QIODevice::WriteOnly|QIODevice::Text);
    const bool rc = processResourceFile(filenamesIn, filenameOut, list, standardError);
    standardError.close();
    return rc;
}

bool RCCBuilder::processResourceFile(const QStringList &filenamesIn, const QString &filenameOut,
                                     bool list, QIODevice &errorDevice) const
{
    if (verbose)
        errorDevice.write("Qt resource compiler\n");

    //setup
    RCCResourceLibrary library;
    library.setInputFiles(filenamesIn);
    initializeLibrary(library);

    if (!library.readFiles(list, errorDevice))
        return false;

    //open output
    bool useStdOut = filenameOut.isEmpty() || filenameOut == QLatin1String("-");
    QFile out;
    QIODevice::OpenMode mode = QIODevice::WriteOnly;
    if (!writeBinary)
        mode |= QIODevice::Text;

    if (useStdOut) {
        // using this overload close() only flushes.
        out.open(stdout, mode);
    } else {
        out.setFileName(filenameOut);
        if (!out.open(mode)) {
            const QString msg = QString::fromUtf8("Unable to open %1 for writing: %2\n").arg(filenameOut).arg(out.errorString());
            errorDevice.write(msg.toUtf8());
            return false;
        }
    }

    //do the task
    bool ret = true;
    if (list) {
        const QStringList data = library.dataFiles();
        for (int i = 0; i < data.size(); ++i) {
            out.write(qPrintable(QDir::cleanPath(data.at(i))));
            out.write("\n");
        }
    } else {
        ret = library.output(out, errorDevice);
    }
    out.close();

    //done
    return ret;
}


///////////////////////////////////////////////////////////
//
// RCCFileInfo
//
///////////////////////////////////////////////////////////

struct RCCFileInfo
{
    enum Flags
    {
        NoFlags = 0x00,
        Compressed = 0x01,
        Directory = 0x02
    };

    RCCFileInfo(QString name = QString(), QFileInfo fileInfo = QFileInfo(),
                QLocale::Language language = QLocale::C,
                QLocale::Country country = QLocale::AnyCountry,
                uint flags = NoFlags,
                int compressLevel = CONSTANT_COMPRESSLEVEL_DEFAULT,
                int compressThreshold = CONSTANT_COMPRESSTHRESHOLD_DEFAULT);
    ~RCCFileInfo();

    QString resourceName() const;

    int flags;
    QString name;
    QLocale::Language language;
    QLocale::Country country;
    QFileInfo fileInfo;
    RCCFileInfo *parent;
    QHash<QString, RCCFileInfo*> children;
    int mCompressLevel;
    int mCompressThreshold;

    qint64 nameOffset, dataOffset, childOffset;
    qint64 writeDataBlob(QIODevice &out, qint64 offset, RCCResourceLibrary::Format format, QString *errorMessage);
    qint64 writeDataName(QIODevice &out, qint64 offset, RCCResourceLibrary::Format format);
    bool   writeDataInfo(QIODevice &out, RCCResourceLibrary::Format format);
};

RCCFileInfo::RCCFileInfo(QString name, QFileInfo fileInfo,
    QLocale::Language language, QLocale::Country country, uint flags,
    int compressLevel, int compressThreshold)
{
    this->name = name;
    this->fileInfo = fileInfo;
    this->language = language;
    this->country = country;
    this->flags = flags;
    this->parent = 0;
    this->nameOffset = this->dataOffset = this->childOffset = 0;
    this->mCompressLevel = compressLevel;
    this->mCompressThreshold = compressThreshold;
}

RCCFileInfo::~RCCFileInfo()
{
    qDeleteAll(children);
}

QString RCCFileInfo::resourceName() const
{
    QString resource = name;
    for (RCCFileInfo *p = parent; p; p = p->parent)
        resource = resource.prepend(p->name + QLatin1Char('/'));
    return QLatin1Char(':') + resource;
}

bool RCCFileInfo::writeDataInfo(QIODevice &out, RCCResourceLibrary::Format format)
{
    //some info
    if (format == RCCResourceLibrary::C_Code) {
        if (language != QLocale::C) {
            out.write("  // ");
            out.write(resourceName().toLocal8Bit().constData());
            out.write(" [");
            out.write(QByteArray::number(country));
            out.write("::");
            out.write(QByteArray::number(language));
            out.write("[\n  ");
        } else {
            out.write("  // ");
            out.write(resourceName().toLocal8Bit().constData());
            out.write("\n  ");
        }
    }

    //pointer data
    if (flags & RCCFileInfo::Directory) {
        //name offset
        qt_rcc_write_number(out, nameOffset, 4, format);
        if (format == RCCResourceLibrary::C_Code)
            out.write(",");

        //flags
        qt_rcc_write_number(out, flags, 2, format);
        if (format == RCCResourceLibrary::C_Code)
            out.write(",");

        //child count
        qt_rcc_write_number(out, children.size(), 4, format);
        if (format == RCCResourceLibrary::C_Code)
            out.write(",");

        //first child offset
        qt_rcc_write_number(out, childOffset, 4, format);
    } else {
        //name offset
        qt_rcc_write_number(out, nameOffset, 4, format);
        if (format == RCCResourceLibrary::C_Code)
            out.write(",");

        //flags
        qt_rcc_write_number(out, flags, 2, format);
        if (format == RCCResourceLibrary::C_Code)
            out.write(",");

        //locale
        qt_rcc_write_number(out, country, 2, format);
        if (format == RCCResourceLibrary::C_Code)
            out.write(",");
        qt_rcc_write_number(out, language, 2, format);
        if (format == RCCResourceLibrary::C_Code)
            out.write(",");

        //data offset
        qt_rcc_write_number(out, dataOffset, 4, format);
    }
    if (format == RCCResourceLibrary::C_Code)
        out.write(",\n");
    return true;
}

qint64 RCCFileInfo::writeDataBlob(QIODevice &out, qint64 offset, RCCResourceLibrary::Format format, QString *errorMessage)
{
    //capture the offset
    dataOffset = offset;

    //find the data to be written
    QFile file(fileInfo.absoluteFilePath());
    if (!file.open(QFile::ReadOnly)) {
        *errorMessage = msgOpenReadFailed(fileInfo.absoluteFilePath(), file.errorString());
        return 0;
    }
    QByteArray data = file.readAll();

#ifndef QT_NO_COMPRESS
    // Check if compression is useful for this file
    if (mCompressLevel != 0 && data.size() != 0) {
        QByteArray compressed = qCompress(reinterpret_cast<uchar *>(data.data()), data.size(), mCompressLevel);

        int compressRatio = int(100.0f * (float(data.size() - compressed.size()) / float(data.size())));
        if (compressRatio >= mCompressThreshold) {
            data = compressed;
            flags |= Compressed;
        }
    }
#endif // QT_NO_COMPRESS

    //some info
    if (format == RCCResourceLibrary::C_Code) {
        out.write("  // ");
        out.write(fileInfo.absoluteFilePath().toLocal8Bit().constData());
        out.write("\n  ");
    }

    //write the length
    qt_rcc_write_number(out, data.size(), 4, format);
    if (format == RCCResourceLibrary::C_Code)
        out.write(",\n  ");
    offset += 4;

    //write the payload
    for (int i=0; i<data.size(); i++) {
        qt_rcc_write_number(out, data.at(i), 1, format);
        if (format == RCCResourceLibrary::C_Code) {
            out.write(",");
            if (!(i % 16))
                out.write("\n  ");
        }
    }
    offset += data.size();

    //done
    if (format == RCCResourceLibrary::C_Code)
        out.write("\n  ");
    return offset;
}

qint64 RCCFileInfo::writeDataName(QIODevice &out, qint64 offset,
    RCCResourceLibrary::Format format)
{
    //capture the offset
    nameOffset = offset;

    //some info
    if (format == RCCResourceLibrary::C_Code) {
        out.write("  // ");
        out.write(name.toLocal8Bit().constData());
        out.write("\n  ");
    }

    //write the length
    qt_rcc_write_number(out, name.length(), 2, format);
    if (format == RCCResourceLibrary::C_Code)
        out.write(",\n  ");
    offset += 2;

    //write the hash
    qt_rcc_write_number(out, qHash(name), 4, format);
    if (format == RCCResourceLibrary::C_Code)
        out.write(",\n  ");
    offset += 4;

    //write the name
    const QChar *unicode = name.unicode();
    for (int i=0; i<name.length(); i++) {
        qt_rcc_write_number(out, unicode[i].unicode(), 2, format);
        if (format == RCCResourceLibrary::C_Code) {
            out.write(",");
            if (!(i % 16))
                out.write("\n  ");
        }
    }
    offset += name.length()*2;

    //done
    if (format == RCCResourceLibrary::C_Code)
        out.write("\n  ");
    return offset;
}


///////////////////////////////////////////////////////////
//
// RCCResourceLibrary
//
///////////////////////////////////////////////////////////

RCCResourceLibrary::Strings::Strings() :
   TAG_RCC(QLatin1String("RCC")),
   TAG_RESOURCE(QLatin1String("qresource")),
   TAG_FILE(QLatin1String("file")),
   ATTRIBUTE_LANG(QLatin1String("lang")),
   ATTRIBUTE_PREFIX(QLatin1String("prefix")),
   ATTRIBUTE_ALIAS(QLatin1String("alias")),
   ATTRIBUTE_THRESHOLD(QLatin1String("threshold")),
   ATTRIBUTE_COMPRESS(QLatin1String("compress"))
{
}

RCCResourceLibrary::RCCResourceLibrary() :
    root(0),
    mFormat(C_Code),
    mVerbose(false),
    mCompressLevel(CONSTANT_COMPRESSLEVEL_DEFAULT),
    mCompressThreshold(CONSTANT_COMPRESSTHRESHOLD_DEFAULT),
    mTreeOffset(0),
    mNamesOffset(0),
    mDataOffset(0),
    mUseNameSpace(CONSTANT_USENAMESPACE),
    m_errorDevice(0)
{
}

RCCResourceLibrary::~RCCResourceLibrary()
{
    delete root;
}

bool RCCResourceLibrary::interpretResourceFile(QIODevice *inputDevice,
    const QString &fname, QString currentPath, bool ignoreErrors)
{
    Q_ASSERT(m_errorDevice);
    const QChar slash =  QLatin1Char('/');
    if (!currentPath.isEmpty() && !currentPath.endsWith(slash))
        currentPath += slash;

    QDomDocument document;
    {
        QString errorMsg;
        int errorLine = 0, errorColumn = 0;
        if (!document.setContent(inputDevice, &errorMsg, &errorLine, &errorColumn)) {
            if (ignoreErrors)
                return true;
            const QString msg = QString::fromUtf8("RCC Parse Error: '%1' Line: %2 Column: %3 [%4]\n").arg(fname).arg(errorLine).arg(errorColumn).arg(errorMsg);
            m_errorDevice->write(msg.toUtf8());
            return false;
        }
    }

    QDomElement domRoot = document.firstChildElement(m_strings.TAG_RCC).toElement();
    if (!domRoot.isNull() && domRoot.tagName() == m_strings.TAG_RCC) {
        for (QDomNode node = domRoot.firstChild(); !node.isNull(); node = node.nextSibling()) {
            if (!node.isElement())
                continue;

            QDomElement child = node.toElement();
            if (!child.isNull() && child.tagName() == m_strings.TAG_RESOURCE) {
                QLocale::Language language = QLocale::c().language();
                QLocale::Country country = QLocale::c().country();

                if (child.hasAttribute(m_strings.ATTRIBUTE_LANG)) {
                    QString attribute = child.attribute(m_strings.ATTRIBUTE_LANG);
                    QLocale lang = QLocale(attribute);
                    language = lang.language();
                    if (2 == attribute.length()) {
                        // Language only
                        country = QLocale::AnyCountry;
                    } else {
                        country = lang.country();
                    }
                }

                QString prefix;
                if (child.hasAttribute(m_strings.ATTRIBUTE_PREFIX))
                    prefix = child.attribute(m_strings.ATTRIBUTE_PREFIX);
                if (!prefix.startsWith(slash))
                    prefix.prepend(slash);
                if (!prefix.endsWith(slash))
                    prefix += slash;

                for (QDomNode res = child.firstChild(); !res.isNull(); res = res.nextSibling()) {
                    if (res.isElement() && res.toElement().tagName() == m_strings.TAG_FILE) {

                        QString fileName(res.firstChild().toText().data());
                        if (fileName.isEmpty()) {
                            const QString msg = QString::fromUtf8("RCC: Warning: Null node in XML of '%1'\n").arg(fname);
                            m_errorDevice->write(msg.toUtf8());
                        }
                        QString alias;
                        if (res.toElement().hasAttribute(m_strings.ATTRIBUTE_ALIAS))
                            alias = res.toElement().attribute(m_strings.ATTRIBUTE_ALIAS);
                        else
                            alias = fileName;

                        int compressLevel = mCompressLevel;
                        if (res.toElement().hasAttribute(m_strings.ATTRIBUTE_COMPRESS))
                            compressLevel = res.toElement().attribute(m_strings.ATTRIBUTE_COMPRESS).toInt();
                        int compressThreshold = mCompressThreshold;
                        if (res.toElement().hasAttribute(m_strings.ATTRIBUTE_THRESHOLD))
                            compressThreshold = res.toElement().attribute(m_strings.ATTRIBUTE_THRESHOLD).toInt();

                        // Special case for -no-compress. Overrides all other settings.
                        if (mCompressLevel == -2)
                            compressLevel = 0;

                        alias = QDir::cleanPath(alias);
                        while (alias.startsWith(QLatin1String("../")))
                            alias.remove(0, 3);
                        alias = QDir::cleanPath(mResourceRoot) + prefix + alias;

                        QString absFileName = fileName;
                        if (QDir::isRelativePath(absFileName))
                            absFileName.prepend(currentPath);
                        QFileInfo file(absFileName);
                        if (!file.exists()) {
                            mFailedResources.push_back(absFileName);
                            const QString msg = QString::fromUtf8("RCC: Error in '%1': Cannot find file '%2'\n").arg(fname).arg(fileName);
                            m_errorDevice->write(msg.toUtf8());
                            if (ignoreErrors)
                                continue;
                            else
                                return false;
                        } else if (file.isFile()) {
                            const bool arc = addFile(alias, RCCFileInfo(alias.section(slash, -1), file, language, country,
                                                                        RCCFileInfo::NoFlags, compressLevel, compressThreshold));
                            if (!arc)
                                mFailedResources.push_back(absFileName);
                        } else {
                            QDir dir;
                            if (file.isDir()) {
                                dir.setPath(file.filePath());
                            } else {
                                dir.setPath(file.path());
                                dir.setNameFilters(QStringList(file.fileName()));
                                if (alias.endsWith(file.fileName()))
                                    alias = alias.left(alias.length()-file.fileName().length());
                            }
                            if (!alias.endsWith(slash))
                                alias += slash;
                            QDirIterator it(dir, QDirIterator::FollowSymlinks|QDirIterator::Subdirectories);
                            while (it.hasNext()) {
                                it.next();
                                QFileInfo child(it.fileInfo());
                                if (child.fileName() != QLatin1String(".") && child.fileName() != QLatin1String("..")) {
                                    const bool arc = addFile(alias + child.fileName(),
                                                             RCCFileInfo(child.fileName(), child, language, country,
                                                             RCCFileInfo::NoFlags, compressLevel, compressThreshold));
                                    if (!arc)
                                        mFailedResources.push_back(child.fileName());
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (root == 0) {
        const QString msg = QString::fromUtf8("RCC: Warning: No resources in '%1'.\n").arg(fname);
        m_errorDevice->write(msg.toUtf8());
        if (!ignoreErrors && mFormat == Binary) {
            // create dummy entry, otherwise loading qith QResource will crash
            root = new RCCFileInfo(QString(), QFileInfo(),
                    QLocale::C, QLocale::AnyCountry, RCCFileInfo::Directory);
        }
    }

    return true;
}

bool RCCResourceLibrary::addFile(const QString &alias, const RCCFileInfo &file)
{
    Q_ASSERT(m_errorDevice);
    if (file.fileInfo.size() > 0xffffffff) {
        const QString msg = QString::fromUtf8("File too big: %1\n").arg(file.fileInfo.absoluteFilePath());
        m_errorDevice->write(msg.toUtf8());
        return false;
    }
    if (!root)
        root = new RCCFileInfo(QString(), QFileInfo(), QLocale::C, QLocale::AnyCountry, RCCFileInfo::Directory);

    RCCFileInfo *parent = root;
    const QStringList nodes = alias.split(QLatin1Char('/'));
    for (int i = 1; i < nodes.size()-1; ++i) {
        const QString node = nodes.at(i);
        if (node.isEmpty())
            continue;
        if (!parent->children.contains(node)) {
            RCCFileInfo *s = new RCCFileInfo(node, QFileInfo(), QLocale::C, QLocale::AnyCountry, RCCFileInfo::Directory);
            s->parent = parent;
            parent->children.insert(node, s);
            parent = s;
        } else {
            parent = parent->children[node];
        }
    }

    const QString filename = nodes.at(nodes.size()-1);
    RCCFileInfo *s = new RCCFileInfo(file);
    s->parent = parent;
    parent->children.insertMulti(filename, s);
    return true;
}

void RCCResourceLibrary::reset()
{
     if (root) {
        delete root;
        root = 0;
    }
    m_errorDevice = 0;
    mFailedResources.clear();
}


bool RCCResourceLibrary::readFiles(bool ignoreErrors, QIODevice &errorDevice)
{
    reset();
    m_errorDevice = &errorDevice;
    //read in data
    if (mVerbose) {
        const QString msg = QString::fromUtf8("Processing %1 files [%2]\n").arg(mFileNames.size()).arg(static_cast<int>(ignoreErrors));
        m_errorDevice->write(msg.toUtf8());
    }
    for (int i=0; i<mFileNames.size(); ++i) {
        QFile fileIn;
        QString fname = mFileNames.at(i), pwd;
        if (fname == QLatin1String("-")) {
            fname = QLatin1String("(stdin)");
            pwd = QDir::currentPath();
            fileIn.setFileName(fname);
            if (!fileIn.open(stdin, QIODevice::ReadOnly)) {
                m_errorDevice->write(msgOpenReadFailed(fname, fileIn.errorString()).toUtf8());
                return false;
            }
        } else {
            pwd = QFileInfo(fname).path();
            fileIn.setFileName(fname);
            if (!fileIn.open(QIODevice::ReadOnly)) {
                m_errorDevice->write(msgOpenReadFailed(fname, fileIn.errorString()).toUtf8());
                return false;
            }
        }
        if (mVerbose) {
            const QString msg = QString::fromUtf8("Interpreting %1\n").arg(fname);
            m_errorDevice->write(msg.toUtf8());
        }

        if (!interpretResourceFile(&fileIn, fname, pwd, ignoreErrors))
            return false;
    }
    return true;
}

QStringList RCCResourceLibrary::dataFiles() const
{
    QStringList ret;
    QStack<RCCFileInfo*> pending;

    if (!root)
        return ret;
    pending.push(root);
    while (!pending.isEmpty()) {
        RCCFileInfo *file = pending.pop();
        for (QHash<QString, RCCFileInfo*>::iterator it = file->children.begin();
            it != file->children.end(); ++it) {
            RCCFileInfo *child = it.value();
            if (child->flags & RCCFileInfo::Directory)
                pending.push(child);
            ret.append(child->fileInfo.filePath());
        }
    }
    return ret;
}

// Determine map of resource identifier (':/newPrefix/images/p1.png') to file via recursion
static void resourceDataFileMapRecursion(const RCCFileInfo *root, const QString &path, RCCResourceLibrary::ResourceDataFileMap &m)
{
    typedef QHash<QString, RCCFileInfo*>::const_iterator ChildConstIterator;
    const QChar slash = QLatin1Char('/');
    const ChildConstIterator cend =root->children.constEnd();
    for (ChildConstIterator it = root->children.constBegin(); it != cend; ++it) {
        const RCCFileInfo *child = it.value();
        QString childName = path;
        childName += slash;
        childName += child->name;
        if (child->flags & RCCFileInfo::Directory) {
            resourceDataFileMapRecursion(child, childName, m);
        } else {
            m.insert(childName, child->fileInfo.filePath());
        }
    }
}

RCCResourceLibrary::ResourceDataFileMap RCCResourceLibrary::resourceDataFileMap() const
{
    ResourceDataFileMap rc;
    if (root)
        resourceDataFileMapRecursion(root, QString(QLatin1Char(':')),  rc);
    return rc;
}
bool RCCResourceLibrary::output(QIODevice &out, QIODevice &errorDevice)
{
     m_errorDevice = &errorDevice;
    //write out
    if (mVerbose)
        m_errorDevice->write("Outputting code\n");
    if (!writeHeader(out)) {
        m_errorDevice->write("Could not write header\n");
        return false;
    }
    if (this->root) {
        if (!writeDataBlobs(out)) {
            m_errorDevice->write("Could not write data blobs.\n");
            return false;
        }
        if (!writeDataNames(out)) {
            m_errorDevice->write("Could not write file names\n");
            return false;
        }
        if (!writeDataStructure(out)) {
            m_errorDevice->write("Could not write data tree\n");
            return false;
        }
    }
    if (!writeInitializer(out)) {
        m_errorDevice->write("Could not write footer\n");
        return false;
    }
    return true;
}

bool RCCResourceLibrary::writeHeader(QIODevice &out)
{
    if (mFormat == C_Code) {
        out.write("/****************************************************************************\n");
        out.write("** Resource object code\n");
        out.write("**\n");
        out.write("** Created: ");
        out.write(qPrintable(QDateTime::currentDateTime().toString()));
        out.write("\n**      by: The Resource Compiler for Qt version ");
        out.write(QT_VERSION_STR);
        out.write("\n**\n");
        out.write("** WARNING! All changes made in this file will be lost!\n");
        out.write( "*****************************************************************************/\n\n");
        out.write("#include <QtCore/qglobal.h>\n\n");
    } else if (mFormat == Binary) {
        out.write("qres");
        qt_rcc_write_number(out, 0, 4, mFormat);
        qt_rcc_write_number(out, 0, 4, mFormat);
        qt_rcc_write_number(out, 0, 4, mFormat);
        qt_rcc_write_number(out, 0, 4, mFormat);
    }
    return true;
}

bool RCCResourceLibrary::writeDataBlobs(QIODevice &out)
{
    Q_ASSERT(m_errorDevice);
    if (mFormat == C_Code)
        out.write("static const unsigned char qt_resource_data[] = {\n");
    else if (mFormat == Binary)
        mDataOffset = out.pos();
    QStack<RCCFileInfo*> pending;

    if (!root)
        return false;

    pending.push(root);
    qint64 offset = 0;
    QString errorMessage;
    while (!pending.isEmpty()) {
        RCCFileInfo *file = pending.pop();
        for (QHash<QString, RCCFileInfo*>::iterator it = file->children.begin();
            it != file->children.end(); ++it) {
            RCCFileInfo *child = it.value();
            if (child->flags & RCCFileInfo::Directory)
                pending.push(child);
            else {
                offset = child->writeDataBlob(out, offset, mFormat, &errorMessage);
                if (offset == 0)
                    m_errorDevice->write(errorMessage.toUtf8());
            }
        }
    }
    if (mFormat == C_Code)
        out.write("\n};\n\n");
    return true;
}

bool RCCResourceLibrary::writeDataNames(QIODevice &out)
{
    if (mFormat == C_Code)
        out.write("static const unsigned char qt_resource_name[] = {\n");
    else if (mFormat == Binary)
        mNamesOffset = out.pos();

    QHash<QString, int> names;
    QStack<RCCFileInfo*> pending;

    if (!root)
        return false;

    pending.push(root);
    qint64 offset = 0;
    while (!pending.isEmpty()) {
        RCCFileInfo *file = pending.pop();
        for (QHash<QString, RCCFileInfo*>::iterator it = file->children.begin();
            it != file->children.end(); ++it) {
            RCCFileInfo *child = it.value();
            if (child->flags & RCCFileInfo::Directory)
                pending.push(child);
            if (names.contains(child->name)) {
                child->nameOffset = names.value(child->name);
            } else {
                names.insert(child->name, offset);
                offset = child->writeDataName(out, offset, mFormat);
            }
        }
    }
    if (mFormat == C_Code)
        out.write("\n};\n\n");
    return true;
}

static bool qt_rcc_compare_hash(const RCCFileInfo *left, const RCCFileInfo *right)
{
    return qHash(left->name) < qHash(right->name);
}

bool RCCResourceLibrary::writeDataStructure(QIODevice &out)
{
    if (mFormat == C_Code)
        out.write("static const unsigned char qt_resource_struct[] = {\n");
    else if (mFormat == Binary)
        mTreeOffset = out.pos();
    QStack<RCCFileInfo*> pending;

    if (!root)
        return false;

    //calculate the child offsets (flat)
    pending.push(root);
    int offset = 1;
    while (!pending.isEmpty()) {
        RCCFileInfo *file = pending.pop();
        file->childOffset = offset;

        //sort by hash value for binary lookup
        QList<RCCFileInfo*> children = file->children.values();
        qSort(children.begin(), children.end(), qt_rcc_compare_hash);

        //write out the actual data now
        for (int i = 0; i < children.size(); ++i) {
            RCCFileInfo *child = children.at(i);
            ++offset;
            if (child->flags & RCCFileInfo::Directory)
                pending.push(child);
        }
    }

    //write out the structure (ie iterate again!)
    pending.push(root);
    root->writeDataInfo(out, mFormat);
    while (!pending.isEmpty()) {
        RCCFileInfo *file = pending.pop();

        //sort by hash value for binary lookup
        QList<RCCFileInfo*> children = file->children.values();
        qSort(children.begin(), children.end(), qt_rcc_compare_hash);

        //write out the actual data now
        for (int i = 0; i < children.size(); ++i) {
            RCCFileInfo *child = children.at(i);
            child->writeDataInfo(out, mFormat);
            if (child->flags & RCCFileInfo::Directory)
                pending.push(child);
        }
    }
    if (mFormat == C_Code)
        out.write("\n};\n\n");

    return true;
}

// Helper to write a function with namespace modifier
enum NameSpaceMode { NameSpaceNone, NameSpaceMangle, NameSpaceAdd };
static inline void writeFunction(QIODevice &out,NameSpaceMode nm,  const char *name)
{
    switch(nm) {
    case NameSpaceNone:
        break;
    case NameSpaceMangle:
        out.write("QT_MANGLE_NAMESPACE(");
        break;
    case NameSpaceAdd:
        out.write("QT_PREPEND_NAMESPACE(");
        break;
    }
    out.write(name);
    if (nm != NameSpaceNone)
        out.write(")");
}

bool RCCResourceLibrary::writeInitializer(QIODevice &out)
{
    if (mFormat == C_Code) {
        //out.write("\nQT_BEGIN_NAMESPACE\n");
        QString initName = mInitName;
        if (!initName.isEmpty()) {
            initName.prepend(QLatin1Char('_'));
            initName.replace(QRegExp(QLatin1String("[^a-zA-Z0-9_]")), QLatin1String("_"));
        }

        //init
        if (mUseNameSpace)
            out.write("QT_BEGIN_NAMESPACE\n\n");
        if (this->root) {
            out.write("extern bool qRegisterResourceData\n    "
                "(int, const unsigned char *, "
                "const unsigned char *, const unsigned char *);\n\n");
            out.write("extern bool qUnregisterResourceData\n    "
                "(int, const unsigned char *, "
                "const unsigned char *, const unsigned char *);\n\n");
        }
        if (mUseNameSpace)
            out.write("QT_END_NAMESPACE\n\n\n");
        QString initResources = QLatin1String("qInitResources");
        initResources += initName;
        out.write("int ");
        writeFunction(out, mUseNameSpace ? NameSpaceMangle : NameSpaceNone, initResources.toLatin1().constData());
        out.write("()\n{\n");

        if (this->root) {
            out.write("    ");
            writeFunction(out, mUseNameSpace ?  NameSpaceAdd : NameSpaceNone, "qRegisterResourceData");
             out.write("\n        (0x01, qt_resource_struct, "
                       "qt_resource_name, qt_resource_data);\n");
        }
        out.write("    return 1;\n");
        out.write("}\n\n");
        out.write("Q_CONSTRUCTOR_FUNCTION(");
        writeFunction(out, mUseNameSpace ? NameSpaceMangle : NameSpaceNone, initResources.toLatin1().constData());
        out.write(")\n\n");

        //cleanup
        QString cleanResources = QLatin1String("qCleanupResources");
        cleanResources += initName;
        out.write("int ");
        writeFunction(out, mUseNameSpace ? NameSpaceMangle : NameSpaceNone, cleanResources.toLatin1().constData());
        out.write("()\n{\n");
        if (this->root) {
            out.write("    ");
            writeFunction(out, mUseNameSpace ?  NameSpaceAdd : NameSpaceNone, "qUnregisterResourceData");
            out.write("\n       (0x01, qt_resource_struct, "
                      "qt_resource_name, qt_resource_data);\n");
        }
        out.write("    return 1;\n");
        out.write("}\n\n");
        out.write("Q_DESTRUCTOR_FUNCTION(");
        writeFunction(out, mUseNameSpace ? NameSpaceMangle : NameSpaceNone, cleanResources.toLatin1().constData());
        out.write(")\n\n");
    } else if (mFormat == Binary) {
        const long old_pos = out.pos();
        out.seek(4);
        qt_rcc_write_number(out, 0x01, 4, mFormat);
        qt_rcc_write_number(out, mTreeOffset, 4, mFormat);
        qt_rcc_write_number(out, mDataOffset, 4, mFormat);
        qt_rcc_write_number(out, mNamesOffset, 4, mFormat);
        out.seek(old_pos);
    }
    return true;
}

QT_END_NAMESPACE

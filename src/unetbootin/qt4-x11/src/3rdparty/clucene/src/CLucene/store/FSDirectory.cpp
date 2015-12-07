/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
*/
#include <QtCore/QDir>
#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>
#include <QtCore/QByteArray>
#include <QtCore/QCryptographicHash>

#include "CLucene/StdHeader.h"
#include "FSDirectory.h"
#include "CLucene/index/IndexReader.h"
#include "CLucene/util/Misc.h"
#include "CLucene/debug/condition.h"

CL_NS_DEF(store)
CL_NS_USE(util)

bool FSDirectory::disableLocks = false;

// This cache of directories ensures that there is a unique Directory instance
// per path, so that synchronization on the Directory can be used to synchronize
// access between readers and writers.
static CL_NS(util)::CLHashMap<QString, FSDirectory*,
    CL_NS(util)::Compare::Qstring, CL_NS(util)::Equals::Qstring,
    CL_NS(util)::Deletor::DummyQString> DIRECTORIES(false, false);
 
// # pragma mark -- FSDirectory::FSLock

FSDirectory::FSLock::FSLock(const QString& _lockDir, const QString& name)
    : lockDir(_lockDir)
    , lockFile(_lockDir + QDir::separator() + name)
{
}

FSDirectory::FSLock::~FSLock()
{
}

bool FSDirectory::FSLock::obtain()
{
    if (disableLocks)
        return true;

    if (QFile::exists(lockFile))
        return false;

    QDir dir(lockDir);
    if (!dir.exists()) {
        if (!dir.mkpath(lockDir)) {
            // 34: len of "Couldn't create lock directory: "
            char* err = _CL_NEWARRAY(
                char, 34 + strlen(lockDir.toLocal8Bit().constData()) + 1);
            strcpy(err, "Couldn't create lock directory: ");
            strcat(err, lockDir.toLocal8Bit().constData());
            _CLTHROWA_DEL(CL_ERR_IO, err);
        }
    }

    QFile file(lockFile);
    return file.open(QIODevice::ReadWrite);
}

void FSDirectory::FSLock::release()
{
    if (disableLocks)
        return;
    
    QFile file(lockFile);
    file.remove();
}

bool FSDirectory::FSLock::isLocked()
{
    if (disableLocks)
        return false;
    return QFile::exists(lockFile);
}

QString FSDirectory::FSLock::toString() const
{
    QString ret(QLatin1String("Lock@"));
    return ret.append(lockFile);
}

// # pragma mark -- FSDirectory::FSIndexInput

FSDirectory::FSIndexInput::FSIndexInput(const QString& path, int32_t bufferSize)
    : BufferedIndexInput(bufferSize)	
{
    CND_PRECONDITION(!path.isEmpty(), "path is NULL");

    handle = _CLNEW SharedHandle();
    handle->fhandle.setFileName(path);
    handle->fhandle.open(QIODevice::ReadOnly);
    
    if (handle->fhandle.error() != QFile::NoError) {
        switch(handle->fhandle.error()) {
            case 1:
                _CLTHROWA(CL_ERR_IO, "An error occurred when reading from the file");
                break;
            case 2:
                _CLTHROWA(CL_ERR_IO, "An error occurred when writing to the file.");
                break;
            case 5:
                _CLTHROWA(CL_ERR_IO, "The file could not be opened.");
                break;
            case 6:
                _CLTHROWA(CL_ERR_IO, "The operation was aborted.");
                break;
            case 7:
                _CLTHROWA(CL_ERR_IO, "A timeout occurred.");
                break;
            case 8:
                _CLTHROWA(CL_ERR_IO, "An unspecified error occurred.");
                break;
            case 9:
                _CLTHROWA(CL_ERR_IO, "The file could not be removed.");
                break;
            case 10:
                _CLTHROWA(CL_ERR_IO, "The file could not be renamed.");
                break;
            case 11:
                _CLTHROWA(CL_ERR_IO, "The position in the file could not be changed.");
                break;
            case 12:
                _CLTHROWA(CL_ERR_IO, "The file could not be resized.e");
                break;
            case 13:
                _CLTHROWA(CL_ERR_IO, "The file could not be accessed.");
                break;
            case 14:
                _CLTHROWA(CL_ERR_IO, "The file could not be copied.");
                break;
            case 4:
            default:
                _CLTHROWA(CL_ERR_IO, "A fatal error occurred.");
        }
    }

    //Store the file length
    handle->_length = handle->fhandle.size();
    handle->_fpos = 0;
    this->_pos = 0;
}

FSDirectory::FSIndexInput::FSIndexInput(const FSIndexInput& other)
    : BufferedIndexInput(other)
{
    if (other.handle == NULL)
        _CLTHROWA(CL_ERR_NullPointer, "other handle is null");

    SCOPED_LOCK_MUTEX(other.handle->THIS_LOCK)

    _pos = other.handle->_fpos;
    handle = _CL_POINTER(other.handle);
}

FSDirectory::FSIndexInput::~FSIndexInput()
{
    FSIndexInput::close();
}

void FSDirectory::FSIndexInput::close()
{
    BufferedIndexInput::close();
    _CLDECDELETE(handle);
}

IndexInput* FSDirectory::FSIndexInput::clone() const
{
    return _CLNEW FSDirectory::FSIndexInput(*this);
}

void FSDirectory::FSIndexInput::seekInternal(const int64_t position)
{
    CND_PRECONDITION(position >= 0 && position < handle->_length,
        "Seeking out of range")
    _pos = position;
}

void FSDirectory::FSIndexInput::readInternal(uint8_t* b, const int32_t len)
{
    SCOPED_LOCK_MUTEX(handle->THIS_LOCK)

    CND_PRECONDITION(handle != NULL, "shared file handle has closed");
    CND_PRECONDITION(handle->fhandle.isOpen(), "file is not open");

    if (handle->_fpos != _pos) {
        handle->fhandle.seek(_pos);
        if (handle->fhandle.pos() != _pos)
            _CLTHROWA( CL_ERR_IO, "File IO Seek error");
        handle->_fpos = _pos;
    }

    bufferLength = (int32_t)handle->fhandle.read((char*)b, len);
    if (bufferLength == 0)
        _CLTHROWA(CL_ERR_IO, "read past EOF");

    if (bufferLength == -1)
        _CLTHROWA(CL_ERR_IO, "read error");

    _pos += bufferLength;
    handle->_fpos =_pos;
}

// # pragma mark -- FSDirectory::FSIndexInput::SharedHandle

FSDirectory::FSIndexInput::SharedHandle::SharedHandle()
    : _fpos(0)
    , _length(0)

{
}

FSDirectory::FSIndexInput::SharedHandle::~SharedHandle()
{
    if (fhandle.isOpen())
        fhandle.close();
}

// # pragma mark -- FSDirectory::FSIndexOutput

FSDirectory::FSIndexOutput::FSIndexOutput(const QString& path)
{
    //O_BINARY - Opens file in binary (untranslated) mode
    //O_CREAT - Creates and opens new file for writing. Has no effect if file specified by filename exists
    //O_RANDOM - Specifies that caching is optimized for, but not restricted to, random access from disk.
    //O_WRONLY - Opens file for writing only;
    fhandle.setFileName(path);
    fhandle.open(QIODevice::ReadWrite | QIODevice::Truncate);
    
    if (fhandle.error() != QFile::NoError) {
        switch(fhandle.error()) {
            case 1:
                _CLTHROWA(CL_ERR_IO, "An error occurred when reading from the file");
                break;
            case 2:
                _CLTHROWA(CL_ERR_IO, "An error occurred when writing to the file.");
                break;
            case 5:
                _CLTHROWA(CL_ERR_IO, "The file could not be opened.");
                break;
            case 6:
                _CLTHROWA(CL_ERR_IO, "The operation was aborted.");
                break;
            case 7:
                _CLTHROWA(CL_ERR_IO, "A timeout occurred.");
                break;
            case 8:
                _CLTHROWA(CL_ERR_IO, "An unspecified error occurred.");
                break;
            case 9:
                _CLTHROWA(CL_ERR_IO, "The file could not be removed.");
                break;
            case 10:
                _CLTHROWA(CL_ERR_IO, "The file could not be renamed.");
                break;
            case 11:
                _CLTHROWA(CL_ERR_IO, "The position in the file could not be changed.");
                break;
            case 12:
                _CLTHROWA(CL_ERR_IO, "The file could not be resized.e");
                break;
            case 13:
                _CLTHROWA(CL_ERR_IO, "The file could not be accessed.");
                break;
            case 14:
                _CLTHROWA(CL_ERR_IO, "The file could not be copied.");
                break;
            case 4:
            default:
                _CLTHROWA(CL_ERR_IO, "A fatal error occurred.");
        }
    }
}

FSDirectory::FSIndexOutput::~FSIndexOutput()
{
    if (fhandle.isOpen()) {
        try {
            FSIndexOutput::close();
        } catch (CLuceneError& err) {
            //ignore IO errors...
            if (err.number() != CL_ERR_IO)
                throw;
        }
    }
}

void FSDirectory::FSIndexOutput::close()
{
    try {
        BufferedIndexOutput::close();
    } catch (CLuceneError& err) {
        //ignore IO errors...
        if (err.number() != CL_ERR_IO)
            throw;
    }
    fhandle.close();
}

int64_t FSDirectory::FSIndexOutput::length()
{
    CND_PRECONDITION(fhandle.isOpen(), "file is not open");
    return fhandle.size();
}

void FSDirectory::FSIndexOutput::seek(const int64_t pos)
{
    CND_PRECONDITION(fhandle.isOpen(), "file is not open");
    
    BufferedIndexOutput::seek(pos);
    fhandle.seek(pos);
    if (fhandle.pos() != pos)
        _CLTHROWA(CL_ERR_IO, "File IO Seek error");
}

void FSDirectory::FSIndexOutput::flushBuffer(const uint8_t* b, const int32_t size)
{
    CND_PRECONDITION(fhandle.isOpen(), "file is not open");

    if (size > 0 && fhandle.write((const char*)b, size) != size)
        _CLTHROWA(CL_ERR_IO, "File IO Write error");
}

// # pragma mark -- FSDirectory

FSDirectory::FSDirectory(const QString& path, const bool createDir)
    : Directory()
    , refCount(0)
    , useMMap(false)
{
    //set a realpath so that if we change directory, we can still function
    directory = QFileInfo(path).absoluteFilePath();
    lockDir = directory;

    QDir dir(lockDir);
    if (!dir.exists()) {
        if (!dir.mkpath(lockDir))
            _CLTHROWA_DEL(CL_ERR_IO, "Cannot create temp directory");
    }

    QFileInfo info(lockDir);
    if (info.isFile() || info.isSymLink())
        _CLTHROWA(CL_ERR_IO, "Found regular file where directory expected");

    if (createDir)
        create();

    dir.setPath(directory);
    if (!dir.exists()) {
         //19: len of " is not a directory"
        char* err = 
            _CL_NEWARRAY(char, 19 + strlen(path.toLocal8Bit().constData()) + 1);
        strcpy(err, path.toLocal8Bit().constData());
        strcat(err, " is not a directory");
        _CLTHROWA_DEL(CL_ERR_IO, err);
    }
}

void FSDirectory::create()
{
    SCOPED_LOCK_MUTEX(THIS_LOCK)

    bool clear = false;
    QDir dir(directory);
    if (!dir.exists()) {
        if (!dir.mkpath(directory)) {
            char* err = _CL_NEWARRAY( // 27 len of "Couldn't create directory:"
                char, 27 + strlen(directory.toLocal8Bit().constData()) + 1);
            strcpy(err, "Couldn't create directory: ");
            strcat(err, directory.toLocal8Bit().constData());
            _CLTHROWA_DEL(CL_ERR_IO, err);
        }
    } else {
        clear = true;
    }

    QFileInfo info(directory);
    if (info.isFile() || info.isSymLink()) {
        char tmp[1024];
        _snprintf(tmp, 1024, "%s not a directory",
            directory.toLocal8Bit().constData());
        _CLTHROWA(CL_ERR_IO, tmp);
    }

    if (clear) {
        dir.setPath(directory);
        // clear probably existing lucene index files 
        QStringList fileList = dir.entryList(QDir::Files | QDir::Hidden
            | QDir::NoSymLinks);
        foreach(const QString file, fileList) {
            if (CL_NS(index)::IndexReader::isLuceneFile(file)) {
                if (!dir.remove(file))
                    _CLTHROWA(CL_ERR_IO, "Couldn't delete file ");
            }
        }

        // clear probably existing file locks
        QFileInfo dirInfo(lockDir);
        if (dirInfo.exists() && dirInfo.isReadable() && dirInfo.isWritable()
            && !dirInfo.isFile() && !dirInfo.isSymLink()) {
            QDir lockDirectory(lockDir);
            fileList = dir.entryList(QStringList() << getLockPrefix()
                + QLatin1Char('*'), QDir::Files | QDir::Hidden | QDir::NoSymLinks);

            foreach(const QString file, fileList) {
                if (!lockDirectory.remove(file))
                    _CLTHROWA(CL_ERR_IO, "Couldn't delete file ");
            }
        }
        else {
             //todo: richer error: + lockDir.getAbsolutePath());
            _CLTHROWA(CL_ERR_IO, "Cannot read lock directory");
        }
    }
}

void FSDirectory::priv_getFN(QString& buffer, const QString& name) const
{
    buffer.clear();
    buffer.append(directory);
    buffer.append(QDir::separator());
    buffer.append(name);
}

FSDirectory::~FSDirectory()
{
}

QStringList FSDirectory::list() const
{
    CND_PRECONDITION(!directory.isEmpty(), "directory is not open");

    QDir dir(directory);
    return dir.entryList(QDir::Files | QDir::Hidden);
}

bool FSDirectory::fileExists(const QString& name) const
{
    CND_PRECONDITION(!directory.isEmpty(), "directory is not open");

    QDir dir(directory);
    return dir.entryList().contains(name);
}

QString FSDirectory::getDirName() const
{
    return directory;
}

//static
FSDirectory* FSDirectory::getDirectory(const QString& file, const bool _create)
{
    FSDirectory* dir = NULL;
    {
        if (file.isEmpty())
            _CLTHROWA(CL_ERR_IO, "Invalid directory");

        SCOPED_LOCK_MUTEX(DIRECTORIES.THIS_LOCK)
            dir = DIRECTORIES.get(file);
        if ( dir == NULL  ){
            dir = _CLNEW FSDirectory(file, _create);
            DIRECTORIES.put(dir->directory, dir);
        } else if (_create) {
            dir->create();
        }

        {
            SCOPED_LOCK_MUTEX(dir->THIS_LOCK)
            dir->refCount++;
        }
    }    

    return _CL_POINTER(dir);
}

int64_t FSDirectory::fileModified(const QString& name) const
{
    CND_PRECONDITION(!directory.isEmpty(), "directory is not open");

    QFileInfo fInfo(directory + QDir::separator() + name);
    return fInfo.lastModified().toTime_t();
}

//static
int64_t FSDirectory::fileModified(const QString& dir, const QString& name)
{
    QFileInfo fInfo(dir + QDir::separator() + name);
    return fInfo.lastModified().toTime_t();
}

void FSDirectory::touchFile(const QString& name)
{
    CND_PRECONDITION(!directory.isEmpty(), "directory is not open");

    QFile file(directory + QDir::separator() + name);
    if (!file.open(QIODevice::ReadWrite))
        _CLTHROWA(CL_ERR_IO, "IO Error while touching file");
}

int64_t FSDirectory::fileLength(const QString& name) const
{
    CND_PRECONDITION(!directory.isEmpty(), "directory is not open");

    QFileInfo fInfo(directory + QDir::separator() + name);
    return fInfo.size();
}

IndexInput* FSDirectory::openInput(const QString& name)
{
    return openInput(name, CL_NS(store)::BufferedIndexOutput::BUFFER_SIZE);
}

IndexInput* FSDirectory::openInput(const QString& name, int32_t bufferSize )
{
    CND_PRECONDITION(directory[0]!=0,"directory is not open")

    return _CLNEW FSIndexInput(directory + QDir::separator() + name, bufferSize);
}

void FSDirectory::close()
{
    SCOPED_LOCK_MUTEX(DIRECTORIES.THIS_LOCK)
    {
        SCOPED_LOCK_MUTEX(THIS_LOCK)

        CND_PRECONDITION(!directory.isEmpty(), "directory is not open");

        //refcount starts at 1
        if (--refCount <= 0) {
            Directory* dir = DIRECTORIES.get(getDirName());
            if (dir) {
                //this will be removed in ~FSDirectory
                DIRECTORIES.remove(getDirName());
                _CLDECDELETE(dir);
            }
        }
    }
}

QString FSDirectory::getLockPrefix() const
{
    CND_PRECONDITION(!directory.isEmpty(), "directory is not open");

    QString dirName(QFileInfo(directory).absoluteFilePath());
    if (dirName.isEmpty())
        _CLTHROWA(CL_ERR_Runtime, "Invalid directory path");

    // to be compatible with jlucene,
    // we need to make some changes ...
    if (dirName.at(1) == QLatin1Char(':'))
        dirName[0] = dirName.at(0).toUpper();

    TCHAR tBuffer[2048] = { 0 };
    dirName.toWCharArray(tBuffer);
    
    char aBuffer[4096] = { 0 };
    STRCPY_TtoA(aBuffer, tBuffer, 4096);

    QString string(QLatin1String("lucene-"));
    QByteArray hash(QCryptographicHash::hash(aBuffer, QCryptographicHash::Md5));

    // TODO: verify this !!!
    return string.append(QLatin1String(hash.toHex().constData()));
}

bool FSDirectory::doDeleteFile(const QString& name)
{
    CND_PRECONDITION(!directory.isEmpty(), "directory is not open");

    QDir dir(directory);
    return dir.remove(name);
}

void FSDirectory::renameFile(const QString& from, const QString& to)
{
    CND_PRECONDITION(!directory.isEmpty(), "directory is not open");
    SCOPED_LOCK_MUTEX(THIS_LOCK)
    
    if (fileExists(to))
        deleteFile(to, false);

    QFile file(directory + QDir::separator() + from);
    QString newFile(directory + QDir::separator() + to);
    if (!file.rename(newFile)) {
        // try a second time if we fail
        if (fileExists(to))
            deleteFile(to, false);

        if (!file.rename(newFile)) {
            QString error(QLatin1String("Could not rename: %1 to %2!!!!"));
            error.arg(from).arg(newFile);
            QByteArray bArray(error.toLocal8Bit());
            _CLTHROWA(CL_ERR_IO, bArray.constData());
        }
    }
}

IndexOutput* FSDirectory::createOutput(const QString& name)
{
    CND_PRECONDITION(!directory.isEmpty(), "directory is not open");

    QString file = directory + QDir::separator() + name;
    if (QFileInfo(file).exists()) {
        if (!QFile::remove(file)) {
            QByteArray bArray("Cannot overwrite: ");
            bArray.append(name.toLocal8Bit());
            _CLTHROWA(CL_ERR_IO, bArray.constData());
        }
    }
    return _CLNEW FSIndexOutput(file);
}

LuceneLock* FSDirectory::makeLock(const QString& name)
{
    CND_PRECONDITION(!directory.isEmpty(), "directory is not open");


    QString lockFile(getLockPrefix());
    lockFile.append(QLatin1String("-")).append(name);

    return _CLNEW FSLock(lockDir, lockFile);
}

QString FSDirectory::toString() const
{
    return QString::fromLatin1("FSDirectory@").append(directory);
}

CL_NS_END

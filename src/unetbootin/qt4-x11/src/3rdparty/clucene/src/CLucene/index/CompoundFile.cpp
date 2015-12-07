/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
*
* Changes are Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "CompoundFile.h"
#include "CLucene/util/Misc.h"

CL_NS_USE(store)
CL_NS_USE(util)
CL_NS_DEF(index)

CompoundFileReader::CSIndexInput::CSIndexInput(CL_NS(store)::IndexInput* base,
    const int64_t fileOffset, const int64_t length)
{
    this->base = base;
    this->fileOffset = fileOffset;
    this->_length = length;
}

void CompoundFileReader::CSIndexInput::readInternal(uint8_t* b, const int32_t len)
{
    SCOPED_LOCK_MUTEX(base->THIS_LOCK)

    int64_t start = getFilePointer();
    if(start + len > _length)
        _CLTHROWA(CL_ERR_IO, "read past EOF");
    base->seek(fileOffset + start);
    base->readBytes(b, len);
}

CompoundFileReader::CSIndexInput::~CSIndexInput()
{
}

IndexInput* CompoundFileReader::CSIndexInput::clone() const
{
    return _CLNEW CSIndexInput(*this);
}

CompoundFileReader::CSIndexInput::CSIndexInput(const CSIndexInput& clone)
    : BufferedIndexInput(clone)
{
    this->base = clone.base; //no need to clone this..
    this->fileOffset = clone.fileOffset;
    this->_length = clone._length;
}

void CompoundFileReader::CSIndexInput::close()
{
}

CompoundFileReader::CompoundFileReader(Directory* dir, const QString& name)
    : entries(false, true)
{
    directory = dir;
    fileName = name;

    bool success = false;
    try {
        stream = dir->openInput(name);

        // read the directory and init files
        int32_t count = stream->readVInt();
        FileEntry* entry = NULL;
        TCHAR tid[CL_MAX_PATH];
        for (int32_t i = 0; i < count; i++) {
            int64_t offset = stream->readLong();
            int32_t read = stream->readString(tid, CL_MAX_PATH);
            QString aid(QString::fromWCharArray(tid, read));

            // set length of the previous entry
            if (entry != NULL)
                entry->length = offset - entry->offset;

            entry = _CLNEW FileEntry(offset);
            entries.put(aid, entry);
        }

        // set the length of the final entry
        if (entry != NULL)
            entry->length = stream->length() - entry->offset;
        success = true;
    } _CLFINALLY (
        if (!success && (stream != NULL)) {
            try {
                stream->close();
                _CLDELETE(stream);
            } catch (CLuceneError& err) {
                if (err.number() != CL_ERR_IO)
                    throw err;
            }
        }
    )
}

CompoundFileReader::~CompoundFileReader()
{
    close();
}

Directory* CompoundFileReader::getDirectory()
{
    return directory;
}

QString CompoundFileReader::getName() const
{
    return fileName;
}

void CompoundFileReader::close()
{
    SCOPED_LOCK_MUTEX(THIS_LOCK)

    if (stream != NULL) {
        entries.clear();
        stream->close();
        _CLDELETE(stream);
    }
}

IndexInput* CompoundFileReader::openInput(const QString& id)
{
    SCOPED_LOCK_MUTEX(THIS_LOCK)

    if (stream == NULL)
        _CLTHROWA(CL_ERR_IO, "Stream closed");

    const FileEntry* entry = entries.get(id);
    if (entry == NULL) {
        char buf[CL_MAX_PATH + 30];
        strcpy(buf,"No sub-file with id ");
        strncat(buf, id.toLocal8Bit().constData(), CL_MAX_PATH);
        strcat(buf, " found");
        _CLTHROWA(CL_ERR_IO,buf);
    }
    return _CLNEW CSIndexInput(stream, entry->offset, entry->length);
}

QStringList CompoundFileReader::list() const
{
    // for (EntriesType::const_iterator i=entries.begin();i!=entries.end();i++){
    //     names->push_back(i->first);
    //     ++i;
    // }

    QStringList names;
    EntriesType::const_iterator itr;
    // TODO: verify this, see old code above ???
    for (itr = entries.begin(); itr != entries.end(); ++itr)
        names.push_back(itr->first);

    return names;
}

bool CompoundFileReader::fileExists(const QString& name) const
{
    return entries.exists(name);
}

int64_t CompoundFileReader::fileModified(const QString& name) const
{
    return directory->fileModified(fileName);
}

void CompoundFileReader::touchFile(const QString& name)
{
    directory->touchFile(fileName);
}

bool CompoundFileReader::doDeleteFile(const QString& name)
{
    _CLTHROWA(CL_ERR_UnsupportedOperation,
        "UnsupportedOperationException: CompoundFileReader::doDeleteFile");
}

void CompoundFileReader::renameFile(const QString& from, const QString& to)
{
    _CLTHROWA(CL_ERR_UnsupportedOperation,
        "UnsupportedOperationException: CompoundFileReader::renameFile");
}

int64_t CompoundFileReader::fileLength(const QString& name) const
{
    FileEntry* e = entries.get(name);
    if (e == NULL) {
        char buf[CL_MAX_PATH + 30];
        strcpy(buf,"File ");
        strncat(buf, name.toLocal8Bit().constData(), CL_MAX_PATH);
        strcat(buf," does not exist");
        _CLTHROWA(CL_ERR_IO,buf);
    }
    return e->length;
}

IndexOutput* CompoundFileReader::createOutput(const QString& name)
{
    _CLTHROWA(CL_ERR_UnsupportedOperation,
        "UnsupportedOperationException: CompoundFileReader::createOutput");
}

LuceneLock* CompoundFileReader::makeLock(const QString& name)
{
    _CLTHROWA(CL_ERR_UnsupportedOperation,
        "UnsupportedOperationException: CompoundFileReader::makeLock");
}

QString CompoundFileReader::toString() const
{
    QString ret(QLatin1String("CompoundFileReader@"));
    return ret.append(fileName);
}

CompoundFileWriter::CompoundFileWriter(Directory* dir, const QString& name)
    : ids(false)
    , entries(true)
{
    if (dir == NULL)
        _CLTHROWA(CL_ERR_NullPointer, "directory cannot be null");

    if (name.isEmpty())
        _CLTHROWA(CL_ERR_NullPointer, "name cannot be null");

    merged = false;
    directory = dir;
    fileName = name;
}

CompoundFileWriter::~CompoundFileWriter()
{
}

Directory* CompoundFileWriter::getDirectory()
{
    return directory;
}

/** Returns the name of the compound file. */
QString CompoundFileWriter::getName() const
{
    return fileName;
}

void CompoundFileWriter::addFile(const QString& file)
{
    if (merged)
        _CLTHROWA(CL_ERR_IO, "Can't add extensions after merge has been called");

    if (file.isEmpty())
        _CLTHROWA(CL_ERR_NullPointer, "file cannot be null");

    if (ids.find(file) != ids.end()) {
        char buf[CL_MAX_PATH + 30];
        strcpy(buf, "File ");
        strncat(buf, file.toLocal8Bit().constData(), CL_MAX_PATH);
        strcat(buf," already added");
        _CLTHROWA(CL_ERR_IO,buf);
    }
    ids.insert(file);
    entries.push_back(_CLNEW WriterFileEntry(file));
}

void CompoundFileWriter::close()
{
    if (merged)
        _CLTHROWA(CL_ERR_IO, "Merge already performed");

    if (entries.size() == 0) // isEmpty()
        _CLTHROWA(CL_ERR_IO, "No entries to merge have been defined");

    merged = true;

    // open the compound stream
    IndexOutput* os = NULL;
    try {
        os = directory->createOutput(fileName);

        // Write the number of entries
        os->writeVInt(entries.size());

        // Write the directory with all offsets at 0.
        // Remember the positions of directory entries so that we can
        // adjust the offsets later
        { //msvc6 for scope fix
            TCHAR tfile[CL_MAX_PATH];
            for (CLLinkedList<WriterFileEntry*>::iterator i = entries.begin();
                i != entries.end(); i++) {
                WriterFileEntry* fe = *i;
                fe->directoryOffset = os->getFilePointer();
                os->writeLong(0);    // for now
                tfile[fe->file.toWCharArray(tfile)] = '\0';
                os->writeString(tfile, _tcslen(tfile));
            }
        }

        // Open the files and copy their data into the stream.
        // Remember the locations of each file's data section.
        { //msvc6 for scope fix
            int32_t bufferLength = 1024;
            uint8_t buffer[1024];
            for (CLLinkedList<WriterFileEntry*>::iterator i = entries.begin();
                i != entries.end(); i++) {
                WriterFileEntry* fe = *i;
                fe->dataOffset = os->getFilePointer();
                copyFile(fe, os, buffer, bufferLength);
            }
        }

        { //msvc6 for scope fix
            // Write the data offsets into the directory of the compound stream
            for (CLLinkedList<WriterFileEntry*>::iterator i = entries.begin();
                i != entries.end(); i++) {
                WriterFileEntry* fe = *i;
                os->seek(fe->directoryOffset);
                os->writeLong(fe->dataOffset);
            }
        }


    } _CLFINALLY (
        if (os != NULL) {
            try {
                os->close();
                _CLDELETE(os);
            } catch (...) { }
        }
    );
}

void CompoundFileWriter::copyFile(WriterFileEntry* source, IndexOutput* os,
    uint8_t* buffer, int32_t bufferLength)
{
    IndexInput* is = NULL;
    try {
        int64_t startPtr = os->getFilePointer();

        is = directory->openInput(source->file);
        int64_t length = is->length();
        int64_t remainder = length;
        int32_t chunk = bufferLength;

        while(remainder > 0) {
            int32_t len = (int32_t)min((int64_t)chunk, remainder);
            is->readBytes(buffer, len);
            os->writeBytes(buffer, len);
            remainder -= len;
        }

        // Verify that remainder is 0
        if (remainder != 0) {
            TCHAR buf[CL_MAX_PATH+100];
            _sntprintf(buf, CL_MAX_PATH + 100, _T("Non-zero remainder length ")
                _T("after copying: %d (id: %s, length: %d, buffer size: %d)"),
                remainder, source->file.toLocal8Bit().constData(), length, chunk);
            _CLTHROWT(CL_ERR_IO, buf);
        }

        // Verify that the output length diff is equal to original file
        int64_t endPtr = os->getFilePointer();
        int64_t diff = endPtr - startPtr;
        if (diff != length) {
            TCHAR buf[100];
            _sntprintf(buf, 100, _T("Difference in the output file offsets %d ")
                _T("does not match the original file length %d"), diff, length);
            _CLTHROWT(CL_ERR_IO,buf);
        }
    } _CLFINALLY (
        if (is != NULL) {
            is->close();
            _CLDELETE(is);
        }
    );
}

CL_NS_END

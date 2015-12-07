/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright(C) 2007, 2008 by Nokia Corporation and/or its subsidiary(-ies), all rights reserved.
*/
#include "CLucene/StdHeader.h"
#include "FieldsReader.h"

#include "CLucene/util/VoidMap.h"
#include "CLucene/util/Misc.h"
#include "CLucene/store/Directory.h"
#include "CLucene/document/Document.h"
#include "CLucene/document/Field.h"
#include "FieldInfos.h"
#include "FieldsWriter.h"

CL_NS_USE(store)
CL_NS_USE(document)
CL_NS_USE(util)
CL_NS_DEF(index)

FieldsReader::FieldsReader(Directory* d, const QString& segment, FieldInfos* fn)
    : fieldInfos(fn)
{
    //Func - Constructor
    //Pre  - d contains a valid reference to a Directory
    //       segment != NULL
    //       fn contains a valid reference to a FieldInfos
    //Post - The instance has been created

    CND_PRECONDITION(!segment.isEmpty(), "segment != NULL");

    QString buf = Misc::segmentname(segment, QLatin1String(".fdt"));
    fieldsStream = d->openInput(buf);

    buf = Misc::segmentname(segment, QLatin1String(".fdx"));
    indexStream = d->openInput(buf);

    _size = (int32_t)indexStream->length() / 8;
}

FieldsReader::~FieldsReader()
{
    //Func - Destructor
    //Pre  - true
    //Post - The instance has been destroyed
    close();
}

void FieldsReader::close()
{
    //Func - Closes the FieldsReader
    //Pre  - true
    //Post - The FieldsReader has been closed
    if (fieldsStream) {
        fieldsStream->close();
        _CLDELETE(fieldsStream);
    }

    if(indexStream) {
        indexStream->close();
        _CLDELETE(indexStream);
    }
}

int32_t FieldsReader::size() const
{
    return _size;
}

bool FieldsReader::doc(int32_t n, Document* doc)
{
    if ( n * 8L > indexStream->length() )
        return false;

    indexStream->seek(n * 8L);
    int64_t position = indexStream->readLong();
    fieldsStream->seek(position);

    int32_t numFields = fieldsStream->readVInt();
    for (int32_t i = 0; i < numFields; i++) {
        int32_t fieldNumber = fieldsStream->readVInt();
        FieldInfo* fi = fieldInfos->fieldInfo(fieldNumber);

        if ( fi == NULL )
            _CLTHROWA(CL_ERR_IO, "Field stream is invalid");

        uint8_t bits = fieldsStream->readByte();
        if ((bits & FieldsWriter::FIELD_IS_BINARY) != 0) {
            int32_t fieldLen = fieldsStream->readVInt();
            FieldsReader::FieldsStreamHolder* subStream = new
                FieldsReader::FieldsStreamHolder(fieldsStream, fieldLen);
            uint8_t bits = Field::STORE_YES;
            Field* f = _CLNEW Field(
                fi->name,   // name
                subStream,  // read value
                bits);

            doc->add(*f);

            //now skip over the rest of the field
            if (fieldsStream->getFilePointer() + fieldLen
                == fieldsStream->length()) {
                // set to eof
                fieldsStream->seek(fieldsStream->getFilePointer() + fieldLen - 1);
                fieldsStream->readByte();
            } else {
                fieldsStream->seek(fieldsStream->getFilePointer() + fieldLen);
            }
        } else {
            uint8_t bits = Field::STORE_YES;
            if (fi->isIndexed && (bits & FieldsWriter::FIELD_IS_TOKENIZED)!=0 )
                bits |= Field::INDEX_TOKENIZED;
            else if (fi->isIndexed && (bits & FieldsWriter::FIELD_IS_TOKENIZED) == 0)
                bits |= Field::INDEX_UNTOKENIZED;
            else
                bits |= Field::INDEX_NO;

            if (fi->storeTermVector) {
                if (fi->storeOffsetWithTermVector) {
                    if (fi->storePositionWithTermVector) {
                        bits |= Field::TERMVECTOR_WITH_OFFSETS;
                        bits |= Field::TERMVECTOR_WITH_POSITIONS;
                    } else {
                        bits |= Field::TERMVECTOR_WITH_OFFSETS;
                    }
                } else if (fi->storePositionWithTermVector) {
                    bits |= Field::TERMVECTOR_WITH_POSITIONS;
                } else {
                    bits |= Field::TERMVECTOR_YES;
                }
            } else {
                bits |= Field::TERMVECTOR_NO;
            }

            if ( (bits & FieldsWriter::FIELD_IS_COMPRESSED) != 0 ) {
                bits |= Field::STORE_COMPRESS;
                int32_t fieldLen = fieldsStream->readVInt();
                FieldsStreamHolder* subStream = new 
                    FieldsStreamHolder(fieldsStream, fieldLen);

                // TODO: we dont have gzip inputstream available, must alert
                // user to somehow use a gzip inputstream
                Field* f = _CLNEW Field(
                    fi->name,   // name
                    subStream,  // read value
                    bits);

                f->setOmitNorms(fi->omitNorms);
                doc->add(*f);

                // now skip over the rest of the field
                if (fieldsStream->getFilePointer() + fieldLen
                    == fieldsStream->length()) {
                    //set to eof
                    fieldsStream->seek(fieldsStream->getFilePointer() + fieldLen - 1);
                    fieldsStream->readByte();
                } else {
                    fieldsStream->seek(fieldsStream->getFilePointer() + fieldLen);
                }
            } else {
                TCHAR* fvalue = fieldsStream->readString(true);
                Field* f = _CLNEW Field(
                    fi->name,   // name
                    fvalue,     // read value
                    bits);
                // TODO: could optimise this
                _CLDELETE_CARRAY(fvalue);
                f->setOmitNorms(fi->omitNorms);
                doc->add(*f);
            }
        }
    }
    return true;
}

FieldsReader::FieldsStreamHolder::FieldsStreamHolder(IndexInput* indexInput,
    int32_t subLength)
{
    this->indexInput = indexInput->clone();
    this->indexInputStream = new IndexInputStream(this->indexInput);
    this->subStream = new jstreams::SubInputStream<char>(indexInputStream,
        subLength);

    this->size = subStream->getSize();
    this->position = subStream->getPosition();
    this->error = subStream->getError();
    this->status = subStream->getStatus();
}

FieldsReader::FieldsStreamHolder::~FieldsStreamHolder()
{
    delete subStream;
    delete indexInputStream;

    indexInput->close();
    _CLDELETE(indexInput);
}

int32_t FieldsReader::FieldsStreamHolder::read(const char*& start, int32_t _min,
    int32_t _max)
{
    int32_t ret = subStream->read(start,_min,_max);
    this->position = subStream->getPosition();
    this->error = subStream->getError();
    this->status = subStream->getStatus();
    return ret;
}

int64_t FieldsReader::FieldsStreamHolder::skip(int64_t ntoskip)
{
    int64_t ret = subStream->skip(ntoskip);
    this->position = subStream->getPosition();
    this->error = subStream->getError();
    this->status = subStream->getStatus();
    return ret;
}

int64_t FieldsReader::FieldsStreamHolder::reset(int64_t pos)
{
    int64_t ret = subStream->reset(pos);
    this->position = subStream->getPosition();
    this->error = subStream->getError();
    this->status = subStream->getStatus();
    return ret;
}

CL_NS_END

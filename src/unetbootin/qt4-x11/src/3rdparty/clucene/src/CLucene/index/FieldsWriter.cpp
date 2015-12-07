/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright(C) 2007, 2008 by Nokia Corporation and/or its subsidiary(-ies), all rights reserved.
*/
#include "CLucene/StdHeader.h"
#include "FieldsWriter.h"

#include "CLucene/util/VoidMap.h"
#include "CLucene/util/Reader.h"
#include "CLucene/util/Misc.h"
#include "CLucene/store/Directory.h"
#include "CLucene/store/IndexOutput.h"
#include "CLucene/document/Document.h"
#include "CLucene/document/Field.h"
#include "FieldInfos.h"

CL_NS_USE(store)
CL_NS_USE(util)
CL_NS_USE(document)
CL_NS_DEF(index)
	
FieldsWriter::FieldsWriter(Directory* d, const QString& segment, FieldInfos* fn)
    : fieldInfos(fn)
{
    //Func - Constructor
    //Pre  - d contains a valid reference to a directory
    //       segment != NULL and contains the name of the segment
    //Post - fn contains a valid reference toa a FieldInfos

	CND_PRECONDITION(!segment.isEmpty(), "segment is NULL");

	QString buf = Misc::segmentname(segment, QLatin1String(".fdt"));
    fieldsStream = d->createOutput(buf);
    
	buf = Misc::segmentname(segment, QLatin1String(".fdx"));
    indexStream = d->createOutput(buf);

	CND_CONDITION(indexStream != NULL, "indexStream is NULL");
}

FieldsWriter::~FieldsWriter()
{
    //Func - Destructor
    //Pre  - true
    //Post - Instance has been destroyed

	close();
}

void FieldsWriter::close()
{
    //Func - Closes all streams and frees all resources
    //Pre  - true
    //Post - All streams have been closed all resources have been freed

    //Check if fieldsStream is valid
    if (fieldsStream) {
        //Close fieldsStream
        fieldsStream->close();
        _CLDELETE(fieldsStream);
    }

    //Check if indexStream is valid
    if (indexStream) {
        //Close indexStream
        indexStream->close();
        _CLDELETE(indexStream);
    }
}

void FieldsWriter::addDocument(Document* doc)
{
    //Func - Adds a document
    //Pre  - doc contains a valid reference to a Document
    //       indexStream != NULL
    //       fieldsStream != NULL
    //Post - The document doc has been added

    CND_PRECONDITION(indexStream != NULL, "indexStream is NULL");
    CND_PRECONDITION(fieldsStream != NULL, "fieldsStream is NULL");

    indexStream->writeLong(fieldsStream->getFilePointer());

    int32_t storedCount = 0;
    DocumentFieldEnumeration* fields = doc->fields();
    while (fields->hasMoreElements()) {
        Field* field = fields->nextElement();
        if (field->isStored())
            storedCount++;
    }
    _CLDELETE(fields);
    fieldsStream->writeVInt(storedCount);

    fields = doc->fields();
    while (fields->hasMoreElements()) {
        Field* field = fields->nextElement();
        if (field->isStored()) {
            fieldsStream->writeVInt(fieldInfos->fieldNumber(field->name()));

            uint8_t bits = 0;
            if (field->isTokenized())
                bits |= FieldsWriter::FIELD_IS_TOKENIZED;
            if (field->isBinary())
                bits |= FieldsWriter::FIELD_IS_BINARY;
            if (field->isCompressed())
                bits |= FieldsWriter::FIELD_IS_COMPRESSED;

            fieldsStream->writeByte(bits);

            if ( field->isCompressed()) {
                _CLTHROWA(CL_ERR_Runtime,
                    "CLucene does not directly support compressed fields. "
                    "Write a compressed byte array instead");
            } else {
                // FEATURE: this problem in Java Lucene too, if using Reader,
                // data is not stored.
                //
                // TODO: this is a logic bug...
                // if the field is stored, and indexed, and is using a reader
                // the field wont get indexed
                //
                // if we could write zero prefixed vints (therefore static
                // length), then we could write a reader directly to the field
                // indexoutput and then go back and write the data length.
                // however this is not supported in lucene yet...
                // if this is ever implemented, then it would make sense to
                // also be able to combine the FieldsWriter and
                // DocumentWriter::invertDocument process, and use a
                // streamfilter to write the field data while the documentwrite
                // analyses the document! how cool would that be! it would cut
                // out all these buffers!!!

                // compression is disabled for the current field
                if (field->isBinary()) {
                    // TODO: since we currently don't support static length vints,
                    // we have to read the entire stream into memory first.... ugly!
                    jstreams::StreamBase<char>* stream = field->streamValue();
                    const char* sd;
                    // how do we make sure we read the entire index in now???
                    // TODO: we need to have a max amount, and guarantee its all
                    // in or throw an error...
                    int32_t rl = stream->read(sd,10000000,0);

                    if ( rl < 0 ) {
                        // TODO: could we detect this earlier and not actually
                        // write the field??
                        fieldsStream->writeVInt(0);
                    } else {
                        // TODO: if this int could be written with a constant
                        // length, then the stream could be read and written a
                        // bit at a time then the length is re-written at the end.
                        fieldsStream->writeVInt(rl);
                        fieldsStream->writeBytes((uint8_t*)sd, rl);
                    }
                } else if (field->stringValue() == NULL ) {
                    // we must be using readerValue
                    CND_PRECONDITION(!field->isIndexed(),
                        "Cannot store reader if it is indexed too")
                    Reader* r = field->readerValue();

                    //read the entire string
                    const TCHAR* rv;
                    int64_t rl = r->read(rv, LUCENE_INT32_MAX_SHOULDBE);
                    if ( rl > LUCENE_INT32_MAX_SHOULDBE )
                        _CLTHROWA(CL_ERR_Runtime, "Field length too long");
                    else if ( rl < 0 )
                        rl = 0;

                    fieldsStream->writeString( rv, (int32_t)rl);
                } else if (field->stringValue() != NULL ) {
                    fieldsStream->writeString(field->stringValue(),
                        _tcslen(field->stringValue()));
                } else {
                    _CLTHROWA(CL_ERR_Runtime, "No values are set for the field");
                }
            }
        }
    }
    _CLDELETE(fields);
}

CL_NS_END

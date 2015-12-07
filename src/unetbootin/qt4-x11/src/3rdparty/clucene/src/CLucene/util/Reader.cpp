/*------------------------------------------------------------------------------
* Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
* 
* Distributable under the terms of either the Apache License (Version 2.0) or 
* the GNU Lesser General Public License, as specified in the COPYING file.
------------------------------------------------------------------------------*/
#include "CLucene/StdHeader.h"
#include "Reader.h"

CL_NS_DEF(util)

StringReader::StringReader ( const TCHAR* value ):
  Reader(NULL,true){
	  reader = new jstreams::StringReader<TCHAR>(value);
}
StringReader::StringReader ( const TCHAR* value, const int32_t length ):
  Reader(NULL,true){
	  reader = new jstreams::StringReader<TCHAR>(value,length);
}
StringReader::StringReader ( const TCHAR* value, const int32_t length, bool copyData ):
  Reader(NULL,true){
	  reader = new jstreams::StringReader<TCHAR>(value,length, copyData);
}
StringReader::~StringReader(){
}


FileReader::FileReader ( const char* path, const char* enc,
						 const int32_t cachelen, const int32_t /*cachebuff*/ ):
	Reader(NULL, true)
{
	this->input = new jstreams::FileInputStream(path, cachelen);
	this->reader = new SimpleInputStreamReader(this->input,enc); //(this is a jstream object)
}

FileReader::~FileReader (){
    if (input) 
		delete input;
}
int32_t FileReader::read(const TCHAR*& start, int32_t _min, int32_t _max) {
    return reader->read(start, _min, _max);
}
int64_t FileReader::mark(int32_t readlimit) {
    return reader->mark(readlimit);
}
int64_t FileReader::reset(int64_t newpos) {
    return reader->reset(newpos);
}



SimpleInputStreamReader::SimpleInputStreamReader(jstreams::StreamBase<char> *i, const char* enc)
{
    finishedDecoding = false;
    input = i;
    charbuf.setSize(262);

	if ( strcmp(enc,"ASCII")==0 )
		encoding = ASCII;
#ifdef _UCS2
	else if ( strcmp(enc,"UTF-8")==0 )
		encoding = UTF8;
	else if ( strcmp(enc,"UCS-2LE")==0 )
		encoding = UCS2_LE;
#endif
	else
		_CLTHROWA(CL_ERR_IllegalArgument,"Unsupported encoding, use jstreams iconv based instead");

	mark(262);
    charsLeft = 0;
}
SimpleInputStreamReader::~SimpleInputStreamReader(){
	input = NULL;
}
int32_t SimpleInputStreamReader::decode(TCHAR* start, int32_t space){
	// decode from charbuf
    const char *inbuf = charbuf.readPos;
    const char *inbufend = charbuf.readPos + charbuf.avail;
    TCHAR *outbuf = start;
	const TCHAR *outbufend = outbuf + space;

	if ( encoding == ASCII ){
		while ( outbuf<outbufend && inbuf<inbufend ){
			*outbuf = *inbuf;
			outbuf++;
			inbuf++;
		}
		
#ifdef _UCS2
	}
	else if ( encoding == UCS2_LE ){
		while ( outbuf<outbufend && (inbuf+1)<inbufend ){
			uint8_t c1 = *inbuf;
			uint8_t c2 = *(inbuf+1);
			unsigned short c = c1 | (c2<<8);
			
			#ifdef _UCS2
				*outbuf = c;
			#else
				*outbuf = LUCENE_OOR_CHAR(c);
			#endif
			outbuf++;
			inbuf+=2;
		}

	}else if ( encoding == UTF8 ){
		while ( outbuf<outbufend && inbuf<inbufend ){
			size_t utflen = lucene_utf8charlen(inbuf);
			if ( utflen==0 ){
				error = "Invalid multibyte sequence.";
				status = jstreams::Error;
				return -1;
			}else if ( inbuf+utflen > inbufend ){
				break; //character incomplete
			}else{
				size_t rd = lucene_utf8towc(outbuf,inbuf,inbufend-inbuf);
				if ( rd == 0 ){
					error = "Invalid multibyte sequence.";
					status = jstreams::Error;
					return -1;
				}else{
					inbuf+=rd;
					outbuf++;
				}
			}
		}
#endif //_UCS2
	}else
		_CLTHROWA(CL_ERR_Runtime,"Unexpected encoding");

	if ( outbuf < outbufend ) { 
		//we had enough room to convert the entire input
		if ( inbuf < inbufend ) {
			// last character is incomplete
			// move from inbuf to the end to the start of
			// the buffer
			memmove(charbuf.start, inbuf, inbufend-inbuf);
			charbuf.readPos = charbuf.start;
			charbuf.avail = inbufend-inbuf;
		} else if ( outbuf < outbufend ) { //input sequence was completely converted
			charbuf.readPos = charbuf.start;
			charbuf.avail = 0;
			if (input == NULL) {
				finishedDecoding = true;
			}
		}
	} else {
        charbuf.readPos += charbuf.avail - (inbufend-inbuf);
        charbuf.avail = inbufend-inbuf;
	}
    return outbuf-start;
}

int32_t SimpleInputStreamReader::fillBuffer(TCHAR* start, int32_t space) {
    // fill up charbuf
    if (input && charbuf.readPos == charbuf.start) {
        const char *begin;
        int32_t numRead;
        numRead = input->read(begin, 1, charbuf.size - charbuf.avail);
        //printf("filled up charbuf\n");
        if (numRead < -1) {
            error = input->getError();
            status = jstreams::Error;
            input = 0;
            return numRead;
        }
        if (numRead < 1) {
            // signal end of input buffer
            input = 0;
            if (charbuf.avail) {
                error = "stream ends on incomplete character";
                status = jstreams::Error;
            }
            return -1;
        }
        // copy data into other buffer
        memmove( charbuf.start + charbuf.avail, begin, numRead * sizeof(char));
        charbuf.avail = numRead + charbuf.avail;
    }
    // decode
    int32_t n = decode(start, space);
    //printf("decoded %i\n", n);
    return n;
}

CL_NS_END

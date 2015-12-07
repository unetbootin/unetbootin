/*
 * Copyright (C) 2003-2006 Ben van Klinken and the CLucene Team
 *
 * Distributable under the terms of either the Apache License (Version 2.0) or 
 * the GNU Lesser General Public License, as specified in the COPYING file.
 *
 * Changes are Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
*/
#include "CLucene/StdHeader.h"
#include "SegmentInfos.h"

#include "CLucene/store/Directory.h"
#include "CLucene/util/Misc.h"

CL_NS_USE(store)
CL_NS_USE(util)
CL_NS_DEF(index)

SegmentInfo::SegmentInfo(const QString& Name, const int32_t DocCount,
    CL_NS(store)::Directory* Dir)
    : docCount(DocCount)
    , dir(Dir)
{
    //Func - Constructor. Initialises SegmentInfo.
    //Pre  - Name holds the unique name in the directory Dir
    //       DocCount holds the number of documents in the segment
    //       Dir holds the Directory where the segment resides
    //Post - The instance has been created. name contains the duplicated string
    //        Name. docCount = DocCount and dir references Dir
    name = Name;
}

SegmentInfo::~SegmentInfo()
{
}

SegmentInfos::SegmentInfos(bool _deleteMembers)
    : deleteMembers(_deleteMembers)
{
    //Func - Constructor
    //Pre  - deleteMembers indicates if the instance to be created must delete
    //       all SegmentInfo instances it manages when the instance is destroyed
    //       or not true -> must delete, false may not delete
    //Post - An instance of SegmentInfos has been created.

    //initialize counter to 0
    counter = 0;
    version = Misc::currentTimeMillis();
}

SegmentInfos::~SegmentInfos()
{
    //Func - Destructor
    //Pre  - true
    //Post - The instance has been destroyed. Depending on the constructor used
    //       the SegmentInfo instances that this instance managed have been
    //       deleted or not.

    if (deleteMembers) {
        segmentInfosType::iterator it;
        for (it = infos.begin(); it != infos.end(); ++it)
            _CLLDELETE(*it);
    }
    //Clear the list of SegmentInfo instances - make sure everything is deleted
    infos.clear();
}
  
SegmentInfo* SegmentInfos::info(int32_t i) const
{
    //Func - Returns a reference to the i-th SegmentInfo in the list.
    //Pre  - i >= 0
    //Post - A reference to the i-th SegmentInfo instance has been returned

    CND_PRECONDITION(i >= 0, "i contains negative number");

    //Get the i-th SegmentInfo instance
    SegmentInfo *ret = infos.value(i, 0);

    //Condition check to see if the i-th SegmentInfo has been retrieved
    CND_CONDITION(ret != NULL, "No SegmentInfo instance found");

    return ret;
}

void SegmentInfos::clearto(size_t _min)
{
    // Make sure we actually need to remove
    if (infos.size() > _min) {
        segmentInfosType::iterator itr;
        segmentInfosType::iterator eitr = infos.end();
        segmentInfosType::iterator bitr = infos.begin() + _min;

        for(itr = bitr; itr != eitr; ++itr)
            _CLLDELETE((*itr));
        infos.erase(bitr, eitr);
    }
}

void SegmentInfos::add(SegmentInfo* info)
{
    infos.push_back(info);
}

int32_t SegmentInfos::size() const
{
    return infos.size();
}

void SegmentInfos::read(Directory* directory)
{
    //Func - Reads segments file that resides in directory. 
    //Pre  - directory contains a valid reference
    //Post - The segments file has been read and for each segment found
    //       a SegmentsInfo intance has been created and stored.

    //Open an IndexInput to the segments file and check if valid
    IndexInput* input = directory->openInput(QLatin1String("segments"));
    if (input) {
        try {
            int32_t format = input->readInt();
            // file contains explicit format info
            if (format < 0) {
                // check that it is a format we can understand
                if (format < FORMAT) {
                    TCHAR err[30];
                    _sntprintf(err, 30, _T("Unknown format version: %d"), format);
                    _CLTHROWT(CL_ERR_Runtime, err);
                }
                // read version
                version = input->readLong();
                // read counter
                counter = input->readInt();
            } else {
                // file is in old format without explicit format info
                counter = format;
            }

            //Temporary variable for storing the name of the segment
            char aname[CL_MAX_PATH] = { 0 };
            TCHAR tname[CL_MAX_PATH] = { 0 };

            //read segmentInfos
            for (int32_t i = input->readInt(); i > 0; --i) { 
                // read the name of the segment
                input->readString(tname, CL_MAX_PATH); 
                STRCPY_TtoA(aname, tname, CL_MAX_PATH);

                //Instantiate a new SegmentInfo Instance
                SegmentInfo* si = _CLNEW SegmentInfo(QLatin1String(aname),
                    input->readInt(), directory);

                //Condition check to see if si points to an instance
                CND_CONDITION(si != NULL, "Memory allocation for si failed")	;

                //store SegmentInfo si
                infos.push_back(si);
            } 

            if (format >= 0) {
                // in old format the version number may be at the end of the file
                if (input->getFilePointer() >= input->length()) {
                    // old file format without version number
                    version = Misc::currentTimeMillis();
                } else {
                    // read version
                    version = input->readLong();
                }
            }
        } _CLFINALLY (
            //destroy the inputStream input. The destructor of IndexInput will 
            //also close the Inputstream input
            _CLDELETE(input);
        );
    }
}

void SegmentInfos::write(Directory* directory)
{
    //Func - Writes a new segments file based upon the SegmentInfo instances it manages
    //Pre  - directory is a valid reference to a Directory
    //Post - The new segment has been written to disk

    //Open an IndexOutput to the segments file and check if valid
    IndexOutput* output = directory->createOutput(QLatin1String("segments.new"));
    if (output) {
        try {
            // write FORMAT
            output->writeInt(FORMAT); 
            // every write changes the index
            output->writeLong(++version);
             // Write the counter
            output->writeInt(counter);

            // Write the number of SegmentInfo Instances which is equal to the number
            // of segments in directory as each SegmentInfo manages a single segment
            output->writeInt(infos.size());			  

            //temporary value for wide segment name
            TCHAR tname[CL_MAX_PATH];

            //Iterate through all the SegmentInfo instances
            for (uint32_t i = 0; i < infos.size(); ++i) {
                //Retrieve the SegmentInfo
                SegmentInfo *si = infos.value(i, 0);
                //Condition check to see if si has been retrieved
                CND_CONDITION(si != NULL, "No SegmentInfo instance found");

                //Write the name of the current segment
                int32_t count = si->name.toWCharArray(tname);
                tname[count] = '\0';
                output->writeString(tname, _tcslen(tname));

                //Write the number of documents in the segment 
                output->writeInt(si->docCount);
            }
        } _CLFINALLY(
            output->close();
            _CLDELETE(output);
        );

        // install new segment info
        directory->renameFile(QLatin1String("segments.new"),
            QLatin1String("segments"));
    }
}

  
int64_t SegmentInfos::readCurrentVersion(Directory* directory)
{
    int32_t format = 0;
    int64_t version = 0;
    IndexInput* input = directory->openInput(QLatin1String("segments"));
    try {
        format = input->readInt();
        if (format < 0){
            if (format < FORMAT) {
                TCHAR err[30];
                _sntprintf(err, 30, _T("Unknown format version: %d"), format);
                _CLTHROWT(CL_ERR_Runtime, err);
            }
            // read version
            version = input->readLong();
        }
    } _CLFINALLY (
        input->close(); 
        _CLDELETE(input);
    );

    if (format < 0)
        return version;

    // We cannot be sure about the format of the file. Therefore we have to
    // read the whole file and cannot simply seek to the version entry.
    SegmentInfos segmentInfos;
    segmentInfos.read(directory);
    return segmentInfos.getVersion();
}

CL_NS_END

/*********************************************
	 Utilities 

	
*********************************************/



#include "UbUtilities.h"
#include <QFile>
 #include <QDir>
#include <QtGlobal>
#include <QtDebug>
#include <QDateTime>
#include <QSysInfo>
#include <QLibraryInfo>

// Initialization for static members:
bool UbUtilities::isInitialized = false;
UbUtilities* UbUtilities::_instance = 0;
 
using namespace std;


//CB>-------------------------------------------------------------------
// 
//  DESCRIPTION:  initClass
//                    
//  PRECONDITIONS: 
//                 
//  PARAMETER:
//      IN:     
//      OUT:    
//
//  RETURN:           

//  SideFX:     
//
//<CE
int UbUtilities::initClass (void) 
{ 
    if (isInitialized)
        return 1;

    isInitialized = true;
    _instance = new UbUtilities();
    //	AssertExp (_instance);

    //  	_instance->setLoggingIsActive (false);
    _instance->defaultFileName = QDir::home().filePath("unetbootinLog.txt").toStdString(); // "/Users/stefan2/unetbootinLog.txt";

    //  	_instance->defaultFlags = _instance->logStream.flags();
    std::ostream freshStream(0);

    _instance->defaultFlags = freshStream.flags();
    _instance->defaultFlags |= ios_base::dec;

    _instance->openFileIfNecessary();
    //	setLogMode (UbUtilities::noLogMode);

    // Qt 5:   qSetMessagePattern("%{file}(%{line}): %{message}");
    qInstallMsgHandler( writeLogMsgToFile );

    qDebug() << "qDebug: UbUtilities::initClass (void)  called";
    qWarning() << "qWarning: UbUtilities::initClass (void)  called";
    logSystemInfo();
    return 0;
}

void UbUtilities::writeLogMsgToFile (QtMsgType type, const char *msg)
{
    // in Qt 5 we could use const QMessageLogContext &context for file, line and time information
    QString timeStr (QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:zzz"));

  //    std::cerr << timeStr.toLocal8Bit().constData() << " - "

    switch (type) {
    case QtDebugMsg:
    case QtWarningMsg:
    case QtCriticalMsg:
    case QtFatalMsg:
        // fprintf(stderr, "Debug: %s\n", msg);
   //   _instance->logStream.write( , strlen(msg) );
          _instance->logStream << timeStr.toLocal8Bit().constData()<< " "  << msg << std::endl << std::flush;
    //    _instance->logStream.flush();
        break;
        //        fprintf(stderr, "Warning: %s\n", msg);
        //        break;
        //        fprintf(stderr, "Critical: %s\n", msg);
        //        break;
        //        fprintf(stderr, "Fatal: %s\n", msg);
        //        abort();
    }
}

//CB>-------------------------------------------------------------------
// 
//  DESCRIPTION:  shutdownClass
//                    
//  PRECONDITIONS: 
//                 
//  PARAMETER:
//      IN:     
//      OUT:    
//
//  RETURN:           

//  SideFX:     
//
//<CE
void UbUtilities::shutdownClass() 
{ 
		if (_instance) {
			delete _instance;
			_instance = 0;
		}
		isInitialized = false;
}

//CB>-------------------------------------------------------------------
// 
//  DESCRIPTION:  openFileIfNecessary
//                    
//<CE
void UbUtilities::openFileIfNecessary (void) 
{
	if (_instance->ourStreamBuf.is_open())
		return;

	// determine a valid file name
	string fileName;
	if (_instance->validFileName.empty() )
		fileName = _instance->defaultFileName;
	else
		fileName = _instance->validFileName;
	
	// check if file is writeable	
	determinePossibleFileName (fileName);
	// AssertExp (!_instance->validFileName.empty());
	_instance->validFileName = fileName;
	
	_instance->ourStreamBuf.open (_instance->validFileName.c_str(), ios::out | ios::app  ); // | ios::app
//	AssertExp (_instance->ourStreamBuf.is_open());
	if (!_instance->ourStreamBuf.is_open()) {
	  // stw 2005-07-04 UbUtilities::setLoggingIsActive (false); // something went wrong, dont try to write
	}
    _instance->logStream.rdbuf (&_instance->ourStreamBuf); // link filebuf to ostream
	return ;
}

//CB>-------------------------------------------------------------------
// 
//  DESCRIPTION:  
//                    
//<CE
//void UbUtilities::setLoggingIsActive (bool value)
//{
//	if (!isInitialized)
//		UbUtilities::initClass();
//	if (value == true && _instance->loggingIsActive == false) {
//		// init
////		UbUtilities::setLogMode (UbUtilities::logFileMode);
//	}
//	_instance->loggingIsActive = value;
//}

//CB>-------------------------------------------------------------------
// 
//  DESCRIPTION:  
//                    
//<CE
bool UbUtilities::getLoggingIsActive (void)  
{ 
	if (!isInitialized)
		UbUtilities::initClass();
	return UbUtilities::_instance->loggingIsActive; 
}

//CB>-------------------------------------------------------------------
// 
//  DESCRIPTION:  
//                    
//<CE
UbUtilities* UbUtilities::getInstance (void) 
{ 
	if (!isInitialized)
		UbUtilities::initClass();
	return _instance; 
}

//CB>-------------------------------------------------------------------
// 
//  DESCRIPTION:  
//                    
//<CE
void UbUtilities::setDefaultFileName (string& value) 
{ 
	if (!isInitialized)
		UbUtilities::initClass();
		
	_instance->defaultFileName = value; 
}

//CB>-------------------------------------------------------------------
// 
//  DESCRIPTION:  determinePossibleFileName
//                    
//<CE
string& UbUtilities::determinePossibleFileName (string& fileName ) 
{
	string testFileName, savedFileName;
	testFileName = fileName;
	savedFileName = fileName;
	// create a new name if open failed by appending a number.
	 int aNumber = 1;
	 bool success = false;
	//	 string newFileName;
	FILE* testFile = 0;
	// try if file exists and is writeable
	testFile = fopen (testFileName.c_str(), "r+"); // was "w" which caused truncation of file
	
	if (testFile != 0) {  
	// file exists and is writeable
		fclose (testFile);
		return fileName;
	}
//	else {
	// try if a new file with this name can be created
	testFile = fopen (testFileName.c_str(), "w"); 
	if (testFile != 0) {  
	// file has beed created (with 0 bytes)
		fclose (testFile);
		return fileName;
	}
	// find a new filename by appending a number at the end of the file name	
	do {
		ostringstream aStream;
	 	string numberString;
	 	aStream << aNumber << ends;
	 	numberString = aStream.str();
	 	testFileName = savedFileName + numberString;
		testFile = fopen (testFileName.c_str(), "w");
	 	success = (testFile != 0);
	 	if (success) {
			fclose (testFile);
			fileName = testFileName;
		}
	 	aNumber ++;
	 	if (aNumber > 200)
	 		break;
	 } while (!success);
	 
	return fileName;
}

//CB>-------------------------------------------------------------------
//
//  DESCRIPTION:  write Hardware and software infos to log
//
//  PRECONDITIONS:
//
//  PARAMETER:
//      IN:
//      OUT:
//
//  RETURN:

//  SideFX:
//
//<CE
void UbUtilities::logSystemInfo(void)
{
//        QSysInfo sysInfo;
//        qWarning() << sysInfo.b ;
//        QLibraryInfo libInfo;
//        qWarning() << libInfo. ;
    qWarning() << "Operating system name: " << getOsName() ;
}

QString UbUtilities::getOsName()
{
#if defined(Q_OS_ANDROID)
    return QLatin1String("android");
#elif defined(Q_OS_BLACKBERRY)
    return QLatin1String("blackberry");
#elif defined(Q_OS_IOS)
    return QLatin1String("ios");
#elif defined(Q_OS_MACOS)
    return QLatin1String("macos");
#elif defined(Q_OS_TVOS)
    return QLatin1String("tvos");
#elif defined(Q_OS_WATCHOS)
    return QLatin1String("watchos");
#elif defined(Q_OS_WINCE)
    return QLatin1String("wince");
#elif defined(Q_OS_WIN)
    return QLatin1String("windows");
#elif defined(Q_OS_LINUX)
    return QLatin1String("linux");
#elif defined(Q_OS_UNIX)
    return QLatin1String("unix");
#else
    return QLatin1String("unknown");
#endif
}

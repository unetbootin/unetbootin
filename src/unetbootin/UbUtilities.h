/*********************************************
	 Utilities 

	
*********************************************/
// to switch off Logging call ::setLoggingIsActive (false);

#ifndef UBUTILITIES_H
#define UBUTILITIES_H

#include <stdio.h>
#include <stdlib.h>
#ifdef __cplusplus  // make it compatible for C-Files

#include <iostream>
#include <ios>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <qapplication.h> // for qt logging

using namespace std;

class UbUtilities 
{
public:
  
private:
	static UbUtilities* _instance;
	static bool isInitialized;
	std::ostream logStream;
	std::string validFileName;
	bool loggingIsActive;
	std::string defaultFileName;
	std::ios_base::fmtflags defaultFlags;
	std::string dateTimeString;
//	LogMode logMode;
	std::filebuf ourStreamBuf;
#ifdef SYSLOG_AVAILABLE
	SysLogBuf ourSysLogBuf;
#endif

public:

	UbUtilities (void) : logStream (0) {
	  loggingIsActive = true;
//	  logMode = noLogMode;
	};

	~UbUtilities (void) { ourStreamBuf.close(); };

	static int initClass (void) ;

	static void shutdownClass() ;
	
	static std::ostream& getLogStream(void);

	static std::string& getDateTimeString(void) ;
	
	static void setLoggingIsActive (bool value);
	
	static bool getLoggingIsActive (void) ;

	static void openFileIfNecessary();
	
//	static void closeFileIfNecessary();
	
  // static void writeLog (void);
	
	static void setDefaultFileName (std::string& value);
	
//	static void setLogMode (LogMode logModeValue);
	
	static std::string& determinePossibleFileName (std::string& fileName ) ;
	static std::ios_base::fmtflags getDefaultFlags (void) {return _instance->defaultFlags; }
  static UbUtilities* getInstance (void) ;
  static void writeLogMsgToFile (QtMsgType type, const char *msg);


};

#endif // #ifdef __cplusplus

#endif //#define UBUTILITIES_Hm

TARGET	 = qsqltds

HEADERS		= ../../../sql/drivers/tds/qsql_tds.h

SOURCES		= main.cpp \
		  ../../../sql/drivers/tds/qsql_tds.cpp

unix:!contains( LIBS, .*sybdb.* ):LIBS 	*= -lsybdb

win32 {
    !win32-borland:LIBS *= -lNTWDBLIB
    win32-borland:LIBS 	*= $(BCB)/lib/PSDK/NTWDBLIB.LIB
}

include(../qsqldriverbase.pri)

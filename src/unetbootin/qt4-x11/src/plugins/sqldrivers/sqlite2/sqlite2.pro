TARGET	 = qsqlite2

HEADERS		= ../../../sql/drivers/sqlite2/qsql_sqlite2.h
SOURCES		= smain.cpp \
		  ../../../sql/drivers/sqlite2/qsql_sqlite2.cpp

!contains(LIBS, .*sqlite.*):LIBS *= -lsqlite

include(../qsqldriverbase.pri)

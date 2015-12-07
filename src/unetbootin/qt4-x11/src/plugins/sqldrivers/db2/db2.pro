TARGET	 = qsqldb2

HEADERS		= ../../../sql/drivers/db2/qsql_db2.h
SOURCES		= main.cpp \
		  ../../../sql/drivers/db2/qsql_db2.cpp

unix:!contains( LIBS, .*db2.* ):LIBS 	*= -ldb2
win32:!contains( LIBS, .*db2.* ):LIBS   *= -ldb2cli

include(../qsqldriverbase.pri)

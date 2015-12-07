TARGET	 = qsqlibase

HEADERS		= ../../../sql/drivers/ibase/qsql_ibase.h
SOURCES		= main.cpp \
		  ../../../sql/drivers/ibase/qsql_ibase.cpp

unix:!contains( LIBS, .*gds.* ):!contains( LIBS, .*libfb.* ):LIBS    *= -lgds

win32:!contains( LIBS, .*gds.* ):!contains( LIBS, .*fbclient.* ) {
	!win32-borland:LIBS *= -lgds32_ms
	win32-borland:LIBS  += gds32.lib
}

include(../qsqldriverbase.pri)

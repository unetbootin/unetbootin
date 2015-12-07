TARGET	 = qsqloci

HEADERS		= ../../../sql/drivers/oci/qsql_oci.h
SOURCES		= main.cpp \
		  ../../../sql/drivers/oci/qsql_oci.cpp

win32:LIBS	*= -loci

unix:!contains( LIBS, .*clnts.* ):LIBS	*= -lclntsh

macx:QMAKE_LFLAGS += -Wl,-flat_namespace,-U,_environ

include(../qsqldriverbase.pri)

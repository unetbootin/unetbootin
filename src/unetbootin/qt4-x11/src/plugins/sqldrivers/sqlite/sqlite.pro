TARGET	 = qsqlite

HEADERS		= ../../../sql/drivers/sqlite/qsql_sqlite.h
SOURCES		= smain.cpp \
		  ../../../sql/drivers/sqlite/qsql_sqlite.cpp

!system-sqlite:!contains( LIBS, .*sqlite.* ) {
    CONFIG(release, debug|release):DEFINES *= NDEBUG
    DEFINES += SQLITE_OMIT_LOAD_EXTENSION SQLITE_OMIT_COMPLETE 
    INCLUDEPATH += ../../../3rdparty/sqlite
    SOURCES += ../../../3rdparty/sqlite/sqlite3.c
} else {
    LIBS *= $$QT_LFLAGS_SQLITE
    QMAKE_CXXFLAGS *= $$QT_CFLAGS_SQLITE
}

include(../qsqldriverbase.pri)

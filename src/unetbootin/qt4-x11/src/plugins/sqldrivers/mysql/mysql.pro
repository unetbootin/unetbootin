TARGET	 = qsqlmysql

HEADERS		= ../../../sql/drivers/mysql/qsql_mysql.h
SOURCES		= main.cpp \
		  ../../../sql/drivers/mysql/qsql_mysql.cpp

unix: {
    isEmpty(QT_LFLAGS_MYSQL) {
        !contains(LIBS, .*mysqlclient.*):!contains(LIBS, .*mysqld.*) {
            use_libmysqlclient_r:LIBS *= -lmysqlclient_r
            else:LIBS *= -lmysqlclient
        }
    } else {
        LIBS *= $$QT_LFLAGS_MYSQL
        QMAKE_CXXFLAGS *= $$QT_CFLAGS_MYSQL
    }
}

win32:!contains(LIBS, .*mysql.*):!contains(LIBS, .*mysqld.*) {
    LIBS     *= -llibmysql    
}

include(../qsqldriverbase.pri)

contains(sql-drivers, all ) {
    sql-driver +=   psql mysql odbc oci tds db2 sqlite ibase
}

contains(sql-drivers, psql) {
    HEADERS +=      drivers/psql/qsql_psql.h
    SOURCES +=      drivers/psql/qsql_psql.cpp

    unix {
        !isEmpty(QT_LFLAGS_PSQL) {
            LIBS *= $$QT_LFLAGS_PSQL
            QMAKE_CXXFLAGS *= $$QT_CFLAGS_PSQL
        }
        !contains(LIBS, .*pq.*):LIBS *= -lpq
    }

    win32 {
	!win32-g++:!contains( LIBS, .*pq.* ):LIBS *= -llibpq
    	win32-g++:!contains( LIBS, .*pq.* ):LIBS *= -lpq
        LIBS *= -lws2_32 -ladvapi32
    }
}

contains(sql-drivers, mysql) {
    HEADERS +=      drivers/mysql/qsql_mysql.h
    SOURCES +=      drivers/mysql/qsql_mysql.cpp

    unix {
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
        !win32-g++:LIBS     *= -llibmysql    
	win32-g++:LIBS	    *= -lmysql
    }    
}

contains(sql-drivers, odbc) {
     HEADERS += drivers/odbc/qsql_odbc.h
     SOURCES += drivers/odbc/qsql_odbc.cpp

     mac:!contains( LIBS, .*odbc.* ):LIBS        *= -liodbc
     unix:!contains( LIBS, .*odbc.* ):LIBS       *= -lodbc

     win32 {
         !win32-borland:LIBS     *= -lodbc32
         win32-borland:LIBS      *= $(BCB)/lib/PSDK/odbc32.lib
     }
}

contains(sql-drivers, oci) {
    HEADERS += drivers/oci/qsql_oci.h
    SOURCES += drivers/oci/qsql_oci.cpp

    unix:!contains( LIBS, .*clnts.* ):LIBS += -lclntsh

    win32:LIBS += -loci
}

contains(sql-drivers, tds) {
    HEADERS += drivers/tds/qsql_tds.h
    SOURCES += drivers/tds/qsql_tds.cpp

    unix:LIBS += -L$SYBASE/lib -lsybdb

    win32 {
        !win32-borland:LIBS += -lNTWDBLIB
        win32-borland:LIBS += $(BCB)/lib/PSDK/NTWDBLIB.LIB
    }
}

contains(sql-drivers, db2) {
    HEADERS += drivers/db2/qsql_db2.h
    SOURCES += drivers/db2/qsql_db2.cpp
    
    unix:LIBS += -ldb2
    
    win32 {
        !win32-borland:LIBS += -ldb2cli
#        win32-borland:LIBS  += $(BCB)/lib/PSDK/db2cli.lib
    }
}

contains(sql-drivers, ibase) {
    HEADERS += drivers/ibase/qsql_ibase.h
    SOURCES += drivers/ibase/qsql_ibase.cpp
    
    unix:LIBS *= -lgds  
    
    win32 {
        !win32-borland:LIBS *= -lgds32_ms
        win32-borland:LIBS  += gds32.lib
    }
}

contains(sql-drivers, sqlite2) {
    HEADERS += drivers/sqlite2/qsql_sqlite2.h
    SOURCES += drivers/sqlite2/qsql_sqlite2.cpp
    !contains(LIBS, .*sqlite.*):LIBS *= -lsqlite
}

contains(sql-drivers, sqlite) {
    !system-sqlite:!contains( LIBS, .*sqlite3.* ) {
        CONFIG(release, debug|release):DEFINES *= NDEBUG
        DEFINES += SQLITE_OMIT_LOAD_EXTENSION SQLITE_OMIT_COMPLETE 
        INCLUDEPATH +=  ../3rdparty/sqlite
        SOURCES +=      ../3rdparty/sqlite/sqlite3.c
    } else {
        LIBS *= $$QT_LFLAGS_SQLITE
        QMAKE_CXXFLAGS *= $$QT_CFLAGS_SQLITE
    }

    HEADERS +=      drivers/sqlite/qsql_sqlite.h
    SOURCES +=      drivers/sqlite/qsql_sqlite.cpp
}

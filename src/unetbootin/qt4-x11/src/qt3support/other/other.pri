# Qt compat module

HEADERS +=      other/q3dropsite.h \
		other/q3dragobject.h \
                other/q3accel.h \
                other/q3mimefactory.h \
		other/q3polygonscanner.h \
		other/q3process.h \
		other/q3membuf_p.h \
		other/q3boxlayout.h \
		other/q3gridlayout.h

SOURCES +=      other/q3dropsite.cpp \
		other/q3dragobject.cpp \
                other/q3accel.cpp \
                other/q3mimefactory.cpp \
		other/q3polygonscanner.cpp \
		other/q3process.cpp \
		other/q3membuf.cpp

unix:SOURCES += other/q3process_unix.cpp
win32:SOURCES+= other/q3process_win.cpp



TEMPLATE	= app
CONFIG		+= qt warn_on
HEADERS		= cannonfield.h \
		  gameboard.h \
		  lcdrange.h
SOURCES		= cannonfield.cpp \
		  gameboard.cpp \
		  lcdrange.cpp \
		  main.cpp
TARGET		= t14
QTDIR_build:REQUIRES="contains(QT_CONFIG, full-config)"
unix:LIBS += -lm

# install
target.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial/t14
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS t14.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial/t14
INSTALLS += target sources

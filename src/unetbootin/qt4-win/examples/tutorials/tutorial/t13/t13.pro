TEMPLATE	= app
CONFIG		+= qt warn_on
HEADERS		= cannonfield.h \
		  gameboard.h \
		  lcdrange.h
SOURCES		= cannonfield.cpp \
		  gameboard.cpp \
		  lcdrange.cpp \
		  main.cpp
TARGET		= t13
QTDIR_build:REQUIRES="contains(QT_CONFIG, full-config)"
unix:LIBS += -lm


# install
target.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial/t13
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS t13.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial/t13
INSTALLS += target sources

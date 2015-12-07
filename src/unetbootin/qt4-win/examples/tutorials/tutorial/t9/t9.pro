TEMPLATE	= app
CONFIG		+= qt warn_on
HEADERS		= cannonfield.h \
		  lcdrange.h
SOURCES		= cannonfield.cpp \
		  lcdrange.cpp \
		  main.cpp
TARGET		= t9
QTDIR_build:REQUIRES="contains(QT_CONFIG, full-config)"

# install
target.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial/t9
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS t9.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial/t9
INSTALLS += target sources

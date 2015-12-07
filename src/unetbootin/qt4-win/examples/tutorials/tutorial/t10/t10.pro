TEMPLATE	= app
CONFIG		+= qt warn_on
HEADERS		= cannonfield.h \
		  lcdrange.h
SOURCES		= cannonfield.cpp \
		  lcdrange.cpp \
		  main.cpp
TARGET		= t10
QTDIR_build:REQUIRES="contains(QT_CONFIG, full-config)"

# install
target.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial/t10
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS t10.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial/t10
INSTALLS += target sources

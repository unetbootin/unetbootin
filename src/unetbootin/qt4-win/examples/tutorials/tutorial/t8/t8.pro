TEMPLATE	= app
CONFIG		+= qt warn_on
HEADERS		= cannonfield.h \
		  lcdrange.h
SOURCES		= cannonfield.cpp \
		  lcdrange.cpp \
		  main.cpp
TARGET		= t8
QTDIR_build:REQUIRES="contains(QT_CONFIG, large-config)"

# install
target.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial/t8
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS t8.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial/t8
INSTALLS += target sources

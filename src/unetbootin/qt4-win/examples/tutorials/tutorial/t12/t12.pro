TEMPLATE	= app
CONFIG		+= qt warn_on
HEADERS		= cannonfield.h \
		  lcdrange.h
SOURCES		= cannonfield.cpp \
		  lcdrange.cpp \
		  main.cpp
TARGET		= t12
QTDIR_build:REQUIRES="contains(QT_CONFIG, full-config)"
unix:LIBS += -lm

# install
target.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial/t12
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS t12.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial/t12
INSTALLS += target sources

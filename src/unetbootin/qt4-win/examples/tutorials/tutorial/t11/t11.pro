TEMPLATE	= app
CONFIG		+= qt warn_on
HEADERS		= cannonfield.h \
		  lcdrange.h
SOURCES		= cannonfield.cpp \
		  lcdrange.cpp \
		  main.cpp
TARGET		= t11
QTDIR_build:REQUIRES="contains(QT_CONFIG, full-config)"
unix:LIBS += -lm


# install
target.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial/t11
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS t11.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial/t11
INSTALLS += target sources

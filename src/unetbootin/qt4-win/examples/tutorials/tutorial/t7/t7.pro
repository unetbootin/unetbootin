TEMPLATE	= app
CONFIG		+= qt warn_on
HEADERS		= lcdrange.h
SOURCES		= lcdrange.cpp \
		  main.cpp
TARGET		= t7
QTDIR_build:REQUIRES="contains(QT_CONFIG, large-config)"

# install
target.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial/t7
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS t7.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial/t7
INSTALLS += target sources

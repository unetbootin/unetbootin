TEMPLATE	= app
CONFIG		+= qt warn_on
HEADERS		=
SOURCES		= main.cpp
TARGET		= t1
QTDIR_build:REQUIRES="contains(QT_CONFIG, small-config)"

# install
target.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial/t1
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS t1.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial/t1
INSTALLS += target sources

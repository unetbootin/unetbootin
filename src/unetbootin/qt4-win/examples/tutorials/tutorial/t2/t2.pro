TEMPLATE	= app
CONFIG		+= qt warn_on
HEADERS		=
SOURCES		= main.cpp
TARGET		= t2
QTDIR_build:REQUIRES="contains(QT_CONFIG, small-config)"

# install
target.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial/t2
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS t2.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial/t2
INSTALLS += target sources

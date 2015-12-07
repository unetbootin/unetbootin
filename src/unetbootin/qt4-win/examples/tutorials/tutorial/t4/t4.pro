TEMPLATE	= app
CONFIG		+= qt warn_on
HEADERS		=
SOURCES		= main.cpp
TARGET		= t4
QTDIR_build:REQUIRES="contains(QT_CONFIG, small-config)"

# install
target.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial/t4
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS t4.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial/t4
INSTALLS += target sources

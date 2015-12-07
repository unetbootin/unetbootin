TEMPLATE	= app
CONFIG		+= qt warn_on
HEADERS		=
SOURCES		= main.cpp
TARGET		= t3
QTDIR_build:REQUIRES="contains(QT_CONFIG, small-config)"

# install
target.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial/t3
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS t3.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial/t3
INSTALLS += target sources

TEMPLATE	= app
CONFIG		+= qt warn_on
HEADERS		=
SOURCES		= main.cpp
TARGET		= t6
QTDIR_build:REQUIRES="contains(QT_CONFIG, large-config)"

# install
target.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial/t6
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS t6.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial/t6
INSTALLS += target sources

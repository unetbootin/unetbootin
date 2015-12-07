TEMPLATE	= app
CONFIG		+= qt warn_on
HEADERS		=
SOURCES		= main.cpp
TARGET		= t5
QTDIR_build:REQUIRES="contains(QT_CONFIG, large-config)"

# install
target.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial/t5
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS t5.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/tutorials/tutorial/t5
INSTALLS += target sources

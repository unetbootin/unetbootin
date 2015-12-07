QT += script
CONFIG += uitools

HEADERS = tetrixboard.h
SOURCES = main.cpp \
	  tetrixboard.cpp

FORMS   = tetrixwindow.ui
RESOURCES = tetrix.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/script/tetrix
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS tetrix.pro *.js
sources.path = $$[QT_INSTALL_EXAMPLES]/script/tetrix
INSTALLS += target sources

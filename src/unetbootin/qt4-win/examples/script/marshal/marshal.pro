QT = core script
CONFIG += console
SOURCES += main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/script/marshal
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS marshal.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/script/marshal
INSTALLS += target sources

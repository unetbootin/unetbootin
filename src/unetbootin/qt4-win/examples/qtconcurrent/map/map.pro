TEMPLATE = app
TARGET = mapdemo
DEPENDPATH += .
INCLUDEPATH += .

# Input
SOURCES += main.cpp
CONFIG += console

# install
target.path = $$[QT_INSTALL_EXAMPLES]/qtconcurrent/map
sources.files = $$SOURCES $$HEADERS $$FORMS $$RESOURCES *.pro *.png
sources.path = $$[QT_INSTALL_EXAMPLES]/qtconcurrent/map
INSTALLS += target sources

TEMPLATE = app

CONFIG += qt warn_on
HEADERS += model.h
SOURCES += model.cpp main.cpp
RESOURCES += interview.qrc

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

# install
target.path = $$[QT_INSTALL_DEMOS]/interview
sources.files = $$SOURCES $$HEADERS $$RESOURCES README *.pro images
sources.path = $$[QT_INSTALL_DEMOS]/interview
INSTALLS += target sources


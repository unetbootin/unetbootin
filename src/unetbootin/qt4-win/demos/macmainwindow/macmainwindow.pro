TEMPLATE        = app
TARGET          = macmainwindow

CONFIG          += qt warn_on console

SOURCES += main.cpp

build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

# install
mac {
target.path = $$[QT_INSTALL_DEMOS]/macmainwindow
sources.files = $$SOURCES  *.pro *.html
sources.path = $$[QT_INSTALL_DEMOS]/macmainwindow
INSTALLS += target sources
}
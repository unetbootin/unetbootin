TEMPLATE = app
QT += svg

# Input
HEADERS += embeddedsvgviewer.h
SOURCES += embeddedsvgviewer.cpp main.cpp
RESOURCES += embeddedsvgviewer.qrc

target.path = $$[QT_INSTALL_DEMOS]/embedded/embeddedsvgviewer
sources.files = $$SOURCES $$HEADERS $$RESOURCES *.pro *.html *.svg files
sources.path = $$[QT_INSTALL_DEMOS]/embedded/embeddedsvgviewer
INSTALLS += target sources 

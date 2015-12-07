TEMPLATE = app
QT += script
# Input
HEADERS += qcontext2dcanvas.h context2d.h domimage.h window.h
SOURCES += qcontext2dcanvas.cpp context2d.cpp domimage.cpp window.cpp main.cpp 
RESOURCES += context2d.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/script/context2d
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS context2d.pro scripts
sources.path = $$[QT_INSTALL_EXAMPLES]/script/context2d
INSTALLS += target sources

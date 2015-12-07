TEMPLATE	= app
TARGET		= portedcanvas

CONFIG		+= qt warn_on

HEADERS		= canvas.h
SOURCES		= canvas.cpp main.cpp
QT +=  qt3support 

RESOURCES += portedcanvas.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/graphicsview/portedcanvas
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS portedcanvas.pro *.png *.xpm *.doc
sources.path = $$[QT_INSTALL_EXAMPLES]/graphicsview/portedcanvas
INSTALLS += target sources

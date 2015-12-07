TEMPLATE = app
INCLUDEPATH += .

# Input
HEADERS += ledmeter.h sprites.h toplevel.h view.h
SOURCES += ledmeter.cpp main.cpp toplevel.cpp view.cpp
#The following line was inserted by qt3to4
QT +=  qt3support 

HEADERS += animateditem.h
SOURCES += animateditem.cpp

RESOURCES += portedasteroids.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/graphicsview/portedasteroids
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS portedasteroids.pro bg.png sounds sprites
sources.path = $$[QT_INSTALL_EXAMPLES]/graphicsview/portedasteroids
INSTALLS += target sources

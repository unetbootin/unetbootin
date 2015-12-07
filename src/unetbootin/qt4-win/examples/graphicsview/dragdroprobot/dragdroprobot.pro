HEADERS += \
	coloritem.h \
	robot.h

SOURCES += \
	coloritem.cpp \
	main.cpp \
	robot.cpp

RESOURCES += \
	robot.qrc


# install
target.path = $$[QT_INSTALL_EXAMPLES]/graphicsview/dragdroprobot
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS dragdroprobot.pro images
sources.path = $$[QT_INSTALL_EXAMPLES]/graphicsview/dragdroprobot
INSTALLS += target sources

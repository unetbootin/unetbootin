QT = core script
win32: CONFIG += console
mac:CONFIG -= app_bundle

SOURCES += main.cpp

include(bytearrayclass.pri)

# install
target.path = $$[QT_INSTALL_EXAMPLES]/script/customclass
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.pro *.pri
sources.path = $$[QT_INSTALL_EXAMPLES]/script/customclass
INSTALLS += target sources

QT += script
SOURCES += main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/script/helloscript
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS helloscript.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/script/helloscript
INSTALLS += target sources

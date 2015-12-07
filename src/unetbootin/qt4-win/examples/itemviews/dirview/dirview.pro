SOURCES       = main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/itemviews/dirview
sources.files = $$SOURCES *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/itemviews/dirview
INSTALLS += target sources

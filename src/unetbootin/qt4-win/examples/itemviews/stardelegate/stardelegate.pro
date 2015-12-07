HEADERS       = stardelegate.h \
                stareditor.h \
                starrating.h
SOURCES       = main.cpp \
                stardelegate.cpp \
                stareditor.cpp \
                starrating.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/itemviews/stardelegate
sources.files = $$SOURCES $$HEADERS *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/itemviews/stardelegate
INSTALLS += target sources


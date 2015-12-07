HEADERS     = movieplayer.h
RESOURCES   = movie.qrc
SOURCES     = main.cpp \
              movieplayer.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/movie
sources.files = $$SOURCES $$HEADERS $$RESOURCES movie.pro images movies
sources.path = $$[QT_INSTALL_EXAMPLES]/widgets/movie
INSTALLS += target sources

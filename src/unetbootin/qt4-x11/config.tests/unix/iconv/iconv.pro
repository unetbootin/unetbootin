SOURCES = iconv.cpp
CONFIG -= qt dylib app_bundle
mac:LIBS += -liconv

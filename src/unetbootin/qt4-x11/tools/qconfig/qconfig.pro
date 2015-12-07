TEMPLATE	= app
CONFIG		+= qt warn_on
build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}
HEADERS		= feature.h featuretreemodel.h graphics.h
SOURCES		= main.cpp feature.cpp featuretreemodel.cpp
INTERFACES	=
TARGET		= qconfig

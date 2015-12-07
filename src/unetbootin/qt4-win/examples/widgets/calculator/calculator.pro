HEADERS       = button.h \
                calculator.h
SOURCES       = button.cpp \
                calculator.cpp \
                main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/calculator
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS calculator.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/widgets/calculator
INSTALLS += target sources

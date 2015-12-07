#! [0]
HEADERS     = calculatorform.h
#! [0] #! [1]
FORMS       = calculatorform.ui
#! [1]
SOURCES     = calculatorform.cpp \
              main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/designer/calculatorform
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS *.pro
sources.path = $$[QT_INSTALL_EXAMPLES]/designer/calculatorform
INSTALLS += target sources

# Qt compat module

HEADERS += \
	dialogs/q3filedialog.h \
	dialogs/q3tabdialog.h \
	dialogs/q3progressdialog.h \
	dialogs/q3wizard.h

SOURCES += \
	dialogs/q3filedialog.cpp \
	dialogs/q3progressdialog.cpp \
	dialogs/q3tabdialog.cpp \
	dialogs/q3wizard.cpp

win32:SOURCES += dialogs/q3filedialog_win.cpp
!embedded:mac:SOURCES  += dialogs/q3filedialog_mac.cpp

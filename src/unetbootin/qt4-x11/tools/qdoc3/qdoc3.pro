DEFINES += QDOC2_COMPAT
#DEFINES += QT_NO_CAST_TO_ASCII
#DEFINES += QT_NO_CAST_FROM_ASCII

QT = core xml
CONFIG += console
build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}
mac:CONFIG -= app_bundle
HEADERS += apigenerator.h \
           archiveextractor.h \
	   atom.h \
	   bookgenerator.h \
	   ccodeparser.h \
           codechunk.h \
           codemarker.h \
	   codeparser.h \
	   command.h \
           config.h \
	   cppcodemarker.h \
	   cppcodeparser.h \
	   cpptoqsconverter.h \
	   dcfsection.h \
           doc.h \
	   editdistance.h \
	   generator.h \
           helpprojectwriter.h \
	   htmlgenerator.h \
           jambiapiparser.h \
	   javacodemarker.h \
           javadocgenerator.h \
           linguistgenerator.h \
           location.h \
	   loutgenerator.h \
	   mangenerator.h \
           node.h \
           openedlist.h \
	   pagegenerator.h \
	   plaincodemarker.h \
	   polyarchiveextractor.h \
	   polyuncompressor.h \
	   qsakernelparser.h \
	   qscodemarker.h \
	   qscodeparser.h \
           quoter.h \
	   separator.h \
	   sgmlgenerator.h \
	   text.h \
	   tokenizer.h \
	   tr.h \
	   tree.h \
	   uncompressor.h \
           webxmlgenerator.h
SOURCES += apigenerator.cpp \
           archiveextractor.cpp \
	   atom.cpp \
	   bookgenerator.cpp \
	   ccodeparser.cpp \
           codechunk.cpp \
           codemarker.cpp \
	   codeparser.cpp \
	   command.cpp \
           config.cpp \
	   cppcodemarker.cpp \
	   cppcodeparser.cpp \
	   cpptoqsconverter.cpp \
	   dcfsection.cpp \
           doc.cpp \
	   editdistance.cpp \
	   generator.cpp \
           helpprojectwriter.cpp \
	   htmlgenerator.cpp \
           jambiapiparser.cpp \
	   javacodemarker.cpp \
           javadocgenerator.cpp \
           linguistgenerator.cpp \
           location.cpp \
	   loutgenerator.cpp \
	   mangenerator.cpp \
           main.cpp \
           node.cpp \
           openedlist.cpp \
	   pagegenerator.cpp \
	   plaincodemarker.cpp \
	   polyarchiveextractor.cpp \
	   polyuncompressor.cpp \
	   qsakernelparser.cpp \
	   qscodemarker.cpp \
	   qscodeparser.cpp \
           quoter.cpp \
	   separator.cpp \
	   sgmlgenerator.cpp \
	   text.cpp \
	   tokenizer.cpp \
	   tree.cpp \
	   uncompressor.cpp \
           webxmlgenerator.cpp \
	   yyindent.cpp


win32 {
    QT_WINCONFIG = release
    CONFIG(debug, debug|release) {
        QT_WINCONFIG = debug
    }    
}

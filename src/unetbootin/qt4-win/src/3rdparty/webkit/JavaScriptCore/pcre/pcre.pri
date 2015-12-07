# Perl Compatible Regular Expressions - Qt4 build info
VPATH += $$PWD
INCLUDEPATH += $$PWD $$OUTPUT_DIR/JavaScriptCore/kjs/tmp

isEmpty(GENERATED_SOURCES_DIR):GENERATED_SOURCES_DIR = tmp

SOURCES += \
    pcre_get.c \
    pcre_refcount.c \
    pcre_ucp_findchar.c \
    pcre_compile.c \
    pcre_globals.c \
    pcre_config.c \
    pcre_version.c \
    pcre_info.c \
    pcre_study.c \
    pcre_exec.c \
    pcre_xclass.c \
    pcre_tables.c \
    pcre_maketables.c \
    pcre_try_flipped.c \
    pcre_ord2utf8.c \
    pcre_fullinfo.c

CTGENFILE += \
    dftables.c

!CONFIG(QTDIR_build) {
    defineTest(addExtraCompiler) {
        QMAKE_EXTRA_COMPILERS += $$1
        generated_files.depends += compiler_$${1}_make_all
        export(QMAKE_EXTRA_COMPILERS)
        export(generated_files.depends)
        return(true)
    }
}

# GENERATOR: "chartables.c": compile and execute the chartables generator (and add it to sources)
ctgen.output = $$GENERATED_SOURCES_DIR/chartables.c
ctgen.commands = $$OUTPUT_DIR/JavaScriptCore/pcre/tmp/dftables ${QMAKE_FILE_OUT}
ctgen.input = CTGENFILE
ctgen.CONFIG += target_predeps no_link
ctgen.variable_out = GENERATED_SOURCES
ctgen.dependency_type = TYPE_C
ctgen.clean = ${QMAKE_FILE_OUT} ${QMAKE_VAR_GENERATED_SOURCES_DIR}${QMAKE_FILE_BASE}
addExtraCompiler(ctgen)

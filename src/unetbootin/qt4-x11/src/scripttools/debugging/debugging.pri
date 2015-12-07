RESOURCES += $$PWD/scripttools_debugging.qrc

SOURCES += \
    $$PWD/qscriptdebuggervalue.cpp \
    $$PWD/qscriptdebuggervalueproperty.cpp \
    $$PWD/qscriptdebuggercommand.cpp \
    $$PWD/qscriptdebuggerresponse.cpp \
    $$PWD/qscriptdebuggerevent.cpp \
    $$PWD/qscriptdebuggercommandexecutor.cpp \
    $$PWD/qscriptdebuggercommandschedulerfrontend.cpp \
    $$PWD/qscriptdebuggerbackend.cpp \
    $$PWD/qscriptdebuggeragent.cpp \
    $$PWD/qscriptdebuggerfrontend.cpp \
    $$PWD/qscriptstdmessagehandler.cpp \
    $$PWD/qscriptenginedebuggerfrontend.cpp \
    $$PWD/qscriptdebuggerjob.cpp \
    $$PWD/qscriptdebuggercommandschedulerjob.cpp \
    $$PWD/qscriptvalueproperty.cpp \
    $$PWD/qscriptobjectsnapshot.cpp \
    $$PWD/qscriptscriptdata.cpp \
    $$PWD/qscriptxmlparser.cpp \
    $$PWD/qscriptbreakpointdata.cpp \
    $$PWD/qscriptdebugger.cpp \
    $$PWD/qscriptdebuggerconsole.cpp \
    $$PWD/qscriptdebuggerconsolecommand.cpp \
    $$PWD/qscriptdebuggerconsolecommandjob.cpp \
    $$PWD/qscriptdebuggerconsolecommandgroupdata.cpp \
    $$PWD/qscriptdebuggerconsolecommandmanager.cpp \
    $$PWD/qscriptcompletiontaskinterface.cpp \
    $$PWD/qscriptcompletiontask.cpp \
    $$PWD/qscriptdebuggerconsoleglobalobject.cpp \
    $$PWD/qscriptdebuggerscriptedconsolecommand.cpp

HEADERS += \
    $$PWD/qscriptdebuggervalue_p.h \
    $$PWD/qscriptdebuggervalueproperty_p.h \
    $$PWD/qscriptdebuggerobjectsnapshotdelta_p.h \
    $$PWD/qscriptdebuggercommand_p.h \
    $$PWD/qscriptdebuggercommandschedulerinterface_p.h \
    $$PWD/qscriptdebuggercommandschedulerfrontend_p.h \
    $$PWD/qscriptdebuggerresponse_p.h \
    $$PWD/qscriptdebuggerresponsehandlerinterface_p.h \
    $$PWD/qscriptdebuggerevent_p.h \
    $$PWD/qscriptdebuggercommandexecutor_p.h \
    $$PWD/qscriptdebuggerbackend_p_p.h \
    $$PWD/qscriptdebuggerbackend_p.h \
    $$PWD/qscriptdebuggeragent_p_p.h \
    $$PWD/qscriptdebuggeragent_p.h \
    $$PWD/qscriptdebuggerfrontend_p_p.h \
    $$PWD/qscriptdebuggerfrontend_p.h \
    $$PWD/qscriptmessagehandlerinterface_p.h \
    $$PWD/qscriptstdmessagehandler_p.h \
    $$PWD/qscriptenginedebuggerfrontend_p.h \
    $$PWD/qscriptdebuggerjob_p_p.h \
    $$PWD/qscriptdebuggerjob_p.h \
    $$PWD/qscriptdebuggercommandschedulerjob_p_p.h \
    $$PWD/qscriptdebuggercommandschedulerjob_p.h \
    $$PWD/qscriptdebuggerjobschedulerinterface_p.h \
    $$PWD/qscriptdebuggereventhandlerinterface_p.h \
    $$PWD/qscriptvalueproperty_p.h \
    $$PWD/qscriptobjectsnapshot_p.h \
    $$PWD/qscriptscriptdata_p.h \
    $$PWD/qscriptxmlparser_p.h \
    $$PWD/qscriptbreakpointdata_p.h \
    $$PWD/qscriptdebugger_p.h \
    $$PWD/qscriptdebuggerconsole_p.h \
    $$PWD/qscriptdebuggerconsolecommand_p_p.h \
    $$PWD/qscriptdebuggerconsolecommand_p.h \
    $$PWD/qscriptdebuggerconsolecommandjob_p_p.h \
    $$PWD/qscriptdebuggerconsolecommandjob_p.h \
    $$PWD/qscriptdebuggerconsolecommandgroupdata_p.h \
    $$PWD/qscriptdebuggerconsolecommandmanager_p.h \
    $$PWD/qscriptdebuggerconsolehistorianinterface_p.h \
    $$PWD/qscriptcompletionproviderinterface_p.h \
    $$PWD/qscriptcompletiontaskinterface_p_p.h \
    $$PWD/qscriptcompletiontaskinterface_p.h \
    $$PWD/qscriptcompletiontask_p.h \
    $$PWD/qscripttooltipproviderinterface_p.h \
    $$PWD/qscriptdebuggerconsoleglobalobject_p.h \
    $$PWD/qscriptdebuggerscriptedconsolecommand_p.h

SOURCES += \
    $$PWD/qscriptedit.cpp \
    $$PWD/qscriptsyntaxhighlighter.cpp

HEADERS += \
    $$PWD/qscriptedit_p.h \
    $$PWD/qscriptsyntaxhighlighter_p.h

SOURCES += \
    $$PWD/qscriptdebuggerconsolewidgetinterface.cpp \
    $$PWD/qscriptdebuggerconsolewidget.cpp \
    $$PWD/qscriptdebuggerstackwidgetinterface.cpp \
    $$PWD/qscriptdebuggerstackwidget.cpp \
    $$PWD/qscriptdebuggerstackmodel.cpp \
    $$PWD/qscriptdebuggerscriptswidgetinterface.cpp \
    $$PWD/qscriptdebuggerscriptswidget.cpp \
    $$PWD/qscriptdebuggerscriptsmodel.cpp \
    $$PWD/qscriptdebuggerlocalswidgetinterface.cpp \
    $$PWD/qscriptdebuggerlocalswidget.cpp \
    $$PWD/qscriptdebuggerlocalsmodel.cpp \
    $$PWD/qscriptdebuggercodewidgetinterface.cpp \
    $$PWD/qscriptdebuggercodewidget.cpp \
    $$PWD/qscriptdebuggercodeviewinterface.cpp \
    $$PWD/qscriptdebuggercodeview.cpp \
    $$PWD/qscriptdebuggercodefinderwidgetinterface.cpp \
    $$PWD/qscriptdebuggercodefinderwidget.cpp \
    $$PWD/qscriptdebugoutputwidgetinterface.cpp \
    $$PWD/qscriptdebugoutputwidget.cpp \
    $$PWD/qscriptbreakpointswidgetinterface.cpp \
    $$PWD/qscriptbreakpointswidget.cpp \
    $$PWD/qscriptbreakpointsmodel.cpp \
    $$PWD/qscripterrorlogwidgetinterface.cpp \
    $$PWD/qscripterrorlogwidget.cpp

HEADERS += \
    $$PWD/qscriptdebuggerconsolewidgetinterface_p_p.h \
    $$PWD/qscriptdebuggerconsolewidgetinterface_p.h \
    $$PWD/qscriptdebuggerconsolewidget_p.h \
    $$PWD/qscriptdebuggerstackwidgetinterface_p_p.h \
    $$PWD/qscriptdebuggerstackwidgetinterface_p.h \
    $$PWD/qscriptdebuggerstackwidget_p.h \
    $$PWD/qscriptdebuggerstackmodel_p.h \
    $$PWD/qscriptdebuggerscriptswidgetinterface_p_p.h \
    $$PWD/qscriptdebuggerscriptswidgetinterface_p.h \
    $$PWD/qscriptdebuggerscriptswidget_p.h \
    $$PWD/qscriptdebuggerscriptsmodel_p.h \
    $$PWD/qscriptdebuggerlocalswidgetinterface_p_p.h \
    $$PWD/qscriptdebuggerlocalswidgetinterface_p.h \
    $$PWD/qscriptdebuggerlocalswidget_p.h \
    $$PWD/qscriptdebuggerlocalsmodel_p.h \
    $$PWD/qscriptdebuggercodewidgetinterface_p_p.h \
    $$PWD/qscriptdebuggercodewidgetinterface_p.h \
    $$PWD/qscriptdebuggercodewidget_p.h \
    $$PWD/qscriptdebuggercodeviewinterface_p_p.h \
    $$PWD/qscriptdebuggercodeviewinterface_p.h \
    $$PWD/qscriptdebuggercodeview_p.h \
    $$PWD/qscriptdebuggercodefinderwidgetinterface_p_p.h \
    $$PWD/qscriptdebuggercodefinderwidgetinterface_p.h \
    $$PWD/qscriptdebuggercodefinderwidget_p.h \
    $$PWD/qscriptdebugoutputwidgetinterface_p_p.h \
    $$PWD/qscriptdebugoutputwidgetinterface_p.h \
    $$PWD/qscriptdebugoutputwidget_p.h \
    $$PWD/qscriptbreakpointswidgetinterface_p_p.h \
    $$PWD/qscriptbreakpointswidgetinterface_p.h \
    $$PWD/qscriptbreakpointswidget_p.h \
    $$PWD/qscriptbreakpointsmodel_p.h \
    $$PWD/qscripterrorlogwidgetinterface_p_p.h \
    $$PWD/qscripterrorlogwidgetinterface_p.h \
    $$PWD/qscripterrorlogwidget_p.h \
    $$PWD/qscriptdebuggerwidgetfactoryinterface_p.h

SOURCES += \
    $$PWD/qscriptenginedebugger.cpp

HEADERS += \
    $$PWD/qscriptenginedebugger.h

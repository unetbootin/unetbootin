/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef QPLUGIN_H
#define QPLUGIN_H

#include <QtCore/qobject.h>
#include <QtCore/qpointer.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

QT_MODULE(Core)

#ifndef Q_EXTERN_C
#  ifdef __cplusplus
#    define Q_EXTERN_C extern "C"
#  else
#    define Q_EXTERN_C extern
#  endif
#endif

typedef QObject *(*QtPluginInstanceFunction)();

void Q_CORE_EXPORT qRegisterStaticPluginInstanceFunction(QtPluginInstanceFunction function);

#define Q_IMPORT_PLUGIN(PLUGIN) \
        extern QT_PREPEND_NAMESPACE(QObject) *qt_plugin_instance_##PLUGIN(); \
        class Static##PLUGIN##PluginInstance{ \
        public: \
                Static##PLUGIN##PluginInstance() { \
                qRegisterStaticPluginInstanceFunction(qt_plugin_instance_##PLUGIN); \
                } \
        }; \
       static Static##PLUGIN##PluginInstance static##PLUGIN##Instance;

#define Q_PLUGIN_INSTANCE(IMPLEMENTATION) \
        { \
            static QT_PREPEND_NAMESPACE(QPointer)<QT_PREPEND_NAMESPACE(QObject)> _instance; \
            if (!_instance)      \
                _instance = new IMPLEMENTATION; \
            return _instance; \
        }

#  define Q_EXPORT_PLUGIN(PLUGIN) \
            Q_EXPORT_PLUGIN2(PLUGIN, PLUGIN)

#  define Q_EXPORT_STATIC_PLUGIN(PLUGIN) \
            Q_EXPORT_STATIC_PLUGIN2(PLUGIN, PLUGIN)

#if defined(QT_STATICPLUGIN)

#  define Q_EXPORT_PLUGIN2(PLUGIN, PLUGINCLASS) \
            Q_DECL_EXPORT QT_PREPEND_NAMESPACE(QObject) \
                *qt_plugin_instance_##PLUGIN() \
            Q_PLUGIN_INSTANCE(PLUGINCLASS)

#  define Q_EXPORT_STATIC_PLUGIN2(PLUGIN, PLUGINCLASS) \
            Q_EXPORT_PLUGIN2(PLUGIN, PLUGINCLASS)

#else
// NOTE: if you change pattern, you MUST change the pattern in
// qlibrary.cpp as well.  changing the pattern will break all
// backwards compatibility as well (no old plugins will be loaded).
#  ifdef QPLUGIN_DEBUG_STR
#    undef QPLUGIN_DEBUG_STR
#  endif
#  ifdef QT_NO_DEBUG
#    define QPLUGIN_DEBUG_STR "false"
#  else
#    define QPLUGIN_DEBUG_STR "true"
#  endif
#  define Q_PLUGIN_VERIFICATION_DATA \
    static const char *qt_plugin_verification_data = \
      "pattern=""QT_PLUGIN_VERIFICATION_DATA""\n" \
      "version="QT_VERSION_STR"\n" \
      "debug="QPLUGIN_DEBUG_STR"\n" \
      "buildkey="QT_BUILD_KEY"\0";

#  if defined (Q_OS_WIN32) && defined(Q_CC_BOR)
#     define Q_STANDARD_CALL __stdcall
#  else
#     define Q_STANDARD_CALL
#  endif

#  define Q_EXPORT_PLUGIN2(PLUGIN, PLUGINCLASS)      \
            Q_PLUGIN_VERIFICATION_DATA \
            Q_EXTERN_C Q_DECL_EXPORT \
            const char * Q_STANDARD_CALL qt_plugin_query_verification_data() \
            { return qt_plugin_verification_data; } \
            Q_EXTERN_C Q_DECL_EXPORT QT_PREPEND_NAMESPACE(QObject) * Q_STANDARD_CALL qt_plugin_instance() \
            Q_PLUGIN_INSTANCE(PLUGINCLASS)

#  define Q_EXPORT_STATIC_PLUGIN2(PLUGIN, PLUGINCLASS)

#endif

QT_END_NAMESPACE

QT_END_HEADER

#endif // Q_PLUGIN_H

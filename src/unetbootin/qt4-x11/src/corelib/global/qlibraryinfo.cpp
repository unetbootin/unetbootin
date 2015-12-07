/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qdir.h"
#include "qfile.h"    
#include "qconfig.h"
#include "qsettings.h"
#include "qlibraryinfo.h"
#include "qpointer.h"

#ifdef QT_BUILD_QMAKE
QT_BEGIN_NAMESPACE
extern QString qmake_libraryInfoFile();
QT_END_NAMESPACE
#else
# include "qcoreapplication.h"
#endif

#ifdef Q_OS_MAC
#  include "private/qcore_mac_p.h"
#endif

#include "qconfig.cpp"

QT_BEGIN_NAMESPACE

#ifndef QT_NO_SETTINGS

struct QLibrarySettings
{
    QLibrarySettings();
    ~QLibrarySettings() { delete static_cast<QSettings *>(settings); }
    QSettings *settings;
};
Q_GLOBAL_STATIC(QLibrarySettings, qt_library_settings)

class QLibraryInfoPrivate
{
public:
    static QSettings *findConfiguration();
    static void cleanup()
    {
        QLibrarySettings *ls = qt_library_settings();
        if (ls) {
            delete static_cast<QSettings *>(ls->settings);
            ls->settings = 0;
        }
    }
    static QSettings *configuration()
    {
#ifdef QT_NO_THREAD
        // This recursion guard should be a temporary solution; the recursive
        // dependency should be found and removed.
        static bool initializing = false;
        if (initializing)
            return 0;
        initializing = true;
#endif
        QLibrarySettings *ls = qt_library_settings();
#ifdef QT_NO_THREAD
        initializing = false;
#endif
        return ls ? static_cast<QSettings *>(qt_library_settings()->settings) : (QSettings*)0;
    }
};

QLibrarySettings::QLibrarySettings()
{
    settings = QLibraryInfoPrivate::findConfiguration();
#ifndef QT_BUILD_QMAKE
    qAddPostRoutine(QLibraryInfoPrivate::cleanup);
#endif
}

QSettings *QLibraryInfoPrivate::findConfiguration()
{
    QString qtconfig = QLatin1String(":/qt/etc/qt.conf");
#ifdef QT_BUILD_QMAKE
    if(!QFile::exists(qtconfig))
        qtconfig = qmake_libraryInfoFile();
#else
    if (!QFile::exists(qtconfig) && QCoreApplication::instance()) {
#ifdef Q_OS_MAC
	CFBundleRef bundleRef = CFBundleGetMainBundle();
        if (bundleRef) {
	    QCFType<CFURLRef> urlRef = CFBundleCopyResourceURL(bundleRef,
							       QCFString(QLatin1String("qt.conf")),
							       0,
							       0);
	    if (urlRef) {
	        QCFString path = CFURLCopyFileSystemPath(urlRef, kCFURLPOSIXPathStyle);
		qtconfig = QDir::cleanPath(path);
	    }
	}
	if (qtconfig.isEmpty())
#endif
            {
                QDir pwd(QCoreApplication::applicationDirPath());
                qtconfig = pwd.filePath(QLatin1String("qt.conf"));
	    }
    }
#endif
    if (QFile::exists(qtconfig))
        return new QSettings(qtconfig, QSettings::IniFormat);
    return 0;     //no luck
}

/*!
    \class QLibraryInfo
    \brief The QLibraryInfo class provides information about the Qt library.

    \ingroup misc
    \mainclass

    Many pieces of information are established when Qt is configured.
    Installation paths, license information, and even a unique build
    key. This class provides an abstraction for accessing this
    information.

    \table
    \header \o Function           \o Return value
    \row    \o buildKey()         \o A string that identifies the Qt version and
                                     the configuration. This key is used to ensure
                                     that \l{plugins} link against the same version
                                     of Qt as the application.
    \row    \o location()         \o The path to a certain Qt
                                     component (e.g., documentation, header files).
    \row    \o licensee(),
               licensedProducts() \o Licensing information.
    \endtable

    You can also use a \c qt.conf file to override the hard-coded paths
    that are compiled into the Qt library. For more information, see
    the \l {Using qt.conf} documentation.

    \sa QSysInfo, {Using qt.conf}
*/

/*! \internal

   You cannot create a QLibraryInfo, instead only the static functions are available to query
   information.
*/

QLibraryInfo::QLibraryInfo()
{ }

/*!
  Returns the person to whom this build of Qt is licensed.

  \sa licensedProducts()
*/

QString
QLibraryInfo::licensee()
{
    const char *str = QT_CONFIGURE_LICENSEE;
    return QString::fromLocal8Bit(str);
}

/*!
  Returns the products that the license for this build of Qt has access to.

  \sa licensee()
*/

QString
QLibraryInfo::licensedProducts()
{
    const char *str = QT_CONFIGURE_LICENSED_PRODUCTS;
    return QString::fromLatin1(str);
}

/*!
    Returns a unique key identifying this build of Qt and its
    configurations. This key is not globally unique, rather only useful
    for establishing of two configurations are compatible. This can be
    used to compare with the \c QT_BUILD_KEY preprocessor symbol.

    \sa location()
*/

QString
QLibraryInfo::buildKey()
{
    return QString::fromLatin1(QT_BUILD_KEY);
}

/*!
  Returns the location specified by \a loc.

*/

QString
QLibraryInfo::location(LibraryLocation loc)
{
    QString ret;
    if(!QLibraryInfoPrivate::configuration()) {
        const char *path = 0;
        switch (loc) {
#ifdef QT_CONFIGURE_PREFIX_PATH
        case PrefixPath:
            path = QT_CONFIGURE_PREFIX_PATH;
            break;
#endif
#ifdef QT_CONFIGURE_DOCUMENTATION_PATH
        case DocumentationPath:
            path = QT_CONFIGURE_DOCUMENTATION_PATH;
            break;
#endif
#ifdef QT_CONFIGURE_HEADERS_PATH
        case HeadersPath:
            path = QT_CONFIGURE_HEADERS_PATH;
            break;
#endif
#ifdef QT_CONFIGURE_LIBRARIES_PATH
        case LibrariesPath:
            path = QT_CONFIGURE_LIBRARIES_PATH;
            break;
#endif
#ifdef QT_CONFIGURE_BINARIES_PATH
        case BinariesPath:
            path = QT_CONFIGURE_BINARIES_PATH;
            break;
#endif
#ifdef QT_CONFIGURE_PLUGINS_PATH
        case PluginsPath:
            path = QT_CONFIGURE_PLUGINS_PATH;
            break;
#endif
#ifdef QT_CONFIGURE_DATA_PATH
        case DataPath:
            path = QT_CONFIGURE_DATA_PATH;
            break;
#endif
#ifdef QT_CONFIGURE_TRANSLATIONS_PATH
        case TranslationsPath:
            path = QT_CONFIGURE_TRANSLATIONS_PATH;
            break;
#endif
#ifdef QT_CONFIGURE_SETTINGS_PATH
        case SettingsPath:
            path = QT_CONFIGURE_SETTINGS_PATH;
            break;
#endif
#ifdef QT_CONFIGURE_EXAMPLES_PATH
        case ExamplesPath:
            path = QT_CONFIGURE_EXAMPLES_PATH;
            break;
#endif
#ifdef QT_CONFIGURE_DEMOS_PATH
        case DemosPath:
            path = QT_CONFIGURE_DEMOS_PATH;
            break;
#endif
        default:
            break;
        }

        if (path)
            ret = QString::fromLocal8Bit(path);
    } else {
        QString key;
        QString defaultValue;
        switch(loc) {
        case PrefixPath:
            key = QLatin1String("Prefix");
            break;
        case DocumentationPath:
            key = QLatin1String("Documentation");
            defaultValue = QLatin1String("doc");
            break;
        case HeadersPath:
            key = QLatin1String("Headers");
            defaultValue = QLatin1String("include");
            break;
        case LibrariesPath:
            key = QLatin1String("Libraries");
            defaultValue = QLatin1String("lib");
            break;
        case BinariesPath:
            key = QLatin1String("Binaries");
            defaultValue = QLatin1String("bin");
            break;
        case PluginsPath:
            key = QLatin1String("Plugins");
            defaultValue = QLatin1String("plugins");
            break;
        case DataPath:
            key = QLatin1String("Data");
            break;
        case TranslationsPath:
            key = QLatin1String("Translations");
            defaultValue = QLatin1String("translations");
            break;
        case SettingsPath:
            key = QLatin1String("Settings");
            break;
        case ExamplesPath:
            key = QLatin1String("Examples");
            break;
        case DemosPath:
            key = QLatin1String("Demos");
            break;
        default:
            break;
        }

        if(!key.isNull()) {
            QSettings *config = QLibraryInfoPrivate::configuration();
            config->beginGroup(QLatin1String("Paths"));

            QString subKey;
            {
                /*
                  find the child group whose version number is closest
                  to the library version.  for example and we have the
                  following groups:

                  Paths
                  Paths/4.0
                  Paths/4.1.2
                  Paths/4.2.5
                  Paths/5

                  if QT_VERSION is 4.0.1, then we use 'Paths/4.0'
                  if QT_VERSION is 4.1.5, then we use 'Paths/4.1.2'
                  if QT_VERSION is 4.6.3, then we use 'Paths/4.2.5'
                  if QT_VERSION is 6.0.2, then we use 'Paths/5'

                  note: any of the trailing version numbers may be
                  omitted (in which case, they default to zero),
                  i.e. 4 == 4.0.0, 4.1 == 4.1.0, and so on
                */
                enum {
                    QT_MAJOR = ((QT_VERSION >> 16) & 0xFF),
                    QT_MINOR = ((QT_VERSION >> 8) & 0xFF),
                    QT_PATCH = (QT_VERSION & 0xFF)
                };
                int maj = 0, min = 0, pat = 0;
                QStringList children = config->childGroups();
                for(int child = 0; child < children.size(); ++child) {
                    QString cver = children.at(child);
                    QStringList cver_list = cver.split(QLatin1Char('.'));
                    if(cver_list.size() > 0 && cver_list.size() < 4) {
                        bool ok;
                        int cmaj = -1, cmin = -1, cpat = -1;
                        cmaj = cver_list[0].toInt(&ok);
                        if(!ok || cmaj < 0)
                            continue;
                        if(cver_list.size() >= 2) {
                            cmin = cver_list[1].toInt(&ok);
                            if(!ok)
                                continue;
                            if(cmin < 0)
                                cmin = -1;
                        }
                        if(cver_list.size() >= 3) {
                            cpat = cver_list[2].toInt(&ok);
                            if(!ok)
                                continue;
                            if(cpat < 0)
                                cpat = -1;
                        }
                        if((cmaj >= maj && cmaj <= QT_MAJOR) &&
                           (cmin == -1 || (cmin >= min && cmin <= QT_MINOR)) &&
                           (cpat == -1 || (cpat >= pat && cpat <= QT_PATCH)) &&
                           config->contains(cver + QLatin1Char('/') + key)) {
                            subKey = cver + QLatin1Char('/');
                            maj = cmaj;
                            min = cmin;
                            pat = cpat;
                        }
                    }
                }
            }
            ret = config->value(subKey + key, defaultValue).toString();
            // expand environment variables in the form $(ENVVAR)
            int rep;
            QRegExp reg_var(QLatin1String("\\$\\(.*\\)"));
            reg_var.setMinimal(true);
            while((rep = reg_var.indexIn(ret)) != -1) {
                ret.replace(rep, reg_var.matchedLength(),
                            QString::fromLocal8Bit(qgetenv(ret.mid(rep + 2,
                                reg_var.matchedLength() - 3).toLatin1().constData()).constData()));
            }
            config->endGroup();
        }
    }

    if (QDir::isRelativePath(ret)) {
        if (loc == PrefixPath) {
            // we make the prefix path absolute to the executable's directory
#ifdef QT_BUILD_QMAKE
            return QFileInfo(qmake_libraryInfoFile()).absolutePath();
#else
            if (QCoreApplication::instance()) {
#ifdef Q_OS_MAC
	        CFBundleRef bundleRef = CFBundleGetMainBundle();
		if (bundleRef) {
		    QCFType<CFURLRef> urlRef = CFBundleCopyBundleURL(bundleRef);
		    if (urlRef) {
		        QCFString path = CFURLCopyFileSystemPath(urlRef, kCFURLPOSIXPathStyle);
		        return QDir::cleanPath(path + QLatin1String("/Contents"));
		    }
		}
#endif
                return QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(ret);
            } else {
                return QDir::current().absoluteFilePath(ret);
            }
#endif
        } else {
            // we make any other path absolute to the prefix directory
            return QDir(location(PrefixPath)).absoluteFilePath(ret);
        }
    }
    return ret;
}

/*!
    \enum QLibraryInfo::LibraryLocation

    \keyword library location

    This enum type is used to specify a specific location
    specifier:

    \value PrefixPath The default prefix for all paths.
    \value DocumentationPath The location for documentation upon install.
    \value HeadersPath The location for all headers.
    \value LibrariesPath The location of installed librarires.
    \value BinariesPath The location of installed Qt binaries (tools and applications).
    \value PluginsPath The location of installed Qt plugins.
    \value DataPath The location of general Qt data.
    \value TranslationsPath The location of translation information for Qt strings.
    \value SettingsPath The location for Qt settings.
    \value ExamplesPath The location for examples upon install.
    \value DemosPath The location for demos upon install.

    \sa location()
*/

#endif // QT_NO_SETTINGS

QT_END_NAMESPACE

#if defined(Q_CC_GNU) && defined(Q_OS_LINUX) && !defined(QT_LINUXBASE) && !defined(QT_BOOTSTRAPPED)

#  include <sys/syscall.h>
#  include <unistd.h>

static const char boilerplate[] =
    "This is the QtCore library version " QT_VERSION_STR "\n"
    "Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).\n"
    "Contact: Qt Software Information (qt-info@nokia.com)\n"
    "\n"
    "Build key:           " QT_BUILD_KEY;

extern "C" {
void qt_core_init_boilerplate() __attribute__((noreturn));
}

#  if defined(QT_ARCH_I386)
#define sysinit() (void)0
#define syswrite(msg, len)                                              \
    ({ int res;                                                         \
        asm volatile ("movl  %%ebx, %%edi\n"                            \
                      "movl  $1, %%ebx\n"                               \
                      "int   $0x80\n"                                   \
                      "movl  %%edi, %%ebx\n"                            \
                      : "=a" (res) : "0" (SYS_write), "c" (msg), "d" (len) : "edi"); res; })
#define sysexit(c)                              \
    asm ("xor   %%ebx, %%ebx\n"                 \
         "int   $0x80\n"                        \
         : : "a" (SYS_exit)); _exit(c)

#  elif defined(QT_ARCH_X86_64)
#define sysinit() (void)0
#define syswrite(msg, len)                                              \
    ({ int res;                                                         \
        asm volatile ("syscall\n"                                       \
                      : "=a" (res) : "0" (SYS_write), "D" (1), "S" (msg), "d" (len) : "rcx"); res; })
#define sysexit(c)                              \
    asm ("syscall\n"                            \
         : : "a" (SYS_exit), "D" (0)); _exit(c)

#  elif defined(QT_ARCH_IA64)
#define sysinit()                                               \
    asm volatile ("{.mlx\n"                                     \
                  "      nop.m  0\n"                            \
                  "      movl   r2  = @pcrel(boilerplate);;"    \
                  "}\n"                                         \
                  "{.mii\n"                                     \
                  "      mov   r10 = @ltoffx(boilerplate)\n"    \
                  "      mov   r1  = ip\n"                      \
                  "      adds  r2  = -16, r2\n;;\n"             \
                  "}\n"                                         \
                  "      add   r1  = r2, r1;;\n"                \
                  "      sub   r1  = r1, r10;;\n"               \
                  : : : "r2", "r10")
#define syswrite(msg, len)                                              \
    ({ const char *_msg = msg;                                          \
        asm ("mov    out0=%1\n"                                         \
             "mov    out1=%2\n"                                         \
             "mov    out2=%3\n"                                         \
             ";;\n"                                                     \
             "mov    r15=%0\n"                                          \
             "break  0x100000;;\n"                                      \
             : : "I" (SYS_write), "I" (1), "r" (_msg), "r" (len)); })
#define sysexit(c)                                                      \
    asm ("mov    out0=%1\n"                                             \
         ";;\n"                                                         \
         "mov    r15=%0\n"                                              \
         "break  0x100000;;\n"                                          \
         : : "I" (SYS_exit), "O" (0)); write(1, 0, 0); _exit(c)
# else
#define sysinit() (void)0
#define syswrite(msg, len) (msg); (len)
#define sysexit(c)         __builtin_exit(c)
#  endif

#define sysputs(msg) syswrite(msg, -1 + sizeof(msg))
#define sysendl()    syswrite("\n", 1)
#define print_qt_configure(_which)              \
    ({const char *which = _which;               \
        which += 12;                            \
        int len = 0;                            \
        while (which[len]) ++len;               \
        syswrite(which, len); })

void qt_core_init_boilerplate()
{
    sysinit();
    sysputs(boilerplate);
    sysputs("\nInstallation prefix: ");
    print_qt_configure(qt_configure_prefix_path_str);
    sysputs("\nLibrary path:        ");
    print_qt_configure(qt_configure_libraries_path_str);
    sysputs("\nInclude path:        ");
    print_qt_configure(qt_configure_headers_path_str);
    sysendl();
    sysexit(0);
}

#endif

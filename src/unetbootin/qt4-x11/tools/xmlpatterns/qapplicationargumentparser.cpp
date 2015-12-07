/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the tools applications of the Qt Toolkit.
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

#include <QtDebug>
#include <QTextBoundaryFinder>
#include <QCoreApplication>
#include <QHash>
#include <QPair>
#include <QStringList>
#include <QTextStream>
#include <QUrl>

#include "qapplicationargument_p.h"

#include "qapplicationargumentparser_p.h"

QT_BEGIN_NAMESPACE

/*!
 \class QApplicationArgumentParser
 \brief The QApplicationArgumentParser class parses the command
        line arguments for an application.
 \reentrant
 \internal
 \since 4.4

 QApplicationArgumentParser simplifies writing command line applications by taking care of:

 \list
    \o Generating help and version arguments
    \o Taking care of converting arguments to QVariant types, since each argument
       has a type: QApplicationArgument::type()
    \o Validates the command line such that the user operates on well-defined input. For instance,
       that the argument is a valid integer if that is the case, that an argument does not
       occur more times than allowed, and so on.
    \o Allows customization through sub-classing.
 \endlist

 The user declares what arguments that can be given to the application with QApplicationArgument. Provided
 with that information, QApplicationArgumentParser takes care of parsing the actual
 command line, appropriately flag errors, generate help messages, and provide
 convenient access to the values of the arguments.

 The way to use it is to create a set of QApplicationArgument by ones choosing, call
 addArgument() for each, and subsequently call parse(). If parse() returns \c false,
 the caller should exit and return exitCode().

 If parse() returns \c true the command line was successfully parsed, its
 values are well-defined, and they can be spectated with count(),
 has(), value() and values().

 \snippet doc/src/snippets/code/tools_patternist_qapplicationargumentparser.cpp 0

 For arguments without a name(such as filename passed to the \c ls utility on Linux) add a
 QApplicationArgument that does not have a name. The minimum and maximum occurrences will be
 respected as usual and the type applies too.

 QApplicationArgumentParser always has two options builtin: \c version and \c help.

 \section1 Changing Parsing Convention

 QApplicationArgumentParser by default parses the command line in the style
 of Qt's utilities, where arguments are preceded by a single dash, and identified
 by a single name. However, in some cases it might be of interest to parse
 another style, such as the well-established UNIX \c getopt convention(\c -l
 and \c --long).

 This can be achieved by sub-classing QApplicationArgumentParser and reimplementing
 parse(). It would do the following:

 \list
    \o Call input() to retrieve the strings the user specified on the command line.
    \o Call declaredArguments() to retrieve the arguments that the implementor has
       decided can be specified.
    \o Parse and validate the input. Salt and pepper as per taste.
    \o If an error occurred, call setExitCode() and return \c false.
    \o Otherwise, call setExitCode(Success), provide access to the
       arguments by calling setUsedArguments(), and return \c true. If a
       help message was requested, call setExitCode(Success) and return \c false.
 \endlist

 \sa QApplicationArgument, QCoreApplication
*/
class QApplicationArgumentParserPrivate
{
    Q_DECLARE_TR_FUNCTIONS(QApplicationArgumentParserPrivate)
public:
    // TODO Isn't it like ten times better with QHash<QApplicationArgument, QList<QVariant> >?
    // TODO test QApplicationArgument::nameless()
    typedef QList<QPair<QApplicationArgument, QVariant> > UsedList;

    /*!
     We initialize exitCode to ParseError such that we consciously flag success.
     */
    inline QApplicationArgumentParserPrivate(QApplicationArgumentParser *const master,
                                             const QStringList &aInput) : exitCode(QApplicationArgumentParser::ParseError)
                                                                        , input(aInput)
                                                                        , q_ptr(master)
    {
        Q_ASSERT(!aInput.isEmpty());
    }

    QApplicationArgument nextNamelessArgument() const;
    static QStringList argumentsFromLocal(const int argc, const char *const *const argv);

    bool error(const QString &message);
    static bool errorMessage(const QString &message);
    static inline bool isSwitch(const QApplicationArgument &arg);
    static inline QVariant conversionError(const QString &typeName,
                                           const QString &input);
    int count(const QApplicationArgument &arg) const;
    bool contains(const QApplicationArgument &arg) const;
    static inline bool isBuiltinVariant(const int type);
    void displayVersion() const;
    void displayHelp() const;
    void parseNameless();
    bool parseNamelessArguments(const QString &in);

    QApplicationArgumentParser::ExitCode    exitCode;
    const QStringList                       input;

    /*!
      Since the QString is QApplicationArgument::name() anyway, why
      not use a QSet?
     */
    QHash<QString, QApplicationArgument>    declaredArguments;

    QList<QApplicationArgument>             declaredNamelessArguments;

    UsedList                                usedArguments;
    QString                                 applicationDescription;
    QString                                 applicationVersion;

private:
    QApplicationArgumentParser *const       q_ptr;
    Q_DECLARE_PUBLIC(QApplicationArgumentParser)

    static QString lineWrap(const QString &input,
                            const int leftIndent,
                            const int width);
    static QList<QApplicationArgument> builtinArguments();
};

QApplicationArgument QApplicationArgumentParserPrivate::nextNamelessArgument() const
{
    /* Count how many nameless arguments we have so far. */
    int count = 0;

    for(int i = 0; i < usedArguments.count(); ++i)
    {
        if(usedArguments.at(i).first.isNameless())
            ++count;
    }

    /* TODO this doesn't work for arguments that have more than one
     * mandatory value(e.g nameless ones), since several values should
     * then only count for one argument. */
    for(int i = 0; i < declaredNamelessArguments.count(); ++i)
    {
        if(count)
        {
            /* Skip the ones we already have processed. */
            --count;
            continue;
        }

        if(declaredNamelessArguments.at(i).isNameless())
            return declaredNamelessArguments.at(i);
    }

    return QApplicationArgument();
}

int QApplicationArgumentParserPrivate::count(const QApplicationArgument &arg) const
{
    const int len = usedArguments.count();
    int count = 0;

    for(int i = 0; i < len; ++i)
    {
        if(usedArguments.at(i).first == arg)
            ++count;
    }

    return count;
}

/*!
 Returns \c true if \a arg has appeared on the command line, not whether it has been declared.
 */
bool QApplicationArgumentParserPrivate::contains(const QApplicationArgument &arg) const
{
    const int len = usedArguments.count();

    for(int i = 0; i < len; ++i)
    {
        if(usedArguments.at(i).first == arg)
            return true;
    }

    return false;
}

/*!
 Returns always \c false.
 */
bool QApplicationArgumentParserPrivate::error(const QString &message)
{
    exitCode = QApplicationArgumentParser::ParseError;
    errorMessage(message);
    return errorMessage(tr("Pass -help for information about the command line."));
}

/*!
 Returns always \c false.
 */
bool QApplicationArgumentParserPrivate::errorMessage(const QString &message)
{
    QTextStream out(stderr, QIODevice::WriteOnly);
    out << message << endl;
    return false;
}

/*!
  \internal
  Determines whether \a arg carries a value or is on/off.
 */
bool QApplicationArgumentParserPrivate::isSwitch(const QApplicationArgument &arg)
{
    return arg.type() == QVariant::Invalid;
}

QVariant QApplicationArgumentParserPrivate::conversionError(const QString &typeName,
                                                            const QString &input)
{
    errorMessage(tr("Cannot convert %1 to type %2.").arg(input, typeName));
    return QVariant();
}

bool QApplicationArgumentParserPrivate::isBuiltinVariant(const int type)
{
    return type < int(QVariant::UserType);
}

/*!
  TODO Temporary, replace with a function in QCoreApplication.
*/
QStringList QApplicationArgumentParserPrivate::argumentsFromLocal(const int argc, const char *const *const argv)
{
    Q_ASSERT(argc >= 1);
    Q_ASSERT(argv);
    QStringList result;

    for(int i = 0; i < argc; ++i)
        result.append(QString::fromLocal8Bit(argv[i]));

    return result;
}

void QApplicationArgumentParserPrivate::displayVersion() const
{
    QTextStream out(stderr);

    out << tr("%1 version %2 using Qt %3").arg(QCoreApplication::applicationName(), applicationVersion, qVersion())
        << endl;
}

/*!
 \internal
 \relates QApplicationArgument

 qLess() functor for QApplicationArgument that considers the name.
 */
template<>
class qLess <QApplicationArgument>
{
public:
    inline bool operator()(const QApplicationArgument &o1,
                           const QApplicationArgument &o2) const
    {
        return o1.name().compare(o2.name()) < 0;
    }
};

void QApplicationArgumentParserPrivate::displayHelp() const
{
    enum Constants
    {
        /**
         * When we want to line wrap, 80 minus a couple of characters. This should
         * be suitable for vt100 compatible terminals.
         */
        LineWrapAt = 78,

        /**
         * The initial "  -" for each option.
         */
        IndentPadding = 3,

        /**
         * Pad for the brackets and space we use when we have a type.
         */
        ValueArgumentPadding = 4
    };

    QList<QApplicationArgument> args(declaredArguments.values());
    args += builtinArguments();

    /* Sort them, such that we get the nameless options at the end, and it
     * generally looks tidy. */
    qSort(args);

    /* This is the basic approach:
     * Switches:
     *  -name description
     * Value arguments:
     *  -name <name-of-value-type> description
     *
     * Nameless arguments
     *  name <type> description
     *
     * It all line-wraps at OutputWidth and the description is indented,
     * where the highest indent is the length of the name plus length of the name
     * of the type. */

    /* First we find the name with the largest width. */
    int maxWidth = 0;

    QList<QApplicationArgument> nameless(declaredNamelessArguments);
    qSort(nameless);

    /* Note, here the nameless arguments appear last, but are sorted
     * with themselves. */
    QList<QApplicationArgument> allArgs(args + nameless);
    const int allArgsCount = allArgs.count();

    for(int i = 0; i < allArgsCount; ++i)
    {
        const QApplicationArgument &at = allArgs.at(i);
        const int nameLength = at.name().length();
        const QString typeName(q_ptr->typeToName(at));
        const int typeNameLength = typeName.length();
        const int padding = at.type() == QVariant::Invalid ? 0 : ValueArgumentPadding;
        maxWidth = qMax(maxWidth, nameLength + typeNameLength + padding);
    }

    QTextStream out(stderr);
    out << endl
        << QString(IndentPadding, QLatin1Char(' '))
        << QCoreApplication::applicationName()
        << QLatin1String(" -- ")
        << applicationDescription
        << endl;
    // TODO synopsis

    /* One extra so we get some space between the overview and the options. */
    out << endl;

    const int indentWidth = maxWidth + 3;

    /* Ok, print them out. */
    for(int i = 0; i < allArgsCount; ++i)
    {
        const QApplicationArgument &at = allArgs.at(i);
        /* "  -name ". Indent a bit first, inspired by Qt's moc. */
        const QString &name = at.name();
        QString prolog(QLatin1String("  "));
       
        /* We have a special case for the single dash. */
        if(name == QChar::fromLatin1('-'))
            prolog.append(name);
        else
        {
            if(!at.isNameless())
                prolog.append(QLatin1Char('-'));

             prolog.append(name + QLatin1Char(' '));
        }

        if(at.type() != QVariant::Invalid)
        {
            /* It's not a switch, it has a value. */

            /* Do we have a default value? If so, the argument is optional. */
            const QString typeName(q_ptr->typeToName(at));

            if(at.defaultValue().isValid())
                prolog.append(QLatin1Char('[') + typeName + QLatin1Char(']'));
            else
                prolog.append(QLatin1Char('<') + typeName + QLatin1Char('>'));
            // TODO Don't we want to display the default value?
            
            prolog.append(QLatin1Char(' '));
        }

        prolog = prolog.leftJustified(indentWidth);

        out << prolog
            << lineWrap(at.description(), indentWidth, LineWrapAt)
            << endl;
    }
}

/*!
 Line wraps \a input and indents each line with \a leftIndent spaces, such that
 the width does not go beyond \a maxWidth.

 The addition of line endings is accounted for by the caller.

 With QTextBoundaryFinder our line wrapping is relatively fancy, since it
 does it the Unicode-way.
 */
QString QApplicationArgumentParserPrivate::lineWrap(const QString &input,
                                                    const int leftIndent,
                                                    const int maxWidth)
{
    const QString indent(QString(leftIndent, QLatin1Char(' ')));
    const int len = input.length();
    const int textWidth = maxWidth - leftIndent;

    QString output;
    QTextBoundaryFinder wrapFinder(QTextBoundaryFinder::Line, input);
    wrapFinder.setPosition(textWidth);

    if(input.length() + leftIndent <= maxWidth)
        return input;

    int from = wrapFinder.toPreviousBoundary();
    output.append(input.left(from));

    while(true)
    {
        if((len - from) + leftIndent > maxWidth)
        {
            /* We need to line wrap. */
            wrapFinder.setPosition(from + textWidth);
            const int currentWidthPos = wrapFinder.toPreviousBoundary();

            output.append(QLatin1Char('\n'));
            output.append(indent);
            output.append(input.mid(from, currentWidthPos - from).trimmed());
            from += (currentWidthPos - from);
        }
        else
        {
            /* Append the remains.  */
            output.append(QLatin1Char('\n'));
            output.append(indent);
            output.append(input.mid(from).trimmed());
            break;
        }
    }

    return output;
}

/*!
 Returns a list with the builtin options that the parser has
 */
QList<QApplicationArgument> QApplicationArgumentParserPrivate::builtinArguments()
{
    QList<QApplicationArgument> result;

    result.append(QApplicationArgument(QLatin1String("help"),
                                       QLatin1String("Displays this help.")));
    result.append(QApplicationArgument(QLatin1String("version"),
                                       QLatin1String("Displays version information.")));
    
    result.append(QApplicationArgument(QLatin1String("-"),
                                       QLatin1String("When appearing, any following options are not interpreted as switches.")));
    return result;
}

/* TODO, I don't think we want this function in a public API. Add it first when there is a demand. */

/*!
 Creates a QApplicationArgumentParser that will parse the input in \a argc and \a argv.
These arguments should be passed directly from the \c main() function, and the decoding
of the input will be taken care of appropriately, depending on platform.

 It is preferred to use the QStringList overload, in case the input is in the form of QStrings.
 */
QApplicationArgumentParser::QApplicationArgumentParser(int argc, char **argv) : d(new QApplicationArgumentParserPrivate(this, QApplicationArgumentParserPrivate::argumentsFromLocal(argc, argv)))
{
    Q_ASSERT_X(argv, Q_FUNC_INFO, "Argv cannot be null.");
    Q_ASSERT_X(argc >= 1, Q_FUNC_INFO,
               "argc must at least contain the application name. "
               "Use the QStringList overload instead.");
}

/*!
 \overload

 Creates a QApplicationArgumentParser that will parse \a input. That is, instead of passing in \c argc
 and \c argv, one can pass in a QStringList.

 The caller guarantees that the first string in \a input is the name of the application.
 */
QApplicationArgumentParser::QApplicationArgumentParser(const QStringList &input) : d(new QApplicationArgumentParserPrivate(this, input))
{
    Q_ASSERT_X(input.count() >= 1, Q_FUNC_INFO,
               "The input must at least contain the application name.");
}

/*!
 This function is only of interest when subclassing.

 Returns the strings that the user specified when starting the application. The first string
 in the list is always the application name.
 */
QStringList QApplicationArgumentParser::input() const
{
    Q_ASSERT_X(d->input.count() >= 1, Q_FUNC_INFO, "Internal error, this should always hold true");
    return d->input;
}

/*!
 This function is only of interest when subclassing.

 Sets the arguments that the user actually used on the command line to \a arguments.
 The parse() function should call this, such that the result afterwards can be inspected
 with for instance has() or count().

\sa usedArguments()
*/
void QApplicationArgumentParser::setUsedArguments(const QList<QPair<QApplicationArgument, QVariant> > &arguments)
{
    d->usedArguments = arguments;
}

/*!
 This function is only of interest when subclassing.

 Returns the arguments that the user used on the command line.

\sa setUsedArguments()
*/
QList<QPair<QApplicationArgument, QVariant> > QApplicationArgumentParser::usedArguments() const
{
    return d->usedArguments;
}

/*!
  Destructs this QApplicationArgumentParser instance.
 */
QApplicationArgumentParser::~QApplicationArgumentParser()
{
    delete d;
}

/*!
  Adds \a argument to this parser.

  This function is provided for convenience. It is equivalent to creating a QList
  containing \a argument, append the existing arguments, and then call setDeclaredArguments() with the list.

  \sa setDeclaredArguments()
 */
void QApplicationArgumentParser::addArgument(const QApplicationArgument &argument)
{
    if(argument.isNameless())
        d->declaredNamelessArguments.append(argument);
    else
        d->declaredArguments.insert(argument.name(), argument);
}

/*!
 Makes the parser recognize all arguments in \a arguments.

 Any arguments previously set, are discarded.

 \sa addArgument(), declaredArguments()
 */
void QApplicationArgumentParser::setDeclaredArguments(const QList<QApplicationArgument> &arguments)
{
    // TODO If we have a QHash internally, why not use it in the public API too?
    const int len = arguments.count();

    for(int i = 0; i < len; ++i)
        d->declaredArguments.insert(arguments.at(i).name(), arguments.at(i));
}

/*!
 Returns the arguments that this parser recognizes.

 \sa addArgument(), setDeclaredArguments()
 */
QList<QApplicationArgument> QApplicationArgumentParser::declaredArguments() const
{
    return d->declaredArguments.values();
}

bool QApplicationArgumentParserPrivate::parseNamelessArguments(const QString &in)
{
    /* It's a nameless options, such as simply "value". */
    const QApplicationArgument nameless(nextNamelessArgument());

    const QVariant val(q_ptr->convertToValue(nameless, in));
    if(val.isValid())
    {
        usedArguments.append(qMakePair(nameless, val));
        return true;
    }
    else
        return false; // TODO error msg?
}

/*!
 Parses input() together with declaredArguments() and returns \c false if the caller 
 should exit immediately, which is the case of which an error was encountered or
 help or the version was requested.

 In the case of \c true was returned, valid arguments were supplied, and they can
 be requested with functions like value(), values(), count() and has().

 parse() must only be called once per QApplicationArgumentParser instance. The
 second time it's called, the effects and return value are undefined.

 \sa convertToValue(), typeToName()
 */
bool QApplicationArgumentParser::parse()
{
    const QChar sep(QLatin1Char('-'));
    const int inputCount = d->input.count();

    /* We skip the first entry, which is the application name. */
    int i = 1;

    for(; i < inputCount; ++i)
    {
        const QString &in = d->input.at(i);

        /* We have a single '-', signalling that the succeeding are not options. */
        if(in == sep)
        {
            ++i;

            for(; i < inputCount; ++i)
            {
                if(!d->parseNamelessArguments(d->input.at(i)))
                    return false;
                /* Process nameless options. Have code for this elsewhere, factor it out. */
            }

            break;
        }

        if(in.startsWith(sep)) /* It is "-name". */
        {
            const QString name(in.mid(1));

            if(name == QLatin1String("help"))
            {
                setExitCode(Success);
                d->displayHelp();
                return false;
            }
            else if(name == QLatin1String("version"))
            {
                setExitCode(Success);
                d->displayVersion();
                return false;
            }

            if(!d->declaredArguments.contains(name))
                return d->error(QApplicationArgumentParserPrivate::tr("\"%1\" is an unknown argument.").arg(name));

            const QApplicationArgument &arg = d->declaredArguments.value(name);
            const int argCount = d->count(arg) + 1;
            const int max = arg.maximumOccurrence();

            if(argCount > max && max != -1)
            {
                /* Let's tailor the message for a common case. */
                if(max == 1)
                    return d->error(QApplicationArgumentParserPrivate::tr("\"%1\" can only be used once.").arg(name));
                else
                    return d->error(QApplicationArgumentParserPrivate::tr("\"%1\" can only be used %2 times.").arg(name, QString::number(max)));
            }

            if(QApplicationArgumentParserPrivate::isSwitch(arg))
            {
                d->usedArguments.append(qMakePair(arg, QVariant()));
                continue;
            }
            else
            {
                ++i;

                if(i == inputCount)
                    return d->error(QApplicationArgumentParserPrivate::tr("\"%1\" must be followed by a value.").arg(name));

                /* Okidoki, got a value, always something. Let's
                 * see if it validates. */
                const QString &value = d->input.at(i);

                const QVariant val(convertToValue(arg, value));
                if(val.isValid())
                {
                    d->usedArguments.append(qMakePair(arg, val));
                    continue;
                }
                else
                    return false; // TODO error msg?
            }
        }
        else
        {
            if(!d->parseNamelessArguments(in))
                return false;
        }
    }

    /* Check that all arguments that have been declared as mandatory, are actually
     * specified. */
    const QList<QApplicationArgument> declaredArguments(d->declaredArguments.values() + d->declaredNamelessArguments);
    const int len = declaredArguments.count();
    for(int i = 0; i < len; ++i)
    {
        const QApplicationArgument &at = declaredArguments.at(i);
        const int min = at.minimumOccurrence();
        const int max = at.maximumOccurrence(); // TODO What about infinite? -1
        if(min == 0)
            continue;
        else
        {
            const int usedLen = d->usedArguments.count();
            int useCount = 0;

            for(int u = 0; u < usedLen; ++u)
            {
                const QPair<QApplicationArgument, QVariant> &used = d->usedArguments.at(u);
                if(used.first == at)
                    ++useCount;
            }

            const QString originalName(at.name());
            const QString effectiveName(originalName.isEmpty() ? QLatin1Char('<') + typeToName(at) + QLatin1Char('>') : originalName);

            if(useCount < min)
            {
                /* For nameless options, we use the type as the name. Looks better. */
                return d->error(QApplicationArgumentParserPrivate::tr("%1 must occur at least %2 times, therefore %3 times is insufficient.", "The number is for %2.", min)
                                                                      .arg(effectiveName, QString::number(min), QString::number(useCount)));
            }
            else if(useCount > max)
                return d->error(QApplicationArgumentParserPrivate::tr("%1 can occur at most %2 times", "", max).arg(effectiveName, QString::number(max)));
        }
    }

    d->exitCode = Success;
    return true;
}

/*!
 This function is only of interest when subclassing.

 parse() calls this function each time a value, that is \a input, on the command line needs to be
 validated and subsequently converted to the type of \a argument. A descriptive error message will
 be outputted if \a input cannot be converted to the required type.

 The default implementation uses QVariant::canConvert() and QVariant::convert() for doing conversions.

 QApplicationArgumentParser can be subclassed and this function subsequently overridden, to handle custom types.

 If \a input isn't valid input for \a argument, this function returns a default constructed
 QVariant.

 \sa typeToName(), parse()
 */
QVariant QApplicationArgumentParser::convertToValue(const QApplicationArgument &argument,
                                                    const QString &input) const
{
    const int type = argument.type();

    switch(type)
    {
        case QVariant::Bool:
        {
            if(input == QLatin1String("true") || input == QChar::fromLatin1('1'))
                return QVariant(true);
            else if(input == QLatin1String("false") || input == QChar::fromLatin1('0'))
                return QVariant(false);
            else
                return QApplicationArgumentParserPrivate::conversionError(typeToName(argument), input);
        }
        case QVariant::RegExp:
        {
            const QRegExp exp(input);

            if(exp.isValid())
                return QVariant(exp);
            else
                return QApplicationArgumentParserPrivate::conversionError(typeToName(argument), input);
        }
        case QVariant::Url:
        {
            const QUrl result(QUrl::fromEncoded(input.toLatin1()));

            if(result.isValid())
                return QVariant(result);
            else
                return QApplicationArgumentParserPrivate::conversionError(typeToName(argument), input);
        }
        default:
        {
            QVariant result(input);

            if(QApplicationArgumentParserPrivate::isBuiltinVariant(type) &&
               result.convert(QVariant::Type(type)))
                return result;
            else
                return QApplicationArgumentParserPrivate::conversionError(typeToName(argument), input);
        }
    }
}

/*!
 This function is only of interest when subclassing.

  convertToValue() calls this function when requiring a string for referring to \a type,
  when generating user messages.

  The implementation uses QVariant::typeToName() for most types, but special handles
  some types, in order to let the message be better tailored for humans.

 \sa convertToValue()
 */
QString QApplicationArgumentParser::typeToName(const QApplicationArgument &argument) const
{
    /* Personally I think nameForType() would be a better name but this is consistent
     * with QVariant's function of the same name. */
    const int type = argument.type();

    switch(type)
    {
        case QVariant::RegExp:
            return QApplicationArgumentParserPrivate::tr("regular expression");
        case QVariant::Url:
            return QLatin1String("URI");
        case QVariant::String:
            return QLatin1String("string");
        default:
        {
            if(QApplicationArgumentParserPrivate::isBuiltinVariant(type))
                return QString::fromLatin1(QVariant::typeToName(QVariant::Type(type)));
            else
                return QLatin1String(QVariant(type, static_cast<void *>(0)).typeName());
        }
    }
}

/*!
 Returns the default value for \a argument. The default implementation returns
 QApplicationArgument::defaultValue(), if \a argument has been added to this parser.

 Overriding this function can be useful if creating the default value is resource
 consuming, such as opening a file.
 */
QVariant QApplicationArgumentParser::defaultValue(const QApplicationArgument &argument) const
{
    return d->declaredArguments.value(argument.name()).defaultValue();
}

/*!
 Returns the count of how many times \a argument was used on the command line.

 \sa has()
 */
int QApplicationArgumentParser::count(const QApplicationArgument &argument) const
{
    Q_ASSERT_X(d->declaredArguments.contains(argument.name()) ||
               d->declaredNamelessArguments.contains(argument), Q_FUNC_INFO,
               "The argument isn't known to the parser. Has addArgument() been called?");
    return d->count(argument);
}

/*!
 Returns \c true if \a argument has been
 specified one or more times on the command line, otherwise \a false.

 \sa count()
 */
bool QApplicationArgumentParser::has(const QApplicationArgument &argument) const
{
    Q_ASSERT_X(d->declaredArguments.contains(argument.name()) ||
               d->declaredNamelessArguments.contains(argument), Q_FUNC_INFO,
               "The argument isn't known to the parser. Has addArgument() been called?");
    return d->contains(argument);
}

/*!
  // TODO docs

 \sa values()
 */
QVariant QApplicationArgumentParser::value(const QApplicationArgument &argument) const
{
    Q_ASSERT_X(d->declaredArguments.contains(argument.name()) ||
               d->declaredNamelessArguments.contains(argument), Q_FUNC_INFO,
               "The argument isn't known to the parser. Has addArgument() been called?");

    const int len = d->usedArguments.count();

    for(int i = 0; i < len; ++i)
    {
        if(d->usedArguments.at(i).first == argument)
            return d->usedArguments.at(i).second;
    }

    return defaultValue(argument);
}

/*!
  // TODO docs
 \sa value()
 */
QVariantList QApplicationArgumentParser::values(const QApplicationArgument &argument) const
{
    Q_ASSERT_X(d->declaredArguments.contains(argument.name()) ||
               d->declaredNamelessArguments.contains(argument),
               Q_FUNC_INFO,
               "The argument isn't known to the parser. Has addArgument() been called?");

    const int len = d->usedArguments.count();

    QVariantList result;
    for(int i = 0; i < len; ++i)
    {
        if(d->usedArguments.at(i).first == argument)
            result.append(d->usedArguments.at(i).second);
    }

    // TODO how do we handle default values?
    return result;
}

/*!
 After parse() has been called, this function returns a code that can be used to
 exit \c main() with. It returns zero upon success or if help was requested, and
 otherwise a value signalling failure.
 */
QApplicationArgumentParser::ExitCode QApplicationArgumentParser::exitCode() const
{
    return d->exitCode;
}

/*!
 This function is only of interest when subclassing.

 Makes exitCode() return \a code.
 */
void QApplicationArgumentParser::setExitCode(ExitCode code)
{
    d->exitCode = code;
}

/*!
 Sets the application description to \a description.

 The application description is a sentence or two used for help and version
 messages, that briefly describes the application.

 The default is the empty string.
 */
void QApplicationArgumentParser::setApplicationDescription(const QString &description)
{
    d->applicationDescription = description;
}

/*!
 Sets the application version to \a version.

 This string, which is arbitrary but typically is "1.0" or so, is used when
 generating a version statement.
*/
void QApplicationArgumentParser::setApplicationVersion(const QString &version)
{
    d->applicationVersion = version;
}

/*!
 Writes out \a message to \c stderr.
 */
void QApplicationArgumentParser::message(const QString &message) const
{
    d->errorMessage(message);
}

QT_END_NAMESPACE

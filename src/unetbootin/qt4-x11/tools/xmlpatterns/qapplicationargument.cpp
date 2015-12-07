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

#include <QHash>
#include <QString>

#include "qapplicationargument_p.h"

QT_BEGIN_NAMESPACE

/*!
 \class QApplicationArgument
 \brief The QApplicationArgument class is a declared of a command line
        argument for QApplicationArgumentParser.
 \reentrant
 \internal
 \since 4.4

 QApplicationArgument describes a valid command line argument,
 by having a set of characteristics:

 \table
   \header
        \o Characteristic
        \o Functions
    \row
        \o A name. For instance, "backend"
        \o setName() and name()
    \row
        \o A description, for human consumption.
        \o setDescription() and description()
    \row
        \o How many times the argument can occur. For instance, whether the argument is optional or not.
        \o setMinimumOccurrence() & minimumOccurrence(), setMaximumOccurrence() & maximumOccurrence()
    \row
        \o The type of the argument's value, if it has one. For instance, \c int or \c bool.
        \o setType() and type()
    \row
        \o The value that should be used in case the argument isn't used.
        \o setDefaultValue() and defaultValue()
 \endtable

 \sa QApplicationArgumentParser
 */

class QApplicationArgumentPrivate
{
public:
    inline QApplicationArgumentPrivate(const QString &newName,
                                       const QString &desc,
                                       const int newType) : name(newName)
                                                          , description(desc)
                                                          , type(newType)
                                                          , minimum(0)
                                                          , maximum(1)
                                                          , isNameless(false)
    {
    }

    QString         name;
    QString         description;
    int             type;
    QVariant        defaultValue;
    int             minimum;
    int             maximum;
    bool            isNameless;
};

/*!
  Constructs an invalid QApplicationArgument instance.
 */
QApplicationArgument::QApplicationArgument() : d_ptr(new QApplicationArgumentPrivate(QString(), QString(), QVariant::Invalid))
{
}

/*!
 Constructs an QApplicationArgument instance that is a copy of \a other.
 */
QApplicationArgument::QApplicationArgument(const QApplicationArgument &other) : d_ptr(new QApplicationArgumentPrivate(*other.d_ptr))
{
}

/*!
 Destructs this QApplicationArgument instance.
 */
QApplicationArgument::~QApplicationArgument()
{
    delete d_ptr;
}

/*!
 Constructs an argument that has the name \a name and is of type
 \a aType.

 Calling this constructor is equivalent to calling setName() and setType()
 on a default constructed QApplicationArgument instance.

 \sa setName(), setType()
 */
QApplicationArgument::QApplicationArgument(const QString &name,
                                           const QString &description,
                                           int aType) : d_ptr(new QApplicationArgumentPrivate(name, description, aType))
{
}

/*!
 Assigns \a other to this QApplicationArgument instance.
 */
QApplicationArgument &QApplicationArgument::operator=(const QApplicationArgument &other)
{
    if(this != &other)
        *d_ptr = *other.d_ptr;

    return *this;
}

// TODO is this really what we want?
/*!
 Returns true if this QApplicationArgument instance is equal to \a other.

 Equalness is defined to only consider name(). If for instance the type() differs
 but the names are equal, this operator will return \c true.
 */
bool QApplicationArgument::operator==(const QApplicationArgument &other) const
{
    return name() == other.name();
}

/*!
  \fn qHash(const QApplicationArgument &);
  \internal

  Returns a hash index of \a argument. This function is used when QApplicationArgument
  is used with QHash.

  The hash index is computed on name(). The other properties are ignored.

 \relates QApplicationArgument
 */

/*!
 Sets this argument's name to \a newName. The name does not
 include any dash, or other prefix that is used by the parser.
 */
void QApplicationArgument::setName(const QString &newName)
{
    d_ptr->name = newName;
}

/*!
 Returns the name that this argument has.

 \sa setName()
 */
QString QApplicationArgument::name() const
{
    return d_ptr->name;
}

/*!
 Sets the tupe to \a newType.

 If \a newType is QVariant::Invalid, it signals that this
 argument does not accept a value at all.

 \a newType can be a QVariant::type() value, or QVariant::userType().

 \sa type()
 */
void QApplicationArgument::setType(int newType)
{
    d_ptr->type = newType;
}

/*!
 Returns the type that the value of this argument has. If it
 is QVariant::Invalid, it means this argument cannot have a value
 and is a switch only.

 The type is by default QVariant::Invalid.
\sa setType()
 */
int QApplicationArgument::type() const
{
    return d_ptr->type;
}

void QApplicationArgument::setDefaultValue(const QVariant &value)
{
    d_ptr->defaultValue = value;
}

QVariant QApplicationArgument::defaultValue() const
{
    return d_ptr->defaultValue;
}

/*!
 Sets the minimum amount of times this argument can occur, to \a minimum.
 For instance, if \a minimum is 2, the argument must be used at least two times.

 If \a minimum is zero, it means the argument is optional.

 \sa minimumOccurrence(), setMaximumOccurrence()
 */
void QApplicationArgument::setMinimumOccurrence(int minimum)
{
    Q_ASSERT_X(minimum >= 0, Q_FUNC_INFO,
           "The minimum cannot be less than zero.");
    d_ptr->minimum = minimum;
}

/*!
 Returns the minimum amount of times an an argument must occur.

 The default is 0.

 \sa setMinimumOccurrence(), maximumOccurrence()
 */
int QApplicationArgument::minimumOccurrence() const
{
    return d_ptr->minimum;
}

/*!
 Sets the maximum occurrence to \a maximum.

 If \a maximum is -1, it means the argument can appear an unlimited
 amount of times. Setting it to zero or less than -1, yields
 undefined behavior.

\sa maximumOccurrence(), setMinimumOccurrence()
 */
void QApplicationArgument::setMaximumOccurrence(int maximum)
{
    Q_ASSERT_X(maximum == -1 || maximum >= 1, Q_FUNC_INFO,
           "The maximum can only be -1 or 1 or larger.");
    d_ptr->maximum = maximum;
}

/*!
 Returns the maximum amount of times this argument can occur. For instance,
 if the maximum occurrence is 2, it would be an error if 3 were specified
 on the command line.

 If the maximum occurrence is -1, it signals the argument can appear an unlimited
 amount of times.

 The default is 1.

 \sa setMaximumOccurrence()
 */
int QApplicationArgument::maximumOccurrence() const
{
    return d_ptr->maximum;
}

/*!
 Sets the description to \a newDescription. The description
 should describe the argument in a sentence or two. It is used
 when displaying a help message, if requested.

 The description should be terminated as if it was a paragraph. This
 typically means a period.

 The description should be translated by wrapping the
 string literal in a call to tr().

 */
void QApplicationArgument::setDescription(const QString &newDescription)
{
    d_ptr->description = newDescription;
}

/*!
 Returns the description of this argument.

 \sa setDescription()
 */
QString QApplicationArgument::description() const
{
    return d_ptr->description;
}

/*!
 \internal
 \relates QApplicationArgument

 Computes a hash key on \a argument's name and returns it.
 */
uint qHash(const QApplicationArgument &argument)
{
    return qHash(argument.name());
}

void QApplicationArgument::setNameless(bool value)
{
    d_ptr->isNameless = value;
}

bool QApplicationArgument::isNameless() const
{
    return d_ptr->isNameless;
}

QT_END_NAMESPACE

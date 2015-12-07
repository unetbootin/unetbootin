/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Qt Assistant of the Qt Toolkit.
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

#include "qhelpsearchquerywidget.h"

#include <QtCore/QDebug>

#include <QtCore/QObject>
#include <QtCore/QStringList>

#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QFocusEvent>
#include <QtGui/QPushButton>
#include <QtGui/QToolButton>

QT_BEGIN_NAMESPACE

class QHelpSearchQueryWidgetPrivate : public QObject
{
    Q_OBJECT

private:
    QHelpSearchQueryWidgetPrivate()
        : QObject()
    {
        searchButton = 0;
        advancedSearchWidget = 0;
        showHideAdvancedSearchButton = 0;
        defaultQuery = 0;
        exactQuery = 0;
        similarQuery = 0;
        withoutQuery = 0;
        allQuery = 0;
        atLeastQuery = 0;
    }

    ~QHelpSearchQueryWidgetPrivate()
    {
        // nothing todo
    }

    QString escapeString(const QString &text)
    {
        QString retValue = text;
        const QString escape(QLatin1String("\\"));
        QStringList escapableCharsList;
        escapableCharsList << QLatin1String("\\") << QLatin1String("+")
            << QLatin1String("-") << QLatin1String("!") << QLatin1String("(")
            << QLatin1String(")") << QLatin1String(":") << QLatin1String("^")
            << QLatin1String("[") << QLatin1String("]") << QLatin1String("{")
            << QLatin1String("}") << QLatin1String("~");

        // make sure we won't end up with an empty string
        foreach (const QString escapeChar, escapableCharsList) {
            if (retValue.contains(escapeChar))
                retValue.replace(escapeChar, QLatin1String(""));
        }
        if (retValue.trimmed().isEmpty())
            return retValue;

        retValue = text; // now realy escape the string...
        foreach (const QString escapeChar, escapableCharsList) {
            if (retValue.contains(escapeChar))
                retValue.replace(escapeChar, escape + escapeChar);
        }
        return retValue;
    }

    QStringList buildTermList(const QString query)
    {
        bool s = false;
        QString phrase;
        QStringList wordList;
        QString searchTerm = query;

        for (int i = 0; i < searchTerm.length(); ++i) {
            if (searchTerm[i] == QLatin1Char('\"') && !s) {
                s = true;
                phrase = searchTerm[i];
                continue;
            }
            if (searchTerm[i] != QLatin1Char('\"') && s)
                phrase += searchTerm[i];
            if (searchTerm[i] == QLatin1Char('\"') && s) {
                s = false;
                phrase += searchTerm[i];
                wordList.append(phrase);
                searchTerm.remove(phrase);
            }
        }
        if (s)
            searchTerm.replace(phrase, phrase.mid(1));

        const QRegExp exp(QLatin1String("\\s+"));
        wordList += searchTerm.split(exp, QString::SkipEmptyParts);
        return wordList;
    }

private slots:
    void showHideAdvancedSearch()
    {
        bool hidden = advancedSearchWidget->isHidden();
        if (hidden) {
            advancedSearchWidget->show();
            showHideAdvancedSearchButton->setText((QLatin1String("-")));
        } else {
            advancedSearchWidget->hide();
            showHideAdvancedSearchButton->setText((QLatin1String("+")));
        }

        defaultQuery->setEnabled(!hidden);
    }

private:
    friend class QHelpSearchQueryWidget;

    QPushButton *searchButton;
    QWidget* advancedSearchWidget;
    QToolButton *showHideAdvancedSearchButton;
    QLineEdit *defaultQuery;
    QLineEdit *exactQuery;
    QLineEdit *similarQuery;
    QLineEdit *withoutQuery;
    QLineEdit *allQuery;
    QLineEdit *atLeastQuery;
};

#include "qhelpsearchquerywidget.moc"


/*!
    \class QHelpSearchQueryWidget
    \since 4.4
    \inmodule QtHelp
    \brief The QHelpSearchQueryWidget class provides a simple line edit or
    an advanced widget to enable the user to input a search term in a
    standardized input mask.
*/

/*!
    \fn void QHelpSearchQueryWidget::search()

    This signal is emitted when a the user has the search button invoked.
    After reciving the signal you can ask the QHelpSearchQueryWidget for the build list
    of QHelpSearchQuery's that you may pass to the QHelpSearchEngine's search() function.
*/

/*!
    Constructs a new search query widget with the given \a parent.
*/
QHelpSearchQueryWidget::QHelpSearchQueryWidget(QWidget *parent)
    : QWidget(parent)
{
    d = new QHelpSearchQueryWidgetPrivate();

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setMargin(0);

    QHBoxLayout* hBoxLayout = new QHBoxLayout();
    QLabel *label = new QLabel(tr("Search for:"), this);
    d->defaultQuery = new QLineEdit(this);
    d->searchButton = new QPushButton(tr("Search"), this);
    hBoxLayout->addWidget(label);
    hBoxLayout->addWidget(d->defaultQuery);
    hBoxLayout->addWidget(d->searchButton);

    vLayout->addLayout(hBoxLayout);

    connect(d->searchButton, SIGNAL(clicked()), this, SIGNAL(search()));
    connect(d->defaultQuery, SIGNAL(returnPressed()), this, SIGNAL(search()));

#if defined(QT_CLUCENE_SUPPORT)
    hBoxLayout = new QHBoxLayout();
    d->showHideAdvancedSearchButton = new QToolButton(this);
    d->showHideAdvancedSearchButton->setText(QLatin1String("+"));
    d->showHideAdvancedSearchButton->setMinimumSize(25, 20);

    label = new QLabel(tr("Advanced search"), this);
    QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    sizePolicy.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
    label->setSizePolicy(sizePolicy);

    QFrame* hLine = new QFrame(this);
    hLine->setFrameStyle(QFrame::HLine);
    hBoxLayout->addWidget(d->showHideAdvancedSearchButton);
    hBoxLayout->addWidget(label);
    hBoxLayout->addWidget(hLine);

    vLayout->addLayout(hBoxLayout);

    // setup advanced search layout
    d->advancedSearchWidget = new QWidget(this);
    QGridLayout *gLayout = new QGridLayout(d->advancedSearchWidget);
    gLayout->setMargin(0);

    label = new QLabel(tr("words <B>similar</B> to:"), this);
    gLayout->addWidget(label, 0, 0);
    d->similarQuery = new QLineEdit(this);
    gLayout->addWidget(d->similarQuery, 0, 1);

    label = new QLabel(tr("<B>without</B> the words:"), this);
    gLayout->addWidget(label, 1, 0);
    d->withoutQuery = new QLineEdit(this);
    gLayout->addWidget(d->withoutQuery, 1, 1);

    label = new QLabel(tr("with <B>exact phrase</B>:"), this);
    gLayout->addWidget(label, 2, 0);
    d->exactQuery = new QLineEdit(this);
    gLayout->addWidget(d->exactQuery, 2, 1);

    label = new QLabel(tr("with <B>all</B> of the words:"), this);
    gLayout->addWidget(label, 3, 0);
    d->allQuery = new QLineEdit(this);
    gLayout->addWidget(d->allQuery, 3, 1);

    label = new QLabel(tr("with <B>at least one</B> of the words:"), this);
    gLayout->addWidget(label, 4, 0);
    d->atLeastQuery = new QLineEdit(this);
    gLayout->addWidget(d->atLeastQuery, 4, 1);

    vLayout->addWidget(d->advancedSearchWidget);
    d->advancedSearchWidget->hide();

    connect(d->exactQuery, SIGNAL(returnPressed()), this, SIGNAL(search()));
    connect(d->similarQuery, SIGNAL(returnPressed()), this, SIGNAL(search()));
    connect(d->withoutQuery, SIGNAL(returnPressed()), this, SIGNAL(search()));
    connect(d->allQuery, SIGNAL(returnPressed()), this, SIGNAL(search()));
    connect(d->atLeastQuery, SIGNAL(returnPressed()), this, SIGNAL(search()));
    connect(d->showHideAdvancedSearchButton, SIGNAL(clicked()),
        d, SLOT(showHideAdvancedSearch()));
#endif
}

/*!
    Destroys the search query widget.
*/
QHelpSearchQueryWidget::~QHelpSearchQueryWidget()
{
    delete d;
}

/*!
    Returns a list of querys to use in combination with the search engines
    search(QList<QHelpSearchQuery> &query) function.
*/
QList<QHelpSearchQuery> QHelpSearchQueryWidget::query() const
{
#if !defined(QT_CLUCENE_SUPPORT)
    QList<QHelpSearchQuery> queryList;
    queryList.append(QHelpSearchQuery(QHelpSearchQuery::DEFAULT,
        QStringList(d->defaultQuery->text())));

    return queryList;
#else
    QList<QHelpSearchQuery> queryList;
    if (d->defaultQuery->isEnabled()) {
        queryList.append(QHelpSearchQuery(QHelpSearchQuery::DEFAULT,
            d->buildTermList(d->escapeString(d->defaultQuery->text()))));
    } else {
        const QRegExp exp(QLatin1String("\\s+"));
        QStringList lst = d->similarQuery->text().split(exp, QString::SkipEmptyParts);
        if (!lst.isEmpty()) {
            QStringList fuzzy;
            foreach (const QString term, lst)
                fuzzy += d->buildTermList(d->escapeString(term));
            queryList.append(QHelpSearchQuery(QHelpSearchQuery::FUZZY, fuzzy));
        }

        lst = d->withoutQuery->text().split(exp, QString::SkipEmptyParts);
        if (!lst.isEmpty()) {
            QStringList without;
            foreach (const QString term, lst)
                without.append(d->escapeString(term));
            queryList.append(QHelpSearchQuery(QHelpSearchQuery::WITHOUT, without));
        }

        if (!d->exactQuery->text().isEmpty()) {
            QString phrase = d->exactQuery->text().remove(QLatin1Char('\"'));
            phrase = d->escapeString(phrase.simplified());
            queryList.append(QHelpSearchQuery(QHelpSearchQuery::PHRASE, QStringList(phrase)));
        }

        lst = d->allQuery->text().split(exp, QString::SkipEmptyParts);
        if (!lst.isEmpty()) {
            QStringList all;
            foreach (const QString term, lst)
                all.append(d->escapeString(term));
            queryList.append(QHelpSearchQuery(QHelpSearchQuery::ALL, all));
        }

        lst = d->atLeastQuery->text().split(exp, QString::SkipEmptyParts);
        if (!lst.isEmpty()) {
            QStringList atLeast;
            foreach (const QString term, lst)
                atLeast += d->buildTermList(d->escapeString(term));
            queryList.append(QHelpSearchQuery(QHelpSearchQuery::ATLEAST, atLeast));
        }
    }
    return queryList;
#endif
}

/*! \reimp
*/
void QHelpSearchQueryWidget::focusInEvent(QFocusEvent *focusEvent)
{
    if (focusEvent->reason() != Qt::MouseFocusReason) {
        d->defaultQuery->selectAll();
        d->defaultQuery->setFocus();
    }
}

QT_END_NAMESPACE

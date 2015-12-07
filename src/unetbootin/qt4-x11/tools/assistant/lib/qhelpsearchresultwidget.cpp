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

#include "qhelpsearchresultwidget.h"

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QPointer>
#include <QtCore/QStringList>

#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QMouseEvent>
#include <QtGui/QHeaderView>
#include <QtGui/QSpacerItem>
#include <QtGui/QToolButton>
#include <QtGui/QTreeWidget>
#include <QtGui/QTextBrowser>
#include <QtGui/QTreeWidgetItem>

QT_BEGIN_NAMESPACE

class QDefaultResultWidget : public QTreeWidget
{
    Q_OBJECT

public:
    QDefaultResultWidget(QWidget *parent = 0)
        : QTreeWidget(parent)
    {
        header()->hide();
        connect(this, SIGNAL(itemActivated(QTreeWidgetItem*, int)),
            this, SLOT(itemActivated(QTreeWidgetItem*, int)));
    }

    void showResultPage(const QList<QHelpSearchEngine::SearchHit> hits)
    {
        foreach (const QHelpSearchEngine::SearchHit hit, hits)
            new QTreeWidgetItem(this, QStringList(hit.first) << hit.second);
    }

signals:
    void requestShowLink(const QUrl &url);

private slots:
    void itemActivated(QTreeWidgetItem *item, int /* column */)
    {
        if (item) {
            QString data = item->data(1, Qt::DisplayRole).toString();
            emit requestShowLink(data);
        }
    }
};


class QCLuceneResultWidget : public QTextBrowser
{
    Q_OBJECT

public:
    QCLuceneResultWidget(QWidget *parent = 0)
        : QTextBrowser(parent)
    {
        connect(this, SIGNAL(anchorClicked(const QUrl&)),
            this, SIGNAL(requestShowLink(const QUrl&)));
        setContextMenuPolicy(Qt::NoContextMenu);
    }

    void showResultPage(const QList<QHelpSearchEngine::SearchHit> hits, bool isIndexing)
    {
        QString htmlFile = QString(QLatin1String("<html><head><title>%1</title></head><body>"))
            .arg(tr("Search Results"));

        int count = hits.count();
        if (count != 0) {
            if (isIndexing)
                htmlFile += QString(QLatin1String("<div style=\"text-align:left; font-weight:bold; color:red\">"
                    "%1&nbsp;<span style=\"font-weight:normal; color:black\">"
                    "%2</span></div></div><br>")).arg(tr("Note:"))
                    .arg(tr("The search results may not be complete since the "
                            "documentation is still being indexed!"));

            foreach (const QHelpSearchEngine::SearchHit hit, hits) {
                htmlFile += QString(QLatin1String("<div style=\"text-align:left; font-weight:bold\""
                "><a href=\"%1\">%2</a><div style=\"color:green; font-weight:normal;"
                " margin:5px\">%1</div></div><p></p>"))
                .arg(hit.first).arg(hit.second);
            }
        } else {
            htmlFile += QLatin1String("<div align=\"center\"><br><br><h2>")
                    + tr("Your search did not match any documents.")
                    + QLatin1String("</h2><div>");
            if (isIndexing)
                htmlFile += QLatin1String("<div align=\"center\"><h3>")
                    + tr("(The reason for this might be that the documentation "
                         "is still being indexed.)")
                    + QLatin1String("</h3><div>");
        }

        htmlFile += QLatin1String("</body></html>");

        setHtml(htmlFile);
    }

signals:
    void requestShowLink(const QUrl &url);

private slots:
    void setSource(const QUrl & /* name */) {}
};


class QHelpSearchResultWidgetPrivate : public QObject
{
    Q_OBJECT

private slots:
    void setResults(int hitsCount)
    {
        if (!searchEngine.isNull()) {
#if defined(QT_CLUCENE_SUPPORT)
            showFirstResultPage();
            updateNextButtonState(((hitsCount > 20) ? true : false));
#else
            resultTreeWidget->clear();
            resultTreeWidget->showResultPage(searchEngine->hits(0, hitsCount));
#endif
        }
    }

    void showNextResultPage()
    {
        if (!searchEngine.isNull()
            && resultLastToShow < searchEngine->hitsCount()) {
            resultLastToShow += 20;
            resultFirstToShow += 20;

            resultTextBrowser->showResultPage(searchEngine->hits(resultFirstToShow,
                resultLastToShow), isIndexing);
            if (resultLastToShow >= searchEngine->hitsCount())
                updateNextButtonState(false);
        }
        updateHitRange();
    }

    void showLastResultPage()
    {
        if (!searchEngine.isNull()) {
            resultLastToShow = searchEngine->hitsCount();
            resultFirstToShow = resultLastToShow - (resultLastToShow % 20);

            if (resultFirstToShow == resultLastToShow)
                resultFirstToShow -= 20;

            resultTextBrowser->showResultPage(searchEngine->hits(resultFirstToShow,
                resultLastToShow), isIndexing);
            updateNextButtonState(false);
        }
        updateHitRange();
    }

    void showFirstResultPage()
    {
        if (!searchEngine.isNull()) {
            resultLastToShow = 20;
            resultFirstToShow = 0;

            resultTextBrowser->showResultPage(searchEngine->hits(resultFirstToShow,
                resultLastToShow), isIndexing);
            updatePrevButtonState(false);
        }
        updateHitRange();
    }

    void showPreviousResultPage()
    {
        if (!searchEngine.isNull()) {
            int count = resultLastToShow % 20;
            if (count == 0 || resultLastToShow != searchEngine->hitsCount())
                count = 20;

            resultLastToShow -= count;
            resultFirstToShow = resultLastToShow -20;

            resultTextBrowser->showResultPage(searchEngine->hits(resultFirstToShow,
                resultLastToShow), isIndexing);
            if (resultFirstToShow == 0)
                updatePrevButtonState(false);
        }
        updateHitRange();
    }

    void updatePrevButtonState(bool state = true)
    {
        firstResultPage->setEnabled(state);
        previousResultPage->setEnabled(state);
    }

    void updateNextButtonState(bool state = true)
    {
        nextResultPage->setEnabled(state);
        lastResultPage->setEnabled(state);
    }

    void indexingStarted()
    {
        isIndexing = true;
    }

    void indexingFinished()
    {
        isIndexing = false;
    }

private:
    QHelpSearchResultWidgetPrivate(QHelpSearchEngine *engine)
        : QObject()
        , searchEngine(engine)
        , isIndexing(false)
    {
        resultTreeWidget = 0;
        resultTextBrowser = 0;

        resultLastToShow = 20;
        resultFirstToShow = 0;

        firstResultPage = 0;
        previousResultPage = 0;
        hitsLabel = 0;
        nextResultPage = 0;
        lastResultPage = 0;

        connect(searchEngine, SIGNAL(indexingStarted()),
            this, SLOT(indexingStarted()));
        connect(searchEngine, SIGNAL(indexingFinished()),
            this, SLOT(indexingFinished()));
    }

    ~QHelpSearchResultWidgetPrivate()
    {
        delete searchEngine;
    }

    QToolButton* setupToolButton(const QString &iconPath)
    {
        QToolButton *button = new QToolButton();
        button->setEnabled(false);
        button->setAutoRaise(true);
        button->setIcon(QIcon(iconPath));
        button->setIconSize(QSize(12, 12));
        button->setMaximumSize(QSize(16, 16));

        return button;
    }

    void updateHitRange()
    {
        int last = 0;
        int first = 0;
        int count = 0;

        if (!searchEngine.isNull()) {
            count = searchEngine->hitsCount();
            if (count > 0) {
                first = resultFirstToShow +1;
                last = resultLastToShow > count ? count : resultLastToShow;
            }
        }
        hitsLabel->setText(tr("%1 - %2 of %3 Hits").arg(first).arg(last).arg(count));
    }

private:
    friend class QHelpSearchResultWidget;

    QPointer<QHelpSearchEngine> searchEngine;

    QDefaultResultWidget *resultTreeWidget;
    QCLuceneResultWidget *resultTextBrowser;

    int resultLastToShow;
    int resultFirstToShow;
    bool isIndexing;

    QToolButton *firstResultPage;
    QToolButton *previousResultPage;
    QLabel *hitsLabel;
    QToolButton *nextResultPage;
    QToolButton *lastResultPage;
};

#include "qhelpsearchresultwidget.moc"


/*!
    \class QHelpSearchResultWidget
    \since 4.4
    \inmodule QtHelp
    \brief The QHelpSearchResultWidget class provides either a tree
    widget or a text browser depending on the used search engine to display
    the hits found by the search.
*/

/*!
    \fn void QHelpSearchResultWidget::requestShowLink(const QUrl &link)

    This signal is emitted when a item is activated and its associated
    \a link should be shown.
*/

QHelpSearchResultWidget::QHelpSearchResultWidget(QHelpSearchEngine *engine)
    : QWidget(0)
    , d(new QHelpSearchResultWidgetPrivate(engine))
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setMargin(0);
    vLayout->setSpacing(0);

#if defined(QT_CLUCENE_SUPPORT)
    QHBoxLayout *hBoxLayout = new QHBoxLayout();
#ifndef Q_OS_MAC
    hBoxLayout->setMargin(0);
    hBoxLayout->setSpacing(0);
#endif
    hBoxLayout->addWidget(d->firstResultPage = d->setupToolButton(
        QString::fromUtf8(":/trolltech/assistant/images/3leftarrow.png")));

    hBoxLayout->addWidget(d->previousResultPage = d->setupToolButton(
        QString::fromUtf8(":/trolltech/assistant/images/1leftarrow.png")));

    d->hitsLabel = new QLabel(tr("0 - 0 of 0 Hits"), this);
    d->hitsLabel->setEnabled(false);
    hBoxLayout->addWidget(d->hitsLabel);
    d->hitsLabel->setAlignment(Qt::AlignCenter);
    d->hitsLabel->setMinimumSize(QSize(150, d->hitsLabel->height()));

    hBoxLayout->addWidget(d->nextResultPage = d->setupToolButton(
        QString::fromUtf8(":/trolltech/assistant/images/1rightarrow.png")));

    hBoxLayout->addWidget(d->lastResultPage = d->setupToolButton(
        QString::fromUtf8(":/trolltech/assistant/images/3rightarrow.png")));

    QSpacerItem *spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    hBoxLayout->addItem(spacer);

    vLayout->addLayout(hBoxLayout);

    d->resultTextBrowser = new QCLuceneResultWidget(this);
    vLayout->addWidget(d->resultTextBrowser);

    connect(d->resultTextBrowser, SIGNAL(requestShowLink(const QUrl&)), this,
        SIGNAL(requestShowLink(const QUrl&)));

    connect(d->nextResultPage, SIGNAL(clicked()), d, SLOT(showNextResultPage()));
    connect(d->lastResultPage, SIGNAL(clicked()), d, SLOT(showLastResultPage()));
    connect(d->firstResultPage, SIGNAL(clicked()), d, SLOT(showFirstResultPage()));
    connect(d->previousResultPage, SIGNAL(clicked()), d, SLOT(showPreviousResultPage()));

    connect(d->firstResultPage, SIGNAL(clicked()), d, SLOT(updateNextButtonState()));
    connect(d->previousResultPage, SIGNAL(clicked()), d, SLOT(updateNextButtonState()));
    connect(d->nextResultPage, SIGNAL(clicked()), d, SLOT(updatePrevButtonState()));
    connect(d->lastResultPage, SIGNAL(clicked()), d, SLOT(updatePrevButtonState()));

#else
    d->resultTreeWidget = new QDefaultResultWidget(this);
    vLayout->addWidget(d->resultTreeWidget);
    connect(d->resultTreeWidget, SIGNAL(requestShowLink(const QUrl&)), this,
        SIGNAL(requestShowLink(const QUrl&)));
#endif

    connect(engine, SIGNAL(searchingFinished(int)), d, SLOT(setResults(int)));
}

/*!
    Destroys the search result widget.
*/
QHelpSearchResultWidget::~QHelpSearchResultWidget()
{
    delete d;
}

/*!
    Returns a reference of the URL that the item at \a point owns, or an
    empty URL if no item exists at that point.
*/
QUrl QHelpSearchResultWidget::linkAt(const QPoint &point)
{
    QUrl url;
#if defined(QT_CLUCENE_SUPPORT)
    if (d->resultTextBrowser)
        url = d->resultTextBrowser->anchorAt(point);
#else
    if (d->resultTreeWidget) {
        QTreeWidgetItem *item = d->resultTreeWidget->itemAt(point);
        if (item)
            url = item->data(1, Qt::DisplayRole).toString();
    }
#endif
    return url;
}

QT_END_NAMESPACE

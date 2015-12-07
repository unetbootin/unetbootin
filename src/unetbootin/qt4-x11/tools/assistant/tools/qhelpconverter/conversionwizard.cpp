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

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QVariant>
#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QMouseEvent>

#include "conversionwizard.h"
#include "inputpage.h"
#include "generalpage.h"
#include "filterpage.h"
#include "identifierpage.h"
#include "pathpage.h"
#include "filespage.h"
#include "outputpage.h"
#include "finishpage.h"
#include "qhpwriter.h"
#include "qhcpwriter.h"
#include "helpwindow.h"

QT_BEGIN_NAMESPACE

ConversionWizard::ConversionWizard()
{
    setWindowIcon(QIcon(QLatin1String(":/trolltech/qhelpconverter/assistant.png")));
    setWindowTitle(tr("Help Conversion Wizard"));
    setPixmap(QWizard::WatermarkPixmap,
        QPixmap(QLatin1String(":/trolltech/qhelpconverter/assistant-128.png"))) ;
    setOptions(QWizard::IndependentPages|QWizard::NoBackButtonOnLastPage
        |QWizard::HaveHelpButton);

    m_inputPage = new InputPage(&m_adpReader);
    setPage(Input_Page, m_inputPage);

    m_generalPage = new GeneralPage();
    setPage(General_Page, m_generalPage);

    m_filterPage = new FilterPage();
    setPage(Filter_Page, m_filterPage);
    m_filterPage->setMaximumHeight(240);

    m_identifierPage = new IdentifierPage();
    setPage(Identifier_Page, m_identifierPage);

    m_pathPage = new PathPage();
    setPage(Path_Page, m_pathPage);
    m_pathPage->setMaximumHeight(240);

    m_filesPage = new FilesPage();
    setPage(Files_Page, m_filesPage);
    m_filesPage->setMaximumHeight(240);

    m_outputPage = new OutputPage();
    setPage(Output_Page, m_outputPage);
    m_outputPage->setMaximumHeight(240);

    m_finishPage = new FinishPage();
    setPage(Finish_Page, m_finishPage);
    m_finishPage->setMaximumHeight(240);

    connect(this, SIGNAL(currentIdChanged(int)),
        this, SLOT(pageChanged(int)));

    m_helpWindow = 0;
    qApp->installEventFilter(this);

    QAbstractButton *btn = button(QWizard::HelpButton);
    btn->setCheckable(true);
    connect(btn, SIGNAL(toggled(bool)), this, SLOT(showHelp(bool)));
}

void ConversionWizard::setAdpFileName(const QString &fileName)
{
    setField(QLatin1String("adpFileName"), fileName);
}

void ConversionWizard::initializePage(int id)
{
    switch (id) {
    case Path_Page: {
        QFileInfo fi(field(QLatin1String("adpFileName")).toString());
        m_pathPage->setPath(fi.absolutePath());
        break;
        }
    case Output_Page: {
        QFileInfo fi(field(QLatin1String("adpFileName")).toString());
        m_outputPage->setPath(fi.absolutePath());
        setField(QLatin1String("ProjectFileName"), fi.baseName()
            + QLatin1String(".qhp"));
        setField(QLatin1String("CollectionFileName"), fi.baseName()
            + QLatin1String(".qhcp"));
        break;
        }
    }
}

void ConversionWizard::pageChanged(int id)
{
    if (id == Files_Page) {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        m_files.clear();
        QFileInfo fi(field(QLatin1String("adpFileName")).toString());
        QDir rootDir = fi.absolutePath();
        foreach (const QString &p, m_pathPage->paths()) {
            QDir dir(p);
            QString rel = rootDir.relativeFilePath(dir.absolutePath());
            if (!rel.isEmpty())
                rel.append(QLatin1Char('/'));
            foreach (const QString &f, dir.entryList(m_pathPage->filters()))
                m_files.append(rel + f);
        }
        m_filesPage->setFilesToRemove(getUnreferencedFiles(m_files));
        QApplication::restoreOverrideCursor();
    } else if (id == Output_Page) {
        m_outputPage->setCollectionComponentEnabled(
            !m_adpReader.properties().isEmpty());
    } else if (id == Finish_Page) {
        QTimer::singleShot(300, this, SLOT(convert()));
    }
}

void ConversionWizard::showHelp(bool toggle)
{
    int w = 180;
    int h = 180;
    if (!m_helpWindow) {
        m_helpWindow = new HelpWindow(this);
        m_helpWindow->setMaximumWidth(w);
        m_helpWindow->setMaximumHeight(h);
        m_helpWindow->setMinimumHeight(h);
    }

    if (toggle) {
        m_helpWindow->setHelp(currentPage()->objectName());
        QAbstractButton *btn = button(QWizard::HelpButton);
        QPoint p = btn->pos();
        int x = p.x();
        if (btn->pos().x() > w)
            x = p.x() + btn->width() - w;
        m_helpWindow->move(x, p.y()-h);
        m_helpWindow->show();
    } else {
        m_helpWindow->hide();
    }
}

bool ConversionWizard::eventFilter(QObject *obj, QEvent *e)
{
    if (m_helpWindow && m_helpWindow->isVisible()) {
        if (obj != button(QWizard::HelpButton)
            && e->type() == QEvent::MouseButtonRelease) {
            QMouseEvent *me = static_cast<QMouseEvent*>(e);
            if (!m_helpWindow->geometry().contains(mapFromParent(me->globalPos()))) {
                m_helpWindow->hide();
                button(QWizard::HelpButton)->setChecked(false);
            }
        } else if (e->type() == QEvent::KeyPress) {
            m_helpWindow->hide();
            button(QWizard::HelpButton)->setChecked(false);
        }
    }
    return QWizard::eventFilter(obj, e);
}

QStringList ConversionWizard::getUnreferencedFiles(const QStringList &files)
{
    QStringList lst;
    QSet<QString> adpFiles = m_adpReader.files();
    foreach (const QString &s, files) {
        if (!adpFiles.contains(s))
            lst.append(s);
    }
    return lst;
}

void ConversionWizard::convert()
{
    QFileInfo fi(field(QLatin1String("adpFileName")).toString());
    m_finishPage->appendMessage(tr("Converting %1...")
        .arg(fi.fileName()));
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QString qhpFileName = field(QLatin1String("ProjectFileName")).toString();
    QhpWriter qhpWriter(field(QLatin1String("namespaceName")).toString(),
        field(QLatin1String("virtualFolder")).toString());
    qhpWriter.setAdpReader(&m_adpReader);
    qhpWriter.setFilterAttributes(m_filterPage->filterAttributes());
    qhpWriter.setCustomFilters(m_filterPage->customFilters());

    foreach (const QString &f, m_filesPage->filesToRemove())
        m_files.removeAll(f);
    qhpWriter.setFiles(m_files);

    if (field(QLatin1String("createIdentifier")).toBool()) {
        if (field(QLatin1String("fileNamePrefix")).toBool())
            qhpWriter.generateIdentifiers(QhpWriter::FilePrefix);
        else
            qhpWriter.generateIdentifiers(QhpWriter::GlobalPrefix,
                field(QLatin1String("globalPrefix")).toString());
    } else {
        qhpWriter.generateIdentifiers(QhpWriter::SkipAll);
    }

    qhpWriter.writeFile(fi.absolutePath() + QDir::separator()
        + qhpFileName);

    m_finishPage->appendMessage(tr("Writing help collection file..."));

    if (!m_adpReader.properties().isEmpty()) {
        QhcpWriter qhcpWriter;
        qhcpWriter.setHelpProjectFile(qhpFileName);
        qhcpWriter.setProperties(m_adpReader.properties());
        qhcpWriter.setTitlePath(QLatin1String("qthelp://")
            + field(QLatin1String("namespaceName")).toString()
            + QLatin1String("/")
            +field(QLatin1String("virtualFolder")).toString());
        qhcpWriter.writeFile(fi.absolutePath() + QDir::separator()
            + field(QLatin1String("CollectionFileName")).toString());
    }

    m_finishPage->appendMessage(tr("Done."));
    QApplication::restoreOverrideCursor();
}

QT_END_NAMESPACE

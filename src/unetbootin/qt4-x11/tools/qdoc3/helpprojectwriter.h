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

#ifndef HELPPROJECTWRITER_H
#define HELPPROJECTWRITER_H

#include <QString>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "config.h"
#include "node.h"

QT_BEGIN_NAMESPACE

class Tree;
typedef QPair<QString, const Node*> QStringNodePair; 

struct SubProject
{
    QString title;
    QString indexTitle;
    QHash<Node::Type, QSet<FakeNode::SubType> > selectors;
    bool sortPages;
    QHash<QString, const Node *> nodes;
};

struct HelpProject
{
    QString name;
    QString helpNamespace;
    QString virtualFolder;
    QString fileName;
    QString indexRoot;
    QString indexTitle;
    QList<QStringList> keywords;
    QSet<QString> files;
    QSet<QString> extraFiles;
    QSet<QString> filterAttributes;
    QHash<QString, QSet<QString> > customFilters;
    QSet<QString> excluded;
    QMap<QString, SubProject> subprojects;
    QHash<const Node *, QSet<Node::Status> > memberStatus;
};

class HelpProjectWriter
{
public:
    HelpProjectWriter(const Config &config, const QString &defaultFileName);
    void addExtraFile(const QString &file);
    void addExtraFiles(const QSet<QString> &files);
    void generate(const Tree *tre);

private:
    void generateProject(HelpProject &project);
    void generateSections(HelpProject &project, QXmlStreamWriter &writer,
                          const Node *node);
    bool generateSection(HelpProject &project, QXmlStreamWriter &writer,
                         const Node *node);
    QStringList keywordDetails(const Node *node) const;
    void writeNode(HelpProject &project, QXmlStreamWriter &writer, const Node *node);
    void readSelectors(SubProject &subproject, const QStringList &selectors);

    const Tree *tree;

    QString outputDir;
    QList<HelpProject> projects;
};

QT_END_NAMESPACE

#endif

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

#include "graphics.h"
#include "feature.h"
#include "featuretreemodel.h"

#include <QtGui>

QT_BEGIN_NAMESPACE

static QString defaultPath;

class FeatureTextBrowser : public QTextBrowser {
    Q_OBJECT
public:
    FeatureTextBrowser(QWidget *parent) : QTextBrowser(parent) {
        QString docRoot;
        docRoot = QLibraryInfo::location(QLibraryInfo::DocumentationPath)
                  + "/html";
        setSearchPaths(searchPaths() << docRoot);
    }

signals:
    void featureClicked(const QString &feature);

public slots:
    void setSource(const QUrl &url)
    {
        if (url.scheme() == "feature")
            emit featureClicked(url.authority());
        else
            QTextBrowser::setSource(url);
    }
};

class Main : public QMainWindow {
    Q_OBJECT
public:
    Main();
    ~Main();
    void loadFeatures(const QString& filename);
    void loadConfig(const QString& filename);

public slots:
    void modelChanged();
    void showInfo(const QModelIndex &index);
    void showInfo(const QString &feature);
    void openConfig();
    void saveConfig();
    void expandView();
    void collapseView();
    void about();
    void aboutQt();
    void quit();
    void clear();
    void enableAll();
    void disableAll();

private:
    QTextBrowser *textBrowser;
    QTreeView *featureTree;
    FeatureTreeModel *featureModel;

    void init();
    void updateStatus(int numFeatures = -1);
    void completelyExpandIndex(const QModelIndex &parent);
};

template<typename Func>
void foreachIndex_helper(const QModelIndex &parent, Func func)
{
    const QAbstractItemModel *model = parent.model();
    const int rows = model->rowCount(parent);
    for (int i = 0; i < rows; ++i) {
        const QModelIndex child = model->index(i, 0, parent);
        func(child);
        foreachIndex_helper(child, func);
    }
}

template<typename Func>
void foreachIndex(const QAbstractItemModel *model, Func func)
{
    const int rows = model->rowCount(QModelIndex());
    for (int i = 0; i < rows; ++i) {
        const QModelIndex child = model->index(i, 0, QModelIndex());
        func(child);
        foreachIndex_helper(child, func);
    }
}

struct CheckStateSetter {

    CheckStateSetter(Qt::CheckState state, QAbstractItemModel *m)
        : checkState(state), model(m) {}

    void operator()(const QModelIndex &index) {
        model->setData(index, checkState, Qt::CheckStateRole);
    }

    Qt::CheckState checkState;
    QAbstractItemModel *model;
};

void Main::disableAll()
{
    QAbstractItemModel *model = featureTree->model();
    foreachIndex(model, CheckStateSetter(Qt::Unchecked, model));
}

void Main::enableAll()
{
    QAbstractItemModel *model = featureTree->model();
    foreachIndex(model, CheckStateSetter(Qt::Checked, model));
}

Main::Main()
{
    setWindowIcon(QIcon(QPixmap(logo_xpm)));

    QSplitter *splitter = new QSplitter(this);

    featureModel = new FeatureTreeModel(this);
    featureTree = new QTreeView(splitter);
    splitter->addWidget(featureTree);
    featureTree->setRootIsDecorated(true);
    featureTree->setModel(featureModel);
    featureTree->show();

    textBrowser = new FeatureTextBrowser(splitter);
    textBrowser->setFrameStyle(QFrame::WinPanel|QFrame::Sunken);
    splitter->addWidget(textBrowser);
    textBrowser->show();

    connect(textBrowser, SIGNAL(featureClicked(const QString&)),
            this, SLOT(showInfo(const QString&)));
    connect(featureTree, SIGNAL(activated(QModelIndex)),
	    this, SLOT(showInfo(QModelIndex)));
    connect(featureModel, SIGNAL(dataChanged(QModelIndex, QModelIndex)),
            this, SLOT(modelChanged()));
    connect(featureTree, SIGNAL(clicked(QModelIndex)),
            this, SLOT(showInfo(QModelIndex)));

    setCentralWidget(splitter);

    QMenu *file = menuBar()->addMenu("&File");
    file->addAction("&Open...", this, SLOT(openConfig()),
                    Qt::CTRL + Qt::Key_O);
    file->addAction("&Save As...", this, SLOT(saveConfig()),
                    Qt::CTRL + Qt::Key_S);
    file->addSeparator();
    file->addAction("&Reset",  this, SLOT(clear()));
    file->addSeparator();
    file->addAction("E&xit",  this, SLOT(quit()), Qt::CTRL + Qt::Key_Q);

    QMenu *edit = menuBar()->addMenu("&Tools");
    edit->addAction("&Enable all features", this, SLOT(enableAll()));
    edit->addAction("&Disable all features", this, SLOT(disableAll()));

    menuBar()->addSeparator();

    QMenu *help = menuBar()->addMenu("&Help");
    help->addAction("&About", this, SLOT(about()));
    help->addAction("About &Qt", this, SLOT(aboutQt()));

    QToolBar *tb = new QToolBar("Expand/Collapse features");
    QToolButton *button;

    button = new QToolButton(tb);
    button->setIcon(QIcon(QPixmap(collapsed_xpm)));
    button->setText("Collapse");
    button->setToolTip("Collapse");
    connect(button, SIGNAL(clicked()), this, SLOT(collapseView()));
    tb->addWidget(button);

    button = new QToolButton(tb);
    button->setIcon(QIcon(QPixmap(expanded_xpm)));
    button->setText("Expand");
    button->setToolTip("Expand");
    connect(button, SIGNAL(clicked()), this, SLOT(expandView()));
    tb->addWidget(button);
    addToolBar(tb);

    init();
}

Main::~Main()
{
    delete textBrowser;
    delete featureModel;
    delete featureTree;
}

void Main::clear()
{
    QSettings settings;
    settings.clear();
    featureModel->clear();
    featureTree->reset();
    init();
}

void Main::quit()
{
    if (isWindowModified()) {
        int button = QMessageBox::question(this, "Quit Program",
                                           "You have unsaved changes.\n"
                                           "Do you want to quit anyway?",
                                           QMessageBox::Yes,
                                           QMessageBox::No);
        if (static_cast<QMessageBox::Button>(button) != QMessageBox::Yes)
            return;
    }
    QApplication::instance()->quit();
}

/*
  Recursively expand expand \a parent and all of its children.
*/
void Main::completelyExpandIndex(const QModelIndex &parent)
{
    featureTree->setExpanded(parent, true);

    const QAbstractItemModel *model = featureTree->model();
    const int rows = model->rowCount(parent);
    for (int i = 0; i < rows; ++i)
        completelyExpandIndex(model->index(i, 0, parent));
}

void Main::expandView()
{
    completelyExpandIndex(QModelIndex());
}

void Main::collapseView()
{
    const QAbstractItemModel *model = featureTree->model();
    const int rows = model->rowCount(QModelIndex());
    for (int i = 0; i < rows; ++i) {
        QModelIndex index = model->index(i, 0, QModelIndex());
        featureTree->setExpanded(index, false);
    }
}

void Main::updateStatus(int numFeatures)
{
    QSettings settings;
    QString featureFile = settings.value("featureFile").toString();
    QString configFile = settings.value("lastConfig").toString();
    QString message("Using features from %1");

    if (numFeatures >= 0) {
        QString s("%1 features loaded from %2");
        statusBar()->showMessage(s.arg(numFeatures).arg(featureFile));
    }
    QString appName = QApplication::applicationName();
    if (configFile.isEmpty())
        configFile = "New File";
    setWindowTitle(appName + " - " + configFile + "[*]");
}

void Main::modelChanged()
{
    setWindowModified(true);
}

void Main::init()
{
    QSettings settings;
    QString features = settings.value("featureFile").toString();

    if (features.isEmpty() || !QFileInfo(features).isFile()) {
        features = QFileDialog::getOpenFileName(this,
                                                "Open a feature file",
                                                defaultPath,
                                                "Qt Features (qfeatures.txt)");
    }
    settings.setValue("featureFile", features);
    loadFeatures(features);

    expandView();
    collapseView();

    QString confFile = settings.value("lastConfig").toString();
    if (confFile.isEmpty())
        return;
    loadConfig(confFile);
}

void Main::openConfig()
{
    QSettings settings;
    QString configDir;

    QString prevFile = settings.value("lastConfig").toString();
    if (!prevFile.isEmpty())
        configDir = QFileInfo(prevFile).path();

    if (configDir.isEmpty())
        configDir = defaultPath;

    QString configFile;
    configFile = QFileDialog::getOpenFileName(this,
                                              "Open a configuration file",
                                              configDir,
                                              "Header files (*.h)");
    enableAll();
    if (!configFile.isEmpty())
        loadConfig(configFile);
    settings.setValue("lastConfig", QFileInfo(configFile).absoluteFilePath());
}

void Main::saveConfig()
{
    QSettings settings;
    QString configDir;

    QString prevFile = settings.value("lastConfig").toString();
    if (!prevFile.isEmpty())
        configDir = QFileInfo(prevFile).path();

    if (configDir.isEmpty())
        configDir = defaultPath;

    QString configFile;
    configFile = QFileDialog::getSaveFileName(this,
                                              "Save configuration file",
                                              configDir,
                                              "Header files (*.h)");
    if (configFile.isEmpty())
        return;

    QFile file(configFile);
    if (!file.open(QIODevice::WriteOnly)) {
	QMessageBox::warning(this,"Warning",
                             "Cannot write to file " + configFile);
	return;
    }

    QTextStream stream(&file);
    FeatureTreeModel *model;
    model = static_cast<FeatureTreeModel*>(featureTree->model());
    model->writeConfig(stream);

    settings.setValue("lastConfig", QFileInfo(configFile).absoluteFilePath());
    setWindowModified(false);
    updateStatus();
}

void Main::loadConfig(const QString &filename)
{
    if (!QFileInfo(filename).isFile())
	return;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
	QMessageBox::warning(this,"Warning", "Cannot open file " + filename);
	return;
    }

    QTextStream stream(&file);
    FeatureTreeModel *model;
    model = static_cast<FeatureTreeModel*>(featureTree->model());
    model->readConfig(stream);

    QSettings settings;
    settings.setValue("lastConfig", QFileInfo(filename).absoluteFilePath());
    setWindowModified(false);
    updateStatus();
}

void Main::loadFeatures(const QString &filename)
{
    Feature::clear();

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
	QMessageBox::warning(this,"Warning", "Cannot open file " + filename);
	return;
    }

    Feature *feature = 0;
    int numFeatures = 0;
    updateStatus(numFeatures);
    QTextStream s(&file);
    for (QString line = s.readLine(); !s.atEnd(); line = s.readLine()) {
	line = line.simplified();
	if (line.isEmpty())
	    continue;
	if (line.startsWith('#'))
	    continue;

	int colon = line.indexOf(':');
	if (colon < 0) { // assume description
            QString description = feature->description().simplified();
            description += " " + line;
            feature->setDescription(description);
	    continue;
	}

	QString tag = line.left(colon);
	QString value = line.mid(colon+1).simplified();
	if (tag == "Feature") {
            if (feature)
                featureModel->addFeature(feature);
            feature = Feature::getInstance(value);
            updateStatus(++numFeatures);
	} else if (tag == "Requires") {
            Q_ASSERT(feature);
            feature->setDependencies(value.split(' ', QString::SkipEmptyParts));
	} else if (tag == "Name") {
	    Q_ASSERT(feature);
            feature->setTitle(value);
	} else if (tag == "Section") {
	    Q_ASSERT(feature);
            feature->setSection(value);
	} else if (tag == "SeeAlso") {
	    Q_ASSERT(feature);
            feature->setRelations(value.split(' ', QString::SkipEmptyParts));
        } else if (tag == "Description") {
            Q_ASSERT(feature);
            feature->setDescription(value);
	}
    }
    if (feature)
        featureModel->addFeature(feature);

    featureTree->resizeColumnToContents(0);

    QSettings settings;
    settings.setValue("featureFile", QFileInfo(filename).absoluteFilePath());

    updateStatus();
}

void Main::showInfo(const QModelIndex &index)
{
    FeatureTreeModel *model;
    model = static_cast<FeatureTreeModel*>(featureTree->model());

    if (const Feature *feature = model->getFeature(index))
	textBrowser->setHtml(feature->toHtml());

    // Ensure index is visible
    QModelIndex parent = model->parent(index);
    while (parent.isValid()) {
        featureTree->setExpanded(parent, true);
        parent = model->parent(parent);
    }

    featureTree->scrollTo(index);
    featureTree->setCurrentIndex(index);
}

void Main::showInfo(const QString &feature)
{
    const Feature *f = Feature::getInstance(feature);
    FeatureTreeModel *model;
    model = static_cast<FeatureTreeModel*>(featureTree->model());
    showInfo(model->index(f));
}

void Main::about()
{
    QMessageBox::about(this, "About qconfig",
	"<p><b><font size=\"+2\">Qtopia Core build configuration</font></b></p>"
	"<p></p>"
	"<p>Version 2.0</p>"
	"<p>Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).</p>"
	"<p></p>"
	"<p>This program is licensed to you under the terms of the GNU General "
	"Public License Version 2 as published by the Free Software Foundation. This "
	"gives you legal permission to copy, distribute and/or modify this software "
	"under certain conditions. For details, see the file 'LICENSE.GPL' that came with "
	"this software distribution. If you did not get the file, send email to "
	"qt-info@nokia.com.</p>\n\n<p>The program is provided AS IS with NO WARRANTY "
	"OF ANY KIND, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS "
	"FOR A PARTICULAR PURPOSE.</p>"
    );
}

void Main::aboutQt()
{
    QMessageBox::aboutQt( this, tr("qconfig") );
}

QT_END_NAMESPACE

int main(int argc, char** argv)
{
    QT_USE_NAMESPACE
    QApplication app(argc,argv);
    app.setOrganizationDomain("trolltech.com");
    app.setOrganizationName("Trolltech");
    app.setApplicationName("QConfig");
    Main m;

    defaultPath = QLibraryInfo::location(QLibraryInfo::PrefixPath)
                  + "/src/corelib/global";

    for (int i = 1; i < argc; ++i) {
	QString arg = argv[i];
	if (arg == "-f" && i+1 < argc)
            m.loadFeatures(argv[++i]);
	else if (arg == "-c" && i+1 < argc)
            m.loadConfig(argv[++i]);
    }
    m.resize(m.sizeHint() + QSize(500,300));
    m.show();
    return app.exec();
}

#include "main.moc"

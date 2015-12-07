//! [0]
    QString s = Q3FileDialog::getOpenFileName(
                    "/home",
                    "Images (*.png *.xpm *.jpg)",
                    this,
                    "open file dialog",
                    "Choose a file");
//! [0]


//! [1]
  "Images (*.png *.xpm *.jpg);;Text files (*.txt);;XML files (*.xml)"
//! [1]


//! [2]
    Q3FileDialog* fd = new Q3FileDialog(this, "file dialog", true);
    fd->setMode(Q3FileDialog::AnyFile);
//! [2]


//! [3]
    fd->setFilter("Images (*.png *.xpm *.jpg)");
//! [3]


//! [4]
    fd->setViewMode(Q3FileDialog::Detail);
//! [4]


//! [5]
    QString fileName;
    if (fd->exec() == QDialog::Accepted)
        fileName = fd->selectedFile();
//! [5]


//! [6]
    class Preview : public QLabel, public Q3FilePreview
    {
    public:
        Preview(QWidget *parent=0) : QLabel(parent) {}

        void previewUrl(const Q3Url &u)
        {
            QString path = u.path();
            QPixmap pix(path);
            if (pix.isNull())
                setText("This is not a pixmap");
            else
                setPixmap(pix);
        }
    };
//! [6]


//! [7]
    Preview* p = new Preview;

    Q3FileDialog* fd = new Q3FileDialog(this);
    fd->setContentsPreviewEnabled(true);
    fd->setContentsPreview(p, p);
    fd->setPreviewMode(Q3FileDialog::Contents);
    fd->show();
//! [7]


//! [8]
    QStringList list = myFileDialog.selectedFiles();
    QStringList::Iterator it = list.begin();
    while(it != list.end()) {
        myProcessing(*it);
        ++it;
    }
//! [8]


//! [9]
     fd->setFilter("All C++ files (*.cpp *.cc *.C *.cxx *.c++)");
     fd->setFilter("*.cpp *.cc *.C *.cxx *.c++");
     fd->setFilter("All C++ files (*.cpp;*.cc;*.C;*.cxx;*.c++)");
     fd->setFilter("*.cpp;*.cc;*.C;*.cxx;*.c++");
//! [9]


//! [10]
    QString s = Q3FileDialog::getOpenFileName(
                    "/home",
                    "Images (*.png *.xpm *.jpg)",
                    this,
                    "open file dialog",
                    "Choose a file to open");
//! [10]


//! [11]
    QString s = Q3FileDialog::getSaveFileName(
                    "/home",
                    "Images (*.png *.xpm *.jpg)",
                    this,
                    "save file dialog",
                    "Choose a filename to save under");
//! [11]


//! [12]
    QString s = Q3FileDialog::getExistingDirectory(
                    "/home",
                    this,
                    "get existing directory",
                    "Choose a directory",
                    true);
//! [12]


//! [13]
    MyFileDialog::MyFileDialog(QWidget* parent, const char* name) :
        Q3FileDialog(parent, name)
    {
        QLabel* label = new QLabel("Added widgets", this);
        QLineEdit* lineedit = new QLineEdit(this);
        QPushButton* pushbutton = new QPushButton(this);

        addWidgets(label, lineedit, pushbutton);
    }
//! [13]


//! [14]
    QString types("Image files (*.png *.xpm *.jpg);;"
                  "Text files (*.txt);;"
                  "Any files (*)");
    Q3FileDialog fd = new Q3FileDialog(this);
    fd->setFilters(types);
    fd->show();
//! [14]


//! [15]
    Q3FileDialog* fd = new Q3FileDialog(this);
    fd->addFilter("Images (*.png *.jpg *.xpm)");
    fd->show();
//! [15]


//! [16]
    QStringList files = Q3FileDialog::getOpenFileNames(
                            "Images (*.png *.xpm *.jpg)",
                            "/home",
                            this,
                            "open files dialog",
                            "Select one or more files to open");
//! [16]


//! [17]
    QStringList list = files;
    QStringList::Iterator it = list.begin();
    while(it != list.end()) {
        myProcessing(*it);
        ++it;
    }
//! [17]


//! [18]
    class Preview : public QLabel, public Q3FilePreview
    {
    public:
        Preview(QWidget *parent=0) : QLabel(parent) {}

        void previewUrl(const Q3Url &u)
        {
            QString path = u.path();
            QPixmap pix(path);
            if (pix.isNull())
                setText("This is not a pixmap");
            else
                setText("This is a pixmap");
        }
    };

  //...

  int main(int argc, char** argv)
  {
    Preview* p = new Preview;

    Q3FileDialog* fd = new Q3FileDialog(this);
    fd->setInfoPreviewEnabled(true);
    fd->setInfoPreview(p, p);
    fd->setPreviewMode(Q3FileDialog::Info);
    fd->show();
  }

//! [18]


//! [19]
    class Preview : public QLabel, public Q3FilePreview
    {
    public:
        Preview(QWidget *parent=0) : QLabel(parent) {}

        void previewUrl(const Q3Url &u)
        {
            QString path = u.path();
            QPixmap pix(path);
            if (pix.isNull())
                setText("This is not a pixmap");
            else
                setPixmap(pix);
        }
    };

  //...

  int main(int argc, char** argv)
  {
    Preview* p = new Preview;

    Q3FileDialog* fd = new Q3FileDialog(this);
    fd->setContentsPreviewEnabled(true);
    fd->setContentsPreview(p, p);
    fd->setPreviewMode(Q3FileDialog::Contents);
    fd->show();
  }
//! [19]



//! [0]
  fileName = QFileDialog::getOpenFileName(this,
      tr("Open Image"), "/home/jana", tr("Image Files (*.png *.jpg *.bmp)"));
//! [0]


//! [1]
  "Images (*.png *.xpm *.jpg);;Text files (*.txt);;XML files (*.xml)"
//! [1]


//! [2]
  QFileDialog dialog(this);
  dialog.setFileMode(QFileDialog::AnyFile);
//! [2]


//! [3]
  dialog.setNameFilter(tr("Images (*.png *.xpm *.jpg)"));
//! [3]


//! [4]
  dialog.setViewMode(QFileDialog::Detail);
//! [4]


//! [5]
  QStringList fileNames;
  if (dialog.exec())
      fileNames = dialog.selectedFiles();
//! [5]


//! [6]
       dialog.setNameFilter("All C++ files (*.cpp *.cc *.C *.cxx *.c++)");
       dialog.setNameFilter("*.cpp *.cc *.C *.cxx *.c++");
//! [6]


//! [7]
      QStringList filters;
      filters << "Image files (*.png *.xpm *.jpg)"
              << "Text files (*.txt)"
              << "Any files (*)";

      QFileDialog dialog(this);
      dialog.setNameFilters(filters);
      dialog.exec();
//! [7]


//! [8]
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "/home",
                                                    tr("Images (*.png *.xpm *.jpg)"));
//! [8]


//! [9]
    QStringList files = QFileDialog::getOpenFileNames(
                            this,
                            "Select one or more files to open",
                            "/home",
                            "Images (*.png *.xpm *.jpg)");
//! [9]


//! [10]
    QStringList list = files;
    QStringList::Iterator it = list.begin();
    while(it != list.end()) {
        myProcessing(*it);
        ++it;
    }
//! [10]


//! [11]
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               "/home/jana/untitled.png",
                               tr("Images (*.png *.xpm *.jpg)"));
//! [11]


//! [12]
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    "/home",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
//! [12]



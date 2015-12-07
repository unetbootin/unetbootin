/****************************************************************************
**
** Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "dialog.h"
#include <QFileDialog>
#include <QBuffer>
#include <QtCore/QDebug>

/*!
  \class Dialog

  \brief This class is a simple example of how to use QSharedMemory.

  It is a simple dialog that presents a few buttons. To compile the
  example, run make in qt/examples/ipc. Then run the executable twice
  to create two processes running the dialog. In one of the processes,
  press the button to load an image into a shared memory segment, and
  then select an image file to load. Once the first process has loaded
  and displayed the image, in the second process, press the button to
  read the same image from shared memory. The second process displays
  the same image loaded from its new loaction in shared memory.
*/

/*!
  The class contains a data member \l {QSharedMemory} {sharedMemory},
  which is initialized with the key "QSharedMemoryExample" to force
  all instances of Dialog to access the same shared memory segment.
  The constructor also connects the clicked() signal from each of the
  three dialog buttons to the slot function appropriate for handling
  each button.
*/
//! [0]
Dialog::Dialog(QWidget *parent)
  : QDialog(parent), sharedMemory("QSharedMemoryExample")
{
    ui.setupUi(this);
    connect(ui.loadFromFileButton, SIGNAL(clicked()), SLOT(loadFromFile()));
    connect(ui.loadFromSharedMemoryButton,
	    SIGNAL(clicked()),
	    SLOT(loadFromMemory()));
    setWindowTitle(tr("SharedMemory Example"));
}
//! [0]

/*!
  This slot function is called when the \tt {Load Image From File...}
  button is pressed on the firs Dialog process. First, it tests
  whether the process is already connected to a shared memory segment
  and, if so, detaches from that segment. This ensures that we always
  start the example from the beginning if we run it multiple times
  with the same two Dialog processes. After detaching from an existing
  shared memory segment, the user is prompted to select an image file.
  The selected file is loaded into a QImage. The QImage is displayed
  in the Dialog and streamed into a QBuffer with a QDataStream.

  Next, it gets a new shared memory segment from the system big enough
  to hold the image data in the QBuffer, and it locks the segment to
  prevent the second Dialog process from accessing it. Then it copies
  the image from the QBuffer into the shared memory segment. Finally,
  it unlocks the shared memory segment so the second Dialog process
  can access it.

  After this function runs, the user is expected to press the \tt
  {Load Image from Shared Memory} button on the second Dialog process.

  \sa loadFromMemory()
 */
//! [1]
void Dialog::loadFromFile()
{
    if (sharedMemory.isAttached())
        detach();

    ui.label->setText(tr("Select an image file"));
    QString fileName = QFileDialog::getOpenFileName(0, QString(), QString(),
                                        tr("Images (*.png *.xpm *.jpg)"));
    QImage image;
    if (!image.load(fileName)) {
        ui.label->setText(tr("Selected file is not an image, please select another."));
        return;
    }
    ui.label->setPixmap(QPixmap::fromImage(image));
//! [1] //! [2]

    // load into shared memory
    QBuffer buffer;
    buffer.open(QBuffer::ReadWrite);
    QDataStream out(&buffer);
    out << image;
    int size = buffer.size();

    if (!sharedMemory.create(size)) {
        ui.label->setText(tr("Unable to create shared memory segment."));
        return;
    }
    sharedMemory.lock();
    char *to = (char*)sharedMemory.data();
    const char *from = buffer.data().data();
    memcpy(to, from, qMin(sharedMemory.size(), size));
    sharedMemory.unlock();
}
//! [2]

/*!
  This slot function is called in the second Dialog process, when the
  user presses the \tt {Load Image from Shared Memory} button. First,
  it attaches the process to the shared memory segment created by the
  first Dialog process. Then it locks the segment for exclusive
  access, copies the image data from the segment into a QBuffer, and
  streams the QBuffer into a QImage. Then it unlocks the shared memory
  segment, detaches from it, and finally displays the QImage in the
  Dialog.

  \sa loadFromFile()
 */
//! [3]
void Dialog::loadFromMemory()
{
    if (!sharedMemory.attach()) {
        ui.label->setText(tr("Unable to attach to shared memory segment.\n" \
			     "Load an image first."));
        return;
    }

    QBuffer buffer;
    QDataStream in(&buffer);
    QImage image;

    sharedMemory.lock();
    buffer.setData((char*)sharedMemory.constData(), sharedMemory.size());
    buffer.open(QBuffer::ReadOnly);
    in >> image;
    sharedMemory.unlock();

    sharedMemory.detach();
    ui.label->setPixmap(QPixmap::fromImage(image));
}
//! [3]

/*!
  This private function is called by the destructor to detach the
  process from its shared memory segment. When the last process
  detaches from a shared memory segment, the system releases the
  shared memory.
 */
void Dialog::detach()
{
    if (!sharedMemory.detach()) 
        ui.label->setText(tr("Unable to detach from shared memory."));
}


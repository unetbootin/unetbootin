/****************************************************************************
**
** Copyright (C) 2005-2008 Trolltech ASA. All rights reserved.
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

#include <QtGui>

#include <math.h>
#include <stdlib.h>

#include "extrafiltersplugin.h"

QStringList ExtraFiltersPlugin::filters() const
{
    return QStringList() << tr("Flip Horizontally") << tr("Flip Vertically")
                         << tr("Smudge...") << tr("Threshold...");
}

QImage ExtraFiltersPlugin::filterImage(const QString &filter,
                                       const QImage &image, QWidget *parent)
{
    QImage original = image.convertToFormat(QImage::Format_RGB32);
    QImage result = original;

    if (filter == tr("Flip Horizontally")) {
        for (int y = 0; y < original.height(); ++y) {
            for (int x = 0; x < original.width(); ++x) {
                int pixel = original.pixel(original.width() - x - 1, y);
                result.setPixel(x, y, pixel);
            }
        }
    } else if (filter == tr("Flip Vertically")) {
        for (int y = 0; y < original.height(); ++y) {
            for (int x = 0; x < original.width(); ++x) {
                int pixel = original.pixel(x, original.height() - y - 1);
                result.setPixel(x, y, pixel);
            }
        }
    } else if (filter == tr("Smudge...")) {
        bool ok;
        int numIters = QInputDialog::getInteger(parent, tr("Smudge Filter"),
                                    tr("Enter number of iterations:"),
                                    5, 1, 20, 1, &ok);
        if (ok) {
            for (int i = 0; i < numIters; ++i) {
                for (int y = 1; y < original.height() - 1; ++y) {
                    for (int x = 1; x < original.width() - 1; ++x) {
                        int p1 = original.pixel(x, y);
                        int p2 = original.pixel(x, y + 1);
                        int p3 = original.pixel(x, y - 1);
                        int p4 = original.pixel(x + 1, y);
                        int p5 = original.pixel(x - 1, y);

                        int red = (qRed(p1) + qRed(p2) + qRed(p3) + qRed(p4)
                                   + qRed(p5)) / 5;
                        int green = (qGreen(p1) + qGreen(p2) + qGreen(p3)
                                     + qGreen(p4) + qGreen(p5)) / 5;
                        int blue = (qBlue(p1) + qBlue(p2) + qBlue(p3)
                                    + qBlue(p4) + qBlue(p5)) / 5;
                        int alpha = (qAlpha(p1) + qAlpha(p2) + qAlpha(p3)
                                     + qAlpha(p4) + qAlpha(p5)) / 5;

                        result.setPixel(x, y, qRgba(red, green, blue, alpha));
                    }
                }
            }
        }
    } else if (filter == tr("Threshold...")) {
        bool ok;
        int threshold = QInputDialog::getInteger(parent, tr("Threshold Filter"),
                                                 tr("Enter threshold:"),
                                                 10, 1, 256, 1, &ok);
        if (ok) {
            int factor = 256 / threshold;
            for (int y = 0; y < original.height(); ++y) {
                for (int x = 0; x < original.width(); ++x) {
                    int pixel = original.pixel(x, y);
                    result.setPixel(x, y, qRgba(qRed(pixel) / factor * factor,
                                                qGreen(pixel) / factor * factor,
                                                qBlue(pixel) / factor * factor,
                                                qAlpha(pixel)));
                }
            }
        }
    }
    return result;
}

Q_EXPORT_PLUGIN2(pnp_extrafilters, ExtraFiltersPlugin)

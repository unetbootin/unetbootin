/*  This file is part of the KDE project.

Copyright (C) 2007 Trolltech ASA. All rights reserved.

This library is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 2.1 or 3 of the License.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "videorenderer_vmr9.h"

#include <QtGui/QWidget>
#include <QtGui/QPainter>
#include <QtCore/QTimerEvent>


QT_BEGIN_NAMESPACE

namespace Phonon
{
    namespace DS9
    {
        VideoRendererVMR9::VideoRendererVMR9(QWidget *target)
        {
            m_filter = Filter(CLSID_VideoMixingRenderer9, IID_IBaseFilter);
            if (!m_filter) {
                qWarning("the video widget could not be initialized correctly");
                return;
            }

            ComPointer<IVMRFilterConfig9> config(m_filter, IID_IVMRFilterConfig9);
            Q_ASSERT(config);
            HRESULT hr = config->SetRenderingMode(VMR9Mode_Windowless);
            Q_ASSERT(SUCCEEDED(hr));
            hr = config->SetNumberOfStreams(1); //for now we limit it to 1 input stream
            Q_ASSERT(SUCCEEDED(hr));
            m_windowlessControl = ComPointer<IVMRWindowlessControl9>(m_filter, IID_IVMRWindowlessControl9);
            m_windowlessControl->SetVideoClippingWindow(reinterpret_cast<HWND>(target->winId()));
        }

        VideoRendererVMR9::~VideoRendererVMR9()
        {
        }

        bool VideoRendererVMR9::isHardwareAccelerated() const
        {
            return true;
        }

       
        void VideoRendererVMR9::repaintCurrentFrame(QWidget *target, const QRect &)
        {
            HDC hDC = target->getDC();
            // repaint the video

            HRESULT hr = m_windowlessControl ? m_windowlessControl->RepaintVideo(target->winId(), hDC) : E_POINTER;
            if (FAILED(hr) || m_dstY > 0 || m_dstX > 0 || true) {
                const QColor c = target->palette().color(target->backgroundRole());
                COLORREF color = RGB(c.red(), c.green(), c.blue());
                HPEN hPen = ::CreatePen(PS_SOLID, 1, color);
                HBRUSH hBrush = ::CreateSolidBrush(color);
                ::SelectObject(hDC, hPen);
                ::SelectObject(hDC, hBrush);
                // repaint the video
                if (FAILED(hr)) {
                    //black background : we use the Win32 API to avoid the ghost effect of the backing store
                    ::Rectangle(hDC, 0, 0, target->width(), target->height());
                } else {
                    if (m_dstY > 0) {
                        ::Rectangle(hDC, 0, 0, target->width(), m_dstY); //top
                        ::Rectangle(hDC, 0, target->height() - m_dstY, target->width(), target->height()); //bottom
                    }
                    if (m_dstX > 0) {
                        ::Rectangle(hDC, 0, m_dstY, m_dstX, m_dstHeight + m_dstY); //left
                        ::Rectangle(hDC, m_dstWidth + m_dstX, m_dstY, target->width(), m_dstHeight + m_dstY); //right
                    }
                }
                ::DeleteObject(hPen);
                ::DeleteObject(hBrush);
            }
            target->releaseDC(hDC);

        }

        void VideoRendererVMR9::notifyResize(const QRect &rect, Phonon::VideoWidget::AspectRatio aspectRatio,
            Phonon::VideoWidget::ScaleMode scaleMode)
        {
            const QSize vsize = videoSize();
            internalNotifyResize(rect.size(), vsize, aspectRatio, scaleMode);

            RECT dstRectWin = { m_dstX, m_dstY, m_dstWidth + m_dstX, m_dstHeight + m_dstY};
            RECT srcRectWin = { 0, 0, vsize.width(), vsize.height()};

            if (m_windowlessControl) {
                m_windowlessControl->SetVideoPosition(&srcRectWin, &dstRectWin);
            }
        }

        QSize VideoRendererVMR9::videoSize() const
        {
            LONG w = 0,
                h = 0;
            if (m_windowlessControl) {
                m_windowlessControl->GetNativeVideoSize( &w, &h, 0, 0);
            }
            return QSize(w, h);
        }

        QImage VideoRendererVMR9::snapshot() const
        {
            if (m_windowlessControl) {
                BYTE *buffer = 0;
                HRESULT hr = m_windowlessControl->GetCurrentImage(&buffer);
                if (SUCCEEDED(hr)) {

                    const BITMAPINFOHEADER  *bmi = reinterpret_cast<BITMAPINFOHEADER*>(buffer);


                    const int w = bmi->biWidth,
                        h = bmi->biHeight;

                    // Create image and copy data into image.
                    QImage ret(w, h, QImage::Format_RGB32);

                    if (!ret.isNull()) {
                        uchar *data = buffer + bmi->biSize;
                        const int bytes_per_line = w * sizeof(QRgb);
                        for (int y = h - 1; y >= 0; --y) {
                            qMemCopy(ret.scanLine(y), //destination
                                data,     //source
                                bytes_per_line);
                            data += bytes_per_line;
                        }
                    }
                    ::CoTaskMemFree(buffer);
                    return ret;
                }
            }
            return QImage();
        }

        void VideoRendererVMR9::setActive(bool b)
        {
            if (b) {
            } else {
                RECT dummyRect = { 0, 0, 0, 0};
                m_windowlessControl->SetVideoPosition(&dummyRect, &dummyRect);
            }
        }


        void  VideoRendererVMR9::applyMixerSettings(qreal brightness, qreal contrast, qreal hue, qreal saturation)
        {
            InputPin sink = BackendNode::pins(m_filter, PINDIR_INPUT).first();
            OutputPin source;
            if (FAILED(sink->ConnectedTo(&source))) {
                return; //it must be connected to work
            }

            //get the mixer (used for brightness/contrast/saturation/hue)
            ComPointer<IVMRMixerControl9> mixer(m_filter, IID_IVMRMixerControl9);
            Q_ASSERT(mixer);

            VMR9ProcAmpControl ctrl;
            ctrl.dwSize = sizeof(ctrl);
            ctrl.dwFlags = ProcAmpControl9_Contrast | ProcAmpControl9_Brightness | ProcAmpControl9_Saturation | ProcAmpControl9_Hue;
            VMR9ProcAmpControlRange range;
            range.dwSize = sizeof(range);

            range.dwProperty = ProcAmpControl9_Contrast;
            HRESULT hr = mixer->GetProcAmpControlRange(0, &range);
            if (FAILED(hr)) {
                return;
            }
            ctrl.Contrast = ((contrast < 0 ? range.MinValue : range.MaxValue) - range.DefaultValue) * qAbs(contrast) + range.DefaultValue;

            //brightness
            range.dwProperty = ProcAmpControl9_Brightness;
            hr = mixer->GetProcAmpControlRange(0, &range);
            if (FAILED(hr)) {
                return;
            }
            ctrl.Brightness = ((brightness < 0 ? range.MinValue : range.MaxValue) - range.DefaultValue) * qAbs(brightness) + range.DefaultValue;

            //saturation
            range.dwProperty = ProcAmpControl9_Saturation;
            hr = mixer->GetProcAmpControlRange(0, &range);
            if (FAILED(hr)) {
                return;
            }
            ctrl.Saturation = ((saturation < 0 ? range.MinValue : range.MaxValue) - range.DefaultValue) * qAbs(saturation) + range.DefaultValue;

            //hue
            range.dwProperty = ProcAmpControl9_Hue;
            hr = mixer->GetProcAmpControlRange(0, &range);
            if (FAILED(hr)) {
                return;
            }
            ctrl.Hue = ((hue < 0 ? range.MinValue : range.MaxValue) - range.DefaultValue) * qAbs(hue) + range.DefaultValue;

            //finally set the settings
            mixer->SetProcAmpControl(0, &ctrl);
        }

    }
}

QT_END_NAMESPACE

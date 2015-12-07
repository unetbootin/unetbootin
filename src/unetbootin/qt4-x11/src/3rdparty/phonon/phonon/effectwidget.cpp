/*  This file is part of the KDE project
    Copyright (C) 2006-2007 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), Trolltech ASA
    (or its successors, if any) and the KDE Free Qt Foundation, which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public 
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "effectwidget.h"
#include "effectwidget_p.h"

#include <QtCore/QtAlgorithms>
#include <QtCore/QList>

#include "effect.h"
#include "effectparameter.h"
#include "phonondefs_p.h"
#include <QtGui/QBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QSlider>
#include <limits>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
static const qreal DEFAULT_MIN = std::numeric_limits<qreal>::min();
static const qreal DEFAULT_MAX = std::numeric_limits<qreal>::max();
static const int DEFAULT_MIN_INT = std::numeric_limits<int>::min();
static const int DEFAULT_MAX_INT = std::numeric_limits<int>::max();
static const int SLIDER_RANGE = 8;
static const int TICKINTERVAL = 4;


QT_BEGIN_NAMESPACE

#ifndef QT_NO_PHONON_EFFECTWIDGET

namespace Phonon
{

EffectWidget::EffectWidget(Effect *effect, QWidget *parent)
    : QWidget(parent),
    k_ptr(new EffectWidgetPrivate(effect))
{
    K_D(EffectWidget);
    d->q_ptr = this;
    d->autogenerateUi();
}

EffectWidget::~EffectWidget()
{
    delete k_ptr;
}

/*
EffectWidget::EffectWidget(EffectWidgetPrivate &dd, QWidget *parent)
    : QWidget(parent)
    , k_ptr(&dd)
{
    K_D(EffectWidget);
    d->q_ptr = this;
    d->autogenerateUi();
}
*/

EffectWidgetPrivate::EffectWidgetPrivate(Effect *e)
    : effect(e)
{
    //TODO: look up whether there is a specialized widget for this effect. This
    //could be a DSO or a Designer ui file found via KTrader.
    //
    //if no specialized widget is available:
}

void EffectWidgetPrivate::autogenerateUi()
{
    Q_Q(EffectWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(q);
    mainLayout->setMargin(0);
    foreach (const EffectParameter &para, effect->parameters()) {
        QVariant value = effect->parameterValue(para);
        QHBoxLayout *pLayout = new QHBoxLayout;
        mainLayout->addLayout(pLayout);

        QLabel *label = new QLabel(q);
        pLayout->addWidget(label);
        label->setText(para.name());
#ifndef QT_NO_TOOLTIP
        label->setToolTip(para.description());
#endif

        QWidget *control = 0;
        switch (para.type()) {
        case QVariant::String:
            {
                QComboBox *cb = new QComboBox(q);
                control = cb;
                if (value.type() == QVariant::Int) {
                    //value just defines the item index
                    foreach (const QVariant &item, para.possibleValues()) {
                        cb->addItem(item.toString());
                    }
                    cb->setCurrentIndex(value.toInt());
                    QObject::connect(cb, SIGNAL(currentIndexChanged(int)), q, SLOT(_k_setIntParameter(int)));
                } else {
                    foreach (const QVariant &item, para.possibleValues()) {
                        cb->addItem(item.toString());
                        if (item == value) {
                            cb->setCurrentIndex(cb->count() - 1);
                        }
                    }
                    QObject::connect(cb, SIGNAL(currentIndexChanged(QString)), q, SLOT(_k_setStringParameter(QString)));
                }
            }
            break;
        case QVariant::Bool:
            {
                QCheckBox *cb = new QCheckBox(q);
                control = cb;
                cb->setChecked(value.toBool());
                QObject::connect(cb, SIGNAL(toggled(bool)), q, SLOT(_k_setToggleParameter(bool)));
            }
            break;
        case QVariant::Int:
            {
                QSpinBox *sb = new QSpinBox(q);
                control = sb;
                bool minValueOk = false;
                bool maxValueOk = false;
                const int minValue = para.minimumValue().toInt(&minValueOk);
                const int maxValue = para.minimumValue().toInt(&maxValueOk);

                sb->setRange(minValueOk ? minValue : DEFAULT_MIN_INT, maxValueOk ? maxValue : DEFAULT_MAX_INT);
                sb->setValue(value.toInt());
                QObject::connect(sb, SIGNAL(valueChanged(int)), q, SLOT(_k_setIntParameter(int)));
            }
            break;
        case QVariant::Double:
            {
                const double minValue = (para.minimumValue().type() == QVariant::Double ?
                    para.minimumValue().toDouble() : DEFAULT_MIN);
                const double maxValue = (para.maximumValue().type() == QVariant::Double ?
                    para.maximumValue().toDouble() : DEFAULT_MAX);

                if (minValue == -1. && maxValue == 1.) {
                    //Special case values between -1 and 1.0 to use a slider for improved usability
                    QSlider *slider = new QSlider(Qt::Horizontal, q);
                    slider->setRange(-SLIDER_RANGE, +SLIDER_RANGE);
                    slider->setValue(int(SLIDER_RANGE * value.toDouble()));
                    slider->setTickPosition(QSlider::TicksBelow);
                    slider->setTickInterval(TICKINTERVAL);
                    QObject::connect(slider, SIGNAL(valueChanged(int)), q, SLOT(_k_setSliderParameter(int)));
                } else {
                    double step = 0.1;
                    if (qAbs(maxValue - minValue) > 50)
                        step = 1.0;
                    QDoubleSpinBox *sb = new QDoubleSpinBox(q);
                    control = sb;
                    sb->setRange(minValue, maxValue);
                    sb->setValue(value.toDouble());
                    sb->setSingleStep(step);
                    QObject::connect(sb, SIGNAL(valueChanged(double)), q,
                        SLOT(_k_setDoubleParameter(double)));
                }
            }
            break;
        default:
            break;
        }

#ifndef QT_NO_TOOLTIP
        control->setToolTip(para.description());
#endif
        if (control) {
#ifndef QT_NO_SHORTCUT
            label->setBuddy(control);
#endif
            pLayout->addWidget(control);
            parameterForObject.insert(control, para);
        }
    }
}

void EffectWidgetPrivate::_k_setToggleParameter(bool checked)
{
    Q_Q(EffectWidget);
    if (parameterForObject.contains(q->sender())) {
        effect->setParameterValue(parameterForObject[q->sender()], checked);
    }
}

void EffectWidgetPrivate::_k_setIntParameter(int value)
{
    Q_Q(EffectWidget);
    if (parameterForObject.contains(q->sender())) {
        effect->setParameterValue(parameterForObject[q->sender()], value);
    }
}

void EffectWidgetPrivate::_k_setDoubleParameter(double value)
{
    Q_Q(EffectWidget);
    if (parameterForObject.contains(q->sender())) {
        effect->setParameterValue(parameterForObject[q->sender()], value);
    }
}

void EffectWidgetPrivate::_k_setStringParameter(const QString &value)
{
    Q_Q(EffectWidget);
    if (parameterForObject.contains(q->sender())) {
        effect->setParameterValue(parameterForObject[q->sender()], value);
    }
}

void EffectWidgetPrivate::_k_setSliderParameter(int value)
{
    Q_Q(EffectWidget);
    if (parameterForObject.contains(q->sender())) {
        effect->setParameterValue(parameterForObject[q->sender()], double(value) / double(SLIDER_RANGE));
    }
}


} // namespace Phonon


#endif // QT_NO_PHONON_EFFECTWIDGET

QT_END_NAMESPACE

#include "moc_effectwidget.cpp"

// vim: sw=4 ts=4

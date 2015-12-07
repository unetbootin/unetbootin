/*  This file is part of the KDE project
    Copyright (C) 2006-2007 Matthias Kretz <kretz@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#include "effectwidget.h"
#include "effectwidget_p.h"

#include <QtAlgorithms>
#include <QtCore/QList>

#include "effect.h"
#include "effectparameter.h"
#include "phonondefs_p.h"
#include <QtGui/QBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QSpinBox>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>

QT_BEGIN_NAMESPACE

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

        QWidget *control;
        if (para.type() == QVariant::String) {
            QComboBox *cb = new QComboBox(q);
            control = cb;
            if (value.type() == QVariant::Int) {
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
        } else if (para.type() == QVariant::Bool) {
            QCheckBox *cb = new QCheckBox(q);
            control = cb;
            cb->setChecked(value.toBool());
            QObject::connect(cb, SIGNAL(toggled(bool)), q, SLOT(_k_setToggleParameter(bool)));
        }
        else if (para.minimumValue().isValid() && para.maximumValue().isValid())
        {
            if (para.type() == QVariant::Int)
            {
                QSpinBox *sb = new QSpinBox(q);
                control = sb;
                sb->setRange(para.minimumValue().toInt(),
                        para.maximumValue().toInt());
                sb->setValue(value.toInt());
                QObject::connect(sb, SIGNAL(valueChanged(int)), q, SLOT(_k_setIntParameter(int)));
            }
            else
            {
                QDoubleSpinBox *sb = new QDoubleSpinBox(q);
                control = sb;
                sb->setRange(para.minimumValue().toDouble(),
                        para.maximumValue().toDouble());
                sb->setValue(value.toDouble());
                sb->setSingleStep((para.maximumValue().toDouble() - para.minimumValue().toDouble()) / 20);
                QObject::connect(sb, SIGNAL(valueChanged(double)), q,
                        SLOT(_k_setDoubleParameter(double)));
            }
        }
        else
        {
            QDoubleSpinBox *sb = new QDoubleSpinBox(q);
            control = sb;
            sb->setDecimals(7);
            sb->setRange(-1e100, 1e100);
            QObject::connect(sb, SIGNAL(valueChanged(double)), q,
                    SLOT(_k_setDoubleParameter(double)));
        }
#ifndef QT_NO_TOOLTIP
        control->setToolTip(para.description());
#endif
        label->setBuddy(control);
        pLayout->addWidget(control);
        parameterForObject.insert(control, para);
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

} // namespace Phonon

QT_END_NAMESPACE

#include "moc_effectwidget.cpp"

// vim: sw=4 ts=4

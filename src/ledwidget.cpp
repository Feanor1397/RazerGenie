/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2018  Luca Weiss <luca@z3ntu.xyz>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QColorDialog>
#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QRadioButton>

#include <QDebug>

#include "ledwidget.h"
#include "razerdevicewidget.h"

LedWidget::LedWidget(QWidget* parent, libopenrazer::Device *device, libopenrazer::Led* led) : QWidget(parent)
{
    this->mLed = led;

    auto *verticalLayout = new QVBoxLayout(this);

    // Set appropriate text
    QLabel *lightingLocationLabel = new QLabel(tr("Lighting")); // TODO: Adjust based on LED

    auto *lightingHBox = new QHBoxLayout();
    verticalLayout->addWidget(lightingLocationLabel);
    verticalLayout->addLayout(lightingHBox);

    auto *comboBox = new QComboBox;
    QLabel *brightnessLabel = nullptr;
    QSlider *brightnessSlider = nullptr;

    comboBox->setObjectName("combobox");
    qDebug() << "CURRENT LED: " << led->getObjectPath().path();
    //TODO More elegant solution instead of the sizePolicy?
    comboBox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

    //TODO Battery
    //TODO Sync effects in comboboxes & colorStuff when the sync checkbox is active

    // Add items from capabilities
    for(auto ledFx : libopenrazer::ledFxList) {
        if(device->hasFx(ledFx.getIdentifier())) {
            comboBox->addItem(ledFx.getDisplayString(), QVariant::fromValue(ledFx));
        }
    }

    // Connect signal from combobox
    connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &LedWidget::fxComboboxChanged);

    // Brightness slider
    if(device->hasFx("brightness")) {
        brightnessLabel = new QLabel(tr("Brightness"));
        brightnessSlider = new QSlider(Qt::Horizontal, this);
        qDebug() << "Brightness:" << led->getBrightness();
        brightnessSlider->setValue(led->getBrightness());
        connect(brightnessSlider, &QSlider::valueChanged, this, &LedWidget::brightnessSliderChanged);
    }

    // Only add combobox if a capability was actually added
    if(comboBox->count() != 0) {
        lightingHBox->addWidget(comboBox);

        /* Color buttons */
        for(int i=1; i<=3; i++) {
            auto *colorButton = new QPushButton(this);
            QPalette pal = colorButton->palette();
            pal.setColor(QPalette::Button, QColor(Qt::green));

            colorButton->setAutoFillBackground(true);
            colorButton->setFlat(true);
            colorButton->setPalette(pal);
            colorButton->setMaximumWidth(70);
            colorButton->setObjectName("colorbutton" + QString::number(i));
            lightingHBox->addWidget(colorButton);

            libopenrazer::RazerCapability capability = comboBox->currentData().value<libopenrazer::RazerCapability>();
            if(capability.getNumColors() < i)
                colorButton->hide();
            connect(colorButton, &QPushButton::clicked, this, &LedWidget::colorButtonClicked);
        }

        /* Wave left/right radio buttons */
        for(int i=1; i<=2; i++) {
            QString name;
            if(i==1)
                name = tr("Left");
            else
                name = tr("Right");
            auto *radio = new QRadioButton(name, this);
            radio->setObjectName("radiobutton" + QString::number(i));
            if(i==1) // set the 'left' checkbox to activated
                radio->setChecked(true);
            // hide by default
            radio->hide();
            lightingHBox->addWidget(radio);
            connect(radio, &QRadioButton::toggled, this, &LedWidget::waveRadioButtonChanged);
        }
    }

    /* Brightness sliders */
    if(brightnessLabel != nullptr && brightnessSlider != nullptr) { // only if brightness capability exists
        verticalLayout->addWidget(brightnessLabel);
        auto *hboxSlider = new QHBoxLayout();
        QLabel *brightnessSliderValue = new QLabel;
        hboxSlider->addWidget(brightnessSlider);
        hboxSlider->addWidget(brightnessSliderValue);
        verticalLayout->addLayout(hboxSlider);
    }
}

void LedWidget::colorButtonClicked()
{
    qDebug() << "color dialog";

    auto *sender = qobject_cast<QPushButton*>(QObject::sender());

    QPalette pal(sender->palette());

    QColor oldColor = pal.color(QPalette::Button);

    QColor color = QColorDialog::getColor(oldColor);
    if(color.isValid()) {
        qDebug() << color.name();
        pal.setColor(QPalette::Button, color);
        sender->setPalette(pal);
    } else {
        qInfo() << "User cancelled the dialog.";
    }
    applyEffect();
}

void LedWidget::fxComboboxChanged(int index)
{
    auto *sender = qobject_cast<QComboBox*>(QObject::sender());
    libopenrazer::RazerCapability capability = sender->itemData(index).value<libopenrazer::RazerCapability>();

    // Show/hide the color buttons
    if(capability.getNumColors() == 0) { // hide all
        for(int i=1; i<=3; i++)
            findChild<QPushButton*>("colorbutton" + QString::number(i))->hide();
    } else {
        for(int i=1; i<=3; i++) {
            if(capability.getNumColors() < i)
                findChild<QPushButton*>("colorbutton" + QString::number(i))->hide();
            else
                findChild<QPushButton*>("colorbutton" + QString::number(i))->show();
        }
    }

    // Show/hide the wave radiobuttons
    if(capability.isWave() == 0) {
        findChild<QRadioButton*>("radiobutton1")->hide();
        findChild<QRadioButton*>("radiobutton2")->hide();
    } else {
        findChild<QRadioButton*>("radiobutton1")->show();
        findChild<QRadioButton*>("radiobutton2")->show();
    }

    applyEffectStandardLoc(capability.getIdentifier());
}

QColor LedWidget::getColorForButton(int num)
{
    QPalette pal = findChild<QPushButton*>("colorbutton" + QString::number(num))->palette();
    return pal.color(QPalette::Button);
}

libopenrazer::WaveDirection LedWidget::getWaveDirection()
{
    return findChild<QRadioButton*>("radiobutton1")->isChecked() ? libopenrazer::WAVE_LEFT : libopenrazer::WAVE_RIGHT;
}

void LedWidget::brightnessSliderChanged(int value)
{
    mLed->setBrightness(value);
}

void LedWidget::applyEffectStandardLoc(QString fxStr)
{
    if(fxStr == "off") {
        mLed->setNone();
    } else if(fxStr == "static") {
        QColor c = getColorForButton(1);
        mLed->setStatic(c);
    } else if(fxStr == "breathing") {
        QColor c = getColorForButton(1);
        mLed->setBreathing (c);
    } else if(fxStr == "breathing_dual") {
        QColor c1 = getColorForButton(1);
        QColor c2 = getColorForButton(2);
        mLed->setBreathingDual(c1, c2);
    } else if(fxStr == "breathing_random") {
        mLed->setBreathingRandom();
    } else if(fxStr == "blinking") {
        QColor c = getColorForButton(1);
        mLed->setBlinking(c);
    } else if(fxStr == "spectrum") {
        mLed->setSpectrum();
    } else if(fxStr == "wave") {
        mLed->setWave(getWaveDirection());
    } else if(fxStr == "reactive") {
        QColor c = getColorForButton(1);
        mLed->setReactive(c, libopenrazer::REACTIVE_500MS); // TODO Configure speed?
    } else {
        qWarning() << fxStr << " is not implemented yet!";
    }
}

void LedWidget::applyEffect()
{
    qDebug() << "applyEffect()";
    QComboBox *combobox = findChild<QComboBox*>("combobox");

    libopenrazer::RazerCapability capability = combobox->itemData(combobox->currentIndex()).value<libopenrazer::RazerCapability>();
    QString identifier = capability.getIdentifier();

    applyEffectStandardLoc(identifier);
}

void LedWidget::waveRadioButtonChanged(bool enabled)
{
    if(enabled)
        applyEffect();
}

libopenrazer::Led * LedWidget::led()
{
    return mLed;
}
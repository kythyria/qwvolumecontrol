#include "volumesliderwidgetinternal.h"

#include "abstractvolumemodel.h"
#include "volumesliderwidget.h"

#include <math.h>
#include <QLabel>
#include <QSlider>
#include <QSignalMapper>
#include <QGridLayout>

#include <mmdeviceapi.h>

const PROPERTYKEY PKEY_AudioEndpoint_PhysicalSpeakers = { { 0x1da5d803, 0xd492, 0x4edd, { 0x8c, 0x23, 0xe0, 0xc0, 0xff, 0xee, 0x7f, 0x0e } }, 3 };

VolumeSliderWidget::Internal::Internal(AbstractVolumeModel *model, VolumeSliderWidget *parent)
    : QObject(parent), model(0), linked(false)
{
    grid = new QGridLayout();
    parent->setLayout(grid); 
    
    mapper = new QSignalMapper(this);
    connect(mapper, SIGNAL(mapped(int)), this, SLOT(sliderMoved(int)));
    
    setVolumeModel(model);
}

VolumeSliderWidget::Internal::~Internal() {
    destroyChannels();
}

void VolumeSliderWidget::Internal::setVolumeModel(AbstractVolumeModel *model) {
    if(this->model) {
        destroyChannels();
        disconnect(this->model, &AbstractVolumeModel::changed, this, &Internal::modelUpdated);
        model = 0;
    }
    
    this->model = model;
    initChannels();
    connect(this->model, SIGNAL(changed()), this, SLOT(modelUpdated()));
}

void VolumeSliderWidget::Internal::initChannels() {
    rows.append(new SliderRow(QString(), grid, 0)); // Master
    int cc = model->channelCount();
    for(int i = 0; i < cc; i++) {
        auto sr = new SliderRow(model->channelName(i), grid, i);
        rows.append(sr);
    }
    
    for(int i = 0; i < rows.length(); i++) {
        mapper->setMapping(rows[i]->slider, i-1);
        connect(rows[i]->slider, SIGNAL(valueChanged(int)), mapper, SLOT(map()));
    }
}

void VolumeSliderWidget::Internal::destroyChannels() {
    for(int i = 0; i < rows.length(); i++) {
        delete rows[i];
    }
    
    rows.clear();
}

void VolumeSliderWidget::Internal::linkChannels(bool linked) {
    this->linked = linked;
}

void VolumeSliderWidget::Internal::hideChannels(bool hidden) {
    if(rows.length() < 2) { return; }
    
    for(int i = 1; i < rows.length(); i++) {
        rows[i]->setVisible(!hidden);
    }
}

void VolumeSliderWidget::Internal::setMasterVisible(bool visible) {
    rows[0]->setVisible(visible);
}

void VolumeSliderWidget::Internal::sliderMoved(int channel) {
    if(channel == -1) {
        auto sr = rows[0];
        float vol = sr->value();
        model->setVolume(vol);
    }
    else {
        auto sr = rows[channel + 1];
        float vol = sr->value();
        model->setChannelVolume(channel, vol);
    }
}

void VolumeSliderWidget::Internal::modelUpdated() {
    auto sr = rows[0];
    float vol = model->volume();
    sr->setValue(vol);
    
    for(int i = 1; i < rows.length(); i++) {
        sr = rows[i];
        vol = model->channelVolume(i);
        sr->setValue(vol);
    }
}

VolumeSliderWidget::Internal::SliderRow::SliderRow(QString name, QGridLayout *grid, int row) {
    lblName = new QLabel(name);
    lblValue = new QLabel("0 %");
    
    slider = new QSlider();
    slider->setMinimum(0);
    slider->setMaximum(1000);
    slider->setOrientation(Qt::Horizontal);
    
    this->grid = grid;
    grid->addWidget(lblName, row, 0);
    grid->addWidget(slider, row, 1);
    grid->addWidget(lblValue, row, 2);
}

void VolumeSliderWidget::Internal::SliderRow::setVisible(bool visible) {
    lblName->setVisible(visible);
    lblValue->setVisible(visible);
    slider->setVisible(visible);
}

float VolumeSliderWidget::Internal::SliderRow::value() {
    return slider->value() / 1000.0f;
}

void VolumeSliderWidget::Internal::SliderRow::setValue(float val) {
    slider->setValue(roundf(val * 1000.0f));
}

VolumeSliderWidget::Internal::SliderRow::~SliderRow() {
    delete lblName;
    delete lblValue;
    delete slider;
}

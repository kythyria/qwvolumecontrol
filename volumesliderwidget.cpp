#include "volumesliderwidget.h"

#include <QGridLayout>
#include <QSlider>
#include <QLabel>
#include <QSignalMapper>
#include <QList>

#include "abstractvolumemodel.h"

#include "volumesliderwidgetinternal.h"

VolumeSliderWidget::VolumeSliderWidget(QSharedPointer<AbstractVolumeModel> model, QWidget *parent) : QWidget(parent) {
    stuff = new Internal(model, this);
}

VolumeSliderWidget::~VolumeSliderWidget() {
    delete stuff;
}

QSharedPointer<AbstractVolumeModel> VolumeSliderWidget::volumeModel() {
    return stuff->model;
}

void VolumeSliderWidget::linkChannels(bool linked) {
    stuff->linkChannels(linked);
}

void VolumeSliderWidget::hideChannels(bool hidden) {
    stuff->hideChannels(hidden);
}

void VolumeSliderWidget::setMasterVisible(bool visible) {
    stuff->setMasterVisible(visible);
}

#include "abstractvolumemodel.h"

AbstractVolumeModel::AbstractVolumeModel(QObject *parent) : QObject(parent)
{
}

AbstractVolumeModel::~AbstractVolumeModel() { }

bool AbstractVolumeModel::hasDecibels() {
    return false;
}

float AbstractVolumeModel::decibelsMin() {
    return 0.0f;
}

float AbstractVolumeModel::decibelsMax() {
    return 0.0f;
}

float AbstractVolumeModel::decibels() {
    return 0.0f;
}

void AbstractVolumeModel::setDecibels(float dbVolume) {
    
}

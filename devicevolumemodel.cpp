#include "devicevolumemodel.h"

class DeviceVolumeModel::Internal {
public:
    IAudioEndpointVolumePtr vol;
    
    Internal(IAudioEndpointVolumePtr vol);
};

DeviceVolumeModel::DeviceVolumeModel(IAudioEndpointVolumePtr vol, QObject *parent) : AbstractVolumeModel(parent)
{
    stuff = new Internal(vol);
}

DeviceVolumeModel::~DeviceVolumeModel() {
    delete stuff;
}

DeviceVolumeModel::Internal::Internal(IAudioEndpointVolumePtr vol) {
    this->vol = vol;
}

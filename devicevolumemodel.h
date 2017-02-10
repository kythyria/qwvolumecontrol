#ifndef DEVICEVOLUMEMODEL_H
#define DEVICEVOLUMEMODEL_H

#include "abstractvolumemodel.h"
#include <comip.h>
#include <comdef.h>
#include <endpointvolume.h>

_COM_SMARTPTR_TYPEDEF(IAudioEndpointVolume, __uuidof(IAudioEndpointVolume));

class DeviceVolumeModel : public AbstractVolumeModel
{
    class Internal;
    
    Internal *stuff;
    
public:
    explicit DeviceVolumeModel(IAudioEndpointVolumePtr vol, QObject *parent = 0);
    virtual ~DeviceVolumeModel();
};

#endif // DEVICEVOLUMEMODEL_H

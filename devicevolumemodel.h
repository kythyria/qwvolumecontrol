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
    
    virtual uint channelCount();
    virtual float channelVolume(uint channel);
    virtual void setChannelVolume(uint channel, float volume);
    
    virtual bool hasDecibels();
    virtual float decibelsMin();
    virtual float decibelsMax();
    virtual float decibels();
    virtual void setDecibels(float dbVolume);
    
    virtual float volume();
    virtual void setVolume(float volume);
    
    virtual bool muted();
    virtual void setMuted(bool muted);
};

#endif // DEVICEVOLUMEMODEL_H

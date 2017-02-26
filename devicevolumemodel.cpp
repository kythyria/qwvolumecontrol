#include "devicevolumemodel.h"
#include "util.h"
#include <functiondiscoverykeys.h>

#include <QtDebug>

_COM_SMARTPTR_TYPEDEF(IAudioEndpointVolume, __uuidof(IAudioEndpointVolume));
_COM_SMARTPTR_TYPEDEF(IPropertyStore, __uuidof(IPropertyStore));


class DeviceVolumeModel::Internal : public IAudioEndpointVolumeCallback {
    DeviceVolumeModel *owner;
    ULONG refcount;
    
public:
    uint channelMask;
    IAudioEndpointVolumePtr vol;
    
    DeviceVolumeModel::Internal(DeviceVolumeModel *owner, IAudioEndpointVolumePtr vol);
    
    // IUnknown
    virtual ULONG AddRef();
    virtual ULONG Release();
    virtual HRESULT QueryInterface(REFIID riid, void **ppvObject);
    
    // IAudioEndpointVolumeCallback
    virtual HRESULT OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify);
    
};

DeviceVolumeModel::DeviceVolumeModel(IMMDevicePtr device, QObject *parent) : AbstractVolumeModel(parent)
{
    IAudioEndpointVolumePtr vol;
    IAudioEndpointVolume **paev = &(vol);
    HRESULT hr = device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)paev);
    AlertHresult(hr, QString("Unable to open the volume control (%1)"));
    
    stuff = new Internal(this, vol);
    
    IPropertyStorePtr props;
     hr = device->OpenPropertyStore(STGM_READ, &props);
    assertHR(hr, "Couldn't open device property store (%0)");
    
    // FIXME: Why does this return zero successfully?
    PROPVARIANT pv;
    hr = props->GetValue(PKEY_AudioEndpoint_PhysicalSpeakers, &pv);
    AlertHresult(hr, QString("Couldn't get channel configuration mask (%0)"));
    stuff->channelMask = pv.uintVal;
    PropVariantClear(&pv);
    
    vol->RegisterControlChangeNotify((IAudioEndpointVolumeCallback*)stuff);
    
    qDebug() << "Device channels: " << this->channelLayoutMask();
    for(uint i = 0; i < this->channelCount(); i++) {
        qDebug() << "    " << this->channelName(i);
    }
}

DeviceVolumeModel::~DeviceVolumeModel() {
    stuff->vol->UnregisterControlChangeNotify((IAudioEndpointVolumeCallback*)stuff);
    stuff->Release();
}

DeviceVolumeModel::Internal::Internal(DeviceVolumeModel *owner, IAudioEndpointVolumePtr vol) : refcount(1) {
    this->vol = vol;
    this->owner = owner;
}

HRESULT DeviceVolumeModel::Internal::OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify) {
    (void)pNotify;
    owner->metaObject()->invokeMethod(owner, "changed", Qt::QueuedConnection);
    return S_OK;
}

ULONG STDMETHODCALLTYPE DeviceVolumeModel::Internal::AddRef() {
    return InterlockedIncrement(&refcount);
}

ULONG STDMETHODCALLTYPE DeviceVolumeModel::Internal::Release() {
    ULONG ulRef = InterlockedDecrement(&refcount);
    if (0 == ulRef) {
        delete this;
    }
    return ulRef;
}

HRESULT STDMETHODCALLTYPE DeviceVolumeModel::Internal::QueryInterface(REFIID riid, void **ppvInterface) {
    if (IID_IUnknown == riid) {
        AddRef();
        *ppvInterface = (IUnknown*)this;
    }
    else if (__uuidof(IAudioEndpointVolumeCallback) == riid) {
        AddRef();
        *ppvInterface = (IAudioEndpointVolumeCallback*)this;
    }
    else {
        *ppvInterface = NULL;
        return E_NOINTERFACE;
    }
    return S_OK;
}

uint DeviceVolumeModel::channelCount() {
    uint count;
    HRESULT hr = stuff->vol->GetChannelCount(&count);
    return SUCCEEDED(hr) ? count : 0;
}

uint DeviceVolumeModel::channelLayoutMask() {
    return stuff->channelMask;
}

float DeviceVolumeModel::channelVolume(uint channel) {
    float level;
    HRESULT hr = stuff->vol->GetChannelVolumeLevelScalar(channel, &level);
    return SUCCEEDED(hr) ? level : 0.0f;
}

void DeviceVolumeModel::setChannelVolume(uint channel, float volume) {
    stuff->vol->SetChannelVolumeLevelScalar(channel, volume, nullptr);
}

bool DeviceVolumeModel::hasDecibels() {
    return true;
}

float DeviceVolumeModel::decibelsMin() {
    float dbmin, dbmax, dbstep;
    HRESULT hr = stuff->vol->GetVolumeRange(&dbmin, &dbmax, &dbstep);
    return SUCCEEDED(hr) ? dbmin : 0.0f;
}

float DeviceVolumeModel::decibelsMax() {
    float dbmin, dbmax, dbstep;
    HRESULT hr = stuff->vol->GetVolumeRange(&dbmin, &dbmax, &dbstep);
    return SUCCEEDED(hr) ? dbmax : 0.0f;
}

float DeviceVolumeModel::decibels() {
    float level;
    HRESULT hr = stuff->vol->GetMasterVolumeLevel(&level);
    return SUCCEEDED(hr) ? level : 0.0f;
}

void DeviceVolumeModel::setDecibels(float dbVolume) {
    stuff->vol->GetMasterVolumeLevel(&dbVolume);
}

float DeviceVolumeModel::volume() {
    float level;
    HRESULT hr = stuff->vol->GetMasterVolumeLevelScalar(&level);
    return SUCCEEDED(hr) ? level : 0.0f;
}

void DeviceVolumeModel::setVolume(float volume) {
    stuff->vol->SetMasterVolumeLevelScalar(volume, NULL);
}

bool DeviceVolumeModel::muted() {
    BOOL m;
    HRESULT hr = stuff->vol->GetMute(&m);
    return SUCCEEDED(hr) && (m == TRUE) ? m : false;
}

void DeviceVolumeModel::setMuted(bool muted) {
    stuff->vol->SetMute(muted, NULL);
}

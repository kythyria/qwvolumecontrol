#include "devicevolumemodel.h"

class DeviceVolumeModel::Internal : public IAudioEndpointVolumeCallback {
    DeviceVolumeModel *owner;
    ULONG refcount;
    
public:
    IAudioEndpointVolumePtr vol;
    
    DeviceVolumeModel::Internal(DeviceVolumeModel *owner, IAudioEndpointVolumePtr vol);
    
    // IUnknown
    virtual ULONG AddRef();
    virtual ULONG Release();
    virtual HRESULT QueryInterface(REFIID riid, void **ppvObject);
    
    // IAudioEndpointVolumeCallback
    virtual HRESULT OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify);
    
};

DeviceVolumeModel::DeviceVolumeModel(IAudioEndpointVolumePtr vol, QObject *parent) : AbstractVolumeModel(parent)
{
    stuff = new Internal(this, vol);
    vol->RegisterControlChangeNotify((IAudioEndpointVolumeCallback*)stuff);
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

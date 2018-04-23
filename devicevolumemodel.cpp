#include "devicevolumemodel.h"
#include "util.h"
#include "speakerformats.h"

#include <functiondiscoverykeys.h>
#include <QtDebug>
#include <windows.h>

COM_SMARTPTR(IAudioEndpointVolume);
COM_SMARTPTR(IPropertyStore);

const PROPERTYKEY PKEY_AudioEndpoint_PhysicalSpeakers = { { 0x1da5d803, 0xd492, 0x4edd, { 0x8c, 0x23, 0xe0, 0xc0, 0xff, 0xee, 0x7f, 0x0e } }, 3 };

class DeviceVolumeModel::Internal : public IAudioEndpointVolumeCallback {
    DeviceVolumeModel *owner;
    ULONG refcount;
    
public:
    uint channelMask;
    IAudioEndpointVolumePtr vol;
    IMMDevicePtr device;
    IPropertyStorePtr props;
    
    QString deviceName;
    QString deviceDescription;
    
    DeviceVolumeModel::Internal(DeviceVolumeModel *owner);
    
    // IUnknown
    virtual ULONG AddRef();
    virtual ULONG Release();
    virtual HRESULT QueryInterface(REFIID riid, void **ppvObject);
    
    // IAudioEndpointVolumeCallback
    virtual HRESULT OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify);
    
};

DeviceVolumeModel::DeviceVolumeModel(IMMDevicePtr device, QObject *parent) : AbstractVolumeModel(parent)
{
    stuff = new Internal(this);
    stuff->device = device;
    
    HRESULT hr = device->OpenPropertyStore(STGM_READ, &stuff->props);
    assertHR(hr, "Couldn't open device property store (%0)");
    
    PROPVARIANT pv;
    
    hr = stuff->props->GetValue(PKEY_Device_DeviceDesc, &pv);
    assertHR(hr, "Couldn't get device description (%0)");
    stuff->deviceDescription = QString::fromWCharArray(pv.pwszVal);
    PropVariantClear(&pv);
    
    hr = stuff->props->GetValue(PKEY_DeviceInterface_FriendlyName, &pv);
    //if(AlertHresult(hr, QString("Couldn't get device friendly name for %0 (%1)").arg(lblDeviceDesc->text()))) {
    if(SUCCEEDED(hr)) {
        stuff->deviceName = QString::fromWCharArray(pv.pwszVal);
    }
    PropVariantClear(&pv);
    
    if(!currentlyHasVolume()) { return; }
    
    IAudioEndpointVolume **paev = &(stuff->vol);
    hr = device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)paev);
    AlertHresult(hr, QString("Unable to open the volume control (%1)"));
    
    // Okay, so IPropertyStore succeeds but pv.vt == VT_EMPTY if the property doesn't exist.
    // FIXME: Deduce the channel names if the user hasn't configured it.
    hr = stuff->props->GetValue(PKEY_AudioEndpoint_PhysicalSpeakers, &pv);
    stuff->channelMask = pv.uintVal;
    PropVariantClear(&pv);
    
    // HACK: This is a *shameless* heuristic. In the event that the user hasn't explicitly
    // configured what kind of speakers are connected to that endpoint, we can guess in at
    // least two cases what sort of names to use.
    
    if(stuff->channelMask == 0 && this->channelCount() > 0) {
        switch(this->channelCount()) {
        case 1:
            stuff->channelMask = KSAUDIO_SPEAKER_MONO;
            break;
        case 2:
            stuff->channelMask = KSAUDIO_SPEAKER_STEREO;
            break;
        default:
            qDebug() << "Couldn't guess the channel names.";
        }
    }
    
    stuff->vol->RegisterControlChangeNotify((IAudioEndpointVolumeCallback*)stuff);
    
    qDebug() << "Device channels: " << this->channelLayoutMask();
    for(uint i = 0; i < this->channelCount(); i++) {
        qDebug() << "    " << this->channelName(i);
    }
}

DeviceVolumeModel::~DeviceVolumeModel() {
    stuff->vol->UnregisterControlChangeNotify((IAudioEndpointVolumeCallback*)stuff);
    stuff->Release();
}

DeviceVolumeModel::Internal::Internal(DeviceVolumeModel *owner) :
    refcount(1),
    owner(owner)
{ }

HRESULT DeviceVolumeModel::Internal::OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA pNotify) {
    owner->metaObject()->invokeMethod(owner, "muteChanged", Qt::QueuedConnection, Q_ARG(bool, pNotify->bMuted));
    owner->metaObject()->invokeMethod(owner, "volumeChanged", Qt::QueuedConnection, Q_ARG(float, pNotify->fMasterVolume));
    return S_OK;
}

COM_IMPL_REFCOUNT(DeviceVolumeModel::Internal)
COM_IMPL_QUERYINTERFACE(DeviceVolumeModel::Internal,
    COM_IMPL_QICASE(IAudioEndpointVolumeCallback))

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
    HRESULT hr = stuff->vol->SetMasterVolumeLevelScalar(volume, NULL);
    (void)hr;
}

bool DeviceVolumeModel::muted() {
    BOOL m;
    HRESULT hr = stuff->vol->GetMute(&m);
    return SUCCEEDED(hr) && (m == TRUE) ? m : false;
}

void DeviceVolumeModel::setMuted(bool muted) {
    stuff->vol->SetMute(muted, NULL);
}

// No, these aren't backwards. For some reason the "name" of the device as identified
// by windows' choices of what to display where is in the description.

QString DeviceVolumeModel::name() {
    return stuff->deviceDescription;
}

QString DeviceVolumeModel::description() {
    return stuff->deviceName;
}

bool DeviceVolumeModel::currentlyHasVolume() {
    DWORD state;
    HRESULT hr =stuff->device->GetState(&state);
    
    return SUCCEEDED(hr) && state == DEVICE_STATE_ACTIVE;
}

IMMDevicePtr DeviceVolumeModel::device() {
    return stuff->device;
}

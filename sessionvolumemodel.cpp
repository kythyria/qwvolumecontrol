#include "sessionvolumemodel.h"

#include "getresource.h"
#include "util.h"

Q_DECLARE_METATYPE(AudioSessionState)
REGISTER_METATYPE(AudioSessionState)

COM_SMARTPTR(ISimpleAudioVolume);

class SessionVolumeModel::Internal : public IAudioSessionEvents {
    ULONG refcount;
    
public:
    IAudioSessionControl2Ptr session;
    ISimpleAudioVolumePtr volume;
    SessionVolumeModel *model;
    GUID eventContext;
    
    explicit Internal(IAudioSessionControl2Ptr session, SessionVolumeModel *model);
    virtual ~Internal();
    
    // IUnknown
    virtual ULONG AddRef();
    virtual ULONG Release();
    virtual HRESULT QueryInterface(REFIID riid, void **ppvObject);
    
    // IAudioSessionEvents
    virtual HRESULT OnChannelVolumeChanged(DWORD ChannelCount, float NewChannelVolumeArray[], DWORD ChangedChannel, LPCGUID EventContext);
    virtual HRESULT OnDisplayNameChanged(LPCWSTR NewDisplayName, LPCGUID EventContext);
    virtual HRESULT OnGroupingParamChanged(LPCGUID NewGroupingParam, LPCGUID EventContext);
    virtual HRESULT OnIconPathChanged(LPCWSTR NewIconPath, LPCGUID EventContext);
    virtual HRESULT OnSessionDisconnected(AudioSessionDisconnectReason DisconnectReason);
    virtual HRESULT OnSimpleVolumeChanged(float NewVolume, BOOL NewMute, LPCGUID EventContext);
    virtual HRESULT OnStateChanged(AudioSessionState NewState);
};

SessionVolumeModel::SessionVolumeModel(IAudioSessionControl2Ptr session, QObject *parent) : AbstractVolumeModel(parent)
{
    stuff = new Internal(session, this);
    CoCreateGuid(&stuff->eventContext);
    
    HRESULT hr = session->QueryInterface(&stuff->volume);
    if(FAILED(hr)) {
        stuff->volume = nullptr;
        DBG_PRINT << "Unable to get volume control for session (" << hr << ")";
    }
}

SessionVolumeModel::~SessionVolumeModel() {
    stuff->Release();
}

SessionVolumeModel::Internal::Internal(IAudioSessionControl2Ptr session, SessionVolumeModel *model) :
    refcount(1),
    session(session),
    model(model)
{
    session->RegisterAudioSessionNotification(this);
}

SessionVolumeModel::Internal::~Internal() {
    session->UnregisterAudioSessionNotification(this);
}

COM_IMPL_REFCOUNT(SessionVolumeModel::Internal)
COM_IMPL_QUERYINTERFACE(SessionVolumeModel::Internal,
    COM_IMPL_QICASE(IAudioSessionEvents))

#define DEFER_EMIT(target, method, ...) \
    target->metaObject()->invokeMethod(target, #method, Qt::QueuedConnection, ##__VA_ARGS__)

HRESULT SessionVolumeModel::Internal::OnChannelVolumeChanged(DWORD ChannelCount, float NewChannelVolumeArray[], DWORD ChangedChannel, LPCGUID EventContext) {
    (void)ChannelCount;
    (void)NewChannelVolumeArray;
    (void)ChangedChannel;
    if(!IsEqualGUID(*EventContext, eventContext)) {
        DEFER_EMIT(model, volumeChanged);
    }
    return S_OK;
}

HRESULT SessionVolumeModel::Internal::OnDisplayNameChanged(LPCWSTR NewDisplayName, LPCGUID EventContext) {
    if(!IsEqualGUID(*EventContext, eventContext)) {
        QString newname = QString::fromWCharArray(NewDisplayName);
        DEFER_EMIT(model, nameChanged, Q_ARG(QString, newname));
    }
    return S_OK;
}

HRESULT SessionVolumeModel::Internal::OnGroupingParamChanged(LPCGUID NewGroupingParam, LPCGUID EventContext) {
    return S_OK;
}

HRESULT SessionVolumeModel::Internal::OnIconPathChanged(LPCWSTR NewIconPath, LPCGUID EventContext) {
    if(!IsEqualGUID(*EventContext, eventContext)) {
        QString newpath = QString::fromWCharArray(NewIconPath);
        DEFER_EMIT(model, iconPathChanged, Q_ARG(QString, newpath));
    }
    return S_OK;
}

HRESULT SessionVolumeModel::Internal::OnSessionDisconnected(AudioSessionDisconnectReason DisconnectReason) {
    (void)DisconnectReason;
    DEFER_EMIT(model, sessionDisconnected);
    return S_OK;
}

HRESULT SessionVolumeModel::Internal::OnSimpleVolumeChanged(float NewVolume, BOOL NewMute, LPCGUID EventContext) {
    (void)NewVolume;
    (void)NewMute;
    if(!IsEqualGUID(*EventContext, eventContext)) {
        DEFER_EMIT(model, volumeChanged, Q_ARG(FLOAT, NewVolume));
    }
    return S_OK;
}

HRESULT SessionVolumeModel::Internal::OnStateChanged(AudioSessionState NewState) {
    DEFER_EMIT(model, stateChanged, Q_ARG(AudioSessionState, NewState));
    return S_OK;
}

uint SessionVolumeModel::channelCount() {
    if(stuff->volume.GetInterfacePtr()) {
        return 1;
    }
    return 0;
}

uint SessionVolumeModel::channelLayoutMask() {
    return KSAUDIO_SPEAKER_MONO; // wrong, but we don't have one for idk
}

float SessionVolumeModel::channelVolume(uint channel) {
    if(channel == 0) {
        return this->volume();
    }
    return 0.0f;
}

void SessionVolumeModel::setChannelVolume(uint channel, float volume) {
    if(channel == 0) {
        this->setVolume(volume);
    }
}

float SessionVolumeModel::volume() {
    if(stuff->volume.GetInterfacePtr()) {
        float vol;
        HRESULT hr = stuff->volume->GetMasterVolume(&vol);
        if(SUCCEEDED(hr)) {
            return vol;
        }
        else {
            DBG_PRINT << "Couldn't get session volume (" << hr << ")";
        }
    }
    return 0.0f;
}

void SessionVolumeModel::setVolume(float volume) {
    if(stuff->volume.GetInterfacePtr()) {
        HRESULT hr = stuff->volume->SetMasterVolume(volume, &stuff->eventContext);
        if(FAILED(hr)) {
            DBG_PRINT << "Couldn't set session volume (" << hr << ")";
        }
    }
}

bool SessionVolumeModel::muted() {
    if(stuff->volume.GetInterfacePtr()) {
        BOOL mute;
        HRESULT hr = stuff->volume->GetMute(&mute);
        if(SUCCEEDED(hr)) {
            return mute;
        }
        else {
            DBG_PRINT << "Couldn't get session mutedness (" << hr << ")";
        }
    }
    return false;
}

void SessionVolumeModel::setMuted(bool muted) {
    if(stuff->volume.GetInterfacePtr()) {
        HRESULT hr = stuff->volume->SetMute(muted, &stuff->eventContext);
        if(FAILED(hr)) {
            DBG_PRINT << "Couldn't set session mutedness (" << hr << ")";
        }
    }
}

bool SessionVolumeModel::currentlyHasVolume() {
    return stuff->volume.GetInterfacePtr() != nullptr;
}

void SessionVolumeModel::setName(QString newName) {
    stuff->session->SetDisplayName((const wchar_t*)newName.utf16(), &stuff->eventContext);
}

void SessionVolumeModel::setIconPath(QString newIconPath) {
    stuff->session->SetDisplayName((const wchar_t*)newIconPath.utf16(), &stuff->eventContext);
}

QString SessionVolumeModel::name() {
    LPWSTR lpwDisplayName;
    QString displayname;
    HRESULT hr = stuff->session->GetDisplayName(&lpwDisplayName);
    if(SUCCEEDED(hr)) {
        displayname = QString::fromWCharArray(lpwDisplayName);
        CoTaskMemFree(lpwDisplayName);
    }
    else {
        DBG_PRINT << "Couldn't get session display name (" << hr << ")";
        if(displayname.length() > 0) {
            displayname = GetStringByPossibleResource(displayname);
        }
        else { // As usual, the display name isn't set. Time to guess.
            DWORD pid;
            hr = stuff->session->GetProcessId(&pid);
            assertHR(hr, QString("Couldn't get session process id (%1)"));
            displayname = GuessNameForPid(pid);
        }
    }
    return displayname;
}

QString SessionVolumeModel::iconPath() {
    LPWSTR lpwIconPath;
    QString path;
    HRESULT hr = stuff->session->GetIconPath(&lpwIconPath);
    if(SUCCEEDED(hr)) {
        path = QString::fromWCharArray(lpwIconPath);
        CoTaskMemFree(lpwIconPath);
    }
    else {
        DBG_PRINT << "Couldn't get session icon path (" << hr << ")";
        // TODO: Guess
        path = QString("");
    }
    return path;
}

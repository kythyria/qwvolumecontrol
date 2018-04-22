#include "sessionvolumemodel.h"

#include "getresource.h"
#include "util.h"

COM_SMARTPTR(ISimpleAudioVolume);

class SessionVolumeModel::Internal : public IAudioSessionEvents {
    ULONG refcount;
    
public:
    IAudioSessionControl2Ptr session;
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
    target->metaObject()->invokeMethod(target, method, Qt::QueuedConnection, ##__VA_ARGS__)

HRESULT SessionVolumeModel::Internal::OnChannelVolumeChanged(DWORD ChannelCount, float NewChannelVolumeArray[], DWORD ChangedChannel, LPCGUID EventContext) {
    (void)ChannelCount;
    (void)NewChannelVolumeArray;
    (void)ChangedChannel;
    if(!IsEqualGUID(*EventContext, eventContext)) {
        DEFER_EMIT(model, &SessionVolumeModel::changed);
    }
    return S_OK;
}

HRESULT SessionVolumeModel::Internal::OnDisplayNameChanged(LPCWSTR NewDisplayName, LPCGUID EventContext) {
    if(!IsEqualGUID(*EventContext, eventContext)) {
        QString newname = QString::fromWCharArray(NewDisplayName);
        emit model->nameChanged(newname);
    }
    return S_OK;
}

HRESULT SessionVolumeModel::Internal::OnGroupingParamChanged(LPCGUID NewGroupingParam, LPCGUID EventContext) {
    return S_OK;
}

HRESULT SessionVolumeModel::Internal::OnIconPathChanged(LPCWSTR NewIconPath, LPCGUID EventContext) {
    if(!IsEqualGUID(*EventContext, eventContext)) {
        QString newpath = QString::fromWCharArray(NewIconPath);
        emit model->iconPathChanged(newpath);
    }
    return S_OK;
}

HRESULT SessionVolumeModel::Internal::OnSessionDisconnected(AudioSessionDisconnectReason DisconnectReason) {
    (void)DisconnectReason;
    emit model->sessionDisconnected();
    return S_OK;
}

HRESULT SessionVolumeModel::Internal::OnSimpleVolumeChanged(float NewVolume, BOOL NewMute, LPCGUID EventContext) {
    (void)NewVolume;
    (void)NewMute;
    if(!IsEqualGUID(*EventContext, eventContext)) {
        emit model->changed();
    }
    return S_OK;
}

HRESULT SessionVolumeModel::Internal::OnStateChanged(AudioSessionState NewState) {
    emit model->stateChanged(NewState);
    return S_OK;
}

uint SessionVolumeModel::channelCount() {
    return 0;
}

uint SessionVolumeModel::channelLayoutMask() {
    return 0;
}

float SessionVolumeModel::channelVolume(uint channel) {
    return 0;
}

void SessionVolumeModel::setChannelVolume(uint channel, float volume) {
    
}

float SessionVolumeModel::volume() {
    return 0;
}

void SessionVolumeModel::setVolume(float volume) {
    
}

bool SessionVolumeModel::muted() {
    return true;
}

void SessionVolumeModel::setMuted(bool muted) {
    
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

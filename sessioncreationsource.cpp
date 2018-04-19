#include "sessioncreationsource.h"

#include <QThread>
#include <QDebug>
#include <objbase.h>
#include <util.h>

namespace MetatypeId {
static RegisterMetatype<IAudioSessionControl2Ptr> IAudioSessionControl2Ptr;
}

COM_SMARTPTR(IAudioSessionEnumerator);
COM_SMARTPTR(IAudioSessionControl);

class SessionCreationSource::SessionNotificationCallback : IAudioSessionNotification {
    ULONG refcount;
    SessionCreationSource *manager;
    
public:
    SessionNotificationCallback(SessionCreationSource *src);
    virtual ~SessionNotificationCallback();
    
    // IUnknown
    virtual ULONG AddRef();
    virtual ULONG Release();
    virtual HRESULT QueryInterface(REFIID riid, void **ppvObject);
    
    // IAudioSessionNotification
    
    virtual HRESULT OnSessionCreated(IAudioSessionControl *NewSession);
};

SessionCreationSource::SessionNotificationCallback::SessionNotificationCallback(SessionCreationSource *src) :
    refcount(0),
    manager(src)
{ }

SessionCreationSource::SessionNotificationCallback::~SessionNotificationCallback() { }

ULONG STDMETHODCALLTYPE SessionCreationSource::SessionNotificationCallback::AddRef() {
    return InterlockedIncrement(&refcount);
}

ULONG STDMETHODCALLTYPE SessionCreationSource::SessionNotificationCallback::Release() {
    ULONG ulRef = InterlockedDecrement(&refcount);
    if (0 == ulRef) {
        delete this;
    }
    return ulRef;
}

HRESULT STDMETHODCALLTYPE SessionCreationSource::SessionNotificationCallback::QueryInterface(REFIID riid, void **ppvInterface) {
    if (IID_IUnknown == riid) {
        AddRef();
        *ppvInterface = (IUnknown*)this;
    }
    else if (__uuidof(IAudioSessionNotification) == riid) {
        AddRef();
        *ppvInterface = (IAudioSessionNotification*)this;
    }
    else {
        *ppvInterface = NULL;
        return E_NOINTERFACE;
    }
    return S_OK;
}

HRESULT SessionCreationSource::SessionNotificationCallback::OnSessionCreated(IAudioSessionControl *NewSession) {
    IAudioSessionControl2Ptr s2;
    HRESULT hr = NewSession->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&s2);
    if(FAILED(hr)) {
        qDebug() << "Couldn't cast session (" << hr << ")";
    }
    
    emit manager->sessionExists(s2);
    return S_OK;
}

class SessionNotificationFactoryThread : QThread {
protected:
    void run() {
        auto hr = CoInitializeEx(0, COINIT_MULTITHREADED);
        
        if(FAILED(hr)) {
            QString failmsg = QString("CoInitializeEx failed in session factory thread (%0)").arg(hr);
            wchar_t *msgdata = new wchar_t[failmsg.length()];
            failmsg.toWCharArray(msgdata);
            MessageBox(NULL, msgdata, L"Volume Control", MB_OK);
            delete[] msgdata;
            return;
        }
        
        this->exec();
        
        CoUninitialize();
    }
    
private:
    static SessionNotificationFactoryThread *thread;
public:
    SessionNotificationFactoryThread *runningThread() { 
        if(thread == nullptr) {
            thread = new SessionNotificationFactoryThread();
            thread->start();
        }
        
        return thread;
    }
};

SessionNotificationFactoryThread *SessionNotificationFactoryThread::thread = nullptr;

SessionCreationSource::SessionCreationSource(IAudioSessionManager2Ptr sessionManager, QObject *parent) :
    QObject(parent),
    manager(manager)
{
    this->callback = new SessionNotificationCallback(this);
    
    HRESULT hr;
    hr = this->callback->QueryInterface(__uuidof(IAudioSessionNotification), (void**)&callback_as_callback);
    assertHR(hr, "Somehow failed to cast the session notification callback (%0)");
    
    this->manager->RegisterSessionNotification(this->callback_as_callback);
}

SessionCreationSource::~SessionCreationSource() {
    this->manager->UnregisterSessionNotification(this->callback_as_callback);
    delete this->callback;
}

void SessionCreationSource::triggerEnumeration() {
    IAudioSessionEnumeratorPtr enumer;
    int sessionCount;
    
    HRESULT hr;
    
    hr = this->manager->GetSessionEnumerator(&enumer);
    assertHR(hr, "Failed to get session enumerator (%0)");
    
    hr = enumer->GetCount(&sessionCount);
    assertHR(hr, "Failed to get session count (%0)");
    
    for(int i = 0; i < sessionCount; i++) {
        IAudioSessionControlPtr session;
        hr = enumer->GetSession(i, &session);
        if(FAILED(hr)) {
            qDebug() << "Couldn't get session " << i;
        }
        
        IAudioSessionControl2Ptr s2;
        hr = session.QueryInterface(__uuidof(IAudioSessionControl2), &s2);
        if(FAILED(hr)) {
            qDebug() << "Couldn't cast session " << i;
        }
        emit sessionExists(s2);
    }
}

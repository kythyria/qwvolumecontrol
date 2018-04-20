#include "sessionmixerlistwidget.h"
#include "sessioncreationsource.h"

#include <QDebug>
#include <QVBoxLayout>
#include "util.h"

#include "sessionmixerwidget.h"

class SessionMixerListWidget::Internal {
    
public:
    SessionCreationSource *sessionSource;
    QVBoxLayout *list;
    
    Internal();
    virtual ~Internal();
    
};

SessionMixerListWidget::SessionMixerListWidget(IAudioSessionManager2Ptr smgr)
{
    stuff = std::make_unique<Internal>();
    stuff->sessionSource = new SessionCreationSource(smgr, this);
    stuff->list = new QVBoxLayout();
    this->setLayout(stuff->list);
    connect(stuff->sessionSource, &SessionCreationSource::sessionExists, this, &SessionMixerListWidget::sessionExists);
    stuff->sessionSource->triggerEnumeration();
}

SessionMixerListWidget::~SessionMixerListWidget() { }

SessionMixerListWidget::Internal::Internal()  { }
SessionMixerListWidget::Internal::~Internal() { }

void SessionMixerListWidget::sessionExists(IAudioSessionControl2 *s) {
    IAudioSessionControl2Ptr session(s, true);
    
    LPWSTR sessioniid;
    HRESULT hr = session->GetSessionInstanceIdentifier(&sessioniid);
    if(FAILED(hr)) {
        qDebug() << "Failed to get session instance identifier (" << hr << ")";
    }
    else {
        qDebug() << "Saw session: " << QString::fromWCharArray(sessioniid);
        CoTaskMemFree(sessioniid);
    }
    
    stuff->list->addWidget(new SessionMixerWidget(session, this));
}

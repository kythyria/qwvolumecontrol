#include "sessionmixerlistwidget.h"
#include "sessioncreationsource.h"

#include <QDebug>
#include <QVBoxLayout>
#include "util.h"

#include "sessionvolumemodel.h"
#include "devicemixerwidget.h"

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
    
    QSharedPointer<AbstractVolumeModel> svm(new SessionVolumeModel(session));
    
    stuff->list->addWidget(new DeviceMixerWidget(svm, this));
}

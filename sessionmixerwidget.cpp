#include "sessionmixerwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QLabel>
#include <QSlider>

#include "util.h"
#include "getresource.h"
#include <QDebug>

class SessionMixerWidget::Internal {
public:
    IAudioSessionControl2Ptr session;
    
    QVBoxLayout *headerTextLayout;
    
    QLabel *lblDisplayName;
    QLabel *lblInstanceId;
    
    void InitWidgets();
    void PopulateWidgets();
};

SessionMixerWidget::SessionMixerWidget(IAudioSessionControl2Ptr session, QWidget *parent) : QWidget(parent)
{
    stuff = std::make_unique<Internal>();
    stuff->session = session;
    stuff->InitWidgets();
    stuff->PopulateWidgets();
    this->setLayout(this->stuff->headerTextLayout);
}

void SessionMixerWidget::Internal::InitWidgets() {
    lblDisplayName = new QLabel("Display Name");
    lblInstanceId = new QLabel("Instance ID");
    
    headerTextLayout = new QVBoxLayout();
    headerTextLayout->addWidget(lblDisplayName);
    headerTextLayout->addWidget(lblInstanceId);
}

void SessionMixerWidget::Internal::PopulateWidgets() {
    HRESULT hr;

    LPWSTR lpwInstanceid;
    hr = session->GetSessionInstanceIdentifier(&lpwInstanceid);
    QString instanceid = QString::fromWCharArray(lpwInstanceid);
    
    assertHR(hr, "Couldn't get session instance id (%0)");
    lblInstanceId->setText(instanceid);
    
    LPWSTR lpwDisplayName;
    hr = session->GetDisplayName(&lpwDisplayName);
    QString displayname = QString::fromWCharArray(lpwDisplayName);
    assertHR(hr, QString("Couldn't get session display name (%1)\n%0").arg(instanceid));
    displayname = QString::fromWCharArray(lpwDisplayName);
    if(displayname.length() > 0) {
        displayname = GetStringByPossibleResource(displayname);
    }
    else { // As usual, the display name isn't set. Time to guess.
        
    }
    lblDisplayName->setText(displayname);
    
    CoTaskMemFree(lpwInstanceid);
    CoTaskMemFree(lpwDisplayName);
}

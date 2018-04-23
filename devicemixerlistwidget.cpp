#include "devicemixerlistwidget.h"

#include <mmdeviceapi.h>
#include <QVBoxLayout>
#include <QLabel>

#include "util.h"
#include "devicemixerwidget.h"
#include "devicevolumemodel.h"
#include "audiopolicy.h"
#include "sessionmixerlistwidget.h"

COM_SMARTPTR(IMMDevice);
COM_SMARTPTR(IAudioSessionManager2);

class DeviceMixerListWidget_internal {
public:
    IMMDeviceCollectionPtr devices;
    QVBoxLayout *vbox;
};

DeviceMixerListWidget::DeviceMixerListWidget(IMMDeviceCollectionPtr devices, QWidget *parent)
    : QWidget(parent) {
    stuff = std::make_unique<DeviceMixerListWidget_internal>();
    stuff->devices = devices;
    
    stuff->vbox = new QVBoxLayout();
    this->setLayout(stuff->vbox);
    
    UpdateListing();
}

DeviceMixerListWidget::~DeviceMixerListWidget() { }

void DeviceMixerListWidget::UpdateListing() {
    QLayoutItem *child;
    while ((child = stuff->vbox->takeAt(0)) != 0) {
        delete child;
    }
    
    uint numDevices;
    
    if(!AlertHresult(stuff->devices->GetCount(&numDevices), "Couldn't get device count(%0)")) {
        return;
    }
    
    for(uint i = 0; i < numDevices; i++) {
        IMMDevicePtr dev;
        if(!AlertHresult(stuff->devices->Item(i, &dev), QString("Couldn't get device %0 (%1)").arg(i))) {
            return;
        }
        
        auto dvm = new DeviceVolumeModel(dev);
        
        DeviceMixerWidget *dmw = new DeviceMixerWidget(dvm);
        connect(dmw, &DeviceMixerWidget::detailButtonClicked, this, &DeviceMixerListWidget::showSessions);
        stuff->vbox->addWidget(dmw);
    }
}

void DeviceMixerListWidget::showSessions() {
    auto dmw = (DeviceMixerWidget*)sender();
    auto dvm = dynamic_cast<DeviceVolumeModel*>(dmw->model());
    
    IAudioSessionManager2Ptr iasm;
    HRESULT hr = dvm->device()->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void**)&iasm);
    if(SUCCEEDED(hr)){
        auto s = new SessionMixerListWidget(iasm);
        s->show();
    }
}

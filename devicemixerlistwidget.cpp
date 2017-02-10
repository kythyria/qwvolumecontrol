#include "devicemixerlistwidget.h"

#include <mmdeviceapi.h>
#include <QVBoxLayout>
#include <QLabel>

#include "util.h"
#include "devicemixerwidget.h"

_COM_SMARTPTR_TYPEDEF(IMMDevice, __uuidof(IMMDevice));

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
        
        DeviceMixerWidget *dmw = new DeviceMixerWidget(dev);
        stuff->vbox->addWidget(dmw);
    }
}

DeviceMixerListWidget::~DeviceMixerListWidget() { }

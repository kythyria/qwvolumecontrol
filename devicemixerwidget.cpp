#include "devicemixerwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QLabel>
#include <Functiondiscoverykeys_devpkey.h>

#include "util.h"

_COM_SMARTPTR_TYPEDEF(IPropertyStore, __uuidof(IPropertyStore));

class DeviceMixerWidget::Internals {
public:
    IMMDevicePtr device;
    
    QVBoxLayout *vbox;
    QHBoxLayout *headerLayout;
    QVBoxLayout *headerTextLayout;
    
    QWidget *iconWidget;
    
    QLabel *lblDeviceDesc;
    QLabel *lblDeviceName;
    QLabel *lblStatus;
    
    QPushButton *btnMute;
    QPushButton *btnLinkChannels;
};

DeviceMixerWidget::DeviceMixerWidget(IMMDevicePtr device, QWidget *parent) : QWidget(parent) {
    stuff = new Internals();
    stuff->device = device;
    
    IPropertyStorePtr props;
    HRESULT hr;
    hr = device->OpenPropertyStore(STGM_READ, &props);
    
    assertHR(hr, "Couldn't open device property store (%0)");
    
    PROPVARIANT pv;
    
    stuff->lblDeviceDesc = new QLabel();
    hr = props->GetValue(PKEY_Device_DeviceDesc, &pv);
    assertHR(hr, "Couldn't get device description (%0)");
    stuff->lblDeviceDesc->setText(QString::fromWCharArray(pv.pwszVal));
    PropVariantClear(&pv);
    
    stuff->lblDeviceName = new QLabel();
    hr = props->GetValue(PKEY_DeviceInterface_FriendlyName, &pv);
    assertHR(hr, QString("Couldn't get device friendly name for %0 (%1)").arg(stuff->lblDeviceDesc->text()));
    stuff->lblDeviceName->setText(QString::fromWCharArray(pv.pwszVal));
    PropVariantClear(&pv);
    
    stuff->lblStatus = new QLabel("Status");
    
    stuff->headerTextLayout = new QVBoxLayout();
    stuff->headerTextLayout->addWidget(stuff->lblDeviceDesc);
    stuff->headerTextLayout->addWidget(stuff->lblDeviceName);
    stuff->headerTextLayout->addWidget(stuff->lblStatus);
    
    QFrame *iconWidget = new QFrame();
    iconWidget->setFrameStyle(QFrame::Box);
    iconWidget->setBackgroundRole(QPalette::Text);
    iconWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    iconWidget->setMinimumSize(48,48);
    iconWidget->resize(48,48);
    stuff->iconWidget = iconWidget;
    
    stuff->btnMute = new QPushButton();
    stuff->btnLinkChannels = new QPushButton();
    
    stuff->headerLayout = new QHBoxLayout();
    stuff->headerLayout->addWidget(stuff->iconWidget);
    stuff->headerLayout->addLayout(stuff->headerTextLayout, 1);
    stuff->headerLayout->addWidget(stuff->btnMute, 0, Qt::AlignTop);
    stuff->headerLayout->addWidget(stuff->btnLinkChannels, 0, Qt::AlignTop);
    
    stuff->vbox = new QVBoxLayout();
    stuff->vbox->addLayout(stuff->headerLayout);
    
    this->setLayout(stuff->vbox);
}

DeviceMixerWidget::~DeviceMixerWidget() {
    delete stuff;
}

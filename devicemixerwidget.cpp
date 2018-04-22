#include "devicemixerwidget.h"

#include "math.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QLabel>
#include <QSlider>

#include <Functiondiscoverykeys_devpkey.h>
#include <endpointvolume.h>

#include "util.h"
#include "devicevolumemodel.h"
#include "volumesliderwidget.h"
#include "sessionmixerlistwidget.h"

COM_SMARTPTR(IPropertyStore);
COM_SMARTPTR(IAudioEndpointVolume);

class DeviceMixerWidget::Internals {
public:
    IMMDevicePtr device;
    IAudioEndpointVolumePtr volume;
    DeviceVolumeModel *dvm;
    
    QVBoxLayout *vbox;
    QHBoxLayout *headerLayout;
    QVBoxLayout *headerTextLayout;
    
    QWidget *iconWidget;
    
    QLabel *lblDeviceDesc;
    QLabel *lblDeviceName;
    QLabel *lblStatus;
    
    QPushButton *btnShowSessions;
    QPushButton *btnMute;
    QPushButton *btnLinkChannels;
    
    VolumeSliderWidget *sliders;
    
    void InitHeaderWidgets();
    void PopulateHeaderWidgets();
};

DeviceMixerWidget::DeviceMixerWidget(IMMDevicePtr device, QWidget *parent) : QWidget(parent) {
    stuff = new Internals();
    stuff->device = device;
    
    stuff->InitHeaderWidgets();
    stuff->PopulateHeaderWidgets();
    
    DWORD state;
    HRESULT hr = device->GetState(&state);
    
    stuff->dvm = new DeviceVolumeModel(stuff->device, this);
    
    if(stuff->dvm->currentlyHasVolume()) {
        IAudioEndpointVolume **paev = &(stuff->volume);
        HRESULT hr = device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)paev);
        AlertHresult(hr, QString("Unable to open the volume control for %0 (%1)").arg(stuff->lblDeviceName->text()));
        
        
        
        stuff->sliders = new VolumeSliderWidget(stuff->dvm);
        stuff->vbox->addWidget(stuff->sliders);
        connect(stuff->btnLinkChannels, &QPushButton::toggled, stuff->sliders, &VolumeSliderWidget::linkChannels);
        stuff->btnLinkChannels->setChecked(true);
        
        connect(stuff->dvm, &DeviceVolumeModel::volumeChanged, this, &DeviceMixerWidget::refresh);
        connect(stuff->btnShowSessions, &QPushButton::clicked, this, [this]() {
            IAudioSessionManager2Ptr iasm;
            HRESULT hr = this->stuff->device->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void**)&iasm);
            if(SUCCEEDED(hr)){
                auto s = new SessionMixerListWidget(iasm);
                s->show();
            }
        });
    }
    
    this->setLayout(stuff->vbox);
}

DeviceMixerWidget::~DeviceMixerWidget() {
    delete stuff;
}

void DeviceMixerWidget::Internals::InitHeaderWidgets() {
    lblDeviceDesc = new QLabel("Device Description");
    lblDeviceName = new QLabel("(Unknown Device)");
    lblStatus = new QLabel("Status");
    
    headerTextLayout = new QVBoxLayout();
    headerTextLayout->addWidget(lblDeviceDesc);
    headerTextLayout->addWidget(lblDeviceName);
    headerTextLayout->addWidget(lblStatus);
    
    QFrame *iconWidgetFrame = new QFrame();
    iconWidgetFrame->setFrameStyle(QFrame::Box);
    iconWidgetFrame->setBackgroundRole(QPalette::Text);
    iconWidgetFrame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    iconWidgetFrame->setMinimumSize(48,48);
    iconWidgetFrame->resize(48,48);
    iconWidget = iconWidgetFrame;
    
    btnShowSessions = new QPushButton("Sessions");
    btnMute = new QPushButton("Mute");
    btnLinkChannels = new QPushButton("Link");
    btnLinkChannels->setCheckable(true);
    
    headerLayout = new QHBoxLayout();
    headerLayout->addWidget(iconWidget);
    headerLayout->addLayout(headerTextLayout, 1);
    headerLayout->addWidget(btnMute, 0, Qt::AlignTop);
    headerLayout->addWidget(btnLinkChannels, 0, Qt::AlignTop);
    headerLayout->addWidget(btnShowSessions, 0, Qt::AlignTop);
    
    vbox = new QVBoxLayout();
    vbox->addLayout(headerLayout);
}

void DeviceMixerWidget::Internals::PopulateHeaderWidgets() {
    IPropertyStorePtr props;
    HRESULT hr;
    hr = device->OpenPropertyStore(STGM_READ, &props);
    
    assertHR(hr, "Couldn't open device property store (%0)");
    
    PROPVARIANT pv;
    
    hr = props->GetValue(PKEY_Device_DeviceDesc, &pv);
    assertHR(hr, "Couldn't get device description (%0)");
    lblDeviceDesc->setText(QString::fromWCharArray(pv.pwszVal));
    PropVariantClear(&pv);
    
    hr = props->GetValue(PKEY_DeviceInterface_FriendlyName, &pv);
    //if(AlertHresult(hr, QString("Couldn't get device friendly name for %0 (%1)").arg(lblDeviceDesc->text()))) {
    if(SUCCEEDED(hr)) {
        lblDeviceName->setText(QString::fromWCharArray(pv.pwszVal));
    }
    PropVariantClear(&pv);
}

void DeviceMixerWidget::refresh() {
}

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
    
    stuff->dvm = new DeviceVolumeModel(stuff->device, this);
    stuff->PopulateHeaderWidgets();
    
    if(stuff->dvm->currentlyHasVolume()) {
        stuff->sliders = new VolumeSliderWidget(stuff->dvm);
        stuff->vbox->addWidget(stuff->sliders);
        connect(stuff->btnLinkChannels, &QPushButton::toggled, stuff->sliders, &VolumeSliderWidget::linkChannels);
        stuff->btnLinkChannels->setChecked(true);
        
        connect(stuff->dvm, &AbstractVolumeModel::muteChanged, stuff->btnMute, &QPushButton::setChecked);
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
    lblDeviceDesc->setText(dvm->description());
    lblDeviceName->setText(dvm->name());
}

void DeviceMixerWidget::refresh() {
}

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
#include "abstractvolumemodel.h"
#include "volumesliderwidget.h"
#include "sessionmixerlistwidget.h"

COM_SMARTPTR(IPropertyStore);
COM_SMARTPTR(IAudioEndpointVolume);

class DeviceMixerWidget::Internals {
public:
    IMMDevicePtr device;
    QSharedPointer<AbstractVolumeModel> dvm;
    
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

DeviceMixerWidget::DeviceMixerWidget(QSharedPointer<AbstractVolumeModel> device, QWidget *parent) : QWidget(parent) {
    stuff = new Internals();
    
    stuff->InitHeaderWidgets();
    
    stuff->dvm = device;
    stuff->dvm->setParent(this);
    stuff->PopulateHeaderWidgets();
    
    if(stuff->dvm->currentlyHasVolume()) {
        stuff->sliders = new VolumeSliderWidget(stuff->dvm);
        stuff->vbox->addWidget(stuff->sliders);
        connect(stuff->btnLinkChannels, &QPushButton::toggled, stuff->sliders, &VolumeSliderWidget::linkChannels);
        stuff->btnLinkChannels->setChecked(true);
        connect(stuff->btnMute, &QPushButton::clicked, stuff->dvm.data(), &AbstractVolumeModel::setMuted);
        
        connect(stuff->dvm.data(), &AbstractVolumeModel::muteChanged, this, &DeviceMixerWidget::muteClicked);
        connect(stuff->btnShowSessions, &QPushButton::clicked, this, &DeviceMixerWidget::detailButtonClicked);
    }
    
    this->setLayout(stuff->vbox);
}

DeviceMixerWidget::~DeviceMixerWidget() {
    delete stuff;
}

void DeviceMixerWidget::Internals::InitHeaderWidgets() {
    lblDeviceDesc = new QLabel("Device Description");
    lblDeviceName = new QLabel("(Unknown Device)");
    lblStatus = new QLabel("");//("Status");
    
    headerTextLayout = new QVBoxLayout();
    headerTextLayout->addWidget(lblDeviceName);
    headerTextLayout->addWidget(lblDeviceDesc);
    headerTextLayout->addWidget(lblStatus);
    
    QFrame *iconWidgetFrame = new QFrame();
    iconWidgetFrame->setFrameStyle(QFrame::Box);
    iconWidgetFrame->setBackgroundRole(QPalette::Text);
    iconWidgetFrame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    iconWidgetFrame->setMinimumSize(48,48);
    iconWidgetFrame->resize(48,48);
    iconWidget = iconWidgetFrame;
    
    btnShowSessions = new QPushButton("Details");
    btnMute = new QPushButton(QString::fromWCharArray(L"\uE198")); // mute icon
    btnMute->setFont(QFont("Segoe UI Symbol"));
    btnMute->setCheckable(true);
    btnLinkChannels = new QPushButton("Link");
    btnLinkChannels->setCheckable(true);
    
    headerLayout = new QHBoxLayout();
    headerLayout->addWidget(iconWidget);
    headerLayout->addLayout(headerTextLayout, 1);
    headerLayout->addWidget(btnLinkChannels, 0, Qt::AlignTop);
    headerLayout->addWidget(btnShowSessions, 0, Qt::AlignTop);
    headerLayout->addWidget(btnMute, 0, Qt::AlignTop);
    
    vbox = new QVBoxLayout();
    vbox->addLayout(headerLayout);
}

void DeviceMixerWidget::Internals::PopulateHeaderWidgets() {
    lblDeviceDesc->setText(dvm->description());
    lblDeviceName->setText(dvm->name());
}

QSharedPointer<AbstractVolumeModel> DeviceMixerWidget::model() {
    return stuff->dvm;
}

void DeviceMixerWidget::refresh() {
}

void DeviceMixerWidget::muteClicked(bool checked) {
    stuff->dvm->setMuted(checked);
    auto label = checked ? L"\uE198" : L"\uE15D";
    stuff->btnMute->setText(QString::fromWCharArray(label));
}

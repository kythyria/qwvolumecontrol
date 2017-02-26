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

_COM_SMARTPTR_TYPEDEF(IPropertyStore, __uuidof(IPropertyStore));
_COM_SMARTPTR_TYPEDEF(IAudioEndpointVolume, __uuidof(IAudioEndpointVolume));

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
    
    QPushButton *btnMute;
    QPushButton *btnLinkChannels;
    
    QHBoxLayout *masterSliderLayout;
    QSlider *masterSlider;
    QLabel *masterSliderText;
    
    VolumeSliderWidget *sliders;
    
    void InitHeaderWidgets();
    void PopulateHeaderWidgets();
    void createMasterSlider();
};

DeviceMixerWidget::DeviceMixerWidget(IMMDevicePtr device, QWidget *parent) : QWidget(parent) {
    stuff = new Internals();
    stuff->device = device;
    
    stuff->InitHeaderWidgets();
    stuff->PopulateHeaderWidgets();
    
    DWORD state;
    HRESULT hr = device->GetState(&state);
    
    stuff->dvm = nullptr;
    
    if(SUCCEEDED(hr) && state == DEVICE_STATE_ACTIVE) {
        IAudioEndpointVolume **paev = &(stuff->volume);
        HRESULT hr = device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void**)paev);
        AlertHresult(hr, QString("Unable to open the volume control for %0 (%1)").arg(stuff->lblDeviceName->text()));
        
        stuff->dvm = new DeviceVolumeModel(stuff->device, this);
        stuff->createMasterSlider();
        
        stuff->sliders = new VolumeSliderWidget(stuff->dvm);
        stuff->vbox->addWidget(stuff->sliders);
        
        connect(stuff->dvm, &DeviceVolumeModel::changed, this, &DeviceMixerWidget::refresh);
        connect(stuff->masterSlider, &QSlider::valueChanged, this, &DeviceMixerWidget::masterSliderChanged);
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
    
    btnMute = new QPushButton();
    btnLinkChannels = new QPushButton();
    
    headerLayout = new QHBoxLayout();
    headerLayout->addWidget(iconWidget);
    headerLayout->addLayout(headerTextLayout, 1);
    headerLayout->addWidget(btnMute, 0, Qt::AlignTop);
    headerLayout->addWidget(btnLinkChannels, 0, Qt::AlignTop);
    
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

void DeviceMixerWidget::Internals::createMasterSlider() {
    masterSliderLayout = new QHBoxLayout();
    
    masterSlider = new QSlider();
    masterSlider->setMinimum(0);
    masterSlider->setMaximum(100);
    masterSlider->setOrientation(Qt::Horizontal);
    
    float vol = 0.0f;
    volume->GetMasterVolumeLevelScalar(&vol);
    masterSlider->setValue(roundf(vol*100.0f));
    
    masterSliderLayout->addWidget(masterSlider);
    vbox->addLayout(masterSliderLayout);
}

void DeviceMixerWidget::refresh() {
    float vol = stuff->dvm->volume();
    stuff->masterSlider->setValue(roundf(vol*100.0f));
}

void DeviceMixerWidget::masterSliderChanged(int val) {
    float vol = val / 100.0f;
    stuff->dvm->setVolume(vol);
}

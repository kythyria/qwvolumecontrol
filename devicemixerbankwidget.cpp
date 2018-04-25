#include "devicemixerbankwidget.h"

#include "devicemixerwidget.h"
#include "sessionmixerlistwidget.h"
#include "devicevolumemodel.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QStyle>

class DeviceMixerBankWidget::Internal {
public:
    QAbstractItemModel *devices;
    QSharedPointer<AbstractVolumeModel> currentDevice;
    
    QVBoxLayout *layout;
    QHBoxLayout *devicemixerpad;
    DeviceMixerWidget *devicemixer;
    QScrollArea *sessionBankArea;
    
    void setSessionDisplay();
};

DeviceMixerBankWidget::DeviceMixerBankWidget(QAbstractItemModel *devices, QWidget *parent) :
    QWidget(parent),
    stuff(new Internal)
{
    stuff->devices = devices;
    
    stuff->layout = new QVBoxLayout();
    stuff->devicemixer = new DeviceMixerWidget(stuff->devices);
    stuff->sessionBankArea = new QScrollArea();
    stuff->sessionBankArea->setWidgetResizable(true);
    stuff->sessionBankArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    stuff->sessionBankArea->setStyleSheet("QScrollArea { border-top: 1px solid palette(shadow); }");
    
    stuff->devicemixerpad = new QHBoxLayout();
    stuff->devicemixerpad->addWidget(stuff->devicemixer);
    
    auto sty = style();
    int scrollbarwidth = sty->pixelMetric(QStyle::PM_ScrollBarExtent);
    stuff->devicemixerpad->addSpacing(scrollbarwidth);
    
    stuff->layout->addLayout(stuff->devicemixerpad);
    stuff->layout->addWidget(stuff->sessionBankArea, 1);
    
    connect(stuff->devicemixer, &DeviceMixerWidget::selectedIndexChanged, this, &DeviceMixerBankWidget::selectedDeviceChanged_internal);
    stuff->currentDevice = stuff->devicemixer->model();
    stuff->setSessionDisplay();
    
    this->setLayout(stuff->layout);
}

void DeviceMixerBankWidget::Internal::setSessionDisplay() {
    auto dvm = qSharedPointerDynamicCast<DeviceVolumeModel>(currentDevice);
    IAudioSessionManager2Ptr iasm;
    HRESULT hr = dvm->device()->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL, NULL, (void**)&iasm);
    if(SUCCEEDED(hr)){
        auto s = new SessionMixerListWidget(iasm);
        sessionBankArea->setWidget(s);
        s->setAutoFillBackground(false);
    }
}

void DeviceMixerBankWidget::selectedDeviceChanged_internal(int index) {
    auto qi = stuff->devicemixer->selectorModel()->index(index, 0);
    auto qv = stuff->devicemixer->selectorModel()->data(qi, Qt::UserRole);
    stuff->currentDevice = qv.value<QSharedPointer<AbstractVolumeModel>>();
    
    stuff->setSessionDisplay();
}

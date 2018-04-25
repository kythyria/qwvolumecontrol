#include "devicemixerwidget.h"

#include "math.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QAbstractItemModel>
#include <QComboBox>

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
    QAbstractItemModel *selectorModel;
    
    QVBoxLayout *vbox;
    QHBoxLayout *headerLayout;
    QVBoxLayout *headerTextLayout;
    
    QWidget *iconWidget;
    
    QComboBox *cbxDeviceName;
    QLabel *lblDeviceName;
    QLabel *lblDeviceDesc;
    QLabel *lblStatus;
    
    QPushButton *btnShowSessions;
    QPushButton *btnMute;
    QPushButton *btnLinkChannels;
    
    VolumeSliderWidget *sliders;
    
    void InitHeaderWidgets();
};

DeviceMixerWidget::DeviceMixerWidget(QWidget *parent) : QFrame(parent) {
    stuff = new Internals();
    stuff->selectorModel = nullptr;
    stuff->InitHeaderWidgets();
    this->setLayout(stuff->vbox);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    //this->setFrameShape(QFrame::Box);
    connect(stuff->cbxDeviceName, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DeviceMixerWidget::selectedIndexChanged_internal);
}

DeviceMixerWidget::DeviceMixerWidget(QSharedPointer<AbstractVolumeModel> device, QWidget *parent) :
    DeviceMixerWidget(parent)
{
    setModel(device);
}

DeviceMixerWidget::DeviceMixerWidget(QAbstractItemModel *devices, QWidget *parent) :
    DeviceMixerWidget(parent)
{
    setSelectorModel(devices);
}

DeviceMixerWidget::~DeviceMixerWidget() {
    delete stuff;
}

void DeviceMixerWidget::Internals::InitHeaderWidgets() {
    cbxDeviceName = new QComboBox();
    lblDeviceDesc = new QLabel("Device Description");
    lblDeviceName = new QLabel("(Unknown Device)");
    //lblStatus = new QLabel("");//("Status");
    
    headerTextLayout = new QVBoxLayout();
    headerTextLayout->addWidget(cbxDeviceName);
    cbxDeviceName->hide();
    headerTextLayout->addWidget(lblDeviceName);
    headerTextLayout->addWidget(lblDeviceDesc);
    //headerTextLayout->addWidget(lblStatus);
    
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

void DeviceMixerWidget::setModel(QSharedPointer<AbstractVolumeModel> newModel) {
    if (stuff->sliders) {
        stuff->vbox->removeWidget(stuff->sliders);
        delete stuff->sliders;
    }
    
    stuff->dvm.clear();
    stuff->dvm = newModel;
    
    stuff->lblDeviceDesc->setText(stuff->dvm->description());
    stuff->lblDeviceName->setText(stuff->dvm->name());
    
    if(stuff->dvm->currentlyHasVolume()) {
        stuff->sliders = new VolumeSliderWidget(stuff->dvm);
        stuff->vbox->addWidget(stuff->sliders);
        connect(stuff->btnLinkChannels, &QPushButton::toggled, stuff->sliders, &VolumeSliderWidget::linkChannels);
        stuff->btnLinkChannels->setChecked(true);
        connect(stuff->btnMute, &QPushButton::clicked, stuff->dvm.data(), &AbstractVolumeModel::setMuted);
        
        connect(stuff->dvm.data(), &AbstractVolumeModel::muteChanged, this, &DeviceMixerWidget::muteClicked);
        connect(stuff->btnShowSessions, &QPushButton::clicked, this, &DeviceMixerWidget::detailButtonClicked);
    }
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

QAbstractItemModel *DeviceMixerWidget::selectorModel() {
    return stuff->selectorModel;
}

void DeviceMixerWidget::setSelectorModel(QAbstractItemModel *items) {
    if(!stuff->selectorModel) {
        stuff->lblDeviceName->hide();
        stuff->cbxDeviceName->show();
    }
    stuff->selectorModel = items;
    stuff->cbxDeviceName->setModel(items);
    stuff->cbxDeviceName->setCurrentIndex(0);
}

void DeviceMixerWidget::clearSelectorModel() {
    stuff->cbxDeviceName->hide();
    stuff->lblDeviceName->show();
    stuff->selectorModel = nullptr;
}

void DeviceMixerWidget::selectedIndexChanged_internal(int index) {
    auto qi = stuff->selectorModel->index(index, 0);
    auto qv = stuff->selectorModel->data(qi, Qt::UserRole);
    auto volumemodel = qv.value<QSharedPointer<AbstractVolumeModel>>();
    
    setModel(volumemodel);
    emit selectedIndexChanged(index);
}

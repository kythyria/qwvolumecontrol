#include "volmainwindow.h"

#include <QTabWidget>
#include <QScrollArea>
#include <stdlib.h>
#include <mmdeviceapi.h>

#include "devicemixerlistwidget.h"
#include "devicemixerbankwidget.h"
#include "devicecollectionmodel.h"
#include "util.h"
#include "comstuff.h"

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);

COM_SMARTPTR(IMMDeviceEnumerator);
//_COM_SMARTPTR_TYPEDEF(IMMDeviceCollection, __uuidof(IMMDeviceCollection));

class VolMainWindow_internal {
public:
    VolMainWindow *self;
    QTabWidget *tabs;
    //QScrollArea *playbackPage;
    DeviceMixerBankWidget *playbackPage;
    QScrollArea *recordingPage;
    QScrollArea *deviceDetailPage;
    QWidget *optionPage;
    
    IMMDeviceEnumeratorPtr devenum;
    
    DeviceCollectionModel *playbackDevices;
    
    VolMainWindow_internal(VolMainWindow *window);
};

VolMainWindow_internal::VolMainWindow_internal(VolMainWindow *window) {
    
    recordingPage    = new QScrollArea();
    recordingPage->setWidgetResizable(true);
    
    deviceDetailPage = new QScrollArea();
    optionPage       = new QWidget();
    
    tabs = new QTabWidget();
    tabs->addTab(recordingPage, "Recording");
    tabs->addTab(deviceDetailPage, "Device Details");
    tabs->addTab(optionPage, "Options");
    
    window->setCentralWidget(tabs);
}

VolMainWindow::VolMainWindow(QWidget *parent) : QMainWindow(parent) {
    this->resize(580,340);
    
    stuff = std::make_unique<VolMainWindow_internal>(this);
    
    HRESULT hr = CoCreateInstance(
           CLSID_MMDeviceEnumerator, NULL,
           CLSCTX_ALL, IID_IMMDeviceEnumerator,
           (void**)&stuff->devenum);
    
    assertHR(hr, "Couldn't create IMMDeviceEnumerator (%0)");
    
    IMMDeviceCollectionPtr col;
    
    stuff->playbackDevices = new DeviceCollectionModel(stuff->devenum, eRender, DEVICE_STATE_ACTIVE);
    stuff->playbackPage = new DeviceMixerBankWidget(stuff->playbackDevices);
    stuff->tabs->addTab(stuff->playbackPage, "Playback");
    
    hr = stuff->devenum->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &col);
    assertHR(hr, "Couldn't enumerate input devices (%0)");
    
    stuff->recordingPage->setWidget(new DeviceMixerListWidget(col));
}

VolMainWindow::~VolMainWindow() { }

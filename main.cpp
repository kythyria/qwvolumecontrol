#include <QApplication>
#include <QQmlApplicationEngine>
#include <QFile>
#include <objbase.h>

#include <volmainwindow.h>

int main(int argc, char *argv[]) {
    auto hr = CoInitializeEx(0, COINIT_APARTMENTTHREADED);
    
    if(FAILED(hr)) {
        QString failmsg = QString("CoInitializeEx failed (%0)").arg(hr);
        wchar_t *msgdata = new wchar_t[failmsg.length()];
        failmsg.toWCharArray(msgdata);
        MessageBox(NULL, msgdata, L"Volume Control", MB_OK);
        delete[] msgdata;
        return 0;
    }
    
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    
    /*QQmlApplicationEngine engine;
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));*/
    
    QFile styleFile( ":/appstyle.qss" );
    styleFile.open( QFile::ReadOnly );
    QString style( styleFile.readAll() );
    styleFile.close();
    app.setStyleSheet(style);
    
    auto vol = new VolMainWindow(0);
    vol->show();
    
    return app.exec();
    
    CoUninitialize();
    return 0;
}

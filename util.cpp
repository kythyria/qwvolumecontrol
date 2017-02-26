#include <windows.h>
#include <QString>

bool AlertHresult(long hr, QString failmessage) {
    if(FAILED(hr)) {
        QString failmsg = failmessage.arg(hr);
        auto msgdata = (wchar_t*)failmsg.utf16();
        MessageBox(NULL, msgdata, L"Volume Control", MB_OK);
        return false;
    }
    
    return true;
}

void assertHR(long hr, QString failmessage) {
    if(!AlertHresult(hr, failmessage)) { exit(1); }
}

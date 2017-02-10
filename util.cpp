#include <windows.h>
#include <QString>

bool AlertHresult(long hr, QString failmessage) {
    if(FAILED(hr)) {
        QString failmsg = failmessage.arg(hr);
        wchar_t *msgdata = new wchar_t[failmsg.length()];
        failmsg.toWCharArray(msgdata);
        MessageBox(NULL, msgdata, L"Volume Control", MB_OK);
        delete[] msgdata;
        return false;
    }
    
    return true;
}

void assertHR(long hr, QString failmessage) {
    if(!AlertHresult(hr, failmessage)) { exit(1); }
}

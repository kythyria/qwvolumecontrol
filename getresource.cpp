#include <QtWinExtras/QtWin>
#include <QPixmap>

#include "getresource.h"

#include <windows.h>
#include <Shlwapi.h>
#include <wchar.h>

QPixmap GetIconFromResource(wchar_t *path, QSize desiredSize) {
    return GetIconFromResource(QString::fromWCharArray(path), desiredSize);
}

QPixmap GetIconFromResource(QString path, QSize desiredSize) {
    int comma = path.lastIndexOf(QChar(','));
    
    if(comma == -1 || comma == path.length() - 1) {
        // Not a DLL?
        QPixmap foo = QPixmap::fromImage(QImage(path));
    }
    return QPixmap();
}

QString GetStringByPossibleResource(QString reference) {
    // While SHLoadIndirectString does exactly as required, it doesn't
    // tell you if it truncated. So we have to loop. Fortunately it just
    // copies the input if it *isn't* indirect.
    
    int outbuf_len;
    PWSTR outbuf = nullptr;
    
    do {
        delete[] outbuf;
        outbuf_len = reference.length() * 2;
        outbuf = new WCHAR[outbuf_len];
        SHLoadIndirectString((wchar_t*)reference.utf16(), outbuf, outbuf_len, NULL);
    } while(wcslen(outbuf) >= outbuf_len - 1);
    
    QString result = QString::fromWCharArray(outbuf);
    delete[] outbuf;
    return result;
}

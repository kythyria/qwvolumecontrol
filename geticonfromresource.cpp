#include <QtWinExtras/QtWin>
#include <QPixmap>

#include "geticonfromresource.h"

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

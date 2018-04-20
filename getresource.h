#ifndef GETICONFROMRESOURCE_H
#define GETICONFROMRESOURCE_H

#include <QIcon>
#include <QString>
#include <QSize>

QPixmap GetIconFromResource(wchar_t *path, QSize desiredSize);
QPixmap GetIconFromResource(QString path, QSize desiredSize);
QPixmap GetIconFromResource(QString path, int id, QSize desiredSize);

QString GetStringByPossibleResource(QString reference);

#endif // GETICONFROMRESOURCE_H

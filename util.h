#ifndef UTIL_H
#define UTIL_H

#include <QDebug>

bool AlertHresult(long hr, QString failmessage);
void assertHR(long hr, QString failmessage);

#define DBG_PRINT qDebug() << __FILE__  ":" << __LINE__

#endif // UTIL_H

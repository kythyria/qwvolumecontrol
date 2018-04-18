#ifndef COMUTIL_H
#define COMUTIL_H

#include <comip.h>
#include <comdef.h>
#include <QObject>

template<typename T>
class RegisterMetatype {
    int reg_id;
    
public:
    RegisterMetatype() {
        reg_id = qRegisterMetaType<T>();
    }
    
    const int id() const { return reg_id; }
};

#define COM_SMARTPTR(typ) \
    _COM_SMARTPTR_TYPEDEF(typ, __uuidof(typ))

#endif // COMUTIL_H

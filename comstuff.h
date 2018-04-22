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

#define COM_IMPL_REFCOUNT(typ) \
    ULONG STDMETHODCALLTYPE typ::AddRef() { \
        return InterlockedIncrement(&refcount); \
    } \
      \
    ULONG STDMETHODCALLTYPE typ::Release() {  \
        ULONG ulRef = InterlockedDecrement(&refcount); \
        if (0 == ulRef) { \
            delete this; \
        } \
        return ulRef; \
    }

#define COM_IMPL_QUERYINTERFACE(typ, calls) \
    HRESULT STDMETHODCALLTYPE typ::QueryInterface(REFIID riid, void **ppvInterface) { \
        if (IID_IUnknown == riid) { \
            AddRef(); \
            *ppvInterface = (IUnknown*)this; \
        } \
        calls \
        else { \
            *ppvInterface = NULL; \
            return E_NOINTERFACE; \
        } \
        return S_OK; \
    }

#define COM_IMPL_QICASE(iid) \
    else if (__uuidof(iid) == riid) { \
        AddRef(); \
        *ppvInterface = (iid*)this; \
    }

#endif // COMUTIL_H

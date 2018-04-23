#include <QtWinExtras/QtWin>
#include <QPixmap>

#include "getresource.h"

#include <windows.h>
#include <Shlwapi.h>
#include <wchar.h>
#include <Processthreadsapi.h>
#include <qdebug.h>
#include "util.h"

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
    
    int outbuf_len = reference.length();
    PWSTR outbuf = nullptr;
    
    do {
        delete[] outbuf;
        outbuf_len *= 2;
        outbuf = new WCHAR[outbuf_len];
        SHLoadIndirectString((wchar_t*)reference.utf16(), outbuf, outbuf_len, NULL);
    } while(wcslen(outbuf) >= outbuf_len - 1);
    
    QString result = QString::fromWCharArray(outbuf);
    delete[] outbuf;
    return result;
}

struct GuessNameForPid_EnumWindowsClosure {
    HWND found;
    unsigned long pid;
    QString title;
};

// Callback to look for the first unowned visible window with a title,
// and get the title
BOOL GuessNameForPid_EnumWindowsCallback(HWND hWnd, LPARAM lParam) {
    auto closure = (GuessNameForPid_EnumWindowsClosure*)lParam;
    
    if(!IsWindowVisible(hWnd)) { return TRUE; }
    
    DWORD wtid, wpid;
    wtid = GetWindowThreadProcessId(hWnd, &wpid);
    if (wpid != closure->pid) { return TRUE; }
    
    HWND parent = GetWindow(hWnd, GW_OWNER);
    if (parent != NULL) { return TRUE; }
    
    // Okay, it's one we might want, now get the title.
    
    int titlelen = GetWindowTextLength(hWnd);
    if(titlelen == 0) { return TRUE; }
    
    PWSTR title = new WCHAR[titlelen] { 0 };
    int count = GetWindowText(hWnd, title, titlelen);
    
    while(count == titlelen - 1) {
        delete[] title;
        titlelen *= 2;
        title = new WCHAR[titlelen] { 0 };
        GetWindowText(hWnd, title, titlelen);
    }
    
    closure->title = QString::fromWCharArray(title);
    delete[] title;
    
    closure->found = hWnd;
    return FALSE;
}

QString GuessNameForPid(unsigned long pid) {
    // As far as I know sndvol.exe's algorithm for doing this is to use the title of the
    // most recently selected window belonging to the process, or the process description
    // if it doesn't have any windows.
    
    QString result = QString("(process %0)").arg(pid);
    
    // Enumerate windows. We look for the first unowned visible one that has a title.
    GuessNameForPid_EnumWindowsClosure closure = { NULL, pid };
    EnumWindows(GuessNameForPid_EnumWindowsCallback, (LPARAM)(&closure));
    
    if (closure.found != NULL) {
        return closure.title;
    }
    
    // That failed, use the process description
    
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if(process == NULL) {
        DBG_PRINT << "Failed to open process " << pid << " error " << GetLastError();
    }
    else {
        DWORD path_len = MAX_PATH;
        DWORD received_len = path_len;
        PWSTR path = NULL;
        
        while (received_len >= path_len - 1) {
            delete[] path;
            path = new WCHAR[path_len] {0};
            if (!QueryFullProcessImageName(process, 0, path, &received_len)) {
                DBG_PRINT << "Failed to get process name " << pid << " error " << GetLastError();
                return result;
            }
            path_len *= 2;
        }
        CloseHandle(process);
        
        QString qspath = QString::fromWCharArray(path);
        result = QString("(process %0: %1)").arg(pid).arg(qspath);
        
        DWORD versionSize = GetFileVersionInfoSize(path, NULL);
        if (versionSize == 0) {
            DBG_PRINT << "Failed to get version info size of process " << pid << ", error " << GetLastError();
        }
        else {
            byte *versionBuffer = new byte[versionSize];
            if(!GetFileVersionInfoEx(FILE_VER_GET_LOCALISED, path, NULL, versionSize, versionBuffer)) {
                DBG_PRINT << "Failed to get version info of process " << pid << ", error " << GetLastError();
            }
            else {
                // Version info is localised, so we need to know the user's preferred languages.
                DWORD threadlanglistsize = 0;
                DWORD threadlanglistcount = 0;
                PZZWSTR threadlanglist = NULL;
                
                if(!GetThreadPreferredUILanguages(MUI_LANGUAGE_ID | MUI_UI_FALLBACK, &threadlanglistcount, threadlanglist, &threadlanglistsize)) {
                    DBG_PRINT << "Failed to get language list size (" << GetLastError() << ")";
                }
                else {
                    threadlanglist = new WCHAR[threadlanglistsize];
                    if(!GetThreadPreferredUILanguages(MUI_LANGUAGE_ID | MUI_UI_FALLBACK, &threadlanglistcount, threadlanglist, &threadlanglistsize)) {
                        DBG_PRINT << "Failed to get language list (" << GetLastError() << ")";
                    }
                    else {
                        // Now we have a list, we can actually query the version info.
                        // Has to be %00 otherwise the first char of 04B0 is consumed by arg().
                        QString templ("\\StringFileInfo\\%0004B0\\FileDescription");
                        WCHAR *description = NULL;
                        unsigned int descriptionlength = 0;
                        for(DWORD i = 0; i < threadlanglistcount; i++) {
                            QString lcid = QString::fromWCharArray(threadlanglist + (i*5), 4);
                            QString verid = templ.arg(lcid);
                            
                            const ushort* veridbuf = verid.utf16();
                            
                            if(!VerQueryValue(versionBuffer, (WCHAR*)veridbuf, (LPVOID*)&description, &descriptionlength)) {
                                description = NULL;
                                descriptionlength = 0;
                            }
                            else {
                                break;
                            }
                        }
                        
                        // 0000 isn't necessarily in the list for some reason,
                        // and some programs only have stuff under that (eg, Firefox).
                        if(descriptionlength == 0) {
                            QString verid = templ.arg("0000");
                            const ushort* veridbuf = verid.utf16();
                            
                            if(!VerQueryValue(versionBuffer, (WCHAR*)veridbuf, (LPVOID*)&description, &descriptionlength)) {
                                description = NULL;
                                descriptionlength = 0;
                            }
                        }
                        
                        if(descriptionlength > 0) {
                            result = QString::fromWCharArray(description, descriptionlength);
                        }
                    }
                    delete[] threadlanglist;
                }
            }
            delete[] versionBuffer;
        }
        
        delete[] path;
    }
    
    return result;
}

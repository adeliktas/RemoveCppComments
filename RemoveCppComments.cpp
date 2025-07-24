#include <windows.h>
#include <tchar.h>
#include "PluginInterface.h"
#include <string>
#include <regex>
#include <wchar.h>  // For wcscpy

NppData nppData;
FuncItem funcItem[1];

void removeCppComments();

HWND getCurrentScintilla() {
    int which = -1;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
    if (which == -1) return NULL;
    return (which == 0) ? nppData._scintillaMainHandle : nppData._scintillaSecondHandle;
}

void removeCppComments() {
    HWND curScintilla = getCurrentScintilla();
    if (!curScintilla) return;

    size_t length = ::SendMessage(curScintilla, SCI_GETLENGTH, 0, 0);
    if (length == 0) return;

    char* buffer = new char[length + 1];
    ::SendMessage(curScintilla, SCI_GETTEXT, length + 1, (LPARAM)buffer);
    std::string text(buffer);
    delete[] buffer;

    // Remove multi-line comments: /* ... */
    text = std::regex_replace(text, std::regex(R"(/\*[\s\S]*?\*/)"), "");

    // Remove single-line comments: // ...
    text = std::regex_replace(text, std::regex("//.*"), "");

    ::SendMessage(curScintilla, SCI_SETTEXT, 0, (LPARAM)text.c_str());
}

extern "C" __declspec(dllexport) void setInfo(NppData notepadPlusData) {
    nppData = notepadPlusData;
}

extern "C" __declspec(dllexport) const wchar_t* getName() {
    return L"RemoveCppComments";
}

extern "C" __declspec(dllexport) FuncItem* getFuncsArray(int* nbF) {
    funcItem[0]._pFunc = removeCppComments;
    wcscpy(funcItem[0]._itemName, L"Remove C++ Comments");
    funcItem[0]._init2Check = false;
    funcItem[0]._pShKey = NULL;

    *nbF = 1;
    return funcItem;
}

extern "C" __declspec(dllexport) void beNotified(SCNotification* notifyCode) {
    // No notifications needed for this plugin
}

extern "C" __declspec(dllexport) LRESULT messageProc(UINT Message, WPARAM wParam, LPARAM lParam) {
    return TRUE;
}

extern "C" __declspec(dllexport) BOOL isUnicode() {
    return TRUE;
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD reasonForCall, LPVOID lpReserved) {
    return TRUE;
}
